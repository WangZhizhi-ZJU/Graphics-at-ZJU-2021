#ifndef GDRESCALEDIALOG_H
#define GDRESCALEDIALOG_H

#include <QWidget>
#include "ui_GDRescaleDialog.h"
#include "MeshModel.h"

class GDRescaleDialog : public QWidget
{
	Q_OBJECT

public:
    GDRescaleDialog(MeshModel* meshModel, QWidget* parent = Q_NULLPTR);
	~GDRescaleDialog();
    void pushButton_click();

private:
	Ui::GDRescaleDialog ui;
};

#endif