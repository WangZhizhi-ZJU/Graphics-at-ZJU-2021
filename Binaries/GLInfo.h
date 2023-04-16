#ifndef GLINFO_H
#define GLINFO_H

#include <QWidget>
#include <QTimer>
#include "ui_GLInfo.h"
#include "GLArea.h"
#include <QPixmap>

class GLArea;

class GLInfo : public QWidget
{
	Q_OBJECT

public:
    QPixmap* noDocument = new QPixmap(":/GraphicsatZJU/Resources/ToolbarIcons/0x075.png", 0, Qt::AutoColor);
    QPixmap* edited = new QPixmap(":/GraphicsatZJU/Resources/ToolbarIcons/0x086.png", 0, Qt::AutoColor);
    QPixmap* saved = new QPixmap(":/GraphicsatZJU/Resources/ToolbarIcons/0x085.png", 0, Qt::AutoColor);

    QTimer* statusChecker;
    GLArea* correspondingArea = nullptr;
	GLInfo(QWidget *parent = Q_NULLPTR, QRect fatherWndGeometry = QRect(0, 0, 640, 480));
	~GLInfo(); 

private slots:
    void updateGLInfo();

private:
	Ui::GLInfo ui;
};

#endif