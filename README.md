# 8数码问题求解器 (8-Puzzle Solver)

基于 C++ 实现的八数码问题求解器，对比 BFS、A* 和 IDA* 算法的性能差异。

8-Puzzle solver with BFS, A*, and IDA* algorithms in C++. Includes performance benchmarking.

---

## UI 开发接口说明

### **核心接口概览**

为了方便 UI 开发，本项目提供了清晰的 API 接口。所有核心功能都封装在以下类中：

```cpp
#include "PuzzleState.h"   // 状态类
#include "Solver.h"        // 求解器类
#include "Heuristic.h"     // 启发式函数（可选）
```

---

### **1. 生成随机状态**

#### **生成随机可解状态**
```cpp
#include <random>
#include <algorithm>
#include "PuzzleState.h"

// 生成随机可解的八数码状态
std::vector<int> generateRandomState() {
    std::vector<int> state = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    do {
        std::shuffle(state.begin(), state.end(), gen);
        PuzzleState temp(state);
        if (temp.isSolvable()) {  // 检查可解性
            break;
        }
    } while (true);
    
    return state;
}

// 使用示例
std::vector<int> initialPuzzle = generateRandomState();
std::vector<int> goalPuzzle = generateRandomState();
```

**说明：**
- 返回值是一个 `vector<int>`，包含 0-8 九个数字
- 0 代表空格
- 保证返回的状态是可解的

---

### **2. 设置目标状态**

#### **设置自定义目标**
```cpp
#include "PuzzleState.h"

// 设置目标状态（静态方法，全局生效）
std::vector<int> goal = {1, 2, 3, 4, 5, 6, 7, 8, 0};
PuzzleState::setGoalState(goal);

// 或者使用随机生成的目标
std::vector<int> randomGoal = generateRandomState();
PuzzleState::setGoalState(randomGoal);
```

**说明：**
- 必须在创建求解器之前调用
- 一次设置，所有算法都使用该目标
- 默认目标是 `{1,2,3,4,5,6,7,8,0}`

---

### **3. 创建状态对象**

#### **从 vector 创建状态**
```cpp
#include "PuzzleState.h"

// 方式1：从一维数组创建
std::vector<int> puzzle = {1, 2, 3, 4, 0, 5, 7, 8, 6};
PuzzleState state(puzzle);

// 方式2：从二维数组创建
int board[3][3] = {{1,2,3}, {4,0,5}, {7,8,6}};
PuzzleState state2(board);
```

#### **检查可解性**
```cpp
PuzzleState state(puzzle);
if (state.isSolvable()) {
    std::cout << "该问题有解\n";
} else {
    std::cout << "该问题无解\n";
}
```

---

### **4. 调用求解器**

#### **使用 A* 算法（推荐）**
```cpp
#include "Solver.h"

// 创建初始状态
std::vector<int> puzzle = {8, 6, 7, 2, 5, 4, 3, 0, 1};
PuzzleState initialState(puzzle);

// 创建 A* 求解器
AStarSolver solver(initialState);

// 开始求解
SolveResult result = solver.solve();

// 检查是否找到解
if (result.success) {
    std::cout << "找到解！\n";
    std::cout << "步数: " << result.path.size() - 1 << "\n";
    std::cout << "扩展节点数: " << result.nodesExpanded << "\n";
    std::cout << "内存使用: " << result.memoryUsed / 1024.0 << " KB\n";
    std::cout << "用时: " << result.timeUsed << " ms\n";
} else {
    std::cout << "未找到解\n";
}
```

#### **使用其他算法**
```cpp
// BFS（广度优先搜索）
BFSSolver bfsSolver(initialState);
SolveResult bfsResult = bfsSolver.solve();

// IDA*（迭代加深 A*）
IDAStarSolver idaSolver(initialState);
SolveResult idaResult = idaSolver.solve();
```

---

### **5. 获取求解结果**

#### **SolveResult 结构体**
```cpp
struct SolveResult {
    bool success;                      // 是否找到解
    std::vector<PuzzleState> path;     // 解的路径（从初始到目标）
    int nodesExpanded;                 // 扩展的节点数
    double timeUsed;                   // 用时（毫秒）
    size_t memoryUsed;                 // 内存使用（字节）
};
```

#### **访问结果字段**
```cpp
SolveResult result = solver.solve();

// 基本信息
bool found = result.success;               // 是否找到解
int steps = result.path.size() - 1;        // 步数
int nodes = result.nodesExpanded;          // 扩展节点数
double time = result.timeUsed;             // 时间（ms）
double memory = result.memoryUsed / 1024.0; // 内存（KB）

// 获取路径
std::vector<PuzzleState> path = result.path;
```

---

### **6. 获取路径信息**

#### **遍历解的路径**
```cpp
if (result.success) {
    std::cout << "解的步骤：\n";
    for (size_t i = 0; i < result.path.size(); i++) {
        std::cout << "步骤 " << i << ":\n";
        
        PuzzleState state = result.path[i];
        
        // 获取棋盘状态（3x3）
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                int value = state.getBoard(row, col);
                std::cout << value << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}
```

#### **打印单个状态**
```cpp
PuzzleState state = result.path[i];
state.print();  // 使用内置的格式化打印
```

---

### **7. 完整示例代码**

#### **示例1：随机状态 + A* 求解**
```cpp
#include <iostream>
#include <random>
#include <algorithm>
#include "PuzzleState.h"
#include "Solver.h"

// 生成随机可解状态
std::vector<int> generateRandomState() {
    std::vector<int> state = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::random_device rd;
    std::mt19937 gen(rd());
    do {
        std::shuffle(state.begin(), state.end(), gen);
        PuzzleState temp(state);
        if (temp.isSolvable()) break;
    } while (true);
    return state;
}

int main() {
    // 1. 生成随机初始状态和目标状态
    std::vector<int> initial = generateRandomState();
    std::vector<int> goal = generateRandomState();
    
    // 2. 设置目标状态
    PuzzleState::setGoalState(goal);
    
    // 3. 创建初始状态
    PuzzleState initialState(initial);
    
    // 4. 检查可解性
    if (!initialState.isSolvable()) {
        std::cout << "该问题无解！\n";
        return 1;
    }
    
    // 5. 使用 A* 求解
    AStarSolver solver(initialState);
    SolveResult result = solver.solve();
    
    // 6. 显示结果
    if (result.success) {
        std::cout << "找到解！\n";
        std::cout << "步数: " << result.path.size() - 1 << "\n";
        std::cout << "扩展节点数: " << result.nodesExpanded << "\n";
        std::cout << "用时: " << result.timeUsed << " ms\n";
        
        // 7. 获取路径供 UI 使用
        for (const auto& state : result.path) {
            // UI 可以在这里获取每一步的状态
            // state.getBoard(row, col) 获取具体位置的值
        }
    }
    
    return 0;
}
```

---

### **8. UI 集成建议**

#### **推荐的 UI 工作流程**

```
用户操作 → UI 层 → 核心算法 → 返回结果 → UI 显示
```

**具体步骤：**

1. **初始化**
   ```cpp
   // UI 生成随机状态
   std::vector<int> initial = generateRandomState();
   std::vector<int> goal = generateRandomState();
   PuzzleState::setGoalState(goal);
   ```

2. **显示状态**
   ```cpp
   // UI 显示初始状态和目标状态
   PuzzleState initialState(initial);
   PuzzleState goalState(goal);
   
   // 获取每个格子的值
   for (int i = 0; i < 3; i++) {
       for (int j = 0; j < 3; j++) {
           int value = initialState.getBoard(i, j);
           // 在 UI 的 (i, j) 位置显示 value
       }
   }
   ```

3. **求解**
   ```cpp
   // UI 调用求解器（可在后台线程运行）
   AStarSolver solver(initialState);
   SolveResult result = solver.solve();
   ```

4. **动画显示**
   ```cpp
   // UI 逐步显示路径
   for (size_t step = 0; step < result.path.size(); step++) {
       PuzzleState state = result.path[step];
       
       // 延迟显示动画
       Sleep(500);  // 或 std::this_thread::sleep_for
       
       // 更新 UI 显示当前状态
       for (int i = 0; i < 3; i++) {
           for (int j = 0; j < 3; j++) {
               updateUICell(i, j, state.getBoard(i, j));
           }
       }
   }
   ```

5. **显示统计信息**
   ```cpp
   // UI 显示性能数据
   showStatistics(
       result.path.size() - 1,           // 步数
       result.nodesExpanded,              // 节点数
       result.timeUsed,                   // 时间
       result.memoryUsed / 1024.0         // 内存(KB)
   );
   ```

---

### **9. 常见问题处理**

#### **问题1：如何判断两个状态是否相同？**
```cpp
PuzzleState state1(puzzle1);
PuzzleState state2(puzzle2);

if (state1 == state2) {
    std::cout << "两个状态相同\n";
}
```

#### **问题2：如何获取状态的字符串表示？**
```cpp
PuzzleState state(puzzle);
std::string stateStr = state.toString();  // 例如 "123405786"
```

#### **问题3：求解时间太长怎么办？**
```cpp
// 使用 IDA* 算法（内存小，速度快）
IDAStarSolver solver(initialState);
SolveResult result = solver.solve();

// 或者设置超时机制（需要在后台线程中运行）
// 可以定期检查求解是否完成
```

#### **问题4：如何在 UI 中显示"正在求解"？**
```cpp
// 方案：在单独的线程中运行求解
#include <thread>
#include <future>

std::future<SolveResult> future = std::async(std::launch::async, [&]() {
    AStarSolver solver(initialState);
    return solver.solve();
});

// UI 线程可以显示加载动画
while (future.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
    // 更新 UI 加载动画
    updateLoadingAnimation();
}

// 获取结果
SolveResult result = future.get();
```

---

### **10. API 快速参考**

| 功能 | 类/函数 | 说明 |
|------|---------|------|
| 生成随机状态 | `generateRandomState()` | 返回可解的随机状态 |
| 设置目标 | `PuzzleState::setGoalState(goal)` | 设置全局目标状态 |
| 检查可解性 | `state.isSolvable()` | 返回 true/false |
| 创建求解器 | `AStarSolver(initialState)` | 推荐使用 A* |
| 开始求解 | `solver.solve()` | 返回 SolveResult |
| 获取步数 | `result.path.size() - 1` | 解的步数 |
| 获取路径 | `result.path[i]` | 第 i 步的状态 |
| 获取格子值 | `state.getBoard(row, col)` | 返回 0-8 |
| 打印状态 | `state.print()` | 格式化输出 |

---

## 项目结构

```
8-Puzzle-Solver-CPP/
├── PuzzleState.h/cpp      # 状态表示类 
├── Heuristic.h/cpp        # 启发式函数 
├── Solver.h/cpp           # 三种搜索算法 
├── main.cpp               # 简单测试程序 
├── main_test.cpp          # 完整测试套件 
└── build/                 # 编译输出目录
```

---

## 🎯 功能特性

- ✅ **PuzzleState 状态类** - 完整实现
  - 状态表示和移动操作
  - 可解性判断（逆序对算法）
  - 支持自定义目标状态
  - 哈希函数支持

- ✅ **启发式函数** - 完整实现
  - 曼哈顿距离（Manhattan Distance）⭐ 推荐
  - 错位瓦片数（Misplaced Tiles）

- ✅ **三种搜索算法** - 完整实现
  - BFS（广度优先搜索）
  - A*（启发式搜索）
  - IDA*（迭代加深A*）

- ✅ **性能对比** - 完整实现
  - 节点扩展数统计
  - 运行时间统计
  - 对比报告生成

---

## 🚀 编译与运行

### 方法1：使用 g++ (推荐)
```bash
# 编译所有源文件
g++ -c PuzzleState.cpp -o build/PuzzleState.o -std=c++17
g++ -c Heuristic.cpp -o build/Heuristic.o -std=c++17
g++ -c Solver.cpp -o build/Solver.o -std=c++17
g++ -c main.cpp -o build/main.o -std=c++17

# 链接生成可执行文件
g++ build/*.o -o build/PuzzleSolver.exe -std=c++17

# 运行
./build/PuzzleSolver.exe
```

### 方法2：一步编译
```bash
g++ -std=c++17 -o build/puzzle.exe main.cpp PuzzleState.cpp Heuristic.cpp Solver.cpp
./build/puzzle.exe
```

### 方法3：使用完整测试套件
```bash
g++ -std=c++17 -o build/test.exe main_test.cpp PuzzleState.cpp Heuristic.cpp Solver.cpp
./build/test.exe
```

---

## 📊 性能测试结果

### 简单问题（2步）
```
初始: 1 2 3    目标: 1 2 3
     4 5 6          4 5 6
     0 7 8          7 8 0
```

| 算法 | 扩展节点 | 时间 | 相对效率 |
|------|---------|------|---------|
| BFS  | 7       | 0.05ms | 1x |
| A*   | 3       | 0.01ms | 5x faster |
| IDA* | 2       | 0.00ms | 快 |

### 困难问题（31步）
```
初始: 8 6 7    目标: 1 2 3
     2 5 4          4 5 6
     3 0 1          7 8 0
```

| 算法 | 扩展节点 | 时间 | 相对效率 |
|------|---------|------|---------|
| BFS  | 181,439 | 313ms | 1x |
| A*   | 7,863   | 21ms  | **15x faster** ⭐ |
| IDA* | 14,195  | 9.6ms | **33x faster** 🏆 |

**关键发现：**
- ✅ 所有算法都找到相同的最短路径
- 🚀 A* 将节点扩展减少了 **97%**
- 🏆 IDA* 在困难问题上最快

---

## 📚 算法详解

### 1. BFS（广度优先搜索）
- **特点**：按层级扩展，保证最短路径
- **优点**：简单易懂，一定能找到最优解
- **缺点**：空间复杂度高 O(b^d)，在困难问题上很慢
- **适用**：简单问题（≤15步）

### 2. A*（启发式搜索）⭐ **推荐**
- **特点**：使用 f = g + h 评估函数，优先扩展最有希望的节点
- **优点**：效率高，节点扩展少，保证最优解
- **缺点**：需要存储所有访问过的状态
- **适用**：中等问题（15-30步）
- **启发式**：曼哈顿距离

### 3. IDA*（迭代加深A*）
- **特点**：深度优先 + 迭代加深 + f值剪枝
- **优点**：空间复杂度极低 O(d)，困难问题上最快
- **缺点**：会重复访问节点
- **适用**：困难问题（30+步）或内存受限环境

---

## 🧮 启发式函数

### 曼哈顿距离（Manhattan Distance）⭐ 推荐
```cpp
h(n) = Σ |current_x - goal_x| + |current_y - goal_y|
```
- **可采纳**（admissible）：永远不会高估实际距离
- 效率高，八数码问题的最佳选择
- A* 和 IDA* 默认使用此函数

### 错位瓦片数（Misplaced Tiles）
```cpp
h(n) = 不在正确位置的瓦片数（不计空格）
```
- 可采纳，但不够"紧"
- 效率低于曼哈顿距离

---

## 💡 使用示例

```cpp
#include "PuzzleState.h"
#include "Solver.h"

// 创建初始状态
std::vector<int> puzzle = {8, 6, 7, 2, 5, 4, 3, 0, 1};
PuzzleState initial(puzzle);

// 检查可解性
if (!initial.isSolvable()) {
    std::cout << "此问题无解！\n";
    return;
}

// 使用 A* 求解（推荐）
AStarSolver solver(initial);
SolveResult result = solver.solve();

if (result.success) {
    std::cout << "步数: " << result.path.size() - 1 << "\n";
    std::cout << "扩展节点: " << result.nodesExpanded << "\n";
    std::cout << "时间: " << result.timeUsed << "ms\n";
    
    // 打印路径
    for (const auto& state : result.path) {
        state.print();
    }
}
```

---

## 🔧 自定义目标状态

项目支持自定义目标状态（默认为 `{1,2,3,4,5,6,7,8,0}`）：

```cpp
// 设置自定义目标状态
std::vector<int> customGoal = {0,1,2,3,4,5,6,7,8};
PuzzleState::setGoalState(customGoal);

// 现在所有算法会向这个新目标求解
```

---

## 🧪 测试用例

项目包含多个难度的测试用例：

```cpp
// 极简单（1-2步）
{1, 2, 3, 4, 5, 6, 0, 7, 8}
{1, 2, 3, 4, 5, 6, 7, 0, 8}

// 简单（5-10步）
{1, 2, 3, 4, 0, 5, 7, 8, 6}

// 困难（20-30步）
{8, 6, 7, 2, 5, 4, 3, 0, 1}

// 极难（30+步）- BFS 会很慢
{8, 7, 6, 5, 4, 3, 2, 1, 0}
```

---

## 🎯 算法选择建议

| 问题难度 | 推荐算法 | 原因 |
|---------|---------|------|
| 简单（<10步） | BFS 或 A* | 都很快 |
| 中等（10-25步） | **A*** ⭐ | 最佳平衡 |
| 困难（25-35步） | **A*** 或 IDA* | A*更快，IDA*省内存 |
| 极难（35+步） | **IDA*** | 唯一可行 |

---

## 🔑 关键技术点

1. **可解性判断**：通过计算逆序对数量，偶数则有解
2. **状态去重**：使用 `toString()` 将状态转为字符串作为哈希键
3. **路径重建**：通过 `parentMap` 记录父子关系，回溯完整路径
4. **优先队列**：A* 使用自定义比较器，按 f 值排序
5. **f值剪枝**：IDA* 的核心，超过阈值立即返回
6. **避免循环**：IDA* 的 `path` 向量记录当前路径，防止重复访问

---

## 📈 性能优化建议

1. **使用 A\***：在大多数情况下是最佳选择
2. **曼哈顿距离**：比错位瓦片数效率高得多
3. **预先检查可解性**：避免无解问题的无效搜索
4. **BFS 谨慎使用**：只在简单问题上使用，困难问题会内存溢出

---

## 👨‍💻 作者

**YanYun**

---

## 📅 开发日期

2026-06-30 ~ 2026-07-03

---

## 📄 许可证

本项目仅供学习交流使用。

---

## 🙏 致谢

感谢所有为本项目提供帮助和建议的人！
