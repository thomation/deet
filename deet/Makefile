# 样本程序
SAMPLE_SRCS = $(wildcard samples/*.c)
SAMPLE_PROGS = $(patsubst %.c,%,$(SAMPLE_SRCS))

# 调试器程序
DEBUGGER_SRCS = $(wildcard src/*.c)
DEBUGGER_OBJS = $(DEBUGGER_SRCS:.c=.o)
DEBUGGER = deet

# 包含路径
INCLUDES = -Iinclude

# 编译器标志
CFLAGS = -O0 -g -no-pie -fno-omit-frame-pointer -Wall -Wextra $(INCLUDES)
CC = gcc

# 默认目标
all: $(DEBUGGER) $(SAMPLE_PROGS)

# 链接调试器
$(DEBUGGER): $(DEBUGGER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 编译调试器对象文件
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 编译样本程序
samples/%: samples/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(DEBUGGER) $(DEBUGGER_OBJS) $(SAMPLE_PROGS)

test: $(DEBUGGER) $(SAMPLE_PROGS)
	./$(DEBUGGER) samples/sleepy_print

info:
	@echo "调试器源文件: $(DEBUGGER_SRCS)"
	@echo "调试器对象文件: $(DEBUGGER_OBJS)"
	@echo "样本程序: $(SAMPLE_PROGS)"

help:
	@echo "可用目标:"
	@echo "  all     - 编译调试器和所有样本程序"
	@echo "  clean   - 清理所有生成的文件"
	@echo "  test    - 运行调试器测试"
	@echo "  info    - 显示编译信息"
	@echo "  help    - 显示此帮助信息"

.PHONY: all clean test info help
