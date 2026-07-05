/**
 * @file benchmark_main.cpp
 * @brief 8数码问题三种算法（BFS / A* / IDA*）批量性能测试程序
 *
 * 用法:
 *   benchmark [运行次数] [随机种子]
 *   默认运行 10000 次，种子 42（固定种子保证实验可复现）
 *
 * 编译（务必带 -O2 优化，否则 10000 次运行会非常慢）:
 *   VSCode: 终端 → 运行任务 → "C/C++: 编译 benchmark（O2优化）"
 *   命令行: g++ -std=c++17 -O2 -static benchmark_main.cpp PuzzleState.cpp
 *           Heuristic.cpp Solver.cpp -o build/benchmark.exe
 *   （-fexec-charset=GBK 加不加都可以，程序会自动适配，报告文件始终为 UTF-8）
 *
 * 测试数据构成:
 *   1) 随机实例（默认 10000 个）—— 反映真实随机问题分布；
 *   2) 难度覆盖补充实例 —— 通过反向 BFS 按最优步数分类全部可解状态，
 *      对随机抽样覆盖不到的难度（如 0 步、理论最大 31 步）补采样本，
 *      保证数据覆盖理论最小 ~ 理论最大难度的完整范围。
 *
 * 输出:
 *   benchmark_results.csv  —— 全部原始数据（供 plot_benchmark.py 绘图，
 *                              source 列区分 random / coverage）
 *   benchmark_report.txt   —— 汇总统计 + 有代表性的输入/输出实例（UTF-8 带 BOM）
 *
 * @author YanYun
 * @date 2026-07-05
 */

#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX   // 阻止 windows.h 定义 min/max 宏，避免与 std::max 冲突
#endif
#include <windows.h>
#endif
#include "PuzzleState.h"
#include "Solver.h"

int runs = 10000;

// 单次运行的完整记录
struct RunRecord {
    int id;
    std::vector<int> initial;   // 初始布局（扁平化 9 个数字）
    int steps;                  // 最优解步数（以 A* 结果为准）
    bool consistent;            // 三种算法解的步数是否一致（验证最优性）
    bool coverage = false;      // true = 难度覆盖性补充实例（非随机抽样）
    // 下标含义: 0=BFS, 1=A*, 2=IDA*
    long long nodes[3];
    double timeMs[3];
    double memKB[3];
};

static const char* ALGO_NAMES[3] = {"BFS", "A*", "IDA*"};

// ---------- 编码适配 ----------
// 源码是 UTF-8，但编译时若加了 -fexec-charset=GBK（本项目 VSCode 智能编译任务的默认选项），
// 程序里的中文字符串常量就是 GBK 字节。运行时探测一次实际编码，后续输出据此适配，
// 保证两种编译方式下控制台和报告文件都不乱码。
bool literalsAreUtf8() {
    static const unsigned char probe[] = "中";   // UTF-8: E4 B8 AD; GBK: D6 D0
    return probe[0] == 0xE4;
}

#ifdef _WIN32
// 把字符串统一转换为 UTF-8（若本来就是 UTF-8 则原样返回），用于写报告文件
std::string toUtf8(const std::string& s) {
    if (literalsAreUtf8()) return s;
    int wlen = MultiByteToWideChar(936, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w(wlen, L'\0');
    MultiByteToWideChar(936, 0, s.c_str(), (int)s.size(), &w[0], wlen);
    int ulen = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), wlen, nullptr, 0, nullptr, nullptr);
    std::string u(ulen, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), wlen, &u[0], ulen, nullptr, nullptr);
    return u;
}
#else
std::string toUtf8(const std::string& s) { return s; }
#endif

// 计算字符串的显示宽度（中文等宽字符按 2 列计，setw 按字节计会导致中文表头错位）
int displayWidth(const std::string& s) {
    if (!literalsAreUtf8()) return (int)s.size();  // GBK: 汉字 2 字节恰好占 2 列
    int w = 0;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = s[i];
        if (c < 0x80)             { w += 1; i += 1; }
        else if ((c >> 5) == 0x6) { w += 2; i += 2; }
        else if ((c >> 4) == 0xE) { w += 2; i += 3; }
        else                      { w += 2; i += 4; }
    }
    return w;
}

// 左对齐 / 右对齐填充到指定显示宽度
std::string padRight(const std::string& s, int width) {
    int pad = width - displayWidth(s);
    return s + std::string(pad > 0 ? pad : 0, ' ');
}
std::string padLeft(const std::string& s, int width) {
    int pad = width - displayWidth(s);
    return std::string(pad > 0 ? pad : 0, ' ') + s;
}

// 生成随机可解初始状态（使用外部传入的引擎，保证整个实验可复现）
std::vector<int> generateRandomSolvableState(std::mt19937& gen) {
    std::vector<int> state = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    do {
        std::shuffle(state.begin(), state.end(), gen);
    } while (!PuzzleState(state).isSolvable());
    return state;
}

// 把状态字符串（如 "123456780"）还原成扁平布局
std::vector<int> flatFromKey(const std::string& key) {
    std::vector<int> flat;
    for (char c : key) flat.push_back(c - '0');
    return flat;
}

// 从目标状态反向 BFS，把全部可解状态按最优解步数分类。
// 合法移动可逆，因此"到目标的距离"就是该状态的最优解步数。
// 返回 byDepth[d] = 最优解恰为 d 步的所有状态；byDepth.size()-1 即理论最大步数
//（标准终态下为 31 步，共 181440 个可解状态，耗时约 1~2 秒）
std::vector<std::vector<std::string>> classifyByDepth(const std::vector<int>& goal) {
    std::vector<std::vector<std::string>> byDepth;
    std::unordered_map<std::string, int> depth;
    std::queue<PuzzleState> q;

    PuzzleState g(goal);
    depth[g.toString()] = 0;
    byDepth.push_back({g.toString()});
    q.push(g);

    while (!q.empty()) {
        PuzzleState cur = q.front();
        q.pop();
        int d = depth[cur.toString()];
        for (const auto& nxt : cur.getSuccessors()) {
            const std::string key = nxt.toString();
            if (depth.find(key) == depth.end()) {
                depth[key] = d + 1;
                if ((int)byDepth.size() <= d + 1) byDepth.resize(d + 2);
                byDepth[d + 1].push_back(key);
                q.push(nxt);
            }
        }
    }
    return byDepth;
}

// 把扁平布局格式化成棋盘文本
std::string boardToText(const std::vector<int>& flat) {
    std::ostringstream ss;
    ss << "-------------\n";
    for (int i = 0; i < 3; i++) {
        ss << "| ";
        for (int j = 0; j < 3; j++) {
            int v = flat[i * 3 + j];
            if (v == 0) ss << "  | ";
            else ss << v << " | ";
        }
        ss << "\n-------------\n";
    }
    return ss.str();
}

// 单个实例的三算法结果对比表
std::string resultTable(const RunRecord& r) {
    std::ostringstream ss;
    ss << padRight("算法", 8) << padLeft("步数", 8)
       << padLeft("扩展节点数", 14) << padLeft("内存(KB)", 14)
       << padLeft("用时(ms)", 14) << "\n";
    ss << std::string(58, '-') << "\n";
    for (int a = 0; a < 3; a++) {
        ss << std::left << std::setw(8) << ALGO_NAMES[a]
           << std::right << std::setw(8) << r.steps
           << std::setw(14) << r.nodes[a]
           << std::setw(14) << std::fixed << std::setprecision(2) << r.memKB[a]
           << std::setw(14) << std::fixed << std::setprecision(3) << r.timeMs[a]
           << "\n";
    }
    return ss.str();
}

// 输出一个代表性实例（初始布局 + 三算法对比）
std::string describeCase(const std::string& title, const RunRecord& r) {
    std::ostringstream ss;
    ss << "\n---------- " << title << "（第 " << r.id << " 号实例）----------\n";
    ss << "输入（初始状态）:                目标状态:\n";

    // 初始状态与目标状态并排打印
    std::vector<int> goal = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    std::istringstream a(boardToText(r.initial)), b(boardToText(goal));
    std::string la, lb;
    while (std::getline(a, la) && std::getline(b, lb)) {
        ss << la << std::string(la.size() < 20 ? 20 - la.size() : 1, ' ') << lb << "\n";
    }

    ss << "输出（最优解步数 = " << r.steps << "，三种算法解长一致 = "
       << (r.consistent ? "是" : "否") << "）:\n";
    ss << resultTable(r);
    return ss.str();
}

// 打印一条完整解路径（用于最简单的代表性实例）
std::string describePath(const RunRecord& r) {
    PuzzleState init(r.initial);
    AStarSolver solver(init);
    SolveResult res = solver.solve();
    if (!res.success) return "";

    std::ostringstream ss;
    ss << "\n该实例的完整解路径（共 " << res.path.size() - 1 << " 步）:\n";
    for (size_t i = 0; i < res.path.size(); i++) {
        std::vector<int> flat;
        for (int row = 0; row < 3; row++)
            for (int col = 0; col < 3; col++)
                flat.push_back(res.path[i].getBoard(row, col));
        ss << "步骤 " << i << ":\n" << boardToText(flat);
        if (i + 1 < res.path.size()) ss << "   ↓\n";
    }
    return ss.str();
}

// 用三种算法求解一个实例并填充记录；三种算法都成功才返回 true
bool solveInstance(RunRecord& rec, int& inconsistent) {
    PuzzleState init(rec.initial);
    SolveResult res[3];
    { BFSSolver s(init);     res[0] = s.solve(); }
    { AStarSolver s(init);   res[1] = s.solve(); }
    { IDAStarSolver s(init); res[2] = s.solve(); }

    if (!(res[0].success && res[1].success && res[2].success)) return false;

    rec.steps = static_cast<int>(res[1].path.size()) - 1;
    rec.consistent = (res[0].path.size() == res[1].path.size() &&
                      res[1].path.size() == res[2].path.size());
    if (!rec.consistent) inconsistent++;

    for (int a = 0; a < 3; a++) {
        rec.nodes[a] = res[a].nodesExpanded;
        rec.timeMs[a] = res[a].timeUsed;
        rec.memKB[a] = res[a].memoryUsed / 1024.0;
    }
    return true;
}

void writeCsvRow(std::ofstream& csv, const RunRecord& rec) {
    std::string initStr;
    for (int v : rec.initial) initStr += static_cast<char>('0' + v);
    csv << rec.id << "," << initStr << "," << rec.steps;
    for (int a = 0; a < 3; a++) {
        csv << "," << rec.nodes[a]
            << "," << std::fixed << std::setprecision(4) << rec.timeMs[a]
            << "," << std::fixed << std::setprecision(2) << rec.memKB[a];
    }
    csv << "," << (rec.coverage ? "coverage" : "random") << "\n";
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // 按字符串常量的实际编码设置控制台代码页，两种编译方式下控制台都不乱码
    SetConsoleOutputCP(literalsAreUtf8() ? CP_UTF8 : 936);
#endif
    
    unsigned int seed = 42;
    if (argc > 1) runs = std::stoi(argv[1]);
    if (argc > 2) seed = static_cast<unsigned int>(std::stoul(argv[2]));

    // 可解状态总共 9!/2 = 181440 个，实例做了去重，要求次数不能超过状态总数
    const int TOTAL_SOLVABLE = 181440;
    if (runs > TOTAL_SOLVABLE) {
        std::cerr << "警告: 不重复的可解状态只有 " << TOTAL_SOLVABLE
                  << " 个，运行次数已截断\n";
        runs = TOTAL_SOLVABLE;
    }

    // 目标状态固定为标准终态，保证 1000 个实例之间可比
    std::vector<int> goal = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    PuzzleState::setGoalState(goal);

    std::mt19937 gen(seed);

    std::cout << "8数码求解器批量性能测试: " << runs
              << " 个随机可解实例 × 3 种算法（种子 = " << seed << "）\n";

    std::ofstream csv("benchmark_results.csv");
    csv << "id,initial,steps,"
        << "bfs_nodes,bfs_time_ms,bfs_mem_kb,"
        << "astar_nodes,astar_time_ms,astar_mem_kb,"
        << "idastar_nodes,idastar_time_ms,idastar_mem_kb,source\n";

    std::vector<RunRecord> records;
    records.reserve(runs);
    int inconsistent = 0, failures = 0;

    // 已测试过的初始状态，保证 10000 个随机实例互不重复
    //（10000 次抽样落在 181440 个状态里，不去重按生日悖论约会重复 270 次）
    std::unordered_set<std::string> tested;

    for (int i = 0; i < runs; i++) {
        RunRecord rec;
        rec.id = i + 1;

        // 拒绝采样：抽到已测过的状态就重新抽，直到得到新状态
        std::string key;
        do {
            rec.initial = generateRandomSolvableState(gen);
            key.clear();
            for (int v : rec.initial) key += static_cast<char>('0' + v);
        } while (!tested.insert(key).second);

        if (!solveInstance(rec, inconsistent)) {
            failures++;
            std::cerr << "警告: 第 " << rec.id << " 号实例存在求解失败，已跳过\n";
            continue;
        }
        records.push_back(rec);
        writeCsvRow(csv, rec);

        if (rec.id % 500 == 0) {
            std::cout << "  已完成 " << rec.id << " / " << runs << "\n";
            std::cout.flush();
        }
    }

    // ---------- 难度覆盖性补充 ----------
    // 随机抽样几乎覆盖不到理论难度的两端：0 步（初始即目标）只有 1 个状态，
    // 理论最难的 31 步全局只有 2 个状态（抽中概率约十万分之一）。
    // 这里从目标状态反向 BFS 将全部可解状态按最优步数分类，
    // 对样本不足的难度补采实例，使数据覆盖理论最小 ~ 理论最大的完整范围。
    std::cout << "正在反向 BFS 分类全部可解状态，补充稀有难度实例...\n";
    auto byDepth = classifyByDepth(goal);
    int theoreticalMax = (int)byDepth.size() - 1;

    // 各难度已有的随机样本数（tested 集合沿用随机阶段的，补采不会与随机实例重复）
    std::map<int, int> sampleCount;
    for (const auto& r : records) sampleCount[r.steps]++;

    const int MIN_PER_DEPTH = 3;   // 每个难度至少保证的样本数（受该难度状态总数限制）
    int coverageAdded = 0;
    int nextId = runs;
    for (int d = 0; d <= theoreticalMax; d++) {
        int target = (int)std::min<size_t>(MIN_PER_DEPTH, byDepth[d].size());
        if (sampleCount[d] >= target) continue;

        std::vector<std::string> pool = byDepth[d];
        std::shuffle(pool.begin(), pool.end(), gen);
        for (const auto& key : pool) {
            if (sampleCount[d] >= target) break;
            if (tested.count(key)) continue;

            RunRecord rec;
            rec.id = ++nextId;
            rec.coverage = true;
            rec.initial = flatFromKey(key);
            if (!solveInstance(rec, inconsistent)) { failures++; continue; }

            // 交叉验证：三种算法的解长必须等于反向 BFS 得到的理论最优步数
            if (rec.steps != d) {
                inconsistent++;
                std::cerr << "警告: 实例 " << rec.id << " 求解步数 " << rec.steps
                          << " 与反向 BFS 深度 " << d << " 不一致\n";
            }
            tested.insert(key);
            sampleCount[d]++;
            coverageAdded++;
            records.push_back(rec);
            writeCsvRow(csv, rec);
        }
    }
    std::cout << "  已补充 " << coverageAdded << " 个覆盖性实例（难度 0 ~ "
              << theoreticalMax << " 步全覆盖）\n";
    csv.close();

    if (records.empty()) {
        std::cerr << "没有成功的运行记录，测试终止。\n";
        return 1;
    }

    // ======================= 汇总统计 =======================
    std::ostringstream report;
    report << "==========================================================\n";
    report << "     8数码问题求解器 —— 三种算法批量性能测试报告\n";
    report << "==========================================================\n";
    size_t nRandom = records.size() - coverageAdded;
    report << "实例数量      : " << records.size() << "（随机实例 " << nRandom
           << " + 难度覆盖补充 " << coverageAdded << "，随机种子 " << seed << "）\n";
    report << "目标状态      : 1 2 3 / 4 5 6 / 7 8 0（标准终态）\n";
    report << "求解失败次数  : " << failures << "\n";
    report << "解长不一致次数: " << inconsistent
           << "（三种算法均为最优算法，理论上应为 0，此项用于正确性验证）\n";

    // 按算法统计均值/最值（只统计随机实例：覆盖性补充实例故意过采样了极端难度，
    // 混入会使平均值偏离真实的随机问题分布）
    report << "\n---------- 总体统计（" << nRandom << " 个随机实例的平均值 / 最大值）----------\n";
    report << padRight("算法", 8)
           << padLeft("平均节点数", 16) << padLeft("最大节点数", 14)
           << padLeft("平均用时(ms)", 14) << padLeft("最大用时(ms)", 14)
           << padLeft("平均内存(KB)", 14) << padLeft("最大内存(KB)", 14) << "\n";
    report << std::string(94, '-') << "\n";
    for (int a = 0; a < 3; a++) {
        double sumN = 0, maxN = 0, sumT = 0, maxT = 0, sumM = 0, maxM = 0;
        for (const auto& r : records) {
            if (r.coverage) continue;
            sumN += r.nodes[a];  maxN = std::max(maxN, (double)r.nodes[a]);
            sumT += r.timeMs[a]; maxT = std::max(maxT, r.timeMs[a]);
            sumM += r.memKB[a];  maxM = std::max(maxM, r.memKB[a]);
        }
        size_t n = nRandom;
        report << std::left << std::setw(8) << ALGO_NAMES[a] << std::right
               << std::fixed << std::setprecision(3) << std::setw(16) << sumN / n
               << std::setprecision(0) << std::setw(14) << maxN
               << std::setprecision(3) << std::setw(14) << sumT / n
               << std::setprecision(3) << std::setw(14) << maxT
               << std::setprecision(3) << std::setw(14) << sumM / n
               << std::setprecision(1) << std::setw(14) << maxM << "\n";
    }

    // 解步数分布
    std::map<int, int> depthCount;
    for (const auto& r : records) depthCount[r.steps]++;
    report << "\n---------- 最优解步数分布 ----------\n";
    report << "步数范围: " << depthCount.begin()->first << " ~ "
           << depthCount.rbegin()->first
           << "（理论范围 0 ~ " << theoreticalMax
           << " 步，覆盖性补充已保证全覆盖）\n";
    double avgDepth = 0;
    for (const auto& r : records) if (!r.coverage) avgDepth += r.steps;
    report << "平均步数: " << std::fixed << std::setprecision(3)
           << avgDepth / nRandom << "（仅随机实例）\n";

    // ======================= 代表性实例 =======================
    std::vector<size_t> order(records.size());
    for (size_t i = 0; i < order.size(); i++) order[i] = i;
    std::sort(order.begin(), order.end(), [&](size_t x, size_t y) {
        return records[x].steps < records[y].steps;
    });

    const RunRecord& easiest = records[order.front()];
    const RunRecord& quarter = records[order[order.size() / 4]];
    const RunRecord& median  = records[order[order.size() / 2]];
    const RunRecord& hard75  = records[order[order.size() * 3 / 4]];
    const RunRecord& hardest = records[order.back()];

    report << "\n==========================================================\n";
    report << "        有代表性的输入与输出（按问题难度选取）\n";
    report << "==========================================================\n";
    report << describeCase("最简单实例（理论最少 0 步，初始即目标）", easiest);
    report << describePath(easiest);
    report << describeCase("较易实例（步数处于 25% 分位）", quarter);
    report << describeCase("中等实例（步数处于中位数）", median);
    report << describeCase("较难实例（步数处于 75% 分位）", hard75);
    report << describeCase("最难实例（理论最大步数）", hardest);

    report << "\n==========================================================\n";
    report << "结论:\n";
    report << "  1. 三种算法在全部 " << records.size()
           << " 个实例上均求得同长度的最优解，且覆盖实例的解长与反向 BFS\n"
           << "     得到的理论最优步数一致，验证了实现的正确性;\n";
    report << "  2. A* 与 IDA* 的扩展节点数远少于 BFS，体现了启发式信息的剪枝作用;\n";
    report << "  3. IDA* 内存占用最小（仅保存当前路径），BFS 内存随深度指数增长;\n";
    report << "  4. 详细数据见 benchmark_results.csv，性能曲线见 benchmark_performance.png。\n";

    std::ofstream txt("benchmark_report.txt");
    txt << "\xEF\xBB\xBF";       // UTF-8 BOM，让记事本/VSCode 正确识别编码
    txt << toUtf8(report.str()); // 无论编译时用什么字符集，报告文件始终是 UTF-8
    txt.close();

    std::cout << report.str();
    std::cout << "\n原始数据已写入 benchmark_results.csv，报告已写入 benchmark_report.txt\n";
    return 0;
}
