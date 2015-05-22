SHELL = /bin/sh

CC = gcc
CFLAGS = -Wno-missing-field-initializers -O3
ALL_FLAGS = -std=gnu99 -lm

TARGET = dpd

OBJS_DIR = build

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(OBJS_DIR)/%.o)

.PHONY: all
all: objs_dir $(TARGET) 

.PHONY: objs_dir
objs_dir: 
ifdef SystemRoot
	if not exist $(OBJS_DIR) mkdir $(OBJS_DIR)
else
	mkdir -p $(OBJS_DIR)
endif

$(TARGET): $(OBJS) 
	$(CC) $(ALL_FLAGS) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(OBJS_DIR)/%.o: %.c
	$(CC) -c $(ALL_FLAGS) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
ifdef SystemRoot
	DEL $(TARGET).exe 2>NUL
	DEL /S /Q $(OBJS_DIR) 2>NUL
	DEL .depend 2>NUL
else
	rm $(TARGET) 2>/dev/null || true
	rm -rd $(OBJS_DIR) 2>/dev/null || true
	rm .depend 2>/dev/null || true
endif

.PHONY: depend dep
depend dep:
	$(CXX) -E -MM $(SRCS) |\
		sed 's/^[a-zA-Z_0-9]*.o/$(OBJS_DIR)\/&/' > .depend

-include .depend
