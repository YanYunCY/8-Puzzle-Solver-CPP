#include "Heuristic.h"

int Heuristic::manhattanDistance(const PuzzleState& state) {
    // TODO: 计算曼哈顿距离
    // 提示：遍历棋盘，对每个非0数字计算其当前位置与目标位置的距离
    // 目标位置计算：goalRow = (value-1)/3, goalCol = (value-1)%3

    return 0;
}

int Heuristic::misplacedTiles(const PuzzleState& state) {
    // TODO: 计算错位数字的个数
    // 提示：遍历棋盘，统计有多少个数字不在目标位置

    return 0;
}
