#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QQueue>
#include <QThread>
#include <QFile>
#include <QMutex>
#include <QTcpSocket>
#include <QWaitCondition>
#include "FileStruct.pb.h"
#include "constants.h"
#include <QTimer>

class FileTask{
public:
    enum TaskType{
        SENDFILE,
        RECEIVEFILE
    };

    FileTask(QString target,QString id,QString url)
        :target_(target),msgUid_(id),url_(url),type_(TaskType::SENDFILE){
        sentSize_=0;
        totalSize_=0;
        ready_=false;
    }
    FileTask(){}

    QString target_;
    QString msgUid_;
    QString url_;
    qint64 sentSize_;
    qint64 totalSize_;
    bool ready_;
    TaskType type_;

    void clear()
    {
        target_="";
        url_="";
        totalSize_=sentSize_=0;
        msgUid_="";
    }
};

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager();
    FileTask getFileTask();
    void stopTask(QString id);
public slots:
    void addFileTask(FileTask task);
    void cancelFileTask(QString id);
    void requestStop();
    void destory();
    void downloadFile(QString id,QString savePath,QString target);

protected:
    void run();
    void sendData(FileStruct &f);
    void sendFileInfo();
    void sendFileData();
    void receiveData();
    void handleReceiveData(FileStruct &f);

    void requestFile();

private:
    FileTask task_;
    QQueue<FileTask> taskQueue_;
    bool stop_=false;

    QTcpSocket *socket_;
    QFile file_;
    CustomHeader header_;
    QByteArray currentData_;
signals:
    void taskResultdSig(QString target,QString id,bool result,bool finalUpdate);
    void taskProgress(QString target,QString id,double percentage);
};

#endif // FILEMANAGER_H
