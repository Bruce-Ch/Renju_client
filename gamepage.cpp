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
    client->connectToHost("127.0.0.1", 10086);
    //client->connectToHost("39.106.78.242", 10086);
    connect(client, &QTcpSocket::connected, this, &GamePage::initConnection);
    connect(client, &QTcpSocket::readyRead, this, &GamePage::implementMessage);
    connect(client, &QTcpSocket::disconnected, this, &GamePage::loseConnection);
    qRegisterMetaType<QVector<int> >("QVector<int>");
}

GamePage::~GamePage()
{
    delete ui;
    delete client;
}

void GamePage::initConnection(){
    QMap<QString, QString> form;
    form["form_type"] = "1";
    form["version"] = "v0.3.0";
    form["game_mode"] = "3";
    sendForm(11, form);
}

void GamePage::getCurrentPlayer(){
    QVector<qint8> info;
    sendInfo(7, info);
}

void GamePage::getWinner(){
    QVector<qint8> info;
    sendInfo(8, info);
}

void GamePage::sendInfo(qint8 cmd, const QVector<qint8>& info){
    QByteArray block;
    QDataStream clientStream(&block, QIODevice::ReadWrite);
    clientStream << cmd << info;
    client->write(block);
}

void GamePage::sendForm(qint8 cmd, const QMap<QString, QString>& form){
    QByteArray block;
    QDataStream clientStream(&block, QIODevice::ReadWrite);
    clientStream << cmd << form;
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
    for (int i = 0; i < 15; i++) {
        painter.drawLine(50 + i * 32, 50, 50 + i * 32, 498);
        painter.drawLine(50, 50 + i * 32, 498, 50 + i * 32);
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
    QVector<qint8> info;
    sendInfo(6, info);
}

void GamePage::updateLastInfo(){
    QVector<qint8> info;
    sendInfo(9, info);
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
    QVector<qint8> info;
    info.push_back(color_);
    info.push_back(row);
    info.push_back(col);
    sendInfo(1, info);
}

void GamePage::retract(){
    QVector<qint8> info;
    info.push_back(color_);
    sendInfo(2, info);
}

void GamePage::sueForPeace(){
    QVector<qint8> info;
    info.push_back(color_);
    sendInfo(3, info);
}

void GamePage::abort(){
    QVector<qint8> info;
    info.push_back(color_);
    sendInfo(4, info);
}

void GamePage::loseConnection(){
    ui->currentPlayer->setText("You have lost the connection!");
}

void GamePage::goImplement(const QVector<qint8>& subcmd){
    updateGameInfo();
}

void GamePage::retractImplement(const QVector<qint8>& subcmd){
    updateGameInfo();
    if(!subcmd[0]){
    } else if(subcmd[0] == 1){
        QMessageBox msgBox;
        QString info = "The other player don't allow you to retract.";
        msgBox.setText(info);
        msgBox.exec();
    } else if(subcmd[0] == 2){
        QMessageBox msgBox;
        QString info = "There is nothing to retract.";
        msgBox.setText(info);
        msgBox.exec();
    } else {
        qDebug() << "There is something wrong when trying to retract.";
    }
}

void GamePage::sueForPeaceImplement(const QVector<qint8>& subcmd){
    updateGameInfo();
    if(!subcmd[0]){
    } else if(subcmd[0] == 1){
        QMessageBox msgBox;
        QString info = "The other player do not want a tie.";
        msgBox.setText(info);
        msgBox.exec();
    } else {
        qDebug() << "There is something wrong when suing for peace.";
    }
}

void GamePage::abortImplement(const QVector<qint8>& subcmd){
    updateGameInfo();
    if(!subcmd[0]){
    } else if(subcmd[0] == 1){
        QMessageBox msgBox;
        QString info = "You cannot abort now.";
        msgBox.setText(info);
        msgBox.exec();
    } else {
        qDebug() << "There is something wrong when aborting.";
    }
}

void GamePage::getColorImplement(const QVector<qint8>& subcmd){
    color_ = subcmd[0];
}

void GamePage::updateChessBoardImplement(const QString& info){
    clientBoard.update(info);
    update();
}

void GamePage::getCurrentPlayerImplement(const QVector<qint8>& subcmd){
    QString text = "Current Player: ";
    text += (subcmd[0] ? "Black" : "White");
    ui->currentPlayer->setText(text);
}

void GamePage::getWinnerImplement(const QVector<qint8>& subcmd){
    if(subcmd[0] == -1){
        return;
    }
    QMessageBox msgBox;
    if(subcmd[0] == 2){
        QString info = "Game over with a tie.";
        msgBox.setText(info);
    } else {
        QString info = "The winner is ";
        info += (subcmd[0] ? "black" : "white");
        info += ".";
        msgBox.setText(info);
    }
    msgBox.exec();
    exit(0);
}

void GamePage::updateLastImplement(const QVector<qint8>& subcmd){
    lastColor = subcmd[0];
    lastRow = subcmd[1];
    lastCol = subcmd[2];
    update();
}

void GamePage::errorImplement(const QVector<qint8> &errorNumber){
    for(int en: errorNumber){
        switch (en) {
        case 0:{
            break;
        }
        case 1:{
            QMessageBox msgBox;
            QString info = "The version is too low. Please download the newest release.";
            msgBox.setText(info);
            msgBox.exec();
            exit(1);
        }
        case 2:{
            qDebug() << "There is a syntax error.";
            break;
        }
        default:{
            qDebug() << "Unknown error number: " << en;
        }
        }
    }
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
        case 1:{
            QVector<qint8> subcmd;
            clientstream >> subcmd;
            goImplement(subcmd);
            break;
        }
        case 2:{
            QVector<qint8> subcmd;
            clientstream >> subcmd;
            retractImplement(subcmd);
            break;
        }
        case 3:{
            QVector<qint8> subcmd;
            clientstream >> subcmd;
            sueForPeaceImplement(subcmd);
            break;
        }
        case 4:{
            QVector<qint8> subcmd;
            clientstream >> subcmd;
            abortImplement(subcmd);
            break;
        }
        case 5:{
            QVector<qint8> subcmd;
            clientstream >> subcmd;
            getColorImplement(subcmd);
            break;
        }
        case 6:{
            QString info;
            clientstream >> info;
            updateChessBoardImplement(info);
            break;
        }
        case 7:{
            QVector<qint8> subcmd;
            clientstream >> subcmd;
            getCurrentPlayerImplement(subcmd);
            break;
        }
        case 8:{
            QVector<qint8> subcmd;
            clientstream >> subcmd;
            getWinnerImplement(subcmd);
            break;
        }
        case 9:{
            QVector<qint8> subcmd;
            clientstream >> subcmd;
            updateLastImplement(subcmd);
            break;
        }
        case 11:{
            QVector<qint8> errorNumber;
            clientstream >> errorNumber;
            errorImplement(errorNumber);
            break;
        }
        case 12:{
            QVector<qint8> subcmd;
            clientstream >> subcmd;
            break;
        }
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
