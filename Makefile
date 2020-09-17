CFLAGS=-std=gnu11 -Wall -Wextra -pedantic -Wshadow -Wconversion -Wdouble-promotion
CFLAGS+=-Wnull-dereference -Wformat=2 -Wformat-overflow -Wformat-truncation
CFLAGS+=-Wswitch-default -Wformat-security -Wmissing-declarations -Winit-self
CFLAGS+=-Wpointer-arith -Wswitch-enum -Wundef -Wstrict-prototypes
CFLAGS+=-D_FORTIFY_SOURCE=1 -fstack-protector -fno-common
CFLAGS+=-Isrc -I/usr/include/SDL2 -rdynamic $(OPTFLAGS)
LDFLAGS=-Wl,-O1 -Wl,-z,relro -Wl,-z,now
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

dev: CFLAGS += -Og -g
dev: build $(TARGET)

$(TARGET): $(OBJECTS) $(DEPS)
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@

build:
	@mkdir -p build

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
	install -d $(DESTDIR)/$(PREFIX)/bin/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/bin/

.PHONY: run
run:
	$(TARGET)

.PHONY: format
format:
	clang-format -i $(SOURCES) $(DEPS)

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)\
		|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true

