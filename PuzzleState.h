#ifndef PUZZLE_STATE_H
#define PUZZLE_STATE_H

#include <vector>
#include <string>

// 表示8数码问题的一个状态
class PuzzleState {
private:
    int board[3][3];        // 3x3棋盘，0表示空格
    int zeroRow, zeroCol;   // 空格的位置
    int gCost;              // 从起点到当前状态的实际代价
    int hCost;              // 从当前状态到目标的估计代价

public:
    // 构造函数
    PuzzleState();
    PuzzleState(const int initBoard[3][3]);
    PuzzleState(const std::vector<int>& flatBoard);

    // 基本操作
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

    // getter和setter
    int getGCost() const { return gCost; }
    int getHCost() const { return hCost; }
    int getFCost() const { return gCost + hCost; }
    void setGCost(int g) { gCost = g; }
    void setHCost(int h) { hCost = h; }
    int getBoard(int row, int col) const { return board[row][col]; }

    bool operator==(const PuzzleState& other) const;
    bool operator<(const PuzzleState& other) const;
};

// 哈希函数
namespace std {
    template<>
    struct hash<PuzzleState> {
        size_t operator()(const PuzzleState& state) const;
    };
}

#endif
