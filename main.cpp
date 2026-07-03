/**
 * @file main.cpp
 * @brief 8数码问题求解器主程序
 * @author YanYun
 * @date 2026-06-30
 */

#include <iostream>
#include "PuzzleState.h"
#include "Solver.h"

void printResult(const std::string& algorithmName, const SolveResult& result) {
    std::cout << "\n========== " << algorithmName << " ==========\n";
    if (result.success) {
        std::cout << "找到解！\n";
        std::cout << "步数: " << result.path.size() - 1 << "\n";
        std::cout << "扩展节点数: " << result.nodesExpanded << "\n";
        std::cout << "用时: " << result.timeUsed << " ms\n";

        // TODO: 可选 - 打印解的步骤
        // std::cout << "\n解的路径:\n";
        // for (const auto& state : result.path) {
        //     state.print();
        //     std::cout << "\n";
        // }
    } else {
        std::cout << "未找到解或超时。\n";
    }
}

int main() {
    std::cout << "========== 8数码问题求解器 ==========\n\n";

    // TODO: 创建不同难度的测试用例
    std::vector<int> easyPuzzle = {1, 2, 3, 4, 5, 6, 0, 7, 8};
    // std::vector<int> mediumPuzzle = {1, 2, 3, 4, 0, 5, 7, 8, 6};
    // std::vector<int> hardPuzzle = {8, 6, 7, 2, 5, 4, 3, 0, 1};

    PuzzleState initialState(easyPuzzle);

    std::cout << "初始状态:\n";
    initialState.print();

    if (!initialState.isSolvable()) {
        std::cout << "\n此问题无解！\n";
        return 0;
    }

    std::cout << "\n问题有解。\n";

    // TODO: 测试BFS算法
    // BFSSolver bfsSolver(initialState);
    // SolveResult bfsResult = bfsSolver.solve();
    // printResult("BFS", bfsResult);

    // TODO: 测试A*算法
    // AStarSolver astarSolver(initialState);
    // SolveResult astarResult = astarSolver.solve();
    // printResult("A*", astarResult);

    // TODO: 测试IDA*算法
    // IDAStarSolver idastarSolver(initialState);
    // SolveResult idastarResult = idastarSolver.solve();
    // printResult("IDA*", idastarResult);

    // TODO: 性能对比表格
    // std::cout << "\n========== 性能对比 ==========\n";
    // printf("%-10s %-10s %-15s %-10s\n", "算法", "步数", "扩展节点数", "时间(ms)");

    std::cout << "\n准备好实现搜索算法了！\n";
    return 0;
}
