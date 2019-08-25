#include "snakegameview.h"
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>

SnakeGameBoard::SnakeGameBoard(QWidget *parent) : QWidget(parent) {}

void SnakeGameBoard::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setPen(Qt::transparent);
    p.setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);
    auto r = rect();
    auto rc = rowCount();
    auto cc = columnCount();
    for (int i = 0; i < rc; i++) {
        for (int j = 0; j < cc; j++) {
            p.setBrush(
                QBrush((i + j) % 2 ? QColor("#A7D948") : QColor("#8ECC39")));
            p.drawRect(QRect(QPoint(m_blockWidth * j, i * m_blockHeight),
                             QSize(m_blockWidth, m_blockHeight)));
        }
    }
}

int SnakeGameBoard::blockWidth() const { return m_blockWidth; }

void SnakeGameBoard::setBlockWidth(int blockWidth) {
    m_blockWidth = blockWidth;
}

int SnakeGameBoard::rowCount() const { return height() / m_blockHeight; }

int SnakeGameBoard::columnCount() const { return width() / m_blockWidth; }

int SnakeGameBoard::blockHeight() const { return m_blockHeight; }

void SnakeGameBoard::setBlockHeight(int blockHeight) {
    m_blockHeight = blockHeight;
}

void Snake::move() { move_impl(); }

void Snake::grow() {
    std::scoped_lock<std::mutex> s(m_mutex);
    auto newHead = m_head;

    switch (m_direction) {
    case Direction::Up:
        newHead.ry()--;
        break;
    case Direction::Down:
        newHead.ry()++;
        break;
    case Direction::Right:
        newHead.rx()++;
        break;
    case Direction::Left:
        newHead.rx()++;
    }

    m_tail.push_front(m_head);
    m_head = newHead;
}

Snake::Direction Snake::direction() const { return m_direction; }

void Snake::setDirection(Direction direction) {
    if (m_direction == Right && direction == Left)
        return;
    if (m_direction == Left && direction == Right)
        return;
    if (m_direction == Up && direction == Down)
        return;
    if (m_direction == Down && direction == Up)
        return;

    std::scoped_lock<std::mutex> s(m_mutex);
    m_direction = direction;
}

const std::deque<QPoint> &Snake::tail() const { return m_tail; }

QPoint Snake::head() const { return m_head; }

void Snake::move_impl() {
    std::scoped_lock<std::mutex> s(m_mutex);
    auto newHead = m_head;

    switch (m_direction) {
    case Direction::Up:
        newHead.ry()--;
        break;
    case Direction::Down:
        newHead.ry()++;
        break;
    case Direction::Right:
        newHead.rx()++;
        break;
    case Direction::Left:
        newHead.rx()--;
    }

    m_tail.push_front(m_head);
    m_tail.pop_back();

    m_head = newHead;
}

#include <QTimer>
SnakeGame::SnakeGame(QWidget *parent) : SnakeGameBoard(parent), m_snake(this) {

    resetApplePos();

    auto t = new QTimer(this);
    t->setInterval(200);
    m_snake.setDirection(Snake::Direction::Down);
    connect(t, &QTimer::timeout, [this]() {
        m_snake.move();
        if (m_snake.head() == m_applePos) {
            m_snake.grow();
            resetApplePos();
        }
        this->update();
    });
    t->start();
    setFocus();
}

void SnakeGame::resetApplePos() {
    const auto tail = m_snake.tail();
    do {
        m_applePos = QPoint{rand() % columnCount(), rand() % rowCount()};
    } while (m_applePos == m_snake.head() ||
             std::find(tail.begin(), tail.end(), m_applePos) != tail.end());
}

void SnakeGame::paintEvent(QPaintEvent *event) {
    SnakeGameBoard::paintEvent(event);
    QPainter p(this);
    p.setRenderHint(QPainter::RenderHint::Antialiasing);
    auto h = m_snake.head();
    QRect r{h.x() * blockWidth(), h.y() * blockHeight(), blockWidth(),
            blockHeight()};
    p.drawChord(r, 0, 360 * 16);

    for (auto &t : m_snake.tail()) {
        r.setX(t.x() * blockWidth());
        r.setY(t.y() * blockHeight());
        r.setWidth(blockWidth());
        r.setHeight(blockHeight());
        p.drawChord(r, 0, 360 * 16);
    }

    r = QRect({m_applePos.x() * blockWidth(), m_applePos.y() * blockHeight()},
              QSize(blockWidth(), blockHeight()));
    p.drawChord(r, 0, 360 * 16);
}

void SnakeGame::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up)
        m_snake.setDirection(Snake::Up);
    else if (event->key() == Qt::Key_Down)
        m_snake.setDirection(Snake::Down);
    else if (event->key() == Qt::Key_Left)
        m_snake.setDirection(Snake::Left);
    else if (event->key() == Qt::Key_Right)
        m_snake.setDirection(Snake::Right);
}
