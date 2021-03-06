#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdlib.h>
#include <QMainWindow>
#include "qrightclickbutton.h"
#include "QDebug"
#include "QGridLayout"
#include "QMessageBox"
#include "QLabel"
#include "QElapsedTimer"
#include "QTimer"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "qrightclickbutton.h"
#include "assets.h"

typedef struct {
    int value;
    bool visited;
    bool flaged;
}Cell;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int, int);
    MainWindow(int, int, int);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    int xDimension = 10;
    int yDimension = 10;
    int numberOfBombs;

    QLabel* bombsLabel;
    QLabel* timeLabel;

    QTimer* timer;
    QElapsedTimer* elapsedTime;

    QGridLayout* btnLayout;
    QRightClickButton*** newBtns;

    Assets *assets;

    /*
     * -1 <= mines
     *  0 <= no mines in neighbour
     *  1-6 <= no. of mines located in neighbour
     */
    Cell **cells;

    void init();
    void newGame();
    void generateMines();
    void countMines(int, int);
    void incNum(int, int);
    void showMines(int, int);
    void clearField(int, int);
    int btnsLeft();
    void printMines();
    void fillmines();
    void restart();
    void connectActions();
    void setIconNumber(int, int);

private slots:
    void btn_action(int, int);
    void update_time();

    void onRightClicked(int, int);

    void winmsg();
    void gameOverMsg();

signals:
    void win();
    void gameOver();
};

#endif // MAINWINDOW_H
