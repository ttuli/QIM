#include "filemanager.h"
#include <string>
#include <QSharedPointer>

FileManager::FileManager(QObject *parent)
    : QObject{parent}
{
    socket_=nullptr;
    header_.dataSize_=-1;
    connect(this,&FileManager::taskResultdSig,[&]{
        header_.dataSize_=-1;
        currentData_.clear();
        task_.clear();
        file_.close();
        run();
    });
    task_.clear();
}

FileManager::~FileManager()
{

}

void FileManager::destory()
{
    if(socket_)
        socket_->abort();
    qDebug()<<"FileManager::destoty";
}

void FileManager::downloadFile(QString target,QString id, QString savePath)
{
    FileTask task;
    task.type_=FileTask::RECEIVEFILE;
    task.url_=savePath;
    task.totalSize_=task.sentSize_=0;
    task.target_=target;
    task.msgUid_=id;
    addFileTask(task);
}

void FileManager::addFileTask(FileTask task)
{
    taskQueue_.enqueue(task);
    run();
}

void FileManager::cancelFileTask(QString id)
{
    if(id==task_.msgUid_) {
        FileStruct f;
        f.mutable_transfer()->set_status(TRANSFER_STATUS_CANCELED);
        f.set_id(id.toStdString());
        sendData(f);
    } else {
        for(int i=0;i<taskQueue_.size();++i){
            if(taskQueue_.at(i).msgUid_==id){
                taskQueue_.removeAt(i);
                break;
            }
        }
    }
}

FileTask FileManager::getFileTask()
{
    if(taskQueue_.size())
        return taskQueue_.dequeue();
    return FileTask();
}

void FileManager::stopTask(QString id)
{
    stop_=true;
    QMetaObject::invokeMethod(this,"cancelFileTask",Q_ARG(QString,id));
}

void FileManager::requestStop()
{
    stop_=true;
}

void FileManager::sendFileInfo()
{
    file_.setFileName(task_.url_);
    if(file_.open(QIODevice::ReadOnly)){
        FileStruct f;
        f.set_id(task_.msgUid_.toStdString());
        f.mutable_info()->set_filesize(file_.size());
        task_.totalSize_=file_.size();
        sendData(f);
        if(!socket_->waitForReadyRead()){
            emit taskResultdSig(task_.target_,task_.msgUid_,false,true);
            socket_->abort();
        }
    }
}

void FileManager::sendFileData()
{
    if(task_.type_==FileTask::SENDFILE&&task_.sentSize_<task_.totalSize_){
        FileStruct f;
        f.set_id(task_.msgUid_.toStdString());
        f.mutable_transfer()->set_status(TransferStatus::TRANSFER_STATUS_SENDING);
        if(stop_){
            f.mutable_transfer()->set_status(TRANSFER_STATUS_CANCELED);
            sendData(f);
            return;
        }
        qint64 res=(task_.totalSize_-task_.sentSize_>Send_Chunk_Size?Send_Chunk_Size:task_.totalSize_-task_.sentSize_);
        if(file_.size()<=1024*1024*10)
            res=file_.size();
        QByteArray bytes=file_.read(res);
        f.mutable_transfer()->set_data(bytes.constData(),bytes.size());
        f.mutable_transfer()->set_datastart(task_.sentSize_+1);
        f.mutable_transfer()->set_dataend(task_.sentSize_+res);
        sendData(f);
        if(!socket_->waitForReadyRead(MAX_WAIT_TIME)){
            emit taskResultdSig(task_.target_,task_.msgUid_,false,true);
            return;
        }
        task_.sentSize_+=res;
        emit taskProgress(task_.target_,task_.msgUid_,task_.sentSize_/1.0/task_.totalSize_);
    } else if(task_.type_==FileTask::RECEIVEFILE){
        FileStruct f;
        f.set_id(task_.msgUid_.toStdString());
        f.mutable_request()->set_status(TRANSFER_STATUS_SENDING);
        sendData(f);
    }
}

void FileManager::receiveData()
{
    if(socket_->bytesAvailable()<sizeof(CustomHeader))
        return;
    if(header_.dataSize_==-1){
        QByteArray header=socket_->read(sizeof(CustomHeader));
        QDataStream stream(&header,QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream>>header_.type_>>header_.dataSize_;
    }
    int resByte=header_.dataSize_-currentData_.size();
    int availableByte=socket_->bytesAvailable();
    currentData_+=socket_->read((availableByte>=resByte?resByte:availableByte));

    if(currentData_.size()==header_.dataSize_){
        FileStruct f;
        f.ParseFromString(currentData_.toStdString());
        handleReceiveData(f);
        currentData_.clear();
        header_.dataSize_=-1;
        if(socket_->bytesAvailable())
            receiveData();
    }
}

void FileManager::handleReceiveData(FileStruct &f)
{
    if(f.id()!=task_.msgUid_.toStdString()){
        qDebug()<<__FUNCTION__<<":not same id";
        return;
    }
    if(f.has_respond()){
        const FileRespond &respond=f.respond();
        if(!respond.result()){
            emit taskResultdSig(task_.target_,task_.msgUid_,false,true);
            return;
        }
        if(respond.type()==TransferType::TRANSFER_TYPE_FILE_INFO){
            task_.ready_=true;
            sendFileData();
        } else if(respond.type()==TransferType::TRANSFER_TYPE_FILE_DATA){
            if(respond.status()==TransferStatus::TRANSFER_STATUS_COMPLETED){
                qDebug()<<"finish";
                emit taskResultdSig(task_.target_,task_.msgUid_,respond.result(),task_.type_==FileTask::RECEIVEFILE);
            } else if(respond.status()==TransferStatus::TRANSFER_STATUS_CANCELED){
                emit taskResultdSig(task_.target_,task_.msgUid_,false,true);
                run();
            } else if(respond.status()==TransferStatus::TRANSFER_STATUS_SENDING){
                sendFileData();
            }
        }
    } else if(f.has_info()){
        task_.totalSize_=f.info().filesize();
    } else if(f.has_request()){
        const FileRequest &request=f.request();
        if(request.data()==""){
            return;
        }
        file_.write(request.data().data(),request.data().size());
        task_.sentSize_+=request.data().size();
        emit taskProgress(task_.target_,task_.msgUid_,task_.sentSize_/1.0/task_.totalSize_);
        if(task_.sentSize_<task_.totalSize_){
            f.mutable_request()->set_data("");
            sendData(f);
        }

    }
}

void FileManager::requestFile()
{
    if(!file_.isOpen()){
        file_.setFileName(task_.url_);
        if(!file_.open(QIODevice::WriteOnly)){
            emit taskResultdSig(task_.target_,task_.msgUid_,false,true);
            return;
        }
    }
    FileStruct f;
    f.set_id(task_.msgUid_.toStdString());
    f.mutable_request()->set_data("");
    sendData(f);
}

void FileManager::sendData(FileStruct &f)
{
    CustomHeader header;
    header.type_=FILEMSG;

    std::string stringData;
    f.SerializeToString(&stringData);
    header.dataSize_=stringData.size();

    QByteArray headerData;
    QDataStream stream(&headerData,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream<<header.type_<<header.dataSize_;
    QByteArray bytes=headerData+QByteArray::fromStdString(stringData);

    if(socket_->state()!=QAbstractSocket::ConnectedState){
        qDebug()<<"FileManager "<<__FUNCTION__<<"socket not connect";
        return;
    }
    QSharedPointer<QByteArray> dataCopy=QSharedPointer<QByteArray>::create(std::move(bytes));
    socket_->write(*dataCopy);
}

void FileManager::run()
{
    if(task_.sentSize_!=0)return;
    if(!socket_){
        socket_=new QTcpSocket(this);
        connect(socket_,&QTcpSocket::readyRead,this,&FileManager::receiveData);
    }
    stop_=false;
    task_=getFileTask();
    if(task_.msgUid_!=""){
        if(socket_->state()!=QAbstractSocket::ConnectedState){
            socket_->connectToHost(IM_serverAddr,IM_port);
            if(socket_->waitForConnected(MAX_WAIT_TIME)){
                if(task_.type_==FileTask::SENDFILE){
                    sendFileInfo();
                }else if(task_.type_==FileTask::RECEIVEFILE){
                    requestFile();
                }
            } else {
                emit taskResultdSig(task_.target_,task_.msgUid_,false,true);
            }
        }
    } else {
        socket_->abort();
    }
}
