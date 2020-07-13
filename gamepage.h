#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QTcpSocket>
#include <QVector>
#include <tuple>
#include <string>
#include <sstream>
#include <QDebug>
#include <vector>

#include "clientboard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GamePage; }
QT_END_NAMESPACE

class GamePage : public QMainWindow
{
    Q_OBJECT

public:
    GamePage(QWidget *parent = nullptr);
    virtual ~GamePage();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void getCurrentPlayer();
    void getWinner();
    //void getColor();

    void updateChessBoard();
    void updateGameInfo();
    void updateLastInfo();

    void paintChessBoard(QPainter& painter);
    void paintGoMark(QPainter& painter);
    void paintLastMark(QPainter& painter);

    void sendInfo(qint8 cmd, const QVector<qint8>& info);
    void updateWindow();

    virtual void goImplement(const QVector<qint8>& subcmd);
    virtual void retractImplement(const QVector<qint8>& subcmd);
    virtual void sueForPeaceImplement(const QVector<qint8>& subcmd);
    virtual void abortImplement(const QVector<qint8>& subcmd);
    virtual void getColorImplement(const QVector<qint8>& subcmd);
    virtual void updateChessBoardImplement(const QString& info);
    virtual void getCurrentPlayerImplement(const QVector<qint8>& subcmd);
    virtual void getWinnerImplement(const QVector<qint8>& subcmd);
    virtual void updateLastImplement(const QVector<qint8>& subcmd);

    std::pair<int, int> xy2idx(int x, int y);
    std::pair<int, int> getRealPoint(int x, int y);

private:
    Ui::GamePage *ui;
    ClientBoard clientBoard;
    int row_ = 0, col_ = 0;
    QTcpSocket* client = nullptr;
    int lastColor = -1, lastRow = 0, lastCol = 0;

protected:
    int color_ = -1;

protected slots:
    void setTimeLabel();
    void go(int row, int col);
    void retract();
    void sueForPeace();
    void abort();
    void implementMessage();
    void loseConnection();


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
