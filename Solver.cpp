/**
 * @file Solver.cpp
 * @brief 搜索算法的实现
 * @author YanYun
 * @date 2026-06-30
 */

#include "Solver.h"
#include "Heuristic.h"
#include <chrono>

// ==================== BFS 实现 ====================

SolveResult BFSSolver::solve() {
    SolveResult result;
    auto startTime = std::chrono::high_resolution_clock::now();

    // TODO: 实现BFS算法（广度优先搜索）
    //
    // 实现步骤：
    // 1. 创建 std::queue<PuzzleState> 作为开放列表
    // 2. 创建 std::unordered_set<std::string> 记录已访问状态
    // 3. 创建 std::unordered_map<std::string, PuzzleState> 记录父节点（用于重建路径）
    // 4. 将初始状态加入队列和已访问集合
    // 5. 循环：当队列不为空时
    //    a. 从队列前端取出当前状态
    //    b. 如果当前状态是目标状态：
    //       - 从父节点映射中重建路径
    //       - 设置 result.success = true
    //       - 设置 result.path（从初始状态到目标状态的路径）
    //       - 跳出循环
    //    c. 获取当前状态的所有后继状态（getSuccessors）
    //    d. 对于每个后继状态：
    //       - 获取后继状态的字符串键（toString）
    //       - 如果不在已访问集合中：
    //         * 加入已访问集合
    //         * 加入队列
    //         * 在父节点映射中记录：parentMap[successorKey] = current
    //         * nodesExpanded++
    //
    // 示例代码框架：
    //   std::queue<PuzzleState> openSet;
    //   std::unordered_set<std::string> visited;
    //   std::unordered_map<std::string, PuzzleState> parentMap;
    //
    //   openSet.push(initialState);
    //   visited.insert(initialState.toString());
    //
    //   while (!openSet.empty()) {
    //       PuzzleState current = openSet.front();
    //       openSet.pop();
    //
    //       if (current.isGoalState()) {
    //           // 重建路径...
    //           result.success = true;
    //           break;
    //       }
    //
    //       for (const auto& successor : current.getSuccessors()) {
    //           std::string key = successor.toString();
    //           if (visited.find(key) == visited.end()) {
    //               visited.insert(key);
    //               openSet.push(successor);
    //               parentMap[key] = current;
    //               nodesExpanded++;
    //           }
    //       }
    //   }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeUsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.nodesExpanded = nodesExpanded;
    return result;
}

// ==================== A* 实现 ====================

SolveResult AStarSolver::solve() {
    SolveResult result;
    auto startTime = std::chrono::high_resolution_clock::now();

    // TODO: 实现A*算法（启发式搜索）
    //
    // 实现步骤：
    // 1. 创建优先队列（按fCost排序，使用自定义比较函数）
    // 2. 创建 std::unordered_set<std::string> 作为关闭列表
    // 3. 创建 std::unordered_map<std::string, PuzzleState> 记录父节点
    // 4. 创建 std::unordered_map<std::string, int> 记录每个状态的gCost
    // 5. 初始化起始状态：
    //    a. 创建初始状态的副本
    //    b. 设置 gCost = 0
    //    c. 设置 hCost = Heuristic::manhattanDistance(initialState)
    //    d. 加入优先队列
    // 6. 循环：当队列不为空时
    //    a. 取出fCost最小的状态（队列顶部）
    //    b. 如果已在关闭列表中，跳过（continue）
    //    c. 加入关闭列表
    //    d. 如果是目标状态：
    //       - 重建路径
    //       - 设置 result.success = true
    //       - 跳出循环
    //    e. 获取所有后继状态
    //    f. 对于每个后继状态：
    //       - 计算新的gCost = current.getGCost() + 1
    //       - 如果后继状态未访问过 或 新gCost更小：
    //         * 设置 successor.setGCost(newGCost)
    //         * 设置 successor.setHCost(Heuristic::manhattanDistance(successor))
    //         * 更新gCostMap[successorKey] = newGCost
    //         * 记录父节点：parentMap[successorKey] = current
    //         * 加入优先队列
    //         * nodesExpanded++
    //
    // 示例代码框架：
    //   std::priority_queue<PuzzleState, std::vector<PuzzleState>, CompareState> openSet;
    //   std::unordered_set<std::string> closedSet;
    //   std::unordered_map<std::string, PuzzleState> parentMap;
    //   std::unordered_map<std::string, int> gCostMap;
    //
    //   PuzzleState start = initialState;
    //   start.setGCost(0);
    //   start.setHCost(Heuristic::manhattanDistance(start));
    //   openSet.push(start);
    //   gCostMap[start.toString()] = 0;
    //
    //   while (!openSet.empty()) {
    //       PuzzleState current = openSet.top();
    //       openSet.pop();
    //
    //       if (closedSet.find(current.toString()) != closedSet.end()) continue;
    //       closedSet.insert(current.toString());
    //
    //       if (current.isGoalState()) {
    //           // 重建路径...
    //           result.success = true;
    //           break;
    //       }
    //
    //       for (auto successor : current.getSuccessors()) {
    //           int newGCost = current.getGCost() + 1;
    //           std::string successorKey = successor.toString();
    //
    //           if (gCostMap.find(successorKey) == gCostMap.end() ||
    //               newGCost < gCostMap[successorKey]) {
    //               successor.setGCost(newGCost);
    //               successor.setHCost(Heuristic::manhattanDistance(successor));
    //               gCostMap[successorKey] = newGCost;
    //               parentMap[successorKey] = current;
    //               openSet.push(successor);
    //               nodesExpanded++;
    //           }
    //       }
    //   }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeUsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.nodesExpanded = nodesExpanded;
    return result;
}

// ==================== IDA* 实现 ====================

SolveResult IDAStarSolver::solve() {
    SolveResult result;
    auto startTime = std::chrono::high_resolution_clock::now();

    // TODO: 实现IDA*算法（迭代加深A*）
    //
    // 实现步骤：
    // 1. 设置初始阈值 threshold = Heuristic::manhattanDistance(initialState)
    // 2. 清空路径向量 path
    // 3. 将初始状态加入路径
    // 4. 循环：
    //    a. 调用 searchResult = search(initialState, 0, threshold)
    //    b. 如果 searchResult == -1（找到解）：
    //       - 设置 result.success = true
    //       - 设置 result.path = path
    //       - 跳出循环
    //    c. 如果 searchResult == INT_MAX（无解）：
    //       - result.success = false
    //       - 跳出循环
    //    d. 否则：
    //       - 更新 threshold = searchResult（下一个最小的f值）
    //
    // 示例代码框架：
    //   threshold = Heuristic::manhattanDistance(initialState);
    //   path.clear();
    //   path.push_back(initialState);
    //
    //   while (true) {
    //       int temp = search(initialState, 0, threshold);
    //
    //       if (temp == -1) {
    //           result.path = path;
    //           result.success = true;
    //           break;
    //       }
    //
    //       if (temp == INT_MAX) {
    //           result.success = false;
    //           break;
    //       }
    //
    //       threshold = temp;
    //   }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeUsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.nodesExpanded = nodesExpanded;
    return result;
}

int IDAStarSolver::search(const PuzzleState& state, int g, int bound) {
    // TODO: 实现带f值上界的递归深度优先搜索
    //
    // 参数说明：
    //   state - 当前正在探索的状态
    //   g - 从起点到当前状态的代价（深度）
    //   bound - 当前的f值上界（阈值）
    //
    // 返回值：
    //   -1 表示找到目标
    //   INT_MAX 表示无解
    //   其他正整数：超出bound的最小f值（用于下一次迭代）
    //
    // 实现步骤：
    // 1. 计算 f = g + Heuristic::manhattanDistance(state)
    // 2. 如果 f > bound：
    //    - 返回 f（超出阈值，剪枝此分支）
    // 3. 如果 state.isGoalState()：
    //    - 返回 -1（找到解）
    // 4. 初始化 min = INT_MAX
    // 5. 获取state的所有后继状态
    // 6. 对于每个后继状态：
    //    a. 检查后继状态是否已在当前路径中（避免循环）：
    //       - 遍历path向量
    //       - 如果找到相同状态，跳过此后继状态
    //    b. 将后继状态加入path
    //    c. 递归调用： temp = search(successor, g+1, bound)
    //    d. 如果 temp == -1（找到解）：
    //       - 立即返回 -1
    //    e. 如果 temp < min：
    //       - 更新 min = temp（追踪最小的超出bound的f值）
    //    f. 从path中移除后继状态（回溯）
    //    g. nodesExpanded++
    // 7. 返回 min
    //
    // 示例代码框架：
    //   int f = g + Heuristic::manhattanDistance(state);
    //
    //   if (f > bound) return f;
    //   if (state.isGoalState()) return -1;
    //
    //   int min = INT_MAX;
    //
    //   for (const auto& successor : state.getSuccessors()) {
    //       // 检查是否在路径中
    //       bool inPath = false;
    //       for (const auto& s : path) {
    //           if (s == successor) {
    //               inPath = true;
    //               break;
    //           }
    //       }
    //
    //       if (!inPath) {
    //           path.push_back(successor);
    //           int temp = search(successor, g + 1, bound);
    //
    //           if (temp == -1) return -1;
    //           if (temp < min) min = temp;
    //
    //           path.pop_back();
    //           nodesExpanded++;
    //       }
    //   }
    //
    //   return min;

    return -1;
}
