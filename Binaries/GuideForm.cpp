#include "GuideForm.h"
#include <QAbstractItemView>
#include <QHeaderView>

GuideForm::GuideForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowMinimizeButtonHint);
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui.tableWidget->setColumnWidth(0, 147);
    ui.tableWidget->setColumnWidth(1, 145);
    for (int i = 0; i < 67; i++) {
        ui.tableWidget->setRowHeight(i, 25);
        ui.tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui.tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
}