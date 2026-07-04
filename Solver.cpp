/**
 * @file Solver.cpp
 * @brief 搜索算法的实现
 * @author YanYun
 * @date 2026-06-30
 */

#include "Solver.h"
#include "Heuristic.h"
#include <chrono>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

// ==================== BFS 实现 ====================

SolveResult BFSSolver::solve() {
    SolveResult result;
    auto startTime = std::chrono::high_resolution_clock::now();

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

    std::queue<PuzzleState> openSet;
    std::unordered_set<std::string> visited;    //记录已扩展的状态
    std::unordered_map<std::string, PuzzleState> parentMap;     //记录父节点，用于重建路径

    openSet.push(initialState);
    visited.insert(initialState.toString());

    while(!openSet.empty()) {
        PuzzleState current = openSet.front();
        openSet.pop();

        nodesExpanded++;

        if (current.isGoalState()) {
            // 重建路径
            std::vector<PuzzleState> path;
            std::string key = current.toString();
            while (parentMap.find(key) != parentMap.end()) {
                path.push_back(current);
                current = parentMap[key];
                key = current.toString();
            }
            path.push_back(initialState);
            std::reverse(path.begin(), path.end());
            result.path = path;
            result.success = true;
            break;
        }

        for (const auto& successor : current.getSuccessors()) {
             // 范围for循环，对每个 successor 执行操作
            std::string key = successor.toString();
            if (visited.find(key) == visited.end()) {
                visited.insert(key);
                openSet.push(successor);
                parentMap[key] = current;
            }
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeUsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.nodesExpanded = nodesExpanded;

    // 计算 BFS 内存使用（估算）
    size_t stateSize = sizeof(PuzzleState);
    size_t stringSize = 10;  // 平均字符串大小
    result.memoryUsed = visited.size() * stringSize +           // visited set
                        openSet.size() * stateSize +             // queue
                        parentMap.size() * (stringSize + stateSize);  // parent map

    return result;
}

// ==================== A* 实现 ====================

//比较器
struct CompareState {
    bool operator()(const PuzzleState& a, const PuzzleState& b) const {
        return a.getFCost() > b.getFCost(); // fCost越小优先级越高
    }
};

SolveResult AStarSolver::solve() {
    SolveResult result;
    auto startTime = std::chrono::high_resolution_clock::now();

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
    
    std::priority_queue<PuzzleState, std::vector<PuzzleState>, CompareState> openSet;   //优先队列，按fCost排序,小的先
    std::unordered_set<std::string> closedSet;   //记录已扩展的状态
    std::unordered_map<std::string, PuzzleState> parentMap;
    std::unordered_map<std::string, int> gCostMap;      // 记录每个状态的gCost

    PuzzleState start = initialState;
    start.setGCost(0);
    start.setHCost(Heuristic::manhattanDistance(start));
    openSet.push(start);
    gCostMap[start.toString()] = 0;

    while(!openSet.empty()) {
        //取出f值最小的状态
        PuzzleState current = openSet.top();
        openSet.pop();

        std::string currentKey = current.toString();
        if (closedSet.find(currentKey) != closedSet.end()) {
            continue; // 已经扩展过，跳过
        }

        closedSet.insert(currentKey);
        nodesExpanded++;

        if(current.isGoalState()) {
            // 重建路径
            std::vector<PuzzleState> path;
            while (parentMap.find(currentKey) != parentMap.end()) {
                path.push_back(current);
                current = parentMap[currentKey];
                currentKey = current.toString();
            }
            path.push_back(initialState);
            std::reverse(path.begin(), path.end());
            result.path = path;
            result.success = true;
            break;
        }

        for (auto& successor : current.getSuccessors()) {
            std::string key = successor.toString();
            int newGCost = current.getGCost() + 1;

            if(gCostMap.find(key) == gCostMap.end() || newGCost < gCostMap[key]) {      //// 检查：是否未访问过，或找到了更好的路径
                // 更新gCost和hCost
                successor.setGCost(newGCost);
                successor.setHCost(Heuristic::manhattanDistance(successor));
                gCostMap[key] = newGCost;
                parentMap[key] = current;

                openSet.push(successor);    // 加入优先队列
            }

        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeUsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.nodesExpanded = nodesExpanded;

    // 计算 A* 内存使用（估算）
    size_t stateSize = sizeof(PuzzleState);
    size_t stringSize = 10;  // 平均字符串大小
    result.memoryUsed = closedSet.size() * stringSize +         // closed set
                        openSet.size() * stateSize +             // priority queue
                        parentMap.size() * (stringSize + stateSize) +  // parent map
                        gCostMap.size() * (stringSize + sizeof(int));   // gCost map

    return result;
}

// ==================== IDA* 实现 ====================

SolveResult IDAStarSolver::solve() {
    SolveResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
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

    threshold = Heuristic::manhattanDistance(initialState);
    path.clear();
    path.push_back(initialState);

    while (true) {
        int searchResult = search(initialState, 0, threshold);

        if (searchResult == -1) {
            result.path = path;
            result.success = true;
            break;
        }

        if (searchResult == INT_MAX) {
            result.success = false;
            break;
        }

        threshold = searchResult;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeUsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.nodesExpanded = nodesExpanded;

    // 计算 IDA* 内存使用（估算）
    size_t stateSize = sizeof(PuzzleState);
    // IDA* 只需要存储当前路径
    result.memoryUsed = path.capacity() * stateSize;  // path vector

    return result;
}

int IDAStarSolver::search(const PuzzleState& state, int g, int bound) {
    // 参数说明：
    //   state - 当前正在探索的状态
    //   g - 从起点到当前状态的代价（深度）
    //   bound - 当前的f值上界（阈值）

    int f = g + Heuristic::manhattanDistance(state);

    if (f > bound)
        return f;   // 超出阈值，剪枝此分支
    if (state.isGoalState()) 
        return -1;

    nodesExpanded++;
       
    int min = INT_MAX;

    for (const auto& successor : state.getSuccessors()) {
        // 检查是否在路径中
        bool inPath = false;
        for (const auto& s : path) {
            if (s == successor) {
                inPath = true;
                break;
            }
        }

        if (!inPath) {
            path.push_back(successor);
            int temp = search(successor, g + 1, bound);

            if (temp == -1) 
                return -1;
            if (temp < min) 
                min = temp;

            path.pop_back();
        }
    }

    return min;
}
