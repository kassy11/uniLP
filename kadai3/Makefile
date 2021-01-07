PROG := cr
SRCS := token-list.c scan.c id-list.c LL.c CR.c
OBJS := $(SRCS:%.c=%.o)
DEPS := $(SRCS:%.c=%.d)

CC := gcc

all: $(PROG)

-include $(DEPS)

$(PROG): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -O0 -c -MMD -MP $<

clean:
	rm -f $(PROG) $(OBJS) $(DEPS)
	rm -rf results/ html/ latex/

test: $(PROG)
	chmod +x test.sh
	./test.sh

.PHONY: test clean
