#include "uimanager.h"
#include "DataLayer/datamanager.h"
#include "constants.h"
#include <QDateTime>
#include <QFutureWatcher>
#include <QFileDialog>
#include <QCryptographicHash>

UIManager::UIManager(QObject *parent)
    : QObject{parent}
{
    login_=new LoginUI;
    connect(login_,&LoginUI::readyCloseSig,this,&UIManager::doClose);
    connect(login_,&LoginUI::successSig,login_,&LoginUI::deleteLater);
    connect(login_,&LoginUI::successSig,[this](QString account){
        currentLoginAccount=account;
        DataManager::getInstance()->initLocalDB(account);
        pullFriends(account);
        pullSelfUserInfo();
    });

    register_=new RegisterDialog(login_);
    connect(login_,&LoginUI::registerSig,register_,&RegisterDialog::show);

    soundEffect_F_=new QSoundEffect(this);
    soundEffect_F_->setSource(QUrl("qrc:/xm3495.wav"));
    soundEffect_F_->setVolume(80);
    soundEffect_MSG_=new QSoundEffect(this);
    soundEffect_MSG_->setVolume(80);
    soundEffect_MSG_->setSource(QUrl("qrc:/xm3527.wav"));

    friendVerifyModel_=new FriendVerifyModel(this,soundEffect_F_);
    friendListModel_=new FriendListModel(this);
    friendManager_=new FriendManager(nullptr,friendVerifyModel_,friendListModel_);
    connect(friendManager_,&FriendManager::updateRelationship,this,&UIManager::handleUpdateRelationship);
    connect(DataManager::getInstance(),&DataManager::loadVerifyInfoSig,friendVerifyModel_,&FriendVerifyModel::loadHistoryInfo);
    connect(friendManager_,&FriendManager::newUserInfoDialog,[this](QString account){
        onUserInfoDialog(account,DialogType::FRIEND);
    });

    chatListModel_=new ChatListModel(this);
    chatManager_=new ChatManager(nullptr,chatListModel_);
    connect(DataManager::getInstance(),&DataManager::loadChatListSig,chatListModel_,&ChatListModel::loadHistoryList);
    connect(chatManager_,&ChatManager::createChatDialogSig,this,[this](QString account,QString name){
        createChatDialog(account,name);
    });

    mainWindow_=new MainWindow(nullptr,friendManager_,chatManager_);
    connect(mainWindow_,&MainWindow::newUserInfoDialog,[this](QString account,DialogType type){
        onUserInfoDialog(account,type);
    });
    connect(mainWindow_,&MainWindow::closeSig,this,&UIManager::doClose);
    connect(login_,&LoginUI::successSig,mainWindow_,&MainWindow::init);
    connect(mainWindow_,&MainWindow::showSig,this,&UIManager::onMainWindowShow);


    searchModel_=new SearchModel_F(this);
    searchDialog_=new SearchDialog(nullptr,searchModel_);
    connect(mainWindow_,&MainWindow::searchSig,searchDialog_,&SearchDialog::show);
    connect(searchDialog_,&SearchDialog::doSearch,this,&UIManager::handleSearch);
    connect(searchDialog_,&SearchDialog::doAdd,this,&UIManager::handleAdd);
    connect(searchDialog_,&SearchDialog::newUserInfoDia,[this](QString account,int index){
        onUserInfoDialog(account,DialogType::STRANGER,index);
    });


    business_=new MainBusiness(this);
    connect(login_,&LoginUI::loginSig,this,&UIManager::handleLogin);
    connect(register_,&RegisterDialog::registerSig,this,&UIManager::handleRegister);
    connect(business_,&MainBusiness::serverPushSig,this,&UIManager::handleServerPush);

    picviewManager_=new PictureViewManager(this);
    connect(picviewManager_,&PictureViewManager::saveImageSig,this,&UIManager::handleSaveImage);

    fileManager_=new FileManager();
    fileThread_=new QThread(this);
    fileManager_->moveToThread(fileThread_);
    fileThread_->start();
    connect(fileManager_,&FileManager::taskProgress,[&](QString target,QString id,double progress){
        qDebug()<<chatDialogCache_.contains(target)<<progress;
        if(chatDialogCache_.contains(target)){
            ChatDialog *dia=chatDialogCache_[target];
            int tmp=progress*100;
            progress=tmp/100.0;
            dia->updateInfo(id,ChatMessageModel::FileProgressRole,progress,false);
        }
    });
    connect(fileManager_,&FileManager::taskResultdSig,this,[&](QString target,QString id,bool result,bool finalUpdate){
        if(chatDialogCache_.contains(target)){
            ChatDialog *dia=chatDialogCache_[target];
            ChatMessage::MessageStatus st=result?ChatMessage::Sent:ChatMessage::Failed;
            if(!finalUpdate)
                handleSendFileMsg(target,dia->getMessage(id));
            else
                dia->updateInfo(id,ChatMessageModel::StatusRole,st);
        }
    });
}

UIManager::~UIManager()
{
    fileManager_->requestStop();
    QMetaObject::invokeMethod(fileManager_,"destory");
    fileThread_->quit();
    fileThread_->wait();
    delete fileManager_;
    if(currentLoginAccount!=""){
        DataManager::getInstance()->writeVerifyInfo(friendVerifyModel_->getAllInfo());
        DataManager::getInstance()->writeChatList(chatListModel_->getAllInfo());
    }
    if(mainWindow_!=nullptr)
        delete mainWindow_;
    if(login_!=nullptr)
        delete login_;
    if(searchDialog_!=nullptr)
        delete searchDialog_;
    if(friendManager_!=nullptr){
        delete friendManager_;
    }
    if(chatManager_!=nullptr){
        delete chatManager_;
    }
    for(auto i:infoDialog_){
        delete i;
    }
    infoDialog_.clear();
}

void UIManager::start()
{
#if DOTEST
    mainWindow_->show();
#else
    login_->show();
#endif
}

void UIManager::onMainWindowShow()
{
    while (msgStore_.size()) {
        handleServerPush(msgStore_.dequeue());
    }
}

void UIManager::pullFriends(QString account)
{
    PullRequest request;
    request.mutable_friend_request()->set_user_id(account.toStdString());
    business_->sendData(request,PULLFRIEND,RequestCallbcak{
        [this](PullRespond respond){
            if(respond.has_friend_respond()){
                const FriendRespond &frespond=respond.friend_respond();
                for(int i=0;i<frespond.infos_size();++i){
                    User user;
                    const UserInfoRespond &urespond=frespond.infos(i);
                    user.account_=QString::fromStdString(urespond.account());
                    user.createTime_=QString::fromStdString(urespond.createtime());
                    user.email_=QString::fromStdString(urespond.email());
                    user.name_=QString::fromStdString(urespond.username());
                    user.sex_=(Sex)urespond.sex();
                    QString tmpAccount=user.account_;
                    DataManager::getInstance()->addInfo(user.account_,GeneralInfoType::FRIEND,ApplyState::Accepted,user);
                    friendListModel_->addInfo(DataManager::getInstance()->getInfo(tmpAccount));
                }
            }
    },nullptr});
}

void UIManager::pullSelfUserInfo()
{
    PullRequest request;
    request.mutable_user_info()->set_account(currentLoginAccount.toStdString());
    business_->sendData(request,USERINFO,RequestCallbcak{
        [](PullRespond respond){
            if(respond.has_user_info()){
                const UserInfoRespond &uresp=respond.user_info();
                currentLoginUserInfo.account_=QString::fromStdString(uresp.account());
                currentLoginUserInfo.createTime_=QString::fromStdString(uresp.createtime());
                currentLoginUserInfo.email_=QString::fromStdString(uresp.email());
                currentLoginUserInfo.name_=QString::fromStdString(uresp.username());
                currentLoginUserInfo.sex_=(Sex)uresp.sex();
                User user(currentLoginUserInfo);
                DataManager::getInstance()->addInfo(currentLoginUserInfo.account_,GeneralInfoType::FRIEND,ApplyState::NONE,user);
            }
        },nullptr
    });
}


void UIManager::doClose()
{
    qApp->quit();
}

void UIManager::handleLogin(QString account, QString password)
{
    PullRequest request;
    request.mutable_login_request()->set_account(account.toStdString());
    request.mutable_login_request()->set_password(password.toStdString());

    business_->sendData(request,HeaderType::LOGIN,
                        RequestCallbcak{
                        [account,password,this](PullRespond resp){
                            if(resp.has_simple_respond()){
                                bool res=resp.simple_respond().success();
                                if(!res){
                                    QString text=QString::fromStdString(resp.simple_respond().msg());
                                    if(text=="logged")
                                        text="账号已登录!";
                                    if(text=="")
                                        text="登录失败，请检查账号密码";
                                    login_->showCaption(text,captionType::FAIL,3000);
                                }
                                else{
                                    login_->showCaption("登陆成功",captionType::SUCCESS,3000);
                                    login_->writeInit(account,password);
                                }
                            } else {
                                login_->showCaption("登陆失败",captionType::FAIL,3000);
                            }
                        },
                        [&](){
                            login_->showCaption("响应超时",captionType::FAIL,3000);
                        }
                        });
}

void UIManager::handleRegister(QString name, QString password, QString email)
{
    PullRequest request;
    request.mutable_register_request()->set_username(name.toStdString());
    request.mutable_register_request()->set_password(password.toStdString());
    request.mutable_register_request()->set_email(email.toStdString());

    business_->sendData(request,HeaderType::REGISTER
                        ,RequestCallbcak{
                        [&](PullRespond resp){
                            if(resp.has_simple_respond()){
                                bool res=resp.simple_respond().success();
                                register_->showInfo(res?"注册成功:"+QString::fromStdString(resp.simple_respond().msg()):"注册失败,请检查网络",
                                                    res?captionType::SUCCESS:captionType::FAIL
                                                    ,3000);
                                if(res){
                                    login_->setInfo(QString::fromStdString(resp.simple_respond().msg())
                                                    ,"",false,false,false);
                                }
                            } else {
                                register_->showInfo("注册失败,请重试!",captionType::FAIL,3000);
                            }
                        },
                        [&](){
                            register_->showInfo("注册失败,请重试!",captionType::FAIL,3000);
                        }
                    });
}

void UIManager::onUserInfoDialog(QString account, DialogType type,int index2SearchDia)
{
    DataManager::getInstance()->removeImageCache(account+"_avatar");
    DataManager::getInstance()->removeImageCache(account+"_avatar_hd");
    if(infoDialog_.contains(account)){
        infoDialog_[account]->activateWindow();
    } else {
        UserInfoDialog *dia=new UserInfoDialog(nullptr,type);
        dia->index2SearchDia=index2SearchDia;
        infoDialog_[account]=dia;
        infoDialog_[account]->setInfo(account,"","","",Sex::MALE);
        connect(dia,&UserInfoDialog::readyClose,[this,account]{
            infoDialog_[account]->deleteLater();
            infoDialog_.remove(account);
        });
        connect(dia,&UserInfoDialog::checkImageSig,picviewManager_,&PictureViewManager::createView);
        if(type==DialogType::MYSELF){
            connect(dia,&UserInfoDialog::changeInfoSig,this,&UIManager::handleChangeInfo);
            connect(dia,&UserInfoDialog::updateImageSig,this,&UIManager::handleUpdateImage);
        } else if(type==DialogType::FRIEND){
            connect(dia,&UserInfoDialog::talkSig,this,&UIManager::handleTalk);
            connect(dia,&UserInfoDialog::deleteFriendSig,this,&UIManager::handleDeleteFriend);
        } else if(type==DialogType::STRANGER){
            connect(dia,&UserInfoDialog::doAdd,this,&UIManager::handleAdd);
        }
        dia->show();

        business_->getUserInfoWithCallback(account,USERINFO,RequestCallbcak{
         [this,account](PullRespond respond){
             QString m_account="";
             QString email="";
             QString name="";
             QString createTime="";
             Sex sex=Sex::MALE;
             if(respond.has_user_info()){
                 m_account=QString::fromStdString(respond.user_info().account());
                 email=QString::fromStdString(respond.user_info().email());
                 name=QString::fromStdString(respond.user_info().username());
                 createTime=QString::fromStdString(respond.user_info().createtime());
                 sex=(!respond.user_info().sex()?Sex::MALE:Sex::FEMALE);
                 if(infoDialog_.contains(account))
                     infoDialog_[account]->setInfo(account,name,email,createTime,sex);
                 User user(account,name,email,createTime);
                 user.sex_=sex;
                DataManager::getInstance()->addInfo(account,GeneralInfoType::FRIEND,ApplyState::NONE,user);
             }
         },
         nullptr
     });
    }
}

void UIManager::handleSearch(QString type, int mode, QString content, int offs)
{
    PullRequest request;
    request.mutable_search_request()->set_content(content.toStdString());
    request.mutable_search_request()->set_offset(offs);
    request.mutable_search_request()->set_mode(
        (mode==SearchDialog::BY_ACCOUNT?SearchRequest::ACCOUNT:SearchRequest::NAME));
    request.mutable_search_request()->set_type(
        (type=="person"?SearchRequest::FRIEND:SearchRequest::GROUP));

    business_->sendData(request,HeaderType::SEARCH
            ,RequestCallbcak{
            [this,offs,type](PullRespond respond){
                if(!offs){
                    searchModel_->clear();
                }
                if(respond.has_search_respond()){
                    const SearchRespond &srespond=respond.search_respond();
                    if(type=="person"){
                        for(int i=0;i<srespond.info_size();i++){
                            const UserInfoRespond &uinfo=srespond.info(i);

                            User user(QString::fromStdString(uinfo.account()),
                                                  QString::fromStdString(uinfo.username()),
                                                  QString::fromStdString(uinfo.email()),
                                                  QString::fromStdString(uinfo.createtime()));
                            user.sex_=uinfo.sex()?Sex::FEMALE:Sex::MALE;

                            DataManager::getInstance()->addInfo(
                                QString::fromStdString(uinfo.account())
                                ,GeneralInfoType::FRIEND
                                ,ApplyState::NONE
                                ,user);
                            searchModel_->addUserInfo(DataManager::getInstance()->getInfo(
                                QString::fromStdString(uinfo.account())));
                        }
                    }
                    if(srespond.info_size()<SearchDialog::searchSingleTime)
                        searchDialog_->setIsGetAll(true);
                    searchDialog_->showInformation("搜索成功",captionType::SUCCESS,1500);
                } else {
                    searchDialog_->showInformation("搜索失败",captionType::FAIL,1500);
                }
            },
          [this]{
              searchDialog_->showInformation("响应超时",captionType::FAIL,1500);
          }
    });
}

void UIManager::handleAdd(QString target, QString content,int index)
{
    if(currentLoginAccount=="")
        return;
    content=content.replace("'", "''");
    content=content.replace("\\", "\\\\");

    PullRequest request;
    request.mutable_add_request()->set_user_id(currentLoginAccount.toStdString());
    request.mutable_add_request()->set_friend_id(target.toStdString());
    request.mutable_add_request()->set_verification_message(content.toStdString());

    if(infoDialog_.contains(target))
        infoDialog_[target]->setLoadding(true);

    business_->sendData(request,ADD
            ,RequestCallbcak{
        [this,index,target,content](PullRespond respond){
            if(respond.has_simple_respond()){
                if(respond.simple_respond().success()){
                    searchDialog_->showInformation("已发送申请",captionType::SUCCESS,1200);
                    searchModel_->updataInfo(target,index,UserProperties::APPLYSTATE,(int)ApplyState::Sent);
                    GeneralInfo *info=DataManager::getInstance()->getInfo(target);
                    info->state_=ApplyState::Sent;
                    friendVerifyModel_->addInfo(info,content,0);
                    if(infoDialog_.contains(target)){
                        infoDialog_[target]->showInfo("成功",captionType::SUCCESS,1200);
                        infoDialog_[target]->changeType(DialogType::SENT);
                    }
                } else {
                    searchDialog_->showInformation("发送失败",captionType::FAIL,1200);
                }
            }
        },
        [this,target]{
            if(infoDialog_.contains(target)){
                infoDialog_[target]->setLoadding(false);
            }
            searchDialog_->showInformation("响应超时",captionType::FAIL,1200);
        }
    });
}

void UIManager::handleServerPush(PullRespond respond)
{
    if(!mainWindow_->isVisible()){
        msgStore_.enqueue(std::move(respond));
        return;
    }
    if(respond.has_server_push()){
        if(respond.server_push().has_add_request()){
            QString id=QString::fromStdString(respond.server_push().add_request().friend_id());
            QString content=QString::fromStdString(respond.server_push().add_request().verification_message());
            GeneralInfo *info=DataManager::getInstance()->getInfo(id);
            if(info){
                info->state_=ApplyState::Pending;
                friendVerifyModel_->addInfo(info,
                                            QString::fromStdString(respond.server_push().add_request().verification_message()));
            }else{
                business_->getUserInfoWithCallback(id,MESSAGE,RequestCallbcak{
                    [this,id,content](PullRespond respond){
                        if(respond.has_user_info()){
                            const UserInfoRespond &uinfo=respond.user_info();
                            User user(QString::fromStdString(uinfo.account()),
                                                  QString::fromStdString(uinfo.username()),
                                                  QString::fromStdString(uinfo.email()),
                                                  QString::fromStdString(uinfo.createtime()));
                            user.sex_=uinfo.sex()?Sex::FEMALE:Sex::MALE;
                            DataManager::getInstance()->addInfo(id,GeneralInfoType::FRIEND,
                                                                ApplyState::Pending,user);
                            friendVerifyModel_->addInfo(DataManager::getInstance()->getInfo(id),
                                                        content);
                        }
                    },nullptr
                });
            }
        } else if(respond.server_push().has_verify_request()){
            User user;
            const UserInfoRespond &userinfo=respond.server_push().verify_request().user_info();
            user.account_=QString::fromStdString(userinfo.account());
            user.name_=QString::fromStdString(userinfo.username());
            user.email_=QString::fromStdString(userinfo.email());
            user.createTime_=QString::fromStdString(userinfo.createtime());
            user.sex_=(Sex)userinfo.sex();
            DataManager::getInstance()->addInfo(user.account_,GeneralInfoType::FRIEND,
                                                (ApplyState)(respond.server_push().verify_request().type()),user);
            friendVerifyModel_->updateInfo(user.account_,verifyInfo::STATE,
                                               respond.server_push().verify_request().type());
            if(respond.server_push().verify_request().type()==(int)ApplyState::Accepted){
                friendListModel_->addInfo(DataManager::getInstance()->getInfo(user.account_));
            }
        } else if(respond.server_push().has_message_request()){
            const MessageRequest mr=respond.server_push().message_request();
            GeneralInfo *g=DataManager::getInstance()->syncGetInfo(QString::fromStdString(mr.target()),SyncUserInfoCallback{
                [respond,this]{
                    handleServerPush(std::move(respond));
                },
                nullptr}
            );
            if(!g){
                qDebug()<<"handleServerPush:null info";
                return;
            }
            bool addUnreadNum=(IM_Current_Window==currentWindow::CHATWINDOW||
                                currentChatDialogAccount!=QString::fromStdString(mr.target())||
                                !mainWindow_->isActiveWindow());
            if(!soundEffect_MSG_->isPlaying()&&addUnreadNum)
                soundEffect_MSG_->play();
            ChatMessage msg;
            QString target=QString::fromStdString(mr.target());
            if(mr.type()==MessageRequest::Text){
                QByteArray byte=QByteArray::fromStdString(mr.text_message().content());
                QDataStream in(&byte,QIODevice::ReadOnly);
                in>>msg;
                chatListModel_->addInfo(g,msg.content,addUnreadNum);
                msg.status=msg.Sent;
            } else if(mr.type()==MessageRequest::Image){
                QByteArray byte=QByteArray::fromStdString(mr.image_message().content());
                QDataStream in(&byte,QIODevice::ReadOnly);
                in>>msg;
                chatListModel_->addInfo(g,"[图片]",addUnreadNum);
                msg.status=msg.Sent;
                DataManager::getInstance()->addImageCache(QString::fromStdString(mr.image_message().md5()),
                                                          QByteArray::fromStdString(mr.image_message().data()));

            } else if(mr.type()==MessageRequest::File){
                QByteArray byte=QByteArray::fromStdString(mr.text_message().content());
                QDataStream in(&byte,QIODevice::ReadOnly);
                in>>msg;
                chatListModel_->addInfo(g,"[文件]",addUnreadNum);
                msg.status=msg.Ready;
                msg.extraData["fileProgress"]=0;
            }
            DataManager::getInstance()->writeChatMessage(target,msg);
            if(chatDialogCache_.contains(target)){
                chatDialogCache_[target]->addMsg(msg);
                chatDialogCache_[target]->setOffset(1);//防止获取历史消息时重复获取
            }
        } else if(respond.server_push().has_delete_request()){
            QString account=QString::fromStdString(respond.server_push().delete_request().target());
            if(infoDialog_.contains(account)){
                UserInfoDialog *dia=infoDialog_[account];
                dia->changeType(DialogType::STRANGER);
            }
            friendListModel_->removeInfo(account);
            if(chatDialogCache_.contains(account)){
                chatDialogCache_[account]->setIsNotFriend(true);
            }
            GeneralInfo *g=DataManager::getInstance()->syncGetInfo(account,SyncUserInfoCallback{
                [this,account]{
                    friendVerifyModel_->addInfo(DataManager::getInstance()->getInfo(account),"");
                    friendVerifyModel_->updateInfo(account,verifyInfo::verifyInfoType::STATE,(int)ApplyState::Deleted);
                },nullptr
            });
            if(g){
                friendVerifyModel_->addInfo(DataManager::getInstance()->getInfo(account),"");
                friendVerifyModel_->updateInfo(account,verifyInfo::verifyInfoType::STATE,(int)ApplyState::Deleted);
            }
        }
    }
}

void UIManager::handleChangeInfo(QString account, QString name, QString email, int sex)
{
    PullRequest request;
    request.mutable_modify_request()->set_user_id(account.toStdString());
    request.mutable_modify_request()->set_email(email.toStdString());
    request.mutable_modify_request()->set_name(name.toStdString());
    request.mutable_modify_request()->set_sex(sex);

    business_->sendData(request,INFOCHANGE
        ,RequestCallbcak{
            [=](PullRespond respond){
            if(respond.has_simple_respond()){
                if(infoDialog_.contains(account)){
                    infoDialog_[account]->showInfo(
                        respond.simple_respond().success()?"更新成功":"更新失败",
                        respond.simple_respond().success()?captionType::SUCCESS:captionType::FAIL,
                        1200);
                    infoDialog_[account]->changeInfoSuccess();
                }
                GeneralInfo *user=DataManager::getInstance()->getInfo(account);
                if(!user)
                    return;
                user->userData.email_=email;
                user->userData.name_=name;
                user->userData.sex_=(Sex)sex;
            }
        },
        [=]{
        if(infoDialog_.contains(account))
        infoDialog_[account]->showInfo(
            "响应超时",
            captionType::FAIL,
            1200);
        }
    });
}

void UIManager::handleUpdateImage(QString id,QImage image,bool compressed)
{
    QtConcurrent::run([this,image,id,compressed]{
        QImage ima=image;
        QByteArray byteArray;
        QByteArray bytes_compressed;
        QBuffer buffer(&byteArray);
        ima.save(&buffer, "PNG");

        PullRequest request;
        request.mutable_update_image()->set_url(id.toStdString());
        if(compressed){
            ima=(ima.width()>ima.height()?
                       ima.scaledToWidth(ImageMaxLength):ima.scaledToHeight(ImageMaxLength));
            QBuffer buffer2(&bytes_compressed);
            ima.save(&buffer2,"PNG");
            request.mutable_update_image()->set_data(bytes_compressed.constData(),bytes_compressed.size());
            request.mutable_update_image()->set_datahd(byteArray.constData(),byteArray.size());
        } else {
            request.mutable_update_image()->set_data(byteArray.constData(),byteArray.size());
        }

        QString account=id.left(id.indexOf('_'));

        QMetaObject::invokeMethod(this,[this,ima,req=std::move(request),image,account,id,compressed]{
            PullRequest request=std::move(req);
            business_->sendData(request,IMAGE_UPDATE
                ,RequestCallbcak{
                    [this,ima,image,account,compressed,id](PullRespond respond){
                        if(respond.has_simple_respond()){
                            if(respond.simple_respond().success()){
                                DataManager::getInstance()->addImageCache(id,ima);
                                if(compressed)
                                    DataManager::getInstance()->addImageCache(id+"_hd",image);
                                mainWindow_->updateUserImage();
                                if(infoDialog_.contains(account)){
                                    infoDialog_[account]->showInfo(
                                        "更新成功",
                                        captionType::SUCCESS,
                                        1200);
                                    infoDialog_[account]->changeInfoSuccess(ChangeInfoType::IMAGE);
                                }
                            }else{
                                if(infoDialog_.contains(account)){
                                    infoDialog_[account]->showInfo(
                                        "更新失败",
                                        captionType::FAIL,
                                        1200);
                                }
                            }
                        }
                    },
                    [this,account]{
                        if(infoDialog_.contains(account))
                            infoDialog_[account]->showInfo(
                                "响应超时",
                                captionType::FAIL,
                                1200);
                    }
                });
            });
    });
}

void UIManager::handleUpdateRelationship(QString target, int type)
{
    PullRequest request;
    GeneralInfo *info=DataManager::getInstance()->syncGetInfo(currentLoginAccount,SyncUserInfoCallback{
        [this,target,type]{
            handleUpdateRelationship(target,type);
        },[this]{
            friendManager_->setInfo("更新失败",captionType::FAIL,1200);
        }
    });
    if(!info){
        return;
    }
    request.mutable_verify_request()->set_user_id(target.toStdString());
    request.mutable_verify_request()->set_type(type);
    request.mutable_verify_request()->set_vtype(VerifyRequest::FRIEND);
    UserInfoRespond *userinfo=request.mutable_verify_request()->mutable_user_info();
    userinfo->set_username(info->userData.name_.toStdString());
    userinfo->set_account(info->userData.account_.toStdString());
    userinfo->set_email(info->userData.email_.toStdString());
    userinfo->set_createtime(info->userData.createTime_.toStdString());
    userinfo->set_sex((bool)info->userData.sex_);

    business_->sendData(request,VERIFY,RequestCallbcak{
        [this,type,target](PullRespond respond){
            if(respond.has_simple_respond()){
                friendManager_->setInfo(respond.simple_respond().success()?"操作成功":"操作失败",
                                        respond.simple_respond().success()?captionType::SUCCESS:captionType::FAIL,1200);
                if(respond.simple_respond().success()){
                    friendVerifyModel_->updateInfo(target,verifyInfo::STATE,type);
                    if(type==(int)ApplyState::Accepted)
                        friendListModel_->addInfo(DataManager::getInstance()->getInfo(target));
                    GeneralInfo *info=DataManager::getInstance()->getInfo(target);
                    info->state_=(ApplyState)type;
                }
            }else {
                friendManager_->setInfo("操作失败!",captionType::FAIL,1200);
            }
        },[this]{
            friendManager_->setInfo("操作失败!",captionType::FAIL,1200);
        }
    });

}

void UIManager::handleDeleteFriend(QString account)
{
    if(currentLoginAccount==""){
        return;
    }
    PullRequest request;
    request.mutable_delete_request()->set_user_id(currentLoginAccount.toStdString());
    request.mutable_delete_request()->set_target(account.toStdString());

    if(infoDialog_.contains(account)){
        infoDialog_[account]->setLoadding(true);
        infoDialog_[account]->showInfo("正在更新关系",captionType::LOADDING,MAX_WAIT_TIME);
    }

    business_->sendData(request,DELETEMSG,RequestCallbcak{
        [this,account](PullRespond respond){
            if(respond.has_simple_respond()&&infoDialog_.contains(account)){
                UserInfoDialog *dia=infoDialog_[account];
                if(respond.simple_respond().success()){
                    dia->changeType(DialogType::STRANGER);
                    dia->showInfo("更新成功",captionType::SUCCESS,1200);
                    dia->setLoadding(false);
                    friendListModel_->removeInfo(account);
                    if(chatDialogCache_.contains(account)){
                        chatDialogCache_[account]->setIsNotFriend(true);
                    }
                    GeneralInfo *g=DataManager::getInstance()->getInfo(account);
                    if(g)
                        g->state_=ApplyState::NONE;
                } else {
                    dia->showInfo("更新失败",captionType::FAIL,1200);
                    dia->setLoadding(false);
                }
            }
        },
        [this,account]{
            if(infoDialog_.contains(account)){
                UserInfoDialog *dia=infoDialog_[account];
                dia->showInfo("更新失败",captionType::FAIL,1200);
                dia->setLoadding(false);
            }
        }
    });
}

void UIManager::handleTalk(QString account)
{
    GeneralInfo* g=DataManager::getInstance()->syncGetInfo(account,SyncUserInfoCallback{
        [this,account]{
             handleTalk(account);
        },nullptr
    });
    if(!g)return;
    chatListModel_->addInfo(g,"",false,QDateTime::currentDateTime(),false);
    mainWindow_->setCurrentIndex(0);
    createChatDialog(account,g->userData.name_);
}

void UIManager::handleSendMsg(QString target,QString content, int type)
{
    if(!chatDialogCache_.contains(target)){
        qDebug()<<__FUNCTION__<<"not contain";
        return;
    }
    GeneralInfo *g=DataManager::getInstance()->syncGetInfo(target,SyncUserInfoCallback{
        [this,target,content,type]{
             handleSendMsg(target,content,type);
        },nullptr
    });
    if(!g)
        return;
    content=content.replace("'", "''");
    content=content.replace("\\", "\\\\");
    chatListModel_->addInfo(g,content,false);
    ChatMessage msg(currentLoginAccount,content,(ChatMessage::MessageType)type);
    QString uid=chatDialogCache_[target]->addMsg(msg);
    if(uid==""){
        qDebug()<<__FUNCTION__<<"null uid";
        return;
    }
    msg.status=ChatMessage::Sent;
    QByteArray byte;
    QDataStream out(&byte,QIODevice::WriteOnly);
    out<<msg;
    PullRequest request;
    request.mutable_message_request()->set_type(MessageRequest::Text);
    request.mutable_message_request()->set_target(target.toStdString());
    request.mutable_message_request()->mutable_text_message()->set_content(byte.constData(),byte.size());

    business_->sendData(request,SENDMSG
            ,RequestCallbcak{
                [this,uid,target](PullRespond respond){
                if(respond.has_simple_respond()&&chatDialogCache_.contains(target)){
                    if(respond.simple_respond().success()){
                        chatDialogCache_[target]->updateInfo(uid,ChatMessageModel::StatusRole,ChatMessage::Sent);
                    } else {
                        chatDialogCache_[target]->updateInfo(uid,ChatMessageModel::StatusRole,ChatMessage::Failed);
                        qDebug()<<"handleSendMsg"<<respond.simple_respond().msg();
                    }
                }
            },
          [this,uid,target]{
              if(chatDialogCache_.contains(target)){
                  chatDialogCache_[target]->updateInfo(uid,ChatMessageModel::StatusRole,ChatMessage::Failed);
              }
          }
        });
}

void UIManager::handleSendPic(QString target)
{
    QString filename=QFileDialog::getOpenFileName(mainWindow_,"选择图片","","*.png *.jpg *.jpeg");
    if(filename.isEmpty()||!chatDialogCache_.contains(target))
        return;

    QtConcurrent::run([=]{
        QImage image(filename);
        QImage compressImage;
        if(image.width()>image.height()&&image.width()>ImageMaxLength_Chat)
            compressImage=image.scaledToWidth(ImageMaxLength_Chat,Qt::SmoothTransformation);
        else if(image.width()<=image.height()&&image.height()>ImageMaxLength_Chat)
            compressImage=image.scaledToHeight(ImageMaxLength_Chat,Qt::SmoothTransformation);
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)){
            qDebug()<<"openFail";
            return;
        }
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (!hash.addData(&file)) {
            qWarning() << "handleSendPic读取文件失败:";
            return;
        }
        QByteArray md5=hash.result().toHex();

        QByteArray byte,hdbyte;
        QBuffer buffer(&byte);
        QBuffer bufferhd(&hdbyte);
        buffer.open(QIODevice::WriteOnly);
        bufferhd.open(QIODevice::WriteOnly);

        image.save(&bufferhd,"PNG");
        compressImage.save(&buffer,"PNG");
        PullRequest request;
        request.mutable_message_request()->set_target(target.toStdString());
        request.mutable_message_request()->set_type(MessageRequest::Image);
        request.mutable_message_request()->mutable_image_message()->set_md5(md5.toStdString());
        request.mutable_message_request()->mutable_image_message()->set_data(byte.constData(),byte.size());
        request.mutable_message_request()->mutable_image_message()->set_datahd(hdbyte.constData(),hdbyte.size());

        QMetaObject::invokeMethod(this,"handleSendPicData"
                                  ,Q_ARG(QString,target)
                                  ,Q_ARG(QString,md5)
                                  ,Q_ARG(QImage,image)
                                  ,Q_ARG(QImage,compressImage)
                                  ,Q_ARG(PullRequest,request));
    });
}

void UIManager::handleSendPicData(QString target,QString md5, QImage image, QImage compressImage, PullRequest request)
{
    GeneralInfo *g=DataManager::getInstance()->syncGetInfo(target,SyncUserInfoCallback{
        [this,target,md5,image,compressImage,request]{
            handleSendPicData(target,md5,image,compressImage,std::move(request));
        },nullptr
    });
    if(!g)return;
    DataManager::getInstance()->addImageCache(md5,compressImage);
    DataManager::getInstance()->addImageCache(md5+"_hd",image,false);
    ChatMessage msg(currentLoginAccount,md5,ChatMessage::Image);
    msg.imageWidth=compressImage.width();
    msg.imageHeight=compressImage.height();
    QString id="";
    chatListModel_->addInfo(g,"[图片]",false);
    if(chatDialogCache_.contains(target)){
        id=chatDialogCache_[target]->addMsg(msg);
    }
    QByteArray array;
    QDataStream out(&array,QIODevice::WriteOnly);
    out<<msg;
    request.mutable_message_request()->mutable_image_message()->set_content(array.constData(),array.size());
    business_->sendData(request,HeaderType::MESSAGE,RequestCallbcak{
      [this,target,id](PullRespond respond){
            if(respond.has_simple_respond()&&chatDialogCache_.contains(target)){
                chatDialogCache_[target]->updateInfo(id,ChatMessageModel::StatusRole,
                                                     respond.simple_respond().success()?ChatMessage::Sent:ChatMessage::Failed);
            } else{
                chatDialogCache_[target]->updateInfo(id,ChatMessageModel::StatusRole,ChatMessage::Failed);
            }
        },
      [this,target,id]{
          chatDialogCache_[target]->updateInfo(id,ChatMessageModel::StatusRole,ChatMessage::Failed);
      }
    });
}

void UIManager::handleSendFile(QString target,QString path)
{
    if(!chatDialogCache_.contains(target)){
        return;
    }
    QFileInfo info(path);
    ChatDialog *dia=chatDialogCache_[target];
    ChatMessage msg(currentLoginAccount,path.last(path.size()-path.lastIndexOf('/')-1),ChatMessage::File);
    msg.extraData["fileProgress"]=0;
    msg.extraData["fileSize"]=info.size();
    dia->addMsg(msg);
    QMetaObject::invokeMethod(fileManager_,"addFileTask",Q_ARG(FileTask,FileTask(target,msg.messageId,path)));
    GeneralInfo *g=DataManager::getInstance()->syncGetInfo(target,SyncUserInfoCallback{
        [this,target]{
            chatListModel_->addInfo(DataManager::getInstance()->getInfo(target),"[文件]",false);
        },nullptr
    });
    if(g){
        chatListModel_->addInfo(g,"[文件]",false);
    }
}

void UIManager::handleSendFileMsg(QString target,ChatMessage msg)
{
    msg.status=ChatMessage::Sent;
    QByteArray array;
    QDataStream out(&array,QIODevice::WriteOnly);
    out<<msg;
    QString id=msg.messageId;
    PullRequest request;
    request.mutable_message_request()->set_target(target.toStdString());
    request.mutable_message_request()->set_type(MessageRequest::File);
    request.mutable_message_request()->mutable_text_message()->set_content(array.constData(),array.size());

    business_->sendData(request,HeaderType::FILEMSG,RequestCallbcak{
        [this,id,target](PullRespond respond){
            if(respond.has_simple_respond()&&chatDialogCache_.contains(target)){
                ChatDialog *dia=chatDialogCache_[target];
                dia->updateInfo(id,ChatMessageModel::StatusRole,respond.simple_respond().success()?ChatMessage::Sent:ChatMessage::Failed);
            }
        },
        [this,id,target]{
            if(chatDialogCache_.contains(target)){
                ChatDialog *dia=chatDialogCache_[target];
                dia->updateInfo(id,ChatMessageModel::StatusRole,ChatMessage::Failed);
            }
        }
    });
}

void UIManager::createChatDialog(QString account, QString name,bool changeChatDia)
{
    if(account==""||name=="")
        return;
    ChatDialog *dia=nullptr;
    if(chatDialogCache_.contains(account)){
        dia=chatDialogCache_[account];
        for(int i=0;i<enterOrder_.size();++i){
            if(enterOrder_[i]==account){
                enterOrder_.move(i,0);
                break;
            }
        }
    }else{

        dia=new ChatDialog(nullptr,account,name,currentLoginUserInfo.name_);
        dia->setIsNotFriend(!allFriends.contains(account));
        connect(dia,&ChatDialog::sendMsgSig,this,&UIManager::handleSendMsg);
        connect(dia,&ChatDialog::writeHistoryMsgSig,DataManager::getInstance(),&DataManager::writeChatMessage);
        connect(dia,&ChatDialog::loadHistoryMsg,[this](QString target,int offset){
            handleReadHistoryChatMsg(target,offset);
        });
        connect(dia,&ChatDialog::removeMsgSig,[this,account](int index,QString uid){
            DataManager::getInstance()->removeMsg(uid,[this,index,account](bool res){
                QMetaObject::invokeMethod(this,[this,index,res,account]{
                    if(chatDialogCache_.contains(account)){
                        ChatDialog *dia=chatDialogCache_[account];
                        dia->setBlock(false);
                        if(res){
                            dia->removeMsg(index);
                        }
                    }
                });
            });
        });
        connect(dia,&ChatDialog::updateChatListAfterDelSig,this,&UIManager::handleUpdateChatListAfterDel);
        connect(dia,&ChatDialog::sendPicSig,this,&UIManager::handleSendPic);
        connect(dia,&ChatDialog::checkHdPicSig,picviewManager_,&PictureViewManager::createView);
        connect(dia,&ChatDialog::sendFileSig,this,&UIManager::handleSendFile);
        connect(dia,&ChatDialog::cancelFileSig,this,[&](QString id){
            fileManager_->stopTask(id);
        });
        connect(dia,&ChatDialog::downloadFileSig,fileManager_,&FileManager::downloadFile);
        enterOrder_.insert(0,account);
        chatDialogCache_[account]=dia;
    }
    while(calculateSize()>MAX_CHATDIALOG_CACHE&&enterOrder_.size()>1){
        QString a=enterOrder_.back();
        enterOrder_.pop_back();
        chatManager_->removeChatDialog(chatDialogCache_[a]);
        chatDialogCache_.remove(a);
    }
    if(changeChatDia){
        chatManager_->setChatDialog(dia);
        chatManager_->setCurrentA(account);
    }
}

void UIManager::handleUpdateChatListAfterDel(QString account,QString content,QDateTime timestamp)
{
    GeneralInfo *g=DataManager::getInstance()->syncGetInfo(account,SyncUserInfoCallback{
        [this,account,content,timestamp]{
            handleUpdateChatListAfterDel(account,content,timestamp);
        },nullptr
    });
    if(!g)return;
    chatListModel_->addInfo(g,content,false,timestamp);
}

void UIManager::handleReadHistoryChatMsg(QString target, int offset)
{
    if(!chatDialogCache_.contains(target)){
        qDebug()<<__FUNCTION__<<"not contains";
        return;
    }
    ChatDialog *dia=chatDialogCache_[target];
    QVector<ChatMessage> res;
    DataManager::getInstance()->readChatMessage(target,offset,res);
    while(res.size()){
        dia->addMsg(res.front(),0);
        res.pop_front();
    }
    if(!res.size())
        dia->setOffset(-1);//代表已获取全部
    else dia->setOffset(res.size());
}

void UIManager::handleSaveImage(QString id,QString savepath)
{
    QImage image=DataManager::getInstance()->getImageCache(id);
    if(!image.isNull()){
        QtConcurrent::run([image,savepath]{
            image.save(savepath);
        });
    }
}

size_t UIManager::calculateSize()
{
    size_t totalSize = 0;
    for (auto it = chatDialogCache_.begin(); it != chatDialogCache_.end(); ++it) {
        totalSize += it.key().capacity() * sizeof(QChar);
        totalSize += sizeof(ChatDialog*);
        if (it.value() != nullptr) {
            totalSize += sizeof(*(it.value()));
        }
    }
    qDebug()<<__FUNCTION__<<totalSize;
    return totalSize;
}



