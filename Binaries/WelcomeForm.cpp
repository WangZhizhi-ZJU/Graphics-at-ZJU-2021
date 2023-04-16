#include "WelcomeForm.h"
#include "GraphicsatZJU.h"
#include <QGraphicsScene>
#include <QTimer>

Ui::WelcomeForm WelcomeFormClass;	// WelcomeForm的实例化供外部调用

WelcomeForm::WelcomeForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	WelcomeFormClass = ui;
    this->setAttribute(Qt::WA_DeleteOnClose);
	initializeForm();
}

void WelcomeForm::initializeForm()
{
	// 隐藏欢迎界面的标题栏
	this->setWindowFlags(Qt::FramelessWindowHint);
	// 新建一个场景，用来承载背景图像
	scene = new QGraphicsScene();
	// 将背景图像挂载到场景
	scene->addPixmap(QPixmap::fromImage(QImage(":/GraphicsatZJU/Resources/Backgrounds/Welcome.png")));
	// 将场景设置为画布的背景图像
	WelcomeFormClass.graphicsView->setScene(scene);
}

// 对欢迎界面原本show()方法的提升，添加2秒的计时功能
void WelcomeForm::display()
{
	this->show();
	QTimer::singleShot(3000, this, SLOT(welcomeFormQuit()));
}

// 用于计时器结束后关闭欢迎界面并打开主窗口的槽函数
void WelcomeForm::welcomeFormQuit()
{
	this->close();
    GraphicsatZJU* mainForm = new GraphicsatZJU();
	mainForm->show();
}