.SUFFIXES:

ifeq ($(V),1)
DO =
PP = @:
else
DO = @
PP = @echo
endif

GCC = gcc -Wall -Wextra -Werror
CC = $(GCC) -Isrc
LD = $(GCC)

ifeq ($(R),1)
CC += -DNDEBUG
else
GCC += -g
CC += -DDEBUG
endif

SRCS += src/main.c
SRCS += src/common.c
SRCS += src/parser.c
SRCS += src/client.c
SRCS += src/game.c
SRCS += src/game/chat.c
SRCS += src/game/chess.c

OBJS = $(SRCS:src/%.c=build/%.o)
DEPS = $(SRCS:src/%.c=build/%.d)

build/lbmgs: $(OBJS)
	$(PP) "  LD $@"
	$(DO)$(LD) $(OBJS) -o $@

-include $(DEPS)

build/%.o:
	$(DO)mkdir -p $(dir $@)
	$(PP) "  CC $@"
	$(DO)$(CC) -MMD -c src/$*.c -o $@

.PHONY: clean
clean:
	$(PP) "  RM build"
	$(DO)rm -rf build
