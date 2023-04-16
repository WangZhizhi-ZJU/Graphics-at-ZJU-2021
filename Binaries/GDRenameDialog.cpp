#include "GDRenameDialog.h"
#include <QTextCodec>
#include <QMouseEvent>
#include <QMdiSubWindow>
#include <QAction>

// ÖÐÎÄUnicodeÂë·­ÒëÆ÷
QTextCodec* GDRename_translator = QTextCodec::codecForName("GBK");
#define ch(args) GDRename_translator->toUnicode((args))

GDRenameDialog::GDRenameDialog(GraphicsDocument* gDoc, QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    correspondingDoc = gDoc;
    this->setWindowModality(Qt::ApplicationModal);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowIcon(QIcon(":/GraphicsatZJU/Resources/ToolbarIcons/0x084.png"));
    this->setWindowFlags(windowFlags() & ~Qt::WindowMinimizeButtonHint);
    this->hide();
}

void GDRenameDialog::getDocumentName()
{
    ui.lineEdit->setText(correspondingDoc->documentName);
    this->show();
    ui.lineEdit->setFocus();
    ui.lineEdit->setSelection(0, ui.lineEdit->text().length());
}

#include <iostream>
#include <fstream>
#include <ostream>
void GDRenameDialog::toolButton_OK_click()
{
    correspondingDoc->documentName = ui.lineEdit->text();
    correspondingDoc->correspondingSubWindow->setWindowTitle(correspondingDoc->documentName);
    correspondingDoc->correspondingMenuButton->setText(correspondingDoc->documentName);
    this->close();
}

void GDRenameDialog::toolButton_Cancel_click()
{
    this->close();
}