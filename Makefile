CFLAGS = -g -O2 -Wall -Wextra -Isrc  $(OPTFLAGS)
LIBS = -ldl $(OPTLIBS)
PREFIX ?= /usr/local

# control to echo commands
Q=@

SRCS=$(wildcard src/**/*.c src/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c, %, $(TEST_SRC))

TARGET = build/libzz.a
SO_TARGET = $(patsubst %.a, %.so, $(TARGET))

# The Target Build
all: $(TARGET) $(SO_TARGET) tests

dev: CFLAGS = -g -Wall -Isrc -Wall -Wextra $(OPTLIBS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJS)
	ar rcs $@ $(OBJS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJS)
	$(CC) -shared -o $@ $(OBJS)

build:
	$(Q)mkdir -p build
	$(Q)mkdir -p bin

# The Unit Tests
.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: $(TESTS)
	sh ./tests/runtests.sh

$(TESTS): %:%.c
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@

# The Cleaner
clean:
	rm -rf build $(OBJS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/
