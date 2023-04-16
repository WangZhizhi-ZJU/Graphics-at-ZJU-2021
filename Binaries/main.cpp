#include "WelcomeForm.h"
#include <QtWidgets/QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    // ����Qt�Դ��Ի���ķ����ļ�
    QTranslator tran;
    bool ok = tran.load("qt_zh_CN.qm", "C:\\Users\\Administrator\\Desktop\\Graphics at ZJU - Final Version\\Binaries\\Language");
    application.installTranslator(&tran);

    QTranslator user;
    ok = user.load("app_zh_CN.qm", "C:\\Users\\Administrator\\Desktop\\Graphics at ZJU - Final Version\\Binaries\\Language");
    application.installTranslator(&user);

    // ��ڴ�����ʵ������ӭ���棬�ڻ�ӭ�����ʱ���������������
    WelcomeForm* welcomeForm = new WelcomeForm();
    welcomeForm->display();
    return application.exec();
}