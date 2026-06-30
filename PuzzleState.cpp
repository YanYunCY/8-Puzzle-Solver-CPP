#include "PuzzleState.h"
#include <iostream>
#include <sstream>

// 目标状态
const int GOAL[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}};

PuzzleState::PuzzleState() : gCost(0), hCost(0), zeroRow(2), zeroCol(2) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = GOAL[i][j];
        }
    }
}

PuzzleState::PuzzleState(const int initBoard[3][3]) : gCost(0), hCost(0) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = initBoard[i][j];
            if (board[i][j] == 0) {
                zeroRow = i;
                zeroCol = j;
            }
        }
    }
}

PuzzleState::PuzzleState(const std::vector<int>& flatBoard) : gCost(0), hCost(0) {
    int idx = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = flatBoard[idx++];
            if (board[i][j] == 0) {
                zeroRow = i;
                zeroCol = j;
            }
        }
    }
}

bool PuzzleState::isGoalState() const {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != GOAL[i][j]) return false;
        }
    }
    return true;
}

bool PuzzleState::canMoveUp() const { return zeroRow > 0; }
bool PuzzleState::canMoveDown() const { return zeroRow < 2; }
bool PuzzleState::canMoveLeft() const { return zeroCol > 0; }
bool PuzzleState::canMoveRight() const { return zeroCol < 2; }

PuzzleState PuzzleState::moveUp() const {
    PuzzleState newState = *this;
    newState.board[zeroRow][zeroCol] = newState.board[zeroRow - 1][zeroCol];
    newState.board[zeroRow - 1][zeroCol] = 0;
    newState.zeroRow--;
    return newState;
}

PuzzleState PuzzleState::moveDown() const {
    PuzzleState newState = *this;
    newState.board[zeroRow][zeroCol] = newState.board[zeroRow + 1][zeroCol];
    newState.board[zeroRow + 1][zeroCol] = 0;
    newState.zeroRow++;
    return newState;
}

PuzzleState PuzzleState::moveLeft() const {
    PuzzleState newState = *this;
    newState.board[zeroRow][zeroCol] = newState.board[zeroRow][zeroCol - 1];
    newState.board[zeroRow][zeroCol - 1] = 0;
    newState.zeroCol--;
    return newState;
}

PuzzleState PuzzleState::moveRight() const {
    PuzzleState newState = *this;
    newState.board[zeroRow][zeroCol] = newState.board[zeroRow][zeroCol + 1];
    newState.board[zeroRow][zeroCol + 1] = 0;
    newState.zeroCol++;
    return newState;
}

std::vector<PuzzleState> PuzzleState::getSuccessors() const {
    std::vector<PuzzleState> successors;
    if (canMoveUp()) successors.push_back(moveUp());
    if (canMoveDown()) successors.push_back(moveDown());
    if (canMoveLeft()) successors.push_back(moveLeft());
    if (canMoveRight()) successors.push_back(moveRight());
    return successors;
}

bool PuzzleState::isSolvable() const {
    std::vector<int> flat;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != 0) flat.push_back(board[i][j]);
        }
    }
    int inversions = 0;
    for (size_t i = 0; i < flat.size(); i++) {
        for (size_t j = i + 1; j < flat.size(); j++) {
            if (flat[i] > flat[j]) inversions++;
        }
    }
    return inversions % 2 == 0;
}

std::string PuzzleState::toString() const {
    std::stringstream ss;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            ss << board[i][j];
        }
    }
    return ss.str();
}

void PuzzleState::print() const {
    std::cout << "-------------\n";
    for (int i = 0; i < 3; i++) {
        std::cout << "| ";
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 0) std::cout << "  | ";
            else std::cout << board[i][j] << " | ";
        }
        std::cout << "\n-------------\n";
    }
}

bool PuzzleState::operator==(const PuzzleState& other) const {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != other.board[i][j]) return false;
        }
    }
    return true;
}

bool PuzzleState::operator<(const PuzzleState& other) const {
    return getFCost() < other.getFCost();
}

size_t std::hash<PuzzleState>::operator()(const PuzzleState& state) const {
    return std::hash<std::string>()(state.toString());
}
