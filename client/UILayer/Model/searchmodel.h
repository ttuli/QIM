#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractListModel>
#include "constants.h"

class SearchModel_F : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SearchModel_F(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addUserInfo(GeneralInfo *info,const QModelIndex &parent = QModelIndex());
    void updataInfo(QString account,int position,UserProperties type,QVariant val);

    QHash<int,QByteArray> roleNames() const;
public slots:
    void clear();

private:
    QList<GeneralInfo*> data_;
};

#endif // SEARCHMODEL_H
