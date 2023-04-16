// 更新：由于观察到GLInfo单独放在GLArea中，多子窗体存在时会导致无法即时刷新，所以将GLInfo更改为归属为主窗体。

#include "GLInfo.h"
#include "GraphicsatZJU.h"
#include <QTimer>
#include <QLabel>
#include <QTextCodec>
#include <QMouseEvent>
#include <QComboBox>

class GraphicsatZJU;

// 中文Unicode码翻译器
QTextCodec* translateCh = QTextCodec::codecForName("GBK");

GLInfo::GLInfo(QWidget *parent, QRect fatherWndGeometry)
	: QWidget(parent)
{
	ui.setupUi(this);
    setParent(parent);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_Mapped);
    setAutoFillBackground(false);
    setGeometry(20, fatherWndGeometry.height() - 40 - this->height(), this->width(), this->height());
    
    QGraphicsScene* scene = new QGraphicsScene;
    scene->addRect(-125, 0, 460, 200, QPen(Qt::transparent), QBrush(QColor(0, 122, 204, 255)));
    ui.graphicsView->setScene(scene);
    statusChecker = new QTimer(this);
    connect(statusChecker, SIGNAL(timeout()), this, SLOT(updateGLInfo()));
    statusChecker->start(16);
    QPalette textForegroundColor;
    textForegroundColor.setColor(QPalette::WindowText, Qt::white);
    QPalette textForegroundColorTip;
    textForegroundColorTip.setColor(QPalette::WindowText, QColor(255, 220, 0));

    ui.label_zoomRatio->setPalette(textForegroundColor);
    ui.label_zoomRatioTip->setPalette(textForegroundColorTip);
    ui.label_naturalMag->setPalette(textForegroundColor);
    ui.label_naturalMagTip->setPalette(textForegroundColorTip);
    ui.label_xMove->setPalette(textForegroundColor);
    ui.label_xMoveTip->setPalette(textForegroundColorTip);
    ui.label_yMove->setPalette(textForegroundColor);
    ui.label_yMoveTip->setPalette(textForegroundColorTip);
    ui.label_zMove->setPalette(textForegroundColor);
    ui.label_zMoveTip->setPalette(textForegroundColorTip);
    ui.label_xRot->setPalette(textForegroundColor);
    ui.label_xRotTip->setPalette(textForegroundColorTip);
    ui.label_yRot->setPalette(textForegroundColor);
    ui.label_yRotTip->setPalette(textForegroundColorTip);
    ui.label_zRot->setPalette(textForegroundColor);
    ui.label_zRotTip->setPalette(textForegroundColorTip);
 
    this->setWindowOpacity(0.6);
}

GLInfo::~GLInfo()
{
    statusChecker->stop();
    delete statusChecker;
    correspondingArea = nullptr;
}

void GLInfo::updateGLInfo()
{
    // 获取当前聚焦的文档
    GraphicsatZJU* mainForm = (GraphicsatZJU*)parent();
    GraphicsDocument* currentDoc = mainForm->getCurrentDoc();

    // 对主窗体的toolBar/menuStrip上的按钮进行实时控制
    if (currentDoc == nullptr)
    {
        mainForm->statusBarIcon->setPixmap(noDocument->scaled(QSize(16, 16)));
        mainForm->statusBarLabel->setText(translateCh->toUnicode("当前无打开文档"));

        mainForm->ui.toolButton_Close->setEnabled(false);
        mainForm->ui.toolButton_Reload->setEnabled(false);
        mainForm->ui.toolButton_EmptyBkg->setEnabled(false);
        mainForm->ui.toolButton_MeshBkg->setEnabled(false);
        mainForm->ui.toolButton_GradientBkg->setEnabled(false);
        mainForm->ui.toolButton_LightSource->setEnabled(false);
        mainForm->ui.toolButton_BoundingBox->setEnabled(false);
        mainForm->ui.toolButton_CenterPoint->setEnabled(false);
        mainForm->ui.toolButton_Coordinate->setEnabled(false);
        mainForm->ui.toolButton_Points->setEnabled(false);
        mainForm->ui.toolButton_Wireframe->setEnabled(false);
        mainForm->ui.toolButton_FlatLine->setEnabled(false);
        mainForm->ui.toolButton_FlatFill->setEnabled(false);
        mainForm->ui.toolButton_SmoothFill->setEnabled(false);
        mainForm->ui.toolButton_Perspective->setEnabled(false);
        mainForm->ui.toolButton_Orthogonality->setEnabled(false);
        mainForm->ui.toolButton_StatusMonitor->setEnabled(false);
        mainForm->ui.toolButton_Snapshot->setEnabled(false);
        mainForm->ui.menuButton_Close->setEnabled(false);
        mainForm->ui.menuButton_CloseAll->setEnabled(false);
        mainForm->ui.menuButton_Windowed->setEnabled(false);
        mainForm->ui.menuButton_WindowedAll->setEnabled(false);
        mainForm->ui.menuButton_Maximize->setEnabled(false);
        mainForm->ui.menuButton_MaximizeAll->setEnabled(false);
        mainForm->ui.menuButton_Reload->setEnabled(false);
        mainForm->ui.menuButton_EmptyBkg->setEnabled(false);
        mainForm->ui.menuButton_MeshBkg->setEnabled(false);
        mainForm->ui.menuButton_GradientBkg->setEnabled(false);
        mainForm->ui.menuButton_LightSource->setEnabled(false);
        mainForm->ui.menuButton_BoundingBox->setEnabled(false);
        mainForm->ui.menuButton_CenterPoint->setEnabled(false);
        mainForm->ui.menuButton_Coordinate->setEnabled(false);
        mainForm->ui.menuButton_Points->setEnabled(false);
        mainForm->ui.menuButton_Wireframe->setEnabled(false);
        mainForm->ui.menuButton_FlatLine->setEnabled(false);
        mainForm->ui.menuButton_FlatFill->setEnabled(false);
        mainForm->ui.toolButton_BoundFill->setEnabled(false);
        mainForm->ui.toolButton_DotFill->setEnabled(false);
        mainForm->ui.menuButton_BoundFill->setEnabled(false);
        mainForm->ui.menuButton_DotFill->setEnabled(false);
        mainForm->ui.menuButton_SmoothFill->setEnabled(false);
        mainForm->ui.menuButton_Perspective->setEnabled(false);
        mainForm->ui.menuButton_Orthogonality->setEnabled(false);
        mainForm->ui.menuButton_StatusMonitor->setEnabled(false);
        mainForm->ui.menuButton_Snapshot->setEnabled(false);
        mainForm->ui.toolButton_Export->setEnabled(false);
        mainForm->ui.menuButton_Export->setEnabled(false);
        mainForm->ui.menuButton_F->setEnabled(false);
        mainForm->ui.menuButton_B->setEnabled(false);
        mainForm->ui.menuButton_U->setEnabled(false);
        mainForm->ui.menuButton_D->setEnabled(false);
        mainForm->ui.menuButton_L->setEnabled(false);
        mainForm->ui.menuButton_R->setEnabled(false);
        mainForm->ui.menuButton_xAxisR->setEnabled(false);
        mainForm->ui.menuButton_yAxisR->setEnabled(false);
        mainForm->ui.menuButton_zAxisR->setEnabled(false);
        mainForm->ui.menuButton_xAxisL->setEnabled(false);
        mainForm->ui.menuButton_yAxisL->setEnabled(false);
        mainForm->ui.menuButton_zAxisL->setEnabled(false);
        mainForm->ui.menuButton_CloseFilter->setEnabled(false);
        mainForm->ui.menuButton_Explosure->setEnabled(false);
        mainForm->ui.menuButton_Integrated->setEnabled(false);
        mainForm->ui.menuButton_Colorful->setEnabled(false);
        mainForm->ui.menuButton_ColorfulX->setEnabled(false);
        mainForm->ui.menuButton_Rename->setEnabled(false);
        mainForm->ui.menuButton_Save->setEnabled(false);
        mainForm->ui.toolButton_Save->setEnabled(false);
        mainForm->ui.menuButton_SaveAs->setEnabled(false);
        mainForm->ui.menuButton_SaveAll->setEnabled(false);
        mainForm->ui.toolButton_Points->setChecked(false);
        mainForm->ui.toolButton_Wireframe->setChecked(false);
        mainForm->ui.toolButton_FlatLine->setChecked(false);
        mainForm->ui.toolButton_FlatFill->setChecked(false);
        mainForm->ui.toolButton_SmoothFill->setChecked(false);
        mainForm->ui.toolButton_LightSource->setChecked(false);
        mainForm->ui.toolButton_BoundingBox->setChecked(false);
        mainForm->ui.toolButton_CenterPoint->setChecked(false);
        mainForm->ui.toolButton_Coordinate->setChecked(false);
        mainForm->ui.toolButton_GradientBkg->setChecked(false);
        mainForm->ui.toolButton_MeshBkg->setChecked(false);
        mainForm->ui.toolButton_EmptyBkg->setChecked(false);
        mainForm->ui.toolButton_Perspective->setChecked(false);
        mainForm->ui.toolButton_Orthogonality->setChecked(false);
        mainForm->ui.toolButton_StatusMonitor->setChecked(false);
        mainForm->ui.menuButton_Points->setChecked(false);
        mainForm->ui.menuButton_Wireframe->setChecked(false);
        mainForm->ui.menuButton_FlatLine->setChecked(false);
        mainForm->ui.menuButton_FlatFill->setChecked(false);
        mainForm->ui.menuButton_BoundFill->setChecked(false);
        mainForm->ui.menuButton_DotFill->setChecked(false);
        mainForm->ui.toolButton_BoundFill->setChecked(false);
        mainForm->ui.toolButton_DotFill->setChecked(false);
        mainForm->ui.menuButton_SmoothFill->setChecked(false);
        mainForm->ui.menuButton_LightSource->setChecked(false);
        mainForm->ui.menuButton_BoundingBox->setChecked(false);
        mainForm->ui.menuButton_CenterPoint->setChecked(false);
        mainForm->ui.menuButton_Coordinate->setChecked(false);
        mainForm->ui.menuButton_GradientBkg->setChecked(false);
        mainForm->ui.menuButton_MeshBkg->setChecked(false);
        mainForm->ui.menuButton_EmptyBkg->setChecked(false);
        mainForm->ui.menuButton_Perspective->setChecked(false);
        mainForm->ui.menuButton_Orthogonality->setChecked(false);
        mainForm->ui.menuButton_StatusMonitor->setChecked(false);
        mainForm->ui.menuButton_CloseFilter->setChecked(false);
        mainForm->ui.menuButton_Explosure->setChecked(false);
        mainForm->ui.menuButton_Integrated->setChecked(false);
        mainForm->ui.menuButton_Colorful->setChecked(false);
        mainForm->ui.menuButton_ColorfulX->setChecked(false);
        mainForm->ui.menuButton_Dense->setEnabled(false);
        mainForm->ui.menuButton_Sparse->setEnabled(false);
        mainForm->ui.toolButton_Color->setEnabled(false);
        mainForm->ui.menuButton_Color->setEnabled(false);
        mainForm->ui.lightOption_diffuse->setEnabled(false);
        mainForm->ui.lightOption_Specular->setEnabled(false);
        mainForm->ui.lightOption_diffuse->setChecked(false);
        mainForm->ui.lightOption_Specular->setChecked(false);
        mainForm->ui.menuButton_LoadTexture->setEnabled(false);

        mainForm->ui.lightOption_Directional->setEnabled(false);
        mainForm->ui.lightOption_Positional->setEnabled(false);
        mainForm->ui.lightOption_Directional->setChecked(false);
        mainForm->ui.lightOption_Positional->setChecked(false);
        mainForm->ui.menu_LightOption->setEnabled(false);
        mainForm->ui.toolButton_LoadTexture->setEnabled(false);
        mainForm->ui.menuButton_LoadTexture->setEnabled(false);
    
        mainForm->comboBox->setCurrentIndex(0);
        mainForm->comboBox->setItemText(0, translateCh->toUnicode("当前无打开文档"));
        mainForm->ui.fixedButton_DocIconOpened->setVisible(false);
        mainForm->ui.fixedButton_DocIconUnopened->setVisible(true);
        mainForm->comboBox->resize(120, mainForm->comboBox->height());

        mainForm->currentDocument->setText("Graphics at ZJU");
        mainForm->currentDocument->adjustSize();
        mainForm->currentDocument->resize(mainForm->currentDocument->width() + 10, mainForm->currentDocument->height());
        mainForm->setToolTip(translateCh->toUnicode("当前无打开文档"));
    }
    else
    {
        correspondingArea = currentDoc->correspondingGLArea;
        mainForm->comboBox->setItemText(0, currentDoc->documentName);
        mainForm->comboBox->setCurrentIndex(0);
        mainForm->ui.fixedButton_DocIconOpened->setVisible(true);
        mainForm->ui.fixedButton_DocIconUnopened->setVisible(false);

        mainForm->currentDocument->setText(currentDoc->documentName);
        mainForm->currentDocument->adjustSize();
        mainForm->currentDocument->resize(mainForm->currentDocument->width() + 10, mainForm->currentDocument->height());
        mainForm->setToolTip(currentDoc->savePath);

        if (currentDoc->isEdited) 
        {
            mainForm->statusBarIcon->setPixmap(edited->scaled(QSize(16, 16)));
            mainForm->statusBarLabel->setText(translateCh->toUnicode("有待保存的修改"));
        }
        else
        {
            mainForm->statusBarIcon->setPixmap(saved->scaled(QSize(16, 16)));
            mainForm->statusBarLabel->setText(translateCh->toUnicode("所有修改已保存"));
        }

        // 设置应用程序功能组件按钮
        mainForm->ui.toolButton_Close->setEnabled(true);
        mainForm->ui.menuButton_Close->setEnabled(true);
        mainForm->ui.menuButton_CloseAll->setEnabled(true);
        mainForm->ui.menuButton_Windowed->setEnabled(true);
        mainForm->ui.menuButton_WindowedAll->setEnabled(true);
        mainForm->ui.menuButton_Maximize->setEnabled(true);
        mainForm->ui.menuButton_MaximizeAll->setEnabled(true);
        mainForm->ui.toolButton_Snapshot->setEnabled(true);
        mainForm->ui.menuButton_Snapshot->setEnabled(true);

        mainForm->ui.menuButton_F->setEnabled(true);
        mainForm->ui.menuButton_B->setEnabled(true);
        mainForm->ui.menuButton_U->setEnabled(true);
        mainForm->ui.menuButton_D->setEnabled(true);
        mainForm->ui.menuButton_L->setEnabled(true);
        mainForm->ui.menuButton_R->setEnabled(true);
        mainForm->ui.menuButton_xAxisR->setEnabled(true);
        mainForm->ui.menuButton_yAxisR->setEnabled(true);
        mainForm->ui.menuButton_zAxisR->setEnabled(true);
        mainForm->ui.menuButton_xAxisL->setEnabled(true);
        mainForm->ui.menuButton_yAxisL->setEnabled(true);
        mainForm->ui.menuButton_zAxisL->setEnabled(true);

        mainForm->ui.menuButton_Rename->setEnabled(true);
        mainForm->ui.menuButton_Save->setEnabled(true);
        mainForm->ui.toolButton_Save->setEnabled(true);
        mainForm->ui.menuButton_SaveAs->setEnabled(true);
        mainForm->ui.menuButton_SaveAll->setEnabled(true);
        mainForm->ui.toolButton_Color->setEnabled(true);
        mainForm->ui.menuButton_Color->setEnabled(true);
        mainForm->ui.lightOption_diffuse->setEnabled(true);
        mainForm->ui.lightOption_Specular->setEnabled(true);
        mainForm->ui.lightOption_diffuse->setChecked(currentDoc->correspondingGLArea->isDiffuseAccepted);
        mainForm->ui.lightOption_Specular->setChecked(currentDoc->correspondingGLArea->isSpecularAccepted);
        mainForm->ui.lightOption_Directional->setEnabled(true);
        mainForm->ui.lightOption_Positional->setEnabled(true);
        mainForm->ui.lightOption_Directional->setChecked(currentDoc->correspondingGLArea->isDirectionalLight);
        mainForm->ui.lightOption_Positional->setChecked(!currentDoc->correspondingGLArea->isDirectionalLight);
        mainForm->ui.menu_LightOption->setEnabled(true);

        // 如果模型不为空才允许重新载入和滤镜
        if(correspondingArea->meshModel != nullptr)
        {
            mainForm->ui.toolButton_Reload->setEnabled(true);
            mainForm->ui.menuButton_Reload->setEnabled(true);

            mainForm->ui.menuButton_CloseFilter->setEnabled(true);
            mainForm->ui.menuButton_Explosure->setEnabled(true);
            mainForm->ui.menuButton_Integrated->setEnabled(true);
            mainForm->ui.menuButton_Colorful->setEnabled(true);
            mainForm->ui.menuButton_ColorfulX->setEnabled(true);

            mainForm->ui.menuButton_Explosure->setChecked(correspondingArea->filterMode == FLTR_EXPOSURE);
            mainForm->ui.menuButton_Integrated->setChecked(correspondingArea->filterMode == FLTR_INTEGRAT);
            mainForm->ui.menuButton_Colorful->setChecked(correspondingArea->filterMode == FLTR_COLORFUL);
            mainForm->ui.menuButton_ColorfulX->setChecked(correspondingArea->filterMode == FLTR_CLFIGENV);

            mainForm->ui.toolButton_Export->setEnabled(true);
            mainForm->ui.menuButton_Export->setEnabled(true);

            mainForm->ui.menuButton_Dense->setEnabled(true);
            mainForm->ui.menuButton_Sparse->setEnabled(true);
            mainForm->ui.menuButton_LoadTexture->setEnabled(true);
            mainForm->ui.toolButton_LoadTexture->setEnabled(true);
            mainForm->ui.menuButton_LoadTexture->setEnabled(true);
        }
        else
        {
            mainForm->ui.toolButton_Reload->setEnabled(false);
            mainForm->ui.menuButton_Reload->setEnabled(false);

            mainForm->ui.menuButton_CloseFilter->setEnabled(false);
            mainForm->ui.menuButton_Explosure->setEnabled(false);
            mainForm->ui.menuButton_Integrated->setEnabled(false);
            mainForm->ui.menuButton_Colorful->setEnabled(false);
            mainForm->ui.menuButton_ColorfulX->setEnabled(false);

            mainForm->ui.toolButton_Export->setEnabled(false);
            mainForm->ui.menuButton_Export->setEnabled(false);

            mainForm->ui.menuButton_Dense->setEnabled(false);
            mainForm->ui.menuButton_Sparse->setEnabled(false);
            mainForm->ui.menuButton_LoadTexture->setEnabled(false);
            mainForm->ui.toolButton_LoadTexture->setEnabled(false);
            mainForm->ui.menuButton_LoadTexture->setEnabled(false);
        }

        // 设置文档组建按钮点选状况
        mainForm->ui.toolButton_LightSource->setEnabled(true);
        mainForm->ui.toolButton_BoundingBox->setEnabled(true);
        mainForm->ui.toolButton_CenterPoint->setEnabled(true);
        mainForm->ui.toolButton_Coordinate->setEnabled(true);
        mainForm->ui.toolButton_StatusMonitor->setEnabled(true);
        mainForm->ui.toolButton_LightSource->setChecked(correspondingArea->isLightEnabled);
        mainForm->ui.toolButton_BoundingBox->setChecked(correspondingArea->isShowBoundingBox);
        mainForm->ui.toolButton_CenterPoint->setChecked(correspondingArea->isShowAnchor);
        mainForm->ui.toolButton_Coordinate->setChecked(correspondingArea->isShowCoordinate);
        mainForm->ui.toolButton_StatusMonitor->setChecked(correspondingArea->isShowMonitor);

        mainForm->ui.menuButton_LightSource->setEnabled(true);
        mainForm->ui.menuButton_BoundingBox->setEnabled(true);
        mainForm->ui.menuButton_CenterPoint->setEnabled(true);
        mainForm->ui.menuButton_Coordinate->setEnabled(true);
        mainForm->ui.menuButton_StatusMonitor->setEnabled(true);
        mainForm->ui.menuButton_LightSource->setChecked(correspondingArea->isLightEnabled);
        mainForm->ui.menuButton_BoundingBox->setChecked(correspondingArea->isShowBoundingBox);
        mainForm->ui.menuButton_CenterPoint->setChecked(correspondingArea->isShowAnchor);
        mainForm->ui.menuButton_Coordinate->setChecked(correspondingArea->isShowCoordinate);
        mainForm->ui.menuButton_StatusMonitor->setChecked(correspondingArea->isShowMonitor);
        // 设置背景模式点选状况
        mainForm->ui.toolButton_EmptyBkg->setEnabled(true);
        mainForm->ui.toolButton_MeshBkg->setEnabled(true);
        mainForm->ui.toolButton_GradientBkg->setEnabled(true);
        mainForm->ui.toolButton_EmptyBkg->setChecked(correspondingArea->backgroundMode == BKG_EMPTY);
        mainForm->ui.toolButton_MeshBkg->setChecked(correspondingArea->backgroundMode == BKG_MESH);
        mainForm->ui.toolButton_GradientBkg->setChecked(correspondingArea->backgroundMode == BKG_GRADIENT);

        mainForm->ui.menuButton_EmptyBkg->setEnabled(true);
        mainForm->ui.menuButton_MeshBkg->setEnabled(true);
        mainForm->ui.menuButton_GradientBkg->setEnabled(true);
        mainForm->ui.menuButton_EmptyBkg->setChecked(correspondingArea->backgroundMode == BKG_EMPTY);
        mainForm->ui.menuButton_MeshBkg->setChecked(correspondingArea->backgroundMode == BKG_MESH);
        mainForm->ui.menuButton_GradientBkg->setChecked(correspondingArea->backgroundMode == BKG_GRADIENT);


        // 设置显示方式按钮点选状况
        mainForm->ui.toolButton_Points->setEnabled(true);
        mainForm->ui.toolButton_Wireframe->setEnabled(true);
        mainForm->ui.toolButton_FlatLine->setEnabled(true);
        mainForm->ui.toolButton_FlatFill->setEnabled(true);
        mainForm->ui.toolButton_SmoothFill->setEnabled(true);
        mainForm->ui.toolButton_BoundFill->setEnabled(true);
        mainForm->ui.toolButton_DotFill->setEnabled(true);
        mainForm->ui.toolButton_Points->setChecked(correspondingArea->displayMode == DISP_POINTS);
        mainForm->ui.toolButton_Wireframe->setChecked(correspondingArea->displayMode == DISP_WIREFRAME);
        mainForm->ui.toolButton_FlatLine->setChecked(correspondingArea->displayMode == DISP_FLATLINES);
        mainForm->ui.toolButton_FlatFill->setChecked(correspondingArea->displayMode == DISP_FLATFILL);
        mainForm->ui.toolButton_SmoothFill->setChecked(correspondingArea->displayMode == DISP_SMOOTHFILL);
        mainForm->ui.toolButton_BoundFill->setChecked(correspondingArea->displayMode == DISP_BOUNDFILL);
        mainForm->ui.toolButton_DotFill->setChecked(correspondingArea->displayMode == DISP_DOTFILL);

        mainForm->ui.menuButton_Points->setEnabled(true);
        mainForm->ui.menuButton_Wireframe->setEnabled(true);
        mainForm->ui.menuButton_FlatLine->setEnabled(true);
        mainForm->ui.menuButton_FlatFill->setEnabled(true);
        mainForm->ui.menuButton_SmoothFill->setEnabled(true);
        mainForm->ui.menuButton_BoundFill->setEnabled(true);
        mainForm->ui.menuButton_DotFill->setEnabled(true);
        mainForm->ui.menuButton_Points->setChecked(correspondingArea->displayMode == DISP_POINTS);
        mainForm->ui.menuButton_Wireframe->setChecked(correspondingArea->displayMode == DISP_WIREFRAME);
        mainForm->ui.menuButton_FlatLine->setChecked(correspondingArea->displayMode == DISP_FLATLINES);
        mainForm->ui.menuButton_FlatFill->setChecked(correspondingArea->displayMode == DISP_FLATFILL);
        mainForm->ui.menuButton_SmoothFill->setChecked(correspondingArea->displayMode == DISP_SMOOTHFILL);
        mainForm->ui.menuButton_BoundFill->setChecked(correspondingArea->displayMode == DISP_BOUNDFILL);
        mainForm->ui.menuButton_DotFill->setChecked(correspondingArea->displayMode == DISP_DOTFILL);

        // 设置投影方式按钮点选状况
        mainForm->ui.toolButton_Perspective->setEnabled(true);
        mainForm->ui.toolButton_Orthogonality->setEnabled(true);
        mainForm->ui.toolButton_Perspective->setChecked(correspondingArea->projectionMethod == PROJ_PERSP);
        mainForm->ui.toolButton_Orthogonality->setChecked(correspondingArea->projectionMethod == PROJ_ORTHO);

        mainForm->ui.menuButton_Perspective->setEnabled(true);
        mainForm->ui.menuButton_Orthogonality->setEnabled(true);
        mainForm->ui.menuButton_Perspective->setChecked(correspondingArea->projectionMethod == PROJ_PERSP);
        mainForm->ui.menuButton_Orthogonality->setChecked(correspondingArea->projectionMethod == PROJ_ORTHO);
    }


    // 对状态栏窗口进行实时控制
    if (currentDoc == nullptr)
    {
        // 如果当前没有文档，则设置状态栏为不可见
        this->setVisible(false);
        return;
    }
    else
    {
        // 如果当前有文档，则设置状态栏为可见且针对当前文档中的GLArea进行状态刷新
        correspondingArea = currentDoc->correspondingGLArea;
        this->setVisible(correspondingArea->isShowMonitor);
    }
    float zoomRatio = correspondingArea->zoomRatio * 100;
    ui.label_zoomRatio->setText(QString::number(int(zoomRatio)) + "%");

    float naturalMag = 5 / correspondingArea->naturalMag * 100;
    ui.label_naturalMag->setText(QString::number(int(naturalMag)) + "%");

    char xMove[10], yMove[8], zMove[8];
    sprintf_s(xMove, "%.3f", correspondingArea->moveZ);
    sprintf_s(yMove, "%.3f", correspondingArea->moveX);
    sprintf_s(zMove, "%.3f", correspondingArea->moveY);
    ui.label_xMove->setText(xMove);
    ui.label_yMove->setText(yMove);
    ui.label_zMove->setText(zMove);
    
    // 使GLArea中的三个旋转角度始终保持在-360°至360°之间
    if (correspondingArea->rotateAngle_x < -360.0f)
        correspondingArea->rotateAngle_x += 360.0f;
    else if(correspondingArea->rotateAngle_x > 360.0f)
        correspondingArea->rotateAngle_x -= 360.6f;
    if (correspondingArea->rotateAngle_y < -360.0f)
        correspondingArea->rotateAngle_y += 360.0f;
    else if (correspondingArea->rotateAngle_y > 360.0f)
        correspondingArea->rotateAngle_y -= 360.6f;
    if (correspondingArea->rotateAngle_z < -360.0f)
        correspondingArea->rotateAngle_z += 360.0f;
    else if (correspondingArea->rotateAngle_z > 360.0f)
        correspondingArea->rotateAngle_z -= 360.6f;


    char xRot[10], yRot[10], zRot[10];
    sprintf_s(xRot, "%.2f", correspondingArea->rotateAngle_z);
    sprintf_s(yRot, "%.2f", correspondingArea->rotateAngle_x);
    sprintf_s(zRot, "%.2f", correspondingArea->rotateAngle_y);
    ui.label_xRot->setText(QString(xRot) + translateCh->toUnicode("°"));
    ui.label_yRot->setText(QString(yRot) + translateCh->toUnicode("°"));
    ui.label_zRot->setText(QString(zRot) + translateCh->toUnicode("°"));
}
