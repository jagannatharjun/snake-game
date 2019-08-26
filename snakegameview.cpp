#include "snakegameview.h"
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>

SnakeGameBoard::SnakeGameBoard(QWidget *parent) : QWidget(parent) {
    auto pol = sizePolicy();
    pol.setHeightForWidth(true);
}

void SnakeGameBoard::paintEvent(QPaintEvent *) {
    QPainter p(this);

    p.setPen(Qt::transparent);
    p.setRenderHint(QPainter::RenderHint::Antialiasing);
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

    p.setPen(QPen(Qt::darkGray, 2));
    p.setBrush(QBrush(Qt::transparent));
    p.drawRect(
        QRect(QPoint(0, 0), QSize(cc * m_blockWidth, rc * m_blockHeight)));
}

void SnakeGameBoard::resizeEvent(QResizeEvent *) {
    resize(m_blockWidth * columnCount(), m_blockHeight * rowCount());
    updateGeometry();
}

int SnakeGameBoard::blockWidth() const { return m_blockWidth; }

void SnakeGameBoard::setBlockWidth(int blockWidth) {
    m_blockWidth = blockWidth;
}

int SnakeGameBoard::rowCount() const { return height() / m_blockHeight; }

int SnakeGameBoard::columnCount() const { return width() / m_blockWidth; }

QSize SnakeGameBoard::sizeHint() const {
    return QSize(m_blockWidth * columnCount(), m_blockHeight * rowCount());
}

int SnakeGameBoard::blockHeight() const { return m_blockHeight; }

void SnakeGameBoard::setBlockHeight(int blockHeight) {
    m_blockHeight = blockHeight;
}

Snake::Snake(QObject *parent) : QObject(parent) { reset(); }

void Snake::move() { move_impl(); }

void Snake::grow() {
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
        break;
    }

    m_tail.push_front(m_head);
    m_head = newHead;
}

void Snake::reset() {
    m_head = {0, 0};
    m_tail.clear();
    m_direction = Right;
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

    m_direction = direction;
}

const std::deque<QPoint> &Snake::tail() const { return m_tail; }

QPoint Snake::head() const { return m_head; }

void Snake::move_impl() {
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

SnakeGame::SnakeGame(QWidget *parent) : SnakeGameBoard(parent), m_snake(this) {
    reset();
    m_timer.setInterval(150);
    connect(&m_timer, &QTimer::timeout, this, &SnakeGame::updateGame);
    m_timer.start();
    setFocus();
}

size_t SnakeGame::score() const { return m_score; }

void SnakeGame::reset() {
    m_timer.stop();
    resetApplePos();
    m_score = 0;
    m_snake.reset();
    emit scoreChanged();
    m_timer.start();
}

void SnakeGame::resetApplePos() {
    const auto tail = m_snake.tail();
    do {
        m_applePos = QPoint{rand() % columnCount(), rand() % rowCount()};
    } while (m_applePos == m_snake.head() ||
             std::find(tail.begin(), tail.end(), m_applePos) != tail.end());
}

Qt::GlobalColor randomColor() {
    static Qt::GlobalColor palleteColor[] = {Qt::blue, Qt::darkMagenta,
                                             Qt::magenta};
    return palleteColor[rand() %
                        (sizeof(palleteColor) / sizeof(palleteColor[0]))];
}

void SnakeGame::paintEvent(QPaintEvent *event) {
    SnakeGameBoard::paintEvent(event);

    QPainter p(this);
    p.setRenderHint(QPainter::RenderHint::Antialiasing);

    const static auto snakeColor = QColor(0x4775EA);

    p.drawImage(
        QRect({m_applePos.x() * blockWidth(), m_applePos.y() * blockHeight()},
              QSize(blockWidth(), blockHeight())),
        QImage(":/apple.png"));

    auto h = m_snake.head();
    QRect r{h.x() * blockWidth(), h.y() * blockHeight(), blockWidth(),
            blockHeight()};
    p.setBrush(QBrush(snakeColor));
    p.drawChord(r, 0, 360 * 16);

    p.setBrush(QBrush(Qt::black));
    QPoint e1, e2;
    constexpr float f1 = .30, f2 = .70;
    switch (m_snake.direction()) {
    case Snake::Direction::Up:
        e1 =
            QPoint(r.x() + (blockWidth() * f1), r.y() + (blockHeight() * f1));
        e2 =
            QPoint(r.x() + (blockWidth() * f2), r.y() + (blockHeight() * f1));
        break;
    case Snake::Direction::Down:
        e1 =
            QPoint(r.x() + (blockWidth() * f1), r.y() + (blockHeight() * f2));
        e2 =
            QPoint(r.x() + (blockWidth() * f2), r.y() + (blockHeight() * f2));
        break;
    case Snake::Direction::Left:
        e1 =
            QPoint(r.x() + (blockWidth() * f1), r.y() + (blockHeight() * f1));
        e2 =
            QPoint(r.x() + (blockWidth() * f1), r.y() + (blockHeight() * f2));
        break;
    case Snake::Direction::Right:
        e1 =
            QPoint(r.x() + (blockWidth() * f2), r.y() + (blockHeight() * f1));
        e2 =
            QPoint(r.x() + (blockWidth() * f2), r.y() + (blockHeight() * f2));
        break;
    }
    p.drawChord(QRect(e1, QSize(2, 2)), 0, 360 * 16);
    p.drawChord(QRect(e2, QSize(2, 2)), 0, 360 * 16);

    p.setBrush(QBrush(snakeColor));
    for (auto &t : m_snake.tail()) {
        r.setX(t.x() * blockWidth());
        r.setY(t.y() * blockHeight());
        r.setWidth(blockWidth());
        r.setHeight(blockHeight());
        p.drawChord(r, 0, 360 * 16);
    }
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

void SnakeGame::updateGame() {
    if (m_snake.head() == m_applePos) {
        m_score++;
        emit scoreChanged();
        m_snake.grow();
        resetApplePos();
        m_snake.move();
    } else if (isCollision()) {
        reset();
        emit collisioned();
    } else {
        m_snake.move();
    }
    this->update();
}

bool SnakeGame::isCollision() {
    auto h = m_snake.head();

    if (h.x() >= columnCount() || h.x() < 0 || h.y() >= rowCount() ||
        h.y() < 0) {
        return true;
    }
    return false;
}
