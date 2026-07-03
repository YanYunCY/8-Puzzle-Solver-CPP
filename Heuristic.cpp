/**
 * @file Heuristic.cpp
 * @brief 启发式函数的实现
 * @author YanYun
 * @date 2026-06-30
 */

#include "Heuristic.h"
#include <cmath>
#include <cstdlib>

int Heuristic::manhattanDistance(const PuzzleState& state) {
    // TODO: 计算曼哈顿距离启发函数
    //
    // 曼哈顿距离 = 每个数字距离目标位置的横纵坐标差的绝对值之和
    //
    // 实现步骤：
    // 1. 初始化 distance = 0
    // 2. 遍历3x3棋盘的每个位置(i, j)：
    //    a. 获取 state.getBoard(i, j) 的值
    //    b. 如果值为0（空格），跳过
    //    c. 计算该值在目标状态的位置：
    //       - 目标状态为： 1 2 3
    //                     4 5 6
    //                     7 8 0
    //       - 对于数字1-8： goalRow = (value-1)/3, goalCol = (value-1)%3
    //    d. 累加 abs(i - goalRow) + abs(j - goalCol) 到 distance
    // 3. 返回 distance
    //
    // 示例代码框架：
    //   int distance = 0;
    //   for (int i = 0; i < 3; i++) {
    //       for (int j = 0; j < 3; j++) {
    //           int value = state.getBoard(i, j);
    //           if (value != 0) {
    //               int goalRow = (value - 1) / 3;
    //               int goalCol = (value - 1) % 3;
    //               distance += abs(i - goalRow) + abs(j - goalCol);
    //           }
    //       }
    //   }
    //   return distance;

    return 0;
}

int Heuristic::misplacedTiles(const PuzzleState& state) {
    // TODO: 统计错位的数字个数（更简单的启发函数）
    //
    // 实现步骤：
    // 1. 定义目标状态： {{1,2,3},{4,5,6},{7,8,0}}
    // 2. 初始化 count = 0
    // 3. 遍历每个位置(i, j)：
    //    a. 获取 state.getBoard(i, j) 的值
    //    b. 如果 value != 0 且 value != goalBoard[i][j]：
    //       - count++
    // 4. 返回 count
    //
    // 示例代码框架：
    //   const int GOAL[3][3] = {{1,2,3},{4,5,6},{7,8,0}};
    //   int count = 0;
    //   for (int i = 0; i < 3; i++) {
    //       for (int j = 0; j < 3; j++) {
    //           int value = state.getBoard(i, j);
    //           if (value != 0 && value != GOAL[i][j]) {
    //               count++;
    //           }
    //       }
    //   }
    //   return count;

    return 0;
}
