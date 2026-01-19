CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = elf_parser.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean test test-unit test-integration

all: elf_viewer

# Main executable
elf_viewer: main.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Object files
%.o: %.c elf_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Unit tests (function-level)
test_elf_parser: test_elf_parser.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Integration tests (black-box)
test_elf_viewer: test_elf_viewer.c
	$(CC) $(CFLAGS) -o $@ $<

# Run all tests
test: test-unit test-integration

test-unit: test_elf_parser
	@echo "=== Running Unit Tests ==="
	./test_elf_parser

test-integration: elf_viewer test_elf_viewer
	@echo "\n=== Running Integration Tests ==="
	./test_elf_viewer

clean:
	rm -f elf_viewer test_elf_parser test_elf_viewer
	rm -f $(OBJS)
	rm -f /tmp/test_*.elf /tmp/test_*.bin

# Legacy support (old single file)
legacy: elf-header-viwer.c
	$(CC) $(CFLAGS) -o elf_viewer_legacy $<
