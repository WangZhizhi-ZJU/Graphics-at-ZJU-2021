/**
 * @project: Graphics at ZJU
 **/

#include "GraphicsatZJU.h"
#include "WidgetsEvents.h"
#include "GraphicsDocument.h"
#include <QCloseEvent>
#include <QDebug>
#include <QTextCodeC>
#include <QMessageBox>
#include <vector>
#include <QComboBox>
#include <QWidget>
#include <queue>
#include <QPushButton>
#include <qmath.h>

// ui对象的镜像，用来外部访问主窗体内对象
Ui::GraphicsatZJUClass MainFormClass;

// 主窗体的镜像，用来外部控制主窗体
GraphicsatZJU* MainForm = nullptr;

WidgetsEvents* WidgetsEventsMetaObject = new WidgetsEvents();
std::vector<QAction*> WidgetsEvents::menuButton_gDocsList;              // 声明静态变量用于储存"窗口"菜单栏动态添加的菜单按钮
std::unordered_map<QAction*, GraphicsDocument*> WidgetsEvents::menuButton_to_SubWindow;  // 声明静态变量用于储存"窗口"菜单栏动态添加的菜单按钮和对应窗体间的映射
std::unordered_map<GraphicsDocument*, QAction*> WidgetsEvents::subWindow_to_MenuButton;  // 声明静态变量用于储存窗体间和对应"窗口"菜单栏动态添加的菜单按钮的映射

// 自定义信号的接受者WidgetsEventsClass
#define WidgetsEventsClass WidgetsEventsMetaObject
#define delegator

// 中文Unicode码翻译器
QTextCodec* translateChinese = QTextCodec::codecForName("GBK");

GraphicsatZJU::GraphicsatZJU(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    MainFormClass = ui;
    MainForm = this;

    setWindowFlags(Qt::FramelessWindowHint);

    WidgetsEvents_connect(MainFormClass);
    toolButtons_setCheckable();
    menuButtons_setCheckable();
    correspondingGLInfo = new GLInfo(this, geometry());
    statusBarIcon = new QLabel();
    statusBarIcon->setScaledContents(false);
    statusBarIcon->setMinimumSize(QSize(18, 18));
    statusBarIcon->setMaximumSize(QSize(18, 18));
    ui.statusBar->addWidget(statusBarIcon);
    statusBarLabel = new QLabel();
    statusBarLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    statusBarLabel->setMinimumSize(100, 18);
    statusBarLabel->setMaximumSize(100, 18);
    ui.statusBar->addWidget(statusBarLabel);
    comboBox = new QComboBox();
    comboBox->addItem(translateChinese->toUnicode("当前无打开文档"));
    comboBox->setMinimumWidth(120);
    comboBox->setMaximumWidth(200);
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui.toolBar->insertWidget(ui.fixedButton_Doc, comboBox);
    ui.fixedButton_Doc->setVisible(false);

    closeButtonHover = new QPixmap(":/GraphicsatZJU/Resources/Buttons/QuitButton01.png");
    closeButtonPressed = new QPixmap(":/GraphicsatZJU/Resources/Buttons/QuitButton02.png");
    maximizeButtonHover = new QPixmap(":/GraphicsatZJU/Resources/Buttons/Maximize01.png");
    maximizeButtonPressed = new QPixmap(":/GraphicsatZJU/Resources/Buttons/Maximize02.png");
    minimizeButtonHover = new QPixmap(":/GraphicsatZJU/Resources/Buttons/Minimize01.png");
    minimizeButtonPressed = new QPixmap(":/GraphicsatZJU/Resources/Buttons/Minimize02.png");
    mainFormIconActive = new QPixmap(":/GraphicsatZJU/IDI_ICON1");
    innerCloseButton = new QPixmap(":/GraphicsatZJU/Resources/Buttons/closeButton01.png");
    innerCloseButtonHover = new QPixmap(":/GraphicsatZJU/Resources/Buttons/closeButton02.png");
    innerMaximizeButton = new QPixmap(":/GraphicsatZJU/Resources/Buttons/maximizeButton01.png");
    innerMaximizeButtonHover = new QPixmap(":/GraphicsatZJU/Resources/Buttons/maximizeButton02.png");

    setMouseTracking(true);
    ui.toolBar->setMouseTracking(true);
    ui.menuBar->setMouseTracking(true);
    ui.statusBar->setMouseTracking(true);

    ui.menuBar->setStyleSheet("QMenuBar{background:rgb(238, 238, 242);  \
                                        margin:6px 6px;                 \
                                        padding:0px 3px;                \
                                        font-size:13px;                 \
                                        font-family:Microsoft YaHei};");

    auto menuStyles = "QMenu{font-size:13px; font-family:Microsoft YaHei};}";
    ui.menu_F->setStyleSheet(menuStyles);
    ui.menu_E->setStyleSheet(menuStyles);
    ui.menu_L->setStyleSheet(menuStyles);
    ui.menu_V->setStyleSheet(menuStyles);
    ui.menu_W->setStyleSheet(menuStyles);
    ui.menu_T->setStyleSheet(menuStyles);
    ui.menu_H->setStyleSheet(menuStyles);

    ui.toolBar->setStyleSheet("QToolBar{background:rgb(238, 238, 242);  \
                                        border:none;                    \
                                        margin:5px 5px};                \
                               QToolBar::item{font-size:13px;           \
                                        font-family:Microsoft YaHei");
    comboBox->setStyleSheet("font-size:13px; font-family:Microsoft YaHei");

    ui.statusBar->setStyleSheet("QStatusBar{background:rgb(238, 238, 242); font-size:13px; font-family:Microsoft YaHei; margin:5px 5px}");
    statusBarLabel->setStyleSheet("font-size:13px; font-family:Microsoft YaHei");

    currentDocument = new QLabel(translateChinese->toUnicode("Graphics at ZJU"), this);
    currentDocument->setMouseTracking(true);
    currentDocument->setMaximumSize(3840, 25);
    currentDocument->setMinimumSize(40, 25);
    currentDocument->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    currentDocument->setFont(QFont("Microsoft Yahei", 10, QFont::Bold));
    currentDocument->adjustSize();
    currentDocument->setGeometry(ui.menuBar->x() + ui.menuBar->width() + 10, 5, currentDocument->width() + 10, 25);
    currentDocument->setStyleSheet("background-color:rgb(224, 227, 230)");

    originButtons = new QLabel(this);
    originButtons->setMouseTracking(true);
    originButtons->setMaximumSize(QSize(155, 32));
    originButtons->setMinimumSize(QSize(155, 32));
    originButtons->setPixmap(QPixmap(":/GraphicsatZJU/Resources/Buttons/MainWndBackground.png"));
    originButtons->setGeometry(this->width() - originButtons->width() - 1, 1, originButtons->width(), originButtons->height());
    originButtons->show();

    closeButton = new QLabel(this);
    closeButton->setMouseTracking(true);
    closeButton->setMaximumSize(QSize(45, 25));
    closeButton->setMinimumSize(QSize(45, 25));
    closeButton->setPixmap(*closeButtonHover);
    closeButton->setGeometry(this->width() - closeButton->width() - 1, 1, closeButton->width(), closeButton->height());
    closeButton->setVisible(false);

    maximizeButton = new QLabel(this);
    maximizeButton->setMouseTracking(true);
    maximizeButton->setMaximumSize(QSize(45, 25));
    maximizeButton->setMinimumSize(QSize(45, 25));
    maximizeButton->setPixmap(*maximizeButtonHover);
    maximizeButton->setGeometry(this->width() - maximizeButton->width() - 51, 1, maximizeButton->width(), maximizeButton->height());
    maximizeButton->setVisible(false);

    minimizeButton = new QLabel(this);
    minimizeButton->setMouseTracking(true);
    minimizeButton->setMaximumSize(QSize(45, 25));
    minimizeButton->setMinimumSize(QSize(45, 25));
    minimizeButton->setPixmap(*minimizeButtonHover);
    minimizeButton->setGeometry(this->width() - minimizeButton->width() - 101, 1, minimizeButton->width(), minimizeButton->height());
    minimizeButton->setVisible(false);

    mainFormIcon = new QLabel(this);
    mainFormIcon->setMouseTracking(true);
    mainFormIcon->setMaximumSize(QSize(23, 23));
    mainFormIcon->setMinimumSize(QSize(23, 23));
    mainFormIcon->setPixmap(mainFormIconActive->scaled(QSize(23, 23)));
    mainFormIcon->setGeometry(7, 7, mainFormIcon->width(), mainFormIcon->height());

    ui.mdiArea->setMouseTracking(true);
    ui.centralWidget->setMouseTracking(true);
    setAutoFillBackground(false);
}

#define PADDING 2

void GraphicsatZJU::region(const QPoint& cursorGlobalPoint)
{
    QRect rect = this->rect();
    QPoint coordTopLeft = mapToGlobal(rect.topLeft());
    QPoint coordRightBottom = mapToGlobal(rect.bottomRight());

    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

    if (coordTopLeft.x() + PADDING >= x && coordTopLeft.x() <= x && coordTopLeft.y() + PADDING >= y && coordTopLeft.y() <= y)
    {
        dragStatus = LEFTTOP;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if (x >= coordRightBottom.x() - PADDING && x <= coordRightBottom.x() && y >= coordRightBottom.y() - PADDING && y <= coordRightBottom.y())
    {
        dragStatus = RIGHTBOTTOM;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if (x <= coordTopLeft.x() + PADDING && x >= coordTopLeft.x() && y >= coordRightBottom.y() - PADDING && y <= coordRightBottom.y())
    {
        dragStatus = LEFTBOTTOM;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if (x <= coordRightBottom.x() && x >= coordRightBottom.x() - PADDING && y >= coordTopLeft.y() && y <= coordTopLeft.y() + PADDING)
    {
        dragStatus = RIGHTTOP;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if (x <= coordTopLeft.x() + PADDING && x >= coordTopLeft.x())
    {
        dragStatus = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if (x <= coordRightBottom.x() && x >= coordRightBottom.x() - PADDING)
    {
        dragStatus = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if (y >= coordTopLeft.y() && y <= coordTopLeft.y() + PADDING)
    {
        dragStatus = UP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if (y <= coordRightBottom.y() && y >= coordRightBottom.y() - PADDING)
    {
        dragStatus = DOWN;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else
    {
        dragStatus = NONE;
    }
}

// 为主窗体绘制蓝色边框
void GraphicsatZJU::resizePaint()
{
    QPainter* painter = new QPainter(this);
    painter->setPen(QPen(QColor(0, 122, 204), 1));
    painter->drawRect(borderDistance, borderDistance, this->width() - 2 * borderDistance - 1, this->height() - 2 * borderDistance - 1);
    painter->end();
    delete painter;
}

// 为窗体绘制阴影，会导致绘制变慢，不采用
void GraphicsatZJU::shadowPaint()
{
    /*QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    QRectF rect(borderDistance, borderDistance, this->width() - 2 * borderDistance, this->height() - 2 * borderDistance);
    path.addRect(rect);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(QColor(238, 238, 242)));

    QColor color(0, 0, 0, 50);
    for (int i = 0; i < borderDistance; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(borderDistance - i, borderDistance - i, this->width() - (borderDistance - i) * 2, this->height() - (borderDistance - i) * 2);
        color.setAlpha(255 - qSqrt(i) * 200);
        painter.setPen(color);
        painter.drawPath(path);
    }*/
}

void GraphicsatZJU::paintEvent(QPaintEvent* event)
{
    //GraphicsatZJU::shadowPaint();
    GraphicsatZJU::resizePaint();
}

void GraphicsatZJU::mouseMoveEvent(QMouseEvent* event)
{
    bool isUpdate = false;

    if (closeButton->x() < event->x() && event->x() < closeButton->x() + closeButton->width() && closeButton->y() < event->y() && event->y() < closeButton->y() + closeButton->height())
    {
        isUpdate = true;
        isMouseDragging = false;
        this->setCursor(Qt::OpenHandCursor);
        closeButton->setPixmap(*closeButtonHover);
        closeButton->setVisible(true);
    }
    else
    {
        closeButton->setVisible(false);
    }

    if (maximizeButton->x() < event->x() && event->x() < maximizeButton->x() + maximizeButton->width() && maximizeButton->y() < event->y() && event->y() < maximizeButton->y() + maximizeButton->height())
    {
        isUpdate = true;
        isMouseDragging = false;
        this->setCursor(Qt::OpenHandCursor);
        maximizeButton->setPixmap(*maximizeButtonHover);
        maximizeButton->setVisible(true);
    }
    else
    {
        maximizeButton->setVisible(false);
    }

    if (minimizeButton->x() < event->x() && event->x() < minimizeButton->x() + minimizeButton->width() && minimizeButton->y() < event->y() && event->y() < minimizeButton->y() + minimizeButton->height())
    {
        isUpdate = true;
        isMouseDragging = false;
        this->setCursor(Qt::OpenHandCursor);
        minimizeButton->setPixmap(*minimizeButtonHover);
        minimizeButton->setVisible(true);
    }
    else
        minimizeButton->setVisible(false);

    if (isUpdate == false)
        this->setCursor(Qt::ArrowCursor);
    
    // 这三步是防止触发按钮后瞬间又触发拖动缩放导致缩放异常
    if (event->x() > closeButton->x() && event->y() < closeButton->y() + closeButton->height())
        return;
    if (maximizeButton->x() < event->x() && event->x() < maximizeButton->x() + maximizeButton->width() && event->y() < maximizeButton->y() + maximizeButton->height())
        return;
    if (minimizeButton->x() < event->x() && event->x() < minimizeButton->x() + minimizeButton->width() && event->y() < minimizeButton->y() + minimizeButton->height())
        return;

    if (isMouseDragging && this->isMaximized())
    {
        this->showNormal();
        mouseReleaseEvent(event);
        isMouseDragging = true;
        setGeometry(event->x() - this->normalGeometry().width() / 4, this->y(), this->width(), this->height());
        mouseDragPosition = event->globalPos() - this->pos();  
        return;
    }

    QPoint gloPoint = event->globalPos();
    QRect rect = this->rect();
    QPoint coordTopLeft = mapToGlobal(rect.topLeft());
    QPoint coordRightBottom = mapToGlobal(rect.bottomRight());

    if (!isLeftPressDown)
        this->region(gloPoint);
    else
    {
        if (dragStatus != NONE)
        {
            QRect rMove(coordTopLeft, coordRightBottom);
            switch (dragStatus)
            {
                case LEFT:
                    if (coordRightBottom.x() - gloPoint.x() <= this->minimumWidth())
                        rMove.setX(coordTopLeft.x());
                    else
                        rMove.setX(gloPoint.x());
                    break;
                case RIGHT:
                    rMove.setWidth(gloPoint.x() - coordTopLeft.x());
                    break;
                case UP:
                    if (coordRightBottom.y() - gloPoint.y() <= this->minimumHeight())
                        rMove.setY(coordTopLeft.y());
                    else
                        rMove.setY(gloPoint.y());
                    break;
                case DOWN:
                    rMove.setHeight(gloPoint.y() - coordTopLeft.y());
                    break;
                case LEFTTOP:
                    if (coordRightBottom.x() - gloPoint.x() <= this->minimumWidth())
                        rMove.setX(coordTopLeft.x());
                    else
                        rMove.setX(gloPoint.x());
                    if (coordRightBottom.y() - gloPoint.y() <= this->minimumHeight())
                        rMove.setY(coordTopLeft.y());
                    else
                        rMove.setY(gloPoint.y());
                    break;
                case RIGHTTOP:
                    rMove.setWidth(gloPoint.x() - coordTopLeft.x());
                    rMove.setY(gloPoint.y());
                    break;
                case LEFTBOTTOM:
                    rMove.setX(gloPoint.x());
                    rMove.setHeight(gloPoint.y() - coordTopLeft.y());
                    break;
                case RIGHTBOTTOM:
                    rMove.setWidth(gloPoint.x() - coordTopLeft.x());
                    rMove.setHeight(gloPoint.y() - coordTopLeft.y());
                    break;
                default:
                    break;
            }
            this->setGeometry(rMove);
            return;
        }
    }
    if (isMouseDragging)
        move(event->globalPos() - mouseDragPosition);
}

void GraphicsatZJU::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (this->isMaximized())
        this->showNormal();
    else
        this->showMaximized();
}

void GraphicsatZJU::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        isLeftPressDown = true;
        if (dragStatus != NONE)
            this->mouseGrabber();
        else
            dragPosition = event->globalPos() - this->frameGeometry().topLeft();
    }

    if (closeButton->x() < event->x() && event->x() < closeButton->x() + closeButton->width() && closeButton->y() < event->y() && event->y() < closeButton->y() + closeButton->height())
    {
        this->setCursor(Qt::ClosedHandCursor);
        closeButton->setPixmap(*closeButtonPressed);
    }
    else if (maximizeButton->x() < event->x() && event->x() < maximizeButton->x() + maximizeButton->width() && maximizeButton->y() < event->y() && event->y() < maximizeButton->y() + maximizeButton->height())
    {
        this->setCursor(Qt::ClosedHandCursor);
        maximizeButton->setPixmap(*maximizeButtonPressed);
    }
    else if (minimizeButton->x() < event->x() && event->x() < minimizeButton->x() + minimizeButton->width() && minimizeButton->y() < event->y() && event->y() < minimizeButton->y() + minimizeButton->height())
    {
        this->setCursor(Qt::ClosedHandCursor);
        minimizeButton->setPixmap(*minimizeButtonPressed);
    }
    else if (event->button() == Qt::LeftButton) 
    {
        isMouseDragging = true;
        mouseDragPosition = event->globalPos() - this->pos();
        event->accept();
    }
}

void GraphicsatZJU::mouseReleaseEvent(QMouseEvent* event)
{
    isMouseDragging = false;
    if (dragStatus == RIGHTTOP)
        dragStatus = NONE;

    if (closeButton->x() < event->x() && event->x() < closeButton->x() + closeButton->width() && closeButton->y() < event->y() && event->y() < closeButton->y() + closeButton->height())
    {
        this->setCursor(Qt::OpenHandCursor);
        closeButton->setPixmap(*closeButtonHover);
        MainForm->close();
    }
    else if (maximizeButton->x() < event->x() && event->x() < maximizeButton->x() + maximizeButton->width() && maximizeButton->y() < event->y() && event->y() < maximizeButton->y() + maximizeButton->height())
    {
        this->setCursor(Qt::OpenHandCursor);
        maximizeButton->setPixmap(*maximizeButtonHover);
        maximizeButton->setVisible(false);
        if (MainForm->isMaximized() == false)
            MainForm->showMaximized();
        else
            MainForm->showNormal();
    }
    else if (minimizeButton->x() < event->x() && event->x() < minimizeButton->x() + minimizeButton->width() && minimizeButton->y() < event->y() && event->y() < minimizeButton->y() + minimizeButton->height())
    {
        this->setCursor(Qt::OpenHandCursor);
        minimizeButton->setPixmap(*minimizeButtonHover);
        minimizeButton->setVisible(false);
        MainForm->showMinimized();
    }

    if (event->button() == Qt::LeftButton)
    {
        isLeftPressDown = false;
        if (dragStatus != NONE)
        {
            this->releaseMouse();
        }
    }
}

// 全局控件事件链接器，在这里添加事件
void WidgetsEvents_connect(Ui::GraphicsatZJUClass ui = MainFormClass)
{
    // 工具栏"打开"按钮的点击事件
    QObject::connect(ui.toolButton_Open, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Open_click()));
    // 工具栏"关闭文档"按钮的点击事件
    QObject::connect(ui.toolButton_Close, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Close_click()));
    // 工具栏"新建"按钮的点击事件
    QObject::connect(ui.toolButton_New, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_New_click()));
    // 工具栏"重载"按钮的点击事件
    QObject::connect(ui.toolButton_Reload, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Reload_click()));
    // 工具栏"导出"按钮的点击事件
    QObject::connect(ui.toolButton_Export, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Export_click()));
    // 工具栏"截屏"按钮的点击事件
    QObject::connect(ui.toolButton_Snapshot, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Snapshot_click()));
    QObject::connect(ui.menuButton_Snapshot, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Snapshot_click()));
    // 工具栏"边界框"按钮的点击事件
    QObject::connect(ui.toolButton_Bounding, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Bounding_click()));
    // 工具栏"点阵"按钮的点击事件
    QObject::connect(ui.toolButton_Points, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Points_click()));
    // 工具栏"线框"按钮的点击事件
    QObject::connect(ui.toolButton_Wireframe, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Wireframe_click()));
    // 工具栏"线框填充"按钮的点击事件
    QObject::connect(ui.toolButton_FlatLine, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_FlatLine_click()));
    // 工具栏"面填充"按钮的点击事件
    QObject::connect(ui.toolButton_FlatFill, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_FlatFill_click()));
    // 工具栏"平滑填充"按钮的点击事件
    QObject::connect(ui.toolButton_SmoothFill, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_SmoothFill_click()));
    // 工具栏"开发者信息"按钮的点击事件
    QObject::connect(ui.toolButton_Info, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Info_click()));
    // 工具栏"光照"按钮的点击事件
    QObject::connect(ui.toolButton_LightSource, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_LightSource_click()));
    // 工具栏"显示中心点"按钮的点击事件
    QObject::connect(ui.toolButton_CenterPoint, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_CenterPoint_click()));
    // 工具栏"显示包围盒"按钮的点击事件
    QObject::connect(ui.toolButton_BoundingBox, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_BoundingBox_click()));
    // 工具栏"显示坐标轴"按钮的点击事件
    QObject::connect(ui.toolButton_Coordinate, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Coordinate_click()));
    // 工具栏"渐变背景"按钮的点击事件
    QObject::connect(ui.toolButton_GradientBkg, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_GradientBkg_click()));
    // 工具栏"网格背景"按钮的点击事件
    QObject::connect(ui.toolButton_MeshBkg, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_MeshBkg_click()));
    // 工具栏"黑色背景"按钮的点击事件
    QObject::connect(ui.toolButton_EmptyBkg, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_EmptyBkg_click()));
    // 工具栏"使用透视投影"按钮的点击事件
    QObject::connect(ui.toolButton_Perspective, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Perspective_click()));
    // 工具栏"使用正交投影"按钮的点击事件
    QObject::connect(ui.toolButton_Orthogonality, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Orthogonality_click()));
    // 工具栏"状态监视器"按钮的点击事件
    QObject::connect(ui.toolButton_StatusMonitor, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_StatusMonitor_click()));


    // 下拉菜单的"开发者信息"按钮的点击事件
    QObject::connect(ui.menuButton_Info, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Info_click()));
    // 下拉菜单的"打开"按钮的点击事件
    QObject::connect(ui.menuButton_Open, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Open_click()));
    // 下拉菜单的"新建"按钮的点击事件
    QObject::connect(ui.menuButton_New, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_New_click()));
    // 下拉菜单的"关闭文档"按钮的点击事件
    QObject::connect(ui.menuButton_Close, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Close_click()));
    // 下拉菜单的"关闭文档"按钮的点击事件
    QObject::connect(ui.menuButton_CloseAll, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_CloseAll_click()));
    // 下拉菜单的"重载"按钮的点击事件
    QObject::connect(ui.menuButton_Reload, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Reload_click()));
    // 下拉菜单的"点阵"按钮的点击事件
    QObject::connect(ui.menuButton_Points, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Points_click()));
    // 下拉菜单的"线框"按钮的点击事件
    QObject::connect(ui.menuButton_Wireframe, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Wireframe_click()));
    // 下拉菜单的"线框填充"按钮的点击事件
    QObject::connect(ui.menuButton_FlatLine, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_FlatLine_click()));
    // 下拉菜单的"面填充"按钮的点击事件
    QObject::connect(ui.menuButton_FlatFill, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_FlatFill_click()));
    // 下拉菜单的"平滑填充"按钮的点击事件
    QObject::connect(ui.menuButton_SmoothFill, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_SmoothFill_click()));
    // 下拉菜单的"开发者信息"按钮的点击事件
    QObject::connect(ui.menuButton_Info, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Info_click()));
    // 下拉菜单的"光照"按钮的点击事件
    QObject::connect(ui.menuButton_LightSource, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_LightSource_click()));
    // 下拉菜单的"显示中心点"按钮的点击事件
    QObject::connect(ui.menuButton_CenterPoint, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_CenterPoint_click()));
    // 下拉菜单的"显示包围盒"按钮的点击事件
    QObject::connect(ui.menuButton_BoundingBox, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_BoundingBox_click()));
    // 下拉菜单的"显示坐标轴"按钮的点击事件
    QObject::connect(ui.menuButton_Coordinate, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Coordinate_click()));
    // 下拉菜单的"渐变背景"按钮的点击事件
    QObject::connect(ui.menuButton_GradientBkg, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_GradientBkg_click()));
    // 下拉菜单的"网格背景"按钮的点击事件
    QObject::connect(ui.menuButton_MeshBkg, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_MeshBkg_click()));
    // 下拉菜单的"黑色背景"按钮的点击事件
    QObject::connect(ui.menuButton_EmptyBkg, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_EmptyBkg_click()));
    // 下拉菜单的"使用透视投影"按钮的点击事件
    QObject::connect(ui.menuButton_Perspective, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Perspective_click()));
    // 下拉菜单的"使用正交投影"按钮的点击事件
    QObject::connect(ui.menuButton_Orthogonality, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Orthogonality_click()));
    // 下拉菜单的"状态监视器"按钮的点击事件
    QObject::connect(ui.menuButton_StatusMonitor, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_StatusMonitor_click()));
    // 工具栏"窗口化文档"按钮的点击事件
    QObject::connect(ui.menuButton_Windowed, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Windowed_click()));
    // 工具栏"窗口化全部文档"按钮的点击事件
    QObject::connect(ui.menuButton_WindowedAll, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_WindowedAll_click()));
    // 工具栏"窗口化文档"按钮的点击事件
    QObject::connect(ui.menuButton_Maximize, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Maximize_click()));
    // 工具栏"窗口化全部文档"按钮的点击事件
    QObject::connect(ui.menuButton_MaximizeAll, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_MaximizeAll_click()));
    // 工具栏"退出程序"按钮的点击事件
    QObject::connect(ui.menuButton_Quit, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Quit_click()));

    QObject::connect(ui.menuButton_F, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_F_click()));
    QObject::connect(ui.menuButton_B, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_B_click()));
    QObject::connect(ui.menuButton_U, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_U_click()));
    QObject::connect(ui.menuButton_D, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_D_click()));
    QObject::connect(ui.menuButton_L, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_L_click()));
    QObject::connect(ui.menuButton_R, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_R_click()));
    QObject::connect(ui.menuButton_xAxisR, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_xAxisR_click()));
    QObject::connect(ui.menuButton_yAxisR, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_yAxisR_click()));
    QObject::connect(ui.menuButton_zAxisR, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_zAxisR_click()));
    QObject::connect(ui.menuButton_xAxisL, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_xAxisL_click()));
    QObject::connect(ui.menuButton_yAxisL, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_yAxisL_click()));
    QObject::connect(ui.menuButton_zAxisL, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_zAxisL_click()));
    QObject::connect(ui.menuButton_CloseFilter, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_CloseFilter_click()));
    QObject::connect(ui.menuButton_Explosure, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Explosure_click()));
    QObject::connect(ui.menuButton_Integrated, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Integrated_click()));
    QObject::connect(ui.menuButton_Colorful, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Colorful_click()));
    QObject::connect(ui.menuButton_ColorfulX, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_ColorfulX_click()));
    QObject::connect(ui.menuButton_Rename, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Rename_click()));
    QObject::connect(ui.menuButton_Export, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Export_click()));
    QObject::connect(ui.menuButton_Save, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Save_click()));
    QObject::connect(ui.toolButton_Save, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Save_click()));
    QObject::connect(ui.menuButton_SaveAs, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_SaveAs_click()));
    QObject::connect(ui.menuButton_SaveAll, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_SaveAll_click()));
    QObject::connect(ui.menuButton_Sparse, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Sparse_click()));
    QObject::connect(ui.menuButton_Dense, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Dense_click()));
    QObject::connect(ui.toolButton_Function, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Function_click()));
    QObject::connect(ui.menuButton_Function, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Function_click()));
    QObject::connect(ui.toolButton_Color, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_Color_click()));
    QObject::connect(ui.menuButton_Color, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_Color_click()));
    QObject::connect(ui.toolButton_BoundFill, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_BoundFill_click()));
    QObject::connect(ui.menuButton_BoundFill, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_BoundFill_click()));
    QObject::connect(ui.toolButton_DotFill, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_DotFill_click()));
    QObject::connect(ui.menuButton_DotFill, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_DotFill_click()));

    QObject::connect(ui.lightOption_diffuse, SIGNAL(triggered()), WidgetsEventsClass, SLOT(lightOption_diffuse_click()));
    QObject::connect(ui.lightOption_Specular, SIGNAL(triggered()), WidgetsEventsClass, SLOT(lightOption_Specular_click()));

    QObject::connect(ui.lightOption_Positional, SIGNAL(triggered()), WidgetsEventsClass, SLOT(lightOption_Positional_click()));
    QObject::connect(ui.lightOption_Directional, SIGNAL(triggered()), WidgetsEventsClass, SLOT(lightOption_Directional_click()));

    QObject::connect(ui.toolButton_LoadTexture, SIGNAL(triggered()), WidgetsEventsClass, SLOT(toolButton_LoadTexture_click()));
    QObject::connect(ui.menuButton_LoadTexture, SIGNAL(triggered()), WidgetsEventsClass, SLOT(menuButton_LoadTexture_click()));
    
}

// 将可选中工具栏按钮的可选状态激活
void GraphicsatZJU::toolButtons_setCheckable()
{
    ui.toolButton_Points->setCheckable(true);
    ui.toolButton_Wireframe->setCheckable(true);
    ui.toolButton_FlatLine->setCheckable(true);
    ui.toolButton_FlatFill->setCheckable(true);
    ui.toolButton_SmoothFill->setCheckable(true);
    ui.toolButton_LightSource->setCheckable(true);
    ui.toolButton_BoundingBox->setCheckable(true);
    ui.toolButton_CenterPoint->setCheckable(true);
    ui.toolButton_Coordinate->setCheckable(true);
    ui.toolButton_GradientBkg->setCheckable(true);
    ui.toolButton_MeshBkg->setCheckable(true);
    ui.toolButton_EmptyBkg->setCheckable(true);
    ui.toolButton_Perspective->setCheckable(true);
    ui.toolButton_Orthogonality->setCheckable(true);
    ui.toolButton_StatusMonitor->setCheckable(true);
    ui.toolButton_BoundFill->setCheckable(true);
    ui.toolButton_DotFill->setCheckable(true);
}

// 将可选中菜单按钮的可选状态激活
void GraphicsatZJU::menuButtons_setCheckable()
{
    ui.menuButton_Points->setCheckable(true);
    ui.menuButton_Wireframe->setCheckable(true);
    ui.menuButton_FlatLine->setCheckable(true);
    ui.menuButton_FlatFill->setCheckable(true);
    ui.menuButton_SmoothFill->setCheckable(true);
    ui.menuButton_LightSource->setCheckable(true);
    ui.menuButton_BoundingBox->setCheckable(true);
    ui.menuButton_CenterPoint->setCheckable(true);
    ui.menuButton_Coordinate->setCheckable(true);
    ui.menuButton_GradientBkg->setCheckable(true);
    ui.menuButton_MeshBkg->setCheckable(true);
    ui.menuButton_EmptyBkg->setCheckable(true);
    ui.menuButton_Perspective->setCheckable(true);
    ui.menuButton_Orthogonality->setCheckable(true);
    ui.menuButton_StatusMonitor->setCheckable(true);
    ui.menuButton_Explosure->setCheckable(true);
    ui.menuButton_Integrated->setCheckable(true);
    ui.menuButton_Colorful->setCheckable(true);
    ui.menuButton_ColorfulX->setCheckable(true);
    ui.menuButton_BoundFill->setCheckable(true);
    ui.menuButton_DotFill->setCheckable(true);
    ui.lightOption_diffuse->setCheckable(true);
    ui.lightOption_Specular->setCheckable(true);
    ui.lightOption_Directional->setCheckable(true);
    ui.lightOption_Positional->setCheckable(true);
}

// 主窗体的关闭事件
void GraphicsatZJU::closeEvent(QCloseEvent* event)
{
    QApplication::beep();   // 发出'\a'响铃提示
    if (QMessageBox::warning(this, translateChinese->toUnicode("退出"), translateChinese->toUnicode("你确定要退出求是图形吗?"), translateChinese->toUnicode("确定"), translateChinese->toUnicode("取消")) == 0)
    {
        // 先循环关闭所有的窗体
        for (int i = WidgetsEvents::menuButton_gDocsList.size() - 1; i >= 0; i--)
            WidgetsEvents::menuButton_to_SubWindow.find(WidgetsEvents::menuButton_gDocsList[i])->second->close();
        QApplication::quit();
    }
    else
        event->ignore();    // 否则忽略关闭事件
}

// 主窗体的大小改变事件
void GraphicsatZJU::resizeEvent(QResizeEvent* event) 
{
    QWidget::resizeEvent(event);
    originButtons->setGeometry(width() - originButtons->width() - 1 - borderDistance, 1 + borderDistance, originButtons->width(), originButtons->height());
    closeButton->setGeometry(width() - closeButton->width() - 1 - borderDistance, 1 + borderDistance, closeButton->width(), closeButton->height());
    maximizeButton->setGeometry(width() - maximizeButton->width() - 51 - borderDistance, 1 + borderDistance, maximizeButton->width(), maximizeButton->height());
    minimizeButton->setGeometry(width() - minimizeButton->width() - 101 - borderDistance, 1 + borderDistance, minimizeButton->width(), minimizeButton->height());    
        
    GraphicsDocument* currentDoc = getCurrentDoc();
    if (currentDoc != nullptr)
    {
        currentDoc->correspondingSubWindow->setGeometry(0, 0, this->geometry().width() - 2 - borderDistance * 2, this->geometry().height() - ui.toolBar->height() - ui.menuBar->height() - ui.statusBar->height() - borderDistance * 2);
        currentDoc->setGeometry(0, 0, this->geometry().width() - 2 - borderDistance * 2, this->geometry().height() - ui.toolBar->height() - ui.menuBar->height() - ui.statusBar->height() - borderDistance * 2);
    }

    // 将mdiArea设置为填充满整个工作区域
    ui.mdiArea->resize(this->geometry().width() - 2 - borderDistance * 2, this->geometry().height() - ui.toolBar->height() - ui.menuBar->height() - ui.statusBar->height() - borderDistance * 2);

    //// 如果mdiArea已经有子窗体，这里通过检测是否有对应的菜单栏按钮来实现
    //if (WidgetsEvents::menuButton_gDocsList.size() != 0) 
    //    for (auto menuButton : WidgetsEvents::menuButton_gDocsList) 
    //    {
    //        // 如果有菜单栏按钮，就找到选中的一个，然后将对应的子窗体设置为填充满mdiArea
    //        if (menuButton->isChecked()) 
    //        {
    //            QMdiSubWindow* subWindow = (QMdiSubWindow*)WidgetsEvents::menuButton_to_SubWindow.find(menuButton)->second;
    //            if (subWindow->isMaximized())
    //                subWindow->resize(ui.mdiArea->geometry().width(), ui.mdiArea->geometry().height());
    //            break;
    //        }
    //    }
    // 将GLInfo始终设置在左下角
    correspondingGLInfo->setGeometry(20, height() - 40 - correspondingGLInfo->height(), correspondingGLInfo->width(), correspondingGLInfo->height());
}

// 主窗体的键盘按键按下事件
void GraphicsatZJU::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Control:
            emit ctrlPressed();
            break;
        case Qt::Key_Alt:
            emit altPressed();
            break;
        case Qt::Key_Shift:
            emit shiftPressed();
            break;
    }
}

// 主窗体的键盘按键释放事件
void GraphicsatZJU::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Control:
            emit ctrlReleased();
            break;
        case Qt::Key_Alt:
            emit altReleased();
            break;
        case Qt::Key_Shift:
            emit shiftReleased();
            break;
    }
}

// 获取当前聚焦图形文档窗口的对应按钮
QAction* GraphicsatZJU::getCurrentMenuButton()
{
    if (WidgetsEvents::menuButton_gDocsList.size() == 0)
        return nullptr;
    else
        for (auto menuButton : WidgetsEvents::menuButton_gDocsList)
            if (menuButton->isChecked())
                return menuButton;
}

// 获取当前聚焦图形文档窗口
GraphicsDocument* GraphicsatZJU::getCurrentDoc()
{
    QAction* menuButton = getCurrentMenuButton();
    if (menuButton == nullptr)
        return nullptr;
    else
        return (GraphicsDocument*)WidgetsEvents::menuButton_to_SubWindow.find(menuButton)->second;
}

GraphicsatZJU::~GraphicsatZJU()
{
    delete statusBarIcon;
    delete statusBarLabel;
    delete comboBox;
    delete originButtons;
    delete closeButton;
    delete maximizeButton;
    delete minimizeButton;
    delete mainFormIcon;
    delete currentDocument;
    delete closeButtonHover;
    delete closeButtonPressed;
    delete maximizeButtonHover;
    delete maximizeButtonPressed;
    delete minimizeButtonHover;
    delete minimizeButtonPressed;
    delete mainFormIconActive;
    delete innerCloseButton;
    delete innerCloseButtonHover;
    delete innerMaximizeButton;
    delete innerMaximizeButtonHover;
}