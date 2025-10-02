# 使用 libdw (elfutils)
CFLAGS = -g -O0 -Wall -Wextra
LDFLAGS = -ldw -lelf

TARGET = dwarf_parser
SRCS = dwarf_parser.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

test: $(TARGET)
	./$(TARGET) ../deet/samples/segfault 0x401186

.PHONY: all clean test