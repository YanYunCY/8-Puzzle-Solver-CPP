#ifndef SOLVER_H
#define SOLVER_H

#include "PuzzleState.h"
#include <vector>

// 求解结果结构
struct SolveResult {
    bool success;
    std::vector<PuzzleState> path;
    int nodesExpanded;
    double timeUsed;

    SolveResult() : success(false), nodesExpanded(0), timeUsed(0.0) {}
};

// 求解器基类
class Solver {
protected:
    PuzzleState initialState;
    int nodesExpanded;

public:
    Solver(const PuzzleState& initial) : initialState(initial), nodesExpanded(0) {}
    virtual ~Solver() {}
    virtual SolveResult solve() = 0;
};

// BFS求解器
class BFSSolver : public Solver {
public:
    BFSSolver(const PuzzleState& initial) : Solver(initial) {}
    SolveResult solve() override;
};

// A*求解器
class AStarSolver : public Solver {
public:
    AStarSolver(const PuzzleState& initial) : Solver(initial) {}
    SolveResult solve() override;
};

// IDA*求解器
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
