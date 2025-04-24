#ifndef FRIENDLISTMODEL_H
#define FRIENDLISTMODEL_H

#include <QAbstractListModel>
#include "constants.h"

class FriendListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FriendListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void addInfo(GeneralInfo *info,int position=-1);
    void removeInfo(QString id);

    QHash<int,QByteArray> roleNames() const;
private:
    QList<GeneralInfo> data_;
};

#endif // FRIENDLISTMODEL_H
