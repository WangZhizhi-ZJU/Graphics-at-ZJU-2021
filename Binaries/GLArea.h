#ifndef GLAREA_H
#define GLAREA_H

#include "ui_GLArea.h"
#include "QtOpenGL/qgl.h"
#include "MeshModel.h"
#include <QMessageBox>

#define     BKG_EMPTY       0x000           // 纯黑色背景
#define     BKG_GRADIENT    0x001           // 渐变色背景
#define     BKG_MESH        0x002           // 网格状背景

#define     PROJ_ORTHO      0x011           // 正交投影法
#define     PROJ_PERSP      0x012           // 透视投影法

#define     DISP_POINTS     0x020           // 点阵模式
#define     DISP_WIREFRAME  0x021           // 线框模式
#define     DISP_FLATLINES  0x022           // 线框填充模式
#define     DISP_FLATFILL   0x023           // 面填充模式
#define     DISP_SMOOTHFILL 0x024           // 平滑填充模式
#define     DISP_BOUNDFILL  0x025           // 线面叠加填充模式
#define     DISP_DOTFILL    0x026           // 点面叠加填充模式

#define     FLTR_EXPOSURE   0x030           // 曝光模式
#define     FLTR_INTEGRAT   0x031           // 融合模式
#define     FLTR_COLORFUL   0x032           // 彩色模式
#define     FLTR_CLFIGENV   0x033           // 屏蔽环境光彩色模式

#define     FLTR_NONE       0x03F           // 无滤镜

class GLArea : public QGLWidget
{
	Q_OBJECT

public:
    // 文档属性
    GLint parentWidth;                      // 初始化时容器窗体的宽
    GLint parentHeight;                     // 初始化时容器窗体的高
    MeshModel* meshModel = nullptr;         // 加载的模型

    QLabel* closeButton = nullptr;
    QLabel* maximizeButton = nullptr;
    QPixmap* closeButtonPixel = nullptr;
    QPixmap* maximizeButtonPixel = nullptr;

    int projectionMethod = PROJ_ORTHO;      // 投影方法

    int renderMode = GL_FILL;               // 渲染模式，参考值: GL_FILL、GL_LINE、GL_POINT
    int renderMethod = GL_POLYGON;          // 渲染方法，参考值: GL_POLYGON、GL_QUADS、GL_TRIANGLES、GL_LINE_LOOP、GL_POINTS等
    int surfaceMode = GL_SMOOTH;            // 表面模式，参考值: GL_SMOOTH、GL_FLAT
    int backgroundMode = BKG_GRADIENT;      // 背景图形
    int displayMode = DISP_BOUNDFILL;       // 显示方式
    int filterMode = FLTR_NONE;             // 滤镜模式

    bool isShowAnchor = true;               // 是否显示锚点
    bool isShowCoordinate = true;           // 是否显示坐标系
    bool isLightEnabled = true;             // 是否激活光照
    bool isShowBoundingBox = false;         // 是否显示包围盒
    bool isShowMonitor = true;              // 是否显示状态监视器
    bool* isEdited_delegator = nullptr;

    int isDiffuseAccepted = 0;
    int isSpecularAccepted = 1;
    int isDirectionalLight = 1;

    struct color                            // 模型绘制颜色
    {
        float red;
        float green;
        float blue;
        bool isRainbow;
    } meshColor = {0.75f, 0.75f, 0.75f, false},
      lightColor = {0.125f, 0.125f, 0.125f, false};

    // 模型属性
    GLfloat rotateAngle_x = 0.0f;           // x方向的镜头旋转角
    GLfloat rotateAngle_y = 0.0f;           // y方向的镜头旋转角
    GLfloat rotateAngle_z = 0.0f;           // z方向的镜头旋转角
    GLfloat zoomRatio = 1.00f;              // 镜头放大率
    GLfloat naturalMag = 5;                 // 固有放大率
    GLfloat moveX = 0.0f;                   // 模型锚点围绕原点的x偏移
    GLfloat moveY = 0.0f;                   // 模型锚点围绕原点的y偏移
    GLfloat moveZ = 0.0f;                   // 模型锚点围绕原点的z偏移

    // 控件参数
    int lastMousePos_x = -1;                // 上一次鼠标位置x
    int lastMousePos_y = -1;                // 上一次鼠标位置y
    bool isMouseButtonLeftDown = false;     // 鼠标左键是否被按下
    bool isMouseButtonRightDown = false;    // 鼠标右键是否被按下
    bool isCtrlDown = false;                // Ctrl键是否被按下 
    bool isAltDown = false;                 // Alt键是否被按下
    bool isShiftDown = false;               // Shift键是否被按下
    
    // 接口函数
    bool loadObjMesh(const char* meshModelAddress); // 从外部地址接入模型
    void resetOverall()
    {
        moveX = 0.0f;
        moveY = 0.0f;
        moveZ = 0.0f;
        rotateAngle_x = 0.0f;
        rotateAngle_y = 0.0f;
        rotateAngle_z = 0.0f;
        zoomRatio = 1.0f;
        naturalMag = 5.0f;
        if (meshModel->meshModelPath.endsWith(".obj"))
            loadObjMesh(meshModel->meshModelPath.toStdString().c_str());
        *isEdited_delegator = true;
    }

    void setRenderMode(int render)
    {
        renderMode = render;
        *isEdited_delegator = true;
    }

    void setRenderMethod(int method)
    {
        renderMethod = method;
        *isEdited_delegator = true;
    }
    
    void setSurfaceMode(int surface)
    {
        surfaceMode = surface;
        *isEdited_delegator = true;
    }

    void updateGLArea()  // 用于外部刷新绘图区域
    {
        updateGL();
    }

    void resizeGLArea()
    {
        resizeGL(this->width(), this->height());
    }

    void switchLightEnable()
    {
        isLightEnabled = !isLightEnabled;
        setLightResource();
        setupRC();
        *isEdited_delegator = true;
    }

    void updateLightResource()
    {
        setLightResource();
        setupRC();
    }

    void switchAnchorEnable()
    {
        isShowAnchor = !isShowAnchor;
        *isEdited_delegator = true;
    }

    void switchCoordinateEnable()
    {
        isShowCoordinate = !isShowCoordinate;
        *isEdited_delegator = true;
    }

    void switchBoundingBoxEnable()
    {
        isShowBoundingBox = !isShowBoundingBox;
        *isEdited_delegator = true;
    }

    void switchStatusMonitorEnable()
    {
        isShowMonitor = !isShowMonitor;
        *isEdited_delegator = true;
    }

    void openOuterLines(bool on = true)
    {
        if (on)
            glEnable(GL_POLYGON_SMOOTH);
        else
            glDisable(GL_POLYGON_SMOOTH);
    }

    // 切换点光源/平行光源位置时要注意先将视角旋转回原点再改变光照设置
    void switchLightDirection(int param) 
    {
        isDirectionalLight = param;
        glLoadIdentity();
        if (projectionMethod == PROJ_ORTHO)
        {
            glRotatef(0, 1.0, 0.0, 0.0);
            glRotatef(0, 0.0, 1.0, 0.0);
            glRotatef(0, 0.0, 0.0, 1.0);
            glTranslated(moveX, moveY, moveZ);
        }
        else if (projectionMethod == PROJ_PERSP)
        {
            glTranslated(moveX, moveY, -2.0f + moveZ);
            glRotatef(0, 1.0, 0.0, 0.0);
            glRotatef(0, 0.0, 1.0, 0.0);
            glRotatef(0, 0.0, 0.0, 1.0);
            glTranslated(moveX, moveY, moveZ);
        }
        if (isDirectionalLight) 
        {
            GLfloat lightPosition[] = {1.f, 1.f, 9.f, 0.0f};
            glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        }
        else
        {
            GLfloat lightPosition[] = {1.f, 1.f, 9.f, 1.0f};
            glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        }
        if (projectionMethod == PROJ_ORTHO)
        {
            glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
            glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
            glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
            glTranslated(moveX, moveY, moveZ);
        }
        else if (projectionMethod == PROJ_PERSP)
        {
            glTranslated(moveX, moveY, -2.0f + moveZ);
            glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
            glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
            glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
            glTranslated(moveX, moveY, moveZ);
        }
        glEnd();
    }

    void drawBoundingAndFilledMesh();
    void drawDotAndFilledMesh();

	GLArea(QWidget* parent,
           int width,
           int height,
           bool* isEdited,
           QLabel* close,
           QLabel* maxim,
           QPixmap* closePix,
           QPixmap* maximPix,
           int render = GL_FILL,
           int surface = GL_POLYGON,
           int method = GL_SMOOTH);
	~GLArea();

    void updateGraphicMode(int render, int surface, int method = GL_POLYGON);
    void drawBackground(int mode = BKG_GRADIENT, int width = 640, int height = 640);

    void drawRectangularPyramid(float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
    void drawCube(float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
    void drawCylinder(float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
    void drawCircle();
    void drawBall(float offsetX, float offsetY, float offsetZ, float radius, float piecesX, float piecesY);

    void drawAnchor();                      // 绘制锚点
    void drawCoordinate();                  // 绘制坐标系
    void drawBoundingBox();                 // 绘制包围盒

    void drawMeshModel(float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void setLightResource();
    void setupRC();

    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

private slots:
    void setCtrlPressed() { isCtrlDown = true; }
    void setCtrlReleased() { isCtrlDown = false; }
    void setAltPressed() { isAltDown = true; }
    void setAltReleased() { isAltDown = false; }
    void setShiftPressed() { isShiftDown = true; }
    void setShiftReleased() { isShiftDown = false; }

private:
	Ui::GLArea ui;
    void drawBallEntry(float radius, float densityX, float densityY);
};

#endif