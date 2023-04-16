#include "DeveloperInfoForm.h"
#include "GraphicsatZJU.h"
#include "WidgetsEvents.h"
#include "GraphicsDocument.h"
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodeC>
#include <QMdiSubWindow>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include "GDRenameDialog.h"
#include "GuideForm.h"
#include <QColorDialog>

// ch(�����ַ���) ���Խ������ַ���ת��ΪUnicode��
#define ch(ChineseArg) translateChinese->toUnicode(ChineseArg)

#define delegator

// �ⲿ����������ľ���
extern GraphicsatZJU* MainForm;
extern Ui::GraphicsatZJUClass MainFormClass;
static QTextCodec* translateChinese = QTextCodec::codecForName("GBK");

// ������"��"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Open_click()
{
    QFileDialog* fileDialog = new QFileDialog();
	QString fileName = fileDialog->getOpenFileName(MainForm, ch("����Ŀ�ļ�"), "./", ch("ȫ���ļ�(*gdoc *.obj);;����ͼ���ĵ�(*.gdoc);;3Dģ���ļ�(*.obj)"));
    if (fileName.isEmpty());
	else
	{
        // ���������ļ���".obj"��ʽ�ļ�
        if (fileName.endsWith(".obj"))
        {
            GraphicsDocument* currentDoc = MainForm->getCurrentDoc();
            if (currentDoc != nullptr)  // ����д򿪵��ĵ�
            {
                // ��ǰ�۽��ĵ������ļ���ַ
                currentDoc->correspondingGLArea->loadObjMesh(fileName.toStdString().c_str());
            }
            else // �����ǰû�д򿪵��ĵ�
            {
                QMessageBox::information(MainForm, ch("����άģ��"), ch("������һ����άģ���ļ�����Ϊ������һ��ͼ���ĵ����ڳ�����άģ�͡�"), ch("ȷ��"));
                // �����½�һ���ĵ�
                WidgetsEvents::toolButton_New_click();
                currentDoc = MainForm->getCurrentDoc();
                currentDoc->correspondingGLArea->loadObjMesh(fileName.toStdString().c_str());
            }
        }
        else if (fileName.endsWith(".gdoc"))
        {
            GraphicsDocument* gDoc = new GraphicsDocument();
            gDoc->setFocusPolicy(Qt::StrongFocus);
            gDoc->setMinimumSize(640, 480);
            std::ifstream in(fileName.toStdString());
            std::string args;
            getline(in, args);
            gDoc->documentName = QString::fromStdString(args);
            getline(in, args);
            // ����ظ�
            for (auto menuButton : WidgetsEvents::menuButton_gDocsList)
                if (WidgetsEvents::menuButton_to_SubWindow.find(menuButton)->second->savePath == QString::fromStdString(args)) {
                    QApplication::beep();
                    QMessageBox::warning(MainForm, ch("����"), ch("���ĵ��ѱ��򿪣�"), ch("ȷ��"));
                    delete gDoc;
                    return;
                }
            gDoc->savePath = QString::fromStdString(args);
            QMdiSubWindow* subWindow = (QMdiSubWindow*)MainFormClass.mdiArea->addSubWindow(gDoc);
            subWindow->setWindowFlags(Qt::FramelessWindowHint);
            subWindow->setWindowTitle(gDoc->documentName);
            subWindow->setFocusPolicy(Qt::StrongFocus);
            subWindow->setAttribute(Qt::WA_DeleteOnClose);
            subWindow->setWindowIcon(QIcon(":/GraphicsatZJU/Resources/ToolbarIcons/0x05B.png"));
            subWindow->showMaximized();
            gDoc->correspondingSubWindow = subWindow;
            gDoc->correspondingGLArea->setVisible(true);
            gDoc->correspondingGLArea->setGeometry(subWindow->geometry());
            QAction* menuButton_gDoc = new QAction(MainForm);
            gDoc->setCorrespondingMenuButton(menuButton_gDoc);
            MainFormClass.menu_W->addAction(menuButton_gDoc);
            menuButton_gDoc->setText(gDoc->documentName);
            menuButton_gDoc->setObjectName("menuButton_" + gDoc->documentName);
            menuButton_gDoc->setCheckable(true);
            menuButton_gDocsList.push_back(menuButton_gDoc);
            menuButton_gDoc->setChecked(true);
            menuButton_gDoc_setOtherUnchecked(menuButton_gDoc);
            MainFormClass.retranslateUi(MainForm);
            menuButton_to_SubWindow.insert({menuButton_gDoc, gDoc});
            subWindow_to_MenuButton.insert({gDoc, menuButton_gDoc});
            QObject::connect(gDoc, SIGNAL(focusedIn()), this, SLOT(subWindow_focusedIn()));
            QObject::connect(menuButton_gDoc, SIGNAL(triggered()), this, SLOT(menuButton_gDoc_click()));

            in >> gDoc->isSaved;
            in >> gDoc->correspondingGLArea->parentWidth;
            in >> gDoc->correspondingGLArea->parentHeight;
            in >> gDoc->correspondingGLArea->renderMode;
            in >> gDoc->correspondingGLArea->renderMethod;
            in >> gDoc->correspondingGLArea->surfaceMode;
            in >> gDoc->correspondingGLArea->backgroundMode;
            in >> gDoc->correspondingGLArea->displayMode;
            in >> gDoc->correspondingGLArea->filterMode;
            in >> gDoc->correspondingGLArea->isShowAnchor;
            in >> gDoc->correspondingGLArea->isShowCoordinate;
            in >> gDoc->correspondingGLArea->isLightEnabled;
            in >> gDoc->correspondingGLArea->isShowBoundingBox;
            in >> gDoc->correspondingGLArea->isShowMonitor;
            in >> gDoc->correspondingGLArea->meshColor.red >> gDoc->correspondingGLArea->meshColor.green >> gDoc->correspondingGLArea->meshColor.blue;
            in >> gDoc->correspondingGLArea->lightColor.red >> gDoc->correspondingGLArea->lightColor.green >> gDoc->correspondingGLArea->lightColor.blue;
            in >> gDoc->correspondingGLArea->rotateAngle_x;
            in >> gDoc->correspondingGLArea->rotateAngle_y;
            in >> gDoc->correspondingGLArea->rotateAngle_z;
            in >> gDoc->correspondingGLArea->zoomRatio;
            in >> gDoc->correspondingGLArea->naturalMag;
            in >> gDoc->correspondingGLArea->moveX;
            in >> gDoc->correspondingGLArea->moveY;
            in >> gDoc->correspondingGLArea->moveZ;

            std::string existMeshModel;
            char endsep;
            in >> existMeshModel;
            if (existMeshModel == "1")
            {
                in >> endsep;
                getline(in, args);
                gDoc->correspondingGLArea->loadObjMesh((endsep + args).c_str());
            }
            gDoc->correspondingGLArea->resizeGLArea();
            gDoc->correspondingGLArea->updateGL();

            gDoc->correspondingGLArea->showMaximized();
        }
	}
    delete fileDialog;
}

DeveloperInfoForm* developerInfoForm = nullptr;
// ������"��������Ϣ"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Info_click()
{
    if (developerInfoForm == nullptr) {
        developerInfoForm = new DeveloperInfoForm();
        developerInfoForm->show();
    }
}

// ������"�½�"��ť�ĵ���¼�
void WidgetsEvents::toolButton_New_click()
{
    GraphicsDocument* gDoc = new GraphicsDocument();
    // ������ͼ���ĵ��Ķ�����
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString gDocName(ch("��ͼ���ĵ�") + "-" +
                     (QString::number(currentDateTime.date().month()).length() == 1 ? "0" + QString::number(currentDateTime.date().month()) : QString::number(currentDateTime.date().month())) + (QString::number(currentDateTime.date().day()).length() == 1 ? "0" + QString::number(currentDateTime.date().day()) : QString::number(currentDateTime.date().day())) + QString::number(currentDateTime.time().hour()) + (QString::number(currentDateTime.time().minute()).length() == 1 ? "0" + QString::number(currentDateTime.time().minute()) : QString::number(currentDateTime.time().minute())));
    gDoc->setObjectName(gDocName);
    gDoc->setFocusPolicy(Qt::StrongFocus);
    gDoc->setMinimumSize(640, 480);
    gDoc->documentName = gDocName;
    // ����ͼ���ĵ���������ΪmdiArea���Ӵ���
    QMdiSubWindow* subWindow = (QMdiSubWindow*)MainFormClass.mdiArea->addSubWindow(gDoc);
    subWindow->setWindowIcon(QIcon(":/GraphicsatZJU/Resources/ToolbarIcons/0x05B.png"));
    subWindow->setWindowFlags(subWindow->windowFlags() & ~Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint);
    subWindow->setWindowTitle(gDocName);
    subWindow->setFocusPolicy(Qt::StrongFocus);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    
    // Ĭ�������ʾ
    subWindow->showMaximized();
    gDoc->correspondingSubWindow = subWindow;
    gDoc->correspondingGLArea->setVisible(true);
    gDoc->correspondingGLArea->setGeometry(subWindow->geometry());
    gDoc->correspondingGLArea->showMaximized();
    
    QAction* menuButton_gDoc = new QAction(MainForm);
    gDoc->setCorrespondingMenuButton(menuButton_gDoc);
    menuButton_gDoc->setObjectName("menuButton_" + gDocName);
    menuButton_gDoc->setCheckable(true);
    menuButton_gDoc->setChecked(true);
    menuButton_gDoc_setOtherUnchecked(menuButton_gDoc);
    menuButton_to_SubWindow.insert({menuButton_gDoc, gDoc});
    subWindow_to_MenuButton.insert({gDoc, menuButton_gDoc});
    QObject::connect(gDoc, SIGNAL(focusedIn()), this, SLOT(subWindow_focusedIn()));
    QObject::connect(menuButton_gDoc, SIGNAL(triggered()), this, SLOT(menuButton_gDoc_click()));

    GDRenameDialog* renameDialog = new GDRenameDialog(gDoc);
    renameDialog->getDocumentName();

    menuButton_gDoc->setText(gDoc->documentName);
    MainFormClass.menu_W->addAction(menuButton_gDoc);
    MainFormClass.retranslateUi(MainForm);
    menuButton_gDocsList.push_back(menuButton_gDoc);
}

void WidgetsEvents::menuButton_gDoc_setOtherUnchecked(QAction* menuButton) 
{
    for (auto menuButton_gDoc : menuButton_gDocsList)
        if (menuButton_gDoc != menuButton)
            menuButton_gDoc->setChecked(false);
}

void WidgetsEvents::menuButton_gDoc_click() 
{
    QAction* menuButton = (QAction*)sender();
    menuButton->setChecked(true);
    menuButton_gDoc_setOtherUnchecked(menuButton);
    GraphicsDocument* subWindow = menuButton_to_SubWindow.find(menuButton)->second;
    subWindow->setFocus();
    menuButton = nullptr;
    subWindow = nullptr;
}

void WidgetsEvents::subWindow_focusedIn() 
{
    GraphicsDocument* subWindow = (GraphicsDocument*)sender();
    QAction* menuButton = (QAction*)WidgetsEvents::subWindow_to_MenuButton.find(subWindow)->second;
    menuButton->setChecked(true);
    WidgetsEvents::menuButton_gDoc_setOtherUnchecked(menuButton);
    menuButton = nullptr;
    subWindow = nullptr;
}

void WidgetsEvents::menuButton_New_click()
{
    WidgetsEvents::toolButton_New_click();
}

// "�ļ�"�����˵���"��"��ť�ĵ���¼�
void WidgetsEvents::menuButton_Open_click()
{
    WidgetsEvents::toolButton_Open_click();
}

// ������"�ر��ĵ�"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Close_click()
{
    GraphicsDocument* currentDoc = MainForm->getCurrentDoc();
    if (currentDoc != nullptr)
        currentDoc->close();
    currentDoc = nullptr;
}

void WidgetsEvents::menuButton_CloseAll_click()
{
    // ѭ����ɾ��Ҫ����ɾ��
    for (int i = WidgetsEvents::menuButton_gDocsList.size() - 1; i >= 0 ; i--)
        WidgetsEvents::menuButton_to_SubWindow.find(WidgetsEvents::menuButton_gDocsList[i])->second->close();
}

// ������"����"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Reload_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->resetOverall();
    currentArea->resizeGLArea();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"����"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Export_click()
{
    if (QMessageBox::information(MainForm, ch("����Ϊģ��"), ch("ȷ��Ҫ����ǰ�ĵ��е�ͼ�ε���Ϊ��άģ���ļ���"), ch("ȷ��"), ch("ȡ��")) != 0)
        return;
    QString filePath = QFileDialog::getSaveFileName(MainForm, ch("����ģ��"), "./" + MainForm->getCurrentDoc()->documentName + ".obj", ch("��άģ���ļ�(*.obj)"));
    if (filePath.isEmpty());
    else
    {
        MeshModel* currentModel = MainForm->getCurrentDoc()->correspondingGLArea->meshModel;
        currentModel->exportMesh(filePath.toStdString().c_str());
    }
}

// ������"����"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Snapshot_click()
{
    GraphicsDocument* currentDoc = MainForm->getCurrentDoc();
    GLArea* currentArea = currentDoc->correspondingGLArea;
    QString filePath = QFileDialog::getSaveFileName(MainForm, ch("�������"), "./" + MainForm->getCurrentDoc()->documentName + ".bmp", ch("ȫ���ļ�(*.png;*.bmp);;λͼ�ļ�(*.bmp);;PNGͼ���ļ�(*.png)"));
    if (filePath.isEmpty()) {

    }
    else
    {
        QImage snapShot = currentArea->grabFrameBuffer(true);
        if (filePath.endsWith(".bmp"))
            snapShot.save(filePath);
        else if (filePath.endsWith(".png"))
            snapShot.save(filePath);
    }
    currentDoc = nullptr;
    currentArea = nullptr;
}

// ������"�߽��"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Bounding_click()
{

}

// ������"����"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Points_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->setRenderMode(GL_POINT);
    currentArea->setRenderMethod(GL_POINTS);
    currentArea->openOuterLines(false);
    currentArea->displayMode = DISP_POINTS;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"�߿�"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Wireframe_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->setRenderMode(GL_LINE);
    currentArea->setRenderMethod(GL_LINE_LOOP);
    currentArea->openOuterLines(false);
    currentArea->displayMode = DISP_WIREFRAME;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"�����"��ť�ĵ���¼�
void WidgetsEvents::toolButton_FlatFill_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->setSurfaceMode(GL_FLAT);
    currentArea->setRenderMode(GL_FILL);
    currentArea->setRenderMethod(GL_TRIANGLES);
    currentArea->openOuterLines(false);
    currentArea->displayMode = DISP_FLATFILL;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"�߿����"��ť�ĵ���¼�
void WidgetsEvents::toolButton_FlatLine_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->setSurfaceMode(GL_FLAT);
    currentArea->setRenderMode(GL_LINE);
    currentArea->setRenderMethod(GL_TRIANGLES);
    currentArea->openOuterLines();
    currentArea->displayMode = DISP_FLATLINES;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"ƽ�����"��ť�ĵ���¼�
void WidgetsEvents::toolButton_SmoothFill_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->setSurfaceMode(GL_SMOOTH);
    currentArea->setRenderMode(GL_FILL);
    currentArea->setRenderMethod(GL_TRIANGLES);
    currentArea->openOuterLines(false);
    currentArea->displayMode = DISP_SMOOTHFILL;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::toolButton_BoundFill_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->displayMode = DISP_BOUNDFILL;
    currentArea->updateGL();
    currentArea = nullptr;
}

void WidgetsEvents::toolButton_DotFill_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->displayMode = DISP_DOTFILL;
    currentArea->updateGL();
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_BoundFill_click()
{
    WidgetsEvents::toolButton_BoundFill_click();
}

void WidgetsEvents::menuButton_DotFill_click()
{
    WidgetsEvents::toolButton_DotFill_click();
}

// ������"����"��ť�ĵ���¼�
void WidgetsEvents::toolButton_LightSource_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->switchLightEnable();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"��ʾ���ĵ�"��ť�ĵ���¼�
void WidgetsEvents::toolButton_CenterPoint_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->switchAnchorEnable();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"��ʾ������"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Coordinate_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->switchCoordinateEnable();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"��ʾ��Χ��"��ť�ĵ���¼�
void WidgetsEvents::toolButton_BoundingBox_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->switchBoundingBoxEnable();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"���䱳��"��ť�ĵ���¼�
void WidgetsEvents::toolButton_GradientBkg_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->backgroundMode = 0x001;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"���񱳾�"��ť�ĵ���¼�
void WidgetsEvents::toolButton_MeshBkg_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->backgroundMode = 0x002;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"��ɫ����"��ť�ĵ���¼�
void WidgetsEvents::toolButton_EmptyBkg_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->backgroundMode = 0x000;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"ʹ��͸��ͶӰ"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Perspective_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->projectionMethod = 0x012;
    currentArea->resizeGLArea();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// ������"ʹ������ͶӰ"��ť�ĵ���¼�
void WidgetsEvents::toolButton_Orthogonality_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->projectionMethod = 0x011;
    currentArea->resizeGLArea();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::toolButton_StatusMonitor_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->switchStatusMonitorEnable();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

// "����"�����˵���"��������Ϣ"��ť�ĵ���¼�
void WidgetsEvents::menuButton_Info_click()
{
	WidgetsEvents::toolButton_Info_click();
}

void WidgetsEvents::menuButton_Close_click()
{
    WidgetsEvents::toolButton_Close_click();
}

void WidgetsEvents::menuButton_Reload_click()
{
    WidgetsEvents::toolButton_Reload_click();
}

void WidgetsEvents::menuButton_Export_click() 
{
    WidgetsEvents::toolButton_Export_click();
}

void WidgetsEvents::menuButton_Snapshot_click()
{
    WidgetsEvents::toolButton_Snapshot_click();
}

void WidgetsEvents::menuButton_Bounding_click()
{
    WidgetsEvents::toolButton_Bounding_click();
}

void WidgetsEvents::menuButton_Points_click()
{
    WidgetsEvents::toolButton_Points_click();
}

void WidgetsEvents::menuButton_Wireframe_click()
{
    WidgetsEvents::toolButton_Wireframe_click();
}

void WidgetsEvents::menuButton_FlatLine_click()
{
    WidgetsEvents::toolButton_FlatLine_click();
}

void WidgetsEvents::menuButton_FlatFill_click()
{
    WidgetsEvents::toolButton_FlatFill_click();
}

void WidgetsEvents::menuButton_SmoothFill_click()
{
    WidgetsEvents::toolButton_SmoothFill_click();
}

void WidgetsEvents::menuButton_LightSource_click()
{
    WidgetsEvents::toolButton_LightSource_click();
}

void WidgetsEvents::menuButton_CenterPoint_click()
{
    WidgetsEvents::toolButton_CenterPoint_click();
}

void WidgetsEvents::menuButton_Coordinate_click()
{
    WidgetsEvents::toolButton_Coordinate_click();
}

void WidgetsEvents::menuButton_BoundingBox_click()
{
    WidgetsEvents::toolButton_BoundingBox_click();
}

void WidgetsEvents::menuButton_GradientBkg_click()
{
    WidgetsEvents::toolButton_GradientBkg_click();
}

void WidgetsEvents::menuButton_MeshBkg_click()
{
    WidgetsEvents::toolButton_MeshBkg_click();
}

void WidgetsEvents::menuButton_EmptyBkg_click()
{
    WidgetsEvents::toolButton_EmptyBkg_click();
}

void WidgetsEvents::menuButton_Perspective_click()
{
    WidgetsEvents::toolButton_Perspective_click();
}

void WidgetsEvents::menuButton_Orthogonality_click()
{
    WidgetsEvents::toolButton_Orthogonality_click();
}

void WidgetsEvents::menuButton_StatusMonitor_click()
{
    WidgetsEvents::toolButton_StatusMonitor_click();
}

void WidgetsEvents::menuButton_Windowed_click()
{
    /*
    GraphicsDocument* currentDoc = MainForm->getCurrentDoc();
    if (currentDoc != nullptr)
        currentDoc->showNormal();
    currentDoc = nullptr;
    */
    WidgetsEvents::menuButton_WindowedAll_click();
}

void WidgetsEvents::menuButton_WindowedAll_click()
{
    for (int i = WidgetsEvents::menuButton_gDocsList.size() - 1; i >= 0; i--)
        WidgetsEvents::menuButton_to_SubWindow.find(WidgetsEvents::menuButton_gDocsList[i])->second->correspondingSubWindow->showNormal();
}

void WidgetsEvents::menuButton_Maximize_click()
{
    GraphicsDocument* currentDoc = MainForm->getCurrentDoc();
    if (currentDoc != nullptr)
        currentDoc->showMaximized();
    currentDoc = nullptr;
}

void WidgetsEvents::menuButton_MaximizeAll_click()
{
    for (int i = WidgetsEvents::menuButton_gDocsList.size() - 1; i >= 0; i--)
        WidgetsEvents::menuButton_to_SubWindow.find(WidgetsEvents::menuButton_gDocsList[i])->second->correspondingSubWindow->showMaximized();
}

void WidgetsEvents::menuButton_Quit_click()
{
    MainForm->close();
}

void WidgetsEvents::menuButton_F_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_x = 0.0f;
    currentArea->rotateAngle_y = 0.0f;
    currentArea->rotateAngle_z = 0.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_B_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_x = 0.0f;
    currentArea->rotateAngle_y = 180.0f;
    currentArea->rotateAngle_z = 0.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_U_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_x = 90.0f;
    currentArea->rotateAngle_y = 0.0f;
    currentArea->rotateAngle_z = 0.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_D_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_x = -90.0f;
    currentArea->rotateAngle_y = 0.0f;
    currentArea->rotateAngle_z = 0.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_L_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_x = 0.0f;
    currentArea->rotateAngle_y = 90.0f;
    currentArea->rotateAngle_z = 0.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_R_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_x = 0.0f;
    currentArea->rotateAngle_y = -90.0f;
    currentArea->rotateAngle_z = 0.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_xAxisR_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_z += 90.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_xAxisL_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_z -= 90.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_yAxisR_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_x += 90.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_yAxisL_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_x -= 90.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_zAxisR_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_y += 90.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_zAxisL_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->rotateAngle_y -= 90.0f;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_CloseFilter_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->filterMode = 0x03F;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_Explosure_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->filterMode = 0x030;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_Integrated_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->filterMode = 0x031;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_Colorful_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->filterMode = 0x032;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_ColorfulX_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->filterMode = 0x033;
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_Rename_click()
{
    GDRenameDialog* renameDialog = new GDRenameDialog(MainForm->getCurrentDoc());
    renameDialog->getDocumentName();
}

// "�����߽���"����Ĺر��¼�������
void delegator WidgetsEvents::developerFormDispose() 
{
    developerInfoForm->close();
    developerInfoForm = nullptr;
}


void WidgetsEvents::toolButton_Save_click()
{
    GraphicsDocument* currentDoc = MainForm->getCurrentDoc();
    if (currentDoc == nullptr)
        return;
    WidgetsEvents::saveDocument(currentDoc);
    currentDoc->correspondingSubWindow->setWindowTitle(currentDoc->documentName);
}

void WidgetsEvents::menuButton_Save_click()
{
    WidgetsEvents::toolButton_Save_click();
}

void WidgetsEvents::menuButton_SaveAs_click()
{
    GraphicsDocument* currentDoc = MainForm->getCurrentDoc();
    if (currentDoc == nullptr)
        return;
    QString savePath = currentDoc->savePath;
    currentDoc->isSaved = false;
    WidgetsEvents::saveDocument(currentDoc);
    if (savePath != "")
        currentDoc->savePath = savePath;
}

void WidgetsEvents::menuButton_SaveAll_click()
{
    for (int i = WidgetsEvents::menuButton_gDocsList.size() - 1; i >= 0; i--)
        WidgetsEvents::saveDocument(WidgetsEvents::menuButton_to_SubWindow.find(WidgetsEvents::menuButton_gDocsList[i])->second);
}

void WidgetsEvents::saveDocument(GraphicsDocument* gDoc)
{
    QString filePath;
    if (gDoc->isSaved)
    {
        filePath = gDoc->savePath;
    }
    else
    {
        QFileDialog* saveFileDialog = new QFileDialog();
        filePath = saveFileDialog->getSaveFileName(MainForm, ch("����ͼ���ĵ�: ") + gDoc->documentName, "./" + MainForm->getCurrentDoc()->documentName + ".gdoc", ch("����ͼ���ĵ�(*.gdoc)"));
        if (filePath.isEmpty())
            return;
        gDoc->documentName = filePath.split('/').last();
        if (gDoc->documentName.endsWith(".gdoc"))
            gDoc->documentName = gDoc->documentName.mid(0, gDoc->documentName.length() - 5);
    }
    gDoc->isSaved = true;
    gDoc->savePath = filePath;
    GLArea* currentArea = gDoc->correspondingGLArea;
    std::ofstream out(filePath.toStdString().c_str(), std::ios::ate);
    out << gDoc->documentName.toStdString() << std::endl;
    out << gDoc->savePath.toStdString() << std::endl;
    out << gDoc->isSaved << std::endl;
    out << currentArea->parentWidth << std::endl;
    out << currentArea->parentHeight << std::endl;
    out << currentArea->renderMode << std::endl;
    out << currentArea->renderMethod << std::endl;
    out << currentArea->surfaceMode << std::endl;
    out << currentArea->backgroundMode << std::endl;
    out << currentArea->displayMode << std::endl;
    out << currentArea->filterMode << std::endl;
    out << currentArea->isShowAnchor << std::endl;
    out << currentArea->isShowCoordinate << std::endl;
    out << currentArea->isLightEnabled << std::endl;
    out << currentArea->isShowBoundingBox << std::endl;
    out << currentArea->isShowMonitor << std::endl;
    out << currentArea->meshColor.red << std::endl << currentArea->meshColor.green << std::endl << currentArea->meshColor.blue << std::endl;
    out << currentArea->lightColor.red << std::endl << currentArea->lightColor.green << std::endl << currentArea->lightColor.blue << std::endl;
    out << currentArea->rotateAngle_x << std::endl;
    out << currentArea->rotateAngle_y << std::endl;
    out << currentArea->rotateAngle_z << std::endl;
    out << currentArea->zoomRatio << std::endl;
    out << currentArea->naturalMag << std::endl;
    out << currentArea->moveX << std::endl;
    out << currentArea->moveY << std::endl;
    out << currentArea->moveZ << std::endl;
    if (currentArea->meshModel != nullptr)
    {
        out << "1" << std::endl;
        out << currentArea->meshModel->meshModelPath.toStdString() << std::endl;
    }
    else
        out << "0" << std::endl;
    gDoc->isEdited = false;
}

void WidgetsEvents::menuButton_Dense_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    int result = QMessageBox::warning(MainForm, ch("�������"), ch("��ģ�ͽ���������ܻ���ĵ��е�ģ�����ݲ���ֱ��Ӱ�죬�����Ҫ�ָ�ԭ�ȵ�ģ�������µ���ԭ�ȵ�ģ�ͣ��ڴ�֮ǰ����ȷ����ĸ����ѱ��ݡ�ȷ��Ҫ������"), ch("ȷ��"), ch("ȡ��"));
    if (result == QMessageBox::RejectRole)
        return;
    currentArea->meshModel->getDenseModel();
    currentArea->updateGL();
    QString filePath = currentArea->meshModel->meshModelPath.mid(0, currentArea->meshModel->meshModelPath.length() - 4) + "_Dense.obj";
    currentArea->meshModel->exportMesh(filePath.toStdString().c_str());
    currentArea->meshModel->meshModelPath = filePath;
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_Sparse_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    int result = QMessageBox::warning(MainForm, ch("����ϡ��"), ch("��ģ�ͽ�������ϡ�����ĵ��е�ģ�����ݲ���ֱ��Ӱ�죬�����Ҫ�ָ�ԭ�ȵ�ģ�������µ���ԭ�ȵ�ģ�ͣ��ڴ�֮ǰ����ȷ����ĸ����ѱ��ݡ�ȷ��Ҫ������"), ch("ȷ��"), ch("ȡ��"));
    if (result == QMessageBox::RejectRole)
        return;
    currentArea->meshModel->getSparseModel();
    currentArea->updateGL();
    QString filePath = currentArea->meshModel->meshModelPath.mid(0, currentArea->meshModel->meshModelPath.length() - 4) + "_Sparse.obj";
    currentArea->meshModel->exportMesh(filePath.toStdString().c_str());
    currentArea->meshModel->meshModelPath = filePath;
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}


void WidgetsEvents::toolButton_Function_click()
{
    GuideForm* guideForm = new GuideForm();
    guideForm->show();
}

void WidgetsEvents::menuButton_Function_click()
{
    WidgetsEvents::toolButton_Function_click();
}

void WidgetsEvents::toolButton_Color_click()
{
    QColorDialog* colorDialog = new QColorDialog();
    QColor color = colorDialog->getColor(Qt::red, MainForm);
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->lightColor.red = (float)color.red() / 255.0f;
    currentArea->lightColor.blue = (float)color.blue() / 255.0f;
    currentArea->lightColor.green = (float)color.green() / 255.0f;
    currentArea->updateLightResource();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::menuButton_Color_click() 
{
    WidgetsEvents::toolButton_Color_click();
}

void WidgetsEvents::lightOption_diffuse_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->isDiffuseAccepted = 1 - currentArea->isDiffuseAccepted;
    currentArea->updateLightResource();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::lightOption_Specular_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->isSpecularAccepted = 1 - currentArea->isSpecularAccepted;
    currentArea->updateLightResource();
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::lightOption_Positional_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->switchLightDirection(0);
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}


void WidgetsEvents::lightOption_Directional_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    currentArea->switchLightDirection(1);
    currentArea->updateGL();
    *currentArea->isEdited_delegator = true;
    currentArea = nullptr;
}

void WidgetsEvents::toolButton_LoadTexture_click()
{
    GLArea* currentArea = MainForm->getCurrentDoc()->correspondingGLArea;
    if (currentArea->meshModel->textures == 0)
    {
        QApplication::beep();
        QMessageBox::critical(MainForm, ch("����"), ch("��ģ�Ͳ�����������Ϣ���޷����䵼�����������"), ch("ȷ��"));
        return;
    }
    else 
    {
        QFileDialog* fileDialog = new QFileDialog();
        fileDialog->setAttribute(Qt::WA_DeleteOnClose);
        QString fileName = fileDialog->getOpenFileName(MainForm, ch("�򿪲���"), "./", ch("λͼ�ز��ļ�(*.bmp)"));
        if (fileName.isEmpty())
            ;
        else
        {
            bool isLoaded = currentArea->meshModel->LoadBMPTexture(fileName.toStdString().c_str());
            if (!isLoaded)
            {
                QMessageBox::critical(MainForm, ch("����"), ch("�򿪲�����Ч���յ�/����Ĳ��ʣ�"), ch("ȷ��"));
            }
        }
    }
}

void WidgetsEvents::menuButton_LoadTexture_click()
{
    WidgetsEvents::toolButton_LoadTexture_click();
}