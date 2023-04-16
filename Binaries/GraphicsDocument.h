#ifndef GRAPHICSDOCUMENT_H
#define GRAPHICSDOCUMENT_H

#include <QWidget>
#include "ui_GraphicsDocument.h"
#include "GLArea.h"
#include "GLInfo.h"

class GraphicsDocument : public QWidget
{
	Q_OBJECT

public:
    GLArea* correspondingGLArea = nullptr;
    QMdiSubWindow* correspondingSubWindow = nullptr;

    QLabel* closeButton = nullptr;
    QLabel* maximizeButton = nullptr;

    QString documentName = "";
    QString savePath = "";

    bool isSaved = false;
    bool isEdited = true;

	GraphicsDocument(QWidget *parent = Q_NULLPTR);
    ~GraphicsDocument() {
     };

    QAction* correspondingMenuButton = nullptr;
    void setCorrespondingMenuButton(QAction* menuButton) 
	{
        this->correspondingMenuButton = menuButton;
	}

signals:
    void focusedIn();

private:
    Ui::GraphicsDocument ui;
    
protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void changeEvent(QEvent* event) override;
    virtual void focusInEvent(QFocusEvent* event) override;
    virtual void focusOutEvent(QFocusEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void closeEvent(QCloseEvent* event);
};

#endif
