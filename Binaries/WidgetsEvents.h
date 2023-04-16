#ifndef WIDGETSEVENTS_H
#define WIDGETSEVENTS_H

#include <QtWidgets/QMainWindow>
#include <QMdiSubWindow>
#include "GraphicsDocument.h"
#include <unordered_map>

#define delegator

class WidgetsEvents : public QObject
{
	Q_OBJECT
public:
    static std::vector<QAction*> menuButton_gDocsList;
    static std::unordered_map<QAction*, GraphicsDocument*> menuButton_to_SubWindow;
    static std::unordered_map<GraphicsDocument*, QAction*> subWindow_to_MenuButton;
    void saveDocument(GraphicsDocument* gDoc);

	WidgetsEvents() {};
	~WidgetsEvents() {};
    static void delegator developerFormDispose();
    static void menuButton_gDoc_setOtherUnchecked(QAction* menuButton);

private slots:
    void toolButton_Open_click();
    void toolButton_Info_click();
	void toolButton_Close_click();
    void toolButton_Color_click();
	void toolButton_New_click();
	void toolButton_Reload_click();
	void toolButton_Export_click();
	void toolButton_Snapshot_click();
	void toolButton_Bounding_click();
	void toolButton_Points_click();
    void toolButton_Wireframe_click();
    void toolButton_FlatLine_click();
    void toolButton_FlatFill_click();
    void toolButton_SmoothFill_click();
    void toolButton_LightSource_click();
    void toolButton_CenterPoint_click();
    void toolButton_Coordinate_click();
    void toolButton_BoundingBox_click();
    void toolButton_GradientBkg_click();
    void toolButton_MeshBkg_click();
    void toolButton_EmptyBkg_click();
    void toolButton_Perspective_click();
    void toolButton_Orthogonality_click();
    void toolButton_StatusMonitor_click();
	void menuButton_Open_click();
    void menuButton_Info_click();
    void menuButton_New_click();
    void menuButton_Close_click();
    void menuButton_CloseAll_click();
    void menuButton_Reload_click();
    void menuButton_Export_click();
    void menuButton_Snapshot_click();
    void menuButton_Bounding_click();
    void menuButton_Points_click();
    void menuButton_Wireframe_click();
    void menuButton_FlatLine_click();
    void menuButton_FlatFill_click();
    void menuButton_SmoothFill_click();
    void menuButton_LightSource_click();
    void menuButton_CenterPoint_click();
    void menuButton_Coordinate_click();
    void menuButton_BoundingBox_click();
    void menuButton_GradientBkg_click();
    void menuButton_MeshBkg_click();
    void menuButton_EmptyBkg_click();
    void menuButton_Perspective_click();
    void menuButton_Orthogonality_click();
    void menuButton_StatusMonitor_click();
    void menuButton_Windowed_click();
    void menuButton_WindowedAll_click();
    void menuButton_Quit_click();
    void menuButton_Maximize_click();
    void menuButton_MaximizeAll_click();
    void menuButton_F_click();
    void menuButton_B_click();
    void menuButton_U_click();
    void menuButton_D_click();
    void menuButton_L_click();
    void menuButton_R_click();
    void menuButton_xAxisR_click();
    void menuButton_yAxisR_click();
    void menuButton_zAxisR_click();
    void menuButton_xAxisL_click();
    void menuButton_yAxisL_click();
    void menuButton_zAxisL_click();
    void menuButton_CloseFilter_click();
    void menuButton_Explosure_click();
    void menuButton_Integrated_click();
    void menuButton_Color_click();
    void menuButton_Colorful_click();
    void menuButton_ColorfulX_click();
	void menuButton_gDoc_click();
    void subWindow_focusedIn();
    void menuButton_Rename_click();

    void menuButton_Save_click();
    void menuButton_SaveAs_click();
    void menuButton_SaveAll_click();
    void toolButton_Save_click();

    void menuButton_Dense_click();
    void menuButton_Sparse_click();

    void toolButton_Function_click();
    void menuButton_Function_click();

    void toolButton_BoundFill_click();
    void toolButton_DotFill_click();
    void menuButton_BoundFill_click();
    void menuButton_DotFill_click();

    void lightOption_diffuse_click();
    void lightOption_Specular_click();

    void lightOption_Positional_click();
    void lightOption_Directional_click();

    void toolButton_LoadTexture_click();
    void menuButton_LoadTexture_click();
};

#endif /* WIDGETSEVENTS_H */
