#include "snakegameview.h"
#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QTimer>

int main(int argc, char **argv) {
    QApplication App(argc, argv);

    QWidget centerwidget;
    centerwidget.resize(424, 600);
    centerwidget.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);
    centerwidget.setStyleSheet("background-color:  #578A34");

    QVBoxLayout layout(&centerwidget);
    centerwidget.setLayout(&layout);

    SnakeGame game;
    game.setFocus();
    layout.addWidget(&game, 100, Qt::AlignCenter);
    layout.setMargin(0);

    QLabel status;
    layout.addWidget(&status);
    status.setStyleSheet("color: darkviolet; font: bold 16px");
    status.setAlignment(Qt::AlignCenter);
    QTimer statusClear;
    statusClear.setInterval(1000);
    QObject::connect(&statusClear, &QTimer::timeout, [&]() {
        status.setText("");
    });
    statusClear.start();
    QObject::connect(&game, &SnakeGame::collisioned, [&]() {
        status.setText("Collsion!");
    });


    QWidget scoringArea;
    layout.addWidget(&scoringArea, 1);
    scoringArea.setStyleSheet("background-color: #4A752C");

    QHBoxLayout scoringLayout;
    scoringArea.setLayout(&scoringLayout);

    QLabel score;
    scoringLayout.addWidget(&score);
    score.setText("Score: 0");
    score.setAlignment(Qt::AlignCenter);
    score.setStyleSheet("color: white; font: bold 16px");
    QObject::connect(&game, &SnakeGame::scoreChanged, [&] {
        score.setText("Score: " + QString::number(game.score()));
    });

    game.setFocus();

    centerwidget.show();

    return App.exec();
}
