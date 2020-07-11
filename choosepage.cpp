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
}
