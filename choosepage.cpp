#include "choosepage.h"
#include "ui_choosepage.h"

ChoosePage::ChoosePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChoosePage)
{
    ui->setupUi(this);
}

ChoosePage::~ChoosePage()
{
    delete ui;
    delete gamePage;
}

void ChoosePage::on_oneMachine_clicked()
{
    gamePage = new HomeGamePage;
    close();
    gamePage->show();
}

void ChoosePage::on_twoMachine_clicked()
{
    gamePage = new InternetGamePage;
    close();
    gamePage->show();
}

void ChoosePage::on_peopleVSMachine_clicked()
{
    QMessageBox msgBox(this);
    QString str = "该功能当前尚未开放！";
    msgBox.setText(str);
    msgBox.exec();
}

void ChoosePage::on_help_clicked()
{
    QMessageBox msgBox(this);
    QString str = "该功能当前尚未开放！";
    msgBox.setText(str);
    msgBox.exec();
}
