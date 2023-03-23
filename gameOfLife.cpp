#include "gameOfLife.hpp"
#include "doubleBuffer.hpp"

GameOfLife::GameOfLife(int a_canvasWidth, int a_canvasHeight)
: m_canvasWidth(a_canvasWidth)
, m_canvasHeight(a_canvasHeight)
, m_buffer(m_canvasWidth, m_canvasHeight, "color")
{
    m_threads.reserve(m_numOfThreads);
    m_buffer.fill(m_whitePixel, Side::Front);
}

GameOfLife::~GameOfLife()
{
    for (auto &thread : m_threads) {
        thread.join();
    }
}

void GameOfLife::run()
{
    saveRandomFrame();

    initThreads();

    for (size_t i = 0; i < m_numOfFrames - 1; i++) {
        WaitForThreads();
        m_buffer.switchSides();
        m_buffer.saveColoredImage(m_frameVersion);
        m_frameVersion++;
        m_canCompute.release(m_numOfThreads);
    }
}

void GameOfLife::saveRandomFrame()
{
    srand(time(NULL));
    for (int j = 0; j < 32000; j++) {
        int randomX = rand() % 400;
        int randomY = rand() % 400;
        m_buffer.drawPixel(m_blackPixel, Point(randomX, randomY), Side::Front);
    }

    m_buffer.saveColoredImage(m_frameVersion);
    m_frameVersion++;
}

void GameOfLife::WaitForThreads()
{
    for (size_t i = 0; i < m_numOfThreads; ++i) {
        m_doneCompute.acquire();
    }
}

void GameOfLife::initThreads()
{
    m_threads.push_back(std::thread(&GameOfLife::createNextFrame, this, Point(0, 0), Point(m_canvasWidth / 2 - 1, m_canvasHeight / 2 - 1)));
    m_threads.push_back(std::thread(&GameOfLife::createNextFrame, this, Point(m_canvasWidth / 2, 0), Point(m_canvasWidth - 1, m_canvasHeight / 2 - 1)));
    m_threads.push_back(std::thread(&GameOfLife::createNextFrame, this, Point(0, m_canvasHeight / 2), Point(m_canvasWidth / 2 - 1, m_canvasHeight - 1)));
    m_threads.push_back(std::thread(&GameOfLife::createNextFrame, this, Point(m_canvasWidth / 2, m_canvasHeight / 2), Point(m_canvasWidth - 1, m_canvasHeight - 1)));
}

void GameOfLife::createNextFrame(Point p1, Point p2)
{
    for(size_t i = 0; i < m_numOfFrames - 1; ++i) {
        for (int x = p1.m_x; x <= p2.m_x; ++x) {
            for (int y = p1.m_y; y <= p2.m_y; ++y) {
                int neighbors = countNeighbors(x, y);

                if (isAlive(x, y) && (neighbors < 2 || neighbors > 3)) {
                    m_buffer.drawPixel(m_greenPixel, Point(x, y), Side::Back);
                } else if (isAlive(x, y) && (neighbors == 2 || neighbors == 3)) {
                    m_buffer.drawPixel(m_blackPixel, Point(x, y), Side::Back);
                } else if (!isAlive(x, y) && neighbors == 3) {
                    m_buffer.drawPixel(m_blackPixel, Point(x, y), Side::Back);
                } else {
                    Pixel<int> color = getNewColor(x, y);
                    m_buffer.drawPixel(color, Point(x, y), Side::Back);
                }
            }
        }
        m_doneCompute.release();
        m_canCompute.acquire();
    }
}

int GameOfLife::countNeighbors(int a_x, int a_y)
{
    if (a_x == 0) {
        return checkFirstRow(a_x, a_y);
    } else if (a_x == m_canvasWidth - 1) {
        return checkLastRow(a_x, a_y);
    } else if (a_y == 0) {
        return checkFirstColumn(a_x, a_y);
    } else if (a_y == m_canvasHeight - 1) {
        return checkLastColumn(a_x, a_y);
    } else {
        return checkCellNeighbors(a_x, a_y);
    }
}

int GameOfLife::checkFirstRow(int a_x, int a_y)
{
    if (a_y == 0) {
        // checkLeftTopCorner
        return isAlive(a_x + 1, a_y) + isAlive(a_x + 1, a_y + 1) + isAlive(a_x, a_y + 1);
    } else if (a_y == m_canvasHeight - 1) {
        // checkRightTopCorner
        return isAlive(a_x, a_y - 1) + isAlive(a_x + 1, a_y - 1) + isAlive(a_x + 1, a_y);
    } else {
        return isAlive(a_x, a_y - 1) + isAlive(a_x + 1, a_y - 1) + isAlive(a_x + 1, a_y) + isAlive(a_x + 1, a_y + 1) + isAlive(a_x, a_y + 1);
    }
}

int GameOfLife::checkLastRow(int a_x, int a_y)
{
    if (a_y == 0) {
        // checkLeftBottomCorner
        return isAlive(a_x - 1, a_y) + isAlive(a_x - 1, a_y + 1) + isAlive(a_x, a_y + 1);
    } else if (a_y == m_canvasHeight - 1) {
        // checkRightBottomCorner
        return isAlive(a_x, a_y - 1) + isAlive(a_x - 1, a_y - 1) + isAlive(a_x - 1, a_y);
    } else {
        return isAlive(a_x, a_y - 1) + isAlive(a_x - 1, a_y - 1) + isAlive(a_x - 1, a_y) + isAlive(a_x - 1, a_y + 1) + isAlive(a_x, a_y + 1);
    }
}

int GameOfLife::checkFirstColumn(int a_x, int a_y)
{
    return isAlive(a_x - 1, a_y) + isAlive(a_x - 1, a_y + 1) + isAlive(a_x, a_y + 1) + isAlive(a_x + 1, a_y + 1) + isAlive(a_x + 1, a_y);
}

int GameOfLife::checkLastColumn(int a_x, int a_y)
{
    return isAlive(a_x - 1, a_y) + isAlive(a_x - 1, a_y - 1) + isAlive(a_x, a_y - 1) + isAlive(a_x + 1, a_y - 1) + isAlive(a_x + 1, a_y);
}

int GameOfLife::checkCellNeighbors(int a_x, int a_y)
{
    return isAlive(a_x - 1, a_y - 1) + isAlive(a_x, a_y - 1) +
           isAlive(a_x + 1, a_y - 1) + isAlive(a_x + 1, a_y) +
           isAlive(a_x + 1, a_y + 1) + isAlive(a_x, a_y + 1) +
           isAlive(a_x - 1, a_y + 1) + isAlive(a_x - 1, a_y);
}

bool GameOfLife::isAlive(int a_x, int a_y)
{
    if (m_buffer.getPixel(a_x, a_y) == m_blackPixel) {
        return true;
    }
    
    return false;
}

Pixel<int> GameOfLife::getNewColor(int a_x, int a_y)
{
    Pixel<int> pixel = m_buffer.getPixel(a_x, a_y);
    if (pixel == m_greenPixel) {
        return m_bluePixel;
    } else if (pixel == m_bluePixel) {
        return m_redPixel;
    }
    
    return m_whitePixel;
}