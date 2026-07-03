# 给 UI 开发同学的快速指南

## 📚 文档位置

- **API_GUIDE.md** - 完整的 API 接口文档（推荐先看这个）
- **README.md** - 项目完整说明文档

---

## 🚀 5分钟快速上手

### 1. 包含头文件
```cpp
#include "PuzzleState.h"
#include "Solver.h"
```

### 2. 复制这个函数到你的代码
```cpp
#include <random>
#include <algorithm>

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
```

### 3. 完整流程代码
```cpp
// 生成随机状态
std::vector<int> initial = generateRandomState();
std::vector<int> goal = generateRandomState();

// 设置目标
PuzzleState::setGoalState(goal);

// 创建初始状态并检查
PuzzleState initialState(initial);
if (!initialState.isSolvable()) {
    // 显示"无解"错误
    return;
}

// 求解（推荐 A*）
AStarSolver solver(initialState);
SolveResult result = solver.solve();

// 获取结果
if (result.success) {
    int steps = result.path.size() - 1;        // 步数
    int nodes = result.nodesExpanded;           // 节点数
    double time = result.timeUsed;              // 时间(ms)
    double memory = result.memoryUsed / 1024.0; // 内存(KB)
    
    // 获取路径显示动画
    for (size_t i = 0; i < result.path.size(); i++) {
        PuzzleState state = result.path[i];
        
        // 更新 UI（3x3 棋盘）
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                int value = state.getBoard(row, col);
                // 在 UI 的 (row, col) 显示 value
                // value = 0 表示空格，1-8 是数字
                updateUICell(row, col, value);
            }
        }
        
        // 延迟（可选）
        Sleep(500);  // 每步 500ms
    }
}
```

---

## 🎨 UI 需要实现的功能

### 必需功能
1. **显示初始状态**（3x3 网格）
2. **显示目标状态**（3x3 网格）
3. **"求解"按钮**
4. **显示结果**（步数、时间、节点数、内存）
5. **播放动画**（逐步显示路径）

### 可选功能
1. **暂停/继续动画**
2. **调整动画速度**
3. **显示当前步数 / 总步数**
4. **选择算法**（BFS / A* / IDA*）
5. **重新生成状态**

---

## 📊 数据格式说明

### 状态表示
```cpp
// vector<int> 的顺序对应棋盘位置：
// [0] [1] [2]     位置 0  位置 1  位置 2
// [3] [4] [5]  =  位置 3  位置 4  位置 5
// [6] [7] [8]     位置 6  位置 7  位置 8

// 例如：{1,2,3,4,0,5,7,8,6} 表示：
// 1 2 3
// 4 0 5  （0 是空格）
// 7 8 6
```

### 访问单个格子
```cpp
PuzzleState state(...);
int value = state.getBoard(row, col);  // row, col 都是 0-2

// 例如：
state.getBoard(0, 0) → 左上角
state.getBoard(1, 1) → 中心
state.getBoard(2, 2) → 右下角
```

---

## ⚡ 算法选择建议

| 算法 | 速度 | 内存 | 推荐场景 |
|------|------|------|---------|
| **A*** | 快 | 中 | **通用推荐** |
| BFS | 慢 | 大 | 简单问题 |
| IDA* | 最快 | 最小 | 困难问题 |

**建议：UI 默认使用 A***

---

## 🧵 多线程建议

### 为什么需要多线程？
- 求解可能需要几十毫秒到几百毫秒
- 在主线程求解会卡住 UI

### 简单示例
```cpp
#include <thread>

void OnSolveButtonClick() {
    // 显示"正在求解..."
    ShowLoading();
    
    // 在后台线程求解
    std::thread([this]() {
        AStarSolver solver(initialState);
        SolveResult result = solver.solve();
        
        // 回到主线程更新 UI
        PostToMainThread([this, result]() {
            HideLoading();
            ShowResult(result);
        });
    }).detach();
}
```

---

## 🎯 推荐的 UI 布局

```
┌─────────────────────────────────────┐
│         8数码问题求解器              │
├──────────────┬──────────────────────┤
│  初始状态     │   目标状态           │
│  ┌─┬─┬─┐    │   ┌─┬─┬─┐          │
│  │1│2│3│    │   │5│4│1│          │
│  ├─┼─┼─┤    │   ├─┼─┼─┤          │
│  │4│ │5│    │   │8│7│2│          │
│  ├─┼─┼─┤    │   ├─┼─┼─┤          │
│  │7│8│6│    │   │3│6│ │          │
│  └─┴─┴─┘    │   └─┴─┴─┘          │
├──────────────┴──────────────────────┤
│  [生成新问题]  [开始求解]  [暂停]  │
├─────────────────────────────────────┤
│  当前显示：                         │
│  ┌─┬─┬─┐                          │
│  │1│2│3│    步骤: 5/21            │
│  ├─┼─┼─┤                          │
│  │4│ │5│    进度: [=====>    ]   │
│  ├─┼─┼─┤                          │
│  │7│8│6│                          │
│  └─┴─┴─┘                          │
├─────────────────────────────────────┤
│  统计信息：                         │
│  步数: 21                           │
│  扩展节点数: 462                    │
│  用时: 1.16 ms                      │
│  内存: 72 KB                        │
└─────────────────────────────────────┘
```

---

## ⚠️ 常见错误处理

### 1. 无解问题
```cpp
if (!initialState.isSolvable()) {
    MessageBox("该问题无解！请重新生成");
    return;
}
```

### 2. 求解失败
```cpp
if (!result.success) {
    MessageBox("求解失败！");
    return;
}
```

### 3. 路径为空
```cpp
if (result.path.empty()) {
    MessageBox("未找到路径！");
    return;
}
```

---

## 📞 需要帮助？

1. **详细文档**：查看 `API_GUIDE.md`
2. **完整示例**：查看 `main.cpp`
3. **问题咨询**：联系后端负责人

---

## ✅ 检查清单

开始开发前确认：
- [ ] 已阅读 API_GUIDE.md
- [ ] 已复制 generateRandomState() 函数
- [ ] 已包含头文件 PuzzleState.h 和 Solver.h
- [ ] 已理解状态表示格式（vector<int>）
- [ ] 已理解如何获取路径（result.path）
- [ ] 已理解如何获取单个格子值（getBoard）

---

**开发愉快！** 🚀

有问题随时问后端同学！
