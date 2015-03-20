
TARGET := forward.cgi

C_FILES := $(wildcard src/*.c)
HEADERS := $(wildcard include/*.h)
OBJS := ${C_FILES:.c=.c.o}
LIBS += -ljson-c
CPPFLAGS += -Iinclude
CFLAGS += -std=gnu99 -Wall

ifeq ($(FASTCGI),yes)
	LIBS += -lfcgi
	CPPFLAGS += -DFASTCGI
endif

all: $(TARGET)
.PHONY: all clean clean-all

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

%.c.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

clean: clean-all

clean-all:
	rm -rf $(OBJS) $(TARGET)

