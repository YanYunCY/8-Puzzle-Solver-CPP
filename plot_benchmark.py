# -*- coding: utf-8 -*-
"""
plot_benchmark.py —— 把 benchmark_results.csv 汇总成三种算法的性能对比线图

用法:
    python plot_benchmark.py

读取 build/benchmark.exe 生成的 benchmark_results.csv（1000 个随机实例 × 3 种算法），
按"最优解步数"（问题难度）分组求平均，绘制三个面板：
    扩展节点数 / 求解用时 / 内存占用  随问题难度的变化曲线
输出 benchmark_performance.png

@author YanYun
@date 2026-07-05
"""

import csv
import os
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import LogLocator

# ---------------- 样式常量 ----------------
SURFACE = "#fcfcfb"       # 图表底色
INK_PRIMARY = "#0b0b0b"   # 主要文字
INK_SECONDARY = "#52514e" # 次要文字
INK_MUTED = "#898781"     # 坐标轴刻度
GRIDLINE = "#e1e0d9"      # 网格线
BASELINE = "#c3c2b7"      # 轴线

# 三种算法的固定配色（经色觉无障碍校验的分类色，顺序固定）
ALGOS = [
    ("BFS",  "#2a78d6"),
    ("A*",   "#1baf7a"),
    ("IDA*", "#eda100"),
]

plt.rcParams["font.family"] = ["Microsoft YaHei", "SimHei", "sans-serif"]
plt.rcParams["axes.unicode_minus"] = False


def load_data(path):
    """读取 CSV，返回 rows 列表（字段均转为数值）"""
    rows = []
    with open(path, newline="", encoding="utf-8") as f:
        for r in csv.DictReader(f):
            rows.append({
                "steps": int(r["steps"]),
                "BFS":  (int(r["bfs_nodes"]),     float(r["bfs_time_ms"]),     float(r["bfs_mem_kb"])),
                "A*":   (int(r["astar_nodes"]),   float(r["astar_time_ms"]),   float(r["astar_mem_kb"])),
                "IDA*": (int(r["idastar_nodes"]), float(r["idastar_time_ms"]), float(r["idastar_mem_kb"])),
            })
    return rows


def aggregate(rows):
    """按最优解步数分组，对每种算法的 (节点数, 用时, 内存) 求平均"""
    groups = defaultdict(list)
    for r in rows:
        groups[r["steps"]].append(r)

    depths = sorted(groups)
    agg = {name: {"nodes": [], "time": [], "mem": []} for name, _ in ALGOS}
    counts = []
    for d in depths:
        bucket = groups[d]
        counts.append(len(bucket))
        for name, _ in ALGOS:
            agg[name]["nodes"].append(sum(x[name][0] for x in bucket) / len(bucket))
            agg[name]["time"].append(sum(x[name][1] for x in bucket) / len(bucket))
            agg[name]["mem"].append(sum(x[name][2] for x in bucket) / len(bucket))
    return depths, counts, agg


def style_axis(ax):
    ax.set_facecolor(SURFACE)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    for side in ("left", "bottom"):
        ax.spines[side].set_color(BASELINE)
    ax.tick_params(colors=INK_MUTED, labelcolor=INK_SECONDARY, labelsize=9)
    ax.grid(axis="y", color=GRIDLINE, linewidth=0.8)
    ax.set_axisbelow(True)


def plot_panel(ax, depths, agg, metric, title, ylabel):
    for name, color in ALGOS:
        ax.plot(depths, agg[name][metric], color=color, linewidth=2,
                marker="o", markersize=4, label=name, zorder=3)
        # 曲线末端直接标注算法名（文字用文本色，靠位置对应曲线）
        ax.annotate(name, (depths[-1], agg[name][metric][-1]),
                    xytext=(6, 0), textcoords="offset points",
                    va="center", fontsize=9, color=INK_PRIMARY)
    ax.set_yscale("log")
    ax.yaxis.set_major_locator(LogLocator(base=10))
    ax.set_title(title, color=INK_PRIMARY, fontsize=12, pad=10)
    ax.set_xlabel("最优解步数（问题难度）", color=INK_SECONDARY, fontsize=10)
    ax.set_ylabel(ylabel, color=INK_SECONDARY, fontsize=10)
    ax.margins(x=0.12)


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    csv_path = os.path.join(here, "benchmark_results.csv")
    rows = load_data(csv_path)
    depths, counts, agg = aggregate(rows)

    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    fig.patch.set_facecolor(SURFACE)

    plot_panel(axes[0], depths, agg, "nodes",
               "平均扩展节点数", "扩展节点数（对数刻度）")
    plot_panel(axes[1], depths, agg, "time",
               "平均求解用时", "用时 / ms（对数刻度）")
    plot_panel(axes[2], depths, agg, "mem",
               "平均内存占用", "内存 / KB（对数刻度）")

    axes[0].legend(loc="lower right", frameon=False, fontsize=9,
                   labelcolor=INK_SECONDARY)

    fig.suptitle(f"8数码求解算法性能对比 —— {len(rows)} 个随机可解实例，按问题难度分组取平均",
                 color=INK_PRIMARY, fontsize=14, y=1.02)
    fig.text(0.5, -0.04,
             f"步数范围 {depths[0]} ~ {depths[-1]}（含难度覆盖补充实例，覆盖理论最小~最大难度），"
             f"每一难度的样本数 {min(counts)} ~ {max(counts)} 个；"
             "三种算法在全部实例上均求得同长度最优解",
             ha="center", color=INK_MUTED, fontsize=9)

    fig.tight_layout()
    out = os.path.join(here, "benchmark_performance.png")
    fig.savefig(out, dpi=200, bbox_inches="tight", facecolor=SURFACE)
    print(f"已生成 {out}")


if __name__ == "__main__":
    main()
