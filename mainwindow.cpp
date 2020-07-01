#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTimer* timer = new QTimer(this);
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, &MainWindow::setTimeLabel);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateGameInfo);
    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);
    connect(this, &MainWindow::timeToGo, this, &MainWindow::go);
    connect(this, &MainWindow::timeToRetract, this, &MainWindow::retract);
    connect(this, &MainWindow::timeToSue, this, &MainWindow::sueForPeace);
    connect(this, &MainWindow::timeToAbort, this, &MainWindow::abort);
    client = new QTcpSocket(this);
    //client->connectToHost("127.0.0.1", 9997);
    client->connectToHost("39.106.78.242", 9999);
    getColor();
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::implementMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete client;
}

void MainWindow::getColor(){
    std::vector<qint8> info;
    info.push_back(5);
    sendInfo(info);
}

void MainWindow::getCurrentPlayer(){
    std::vector<qint8> info;
    info.push_back(7);
    sendInfo(info);
}

void MainWindow::getWinner(){
    std::vector<qint8> info;
    info.push_back(8);
    sendInfo(info);
}

void MainWindow::sendInfo(std::vector<qint8> info){
    QByteArray block;
    QDataStream clientStream(&block, QIODevice::ReadWrite);
    for(qint8 num: info){
        clientStream << num;
    }
    client->write(block);
}

void MainWindow::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    paintChessBoard(painter);
    paintGoMark(painter);
    paintLastMark(painter);
    painter.end();
}

void MainWindow::paintChessBoard(QPainter& painter){
    for(int i = 0; i < 14; i ++){
        for(int j = 0; j < 14; j ++){
            painter.drawRect(92 + j * 32, 92 + i * 32, 32, 32);
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
            int x = 32 * j + 80;
            int y = 32 * i + 80;
            painter.drawEllipse(x, y, 24, 24);
            painter.restore();
        }
    }
}

void MainWindow::paintLastMark(QPainter &painter){
    if(lastColor == -1){ return; }
    painter.save();
    painter.setBrush(QColor(lastColor ? "white" : "black"));
    painter.drawEllipse(32 * (lastCol - 1) + 88, 32 * (lastRow - 1) + 88, 8, 8);
    painter.restore();

}

void MainWindow::updateWindow(){
    this->update();
}

void MainWindow::updateGameInfo(){
    getCurrentPlayer();
    getWinner();
    updateChessBoard();
    updateLastInfo();
}

void MainWindow::updateChessBoard(){
    std::vector<qint8> info;
    info.push_back(6);
    sendInfo(info);
}

void MainWindow::updateLastInfo(){
    std::vector<qint8> info;
    info.push_back(9);
    sendInfo(info);
}

void MainWindow::setTimeLabel(){
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

void MainWindow::mouseMoveEvent(QMouseEvent* event){
    int x = event->x(), y = event->y();
    int row, col;
    std::tie(row, col) = xy2idx(x, y);
    row_ = row;
    col_ = col;
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if(row_ == 0 && col_ == 0){ return; }
    emit timeToGo(row_, col_);
}

void MainWindow::go(int row, int col){
    std::vector<qint8> info;
    info.push_back(1);
    info.push_back(color_);
    info.push_back(row);
    info.push_back(col);
    sendInfo(info);
}

void MainWindow::retract(){
    std::vector<qint8> info;
    info.push_back(2);
    info.push_back(color_);
    sendInfo(info);
}

void MainWindow::sueForPeace(){
    std::vector<qint8> info;
    info.push_back(3);
    info.push_back(color_);
    sendInfo(info);
}

void MainWindow::abort(){
    std::vector<qint8> info;
    info.push_back(4);
    info.push_back(color_);
    sendInfo(info);
}

void MainWindow::implementMessage(){
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

void MainWindow::paintGoMark(QPainter& painter){
    if(row_ == 0 && col_ == 0){ return; }
    painter.save();
    painter.setBrush(QColor(color_ ? "black" : "white"));
    painter.drawEllipse(32 * (col_ - 1) + 88, 32 * (row_ - 1) + 88, 8, 8);
    painter.restore();
}

std::pair<int, int> MainWindow::xy2idx(int x, int y){
    if(x > 552 || x < 80 || y > 552 || y < 80){
        return std::make_pair(0, 0);
    }
    int row = (y - 80) / 32;
    int col = (x - 80) / 32;
    if((x - (col * 32 + 92)) * (x - (col * 32 + 92)) + (y - (row * 32 + 92)) * (y - (row * 32 + 92)) <= 144){
        return std::make_pair(row + 1, col + 1);
    } else {
        return std::make_pair(0, 0);
    }
}

void MainWindow::on_retract_clicked()
{
    emit timeToRetract();
}

void MainWindow::on_sueForPeace_clicked()
{
    emit timeToSue();
}

void MainWindow::on_abort_clicked()
{
    emit timeToAbort();
}
