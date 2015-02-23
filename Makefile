SHELL = /bin/sh

CC = gcc
# CFLAGS = -Wall -Wextra -pedantic-g
CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -g
ALL_FLAGS = -std=gnu99

TARGET = main

OBJS_DIR = build

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(OBJS_DIR)/%.o)

.PHONY: all
all: objs_dir $(TARGET) 

.PHONY: objs_dir
objs_dir: 
	mkdir -p $(OBJS_DIR)

$(TARGET): $(OBJS) 
	$(CC) $(ALL_FLAGS) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(OBJS_DIR)/%.o: %.c
	$(CC) -c $(ALL_FLAGS) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(TARGET) 2>/dev/null || true
	rm $(OBJS) 2>/dev/null || true
	rm -rd $(OBJS_DIR) 2>/dev/null || true
	# rm .depend 2>/dev/null || true

.PHONY: depend dep
depend dep:
	$(CXX) -E -MM $(SRCS) |\
		sed 's/^[a-zA-Z_0-9]*.o/$(OBJS_DIR)\/&/' > .depend

.PHONY: docs
docs:
	doxygen docs/config

.PHONY: run
run: all
	./$(TARGET) -v ./test/02092014-000000-MHZ.6I36

-include .depend
