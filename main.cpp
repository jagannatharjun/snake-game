#include "snakegameview.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>

int main(int argc, char **argv) {
    QApplication App(argc, argv);

    QMainWindow win;
    QWidget centerwidget;
    win.setCentralWidget(&centerwidget);
    QHBoxLayout layout;
    centerwidget.setLayout(&layout);

    SnakeGameBoard board;
    Snake snake;
    SnakeGame game(&board, &snake);
    layout.addWidget(&board);

    win.resize(600, 400);
    game.setFocus();
    win.show();

    return App.exec();
}
