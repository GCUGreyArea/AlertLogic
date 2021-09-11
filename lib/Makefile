LIBSRC = libfilestat.c \
		 rdFile.c \
		 internal.c \
		 utilsList.c \
		 buffScan.c \
		 utils.c \
		 hashMap.c

NAME = filestat
LIB = lib$(NAME)
LIBDIR =  .

TARGET = $(LIB).a

TEST = test_$(NAME)

TST_SRC = $(TEST).cpp

LIBOBJ := $(patsubst %.c,%.o,$(LIBSRC))
LIBHDR := $(patsubst %.c,%.h,$(LIBSRC))
OBJ    := $(patsubst %.c,%.o,$(SRC))
HDR    := $(patsubst %.c,%.h,$(SRC))
TST    := $(patsubst %.cpp,%.o,$(TEST_SRC))

CC     = gcc
CPP    = g++
CFLAGS = -g -std=c11 -Wall -ggdb3 -fsanitize=address -I$(LIBDIR) -L$(LIBDIR)
GFLAGS = -g -std=c++14 -Wall -ggdb3 -fsanitize=address -I$(LIBDIR) -L$(LIBDIR)

all: $(LIB)

$(TARGET) : $(LIB)
	$(CC) $(CFLAGS) $(OBJ) $(LIB)  -o $(TARGET) -lpthread -l$(LIB) -I$(LIBDIR)

$(LIB) : $(LIBOBJ) $(LIBHDR)
	ar rcs $(TARGET) $(LIBOBJ)
	ranlib $(TARGET)

clean:
	rm -rf *.a *.o

install: $(TARGET)
	cp $(TARGET) $(INSTDIR)

test : $(LIBOBJ) $(LIBHDR) $(LIB) $(TST)
	$(CPP) $(GFLAGS) $(LIBOBJ) $(TST) -o $(TEST) -l$(NAME) -lgtest -lgtest_main -lpthread  -L$(LIBDIR) -I$(LIBDIR)
	./$(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cpp
	$(CPP) -c $(GFLAGS) -c $< -o $@


.PHONEY: test install clean