#include "chatdialog.h"
#include "UILayer/confirmactiondialog.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QQmlContext>
#include "constants.h"

ChatDialog::ChatDialog(QWidget *parent,QString account,QString name,QString loginName)
    : QWidget{parent},currentAccount_(account),currentName_(name),loginName_(loginName)
{
    interface_=new CustomizeQuickWidget(this);
    interface_->rootContext()->setContextProperty("currentDialog",this);
    interface_->rootContext()->setContextProperty("chatDialogModel",&model_);
    interface_->rootContext()->setContextProperty("caption_content",name);
    interface_->rootContext()->setContextProperty("currentLoginAccount",currentLoginAccount);
    interface_->rootContext()->setContextProperty("loginName",loginName);
    interface_->rootContext()->setContextProperty("type_text",(int)ChatMessage::Text);
    interface_->rootContext()->setContextProperty("type_image",(int)ChatMessage::Image);
    interface_->rootContext()->setContextProperty("type_file",(int)ChatMessage::File);
    interface_->rootContext()->setContextProperty("type_audio",(int)ChatMessage::Audio);
    interface_->rootContext()->setContextProperty("type_video",(int)ChatMessage::Video);
    interface_->rootContext()->setContextProperty("type_system",(int)ChatMessage::System);
    interface_->rootContext()->setContextProperty("type_timestamp",(int)ChatMessage::TimeStamp);
    interface_->rootContext()->setContextProperty("msg_sending",(int)ChatMessage::Sending);
    interface_->rootContext()->setContextProperty("msg_sent",(int)ChatMessage::Sent);
    interface_->rootContext()->setContextProperty("msg_fail",(int)ChatMessage::Failed);
    interface_->rootContext()->setContextProperty("msg_ready",(int)ChatMessage::Ready);

    interface_->setSource(QUrl("qrc:/ChatState/ChatDialog.qml"));
    interfaceItem_=interface_->rootObject();

    QHBoxLayout *qhb=new QHBoxLayout(this);
    qhb->setContentsMargins(0,0,0,0);
    qhb->addWidget(interface_);
}

QString ChatDialog::addMsg(ChatMessage message,int position)
{
    QString res=model_.addInfo(message,position);
    return res;
}

void ChatDialog::updateInfo(const QString &id, ChatMessageModel::Roles role, QVariant data,bool writeLocal)
{
    ChatMessage msg=model_.updateInfo(id,role,data);
    if(msg.senderId!=""&&writeLocal){
        emit writeHistoryMsgSig(currentAccount_,msg);
    }
}

void ChatDialog::setBlock(bool visible)
{
    QMetaObject::invokeMethod(interface_->rootObject(),"setBlockDiaVisible",Q_ARG(QVariant,visible));
}

void ChatDialog::removeMsg(int position)
{
    if((ChatMessage::MessageStatus)model_.getData(position,ChatMessageModel::StatusRole).toInt()
        ==ChatMessage::Sending)
        return;

    model_.removeInfo(position);
    QString lastMsg=model_.getData(model_.rowCount()-1,ChatMessageModel::ContentRole).toString();
    QDateTime timedate=model_.getData(model_.rowCount()-1,ChatMessageModel::TimestampRole).toDateTime();
    emit updateChatListAfterDelSig(currentAccount_,lastMsg,timedate);
}

void ChatDialog::showInfo(QString content, captionType type, int duration)
{
    setBlock(false);
    QMetaObject::invokeMethod(interface_->rootObject(),"showInfo"
                              ,Q_ARG(QVariant,content)
                              ,Q_ARG(QVariant,(int)type)
                              ,Q_ARG(QVariant,duration));
}

ChatMessage ChatDialog::getMessage(QString id, int position)
{
    return model_.getSingleData(id,position);
}

void ChatDialog::handleSendMsg(QString content, int type)
{
    emit sendMsgSig(currentAccount_,content,type);
}

void ChatDialog::handleSendPic()
{
    emit sendPicSig(currentAccount_);
}

void ChatDialog::handleSendFile()
{
    QString filepath=QFileDialog::getOpenFileName(this,"选择一个文件");
    if(filepath.isEmpty())
        return;
    QFileInfo info(filepath);
    if(!info.size()){
        ConfirmActionDialog dia(this,"不能发送大小为0的文件");
        dia.exec();
        return;
    } else if(info.size()>Max_Send_File_Size){
        ConfirmActionDialog dia(this,"文件大小不能超过100MB");
        dia.exec();
        return;
    }
    emit sendFileSig(currentAccount_,filepath);
}

void ChatDialog::pullHistoryMsg()
{
    if(!hasPullAll){
        emit loadHistoryMsg(currentAccount_,currentOffset_);
    }
}

void ChatDialog::handleCheckHdPic(QString id)
{
    emit checkHdPicSig(id+"_hd");
}

void ChatDialog::readyRemoveMsg(int position,QString uid)
{
    ConfirmActionDialog dia(this,"确认删除选中消息?",true,true);
    if(dia.exec()==QDialog::Rejected)
        return;
    if(uid=="")return;
    setBlock(true);
    emit removeMsgSig(position,uid);
}

void ChatDialog::handleFileCancel(QString id, int index)
{
    //model_.updateInfo(index,id,ChatMessageModel::StatusRole,ChatMessage::Failed);
    emit cancelFileSig(id);
}

void ChatDialog::handleFileDownLoad(QString id, int index)
{
    ChatMessage msg=model_.getSingleData(id,index);
    QString path=QFileDialog::getSaveFileName(this,"选择保存位置",msg.content);
    if(path.isEmpty())
        return;
    model_.updateInfo(index,id,ChatMessageModel::StatusRole,ChatMessage::Sending);
    emit downloadFileSig(currentAccount_,id,path);
}
