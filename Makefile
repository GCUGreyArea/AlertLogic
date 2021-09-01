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
CFLAGS = -g -std=c11 -Wall -ggdb3 -I$(LIBDIR) -L$(LIBDIR)
GFLAGS = -g -std=c++14 -Wall -ggdb3  -I$(LIBDIR) -L$(LIBDIR)

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

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) --infile tests/resouces/ascii.txt --wordstats --highfreqwordstats --charstats

doxygen:
	@echo "Making Doxygen documentation"
	cd $(DOXYDIR) && doxygen Doxyfile

test:
	cd $(LIBDIR) && make
	cd $(TESTDIR) && make
	./$(TESTDIR)/$(TESTTARGET)

help:
	@echo "Run: 'make' to build project with test"
	@echo "Run: 'make test' to build and run unit tests"
	@echo "Run: 'make doxygen' to build doxygen documentation"

# .PHONEY: all doxygen
