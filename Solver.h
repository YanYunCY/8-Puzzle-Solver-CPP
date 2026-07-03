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

    SolveResult() : success(false), nodesExpanded(0), timeUsed(0.0) {}
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
    int search(const PuzzleState& state, int g, int bound);

public:
    IDAStarSolver(const PuzzleState& initial) : Solver(initial), threshold(0) {}
    SolveResult solve() override;
};

#endif
