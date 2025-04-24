#ifndef FRIENDVERIFYMODEL_H
#define FRIENDVERIFYMODEL_H

#include <QAbstractListModel>
#include <QSet>
#include <QSoundEffect>
#include "constants.h"

class verifyInfo{
public:
    enum verifyInfoType{
        ACCOUNT=0,
        NAME,
        MESSAGE,
        STATE,
        SHINE_REMIND
    };

    QString account_;
    QString name_;
    QString message_;
    ApplyState state_;
    bool shine_remind_=false;//仅用于闪烁提醒

    verifyInfo(){}

    friend QDataStream &operator<<(QDataStream &out, const verifyInfo &info)
    {
        out << info.account_
            << info.name_
            << info.message_
            << static_cast<qint32>(info.state_)
            << info.shine_remind_;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, verifyInfo &info)
    {
        qint32 stateValue;
        in >> info.account_
            >> info.name_
            >> info.message_
            >> stateValue
            >> info.shine_remind_;
        info.state_ = static_cast<ApplyState>(stateValue);
        return in;
    }
};

class FriendVerifyModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FriendVerifyModel(QObject *parent = nullptr,QSoundEffect *soundeffect=nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addInfo(GeneralInfo *info,QString content,int position=0);
    void updateInfo(QString account,verifyInfo::verifyInfoType type,QVariant data);
    void loadHistoryInfo(QList<verifyInfo> list);

    void setPulseRemind();//闪烁提醒

    QList<verifyInfo> getAllInfo(){return data_;}

    QHash<int,QByteArray> roleNames() const;

public:
    QSoundEffect *soundEffect_;

private:
    QList<verifyInfo> data_;
    QSet<QString> updateMsg_;//每次点开验证消息界面需要提醒有更新的信息的队列
signals:
    void updateSig();
};

#endif // FRIENDVERIFYMODEL_H
