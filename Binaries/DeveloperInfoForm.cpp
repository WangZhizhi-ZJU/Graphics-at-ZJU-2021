#include "DeveloperInfoForm.h"
#include "WidgetsEvents.h"
#include <QMouseEvent>

Ui::DeveloperInfoForm DeveloperInfoFormClass;

DeveloperInfoForm::DeveloperInfoForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	DeveloperInfoFormClass = ui;
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->closeButton_Default = QPixmap(":/GraphicsatZJU/Resources/Buttons/InfoPageButton01.png");
    this->closeButton_Hover = QPixmap(":/GraphicsatZJU/Resources/Buttons/InfoPageButton02.png");
    this->closeButton_Pressed = QPixmap(":/GraphicsatZJU/Resources/Buttons/InfoPageButton03.png");
	initializeForm();
}

void DeveloperInfoForm::initializeForm()
{
    setMouseTracking(true);
    this->ui.closeButton->setMouseTracking(true);
    QPixmap background = QPixmap(":/GraphicsatZJU/Resources/Backgrounds/Developer.png").scaled(this->size());
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Background, QBrush(background));
    this->setPalette(palette);
}

void DeveloperInfoForm::closeEvent(QCloseEvent* event)
{
	WidgetsEvents::developerFormDispose();
}

void DeveloperInfoForm::closeButton_click()
{
    ui.closeButton->setPixmap(closeButton_Pressed);
}

void DeveloperInfoForm::closeButton_hover()
{
    ui.closeButton->setPixmap(closeButton_Hover);
}

void DeveloperInfoForm::closeButton_release()
{
    ui.closeButton->setPixmap(closeButton_Default);
}

#include <fstream>
#include <sstream>

void DeveloperInfoForm::mousePressEvent(QMouseEvent* event)
{
    isMouseDown = true;
    if (752 < event->x() && event->x() < 780 && 20 < event->y() && event->y() < 48)
        closeButton_click();
}

void DeveloperInfoForm::mouseReleaseEvent(QMouseEvent* event)
{
    isMouseDown = false;
    if (752 < event->x() && event->x() < 780 && 20 < event->y() && event->y() < 48) 
    {
        closeButton_hover();
        WidgetsEvents::developerFormDispose();
    }
}

void DeveloperInfoForm::mouseMoveEvent(QMouseEvent* event)
{
    if (752 < event->x() && event->x() < 780 && 20 < event->y() && event->y() < 48)
    {
        setCursor(Qt::OpenHandCursor);
        if (!isMouseDown)
            closeButton_hover();
        else
            closeButton_click();
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        closeButton_release();
    }
}