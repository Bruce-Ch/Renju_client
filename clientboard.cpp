#include "clientboard.h"
#include <QDebug>

ClientBoard::ClientBoard()
{
    for(int i = 0; i < 15; i ++){
        for(int j = 0; j < 15; j ++){
            color[i][j] = 2;
        }
    }
}

void ClientBoard::update(const QString& info){
    QChar tmp;
    int idx = 0;
    for(int i = 0; i < 15; i ++){
        for(int j = 0; j < 15; j ++){
            tmp = info.at(idx);
            if(tmp == '*'){
                color[i][j] = 2;
            } else if(tmp == 'O'){
                color[i][j] = 0;
            } else if(tmp == 'X'){
                color[i][j] = 1;
            } else {
                assert(0);
            }
            idx++;
        }
    }
}
