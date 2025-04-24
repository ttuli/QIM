#include "sqlitemanager.h"
#include <QFile>
#include <QDir>
#include <QBuffer>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QSqlError>

SqliteManager::SqliteManager(QObject *parent)
    : QObject{parent}
{
}

SqliteManager::~SqliteManager()
{
    infoDB_.close();
    chatDB_.close();
}

void SqliteManager::writeUserInfo(GeneralInfo *info)
{
    QByteArray bytes;
    QDataStream stream(&bytes,QIODevice::WriteOnly);
    stream<<*info;
    infoQuery_.bindValue(":account",info->userData.account_);
    infoQuery_.bindValue(":data",bytes);
    infoQuery_.bindValue(":last",QDateTime::currentSecsSinceEpoch());
    if(!infoQuery_.exec()){
        qDebug()<<__FUNCTION__<<"fail";
    }
}

void SqliteManager::readUserInfo_init()
{
    qint64 limit=QDateTime::currentSecsSinceEpoch()-Days_Limit;
    QSqlQuery sql(infoDB_);
    sql.exec("select * from account_data wherelast_accessed>"+QString::number(limit)+";");
    while(sql.next()){
        QString account=sql.value("account").toString();
        QByteArray bytes=sql.value("data").toByteArray();
        QDataStream in(&bytes,QIODevice::ReadOnly);
        GeneralInfo info;
        in>>info;
        emit loadUserInfoSig(account,info);
    }
}

void SqliteManager::writeImageInfo(QString key, QImage image)
{
    QByteArray bytes;
    if(!image.isNull()){
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        if(!image.save(&buffer,"PNG")){
            qDebug()<<"writeImageInfo save fail";
            return;
        }
    }
    if(bytes.isEmpty())bytes=" ";
    imageQuery_.bindValue(":id",key);
    imageQuery_.bindValue(":data",bytes);
    imageQuery_.bindValue(":last",QDateTime::currentSecsSinceEpoch());
    if(!imageQuery_.exec()){
        qDebug()<<"writeImageInfo exec fail";
    }
}

QImage SqliteManager::getSingleImage(const QString &key)
{
    QSqlQuery imageQuery_(infoDB_);
    if(imageQuery_.exec("select data from image_data where id='"+key+"';")){
        while(imageQuery_.next()){
            QImage image;
            image.loadFromData(imageQuery_.value("data").toByteArray());
            return image;
        }
    }
    return QImage();
}

void SqliteManager::readImageInfo_init()
{
    qint64 limit=QDateTime::currentSecsSinceEpoch()-Days_Limit;
    QSqlQuery imageQuery_(infoDB_);
    if(imageQuery_.exec("select * from image_data where last_accessed>"+QString::number(limit)+";")){
        while(imageQuery_.next()){
            QImage image;
            image.loadFromData(imageQuery_.value("data").toByteArray());
            emit loadedImageSig(imageQuery_.value("id").toString(),image);
        }
    }
}

void SqliteManager::writeVerifyInfo(QList<verifyInfo> list)
{
    QSqlQuery sql(chatDB_);
    sql.prepare("INSERT OR REPLACE INTO verify_data(account,data,last_accessed) "
                "values(:account,:data,:last);");
    if (!chatDB_.transaction()) {
        qDebug() << "Failed to begin transaction:" << chatDB_.lastError().text();
        return;
    }
    bool allSuccess=true;
    for(int i=0;i<list.size();i++){
        QByteArray bytes;
        QDataStream out(&bytes,QIODevice::WriteOnly);
        out<<list[i];
        sql.bindValue(":account",list[i].account_);
        sql.bindValue(":data",bytes);
        sql.bindValue(":last",QDateTime::currentSecsSinceEpoch());
        if(!sql.exec()){
            qDebug()<<__FUNCTION__<<sql.lastError().text();
            allSuccess=false;
            break;
        }
    }
    if(allSuccess){
        chatDB_.commit();
    } else {
        chatDB_.rollback();
    }
}

void SqliteManager::readVerifyInfo_init()
{
    QSqlQuery sql(chatDB_);
    sql.exec("select * from verify_data ORDER BY id asc limit 50;");
    QList<verifyInfo> list;
    while(sql.next()){
        verifyInfo info;
        QByteArray byte=sql.value("data").toByteArray();
        QDataStream in(&byte,QIODevice::ReadOnly);
        in>>info;
        list<<info;
    }
    emit loadVerifySig(list);
}

void SqliteManager::readChatList_init()
{
    QSqlQuery sql(chatDB_);
    sql.exec("select * from chat_list ORDER BY id asc;");
    QList<chatListInfo> list;
    while(sql.next()){
        chatListInfo info;
        QByteArray byte=sql.value("data").toByteArray();
        QDataStream in(&byte,QIODevice::ReadOnly);
        in>>info;
        list<<info;
    }
    emit loadChatListSig(list);
}

void SqliteManager::writeChatList(QList<chatListInfo> list)
{
    QSqlQuery sql(chatDB_);
    sql.prepare("INSERT OR REPLACE INTO chat_list(account,data,last_accessed) "
                "values(:account,:data,:last);");
    if (!chatDB_.transaction()) {
        qDebug() << "Failed to begin transaction:" << chatDB_.lastError().text();
        return;
    }
    bool allSuccess=true;
    for(int i=0;i<list.size();i++){
        QByteArray bytes;
        QDataStream out(&bytes,QIODevice::WriteOnly);
        out<<list[i];
        sql.bindValue(":account",list[i].info_.userData.account_);
        sql.bindValue(":data",bytes);
        sql.bindValue(":last",QDateTime::currentSecsSinceEpoch());
        if(!sql.exec()){
            qDebug()<<__FUNCTION__<<sql.lastError().text();
            allSuccess=false;
            break;
        }
    }
    if(allSuccess){
        chatDB_.commit();
    } else {
        chatDB_.rollback();
    }
}

void SqliteManager::writeChatMessage(QString account, ChatMessage message)
{
    QByteArray bytes;
    QDataStream stream(&bytes,QIODevice::WriteOnly);
    stream<<message;
    chatMessageQuery_.prepare("INSERT OR REPLACE INTO chat_msg(account,uid,data,last_accessed) "
                              "values(:account,:uid,:data,:last);");
    chatMessageQuery_.bindValue(":account",account);
    chatMessageQuery_.bindValue(":uid",message.messageId);
    chatMessageQuery_.bindValue(":data",bytes);
    chatMessageQuery_.bindValue(":last",QDateTime::currentSecsSinceEpoch());
    if(!chatMessageQuery_.exec())
        qDebug()<<__FUNCTION__<<chatMessageQuery_.lastError().text();
}

void SqliteManager::readChatMessage(QString target,int offset,QVector<ChatMessage> &results)
{
    chatMessageQuery_.exec(QString("select * from chat_msg where account=%1 order by id desc limit %2 offset %3;")
                               .arg(target).arg(Chat_HistoryFetchLimit).arg(offset));
    while(chatMessageQuery_.next()){
        QByteArray bytes=chatMessageQuery_.value("data").toByteArray();
        QDataStream in(&bytes,QIODevice::ReadOnly);
        ChatMessage message;
        in>>message;
        if(message.status==ChatMessage::Sending){
            message.status=ChatMessage::Failed;
        }
        results.append(message);
    }
}

bool SqliteManager::removeChatMessage(QString uid,std::function<void(bool)> callback)
{
    QSqlQuery sql(chatDB_);
    bool res=sql.exec("delete from chat_msg where uid='"+uid+"';");
    if(!res){
        qDebug()<<__FUNCTION__<<sql.lastError().text();
    }
    callback(res);
}

void SqliteManager::init(QString account)
{
    currentAccount_=account;
    QDir dir;
    if(!dir.exists("DB"))
        dir.mkpath("DB");
    QFile file("DB/generalInfo.db");
    file.open(QIODevice::Append);
    file.close();
    file.setFileName("DB/"+account+".db");
    file.open(QIODevice::Append);
    file.close();
    infoDB_ = QSqlDatabase::addDatabase("QSQLITE","conn1");
    infoDB_.setDatabaseName("DB/generalInfo.db");
    chatDB_=QSqlDatabase::addDatabase("QSQLITE","conn2");
    chatDB_.setDatabaseName("DB/"+account+".db");
    if (infoDB_.open()) {
        infoQuery_=QSqlQuery(infoDB_);
        imageQuery_=QSqlQuery(infoDB_);
        if(!infoQuery_.exec("CREATE TABLE IF NOT EXISTS account_data ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "account BIGINT NOT NULL UNIQUE,data BLOB,"
                            "last_accessed INTEGER NOT NULL);")){
            qDebug()<<"create account_data fail";
        }
        if(!infoQuery_.exec("create table if not exists image_data("
                            "id TEXT not null UNIQUE,"
                            "data blob not null,"
                            "last_accessed INTEGER NOT NULL);")){
            qDebug()<<"create image_data fail";
        }
        infoQuery_.exec("PRAGMA journal_mode=WAL;");
        infoQuery_.prepare("INSERT OR REPLACE INTO account_data(account,data,last_accessed) values(:account,:data,:last);");
        imageQuery_.prepare("INSERT OR REPLACE INTO image_data(id,data,last_accessed) values(:id,:data,:last);");
    }else{
        qDebug()<<"open infoDB fail";
    }
    if (chatDB_.open()){
        chatMessageQuery_=QSqlQuery(chatDB_);
        if(!chatMessageQuery_.exec("create table if not exists verify_data("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             "account BIGINT NOT NULL UNIQUE,"
                             "data blob,"
                      "last_accessed INTEGER NOT NULL);")){
            qDebug()<<"create verify_data fail";
        }
        if(!chatMessageQuery_.exec("create table if not exists chat_list("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "account BIGINT NOT NULL UNIQUE,"
                      "data blob,"
                      "last_accessed INTEGER NOT NULL);")){
            qDebug()<<"create chat_list fail";
        }
        if(!chatMessageQuery_.exec("create table if not exists chat_msg("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "account BIGINT NOT NULL,"
                        "uid text not null,"
                      "data blob,"
                      "last_accessed INTEGER NOT NULL);")){
            qDebug()<<"create chat_msg fail";
        }
    }else {
        qDebug()<<"open chatDB_ fail";
    }

}
