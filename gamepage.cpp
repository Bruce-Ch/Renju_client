#include "gamepage.h"
#include "ui_gamepage.h"

GamePage::GamePage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GamePage)
{
    ui->setupUi(this);
    QTimer* timer = new QTimer(this);
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, &GamePage::setTimeLabel);
    connect(timer, &QTimer::timeout, this, &GamePage::updateGameInfo);
    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);
    connect(this, &GamePage::timeToGo, this, &GamePage::go);
    connect(this, &GamePage::timeToRetract, this, &GamePage::retract);
    connect(this, &GamePage::timeToSue, this, &GamePage::sueForPeace);
    connect(this, &GamePage::timeToAbort, this, &GamePage::abort);
    client = new QTcpSocket(this);
    //client->connectToHost("127.0.0.1", 9999);
    client->connectToHost("39.106.78.242", 9999);
    //getColor();
    connect(client, &QTcpSocket::readyRead, this, &GamePage::implementMessage);
    connect(client, &QTcpSocket::disconnected, this, &GamePage::loseConnection);
}

GamePage::~GamePage()
{
    delete ui;
    delete client;
}

/*
void GamePage::getColor(){
    std::vector<qint8> info;
    info.push_back(5);
    sendInfo(info);
}
*/

void GamePage::getCurrentPlayer(){
    std::vector<qint8> info;
    info.push_back(7);
    sendInfo(info);
}

void GamePage::getWinner(){
    std::vector<qint8> info;
    info.push_back(8);
    sendInfo(info);
}

void GamePage::sendInfo(std::vector<qint8> info){
    QByteArray block;
    QDataStream clientStream(&block, QIODevice::ReadWrite);
    for(qint8 num: info){
        clientStream << num;
    }
    client->write(block);
}

void GamePage::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int side = qMin(int(ui->centralwidget->width() - ui->buttons->width()), ui->centralwidget->height());
    painter.scale(side / 528.0, side / 528.0);
    paintChessBoard(painter);
    paintGoMark(painter);
    paintLastMark(painter);
    painter.end();
}

void GamePage::paintChessBoard(QPainter& painter){
    for(int i = 0; i < 14; i ++){
        for(int j = 0; j < 14; j ++){
            painter.drawRect(50 + j * 32, 50 + i * 32, 32, 32);
        }
    }

    for(int i = 0; i < 15; i ++){
        for(int j = 0; j < 15; j ++){
            if(clientBoard.color[i][j] == 2){
                continue;
            }
            painter.save();
            if(clientBoard.color[i][j] == 0){
                painter.setBrush(QColor("white"));
            } else if(clientBoard.color[i][j] == 1){
                painter.setBrush(QColor("black"));
            }
            int x = 32 * j + 38;
            int y = 32 * i + 38;
            painter.drawEllipse(x, y, 24, 24);
            painter.restore();
        }
    }
}

void GamePage::paintGoMark(QPainter& painter){
    if(row_ == 0 && col_ == 0){ return; }
    painter.save();
    painter.setBrush(QColor(color_ ? "black" : "white"));
    painter.drawEllipse(32 * (col_ - 1) + 46, 32 * (row_ - 1) + 46, 8, 8);
    painter.restore();
}

void GamePage::paintLastMark(QPainter &painter){
    if(lastColor == -1){ return; }
    painter.save();
    painter.setBrush(QColor(lastColor ? "white" : "black"));
    painter.drawEllipse(32 * (lastCol - 1) + 46, 32 * (lastRow - 1) + 46, 8, 8);
    painter.restore();

}

void GamePage::updateWindow(){
    this->update();
}

void GamePage::updateGameInfo(){
    getCurrentPlayer();
    getWinner();
    updateChessBoard();
    updateLastInfo();
}

void GamePage::updateChessBoard(){
    std::vector<qint8> info;
    info.push_back(6);
    sendInfo(info);
}

void GamePage::updateLastInfo(){
    std::vector<qint8> info;
    info.push_back(9);
    sendInfo(info);
}

void GamePage::setTimeLabel(){
    QTime time = QTime::currentTime();
    QString hour, min, sec;
    hour.setNum(time.hour());
    min.setNum(time.minute());
    sec.setNum(time.second());
    hour += ":";
    hour += min;
    hour += ":";
    hour += sec;
    ui->timeLabel->setText(hour);
}

void GamePage::mouseMoveEvent(QMouseEvent* event){
    int x = event->x(), y = event->y();
    std::tie(x, y) = getRealPoint(x, y);
    int row, col;
    std::tie(row, col) = xy2idx(x, y);
    row_ = row;
    col_ = col;
    update();
}

void GamePage::mouseReleaseEvent(QMouseEvent *event){
    if(row_ == 0 && col_ == 0){ return; }
    emit timeToGo(row_, col_);
}

void GamePage::go(int row, int col){
    std::vector<qint8> info;
    info.push_back(1);
    info.push_back(color_);
    info.push_back(row);
    info.push_back(col);
    sendInfo(info);
}

void GamePage::retract(){
    std::vector<qint8> info;
    info.push_back(2);
    info.push_back(color_);
    sendInfo(info);
}

void GamePage::sueForPeace(){
    std::vector<qint8> info;
    info.push_back(3);
    info.push_back(color_);
    sendInfo(info);
}

void GamePage::abort(){
    std::vector<qint8> info;
    info.push_back(4);
    info.push_back(color_);
    sendInfo(info);
}

void GamePage::loseConnection(){
    ui->currentPlayer->setText("You have lost the connection!");
}

void GamePage::implementMessage(){
    QByteArray array = client->readAll();
    QDataStream clientstream(&array, QIODevice::ReadWrite);
    qint8 cmd;
    while(true){
        clientstream >> cmd;
        switch (cmd) {
        case 0:
            return;
        case 1:
            clientstream >> cmd;
            if(cmd){
                clientstream >> cmd >> cmd >> cmd;
            }
            updateGameInfo();
            break;
        case 2:
            updateGameInfo();
            clientstream >> cmd;
            if(!cmd){
            } else if(cmd == 1){
                QMessageBox msgBox;
                QString info = "The other player don't allow you to retract.";
                msgBox.setText(info);
                msgBox.exec();
            } else if(cmd == 2){
                QMessageBox msgBox;
                QString info = "There is nothing to retract.";
                msgBox.setText(info);
                msgBox.exec();
            } else {
                qDebug() << "There is something wrong when trying to retract.";
            }
            break;
        case 3:
            updateGameInfo();
            clientstream >> cmd;
            if(!cmd){
            } else if(cmd == 1){
                QMessageBox msgBox;
                QString info = "The other player do not want a tie.";
                msgBox.setText(info);
                msgBox.exec();
            } else {
                qDebug() << "There is something wrong when suing for peace.";
            }
            break;
        case 4:
            updateGameInfo();
            clientstream >> cmd;
            if(!cmd){
            } else if(cmd == 1){
                QMessageBox msgBox;
                QString info = "You cannot abort now.";
                msgBox.setText(info);
                msgBox.exec();
            } else {
                qDebug() << "There is something wrong when aborting.";
            }
            break;
        case 5:
            clientstream >> cmd;
            color_ = cmd;
            break;
        case 6:{
            QString info;
            clientstream >> info;
            clientBoard.update(info);
            update();
            break;
        }
        case 7:{
            clientstream >> cmd;
            QString text = "Current Player: ";
            text += (cmd ? "Black" : "White");
            ui->currentPlayer->setText(text);
            break;
        }
        case 8:{
            clientstream >> cmd;
            if(cmd == -1){
                break;
            }
            QMessageBox msgBox;
            if(cmd == 2){
                QString info = "Game over with a tie.";
                msgBox.setText(info);
            } else {
                QString info = "The winner is ";
                info += (cmd ? "black" : "white");
                info += ".";
                msgBox.setText(info);
            }
            msgBox.exec();
            exit(0);
            break;
        }
        case 9:
            clientstream >> cmd;
            lastColor = cmd;
            clientstream >> cmd;
            lastRow = cmd;
            clientstream >> cmd;
            lastCol = cmd;
            update();
            break;
        default:
            qDebug() << "Unknown command: " << cmd;
        }
    }
}

std::pair<int, int> GamePage::xy2idx(int x, int y){
    if(x > 510 || x < 38 || y > 510 || y < 38){
        return std::make_pair(0, 0);
    }
    int row = (y - 38) / 32;
    int col = (x - 38) / 32;
    if((x - (col * 32 + 50)) * (x - (col * 32 + 50)) + (y - (row * 32 + 50)) * (y - (row * 32 + 50)) <= 144){
        return std::make_pair(row + 1, col + 1);
    } else {
        return std::make_pair(0, 0);
    }
}

std::pair<int, int> GamePage::getRealPoint(int x, int y){
    double rx, ry;
    double side = qMin(int(ui->centralwidget->width() - ui->buttons->width()), ui->centralwidget->height());
    rx = x / side * 528.0;
    ry = y / side * 528.0;
    return std::make_pair(int(rx), int(ry));
}

void GamePage::on_retract_clicked()
{
    emit timeToRetract();
}

void GamePage::on_sueForPeace_clicked()
{
    emit timeToSue();
}

void GamePage::on_abort_clicked()
{
    emit timeToAbort();
}
