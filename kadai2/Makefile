PROG := pp
SRCS := token-list.c scan.c id-list.c LL.c
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
	rm -f $(PROG) $(OBJS) $(DEPS)
	rm -f *.out
	rm -rf results/
	rm -rf html/ latex/

test: $(PROG)
	chmod +x test.sh
	./test.sh

.PHONY: test clean

