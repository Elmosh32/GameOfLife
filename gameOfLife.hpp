#ifndef GAME_OF_LIFE_HPP
#define GAME_OF_LIFE_HPP

#include <thread>
#include <string.h>
#include <iostream>
#include <functional>
#include <semaphore>

#include "doubleBuffer.hpp"

class GameOfLife
{
public:
    explicit GameOfLife(int a_canvasWidth, int a_canvasHeight);
    ~GameOfLife();
    void run();

private:
    void createNextFrame(Point p1, Point p2);
    int countNeighbors(int x, int y);

    int checkFirstRow(int x, int y);
    int checkLastRow(int x, int y);
    int checkFirstColumn(int x, int y);
    int checkLastColumn(int x, int y);
    int checkCellNeighbors(int x, int y);

    bool isAlive(int x, int y);
    Pixel<int> getNewColor(int a_x, int a_y);
    void saveRandomFrame();
    void initThreads();
    void WaitForThreads();

private:
    int m_canvasWidth;
    int m_canvasHeight;
    int m_frameVersion = 1;
    size_t const m_numOfFrames = 1000;
    size_t const m_numOfThreads = 4;

    DoubleBuffer<Pixel<int>> m_buffer;
    // int m_numOfThreads;
    std::counting_semaphore<4> m_canCompute{4};
    std::counting_semaphore<4> m_doneCompute{0};
    std::vector<std::thread> m_threads;
    Pixel<int> m_blackPixel = Pixel<int>(0, 0, 0);
    Pixel<int> m_redPixel = Pixel<int>(255, 0, 0);
    Pixel<int> m_bluePixel = Pixel<int>(0, 0, 255);
    Pixel<int> m_greenPixel = Pixel<int>(0, 255, 0);
    Pixel<int> m_whitePixel = Pixel<int>(255, 255, 255);
};

// #include "./inl/gameOfLife.hxx"

#endif /* GAME_OF_LIFE_HPP */