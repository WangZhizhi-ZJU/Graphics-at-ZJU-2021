#include "GLArea.h"
#include <QtGui>
#include <QtCore>
#include <QtOpenGL/QtOpenGL>
#include <GL/GLU.h>

// 中文Unicode码翻译器
QTextCodec* translator = QTextCodec::codecForName("GBK");
#define ch(args) translator->toUnicode((args))

GLArea::GLArea(QWidget* parent, int width, int height, bool* isEdited, QLabel* close, QLabel* maxim, QPixmap* closePix, QPixmap* maximPix, int render, int surface, int method)
    : QGLWidget(parent), closeButton(close), maximizeButton(maxim), closeButtonPixel(closePix), maximizeButtonPixel(maximPix)
{
	ui.setupUi(this);
    this->setParent(parent);
    parentWidth = width;
    parentHeight = height;
    setCursor(Qt::OpenHandCursor);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    renderMode = render;
    surfaceMode = surface;
    this->setVisible(false);
    isEdited_delegator = isEdited;
    setMouseTracking(true);
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::StereoBuffers));
    hide();
}

void GLArea::setLightResource()
{
    if (isLightEnabled && isSpecularAccepted)
    {
        GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f /* 镜面开关 */};
        glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight);
        float specularMat[] = {0.4f, 0.4f, 0.4f, 1.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR, specularMat);
        glMateriali(GL_FRONT, GL_SHININESS, 128);
        glEnable(GL_LIGHT1);
    }
    else
        glDisable(GL_LIGHT1);
    if (isLightEnabled && isDiffuseAccepted)
    {
        GLfloat diffuseLight[] = {204.0f / 255.0f, 204.0f / 255.0f, 204.0f / 255.0f, 1.0f /* 反射开关 */};
        glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight);
        float diffuseMat[] = {0.3f, 0.3f, 0.3f, 1.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR, diffuseMat);
        glEnable(GL_LIGHT2);
    }
    else
        glDisable(GL_LIGHT2);
    if (isLightEnabled)
    {
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }
}

void GLArea::setupRC()
{
    GLfloat ambientLight[] = { lightColor.red, lightColor.green, lightColor.blue, 1.0f };
    if (isLightEnabled)
    {
        glEnable(GL_DEPTH_TEST);
        /*if (meshModel != nullptr && meshModel->textures != 0) 
        {
            ambientLight[0] = 32.0f / 255.0f;
            ambientLight[1] = 32.0f / 255.0f;
            ambientLight[2] = 32.0f / 255.0f;
        }*/
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
        ambientLight[0] = 32.0f / 255.0f;
        ambientLight[1] = 32.0f / 255.0f;
        ambientLight[2] = 32.0f / 255.0f;
    }
    glFrontFace(GL_CCW);
    if (isLightEnabled)
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
    }
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    if (isLightEnabled)
        glEnable(GL_COLOR_MATERIAL);
    else
        glDisable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void GLArea::updateGraphicMode(int render, int surface, int method)
{
    this->renderMode = render;
    this->surfaceMode = surface;
    switch (renderMode) {
        case GL_FILL: renderMethod = GL_POLYGON; break;
        case GL_LINE: renderMethod = GL_LINE_LOOP; break;
        case GL_POINT: renderMethod = GL_POINTS; break;
    }
    updateGL();
}

void GLArea::initializeGL() {
    setGeometry(0, 0, parentWidth, parentHeight);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT, renderMode);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    GLfloat lightPosition[] = {1.0f, 1.0f, 9.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    setLightResource();
    setupRC();
}

#define min(x, y) ((x) < (y) ? (x) : (y))

void GLArea::drawBackground(int mode, int width, int height) {
    if (backgroundMode == BKG_EMPTY)
    {
        // 不需要绘制背景
    }
    else if (backgroundMode == BKG_GRADIENT)
    {
        if (isLightEnabled == false)
        {
            float colorR = 0.0f;
            float colorG = 0.0f;
            float colorB = 0.0f;
            float paintDensity = 0.005f / zoomRatio;
            float paintDomainX = 1.0f / zoomRatio;
            float paintDomainY = 1.0f / zoomRatio;
            if (width > height)
                paintDomainX = (float)width / (float)height / zoomRatio;
            else
                paintDomainY = (float)height / (float)width / zoomRatio;
            glClear(GL_COLOR_BUFFER_BIT);
            glLoadIdentity();

            if (projectionMethod == PROJ_ORTHO)
                glTranslatef(0.0f, 0.0f, -5.0f);
            else if (projectionMethod == PROJ_PERSP)
                glTranslatef(0.0f, 0.0f, 1.0f);
            glPointSize(5.0f);
            glBegin(GL_POINTS);
            for (float coordY = -paintDomainY; coordY <= paintDomainY; coordY += paintDensity)
            {
                colorR = min(1.0f, colorR + 60 * paintDensity / 255 * zoomRatio);
                colorG = min(1.0f, colorG + 60 * paintDensity / 255 * zoomRatio);
                colorB = min(1.0f, colorB + 120 * paintDensity / 255 * zoomRatio);
                glColor3f(colorR, colorG, colorB);
                for (float coordX = -paintDomainX; coordX <= paintDomainX; coordX += paintDensity)
                    glVertex2f(coordX, coordY);
            }
            glEnd();
        }
        else
        {
            float colorR = 0.0f;
            float colorG = 0.0f;
            float colorB = 0.0f;
            float paintDensity = 0.005f / zoomRatio;
            float paintDomainX = 1.0f / zoomRatio;
            float paintDomainY = 1.0f / zoomRatio;
            if (width > height)
                paintDomainX = (float)width / (float)height / zoomRatio;
            else
                paintDomainY = (float)height / (float)width / zoomRatio;
            glClear(GL_COLOR_BUFFER_BIT);
            glLoadIdentity();

            if (projectionMethod == PROJ_ORTHO)
                glTranslatef(0.0f, 0.0f, -5.0f);
            else if (projectionMethod == PROJ_PERSP)
                glTranslatef(0.0f, 0.0f, 1.0f);
            glPointSize(5.0f);
            glBegin(GL_POINTS);
            for (float coordY = -paintDomainY; coordY <= paintDomainY; coordY += paintDensity)
            {
                colorR = min(1.0f, colorR + 127 * paintDensity / 255 * zoomRatio);
                colorG = min(1.0f, colorG + 127 * paintDensity / 255 * zoomRatio);
                colorB = min(1.0f, colorB + 127 * paintDensity / 255 * zoomRatio);
                glColor3f(colorR, colorG, colorB);
                for (float coordX = -paintDomainX; coordX <= paintDomainX; coordX += paintDensity)
                    glVertex2f(coordX, coordY);
            }
            glEnd();
        }
    }
    else if (backgroundMode == BKG_MESH)
    {
        glDisable(GL_LIGHTING);
        float paintDensity = 0.005f / zoomRatio;
        float paintDomainX = 1.0f / zoomRatio;
        float paintDomainY = 1.0f / zoomRatio;
        if (width > height)
            paintDomainX = (float)width / (float)height / zoomRatio;
        else
            paintDomainY = (float)height / (float)width / zoomRatio;
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();
        glPointSize(4.0f);
        if (projectionMethod == PROJ_ORTHO)
            glTranslatef(0.0f, 0.0f, -5.0f);
        else if (projectionMethod == PROJ_PERSP)
            glTranslatef(0.0f, 0.0f, 1.0f);

        glBegin(GL_POINTS);
        for (float coordY = -paintDomainY; coordY <= paintDomainY; coordY += paintDensity)
        {
            glColor3f(0.933f, 0.933f, 0.95f);
            for (float coordX = -paintDomainX; coordX <= paintDomainX; coordX += paintDensity)
                glVertex2f(coordX, coordY);
        }
        glEnd();

        glBegin(GL_LINES);
        for (float coordY = -paintDomainY; coordY <= paintDomainY; coordY += paintDensity * 10)
        {
            glColor3f(0.75f, 0.75f, 0.75f);
            for (float coordX = -paintDomainX; coordX <= paintDomainX; coordX += paintDensity * 10)
            {
                glVertex2f(coordX, coordY);
                glVertex2f(coordX, coordY + paintDensity * 10);
            }
        }
        glEnd();

        glBegin(GL_LINES);
        for (float coordY = -paintDomainY; coordY <= paintDomainY; coordY += paintDensity * 10)
        {
            glColor3f(0.75f, 0.75f, 0.75f);
            for (float coordX = -paintDomainX; coordX <= paintDomainX; coordX += paintDensity * 10)
            {
                glVertex2f(coordX, coordY);
                glVertex2f(coordX + paintDensity * 10, coordY);
            }
        }
        glEnd();

        if (isLightEnabled)
            glEnable(GL_LIGHTING);
    }
}

void GLArea::drawRectangularPyramid(float offsetX, float offsetY, float offsetZ) {
    glLoadIdentity();
    glTranslated(offsetX, offsetY, offsetZ);
    glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
    glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
    glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
    glBegin(renderMethod);
    /*前正面的绘制*/
    glColor3f(1.0, 0.0, 0.0);  //上顶点红色
    glVertex3f(0.0, 0.3, 0.0);
    glColor3f(0.0, 0.0, 1.0);  //左下点蓝色
    glVertex3f(-0.3, -0.3, 0.3);
    glColor3f(0.0, 1.0, 0.0);  //右下角绿色
    glVertex3f(0.3, -0.3, 0.3);
    /*右侧面的绘制*/
    glColor3f(1.0, 0.0, 0.0);  //上顶点红色
    glVertex3f(0.0, 0.3, 0.0);
    glColor3f(0.0, 0.0, 1.0);  //左下点蓝色
    glVertex3f(0.3, -0.3, 0.3);
    glColor3f(0.0, 1.0, 0.0);  //右下角绿色
    glVertex3f(0.3, -0.3, -0.3);
    /*后侧面的绘制*/
    glColor3f(1.0, 0.0, 0.0);  //上顶点红色
    glVertex3f(0.0, 0.3, 0.0);
    glColor3f(0.0, 0.0, 1.0);  //左下点蓝色
    glVertex3f(0.3, -0.3, -0.3);
    glColor3f(0.0, 1.0, 0.0);  //右下角绿色
    glVertex3f(-0.3, -0.3, -0.3);
    /*左侧面的绘制*/
    glColor3f(1.0, 0.0, 0.0);  //上顶点红色
    glVertex3f(0.0, 0.3, 0.0);
    glColor3f(0.0, 0.0, 1.0);  //左下点蓝色
    glVertex3f(-0.3, -0.3, -0.3);
    glColor3f(0.0, 1.0, 0.0);  //右下角绿色
    glVertex3f(-0.3, -0.3, 0.3);
    glEnd();
    /*底面四边形的绘制,使四棱锥封闭起来*/
    glBegin(renderMethod);
    glColor3f(0.0, 0.0, 1.0);  //上顶点红色
    glVertex3f(-0.3, -0.3, 0.3);
    glColor3f(0.0, 1.0, 0.0);  //左下点蓝色
    glVertex3f(0.3, -0.3, 0.3);
    glColor3f(0.0, 0.0, 1.0);  //右下角绿色
    glVertex3f(0.3, -0.3, -0.3);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-0.3, -0.3, -0.3);
    glEnd();
}
void GLArea::drawCircle() {}
void GLArea::drawCylinder(float offsetX, float offsetY, float offsetZ) {}
void GLArea::drawBallEntry(float radius, float densityX, float densityY) {
    float gu = 3.1415926535 / densityX, gv = 2.0 * 3.1415926535 / (densityY - 1);
    float x, y, z, r;
    glPushMatrix();
    glBegin(GL_QUADS);
    for (int z_step = 0; z_step < densityX; ++z_step) {
        for (int x_step = 0; x_step < densityY; ++x_step) {

            glColor3f(fabs(1 - 2 * (float)z_step / (float)densityX), 1 - fabs(1 - 2 * (float)z_step / (float)densityX),
                      fabs(1 - 2 * (float)x_step / (float)densityY));

            z = cos(z_step * gu);
            r = sqrt(radius - z * z);
            x = r * cos(x_step * gv);
            y = r * sin(x_step * gv);
            glVertex3f(x, y, z);

            z = cos((z_step + 1) * gu);
            r = sqrt(radius - z * z);
            x = r * cos((x_step)*gv);
            y = r * sin((x_step)*gv);
            glVertex3f(x, y, z);

            z = cos((z_step + 1) * gu);
            r = sqrt(radius - z * z);
            x = r * cos((x_step + 1) * gv);
            y = r * sin((x_step + 1) * gv);
            glVertex3f(x, y, z);

            z = cos(z_step * gu);
            r = sqrt(radius - z * z);
            x = r * cos((x_step + 1) * gv);
            y = r * sin((x_step + 1) * gv);
            glVertex3f(x, y, z);
        }
    }
    glEnd();
    glPopMatrix();
}

void GLArea::drawBall(float offsetX, float offsetY, float offsetZ, float radius, float piecesX, float piecesY) {
    glLoadIdentity();
    glTranslatef(-0.5f, 0.0f, -0.5f);
    glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
    glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
    glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
    drawBallEntry(radius, piecesX, piecesY);
}

void GLArea::drawCube(float offsetX, float offsetY, float offsetZ) {
    /*下面开始画立方体*/
    glLoadIdentity();
    glTranslated(offsetX, offsetY, offsetZ);
    glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
    glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
    glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
    glBegin(renderMethod);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(0.25, 0.25, -0.25);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-0.25, 0.25, -0.25);
    glColor3f(0.0, 1.0, 1.0);
    glVertex3f(-0.25, 0.25, 0.25);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(0.25, 0.25, 0.25);
    glEnd();
    glBegin(renderMethod);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(0.25, -0.25, 0.25);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-0.25, -0.25, 0.25);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-0.25, -0.25, -0.25);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.25, -0.25, -0.25);
    glEnd();
    glBegin(renderMethod);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(0.25, 0.25, 0.25);
    glColor3f(0.0, 1.0, 1.0);
    glVertex3f(-0.25, 0.25, 0.25);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-0.25, -0.25, 0.25);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(0.25, -0.25, 0.25);
    glEnd();
    glBegin(renderMethod);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.25, -0.25, -0.25);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-0.25, -0.25, -0.25);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-0.25, 0.25, -0.25);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(0.25, 0.25, -0.25);
    glEnd();
    glBegin(renderMethod);
    glColor3f(0.0, 1.0, 1.0);
    glVertex3f(-0.25, 0.25, 0.25);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-0.25, 0.25, -0.25);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-0.25, -0.25, -0.25);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-0.25, -0.25, 0.25);
    glEnd();
    glBegin(renderMethod);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(0.25, 0.25, -0.25);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(0.25, 0.25, 0.25);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(0.25, -0.25, 0.25);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.25, -0.25, -0.25);
    glEnd();
}

void GLArea::drawAnchor()
{
    glLoadIdentity();
    glPointSize(10.0f);
    glDisable(GL_LIGHTING);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    if (projectionMethod == PROJ_ORTHO)
        glVertex3f(0.0f, 0.0f, 0.0f);
    else if (projectionMethod == PROJ_PERSP)
        glVertex3f(0.0f, 0.0f, -2.0f);
    glEnd();
    if (isLightEnabled)
        glEnable(GL_LIGHTING);
}

void GLArea::drawCoordinate()
{
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    if (projectionMethod == PROJ_ORTHO)
    {
        glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
        glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
        glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
    }
    else if (projectionMethod == PROJ_PERSP)
    {
        glTranslated(0, 0, -2.0f);
        glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
        glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
        glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
        glTranslated(0, 0, 0);
    }
    // 绘制x轴
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.20f, 0.0f, 0.0f);
    glEnd();
    // 绘制y轴
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.20f, 0.0f);
    glEnd();
    // 绘制z轴
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.20f);
    glEnd();
    if (isLightEnabled)
        glEnable(GL_LIGHTING);
}

void GLArea::drawMeshModel(float offsetX, float offsetY, float offsetZ) {
    glLoadIdentity();
    glShadeModel(surfaceMode);
    glPointSize(1.0f * sqrt(zoomRatio * 5 / naturalMag));
    glDisable(GL_POINT_SMOOTH);
    glLineWidth(0.4);
    if (projectionMethod == PROJ_ORTHO)
    {
        // 注意：模型是围绕{moveX, moveY, moveZ}旋转的，即自身中心点，因此要先旋转再平移
        glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
        glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
        glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
        glTranslated(offsetX, offsetY, offsetZ);
    }
    else if (projectionMethod == PROJ_PERSP)
    {
        // 注意：透视投影下，旋转轴在z方向有1.0f的偏移，因此需要先移动旋转轴至原点
        glTranslated(offsetX, offsetY, -2.0f + offsetZ);
        glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
        glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
        glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
        glTranslated(offsetX, offsetY, offsetZ);
    }

    if (displayMode == DISP_BOUNDFILL)
    {
        surfaceMode = GL_SMOOTH;
        renderMode = GL_FILL;
        renderMethod = GL_TRIANGLES;
        openOuterLines(false);
        *isEdited_delegator = true;
        drawBoundingAndFilledMesh();
    }
    else if (displayMode == DISP_DOTFILL)
    {
        surfaceMode = GL_SMOOTH;
        renderMode = GL_FILL;
        renderMethod = GL_TRIANGLES;
        openOuterLines(false);
        *isEdited_delegator = true;
        drawDotAndFilledMesh();
    }
    else
    {
        if (meshColor.isRainbow == false)
            glColor3f(meshColor.red, meshColor.green, meshColor.blue);
        if (filterMode == FLTR_NONE)
            meshModel->displayMeshModel(naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue);
        else
            meshModel->displayMeshModel(filterMode, naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue, 1);
    }
}

void GLArea::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawBackground(backgroundMode, this->width(), this->height()); 
    if (isShowAnchor)       // 如果需要显示锚点
        drawAnchor();
    if (isShowCoordinate)   // 如果需要显示坐标系
        drawCoordinate();
    if (meshModel == nullptr) {}
    else
        drawMeshModel(moveX, moveY, moveZ);
    if (isShowBoundingBox)
        drawBoundingBox();
}

// 更新：线框与面片填充叠加显示
void GLArea::drawBoundingAndFilledMesh()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (meshColor.isRainbow == false)
        glColor3f(meshColor.red, meshColor.green, meshColor.blue);

    if (filterMode == FLTR_NONE)
        meshModel->displayMeshModel(naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue);
    else
        meshModel->displayMeshModel(filterMode, naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue, 1);
    glColor3f(79.0f / 255.0f, 43.0f / 255.0f, 0.0f / 255.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0, -1.0);
    glDisable(GL_LIGHTING);
    if (filterMode == FLTR_NONE)
        meshModel->displayMeshModel(naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue);
    else
        meshModel->displayMeshModel(filterMode, naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue, 1);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT, GL_FILL);
    if (isLightEnabled)
        glEnable(GL_LIGHTING);
}

// 更新：点阵与面片填充叠加显示
void GLArea::drawDotAndFilledMesh()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (meshColor.isRainbow == false)
        glColor3f(meshColor.red, meshColor.green, meshColor.blue);
    if (filterMode == FLTR_NONE)
        meshModel->displayMeshModel(naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue);
    else
        meshModel->displayMeshModel(filterMode, naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue, 1);
    glColor3f(79.0f / 255.0f, 43.0f / 255.0f, 0.0f / 255.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); 
    glEnable(GL_POLYGON_OFFSET_POINT);
    glPolygonOffset(-1.0, -1.0);
    glDisable(GL_LIGHTING);
    if (filterMode == FLTR_NONE)
        meshModel->displayMeshModel(naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue);
    else
        meshModel->displayMeshModel(filterMode, naturalMag, renderMethod, lightColor.red, lightColor.green, lightColor.blue, 1);
    glDisable(GL_POLYGON_OFFSET_POINT);
    glPolygonMode(GL_FRONT, GL_FILL);
    if (isLightEnabled)
        glEnable(GL_LIGHTING);
}

void GLArea::resizeGL(int width, int height)
{
    if (height == 0)
        height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (projectionMethod == PROJ_ORTHO)
    {
        if (width <= height)
            glOrtho(-1.0 / zoomRatio, 1.0 / zoomRatio, -1.0 / zoomRatio * (GLfloat)height / (GLfloat)width,
                    1.0 / zoomRatio * (GLfloat)height / (GLfloat)width, -10.0f, 10.0f);
        else
            glOrtho(-1.0 / zoomRatio * (GLfloat)width / (GLfloat)height, 1.0 / zoomRatio * (GLfloat)width / (GLfloat)height,
                    -1.0 / zoomRatio, 1.0 / zoomRatio, -10.0f, 10.0f);
    }
    else if (projectionMethod == PROJ_PERSP)
    {
        gluPerspective(60.0f, (float)this->width() / (float)this->height(), 0.1f, 10.0f);
        glOrtho(-1.0 / zoomRatio, 1.0 / zoomRatio, -1.0 / zoomRatio, 1.0 / zoomRatio, 0.1f, 10.0f);
    }
    glMatrixMode(GL_MODELVIEW);
}

GLArea::~GLArea()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0f);
    if (meshModel != nullptr)
        delete meshModel;
    isEdited_delegator = nullptr;
    closeButton = nullptr;
    maximizeButton = nullptr;
    closeButtonPixel = nullptr;
    maximizeButtonPixel = nullptr;
}

void GLArea::mousePressEvent(QMouseEvent* event) 
{
    if (event->button() == Qt::LeftButton) {
        isMouseButtonLeftDown = true;
        setCursor(Qt::ClosedHandCursor);
    }
    if (event->button() == Qt::RightButton) {
        isMouseButtonRightDown = true;
    }
}

void GLArea::mouseReleaseEvent(QMouseEvent* event) 
{
    if (event->button() == Qt::LeftButton) {
        isMouseButtonLeftDown = false;
        setCursor(Qt::OpenHandCursor);
    }
    if (event->button() == Qt::RightButton) {
        isMouseButtonRightDown = false;
    }
}

void GLArea::mouseMoveEvent(QMouseEvent* event) 
{
    if (closeButton != nullptr)
    {
        closeButton->setPixmap(closeButtonPixel->scaled(closeButton->width(), closeButton->height()));
        maximizeButton->setPixmap(maximizeButtonPixel->scaled(maximizeButton->width(), maximizeButton->height()));
    }

    if (isMouseButtonLeftDown) {
        if (lastMousePos_x == -1 && lastMousePos_y == -1) 
        {
            lastMousePos_x = event->x();
            lastMousePos_y = event->y();
            return;
        }
        else if (isCtrlDown == false && isAltDown == false && isShiftDown == false)
        {
            if (event->x() - lastMousePos_x > 0)
                rotateAngle_y += 3.0f;
            else if (event->x() - lastMousePos_x < 0)
                rotateAngle_y -= 3.0f;
            if (event->y() - lastMousePos_y > 0)
                rotateAngle_x += 3.0f;
            else if (event->y() - lastMousePos_y < 0)
                rotateAngle_x -= 3.0f;
        }
        else if (isCtrlDown == true && isAltDown == false && isShiftDown == false)
        {
            if (event->x() - lastMousePos_x > 0)
                moveZ -= 0.001f;
            else if (event->x() - lastMousePos_x < 0)
                moveZ += 0.001f;
            if (event->y() - lastMousePos_y > 0)
                moveY -= 0.001f;
            else if (event->y() - lastMousePos_y < 0)
                moveY += 0.001f;
        }
        else if (isCtrlDown == false && isAltDown == false && isShiftDown == true)
        {
            int deltaX = event->x() - lastMousePos_x;
            int deltaY = event->y() - lastMousePos_y;
            if (fabs(deltaX) < fabs(deltaY))
            {
                if (deltaY > 0)
                    rotateAngle_x += 3.0f;
                else if (deltaY < 0)
                    rotateAngle_x -= 3.0f;
            }
            else if (fabs(deltaX) > fabs(deltaY))
            {
                if (deltaX > 0)
                    rotateAngle_y += 3.0f;
                else if (deltaX < 0)
                    rotateAngle_y -= 3.0f;
            }
        }
        updateGL();
        *isEdited_delegator = true;
        lastMousePos_x = event->x();
        lastMousePos_y = event->y();
    }
    if (isMouseButtonRightDown) {
        if (lastMousePos_x == -1 && lastMousePos_y == -1) {
            lastMousePos_x = event->x();
            lastMousePos_y = event->y();
            return;
        }
        else {
            if (this->width() / 2 < event->x()) {
                if (event->y() - lastMousePos_y > 0)
                    rotateAngle_z -= 3.0f;
                else if (event->y() - lastMousePos_y < 0)
                    rotateAngle_z += 3.0f;
            }
            else if (this->width() / 2 > event->x()) {
                if (event->y() - lastMousePos_y > 0)
                    rotateAngle_z += 3.0f;
                else if (event->y() - lastMousePos_y < 0)
                    rotateAngle_z -= 3.0f;
            }
            updateGL();
            *isEdited_delegator = true;
            lastMousePos_x = event->x();
            lastMousePos_y = event->y();
        }
    }
}

void GLArea::wheelEvent(QWheelEvent* event) 
{
    if (isCtrlDown == false && isAltDown == false)
    {
        if (event->delta() > 0)
            zoomRatio = (5.0f > zoomRatio + 0.05f) ? (zoomRatio + 0.05f) : 5.0f;
        else if (event->delta() < 0)
            zoomRatio = (0.2f < zoomRatio - 0.05f) ? (zoomRatio - 0.05f) : 0.2f;
        // 重设绘图区域的大小，没有此步骤视觉上缩放的则是背景而不是图形
        resizeGL(this->width(), this->height());
        *isEdited_delegator = true;
    }
    else if (isCtrlDown == false && isAltDown == true)
    {
        if (event->delta() > 0)
            naturalMag = (0.001f < naturalMag * 0.95) ? (naturalMag * 0.95) : 0.001f;
        else
            naturalMag = (100.0f > naturalMag * 1.05) ? (naturalMag * 1.05) : 100.0f;
        resizeGL(this->width(), this->height());
        *isEdited_delegator = true;
    }
    else if (isCtrlDown == true && isAltDown == false)
    {
        if (event->delta() > 0)
            moveX += 0.001f;
        else if (event->delta() < 0)
            moveX -= 0.001f;
    }
    updateGL();
    *isEdited_delegator = true;
}

bool GLArea::loadObjMesh(const char* meshModelAddress)
{
    if (meshModel == nullptr)
    {
        try
        {
            delete meshModel;
            meshModel = new MeshModel(meshModelAddress);
        }
        catch (...)
        {
            QApplication::beep();
            QMessageBox::critical(this, ch("错误"), ch("无法打开所选模型文件，文件可能已损坏或丢失！"), ch("确定"));
            delete meshModel;
            meshModel = nullptr;
            return false;
        }
    }
    else
    {
        try
        {
            delete meshModel;
            meshModel = new MeshModel(meshModelAddress);
        }
        catch (...)
        {
            QApplication::beep();
            QMessageBox::critical(this, ch("错误"), ch("无法打开所选模型文件，文件可能已损坏或丢失！"), ch("确定"));
            delete meshModel;
            meshModel = nullptr;
            return false;
        }
    }
    if (meshModel->vertices == 0)
    {
        QApplication::beep();
        QMessageBox::critical(this, ch("错误"), ch("打开模型无效：空的模型/源！"), ch("确定"));
        delete meshModel;
        meshModel = nullptr;
        return false;
    }

    if (meshModel->textures != 0) {
        if (QMessageBox::warning(this, ch("载入材质与纹理"), ch("检测到该三维模型包含材质与纹理，是否要导入位图为其填充？否则模型可能无法被正常显示。"), ch("确定"), ch("取消")) == 0) {
            QFileDialog* fileDialog = new QFileDialog();
            fileDialog->setAttribute(Qt::WA_DeleteOnClose);
            QString fileName = fileDialog->getOpenFileName(this, ch("打开材质"), "./", ch("位图素材文件(*.bmp)"));
            if (fileName.isEmpty());
            else
            {
                bool isLoaded = meshModel->LoadBMPTexture(fileName.toStdString().c_str());
                if (!isLoaded)
                {
                    QMessageBox::critical(this, ch("错误"), ch("打开材质无效：空的/错误的材质！"), ch("确定"));
                }
            }
        }
    }

    return true;
}

// 绘制包围盒
void GLArea::drawBoundingBox()
{
    if (meshModel == nullptr)
        return;
    else
    {
        float xMax = FLT_MIN, yMax = FLT_MIN, zMax = FLT_MIN;
        float xMin = FLT_MAX, yMin = FLT_MAX, zMin = FLT_MAX;
        for (int i = 0; i < meshModel->Faces.size(); i++)
        {
            if (xMax < meshModel->Position[meshModel->Faces[i].Position[0]].x / naturalMag)
                xMax = meshModel->Position[meshModel->Faces[i].Position[0]].x / naturalMag;
            if (xMax < meshModel->Position[meshModel->Faces[i].Position[1]].x / naturalMag)
                xMax = meshModel->Position[meshModel->Faces[i].Position[1]].x / naturalMag;
            if (xMax < meshModel->Position[meshModel->Faces[i].Position[2]].x / naturalMag)
                xMax = meshModel->Position[meshModel->Faces[i].Position[2]].x / naturalMag;
                                                                
            if (yMax < meshModel->Position[meshModel->Faces[i].Position[0]].y / naturalMag)
                yMax = meshModel->Position[meshModel->Faces[i].Position[0]].y / naturalMag;
            if (yMax < meshModel->Position[meshModel->Faces[i].Position[1]].y / naturalMag)
                yMax = meshModel->Position[meshModel->Faces[i].Position[1]].y / naturalMag;
            if (yMax < meshModel->Position[meshModel->Faces[i].Position[2]].y / naturalMag)
                yMax = meshModel->Position[meshModel->Faces[i].Position[2]].y / naturalMag;
                                                                 
            if (zMax < meshModel->Position[meshModel->Faces[i].Position[0]].z / naturalMag)
                zMax = meshModel->Position[meshModel->Faces[i].Position[0]].z / naturalMag;
            if (zMax < meshModel->Position[meshModel->Faces[i].Position[1]].z / naturalMag)
                zMax = meshModel->Position[meshModel->Faces[i].Position[1]].z / naturalMag;
            if (zMax < meshModel->Position[meshModel->Faces[i].Position[2]].z / naturalMag)
                zMax = meshModel->Position[meshModel->Faces[i].Position[2]].z / naturalMag;
                                                            

            if (xMin > meshModel->Position[meshModel->Faces[i].Position[0]].x / naturalMag)
                xMin = meshModel->Position[meshModel->Faces[i].Position[0]].x / naturalMag;
            if (xMin > meshModel->Position[meshModel->Faces[i].Position[1]].x / naturalMag)
                xMin = meshModel->Position[meshModel->Faces[i].Position[1]].x / naturalMag;
            if (xMin > meshModel->Position[meshModel->Faces[i].Position[2]].x / naturalMag)
                xMin = meshModel->Position[meshModel->Faces[i].Position[2]].x / naturalMag;
                                                                  
            if (yMin > meshModel->Position[meshModel->Faces[i].Position[0]].y / naturalMag)
                yMin = meshModel->Position[meshModel->Faces[i].Position[0]].y / naturalMag;
            if (yMin > meshModel->Position[meshModel->Faces[i].Position[1]].y / naturalMag)
                yMin = meshModel->Position[meshModel->Faces[i].Position[1]].y / naturalMag;
            if (yMin > meshModel->Position[meshModel->Faces[i].Position[2]].y / naturalMag)
                yMin = meshModel->Position[meshModel->Faces[i].Position[2]].y / naturalMag;

            if (zMin > meshModel->Position[meshModel->Faces[i].Position[0]].z / naturalMag)
                zMin = meshModel->Position[meshModel->Faces[i].Position[0]].z / naturalMag;
            if (zMin > meshModel->Position[meshModel->Faces[i].Position[1]].z / naturalMag)
                zMin = meshModel->Position[meshModel->Faces[i].Position[1]].z / naturalMag;
            if (zMin > meshModel->Position[meshModel->Faces[i].Position[2]].z / naturalMag)
                zMin = meshModel->Position[meshModel->Faces[i].Position[2]].z / naturalMag;
        }

        glLoadIdentity();
        glLineWidth(1.0);
        if (projectionMethod == PROJ_ORTHO)
        {
            // 注意：模型是围绕{moveX, moveY, moveZ}旋转的，即自身中心点，因此要先旋转再平移
            glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
            glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
            glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
            glTranslated(moveX, moveY, moveZ);
        }
        else if (projectionMethod == PROJ_PERSP)
        {
            // 注意：透视投影下，旋转轴在z方向有1.0f的偏移，因此需要先移动旋转轴至原点
            glTranslated(moveX, moveY, -2.0f + moveZ);
            glRotatef(rotateAngle_x, 1.0, 0.0, 0.0);
            glRotatef(rotateAngle_y, 0.0, 1.0, 0.0);
            glRotatef(rotateAngle_z, 0.0, 0.0, 1.0);
            glTranslated(moveX, moveY, moveZ);
        }
        if (backgroundMode == BKG_MESH)
            glColor3f(1.0f, 0.0f, 0.0f);
        else
            glColor3f(1.0f, 1.0f, 1.0f);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMax, yMin, zMin);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMin, yMin, zMax);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMin, yMax, zMin);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMax, yMin, zMax);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMin, zMax);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMax, zMin);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMin, yMax, zMin);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMin, yMax, zMin);
        glVertex3f(xMin, yMax, zMax);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMax, yMax, zMax);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMin, yMax, zMax);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMax, yMax, zMax);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(xMin, yMax, zMax);
        glVertex3f(xMax, yMax, zMax);
        glEnd();
        if (isLightEnabled)
            glEnable(GL_LIGHTING);
    }
}