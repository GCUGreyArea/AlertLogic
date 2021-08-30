LIBSRC = libfilestat.c \
		 rdFile.c \
		 internal.c \
		 utilsList.c \
		 buffScan.c \
		 utils.c \
		 hashMap.c

LIB = libfilestat.a
LIBDIR =  .

LIBOBJ := $(patsubst %.c,%.o,$(LIBSRC))
LIBHDR := $(patsubst %.c,%.h,$(LIBSRC))
OBJ    := $(patsubst %.c,%.o,$(SRC))
HDR    := $(patsubst %.c,%.h,$(SRC))

CC     = gcc
CFLAGS = -g -std=c11 -Wall -ggdb3 -I$(LIBDIR) -L$(LIBDIR)

all: $(LIB)

$(TARGET) : $(LIB)
	gcc $(CFLAGS) $(OBJ) $(LIB) -o $(TARGET) -lpthread -l$(LIB) -I$(LIBDIR)

$(LIB) : $(LIBOBJ) $(LIBHDR)
	ar rcs $(LIB) $(LIBOBJ)
	ranlib $(LIB)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.a *.o

install: $(TARGET)
	cp $(TARGET) $(INSTDIR)

test: $()
