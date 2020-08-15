#include "machinegamepage.h"

MachineGamePage::MachineGamePage(int color)
{
    color_ = color;
    fakeSocket = new QTcpSocket;
    fakeSocket->connectToHost("127.0.0.1", 10086);
    connect(fakeSocket, &QTcpSocket::disconnected, this, &MachineGamePage::loseConnection);
    sendColor();
}

MachineGamePage::~MachineGamePage(){
    delete fakeSocket;
}

void MachineGamePage::initConnection(){
    QMap<QString, QString> form;
    form["form_type"] = "1";
    form["version"] = "v0.3.0";
    form["game_mode"] = "2";
    sendForm(11, form);
}

void MachineGamePage::sendColor(){
    QVector<qint8> info;
    info.push_back(color_);
    sendInfo(12, info);
}
