TARGET = filestat

TESTDIR    = tests
TESTTARGET = test_$(TARGET)
TESTSRC    = test_$(TARGET).cpp
DOXYDIR	   = Doxygen

LIBSRC = lib$(TARGET).c

SRC = filestat.c
LIB = lib$(TARGET).a

LIBDIR =  ./lib

LIBOBJ  := $(patsubst %.c,%.o,$(LIBSRC))
LIBHDR  := $(patsubst %.c,%.h,$(LIBSRC))
OBJ     := $(patsubst %.c,%.o,$(SRC))
HDR     := $(patsubst %.c,%.h,$(SRC))
TESTOBJ := $(patsubst %.cpp,%.o,$(TESTSRC))
TESTHDR := $(patsubst %.cpp,%.h,$(TESTSRC))

CC     = gcc
CPP    = g++
VG     = valgrind
VGARGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose
VGLOG  = --log-file=$(TARGET)-valgrind-out.txt
CFLAGS = -g -std=c11 -Wall -ggdb3 -fsanitize=address -I$(LIBDIR) -L$(LIBDIR)
GFLAGS = -g -std=c++14 -Wall -ggdb3 -fsanitize=address -I$(LIBDIR) -L$(LIBDIR)
RFLAGS = --infile ./tests/resouces/ascii.txt --tablesize 4096 --wordstats --highfreqwordstats --charstats

all: $(LIB) $(TARGET) $(TESTTARGET)

$(TARGET) : $(OBJ) $(HDR) $(LIB) $(TESTTARGET)
	gcc $(CFLAGS) $(OBJ) -o $(TARGET) -lpthread -l$(TARGET) -I$(LIBDIR)

$(LIB) : $(LIBDIR)/$(LIBOBJ) $(LIBDIR)/$(LIBHDR)
	cd $(LIBDIR) && make

$(TESTTARGET) : $(TESTDIR)/$(TESTOBJ) $(LIB)
	@echo "Making tests"
	cd $(TESTDIR) && make

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cpp
	$(CPP) -c $(GFLAGS) -c $< -o $@

clean:
	rm -rf *.a *.o $(TARGET)
	rm -rf Doxygen/doxygen_log.txt
	rm -rf Doxygen/generated
	rm -rf Doxygen/man
	rm -rf $(LIBDIR)/*.o $(LIBDIR)/*.a
	rm -rf $(TESTDIR)/*.o $(TESTDIR)/$(TESTTARGET)

install: $(TARGET)
	cp $(TARGET) $(INSTDIR)

valgrind: valgrind $(TARGET)
	@echo Running valgrind
	$(VG) $(VGARGS) ./$(TARGET) --infile tests/resouces/ascii.txt --wordstats --highfreqwordstats --charstats


doxygen:
	@echo "Making Doxygen documentation"
	cd $(DOXYDIR) && doxygen Doxyfile

test: ./$(TESTDIR)/$(TESTTARGET)
	cd $(LIBDIR) && make
	cd $(TESTDIR) && make
	cd $(TESTDIR) && ./$(TESTTARGET)

run : $(TARGET)
	./$(TARGET) $(RFLAGS)

help:
	@echo "Run: 'make' to build project with test"
	@echo "Run: 'make test' to build and run unit tests"
	@echo "Run: 'make doxygen' to build doxygen documentation"

.PHONEY: all doxygen valgrind help

