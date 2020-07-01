#ifndef CLIENTBOARD_H
#define CLIENTBOARD_H

#include <QString>
#include <QIODevice>
#include <QDataStream>
#include <sstream>

class ClientBoard
{
    int color[15][15];
public:
    ClientBoard();
    void update(const QString& info);
    friend class MainWindow;
};

#endif // CLIENTBOARD_H
