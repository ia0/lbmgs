GCC = gcc -Wall -Wextra -Werror
CC = $(GCC) -Isrc
LD = $(GCC)

DEBUG = yes
ifdef DEBUG
GCC += -g
CC += -DDEBUG
else
CC += -DNDEBUG
endif

SRCS += src/main.c
SRCS += src/common.c
SRCS += src/parser.c
SRCS += src/client.c
SRCS += src/game.c
SRCS += src/game/chat.c

OBJS = $(patsubst src/%.c,build/%.o,$(SRCS))
DEPS = $(patsubst src/%.c,build/%.d,$(SRCS))

EXE = build/lbmgs

.PHONY: all
all: $(EXE)

$(EXE): $(OBJS)
	@echo "  LD $@"
	@$(LD) $(OBJS) -o $@

-include $(DEPS)

build/%.o build/%.d:
	@mkdir -p $(dir $@)
	@echo "  CC build/$*.o"
	@$(CC) -MMD -c src/$*.c -o build/$*.o

.PHONY: clean
clean:
	rm -rf build
