# 8数码求解器 API 接口文档（一定要记得看）

## 📌 快速开始

### 必需的头文件
```cpp
#include "PuzzleState.h"   // 状态类
#include "Solver.h"        // 求解器
#include "Heuristic.h"     // 启发式函数（可选）
```

---

## 🎲 1. 生成随机状态

### 完整代码
```cpp
#include <random>
#include <algorithm>
#include "PuzzleState.h"

std::vector<int> generateRandomState() {
    std::vector<int> state = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    do {
        std::shuffle(state.begin(), state.end(), gen);
        PuzzleState temp(state);
        if (temp.isSolvable()) {
            break;
        }
    } while (true);
    
    return state;
}
```

### 使用方式
```cpp
// 生成随机初始状态
std::vector<int> initial = generateRandomState();

// 生成随机目标状态
std::vector<int> goal = generateRandomState();
```

**返回值：** `vector<int>` 包含 0-8，保证可解

---

## 🎯 2. 设置目标状态

```cpp
// 必须在求解前调用
PuzzleState::setGoalState(goal);
```

**参数：** `vector<int>` 目标状态  
**说明：** 静态方法，设置后全局生效

---

## 🔧 3. 创建状态对象

```cpp
// 从 vector 创建
std::vector<int> puzzle = {1, 2, 3, 4, 0, 5, 7, 8, 6};
PuzzleState state(puzzle);

// 检查可解性
if (state.isSolvable()) {
    // 有解，可以求解
}
```

---

## 🚀 4. 求解算法

### A* 算法（推荐）
```cpp
AStarSolver solver(initialState);
SolveResult result = solver.solve();
```

### BFS 算法
```cpp
BFSSolver solver(initialState);
SolveResult result = solver.solve();
```

### IDA* 算法
```cpp
IDAStarSolver solver(initialState);
SolveResult result = solver.solve();
```

---

## 📊 5. 获取结果

### SolveResult 结构
```cpp
struct SolveResult {
    bool success;                      // 是否找到解
    std::vector<PuzzleState> path;     // 解的路径
    int nodesExpanded;                 // 扩展节点数
    double timeUsed;                   // 用时（ms）
    size_t memoryUsed;                 // 内存（字节）
};
```

### 访问结果
```cpp
if (result.success) {
    int steps = result.path.size() - 1;       // 步数
    int nodes = result.nodesExpanded;          // 节点数
    double time = result.timeUsed;             // 时间
    double memory = result.memoryUsed / 1024.0; // 内存(KB)
    
    // 获取路径
    std::vector<PuzzleState> path = result.path;
}
```

---

## 🎨 6. 显示状态

### 获取棋盘值
```cpp
PuzzleState state = result.path[i];

for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
        int value = state.getBoard(row, col);
        // 在 UI 的 (row, col) 位置显示 value
        // value 范围：0-8，0 代表空格
    }
}
```

### 打印状态（调试用）
```cpp
state.print();  // 格式化输出到控制台
```

---

## 💡 7. 完整示例

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
    // 1. 生成随机状态
    std::vector<int> initial = generateRandomState();
    std::vector<int> goal = generateRandomState();
    
    // 2. 设置目标
    PuzzleState::setGoalState(goal);
    
    // 3. 创建初始状态
    PuzzleState initialState(initial);
    
    // 4. 检查可解性
    if (!initialState.isSolvable()) {
        std::cout << "无解！\n";
        return 1;
    }
    
    // 5. 求解（推荐使用 A*）
    AStarSolver solver(initialState);
    SolveResult result = solver.solve();
    
    // 6. 获取结果
    if (result.success) {
        std::cout << "找到解！\n";
        std::cout << "步数: " << result.path.size() - 1 << "\n";
        std::cout << "节点数: " << result.nodesExpanded << "\n";
        std::cout << "时间: " << result.timeUsed << " ms\n";
        
        // 7. 遍历路径
        for (size_t i = 0; i < result.path.size(); i++) {
            PuzzleState state = result.path[i];
            std::cout << "\n步骤 " << i << ":\n";
            
            // 获取每个格子的值
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 3; col++) {
                    int value = state.getBoard(row, col);
                    std::cout << value << " ";
                }
                std::cout << "\n";
            }
        }
    }
    
    return 0;
}
```

---

## 🖼️ 8. UI 集成流程

### 推荐流程
```
[生成随机状态] → [显示初始/目标] → [用户点击求解] → 
[后台求解] → [获取路径] → [动画显示]
```

### 伪代码示例
```cpp
// ========== UI 初始化 ==========
void UI_Init() {
    // 生成随机状态
    initial = generateRandomState();
    goal = generateRandomState();
    PuzzleState::setGoalState(goal);
    
    // 显示在界面上
    UI_ShowState(initial, "初始状态");
    UI_ShowState(goal, "目标状态");
}

// ========== 用户点击求解 ==========
void UI_OnSolveButtonClick() {
    // 创建状态
    PuzzleState state(initial);
    
    // 检查可解性
    if (!state.isSolvable()) {
        UI_ShowError("该问题无解！");
        return;
    }
    
    // 显示加载动画
    UI_ShowLoading();
    
    // 后台求解
    std::thread([this]() {
        AStarSolver solver(state);
        SolveResult result = solver.solve();
        
        // 回到主线程显示结果
        UI_OnSolveComplete(result);
    }).detach();
}

// ========== 显示求解结果 ==========
void UI_OnSolveComplete(SolveResult result) {
    UI_HideLoading();
    
    if (!result.success) {
        UI_ShowError("求解失败！");
        return;
    }
    
    // 显示统计信息
    UI_ShowStats(
        result.path.size() - 1,
        result.nodesExpanded,
        result.timeUsed,
        result.memoryUsed / 1024.0
    );
    
    // 播放动画
    UI_AnimatePath(result.path);
}

// ========== 播放路径动画 ==========
void UI_AnimatePath(std::vector<PuzzleState> path) {
    for (size_t i = 0; i < path.size(); i++) {
        // 更新显示
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                int value = path[i].getBoard(row, col);
                UI_UpdateCell(row, col, value);
            }
        }
        
        // 延迟
        Sleep(500);  // 500ms 每步
    }
}
```

---

## 📋 9. API 快速参考表

| 功能 | 代码 | 说明 |
|------|------|------|
| **生成随机状态** | `generateRandomState()` | 返回可解的 vector<int> |
| **设置目标** | `PuzzleState::setGoalState(goal)` | 全局设置 |
| **创建状态** | `PuzzleState state(puzzle)` | 从 vector 创建 |
| **检查可解性** | `state.isSolvable()` | 返回 bool |
| **A* 求解** | `AStarSolver(state).solve()` | 推荐 |
| **BFS 求解** | `BFSSolver(state).solve()` | 简单问题 |
| **IDA* 求解** | `IDAStarSolver(state).solve()` | 困难问题 |
| **获取步数** | `result.path.size() - 1` | int |
| **获取节点数** | `result.nodesExpanded` | int |
| **获取时间** | `result.timeUsed` | double (ms) |
| **获取内存** | `result.memoryUsed / 1024.0` | double (KB) |
| **获取路径** | `result.path[i]` | PuzzleState |
| **获取格子值** | `state.getBoard(row, col)` | 0-8 |

---

## ⚠️ 10. 注意事项

### 1. 线程安全
```cpp
// ❌ 错误：多个线程同时调用
PuzzleState::setGoalState(goal1);  // 线程1
PuzzleState::setGoalState(goal2);  // 线程2

// ✅ 正确：在主线程设置一次
PuzzleState::setGoalState(goal);
// 然后在多个线程中求解
```

### 2. 内存管理
```cpp
// path 是 vector，会自动释放
SolveResult result = solver.solve();
// result 超出作用域时自动清理
```

### 3. 求解时间
```cpp
// 简单问题（<10步）：毫秒级
// 中等问题（10-20步）：几毫秒到几十毫秒
// 困难问题（20-30步）：几十毫秒到几百毫秒
// 极难问题（30+步）：可能需要几秒（BFS）

// 建议：UI 在后台线程运行，显示加载动画
```

### 4. 无解情况
```cpp
// 必须先检查可解性
if (!state.isSolvable()) {
    // 提示用户该问题无解
    return;
}
```

---

## 🎓 11. 常见问题

### Q1: 如何在 UI 中显示数字？
```cpp
int value = state.getBoard(row, col);
if (value == 0) {
    // 显示空格（可以用特殊图标）
} else {
    // 显示数字 1-8
}
```

### Q2: 如何让动画更流畅？
```cpp
// 方案1：逐步显示
for (size_t i = 0; i < path.size(); i++) {
    updateUI(path[i]);
    Sleep(300);  // 调整延迟
}

// 方案2：使用定时器
startTimer(300ms, [&, step = 0]() mutable {
    if (step < path.size()) {
        updateUI(path[step++]);
    } else {
        stopTimer();
    }
});
```

### Q3: 如何选择算法？
```cpp
// 简单问题：任意算法
// 中等问题：A* (推荐)
// 困难问题：IDA* 或 A*
// 极难问题：IDA*
```

### Q4: 状态表示格式？
```cpp
// vector 的顺序：
// [0] [1] [2]
// [3] [4] [5]
// [6] [7] [8]

// 例如 {1,2,3,4,0,5,7,8,6} 表示：
// 1 2 3
// 4 0 5
// 7 8 6
```

---

## 📞 联系方式

如有问题，请联系项目负责人或查看完整文档 README.md

---

**祝开发顺利！** 🎉
