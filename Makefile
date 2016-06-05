# Additional build options
-include config.mk

all: libjinc.a

libjinc.a: tokens.o
	$(AR) rc $@ $^

%.o: %.c jinc.h
	$(CC) -c $(CFLAGS) $< -o $@

test: jinc_test
	./jinc_test -f -v

jinc_test: jinc_test.o
	$(CC) -L. -ljinc $< -o $@

jinc_test.o: jinc_test.c libjinc.a

clean:
	rm -f *.o
	rm -f *.a
	rm -f jinc_test

.PHONY: all clean test
