GCC = gcc -Wall -Wextra -Werror -g
CC = $(GCC) -Isrc
LD = $(GCC)

SRCS += src/chess.c

OBJS = $(patsubst src/%.c,build/%.o,$(SRCS))
DEPS = $(patsubst src/%.c,build/%.d,$(SRCS))

EXE = build/chess

.PHONY: all
all: $(EXE)

$(EXE): $(OBJS) | build
	@echo "  LD $@"
	@$(LD) $(OBJS) -o $@

-include $(DEPS)

build/%.o build/%.d: | build
	@echo "  CC build/$*.o"
	@$(CC) -MMD -c src/$*.c -o build/$*.o

build:
	mkdir build

.PHONY: clean
clean:
	rm -rf build
