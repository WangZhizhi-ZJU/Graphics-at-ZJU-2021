#ifndef GDRENAMEDIALOG_H
#define GDRENAMEDIALOG_H

#include <QWidget>
#include "ui_GDRenameDialog.h"
#include "GraphicsDocument.h"

class GraphicsDocument;

class GDRenameDialog : public QWidget
{
	Q_OBJECT

public:
    GraphicsDocument* correspondingDoc = nullptr;

	GDRenameDialog(GraphicsDocument* gDoc = nullptr, QWidget* parent = Q_NULLPTR);
    ~GDRenameDialog() {};
	
	void getDocumentName();

private slots:
    void toolButton_OK_click();
    void toolButton_Cancel_click();

private:
	Ui::GDRenameDialog ui;
};

#endif