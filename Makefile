CFLAGS = -O2 -Wall -Wextra -Ilib -DNDEBUG $(OPTFLAGS)
LIBS = -ldl $(OPTLIBS)
PREFIX ?= /usr/local

# control to echo commands
Q=@

# this ensures that all is the default target.
all: 
#----------------------------------------------------------------------
# build the library
#----------------------------------------------------------------------
LIB_SRCS = $(wildcard lib/*.c)
LIB_OBJS = $(patsubst %.c, %.o, $(LIB_SRCS))

LIB_TARGET = build/libzz.a
LIB_SO_TARGET = $(patsubst %.a, %.so, $(LIB_TARGET))

$(LIB_TARGET): CFLAGS += -fPIC
$(LIB_TARGET): build $(LIB_OBJS)
	ar rcs $@ $(LIB_OBJS)
	ranlib $@

$(LIB_SO_TARGET): $(LIB_TARGET) $(LIB_OBJS)
	$(CC) -shared -o $@ $(LIB_OBJS)

build:
	$(Q)mkdir -p build
	$(Q)mkdir -p bin

#----------------------------------------------------------------------
# build the executables
#----------------------------------------------------------------------
BINDIR=./bin
SRCDIR=./src
EXES = wf
EXES := $(addprefix $(BINDIR)/, $(EXES))
EXE_SRCS = $(wildcard src/*.c)
EXE_OBJS = $(patsubst %.c, %.o, $(EXE_SRCS))

$(EXE_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXES): $(BINDIR)/% : $(SRCDIR)/%.o
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

# additional pre-requsites

#----------------------------------------------------------------------
# run the tests
#----------------------------------------------------------------------
TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c, %, $(TEST_SRC))

.PHONY: tests
tests: CFLAGS += $(LIB_TARGET)
tests: $(TESTS)
	sh ./tests/runtests.sh

$(TESTS): %:%.c
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@

#----------------------------------------------------------------------
# Other targets.
#----------------------------------------------------------------------
# The Cleaner
clean:
	rm -rf build $(OBJS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`
	rm -rf bin
	rm -rf $(EXE_OBJS)

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(LIB_TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Target Build
all: $(LIB_TARGET) $(LIB_SO_TARGET) tests $(EXES)

dev: CFLAGS = -g -Wall -Isrc -Wall -Wextra $(OPTLIBS)
dev: all
