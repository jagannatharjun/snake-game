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
