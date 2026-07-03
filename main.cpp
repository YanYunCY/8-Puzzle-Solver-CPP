/**
 * @file main.cpp
 * @brief 8数码问题求解器主程序
 * @author YanYun
 * @date 2026-06-30
 */

#include <iostream>
#include <iomanip>
#include <random>
#include <algorithm>
#include "PuzzleState.h"
#include "Solver.h"

// 生成随机的可解状态
std::vector<int> generateRandomSolvableState() {
    std::vector<int> state = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // 使用随机设备和引擎
    std::random_device rd;
    std::mt19937 gen(rd());

    // 随机打乱，直到生成可解状态
    do {
        std::shuffle(state.begin(), state.end(), gen);
        PuzzleState temp(state);
        if (temp.isSolvable()) {
            break;
        }
    } while (true);

    return state;
}

void printResult(const std::string& algorithmName, const SolveResult& result) {
    std::cout << "\n========== " << algorithmName << " ==========\n";
    if (result.success) {
        std::cout << "找到解！\n";
        std::cout << "步数: " << result.path.size() - 1 << "\n";
        std::cout << "扩展节点数: " << result.nodesExpanded << "\n";
        std::cout << "内存使用: " << std::fixed << std::setprecision(2)
                  << result.memoryUsed / 1024.0 << " KB\n";
        std::cout << "用时: " << std::fixed << std::setprecision(2)
                  << result.timeUsed << " ms\n";
    } else {
        std::cout << "未找到解或超时。\n";
    }
}

void printPath(const SolveResult& result) {
    if (!result.success || result.path.empty()) return;

    std::cout << "\n========== 解的路径 ==========\n";
    std::cout << "总共 " << result.path.size() - 1 << " 步:\n\n";

    for (size_t i = 0; i < result.path.size(); i++) {
        std::cout << "步骤 " << i << ":\n";
        result.path[i].print();
        if (i < result.path.size() - 1) {
            std::cout << "   ↓\n";
        }
    }
}

int main() {
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║       8数码问题求解器 - 算法性能测试           ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n\n";

    // 生成随机的初始状态和目标状态
    std::cout << "正在生成随机初始状态...\n";
    std::vector<int> initialPuzzle = generateRandomSolvableState();

    std::cout << "正在生成随机目标状态...\n";
    std::vector<int> goalPuzzle = generateRandomSolvableState();

    // 设置目标状态
    PuzzleState::setGoalState(goalPuzzle);

    PuzzleState initialState(initialPuzzle);

    std::cout << "\n初始状态:\n";
    initialState.print();

    std::cout << "\n目标状态:\n";
    PuzzleState goalState(goalPuzzle);
    goalState.print();

    if (!initialState.isSolvable()) {
        std::cout << "\n此问题无解！\n";
        return 0;
    }

    std::cout << "\n问题有解。\n";

    // 测试BFS算法
    std::cout << "\n[1/3] 正在运行 BFS...\n";
    BFSSolver bfsSolver(initialState);
    SolveResult bfsResult = bfsSolver.solve();
    printResult("BFS 广度优先搜索", bfsResult);

    // 测试A*算法
    std::cout << "\n[2/3] 正在运行 A*...\n";
    AStarSolver astarSolver(initialState);
    SolveResult astarResult = astarSolver.solve();
    printResult("A* 启发式搜索", astarResult);

    // 测试IDA*算法
    std::cout << "\n[3/3] 正在运行 IDA*...\n";
    IDAStarSolver idastarSolver(initialState);
    SolveResult idastarResult = idastarSolver.solve();
    printResult("IDA* 迭代加深搜索", idastarResult);

    // 性能对比表格
    std::cout << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║              性能对比                          ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n";
    std::cout << std::left;
    std::cout << std::setw(12) << "算法"
              << std::setw(10) << "步数"
              << std::setw(15) << "扩展节点数"
              << std::setw(12) << "内存(KB)"
              << std::setw(12) << "时间(ms)" << "\n";
    std::cout << std::string(61, '-') << "\n";

    if (bfsResult.success) {
        std::cout << std::setw(12) << "BFS"
                  << std::setw(10) << (bfsResult.path.size() - 1)
                  << std::setw(15) << bfsResult.nodesExpanded
                  << std::fixed << std::setprecision(2)
                  << std::setw(12) << bfsResult.memoryUsed / 1024.0
                  << std::setw(12) << bfsResult.timeUsed << "\n";
    }

    if (astarResult.success) {
        std::cout << std::setw(12) << "A*"
                  << std::setw(10) << (astarResult.path.size() - 1)
                  << std::setw(15) << astarResult.nodesExpanded
                  << std::fixed << std::setprecision(2)
                  << std::setw(12) << astarResult.memoryUsed / 1024.0
                  << std::setw(12) << astarResult.timeUsed << "\n";
    }

    if (idastarResult.success) {
        std::cout << std::setw(12) << "IDA*"
                  << std::setw(10) << (idastarResult.path.size() - 1)
                  << std::setw(15) << idastarResult.nodesExpanded
                  << std::fixed << std::setprecision(2)
                  << std::setw(12) << idastarResult.memoryUsed / 1024.0
                  << std::setw(12) << idastarResult.timeUsed << "\n";
    }

    // 打印解的路径（只打印 A* 的路径，因为三个算法路径相同）
    printPath(astarResult);

    std::cout << "\n测试完成！\n";
    return 0;
}
