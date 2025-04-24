#include "searchmodel.h"

SearchModel_F::SearchModel_F(QObject *parent)
    : QAbstractListModel(parent)
{}

int SearchModel_F::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return data_.size();
}

QVariant SearchModel_F::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    UserProperties Role=(UserProperties)role;
    if(Role==UserProperties::ACCOUNT)return data_[index.row()]->userData.account_;
    if(Role==UserProperties::CREATETIME)return data_[index.row()]->userData.createTime_;
    if(Role==UserProperties::EMAIL)return data_[index.row()]->userData.email_;
    if(Role==UserProperties::NAME)return data_[index.row()]->userData.name_;
    if(Role==UserProperties::SEX)return (int)data_[index.row()]->userData.sex_;
    if(Role==UserProperties::APPLYSTATE)return (int)data_[index.row()]->state_;
    return QVariant();
}

void SearchModel_F::addUserInfo(GeneralInfo *info,const QModelIndex &parent)
{
    beginInsertRows(parent,rowCount(),rowCount());
    data_<<info;
    endInsertRows();
}

void SearchModel_F::clear()
{
    beginResetModel();
    data_.clear();
    endResetModel();
}

void SearchModel_F::updataInfo(QString account,int position, UserProperties type,QVariant val)
{
    if(position<0||position>=rowCount()){
        qDebug()<<"invaild position";
        return;
    }
    if(account!=data_[position]->userData.account_)
        return;
    QList<int> role;
    if(type==UserProperties::APPLYSTATE){
        role<<(int)UserProperties::APPLYSTATE;
        data_[position]->state_=(ApplyState)(val.toInt());
    } else if(type==UserProperties::NAME) {

    }
    emit dataChanged(index(position),index(position),role);
}

QHash<int, QByteArray> SearchModel_F::roleNames() const
{
    QHash<int,QByteArray> role;
    role.insert((int)UserProperties::ACCOUNT,"user_account");
    role.insert((int)UserProperties::CREATETIME,"user_createtime");
    role.insert((int)UserProperties::EMAIL,"user_email");
    role.insert((int)UserProperties::NAME,"user_name");
    role.insert((int)UserProperties::SEX,"user_sex");
    role.insert((int)UserProperties::APPLYSTATE,"user_applystate");

    return role;
}
