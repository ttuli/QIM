#ifndef CHATLISTMODEL_H
#define CHATLISTMODEL_H

#include <QAbstractListModel>
#include <QDataStream>
#include "constants.h"

class chatListInfo{
public:
    GeneralInfo info_;
    QString lastMsg_;
    QString timeStemp_;
    int unReadMsg_;
    chatListInfo(){}
    chatListInfo(GeneralInfo *g,QString lastMsg,QString timeStemp)
        :info_(*g),lastMsg_(lastMsg),timeStemp_(timeStemp),unReadMsg_(0)
    {}
    enum chatListInfoType{
        ACCOUNT,
        NAME,
        LASTMSG,
        TIMESTEMP,
        UNREADMSG
    };

    friend QDataStream &operator<<(QDataStream &out, const chatListInfo &info)
    {
        out << info.info_;
        out << info.lastMsg_;
        out << info.timeStemp_;
        out << info.unReadMsg_;

        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, chatListInfo &info)
    {
        in >> info.info_;
        in >> info.lastMsg_;
        in >> info.timeStemp_;
        in >> info.unReadMsg_;

        return in;
    }
};

class ChatListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ChatListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const;

    void loadHistoryList(QList<chatListInfo> list);
    QList<chatListInfo> getAllInfo(){return data_;}
public slots:
    void clearUnReadMsg(int position);
    void addInfo(GeneralInfo *info,QString lastMsg,bool addUnreadNum=true,QDateTime timedate=QDateTime::currentDateTime(),bool changeLastMsg=true);
    void removeInfo(int position);

private:
    QList<chatListInfo> data_;
};

#endif // CHATLISTMODEL_H
