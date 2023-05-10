# Target library
lib := libuthread.a
objs := queue.o context.o uthread.o sem.o preempt.o
CC := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS += -g

ifneq ($(V), 1)
Q = @
endif

all: $(lib)

deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

$(lib): $(objs)
	@echo "CC $@"
	$(Q)ar rcs $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(deps)
