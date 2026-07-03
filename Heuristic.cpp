/**
 * @file Heuristic.cpp
 * @brief 启发式函数的实现
 * @author YanYun
 * @date 2026-06-30
 */

#include "Heuristic.h"
#include <cmath>
#include <cstdlib>

void findGoalPosition(int value, int goal[3][3], int& goalRow, int& goalCol) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (goal[i][j] == value) {
                goalRow = i;
                goalCol = j;
                return;
            }
        }
    }
}

int Heuristic::manhattanDistance(const PuzzleState& state) {
    int distance = 0;

    int goal[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            goal[i][j] = PuzzleState::getGoal(i, j);
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int value = state.getBoard(i, j);
            if (value != 0) {
                int goalRow, goalCol;
                findGoalPosition(value, goal, goalRow, goalCol);
                // 计算曼哈顿距离
                distance += std::abs(i - goalRow) + std::abs(j - goalCol);
            }
        }
    }

    return distance;
}

int Heuristic::misplacedTiles(const PuzzleState& state) {
    int count = 0;

    int goal[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            goal[i][j] = PuzzleState::getGoal(i, j);
        }
    }

    // 遍历每个位置，统计错位的数字
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int value = state.getBoard(i, j);
            // 如果不是空格，且当前位置的值与目标不同，则计数
            if (value != 0 && value != goal[i][j]) {
                count++;
            }
        }
    }

    return count;
}
