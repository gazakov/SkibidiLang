CC = gcc
CFLAGS = -Wall -Wextra -pedantic
DEBUGFLAGS = -g
OPTFLAGS = -O2
SRCS = skibidilang.c
HDRS = skibidilang.h
OBJS = $(SRCS:.c=.o)
TARGET = skibidilang

.PHONY: all clean debug release

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += $(DEBUGFLAGS)
debug: clean $(TARGET)

release: CFLAGS += $(OPTFLAGS)
release: clean $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
