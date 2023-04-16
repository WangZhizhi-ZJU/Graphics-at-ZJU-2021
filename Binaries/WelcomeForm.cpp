#include "WelcomeForm.h"
#include "GraphicsatZJU.h"
#include <QGraphicsScene>
#include <QTimer>

Ui::WelcomeForm WelcomeFormClass;	// WelcomeForm��ʵ�������ⲿ����

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
	// ���ػ�ӭ����ı�����
	this->setWindowFlags(Qt::FramelessWindowHint);
	// �½�һ���������������ر���ͼ��
	scene = new QGraphicsScene();
	// ������ͼ����ص�����
	scene->addPixmap(QPixmap::fromImage(QImage(":/GraphicsatZJU/Resources/Backgrounds/Welcome.png")));
	// ����������Ϊ�����ı���ͼ��
	WelcomeFormClass.graphicsView->setScene(scene);
}

// �Ի�ӭ����ԭ��show()���������������2��ļ�ʱ����
void WelcomeForm::display()
{
	this->show();
	QTimer::singleShot(3000, this, SLOT(welcomeFormQuit()));
}

// ���ڼ�ʱ��������رջ�ӭ���沢�������ڵĲۺ���
void WelcomeForm::welcomeFormQuit()
{
	this->close();
    GraphicsatZJU* mainForm = new GraphicsatZJU();
	mainForm->show();
}