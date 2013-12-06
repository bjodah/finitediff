SRCDIR=../src
INCDIR=../include
CFLAGS=-std=c99
.PHONY: tests

tests: test_newton_interval 
	@./test_newton_interval

test_newton_interval: $(INCDIR)/newton_interval.h $(SRCDIR)/newton_interval.c test_newton_interval.c unittest.c unittest.h
	$(CC) $(CFLAGS) -g -I$(INCDIR) -o $@ $(SRCDIR)/newton_interval.c unittest.c test_newton_interval.c -lm
