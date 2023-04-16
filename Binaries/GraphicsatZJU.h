#ifndef GRAPHICSATZJU_H
#define GRAPHICSATZJU_H

#include "ui_GraphicsatZJU.h"
#include "WidgetsEvents.h"
#include <QtWidgets/QMainWindow>

class GraphicsatZJU : public QMainWindow {
    Q_OBJECT

public:
    GLInfo* correspondingGLInfo;
    QLabel* statusBarIcon = nullptr;
    QLabel* statusBarLabel = nullptr;
    QComboBox* comboBox = nullptr;

    QLabel* originButtons = nullptr;
    QLabel* closeButton = nullptr;
    QLabel* maximizeButton = nullptr;
    QLabel* minimizeButton = nullptr;
    QLabel* mainFormIcon = nullptr;
    QLabel* currentDocument = nullptr;

    QPixmap* closeButtonHover = nullptr;
    QPixmap* closeButtonPressed = nullptr;
    QPixmap* maximizeButtonHover = nullptr;
    QPixmap* maximizeButtonPressed = nullptr;
    QPixmap* minimizeButtonHover = nullptr;
    QPixmap* minimizeButtonPressed = nullptr;
    QPixmap* mainFormIconActive = nullptr;
    QPixmap* innerCloseButton = nullptr;
    QPixmap* innerCloseButtonHover = nullptr;
    QPixmap* innerMaximizeButton = nullptr;
    QPixmap* innerMaximizeButtonHover = nullptr;

    int borderDistance = 0;

    GraphicsatZJU(QWidget* parent = Q_NULLPTR);
    ~GraphicsatZJU();
    Ui::GraphicsatZJUClass ui;
    GraphicsDocument* getCurrentDoc();
    QAction* getCurrentMenuButton();

    void toolButtons_setCheckable();
    void menuButtons_setCheckable();

    void resizePaint();
    void shadowPaint();

signals:
    void ctrlPressed();
    void altPressed();
    void ctrlReleased();
    void altReleased();
    void shiftPressed();
    void shiftReleased();

protected:
    // 重载的主窗体的关闭事件
    virtual void GraphicsatZJU::closeEvent(QCloseEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    enum Direction
    {
        UP = 0,
        DOWN = 1,
        LEFT,
        RIGHT,
        LEFTTOP,
        LEFTBOTTOM,
        RIGHTBOTTOM,
        RIGHTTOP,
        NONE
    };

    // 两组标记分别是用于鼠标拖动移动和鼠标拖动缩放的，区分使用
    bool isMouseDragging = false;
    QPoint mouseDragPosition = QPoint(0, 0);
    bool isLeftPressDown = false;
    QPoint dragPosition;
    Direction dragStatus = NONE;

    void region(const QPoint& cursorGlobalPoint);

private slots:
};

// 全局事件链接器
void WidgetsEvents_connect(Ui::GraphicsatZJUClass ui);

#endif /* GRAPHICSATZJU_H */ 
