#pragma once

#include <QWidget>
#include "ui_WelcomeForm.h"

class WelcomeForm : public QWidget
{
	Q_OBJECT

public:
    QGraphicsScene* scene;

	WelcomeForm(QWidget *parent = Q_NULLPTR);
    ~WelcomeForm() { delete scene; };
	void display();
    void initializeForm();

private:
	Ui::WelcomeForm ui;

private slots:
	void welcomeFormQuit();
};
