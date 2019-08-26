/*
 * Copyright (C) 2019  Prince Gupta <jagannatharjun11@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If
 * you modify file(s), you may extend this exception to your version of the
 * file(s), but you are not obligated to do so. If you do not wish to do so,
 * delete this exception statement from your version.
 */
 
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
    status.setStyleSheet("color: red; font: 16px");
    status.setAlignment(Qt::AlignCenter);
    QTimer statusClear;
    statusClear.setInterval(4000);
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
