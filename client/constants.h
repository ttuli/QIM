#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <functional>
#include <QSet>
#include <QDebug>
#include <QUuid>
#include <QDataStream>
#include <QDateTime>
#include "IM_login.pb.h"

#define DOTEST 0

enum class UserState{
    ONLINE,
    OFFLINE,
    BUSY,
    LEAVE,
    STEALTH //隐身
};

enum class ApplyState{
    NONE=0,
    Pending,
    Accepted,
    Rejected,
    Sent,
    Ignore,
    Deleted
};

enum class captionType{
    LOADDING,
    SUCCESS,
    FAIL
};

//请求默认的回调函数格式
typedef struct requestCallbcak{
    std::function<void(PullRespond)> successFunc_;
    std::function<void()> timeoutFunc_;
}RequestCallbcak;

//同步获取用户信息的回调函数
typedef struct syncUserInfoCallback{
    std::function<void()> ifGet_;
    std::function<void()> ifNotGet_;
}SyncUserInfoCallback;


enum class UserProperties{
    ACCOUNT=0,
    NAME,
    STATE,
    CREATETIME,
    SEX,
    EMAIL,
    APPLYSTATE
};
enum class Sex{
    MALE=0,
    FEMALE=1
};

class User{
public:
    User(QString account,QString name,QString email,QString createTime)
        :account_(account),name_(name),email_(email),createTime_(createTime)
    {
        state_=UserState::ONLINE;
    }
    User(){}

    QString account_;
    UserState state_;
    Sex sex_;

    QString name_;
    QString email_;
    QString createTime_;

    friend QDataStream &operator<<(QDataStream &out, const User &user) {
        out << user.account_ << (qint32)user.state_ << (qint32)user.sex_
            << user.name_ << user.email_ << user.createTime_;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, User &user) {
        qint32 stateInt, sexInt;
        in >> user.account_ >> stateInt >> sexInt
            >> user.name_ >> user.email_ >> user.createTime_;
        user.state_ = static_cast<UserState>(stateInt);
        user.sex_ = static_cast<Sex>(sexInt);
        return in;
    }

};

enum GeneralInfoType { FRIEND, GRUOP };

struct GeneralInfo {
    GeneralInfoType type_;
    ApplyState state_;
    User userData;

    GeneralInfo()
        :type_(GeneralInfoType::FRIEND)
    {
        state_=ApplyState::NONE;
    }
    friend QDataStream &operator<<(QDataStream &out, const GeneralInfo &info) {
        out << (qint32)info.type_ << (qint32)info.state_;

        if (info.type_ == GeneralInfoType::FRIEND) {
            out << info.userData;
        } else {
            //out << info.groupData;
        }

        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, GeneralInfo &info) {
        qint32 typeInt, stateInt;
        in >> typeInt >> stateInt;

        info.type_ = static_cast<GeneralInfoType>(typeInt);
        info.state_ = static_cast<ApplyState>(stateInt);

        if (info.type_ == GeneralInfoType::FRIEND) {
            in >> info.userData;
        } else {
            //in >> info.groupData;
        }

        return in;
    }
};

enum HeaderType{
    LOGIN=0,
    REGISTER,
    PULLFRIEND,
    USERINFO,
    SEARCH,
    ADD,
    MESSAGE,
    INFOCHANGE,
    IMAGE_UPDATE,
    IMAGE_GET,
    VERIFY,
    SENDMSG,
    FILEMSG,
    DELETEMSG
};
struct CustomHeader{
    uint32_t type_;
    uint32_t dataSize_;
};

struct ChatMessage {
    // 基本信息
    QString messageId;         // 消息唯一ID
    QString senderId;          // 发送者ID
    QString content;           // 消息内容
    QDateTime timestamp;       // 发送时间
    bool isRead;               // 是否已读

    // 消息类型
    enum MessageType {
        Text,                  // 文本消息
        Image,                 // 图片消息
        File,                  // 文件消息
        Audio,                 // 语音消息
        Video,                 // 视频消息
        System,                // 系统消息
        TimeStamp,             // 时间戳
        Custom                 // 自定义消息类型
    };
    MessageType type;

    // 消息状态
    enum MessageStatus {
        Sending,               // 发送中
        Sent,                  // 已发送
        Delivered,             // 已送达
        Read,                  // 已读
        Failed,                 // 发送失败
        Ready                  //文件可下载
    };
    MessageStatus status;

    int imageWidth,imageHeight;

    // 附加数据 - 用于扩展不同类型消息的特定属性
    QVariantMap extraData;     // 使用键值对存储额外信息

    // 构造函数
    ChatMessage(const QString& sender = "", const QString& msg = "", MessageType msgType = Text)
        : senderId(sender), content(msg),
        timestamp(QDateTime::currentDateTime()),
        isRead(false),type(msgType),status(Sending)
    {
        messageId = QUuid::createUuid().toString();
        imageWidth=imageHeight=0;
    }

    friend QDataStream &operator<<(QDataStream &stream, const ChatMessage &message)
    {
        // 写入版本信息，方便未来扩
        stream << ChatMessage::DataStreamVersion;

        stream << message.messageId
               << message.senderId
               << message.content
               << message.timestamp
               << message.isRead
               << static_cast<qint32>(message.type)
               << static_cast<qint32>(message.status)
                << message.imageWidth
                << message.imageHeight
               << message.extraData;

        return stream;
    }

    friend QDataStream &operator>>(QDataStream &stream, ChatMessage &message)
    {
        quint32 version;
        stream >> version;

        if (version == ChatMessage::DataStreamVersion) {
            qint32 typeInt, statusInt;

            stream >> message.messageId
                >> message.senderId
                >> message.content
                >> message.timestamp
                >> message.isRead
                >> typeInt
                >> statusInt
                >> message.imageWidth
                >> message.imageHeight
                >> message.extraData;

            message.type = static_cast<ChatMessage::MessageType>(typeInt);
            message.status = static_cast<ChatMessage::MessageStatus>(statusInt);
        } else {
            // 处理未知版本，可以抛出异常或设置默认值
            qDebug()<<"ChatMessage:different version";
            message = ChatMessage(); // 重置为默认值
        }
        return stream;
    }

    static const quint32 DataStreamVersion = 1;
};

const QString IM_serverAddr="your server addr";
constexpr const int IM_port=50051;
constexpr const int MAX_WAIT_TIME=20000;//所有任务的响应超时时间
extern bool IM_connectState;

constexpr const size_t MAX_CHATDIALOG_CACHE=1024*1024*100;
extern QString currentLoginAccount;
extern User currentLoginUserInfo;
constexpr const short Chat_HistoryFetchLimit =30;//每次拉取历史聊天记录数量
constexpr const int Time_Interval_Limit=300;//聊天窗口两天消息之间的时间差超过这个值就加一个时间戳(秒)
constexpr const qint64 Days_Limit=24*60*60*3;//启动时从本地获取最近n天的数据

constexpr const qint64 Send_Chunk_Size=1024*1024*3;
// constexpr const int SleepTime_Each=12;//毫秒，与上面搭配达5mb/s
constexpr const qint64 Max_Send_File_Size=1024*1024*100;//100MB

//主窗口的长度和高度
constexpr const int minMainWindowWidth=740;
constexpr const int minMainWindowHeight=640;

enum class currentWindow{
    CHATWINDOW=0,
    FRIENDMANAGER=1
};

extern currentWindow IM_Current_Window;
extern QString currentChatDialogAccount;
extern QSet<QString> allFriends;


constexpr const int ImageLength=50;
constexpr const int ImageMaxLength=100;
constexpr const int ImageMaxLength_Chat=340;

#endif // CONSTANTS_H
