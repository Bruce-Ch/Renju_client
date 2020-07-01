#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QTcpSocket>
#include <tuple>
#include <string>
#include <sstream>
#include <QDebug>
#include <vector>

#include "clientboard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent* event) override;
private:
    void getCurrentPlayer();
    void getWinner();
    void updateChessBoard();
    void updateGameInfo();
    void paintChessBoard(QPainter& painter);
    void paintGoMark(QPainter& painter);
    void updateWindow();
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void getColor();
    void sendInfo(std::vector<qint8> info);
    std::pair<int, int> xy2idx(int x, int y);

private:
    Ui::MainWindow *ui;
    ClientBoard clientBoard;
    int row_ = 0, col_ = 0;
    int color_ = -1;
    QTcpSocket* client = nullptr;

private slots:
    void setTimeLabel();
    void go(int row, int col);
    void retract();
    void sueForPeace();
    void abort();
    void implementMessage();


    void on_retract_clicked();

    void on_sueForPeace_clicked();

    void on_abort_clicked();

signals:
    void timeToGo(int row, int col);
    void timeToRetract();
    void timeToSue();
    void timeToAbort();
};
#endif // MAINWINDOW_H
