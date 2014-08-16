GCC = gcc -Wall -Wextra -Werror
CC = $(GCC) -Isrc
LD = $(GCC)

ifdef DEBUG
GCC += -g
CC += -DDEBUG
else
CC += -DNDEBUG
endif

SRCS += src/chess.c
SRCS += src/common.c
SRCS += src/server.c
SRCS += src/client.c

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
