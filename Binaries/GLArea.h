#ifndef GLAREA_H
#define GLAREA_H

#include "ui_GLArea.h"
#include "QtOpenGL/qgl.h"
#include "MeshModel.h"
#include <QMessageBox>

#define     BKG_EMPTY       0x000           // ����ɫ����
#define     BKG_GRADIENT    0x001           // ����ɫ����
#define     BKG_MESH        0x002           // ����״����

#define     PROJ_ORTHO      0x011           // ����ͶӰ��
#define     PROJ_PERSP      0x012           // ͸��ͶӰ��

#define     DISP_POINTS     0x020           // ����ģʽ
#define     DISP_WIREFRAME  0x021           // �߿�ģʽ
#define     DISP_FLATLINES  0x022           // �߿����ģʽ
#define     DISP_FLATFILL   0x023           // �����ģʽ
#define     DISP_SMOOTHFILL 0x024           // ƽ�����ģʽ
#define     DISP_BOUNDFILL  0x025           // ����������ģʽ
#define     DISP_DOTFILL    0x026           // ����������ģʽ

#define     FLTR_EXPOSURE   0x030           // �ع�ģʽ
#define     FLTR_INTEGRAT   0x031           // �ں�ģʽ
#define     FLTR_COLORFUL   0x032           // ��ɫģʽ
#define     FLTR_CLFIGENV   0x033           // ���λ������ɫģʽ

#define     FLTR_NONE       0x03F           // ���˾�

class GLArea : public QGLWidget
{
	Q_OBJECT

public:
    // �ĵ�����
    GLint parentWidth;                      // ��ʼ��ʱ��������Ŀ�
    GLint parentHeight;                     // ��ʼ��ʱ��������ĸ�
    MeshModel* meshModel = nullptr;         // ���ص�ģ��

    QLabel* closeButton = nullptr;
    QLabel* maximizeButton = nullptr;
    QPixmap* closeButtonPixel = nullptr;
    QPixmap* maximizeButtonPixel = nullptr;

    int projectionMethod = PROJ_ORTHO;      // ͶӰ����

    int renderMode = GL_FILL;               // ��Ⱦģʽ���ο�ֵ: GL_FILL��GL_LINE��GL_POINT
    int renderMethod = GL_POLYGON;          // ��Ⱦ�������ο�ֵ: GL_POLYGON��GL_QUADS��GL_TRIANGLES��GL_LINE_LOOP��GL_POINTS��
    int surfaceMode = GL_SMOOTH;            // ����ģʽ���ο�ֵ: GL_SMOOTH��GL_FLAT
    int backgroundMode = BKG_GRADIENT;      // ����ͼ��
    int displayMode = DISP_BOUNDFILL;       // ��ʾ��ʽ
    int filterMode = FLTR_NONE;             // �˾�ģʽ

    bool isShowAnchor = true;               // �Ƿ���ʾê��
    bool isShowCoordinate = true;           // �Ƿ���ʾ����ϵ
    bool isLightEnabled = true;             // �Ƿ񼤻����
    bool isShowBoundingBox = false;         // �Ƿ���ʾ��Χ��
    bool isShowMonitor = true;              // �Ƿ���ʾ״̬������
    bool* isEdited_delegator = nullptr;

    int isDiffuseAccepted = 0;
    int isSpecularAccepted = 1;
    int isDirectionalLight = 1;

    struct color                            // ģ�ͻ�����ɫ
    {
        float red;
        float green;
        float blue;
        bool isRainbow;
    } meshColor = {0.75f, 0.75f, 0.75f, false},
      lightColor = {0.125f, 0.125f, 0.125f, false};

    // ģ������
    GLfloat rotateAngle_x = 0.0f;           // x����ľ�ͷ��ת��
    GLfloat rotateAngle_y = 0.0f;           // y����ľ�ͷ��ת��
    GLfloat rotateAngle_z = 0.0f;           // z����ľ�ͷ��ת��
    GLfloat zoomRatio = 1.00f;              // ��ͷ�Ŵ���
    GLfloat naturalMag = 5;                 // ���зŴ���
    GLfloat moveX = 0.0f;                   // ģ��ê��Χ��ԭ���xƫ��
    GLfloat moveY = 0.0f;                   // ģ��ê��Χ��ԭ���yƫ��
    GLfloat moveZ = 0.0f;                   // ģ��ê��Χ��ԭ���zƫ��

    // �ؼ�����
    int lastMousePos_x = -1;                // ��һ�����λ��x
    int lastMousePos_y = -1;                // ��һ�����λ��y
    bool isMouseButtonLeftDown = false;     // �������Ƿ񱻰���
    bool isMouseButtonRightDown = false;    // ����Ҽ��Ƿ񱻰���
    bool isCtrlDown = false;                // Ctrl���Ƿ񱻰��� 
    bool isAltDown = false;                 // Alt���Ƿ񱻰���
    bool isShiftDown = false;               // Shift���Ƿ񱻰���
    
    // �ӿں���
    bool loadObjMesh(const char* meshModelAddress); // ���ⲿ��ַ����ģ��
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

    void updateGLArea()  // �����ⲿˢ�»�ͼ����
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

    // �л����Դ/ƽ�й�Դλ��ʱҪע���Ƚ��ӽ���ת��ԭ���ٸı��������
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

    void drawAnchor();                      // ����ê��
    void drawCoordinate();                  // ��������ϵ
    void drawBoundingBox();                 // ���ư�Χ��

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