#include "UsageProgressBar.h"
#include "ui_UsageProgressBar.h"

UsageProgressBar::UsageProgressBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UsageProgressBar)
{
    ui->setupUi(this);
	ui->wProgress->resize(0, ui->wProgress->height());
}

UsageProgressBar::~UsageProgressBar()
{
    delete ui;
}

void UsageProgressBar::setProgress(int value)
{
    if(value>100) value = 100;
    if(value<5) value=5;

    this->progress = value;
	ui->wProgress->resize(value*3.6, ui->wProgress->height());
    if(value<97)
        ui->wProgress->setStyleSheet(QString::fromAscii("background-color: #78B241;"));
    else
        ui->wProgress->setStyleSheet(QString::fromAscii("background-color: #d90007;"));
}
