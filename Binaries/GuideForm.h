#ifndef GUIDEFORM_H
#define GUIDEFORM_H

#include <QWidget>
#include "ui_GuideForm.h"

class GuideForm : public QWidget
{
	Q_OBJECT

public:
	GuideForm(QWidget *parent = Q_NULLPTR);
    ~GuideForm(){};

private:
	Ui::GuideForm ui;
};

#endif