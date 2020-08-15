#ifndef CHOOSECOLOR_H
#define CHOOSECOLOR_H

#include <QDialog>

#include "machinegamepage.h"

namespace Ui {
class ChooseColor;
}

class ChooseColor : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseColor(QWidget *parent = nullptr);
    ~ChooseColor();

private slots:
    void on_black_clicked();

    void on_white_clicked();

private:
    Ui::ChooseColor *ui;
    MachineGamePage* machineGamePage;
};

#endif // CHOOSECOLOR_H
