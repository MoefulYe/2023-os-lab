C_SRCS = $(wildcard src/*.c)
C_EXECUTABLES = $(patsubst src/%.c,%,$(C_SRCS))

# 默认目标，生成所有可执行文件
all: $(C_EXECUTABLES) calc

calc: src/calc.cpp
	clang++ -o out/calc $<

$(C_EXECUTABLES): %: src/%.c
	gcc -o out/$@ $<

clean:
	rm -f out/$(C_EXECUTABLES)
	rm -f out/calc

