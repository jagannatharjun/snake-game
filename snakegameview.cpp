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
    auto rowCount = r.height() / m_blockHeight;
    auto columnCount = r.width() / m_blockWidth;
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < columnCount; j++) {
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
SnakeGame::SnakeGame(SnakeGameBoard *b, Snake *s)
    : m_board{b}, m_snake{s}, QWidget(b) {
    resize(b->width(), b->height());
    auto t = new QTimer(this);
    t->setInterval(400);
    m_snake->setDirection(Snake::Direction::Down);
    m_snake->grow();
    m_snake->grow();
    connect(t, &QTimer::timeout, [this]() {
        m_snake->move();
        this->update();
    });
    t->start();
    setFocus();
}

void SnakeGame::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setRenderHint(QPainter::RenderHint::Antialiasing);
    auto h = m_snake->head();
    QRect r{h.x() * m_board->blockWidth(), h.y() * m_board->blockHeight(),
            m_board->blockWidth(), m_board->blockHeight()};
    p.drawChord(r, 0, 360 * 16);

    for (auto& t : m_snake->tail()) {
        r.setX(t.x() * m_board->blockWidth());
        r.setY(t.y() * m_board->blockHeight());
        r.setWidth(m_board->blockWidth());
        r.setHeight(m_board->blockHeight());
        p.drawChord(r, 0, 360 * 16);
    }
}

void SnakeGame::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
        m_snake->setDirection(Snake::Up);
    else if(event->key() == Qt::Key_Down)
        m_snake->setDirection(Snake::Down);
    else if (event->key() == Qt::Key_Left)
        m_snake->setDirection(Snake::Left);
    else if (event->key() == Qt::Key_Right)
        m_snake->setDirection(Snake::Right);
}
