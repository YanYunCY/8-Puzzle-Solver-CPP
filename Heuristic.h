#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "PuzzleState.h"

// 启发式函数类
class Heuristic {
public:
    static int manhattanDistance(const PuzzleState& state);
    static int misplacedTiles(const PuzzleState& state);
};

#endif
