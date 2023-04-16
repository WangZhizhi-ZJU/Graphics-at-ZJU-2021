#ifndef DEVELOPERINFOFORM_H
#define DEVELOPERINFOFORM_H

#include <QWidget>
#include "ui_DeveloperInfoForm.h"

class DeveloperInfoForm : public QWidget
{
	Q_OBJECT

public:
    QPixmap closeButton_Default;
    QPixmap closeButton_Hover;
    QPixmap closeButton_Pressed;

    bool isMouseDown = false;

	DeveloperInfoForm(QWidget *parent = Q_NULLPTR);
    ~DeveloperInfoForm() {};
	void initializeForm();

protected:
	virtual void closeEvent(QCloseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void closeButton_click();
    void closeButton_hover();
    void closeButton_release();

private:
	Ui::DeveloperInfoForm ui;
};

#endif