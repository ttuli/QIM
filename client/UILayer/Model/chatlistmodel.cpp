#include "chatlistmodel.h"
#include "UILayer/qmlsharedata.h"

ChatListModel::ChatListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int ChatListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return data_.size();
}

void ChatListModel::addInfo(GeneralInfo *info, QString lastMsg,bool addUnreadNum,QDateTime timedate,bool changeLastMsg)
{
    if(!info){
        qDebug()<<__FUNCTION__<<"null info";
        return;
    }
    QList<QPair<int,int>> msgNumList;
    int offset_m=0;
    for(int i=0;i<data_.size();i++){
        if(data_[i].info_.userData.account_==info->userData.account_){
            QList<int> roles;
            if(addUnreadNum){
                data_[i].unReadMsg_++;
                if(msgNumList.size()){
                    offset_m=msgNumList.back().first+1;
                    while(data_[i].unReadMsg_>=msgNumList.back().second){
                        offset_m=msgNumList.back().first;
                        msgNumList.pop_back();
                    }
                }
                if(i!=offset_m){
                    beginMoveRows(QModelIndex(),i,i,QModelIndex(),offset_m);
                    data_.move(i,offset_m);
                    endMoveRows();
                }
                roles<<chatListInfo::UNREADMSG;
                QmlShareData::getInstance()->setMsgnum_chat(1);
            } else {
                data_[i].unReadMsg_=0;
            }
            roles<<chatListInfo::UNREADMSG;
            if(changeLastMsg){
                data_[offset_m].lastMsg_=lastMsg;
                roles<<chatListInfo::LASTMSG;
                data_[offset_m].timeStemp_=timedate.toString("yyyy-M-d");
                roles<<chatListInfo::TIMESTEMP;
            }
            emit dataChanged(index(offset_m),index(offset_m),roles);
        } else if(data_[i].unReadMsg_){
            msgNumList<<qMakePair(i,data_[i].unReadMsg_);
        }
        return;
    }
    chatListInfo cinfo(info,"","");
    if(lastMsg!=""){
        cinfo.lastMsg_=lastMsg;
        cinfo.timeStemp_=QDateTime::currentDateTime().toString("yyyy-M-d");
        if(addUnreadNum){
            cinfo.unReadMsg_=1;
            QmlShareData::getInstance()->setMsgnum_chat(1);
        }
        if(msgNumList.size()){
            offset_m=msgNumList.back().first+1;
            while(cinfo.unReadMsg_>=msgNumList.back().second){
                offset_m=msgNumList.back().first;
                msgNumList.pop_back();
            }
        }
    }

    beginInsertRows(QModelIndex(),offset_m,offset_m);
    data_.insert(offset_m,cinfo);
    endInsertRows();
}

void ChatListModel::removeInfo(int position)
{
    if(position<-1||position>=rowCount()){
        qDebug()<<"ilegal position";
        return;
    }
    if(position==-1){
        beginRemoveRows(QModelIndex(),0,rowCount()-1);
        data_.clear();
        endRemoveRows();
    } else {
        beginRemoveRows(QModelIndex(),position,position);
        data_.removeAt(position);
        endRemoveRows();
    }
}

QVariant ChatListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role==chatListInfo::ACCOUNT)return data_[index.row()].info_.userData.account_;
    if(role==chatListInfo::LASTMSG)return data_[index.row()].lastMsg_;
    if(role==chatListInfo::NAME)return data_[index.row()].info_.userData.name_;
    if(role==chatListInfo::TIMESTEMP)return data_[index.row()].timeStemp_;
    if(role==chatListInfo::UNREADMSG)return data_[index.row()].unReadMsg_;
    return QVariant();
}

QHash<int, QByteArray> ChatListModel::roleNames() const
{
    QHash<int, QByteArray> hash;
    hash[chatListInfo::ACCOUNT]="account";
    hash[chatListInfo::LASTMSG]="lastMsg";
    hash[chatListInfo::NAME]="name";
    hash[chatListInfo::TIMESTEMP]="timestemp";
    hash[chatListInfo::UNREADMSG]="unreadmsg";
    return hash;
}

void ChatListModel::loadHistoryList(QList<chatListInfo> list)
{
    beginInsertRows(QModelIndex(),0,list.size()-1);
    data_=std::move(list);
    endInsertRows();
}

void ChatListModel::clearUnReadMsg(int position)
{
    if(position<0||position>=rowCount()){
        qDebug()<<__FUNCTION__<<"out of range";
        return;
    }
    QmlShareData::getInstance()->setMsgnum_chat(-data_[position].unReadMsg_);
    data_[position].unReadMsg_=0;
    emit dataChanged(index(position),index(position),{chatListInfo::UNREADMSG});
}
