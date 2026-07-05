# 8数码求解器 Makefile
# 所有编译产物（.o / .exe）统一输出到 build/ 目录
#
# 常用命令:
#   make            编译算法测试程序和批量性能测试程序
#   make test       编译 build/test_main.exe（单实例三算法对比）
#   make benchmark  编译 build/benchmark.exe（批量性能测试，用法: build/benchmark.exe [次数] [种子]）
#   make ui         编译 build/ui_main.exe（供 main.py 图形界面调用）
#   make clean      清理 build/ 下的编译产物

CXX = g++
# -static: 静态链接 C++ 运行库，避免运行时加载到系统里其他版本的 libstdc++ DLL
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -static

BUILDDIR = build
CORE_SRCS = PuzzleState.cpp Heuristic.cpp Solver.cpp
CORE_OBJS = $(CORE_SRCS:%.cpp=$(BUILDDIR)/%.o)

# 编码约定：
#   ui_main / test_main 输出 GBK（main.py 按 GBK 解码、适配默认中文控制台）
#   benchmark 输出 UTF-8（报告文件带 BOM，程序运行时自行把控制台切到 UTF-8）
$(BUILDDIR)/ui_main.o $(BUILDDIR)/test_main.o: CHARSET = -fexec-charset=GBK

all: test benchmark

test: $(BUILDDIR)/test_main.exe
benchmark: $(BUILDDIR)/benchmark.exe
ui: $(BUILDDIR)/ui_main.exe

$(BUILDDIR)/test_main.exe: $(BUILDDIR)/test_main.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILDDIR)/benchmark.exe: $(BUILDDIR)/benchmark_main.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILDDIR)/ui_main.exe: $(BUILDDIR)/ui_main.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILDDIR)/%.o: %.cpp PuzzleState.h Heuristic.h Solver.h | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(CHARSET) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -f $(BUILDDIR)/*.o $(BUILDDIR)/*.exe

.PHONY: all test benchmark ui clean
