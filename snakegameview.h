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

#ifndef SNAKEGAMEVIEW_H
#define SNAKEGAMEVIEW_H

#include <QTimer>
#include <QWidget>
#include <deque>
#include <mutex>

class SnakeGameBoard : public QWidget {
    Q_OBJECT
public:
    explicit SnakeGameBoard(QWidget *parent = nullptr);

    int blockHeight() const;
    void setBlockHeight(int blockHeight);

    int blockWidth() const;
    void setBlockWidth(int blockWidth);

    int rowCount() const;
    int columnCount() const;

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    int m_blockHeight = 32, m_blockWidth = 32;
};

class Snake : QObject {
    Q_OBJECT
public:
    Snake(QObject *parent = nullptr);

    enum Direction { Up, Down, Left, Right };
    void move();
    void grow();
    void reset();

    Direction direction() const;
    void setDirection(Direction direction);

    const std::deque<QPoint> &tail() const;
    QPoint head() const;

private:
    std::deque<QPoint> m_tail;
    QPoint m_head;
    Direction m_direction;

    void move_impl();
};

class SnakeGame : public SnakeGameBoard {
    Q_OBJECT
public:
    SnakeGame(QWidget *parent = nullptr);

    size_t score() const;
    void reset();

signals:
    void scoreChanged();
    void collisioned();

private:
    Snake m_snake;
    QPoint m_applePos;
    size_t m_score;
    QTimer m_timer;

    void resetApplePos();
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void updateGame();
    bool isCollision();
};

#endif // SNAKEGAMEVIEW_H
