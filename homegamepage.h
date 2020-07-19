#ifndef HOMEGAMEPAGE_H
#define HOMEGAMEPAGE_H

#include <QTcpSocket>

#include "gamepage.h"

class HomeGamePage: public GamePage
{
public:
    HomeGamePage();
    ~HomeGamePage();

protected:
    void getCurrentPlayerImplement(const QVector<qint8>& subcmd) override;
    void initConnection() override;

private:
    QTcpSocket* fakeSocket;
};

#endif // HOMEGAMEPAGE_H
