/**
 * @file PuzzleState.h
 * @brief 8数码问题的状态表示类
 * @author YanYun
 * @date 2026-06-30
 */

#ifndef PUZZLE_STATE_H
#define PUZZLE_STATE_H

#include <vector>
#include <string>

class PuzzleState {
private:
    static int GOAL[3][3];
    int board[3][3];
    int zeroRow, zeroCol;
    int gCost;
    int hCost;

public:
    PuzzleState();
    PuzzleState(const int initBoard[3][3]);
    PuzzleState(const std::vector<int>& flatBoard);

    static void setGoalState(const std::vector<int>& goalState);
    static int getGoal(int row, int col) { return GOAL[row][col]; }

    bool isGoalState() const;
    bool canMoveUp() const;
    bool canMoveDown() const;
    bool canMoveLeft() const;
    bool canMoveRight() const;

    PuzzleState moveUp() const;
    PuzzleState moveDown() const;
    PuzzleState moveLeft() const;
    PuzzleState moveRight() const;

    std::vector<PuzzleState> getSuccessors() const;
    bool isSolvable() const;
    std::string toString() const;
    void print() const;

    int getGCost() const { return gCost; }
    int getHCost() const { return hCost; }
    int getFCost() const { return gCost + hCost; }
    void setGCost(int g) { gCost = g; }
    void setHCost(int h) { hCost = h; }
    int getBoard(int row, int col) const { return board[row][col]; }

    bool operator==(const PuzzleState& other) const;
    bool operator<(const PuzzleState& other) const;
};

namespace std {
    template<>
    struct hash<PuzzleState> {
        size_t operator()(const PuzzleState& state) const;
    };
}

#endif
