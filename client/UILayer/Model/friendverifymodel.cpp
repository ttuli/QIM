#include "friendverifymodel.h"

FriendVerifyModel::FriendVerifyModel(QObject *parent,QSoundEffect *soundeffect)
    : QAbstractListModel(parent),soundEffect_(soundeffect)
{}

int FriendVerifyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return data_.size();
}

QVariant FriendVerifyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role==verifyInfo::ACCOUNT)return data_[index.row()].account_;
    if(role==verifyInfo::NAME)return data_[index.row()].name_;
    if(role==verifyInfo::STATE)return (int)data_[index.row()].state_;
    if(role==verifyInfo::MESSAGE)return data_[index.row()].message_;
    if(role==verifyInfo::SHINE_REMIND)return data_[index.row()].shine_remind_;


    return QVariant();
}

void FriendVerifyModel::addInfo(GeneralInfo *info,QString content,int position)
{
    if(position==-1)
        position=rowCount();
    if(position<0||position>rowCount())
        return;

    if(!updateMsg_.contains(info->userData.account_)){
        updateMsg_.insert(info->userData.account_);
        emit updateSig();
    }
    for(int i=0;i<rowCount();i++){
        if(data_[i].account_==info->userData.account_){
            data_[i].state_=info->state_;
            data_[i].message_=content;
            emit dataChanged(index(i),index(i),{verifyInfo::STATE,verifyInfo::MESSAGE});
            if(i){
                beginMoveRows(QModelIndex(),i,i,QModelIndex(),0);
                data_.move(i, 0);
                endMoveRows();
            }
            return;
        }
    }
    verifyInfo vinfo;
    vinfo.account_=info->userData.account_;
    vinfo.message_=content;
    vinfo.name_=info->userData.name_;
    vinfo.state_=info->state_;
    beginInsertRows(QModelIndex(),position,position);
    data_.insert(position,vinfo);
    endInsertRows();
}

void FriendVerifyModel::updateInfo(QString account, verifyInfo::verifyInfoType type, QVariant data)
{
    QList<int> role;
    role<<type;
    for(int i=0;i<rowCount();++i){
        if(data_[i].account_==account){
            if(i){
                beginMoveRows(QModelIndex(),i,i,QModelIndex(),0);
                data_.move(i, 0);
                endMoveRows();
            }
            if(type==verifyInfo::STATE){
                data_[0].state_=(ApplyState)data.toInt();
            }
            emit dataChanged(index(0),index(0),role);
            break;
        }
    }
    if(!updateMsg_.contains(account)){
        updateMsg_.insert(account);
        emit updateSig();
    }
}

void FriendVerifyModel::loadHistoryInfo(QList<verifyInfo> list)
{
    beginInsertRows(QModelIndex(),0,list.size()-1);
    data_=std::move(list);
    endInsertRows();
    bool needRemind=false;
    for(int i=0;i<data_.size();++i){
        if(data_[i].state_==ApplyState::Pending){
            needRemind=true;
            if(i){
                beginMoveRows(QModelIndex(),i,i,QModelIndex(),0);
                auto tmp=data_[i];
                data_.move(i,0);
                endMoveRows();
            }
            if(!updateMsg_.contains(data_[0].account_)){
                updateMsg_.insert(data_[0].account_);
                emit updateSig();
            }
        }
    }
    if(needRemind)
        soundEffect_->play();
}

void FriendVerifyModel::setPulseRemind()
{
    int first=-1,last=-1;
    for(int i=0;i<rowCount();i++){
        if(updateMsg_.contains(data_[i].account_)){
            if(first==-1){
                first=i;
            }
            updateMsg_.remove(data_[i].account_);
            if(!updateMsg_.size()){
                last=i;
                break;
            }
        }
    }
    QList<int> role;
    role<<verifyInfo::SHINE_REMIND;
    emit dataChanged(index(first),index(last),role);
}

QHash<int, QByteArray> FriendVerifyModel::roleNames() const
{
    QHash<int,QByteArray> role;
    role[verifyInfo::ACCOUNT]="account";
    role[verifyInfo::NAME]="name";
    role[verifyInfo::STATE]="st";
    role[verifyInfo::MESSAGE]="umessage";
    role[verifyInfo::SHINE_REMIND]="remind";

    return role;
}
