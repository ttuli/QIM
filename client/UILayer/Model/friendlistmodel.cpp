#include "friendlistmodel.h"

FriendListModel::FriendListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int FriendListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return data_.size();
}

QVariant FriendListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    UserProperties r=(UserProperties)role;
    if(r==UserProperties::ACCOUNT)return data_[index.row()].userData.account_;
    if(r==UserProperties::NAME)return data_[index.row()].userData.name_;
    if(r==UserProperties::SEX)return (int)data_[index.row()].userData.sex_;

    return QVariant();
}

void FriendListModel::addInfo(GeneralInfo *info,int position)
{
    if(!info){
        qDebug()<<"FriendListModel::addInfo:null info";
        return;
    }
    if(position==-1)
        position=rowCount();
    if(position<-1||position>rowCount())
        return;
    allFriends.insert(info->userData.account_);
    beginInsertRows(QModelIndex(),position,position);
    data_.insert(position,GeneralInfo(*info));
    endInsertRows();
}

void FriendListModel::removeInfo(QString id)
{
    for(int i=0;i<data_.size();++i){
        if(data_[i].userData.account_==id){
            beginRemoveRows(QModelIndex(),i,i);
            data_.removeAt(i);
            endRemoveRows();
            allFriends.remove(id);
            break;
        }
    }
}

QHash<int, QByteArray> FriendListModel::roleNames() const
{
    QHash<int,QByteArray> hash;
    hash[(int)UserProperties::ACCOUNT]="account";
    hash[(int)UserProperties::NAME]="name";
    hash[(int)UserProperties::SEX]="sex";
    return hash;
}
