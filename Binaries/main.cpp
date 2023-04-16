#include "WelcomeForm.h"
#include <QtWidgets/QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    // 加载Qt自带对话框的翻译文件
    QTranslator tran;
    bool ok = tran.load("qt_zh_CN.qm", "C:\\Users\\Administrator\\Desktop\\Graphics at ZJU - Final Version\\Binaries\\Language");
    application.installTranslator(&tran);

    QTranslator user;
    ok = user.load("app_zh_CN.qm", "C:\\Users\\Administrator\\Desktop\\Graphics at ZJU - Final Version\\Binaries\\Language");
    application.installTranslator(&user);

    // 入口处首先实例化欢迎界面，在欢迎界面计时结束后调用主窗体
    WelcomeForm* welcomeForm = new WelcomeForm();
    welcomeForm->display();
    return application.exec();
}