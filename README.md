# 8-Puzzle-Solver-CPP

基于 C++ 实现的八数码问题求解器。对比 BFS、A* 和 IDA* 算法的性能差异，包含 EasyX 动画可视化。

8-Puzzle solver with BFS, A*, and IDA* algorithms in C++. Includes performance benchmarking and GUI visualization.

---

## 📁 项目结构

```
8-Puzzle-Solver-CPP/
├── PuzzleState.h/cpp       # 状态类（完整实现）
├── Heuristic.h/cpp         # 启发式函数（待实现）
├── Solver.h/cpp            # 三个搜索算法（待实现）
├── main.cpp                # 主程序和测试
├── CMakeLists.txt          # CMake 构建配置
└── README.md               # 项目说明
```

---

## 🎯 功能特性

- ✅ **PuzzleState 状态类** - 完整实现
  - 状态表示和移动操作
  - 可解性判断（逆序数法）
  - 哈希函数支持

- 📝 **三种搜索算法** - 待实现（有详细TODO）
  - BFS（广度优先搜索）
  - A*（启发式搜索）
  - IDA*（迭代加深A*）

- 📝 **启发式函数** - 待实现（有详细TODO）
  - 曼哈顿距离
  - 错位数

- ⏱️ **性能对比** - 框架已搭建
  - 节点扩展数统计
  - 运行时间统计
  - 对比报告生成

---

## 🔧 编译方法

### 方法1：使用 g++
```bash
g++ -std=c++17 -o puzzle.exe main.cpp PuzzleState.cpp Heuristic.cpp Solver.cpp
./puzzle.exe
```

### 方法2：使用 MSVC
```bash
cl /EHsc /std:c++17 /utf-8 /nologo /Fe:puzzle.exe main.cpp PuzzleState.cpp Heuristic.cpp Solver.cpp
puzzle.exe
```

### 方法3：使用 CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
./puzzle
```

### 方法4：使用 Make
```bash
make
./puzzle.exe
```

---

## 📝 实现步骤

### 1. 启发式函数（Heuristic.cpp）
- [ ] `manhattanDistance()` - 曼哈顿距离
- [ ] `misplacedTiles()` - 错位数

### 2. BFS 算法（Solver.cpp）
- [ ] `BFSSolver::solve()` - 广度优先搜索

### 3. A* 算法（Solver.cpp）
- [ ] `AStarSolver::solve()` - A*搜索

### 4. IDA* 算法（Solver.cpp）
- [ ] `IDAStarSolver::solve()` - 迭代加深
- [ ] `IDAStarSolver::search()` - 递归搜索

### 5. 测试和对比（main.cpp）
- [ ] 取消注释测试代码
- [ ] 运行性能对比
- [ ] 生成结果报告

---

## 🧪 测试用例

```cpp
// 简单问题（3步）
std::vector<int> easy = {1, 2, 3, 4, 5, 6, 0, 7, 8};

// 中等问题（约10步）
std::vector<int> medium = {1, 2, 3, 4, 0, 5, 7, 8, 6};

// 困难问题（约20步）
std::vector<int> hard = {8, 6, 7, 2, 5, 4, 3, 0, 1};
```

---

## 📚 算法说明

### BFS（广度优先搜索）
- **特点**：保证找到最短路径
- **缺点**：时间和空间开销大
- **适用**：问题规模较小时

### A*（启发式搜索）
- **特点**：使用启发函数引导搜索
- **优点**：通常比BFS快很多
- **适用**：大多数情况的最佳选择

### IDA*（迭代加深A*）
- **特点**：结合迭代加深和A*
- **优点**：内存占用小
- **适用**：内存受限的场景

---

## 📊 预期性能对比

| 算法 | 节点扩展数 | 内存占用 | 时间复杂度 |
|------|-----------|---------|-----------|
| BFS  | 多        | 高      | O(b^d)    |
| A*   | 少        | 中      | O(b^d)    |
| IDA* | 中        | 低      | O(b^d)    |

*实际性能取决于问题难度和启发函数质量*

---

## 🚀 快速开始

1. **克隆项目**
   ```bash
   git clone <your-repo-url>
   cd 8-Puzzle-Solver-CPP
   ```

2. **编译运行**
   ```bash
   g++ -std=c++17 -o puzzle.exe main.cpp PuzzleState.cpp Heuristic.cpp Solver.cpp
   ./puzzle.exe
   ```

3. **实现算法**
   - 打开 `Heuristic.cpp` 实现启发函数
   - 打开 `Solver.cpp` 实现三个算法
   - 每个TODO都有详细的实现步骤说明

4. **测试验证**
   - 在 `main.cpp` 中取消注释测试代码
   - 重新编译运行
   - 查看性能对比结果

---

## 💡 提示

- 所有待实现的函数都有详细的TODO注释
- 每个TODO包含实现步骤和示例代码框架
- 建议按顺序实现：Heuristic → BFS → A* → IDA*
- 先用简单问题测试，再尝试困难问题

---

## 📄 许可证

MIT License

---

## 👤 作者

YanYun

---

## 🙏 致谢

感谢所有为本项目提供帮助和建议的人！
