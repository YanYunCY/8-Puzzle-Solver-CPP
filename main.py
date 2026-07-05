import sys
import subprocess
import json
import os
import random
import re
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QMessageBox, QWidget, QDesktopWidget,
)
from PyQt5.QtCore import QTimer, QThread, pyqtSignal
from mainwindow import Ui_MainWindow

# UI 设计时的画布尺寸（mainwindow.py 中 resize 的值）
DESIGN_WIDTH = 1385
DESIGN_HEIGHT = 1669

# C++ 求解器路径
SOLVER_PATH = os.path.join(os.path.dirname(__file__), "build", "ui_main.exe")


def solve_puzzle(initial_state, goal_state, algorithm="astar"):
    """调用 C++ 求解器"""
    print("🔍 调用求解器...")
    print("初始状态:", initial_state)
    print("目标状态:", goal_state)
    print("算法:", algorithm)
    
    initial_str = ",".join(map(str, initial_state))
    goal_str = ",".join(map(str, goal_state))
    
    cmd = [SOLVER_PATH, initial_str, goal_str, algorithm]
    print("命令:", " ".join(cmd))
    
    try:
        # C++ 端用 GBK 输出中文，这里用 gbk 解码避免乱码/解码崩溃
        result = subprocess.run(
            cmd, capture_output=True, text=True,
            encoding="gbk", errors="replace", timeout=30,
        )
        print("返回码:", result.returncode)
        print("输出:", result.stdout)
        if result.stderr:
            print("错误:", result.stderr)
        
        if result.returncode != 0:
            return {"success": False, "error": result.stderr}
        
        data = json.loads(result.stdout)
        
        # 估算内存使用（KB）
        if data.get("success") and "path" in data:
            path_len = len(data["path"])
            nodes = data.get("nodesExpanded", 1)
            estimated_bytes = path_len * 200 + nodes * 50
            data["memoryUsed"] = round(estimated_bytes / 1024, 2)
        
        # 时间从毫秒转换为微秒（乘以 1000）
        if data.get("success") and "timeUsed" in data:
            time_ms = data["timeUsed"]
            if time_ms == 0:
                # 极短时间，显示为 < 1 μs
                data["timeUsed"] = "< 1"
            else:
                data["timeUsed"] = round(time_ms * 1000, 2)
        
        return data
    except json.JSONDecodeError as e:
        print("JSON解析失败:", e)
        return {"success": False, "error": f"解析失败: {result.stdout}"}
    except Exception as e:
        print("异常:", e)
        return {"success": False, "error": str(e)}


class SolverThread(QThread):
    """异步求解线程，避免UI冻结"""
    finished = pyqtSignal(dict)

    def __init__(self, initial_state, goal_state, algorithm):
        super().__init__()
        self.initial_state = initial_state
        self.goal_state = goal_state
        self.algorithm = algorithm

    def run(self):
        result = solve_puzzle(self.initial_state, self.goal_state, self.algorithm)
        self.finished.emit(result)


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)

        # 界面是绝对定位布局，按屏幕大小等比缩放每个控件的位置和字体，
        # 使全部内容一屏显示（无滚动条）
        screen = QDesktopWidget().availableGeometry()
        self.ui_scale = min(
            screen.width() * 0.95 / DESIGN_WIDTH,
            screen.height() * 0.85 / DESIGN_HEIGHT,
            1.0,
        )
        self._scale_ui(self.ui_scale)

        self.goal_state = [1, 2, 3, 4, 5, 6, 7, 8, 0]
        self.initial_state = [2, 8, 3, 1, 6, 4, 7, 0, 5]
        self.path = []
        self.step_index = 0
        self.timer = QTimer()
        self.timer.timeout.connect(self.next_step)
        self.solver_thread = None  # 求解线程
        
        # 绑定按钮
        self.btn_generate.clicked.connect(self.generate_puzzle)
        self.btn_solve.clicked.connect(self.start_solve)
        self.btn_reset.clicked.connect(self.reset_puzzle)
        self.btn_pause.clicked.connect(self.pause_animation)
        
        # 显示初始棋盘
        self.update_board("init", self.initial_state)
        self.update_board("goal", self.goal_state)
        self.update_board("cur", self.initial_state)
        self.update_stats(0, 0, 0, 0)
        self.label_status.setText("💡 点击「生成新问题」开始")
        self.label_step_info.setText("步骤: 0 / 0")
        self.progress_bar.setValue(0)

    def _scale_ui(self, k):
        """按比例 k 缩放所有控件的几何位置与字体大小（绝对定位布局适用）"""
        if k >= 0.999:
            return
        MAX_QT = 16777215  # Qt 尺寸约束的默认最大值
        for w in self.centralwidget.findChildren(QWidget):
            g = w.geometry()
            w.setGeometry(
                round(g.x() * k), round(g.y() * k),
                round(g.width() * k), round(g.height() * k),
            )
            # 布局内的控件由 min/max 约束决定大小（如棋盘格子锁定 120x120），
            # 必须同步缩放，否则布局里的控件会被挤压得大小不一
            mn, mx = w.minimumSize(), w.maximumSize()
            if mn.width() > 0 or mn.height() > 0:
                w.setMinimumSize(round(mn.width() * k), round(mn.height() * k))
            if mx.width() < MAX_QT or mx.height() < MAX_QT:
                w.setMaximumSize(
                    round(mx.width() * k) if mx.width() < MAX_QT else MAX_QT,
                    round(mx.height() * k) if mx.height() < MAX_QT else MAX_QT,
                )
            f = w.font()
            if f.pointSize() > 0:
                f.setPointSize(max(6, round(f.pointSize() * k)))
                w.setFont(f)
            ss = w.styleSheet()
            if "font-size" in ss:
                ss = re.sub(
                    r"font-size:\s*(\d+)px",
                    lambda m: f"font-size: {max(6, round(int(m.group(1)) * k))}px",
                    ss,
                )
                w.setStyleSheet(ss)
        self.centralwidget.setFixedSize(
            round(DESIGN_WIDTH * k), round(DESIGN_HEIGHT * k)
        )

    def update_board(self, prefix, state):
        """更新棋盘"""
        fs = max(8, round(24 * self.ui_scale))
        for i in range(3):
            for j in range(3):
                idx = i * 3 + j
                val = state[idx]
                label = getattr(self, f"label_{prefix}_{i}{j}")
                label.setText(str(val) if val != 0 else "")
                if val == 0:
                    label.setStyleSheet(f"background-color: #e0e0e0; font-size: {fs}px; border: 2px solid #999; border-radius: 8px;")
                else:
                    label.setStyleSheet(f"background-color: white; font-size: {fs}px; border: 2px solid #999; border-radius: 8px;")
    
    def update_stats(self, steps, nodes, time_us, memory=0):
        self.label_steps.setText(f"步数: {steps}")
        self.label_expanded.setText(f"扩展节点: {nodes}")
        self.label_time.setText(f"耗时: {time_us} μs")
        self.label_memory.setText(f"内存: {memory} KB")
    
    def generate_puzzle(self):
        """生成随机新问题：目标状态随机打乱，初始状态由目标经随机合法移动得到（保证有解）"""
        goal = list(range(1, 9)) + [0]
        random.shuffle(goal)
        self.goal_state = goal[:]
        self.update_board("goal", goal)

        state = goal[:]

        # 做 20-50 次随机移动
        moves = random.randint(20, 50)
        for _ in range(moves):
            blank = state.index(0)
            row, col = blank // 3, blank % 3
            
            directions = []
            if row > 0: directions.append(-3)
            if row < 2: directions.append(3)
            if col > 0: directions.append(-1)
            if col < 2: directions.append(1)
            
            if directions:
                step = random.choice(directions)
                new_blank = blank + step
                state[blank], state[new_blank] = state[new_blank], state[blank]
        
        self.initial_state = state[:]
        self.update_board("init", state)
        self.update_board("cur", state)
        self.update_stats(0, 0, 0, 0)
        self.path = []
        self.step_index = 0
        self.timer.stop()
        self.btn_pause.setText("⏸ 暂停")
        self.label_status.setText("💡 已生成新问题，点击「开始求解」")
        self.label_step_info.setText("步骤: 0 / 0")
        self.progress_bar.setValue(0)
    
    def reset_puzzle(self):
        self.timer.stop()
        self.update_board("cur", self.initial_state)
        self.update_stats(0, 0, 0, 0)
        self.path = []
        self.step_index = 0
        self.btn_pause.setText("⏸ 暂停")
        self.label_status.setText("💡 已重置")
        self.label_step_info.setText("步骤: 0 / 0")
        self.progress_bar.setValue(0)
    
    def pause_animation(self):
        """暂停/继续动画"""
        if self.timer.isActive():
            self.timer.stop()
            self.btn_pause.setText("▶ 继续")
            self.label_status.setText("⏸ 已暂停")
        else:
            if self.step_index >= len(self.path) and len(self.path) > 0:
                self.label_status.setText("✅ 动画已播放完毕")
                return
            self.timer.start(200)
            self.btn_pause.setText("⏸ 暂停")
            self.label_status.setText("▶ 继续播放...")
    
    def start_solve(self):
        # 如果已有线程在运行，先停止
        if self.solver_thread and self.solver_thread.isRunning():
            return

        # 获取当前选择的算法
        algorithm_text = self.combo_algorithm.currentText().lower()
        if algorithm_text == "bfs":
            algo = "bfs"
        elif algorithm_text == "ida*":
            algo = "ida"
        else:
            algo = "astar"

        self.label_status.setText(f"⏳ 正在使用 {algorithm_text} 求解...")
        self.btn_solve.setEnabled(False)
        self.btn_pause.setText("⏸ 暂停")

        # 创建异步线程
        self.solver_thread = SolverThread(self.initial_state, self.goal_state, algo)
        self.solver_thread.finished.connect(self.on_solve_finished)
        self.solver_thread.start()

    def on_solve_finished(self, result):
        """求解完成回调"""
        if not result.get("success", False):
            self.btn_solve.setEnabled(True)
            error = str(result.get("error", "未知错误")).strip()
            # 区分「无解」和其它错误，界面友好提示，不弹出吓人的警告
            if "无解" in error:
                self.label_status.setText("🚫 此问题无解")
                QMessageBox.information(self, "无解", "当前初始状态无法到达目标状态（无解）。")
            else:
                self.label_status.setText("❌ 求解失败")
                QMessageBox.warning(self, "求解失败", error or "未知错误")
            return

        steps = result.get("steps", 0)
        nodes = result.get("nodesExpanded", 0)
        time_us = result.get("timeUsed", 0)
        memory = result.get("memoryUsed", 0)

        self.update_stats(steps, nodes, time_us, memory)
        self.path = result.get("path", [])

        if not self.path:
            self.label_status.setText("❌ 未找到路径")
            self.btn_solve.setEnabled(True)
            return

        # 重置动画状态
        self.step_index = 0
        self.progress_bar.setValue(0)
        self.label_step_info.setText(f"步骤: 0 / {steps}")
        self.timer.start(200)
        self.label_status.setText(f"✅ 找到解！共 {steps} 步，正在播放动画...")
        self.btn_solve.setEnabled(True)
    
    def next_step(self):
        if self.step_index >= len(self.path):
            self.timer.stop()
            self.btn_pause.setText("⏸ 暂停")
            self.label_status.setText(f"✅ 动画播放完毕！共 {len(self.path)-1} 步")
            return
        
        # 更新棋盘
        self.update_board("cur", self.path[self.step_index])
        
        # 更新步骤信息
        total_steps = len(self.path) - 1
        self.label_step_info.setText(f"步骤: {self.step_index} / {total_steps}")
        
        # 更新进度条
        if total_steps > 0:
            progress = int(self.step_index / total_steps * 100)
            self.progress_bar.setValue(progress)
        
        self.step_index += 1


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.setWindowTitle("8数码问题求解器")
    window.adjustSize()  # 窗口自动适配缩放后的内容大小

    # 窗口在屏幕上居中显示
    frame = window.frameGeometry()
    frame.moveCenter(QDesktopWidget().availableGeometry().center())
    window.move(frame.topLeft())

    window.show()
    sys.exit(app.exec_())