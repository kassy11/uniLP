PROG := tc
TPROG := test
RDIR := results
SRCS := token-list.c scan.c id-list.c blackbox.c
OBJS := $(SRCS:%.c=%.o)
DEPS := $(SRCS:%.c=%.d)

CC := gcc

all: $(PROG)

-include $(DEPS)

$(PROG): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c -MMD -MP $<

clean:
	rm -rf $(PROG) $(OBJS) $(DEPS) $(RDIR)

test: $(PROG)
	./test.sh

.PHONY: test clean


