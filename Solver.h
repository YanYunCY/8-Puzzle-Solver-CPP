/**
 * @file Solver.h
 * @brief 搜索算法求解器类声明
 * @author YanYun
 * @date 2026-06-30
 */

#ifndef SOLVER_H
#define SOLVER_H

#include "PuzzleState.h"
#include <vector>

struct SolveResult {
    bool success;
    std::vector<PuzzleState> path;
    int nodesExpanded;
    double timeUsed;
    size_t memoryUsed;  // 内存使用量（字节）

    SolveResult() : success(false), nodesExpanded(0), timeUsed(0.0), memoryUsed(0) {}
};

class Solver {
protected:
    PuzzleState initialState;
    int nodesExpanded;

public:
    Solver(const PuzzleState& initial) : initialState(initial), nodesExpanded(0) {}
    virtual ~Solver() {}
    virtual SolveResult solve() = 0;
};

class BFSSolver : public Solver {
public:
    BFSSolver(const PuzzleState& initial) : Solver(initial) {}
    SolveResult solve() override;
};

class AStarSolver : public Solver {
public:
    AStarSolver(const PuzzleState& initial) : Solver(initial) {}
    SolveResult solve() override;
};

class IDAStarSolver : public Solver {
private:
    std::vector<PuzzleState> path;
    int threshold;
    size_t maxPathLen;   // 搜索过程中路径的峰值长度（用于统计内存占用）
    int search(const PuzzleState& state, int g, int bound);

public:
    IDAStarSolver(const PuzzleState& initial) : Solver(initial), threshold(0), maxPathLen(0) {}
    SolveResult solve() override;
};

#endif
