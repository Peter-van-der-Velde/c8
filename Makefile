CC=gcc
CFLAGS=-g -Wall -Wextra -Isrc -rdynamic $(OPTFLAGS) -I/usr/include/SDL2
LIBS=-ldl -lSDL2 $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
DEPS=$(wildcard src/**/*.h src/*.h)

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=./build/c8

# The Target Build
all: CFLAGS += -DNDEBUG -O2
all: build $(TARGET)

dev: CFLAGS += -Os
dev: build $(TARGET)

$(TARGET): $(OBJECTS) $(DEPS)
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit Tests
.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: $(TESTS)
	sh ./tests/runtests.sh

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)\
		|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true

