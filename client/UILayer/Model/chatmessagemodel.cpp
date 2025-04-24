#include "chatmessagemodel.h"

ChatMessageModel::ChatMessageModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int ChatMessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return cacheMessage_.size();
}

QVariant ChatMessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const ChatMessage &message = cacheMessage_.at(index.row());

    switch (role) {
        case MessageIdRole:
            return message.messageId;
        case SenderIdRole:
            return message.senderId;
        case ContentRole:
            return message.content;
        case TimestampRole:
            return message.timestamp;
        case IsReadRole:
            return message.isRead;
        case TypeRole:
            return message.type;
        case StatusRole:
            return message.status;
        case ImageWidthRole:
            return message.imageWidth;
        case ImageHeightRole:
            return message.imageHeight;
        case FileProgressRole:
            return message.extraData.value("fileProgress");
        case FileSizeRole:
            return message.extraData.value("fileSize");
        default:
            return QVariant();
    }
}

QString ChatMessageModel::addInfo(ChatMessage &message, int position)
{
    if(position<-1||position>rowCount())
        return "";
    ChatMessage msg;
    msg.type=ChatMessage::TimeStamp;
    msg.content=(message.timestamp.toString("yyyy-MM-dd HH:mm"));
    msg.timestamp=message.timestamp;
    if(position==-1){
        position=rowCount();
        if(!cacheMessage_.size()){
            beginInsertRows(QModelIndex(),position,position+1);
            cacheMessage_<<msg<<message;
            endInsertRows();
            return cacheMessage_[position+1].messageId;
        } else {
            if(cacheMessage_[position-1].type!=ChatMessage::TimeStamp){
                if(NeedTimeSeparator(cacheMessage_[position-1].timestamp,message.timestamp)){
                    beginInsertRows(QModelIndex(),position,position+1);
                    cacheMessage_.insert(position,msg);
                    cacheMessage_.insert(position+1,message);
                    endInsertRows();
                    return cacheMessage_[position+1].messageId;
                } else {
                    beginInsertRows(QModelIndex(),position,position);
                    cacheMessage_.insert(position,message);
                    endInsertRows();
                    return cacheMessage_[position].messageId;
                }
            } else {
                qDebug()<<__FUNCTION__<<" unexpected result(-1)";
                return "";
            }
        }
    } else if (position==0){
        if(!cacheMessage_.size()){
            beginInsertRows(QModelIndex(),position,position+1);
            cacheMessage_<<msg<<message;
            endInsertRows();
        } else {
            if(cacheMessage_[position].type==ChatMessage::TimeStamp){
                if(NeedTimeSeparator(cacheMessage_[position].timestamp,message.timestamp)){
                    beginInsertRows(QModelIndex(),position,position+1);
                    cacheMessage_.insert(position,message);
                    cacheMessage_.insert(position,msg);
                    endInsertRows();
                } else {
                    beginInsertRows(QModelIndex(),position+1,position+1);
                    cacheMessage_.insert(position+1,message);
                    endInsertRows();
                }
            } else {
                qDebug()<<__FUNCTION__<<" unexpected result(0)";
                return "";
            }
        }
        return cacheMessage_[position+1].messageId;
    } else {
        qDebug()<<__FUNCTION__<<":not zero or rowCount";
        return "";
    }

    return "";
}

ChatMessage ChatMessageModel::updateInfo(const QString &id,Roles role, QVariant data)
{
    QList<int> roles;
    roles<<role;
    for(int i=cacheMessage_.size()-1;i>=0;i--){
        if(cacheMessage_[i].messageId==id){
            if(role==StatusRole){
                cacheMessage_[i].status=(ChatMessage::MessageStatus)data.toInt();
            } else if(role==FileProgressRole){
                cacheMessage_[i].extraData["fileProgress"]=data;
            }
            emit dataChanged(index(i),index(i),roles);
            return cacheMessage_[i];
        }
    }
    return ChatMessage();
}

void ChatMessageModel::updateInfo(const int &position, const QString &id, Roles role, QVariant data)
{
    if(position<0&&position>=rowCount())
        return;
    if(cacheMessage_[position].messageId==id){
        QList<int> roles;
        roles<<role;
        if(role==StatusRole){
            cacheMessage_[position].status=(ChatMessage::MessageStatus)data.toInt();
        } else if(role==FileProgressRole){
            cacheMessage_[position].extraData["fileProgress"]=data;
            if((int)(data.toDouble())==1){
                cacheMessage_[position].status=ChatMessage::Sent;
                roles<<StatusRole;
            }
        }
        emit dataChanged(index(position),index(position),roles);
    } else {
        qDebug()<<"ChatMessageModel::updateInfo:unsame id";
    }
}


//判断两天消息之间是否超过Time_Interval_Limit
bool ChatMessageModel::NeedTimeSeparator(QDateTime &timeStamp,QDateTime &timeStamp2)
{
    if(timeStamp.isNull()||timeStamp2.isNull())
        return true;
    qint64 secondsDiff = qAbs(timeStamp.secsTo(timeStamp2));
    return secondsDiff>=Time_Interval_Limit;
}

void ChatMessageModel::removeInfo(int position)
{
    if(position<0||position>=rowCount())
        return;
    int pre=position-1;
    int next=position+1;
    if(pre>0&&cacheMessage_[pre].type==ChatMessage::TimeStamp){
        if(next==rowCount()||cacheMessage_[next].type==ChatMessage::TimeStamp){
            beginRemoveRows(QModelIndex(),pre,pre);
            cacheMessage_.removeAt(pre);
            endRemoveRows();
            position=pre;
        }
    }
    beginRemoveRows(QModelIndex(),position,position);
    cacheMessage_.removeAt(position);
    endRemoveRows();
}

QVariant ChatMessageModel::getData(int position, Roles role)
{
    if(position<0||position>=rowCount())
        return QVariant();
    const ChatMessage &message = cacheMessage_.at(position);

    switch (role) {
    case MessageIdRole:
        return message.messageId;
    case SenderIdRole:
        return message.senderId;
    case ContentRole:
        return message.content;
    case TimestampRole:
        return message.timestamp;
    case IsReadRole:
        return message.isRead;
    case TypeRole:
        return message.type;
    case StatusRole:
        return message.status;
    case ImageWidthRole:
        return message.imageWidth;
    case ImageHeightRole:
        return message.imageHeight;
    case FileProgressRole:
        return message.extraData.value("fileProgress");
    case FileSizeRole:
        return message.extraData.value("fileSize");
    default:
        return QVariant();
    }
}

ChatMessage ChatMessageModel::getSingleData(QString id, int position)
{
    if(position>=0&&position<rowCount()&&cacheMessage_[position].messageId==id){
        return cacheMessage_[position];
    }
    for(int i=rowCount()-1;i>=0;--i){
        if(cacheMessage_[i].messageId==id){
            return cacheMessage_[i];
        }
    }
    return ChatMessage();
}

QHash<int, QByteArray> ChatMessageModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[MessageIdRole] = "messageId";
    roles[SenderIdRole] = "senderId";
    roles[ContentRole] = "content";
    roles[TimestampRole] = "timestamp";
    roles[IsReadRole] = "isRead";
    roles[TypeRole] = "type";
    roles[StatusRole] = "msgstatus";
    roles[ImageWidthRole]="imageWidth";
    roles[ImageHeightRole] ="imageHeight";
    roles[FileProgressRole] = "fileProgress";
    roles[FileSizeRole]="fileSize";
    return roles;
}
