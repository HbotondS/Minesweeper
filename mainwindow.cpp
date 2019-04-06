#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "startwindow.h"

MainWindow::MainWindow(int xDimension, int yDimension):
    ui(new Ui::MainWindow),
    xDimension(xDimension),
    yDimension(yDimension)
{
    ui->setupUi(this);
    init();
    connectActions();
}

MainWindow::MainWindow(int xDimension, int yDimension, int numberOfBombs):
    ui(new Ui::MainWindow),
    xDimension(xDimension),
    yDimension(yDimension),
    numberOfBombs(numberOfBombs)
{
    ui->setupUi(this);
    init();
    connectActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectActions()
{
    connect(ui->action_New_game, &QAction::triggered, [this]{newGame();});
    connect(ui->action_Restart, &QAction::triggered, [this]{restart();});
    connect(ui->action_Exit, &QAction::triggered, []{QApplication::quit();});
    connect(this, SIGNAL(win()), this, SLOT(winmsg()));
}

void MainWindow::init() {
    timer = new QTimer();
    elapsedTime = new QElapsedTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(update_time()));

    QVBoxLayout* mainLayout = new QVBoxLayout();
    btnLayout = new QGridLayout();
    btnLayout->setSpacing(1);
    QHBoxLayout* labelLayout = new QHBoxLayout();

    bombsLabel = new QLabel(QString::number(numberOfBombs));
    labelLayout->addWidget(bombsLabel);
    timeLabel = new QLabel("");
    timeLabel->setAlignment(Qt::AlignRight);
    labelLayout->addWidget(timeLabel);

    mainLayout->addLayout(labelLayout);
    mainLayout->addLayout(btnLayout);

    newBtns = new QRightClickButton**[xDimension];
    cells = new Cell*[xDimension];
    for (int i = 0; i < xDimension; ++i) {
        newBtns[i] = new QRightClickButton*[yDimension];
        cells[i] = new Cell[yDimension];
    }
    fillmines();

    generateMines();
    for (int i = 0; i < xDimension; ++i)
    {
        for (int j = 0; j < yDimension; ++j)
        {
            newBtns[i][j] = new QRightClickButton();
            newBtns[i][j]->setMinimumSize(40, 40);
            newBtns[i][j]->setIconSize(QSize(40, 40));
            connect(newBtns[i][j], &QRightClickButton::clicked, [this, i, j]{btn_action(i, j);});
            connect(newBtns[i][j], &QRightClickButton::rightClicked, [this, i, j]{onRightClicked(i, j);});
            btnLayout->addWidget(newBtns[i][j], i, j);
        }
    }
    ui->centralWidget->setLayout(mainLayout);

    timer->start();
    elapsedTime->start();
}

/**
 * for debuging
 */
void MainWindow::printMines() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            qDebug() << cells[i][j].value << " ";
        }
        qDebug() << "\n";
    }
}

void MainWindow::newGame()
{
    this->close();
    StartWindow *w = new StartWindow();
    w->show();
}

void MainWindow::generateMines()
{
    srand(time(NULL));
    int x, y, currentMines = 0;
    while (currentMines != numberOfBombs)
    {
        x = rand() % xDimension;
        y = rand() % yDimension;

        if (cells[x][y].value != -1)
        {
            cells[x][y].value = -1;
            ++currentMines;
            countMines(x, y);
        }
    }
}

void MainWindow::incNum(int i, int j) {
    if (cells[i][j].value != -1) {
        ++cells[i][j].value;
    }
}

void MainWindow::countMines(int i, int j) {
    if (i-1 >= 0 && j-1 >= 0)
    {
        incNum(i-1, j-1);
    }
    if (i-1 >= 0)
    {
        incNum(i-1, j);
    }
    if (i-1 >= 0 && j+1 < yDimension)
    {
        incNum(i-1, j+1);
    }
    if (j-1 >= 0)
    {
        incNum(i, j-1);
    }
    if (j+1 < yDimension)
    {
        incNum(i, j+1);
    }
    if (i+1 < xDimension && j-1 >= 0)
    {
        incNum(i+1, j-1);
    }
    if (i+1 < xDimension)
    {
        incNum(i+1, j);
    }
    if (i+1 < xDimension && j+1 < yDimension)
    {
        incNum(i+1, j+1);
    }
}

/**
 * This method show all the mines, when the user click one of them.
 * @param x & @param y indicates the location of the clicked mine.
 */
void MainWindow::showMines(int x, int y)
{
    for (int i = 0; i < xDimension; ++i)
    {
        for (int j = 0; j < yDimension; ++j)
        {
            if (cells[i][j].value == -1)
            {
                if (i == x && j == y)
                {

                    QIcon mine(":/assets/mine_clicked.png");
                    newBtns[x][y]->setIcon(mine);
                } else
                {
                    QIcon mine(":/assets/mine.png");
                    newBtns[i][j]->setIcon(mine);
                }
            }
        }
    }
}

void MainWindow::clearField(int x, int y)
{
    if (cells[x][y].value == 0 && !cells[x][y].visited)
    {
        newBtns[x][y]->setStyleSheet("border: none;");
        cells[x][y].visited = true;
        if ((x-1) > -1)
            clearField(x-1, y);
        if ((y-1) > -1)
            clearField(x, y-1);
        if ((x+1) < xDimension)
            clearField(x+1, y);
        if ((y+1) < yDimension)
            clearField(x, y+1);
    }
    else
    {
        if (cells[x][y].value != 0)
        {
            newBtns[x][y]->setStyleSheet("border: none;");
            cells[x][y].visited = true;
            //newBtns[x][y]->setText(QString::number(cells[x][y].value));
            QIcon icon;
            switch (cells[x][y].value) {
            case 1: {
                icon.addFile(":/assets/1.png");
                break;
            }
            case 2: {
                icon.addFile(":/assets/2.png");
                break;
            }
            case 3: {
                icon.addFile(":/assets/3.png");
                break;
            }
            case 4: {
                icon.addFile(":/assets/4.png");
                break;
            }
            case 5: {
                icon.addFile(":/assets/5.png");
                break;
            }
            case 6: {
                icon.addFile(":/assets/6.png");
                break;
            }
            case 7: {
                icon.addFile(":/assets/7.png");
                break;
            }
            case 8: {
                icon.addFile(":/assets/8.png");
                break;
            }
            }
            newBtns[x][y]->setIcon(icon);
        }
    }
}

void MainWindow::fillmines()
{
    for (int i = 0; i < xDimension; ++i) {
        for (int j = 0; j < yDimension; ++j) {
            cells[i][j].value = 0;
            cells[i][j].visited = false;
            cells[i][j].flaged = false;
        }
    }
}

void MainWindow::btn_action(int x, int y)
{
    if (!cells[x][y].flaged) {
        auto isMine = cells[x][y].value;
        switch (isMine)
        {
            case -1:
            {
                timer->stop();
                showMines(x, y);
                QMessageBox messageBox;
                QMessageBox::StandardButton reply;
                reply = messageBox.critical(this, "Game over", "You lost!", QMessageBox::Retry | QMessageBox::Close);
                messageBox.setFixedSize(500,200);

                if (reply == QMessageBox::Retry) {
                    restart();
                } else {
                    QApplication::quit();
                }
                break;
            }
            case 0:
            {
                clearField(x, y);
                break;
            }
            default:
            {
                newBtns[x][y]->setIcon(QIcon(":/assets/1.png"));
                cells[x][y].visited = true;
            }
        }
    }

    if (btnsLeft() == numberOfBombs) {
        emit win();
    }
}

void MainWindow::update_time()
{
    timeLabel->setText(QString::number(elapsedTime->elapsed()/100));
}

void MainWindow::onRightClicked(int x, int y)
{
    int bombsLeft = bombsLabel->text().toInt();
    if (bombsLeft > 0 && !cells[x][y].flaged)
    {
        qDebug() << "flaged";
        bombsLabel->setText(QString::number(--bombsLeft));
        newBtns[x][y]->setIcon(QIcon(":/assets/flag.png"));
        cells[x][y].flaged = true;
    }
    else
    {
        if (cells[x][y].flaged)
        {
            bombsLabel->setText(QString::number(++bombsLeft));
            newBtns[x][y]->setIcon(QIcon());
            cells[x][y].flaged = false;
        }
    }
    if (bombsLeft == 0) {
        if (btnsLeft() == numberOfBombs) {
            emit win();
        }
    }
}

void MainWindow::winmsg()
{
    timer->stop();
    QMessageBox messageBox;
    QMessageBox::StandardButton reply;
    reply = messageBox.critical(this, "Win!", "Your time: " + timeLabel->text(), QMessageBox::Retry | QMessageBox::Close);
    messageBox.setFixedSize(500,200);

    if (reply == QMessageBox::Retry) {
        newGame();
    } else {
        QApplication::quit();
    }
}

void MainWindow::restart() {
    cells = new Cell*[xDimension];
    for (int i = 0; i < xDimension; ++i) {
        cells[i] = new Cell[yDimension];
    }
    fillmines();
    for (int i = 0; i < xDimension; ++i)
    {
        for (int j = 0; j < yDimension; ++j)
        {
            newBtns[i][j]->setText("");
            cells[i][j].visited = false;
            newBtns[i][j]->setIcon(QIcon()); // basicly remove the icon
            newBtns[i][j]->setStyleSheet("border-bottom: 2px solid #7B7B7B; \
                                         border-left: 2px solid #ffffff;    \
                                         border-top: 2px solid #ffffff;     \
                                         border-right: 2px solid #7B7B7B;");
        }
    }

    generateMines();
    timer->start();
    elapsedTime->start();
}

/**
 * @return return the number of cells which are not visited.
 */
int MainWindow::btnsLeft() {
    int temp = 0;
    for (int i = 0; i < xDimension; ++i) {
        for (int j = 0; j < yDimension; ++j) {
            if (!cells[i][j].visited) {
                ++temp;
                if (temp > numberOfBombs) {
                    return temp;
                }
            }
        }
    }

    return temp;
}
