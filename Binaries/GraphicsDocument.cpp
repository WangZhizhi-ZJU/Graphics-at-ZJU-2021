#include "GraphicsDocument.h"
#include "GraphicsatZJU.h"
#include "WidgetsEvents.h"
#include <QAction>
#include <QMdiSubWindow>
#include <QtCore>
#include <QtOpenGL/QtOpenGL>
#include <QtGui>
#include "GLArea.h"

extern GraphicsatZJU* MainForm;
extern QTextCodec* translateChinese;
#define ch(args) translateChinese->toUnicode(args)

GraphicsDocument::GraphicsDocument(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    this->setMinimumSize(640, 480);
    this->setAttribute(Qt::WA_DeleteOnClose);

    correspondingMenuButton = nullptr;
    correspondingGLArea = new GLArea(this, 640, 640, &isEdited, closeButton, maximizeButton, MainForm->innerCloseButton, MainForm->innerMaximizeButton);
    // ��Ctrl��Alt��Shift�����º��ͷŵ��¼���������ͨ���ź���۴��ݸ���ͼ����
    QObject::connect(MainForm, SIGNAL(ctrlPressed()), correspondingGLArea, SLOT(setCtrlPressed()));
    QObject::connect(MainForm, SIGNAL(ctrlReleased()), correspondingGLArea, SLOT(setCtrlReleased()));
    QObject::connect(MainForm, SIGNAL(altPressed()), correspondingGLArea, SLOT(setAltPressed()));
    QObject::connect(MainForm, SIGNAL(altReleased()), correspondingGLArea, SLOT(setAltReleased()));
    QObject::connect(MainForm, SIGNAL(shiftPressed()), correspondingGLArea, SLOT(setShiftPressed()));
    QObject::connect(MainForm, SIGNAL(shiftReleased()), correspondingGLArea, SLOT(setShiftReleased()));

    closeButton = new QLabel(this);
    closeButton->setMouseTracking(true);
    closeButton->setMinimumSize(QSize(25, 25));
    closeButton->setMaximumSize(QSize(20, 25));
    closeButton->setPixmap(MainForm->innerCloseButton->scaled(25, 25));
    closeButton->setGeometry(10, 10, 25, 25);

    maximizeButton = new QLabel(this);
    maximizeButton->setMouseTracking(true);
    maximizeButton->setMinimumSize(QSize(25, 25));
    maximizeButton->setMaximumSize(QSize(25, 25));
    maximizeButton->setPixmap(MainForm->innerMaximizeButton->scaled(25, 25));
    maximizeButton->setGeometry(40, 10, 25, 25);

    correspondingGLArea->closeButton = closeButton;
    correspondingGLArea->maximizeButton = maximizeButton;

    this->setMouseTracking(true);
    this->ui.mdiArea->setMouseTracking(true);
}

void GraphicsDocument::mouseMoveEvent(QMouseEvent* event)
{
    if (this->correspondingSubWindow->isMaximized() == false)
        return;
    if (closeButton->x() < event->x() && event->x() < closeButton->x() + closeButton->width() && closeButton->y() < event->y() && event->y() < closeButton->y() + closeButton->height())
        closeButton->setPixmap(MainForm->innerCloseButtonHover->scaled(25, 25));
    else
        closeButton->setPixmap(MainForm->innerCloseButton->scaled(25, 25));
    if (maximizeButton->x() < event->x() && event->x() < maximizeButton->x() + maximizeButton->width() && maximizeButton->y() < event->y() && event->y() < maximizeButton->y() + maximizeButton->height())
        maximizeButton->setPixmap(MainForm->innerMaximizeButtonHover->scaled(25, 25));
    else
        maximizeButton->setPixmap(MainForm->innerMaximizeButton->scaled(25, 25));
}

void GraphicsDocument::mousePressEvent(QMouseEvent* event)
{
    if (this->correspondingSubWindow->isMaximized() == false)
        return;
    if (closeButton->x() < event->x() && event->x() < closeButton->x() + closeButton->width() && closeButton->y() < event->y() && event->y() < closeButton->y() + closeButton->height())
        this->close();
    else if (maximizeButton->x() < event->x() && event->x() < maximizeButton->x() + maximizeButton->width() && maximizeButton->y() < event->y() && event->y() < maximizeButton->y() + maximizeButton->height())
        this->correspondingSubWindow->showNormal();
}

void GraphicsDocument::mouseReleaseEvent(QMouseEvent* event)
{

}

// ͼ���ĵ��Ĺر��¼�
void GraphicsDocument::closeEvent(QCloseEvent* event) 
{
    if (this->isEdited || !this->isSaved)
    {
        QApplication::beep();
        int result = QMessageBox::warning(MainForm, ch("�ر��ĵ�"), ch("�Ƿ񱣴�Ե�ǰ�ĵ���") + this->documentName + ch(".gdoc���ĸ��ģ�"), ch("����"), ch("������"), ch("ȡ��"));
        if (result == QMessageBox::AcceptRole)
        {
            MainForm->ui.menuButton_Save->trigger();
            if (!this->isSaved || isEdited)
            {
                event->ignore();
                return;
            }
        }
        else if (result == QMessageBox::RejectRole);
        else if (result == QMessageBox::DestructiveRole)
        {
            event->ignore();
            return;
        }
    }
    // �ر�ͼ���ĵ���ͬʱ�����Ӧ�Ĳ˵�����ť���ز�ɾ���ͷ�
    correspondingMenuButton->setVisible(false);
    // �رմ��ڵ�ͬʱ���Ӽ�¼�����Ƴ���Ӧ�Ĳ˵�����
    for (std::vector<QAction*>::iterator it = WidgetsEvents::menuButton_gDocsList.begin(); it < WidgetsEvents::menuButton_gDocsList.end(); it++)
        if (*it == correspondingMenuButton) {
            WidgetsEvents::menuButton_gDocsList.erase(it);
            break;
        }
    // ����б�ǿգ��Զ�ѡ������б��е����һ����ť��������Ӧ��������Ϊ����
    if (WidgetsEvents::menuButton_gDocsList.size() != 0) {
        WidgetsEvents::menuButton_gDocsList[WidgetsEvents::menuButton_gDocsList.size() - 1]->setChecked(true);
        WidgetsEvents::menuButton_to_SubWindow[WidgetsEvents::menuButton_gDocsList[WidgetsEvents::menuButton_gDocsList.size() - 1]]->setFocus();
    }
    delete correspondingMenuButton;
    delete correspondingGLArea;
    correspondingSubWindow->close();
}

void GraphicsDocument::focusInEvent(QFocusEvent* event) 
{
    emit focusedIn();
}

void GraphicsDocument::focusOutEvent(QFocusEvent* event)
{
}

// ͼ���ĵ��Ĵ�С�ı��¼�
void GraphicsDocument::resizeEvent(QResizeEvent* event) 
{
    QWidget::resizeEvent(event);
    if (correspondingGLArea == nullptr)
        return;
    // ����mdiArea�ĳߴ�
    ui.mdiArea->resize(geometry().width(), geometry().height());
    correspondingGLArea->resize(ui.mdiArea->width(), ui.mdiArea->height());
}

// ͼ���ĵ����ƶ��¼�
void GraphicsDocument::changeEvent(QEvent* event) 
{
    if (event->type() != QEvent::WindowStateChange)
        return;
    QWidget::changeEvent(event);
    correspondingGLArea->resize(ui.mdiArea->width(), ui.mdiArea->height());

    // �Ӵ������ʱ�������Ӵ����ͼ�ꡢ��󻯡��رհ�ť��������ʾ֮
    if (correspondingSubWindow != nullptr)
    {
        if (correspondingSubWindow->isMaximized())
        {
            correspondingSubWindow->setWindowFlag(Qt::WindowCloseButtonHint, false);
            correspondingSubWindow->setWindowFlag(Qt::WindowMaximizeButtonHint, false);
            correspondingSubWindow->setWindowFlag(Qt::WindowMinimizeButtonHint, true);
            correspondingSubWindow->setWindowFlag(Qt::FramelessWindowHint, true);
            closeButton->setVisible(true);
            maximizeButton->setVisible(true);
            closeButton->update();
            maximizeButton->update();
        }
        else
        {
            correspondingSubWindow->setWindowFlag(Qt::WindowCloseButtonHint, true);
            correspondingSubWindow->setWindowFlag(Qt::WindowMaximizeButtonHint, true);
            correspondingSubWindow->setWindowFlag(Qt::WindowMinimizeButtonHint, false);
            correspondingSubWindow->setWindowFlag(Qt::FramelessWindowHint, false);
            closeButton->setVisible(false);
            maximizeButton->setVisible(false);
        }
    }
}