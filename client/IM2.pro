QT       += core gui quickwidgets network concurrent sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BusinessLayer/mainbusiness.cpp \
    DataLayer/datamanager.cpp \
    DataLayer/sqlitemanager.cpp \
    IM_login.pb.cc \
    NetworkLayer/networkmanager.cpp \
    UILayer/FileManager/FileStruct.pb.cc \
    UILayer/FileManager/filemanager.cpp \
    UILayer/Model/chatlistmodel.cpp \
    UILayer/Model/chatmessagemodel.cpp \
    UILayer/Model/friendlistmodel.cpp \
    UILayer/Model/friendverifymodel.cpp \
    UILayer/Model/searchmodel.cpp \
    UILayer/PictureViewer/pictureview.cpp \
    UILayer/PictureViewer/pictureviewmanager.cpp \
    UILayer/confirmactiondialog.cpp \
    UILayer/customizequickwidget.cpp \
    UILayer/framless_dialog.cpp \
    UILayer/imageprovider.cpp \
    UILayer/loginui.cpp \
    UILayer/mainWindow/chatdialog.cpp \
    UILayer/mainWindow/chatmanager.cpp \
    UILayer/mainWindow/friendmanager.cpp \
    UILayer/mainWindow/friendrequestdialog.cpp \
    UILayer/mainWindow/mainwindow.cpp \
    UILayer/mainWindow/searchdialog.cpp \
    UILayer/modifyavatordialog.cpp \
    UILayer/qmlsharedata.cpp \
    UILayer/register_dialog.cpp \
    UILayer/uimanager.cpp \
    UILayer/unresizewindowsbackgruond.cpp \
    UILayer/userinfodialog.cpp \
    UILayer/windows_background.cpp \
    constants.cpp \
    main.cpp

HEADERS += \
    BusinessLayer/mainbusiness.h \
    DataLayer/datamanager.h \
    DataLayer/sqlitemanager.h \
    IM_login.pb.h \
    NetworkLayer/networkmanager.h \
    UILayer/FileManager/FileStruct.pb.h \
    UILayer/FileManager/filemanager.h \
    UILayer/Model/chatlistmodel.h \
    UILayer/Model/chatmessagemodel.h \
    UILayer/Model/friendlistmodel.h \
    UILayer/Model/friendverifymodel.h \
    UILayer/Model/searchmodel.h \
    UILayer/PictureViewer/pictureview.h \
    UILayer/PictureViewer/pictureviewmanager.h \
    UILayer/confirmactiondialog.h \
    UILayer/customizequickwidget.h \
    UILayer/framless_dialog.h \
    UILayer/imageprovider.h \
    UILayer/loginui.h \
    UILayer/mainWindow/chatdialog.h \
    UILayer/mainWindow/chatmanager.h \
    UILayer/mainWindow/friendmanager.h \
    UILayer/mainWindow/friendrequestdialog.h \
    UILayer/mainWindow/mainwindow.h \
    UILayer/mainWindow/searchdialog.h \
    UILayer/modifyavatordialog.h \
    UILayer/qmlsharedata.h \
    UILayer/register_dialog.h \
    UILayer/uimanager.h \
    UILayer/unresizewindowsbackgruond.h \
    UILayer/userinfodialog.h \
    UILayer/windows_background.h \
    constants.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD\protobuf\protobuf-3.15.0\src \

LIBS += -ldwmapi \
        -L$$PWD\protobuf\build -lprotobuf \

RESOURCES += \
    qml.qrc \
    res.qrc
