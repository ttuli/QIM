#include "qmlsharedata.h"

QmlShareData *QmlShareData::getInstance()
{
    static QmlShareData obj;
    return &obj;
}

void QmlShareData::setMsgnum_f_verify(int val)
{
    msgnum_f_verify_+=val;
    if(msgnum_f_verify_<0)
        msgnum_f_verify_=0;
    emit msgnum_f_verifyChanged();
}

void QmlShareData::setMsgnum_chat(int val)
{
    msgnum_chat_+=val;
    if(msgnum_chat_<0)
        msgnum_chat_=0;
    emit msgnum_chatChanged();
}

QmlShareData::QmlShareData(QObject *parent)
    : QObject{parent}
{
    msgnum_f_verify_=0;
    msgnum_chat_=0;
}
