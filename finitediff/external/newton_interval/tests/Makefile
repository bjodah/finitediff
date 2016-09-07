SRCDIR=../src
INCDIR=../include
LIBS=-lm
CFLAGS=-Wall -Werror -g
NEWTON_SRCS=$(INCDIR)/newton_interval.h $(SRCDIR)/newton_interval.c
UNIT_SRCS=unittest.c unittest.h
.PHONY: tests

tests: test_newton_interval test_check_nan test_check_strict_monotonicity
	@./test_newton_interval
	@./test_check_nan
	@./test_check_strict_monotonicity

test_newton_interval: $(NEWTON_SRCS) $(UNIT_SRCS) test_newton_interval.c
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ \
	$(SRCDIR)/newton_interval.c unittest.c $@.c $(LIBS)

test_check_nan: $(NEWTON_SRCS) $(UNIT_SRCS) test_check_nan.c
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ \
	$(SRCDIR)/newton_interval.c unittest.c $@.c $(LIBS)

test_check_strict_monotonicity: $(NEWTON_SRCS) $(UNIT_SRCS) test_check_strict_monotonicity.c
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ \
	$(SRCDIR)/newton_interval.c unittest.c $@.c $(LIBS)
