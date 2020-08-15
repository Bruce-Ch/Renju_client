#include "choosecolor.h"
#include "ui_choosecolor.h"

ChooseColor::ChooseColor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseColor)
{
    ui->setupUi(this);
}

ChooseColor::~ChooseColor()
{
    delete ui;
    delete machineGamePage;
}

void ChooseColor::on_black_clicked()
{
    machineGamePage = new MachineGamePage(1);
    close();
    machineGamePage->show();
}

void ChooseColor::on_white_clicked()
{
    machineGamePage = new MachineGamePage(0);
    close();
    machineGamePage->show();
}
