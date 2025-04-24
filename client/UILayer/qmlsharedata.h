#ifndef QMLSHAREDATA_H
#define QMLSHAREDATA_H

#include <QObject>

class QmlShareData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int msgnum_f_verify READ msgnum_f_verify WRITE setMsgnum_f_verify NOTIFY msgnum_f_verifyChanged FINAL)
    Q_PROPERTY(int msgnum_chat READ msgnum_chat WRITE setMsgnum_chat NOTIFY msgnum_chatChanged FINAL)
public:
    static QmlShareData* getInstance();
    QmlShareData(const QmlShareData&)=delete;
    QmlShareData operator=(const QmlShareData&)=delete;

    int msgnum_f_verify(){return msgnum_f_verify_;}
    int msgnum_chat(){return msgnum_chat_;}
public slots:
    void setMsgnum_f_verify(int val);
    void setMsgnum_chat(int val);

private:
    QmlShareData(QObject *parent = nullptr);
    ~QmlShareData()=default;

    int msgnum_f_verify_;
    int msgnum_chat_;
signals:
    void msgnum_f_verifyChanged();
    void msgnum_chatChanged();
};

#endif // QMLSHAREDATA_H
