# UI 相关修改说明

> 日期：2026-07-05
> 说明：本文档记录对 PyQt5 图形界面（`main.py`）及相关代码的全部修改，供撰写项目文档参考。
> 注意：`mainwindow.py` 是 Qt Designer 自动生成的文件，**没有做任何修改**，所有适配都在 `main.py` 中完成（这样重新用 pyuic5 生成也不会丢失改动）。

---

## 一、求解器调用路径修正

**位置**：`main.py` 顶部 `SOLVER_PATH`

**修改前**：指向项目根目录的 `ui_main.exe`（文件不存在）。

**修改后**：指向 `build/ui_main.exe`，与 VSCode 编译任务的输出路径一致。

```python
SOLVER_PATH = os.path.join(os.path.dirname(__file__), "build", "ui_main.exe")
```

---

## 二、异步求解（解决界面卡死）

**问题**：原来 `start_solve` 直接同步调用 `solve_puzzle`，求解期间整个界面冻结无响应，复杂问题可能卡几十秒。

**方案**：新增 `SolverThread`（继承 `QThread`），把 C++ 求解器的调用放到后台线程，完成后通过 `pyqtSignal` 回调 `on_solve_finished` 更新界面。

- `start_solve`：设置状态提示、禁用按钮、启动线程后立即返回；
- `on_solve_finished(result)`：接收求解结果，更新统计信息并播放动画；
- 若已有求解线程在运行，重复点击"开始求解"会被忽略，防止并发调用。

---

## 三、中文编码修正（解决"无解"时程序异常）

**问题**：C++ 端用 `-fexec-charset=GBK` 编译，输出的中文错误信息（如"无解"）是 GBK 编码；Python 的 `subprocess.run` 默认按 UTF-8 解码，遇到 GBK 字节会抛出解码异常，表现为程序"崩溃"。

**方案**：`solve_puzzle` 中显式指定编码：

```python
result = subprocess.run(cmd, capture_output=True, text=True,
                        encoding="gbk", errors="replace", timeout=30)
```

---

## 四、无解情况的友好提示

**问题**：无解时只弹一个警告框，信息不明确。

**方案**：`on_solve_finished` 中区分两种失败：

- 错误信息含"无解" → 状态栏显示 `🚫 此问题无解`，弹出普通信息框（`QMessageBox.information`）说明原因；
- 其它错误 → 状态栏显示 `❌ 求解失败`，弹出警告框显示具体错误。

> 说明：界面默认的初始状态 `[2,8,3,1,6,4,7,0,5]` 相对标准目标确实无解（逆序数奇偶性不同），出现"无解"提示是正确行为，不是 bug。

---

## 五、界面整体等比缩放（一屏显示全部内容）

**问题**：UI 设计画布为 1385×1669，超过普通屏幕高度，内容显示不全。直接改窗口大小会裁掉内容；用滚动条或 `QGraphicsView` 代理嵌入的方案都被否决（后者在 Windows 上对复杂控件渲染不可靠，会出现白屏）。

**方案**：启动时按屏幕尺寸计算缩放比例，**逐个缩放所有控件**（`_scale_ui` 方法）：

```python
self.ui_scale = min(screen.width() * 0.95 / DESIGN_WIDTH,
                    screen.height() * 0.85 / DESIGN_HEIGHT, 1.0)
```

对 `centralwidget` 的每个子控件做四件事：

1. **几何位置**：`setGeometry` 的 x、y、宽、高全部乘以比例（对绝对定位控件生效）；
2. **尺寸约束**：`minimumSize` / `maximumSize` 同比例缩放——这一步很关键，棋盘九宫格放在 `QGridLayout` 里且用 min/max 锁定 120×120，不缩放约束的话布局会把格子挤压得大小不一；
3. **字体（点值）**：`font.pointSize() > 0` 的按比例缩小（下限 6pt）；
4. **字体（样式表）**：用正则把样式表中的 `font-size: NNpx` 替换为缩放后的值。

另外：

- `update_board` 每次刷新都会重设格子样式表，其中的 `font-size` 也改为 `24 * self.ui_scale`，避免走棋后字号弹回；
- 启动时 `window.adjustSize()` 让窗口自动适配缩放后的内容，并用 `frameGeometry().moveCenter(...)` 使窗口在屏幕居中。

**已知无害警告**：终端中的 `QFont::setPointSize: Point size <= 0 (-1)` 来自 `mainwindow.py` 生成代码本身（对用样式表设字体的控件调用了 `setPointSize(-1)`），与本次修改无关，可忽略。

---

## 六、"生成新问题"同时随机化目标状态

**问题**：原来只随机初始状态，目标状态固定为 `[1..8,0]`。

**方案**：`generate_puzzle` 先随机打乱目标状态并刷新目标棋盘，再从新目标做 20–50 次随机合法移动得到初始状态。由于初始状态是从目标"走"出来的，**生成的问题必然有解**。

---

## 七、C++ 端 bug 修复：`isSolvable()`（影响 UI 行为，需一并记录）

**位置**：`PuzzleState.cpp`

**问题**：原实现固定返回"逆序数为偶数即有解"，这只对标准目标 `[1,2,...,8,0]` 成立。目标状态随机化后，大量实际有解的问题被误判为"无解"。

**修复**：正确的判定是**初始状态与目标状态的逆序数奇偶性相同**（3×3 滑块谜题的合法移动不改变逆序数奇偶性）：

```cpp
bool PuzzleState::isSolvable() const {
    return countInversions(board) % 2 == countInversions(GOAL) % 2;
}
```

修复后已验证：随机生成的问题正常求解、真正无解的组合仍正确报告无解。`build/ui_main.exe` 与 `build/test_main.exe` 均已用修复后的代码重新编译。

---

## 八、构建方式说明

项目现在有两个入口，编译时**不能一起编译**（都含 `main` 函数）：

| 入口文件 | 用途 | 输出 |
|---|---|---|
| `ui_main.cpp` | UI 后端：命令行传参，输出 JSON | `build/ui_main.exe` |
| `test_main.cpp` | 算法性能测试（原 `main.cpp` 重命名） | `build/test_main.exe` |

`.vscode/tasks.json` 配置为"智能编译"：按 `Ctrl+Shift+B` 时根据**当前打开的 cpp 文件**决定编译哪个入口（输出文件名与入口文件同名），公共源文件 `PuzzleState.cpp`、`Heuristic.cpp`、`Solver.cpp` 始终参与编译。

## 九、运行环境

- Python 3.13 虚拟环境（`.venv`），Python 3.14 暂不支持 PyQt5，勿升级；
- 依赖：`PyQt5`（在 `.venv` 中安装）；
- 启动方式：`.\.venv\Scripts\python.exe main.py`。
