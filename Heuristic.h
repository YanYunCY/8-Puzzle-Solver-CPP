/**
 * @file Heuristic.h
 * @brief 启发式函数类声明
 * @author YanYun
 * @date 2026-06-30
 */

#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "PuzzleState.h"

class Heuristic {
public:
    static int manhattanDistance(const PuzzleState& state);
    static int misplacedTiles(const PuzzleState& state);
};

#endif
