#include "homegamepage.h"

HomeGamePage::HomeGamePage()
{
    fakeSocket = new QTcpSocket;
    //fakeSocket->connectToHost("127.0.0.1", 10086);
    fakeSocket->connectToHost("39.106.78.242", 10086);
    connect(fakeSocket, &QTcpSocket::disconnected, this, &HomeGamePage::loseConnection);
}

HomeGamePage::~HomeGamePage(){
    delete fakeSocket;
}

void HomeGamePage::getCurrentPlayerImplement(const QVector<qint8>& subcmd){
    color_ = subcmd[0];
    GamePage::getCurrentPlayerImplement(subcmd);
}
