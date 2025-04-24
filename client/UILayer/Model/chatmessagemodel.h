#ifndef CHATMESSAGEMODEL_H
#define CHATMESSAGEMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "constants.h"

class ChatMessageModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        MessageIdRole = Qt::UserRole + 1,
        SenderIdRole,
        ContentRole,
        TimestampRole,
        IsReadRole,
        TypeRole,
        StatusRole,
        ImageWidthRole,
        ImageHeightRole,
        FileProgressRole,
        FileSizeRole,
        FileCurrentSizeRole
    };

    explicit ChatMessageModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QString addInfo(ChatMessage &message,int position=-1);
    ChatMessage updateInfo(const QString &id,Roles role,QVariant data);
    void updateInfo(const int &position,const QString &id,Roles role,QVariant data);
    void addTimeStamp();
    bool NeedTimeSeparator(QDateTime &timeStamp,QDateTime &timeStamp2);
    void removeInfo(int position);

    QVariant getData(int position,Roles role);
    ChatMessage getSingleData(QString id,int position=-1);

    QHash<int, QByteArray> roleNames() const override;
private:
    QVector<ChatMessage> cacheMessage_;
};

#endif // CHATMESSAGEMODEL_H
