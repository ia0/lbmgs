# line-based multiplayer game server
# Copyright (C) 2014 github at ia0 dot eu
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
