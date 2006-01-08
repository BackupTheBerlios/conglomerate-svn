include ../../Makefile.global

CFLAGS = $(CEGUI_CFLAGS)
INCLUDES = -I../include -I../../include -I../../opcode132
LIBS = $(CEGUI_LIBS)  ../../src/libOgreOpcode.a
LIBS_DYNAMIC = $(CEGUI_LIBS) -L../../src/ -L../../opcode132 -lOpcode -lOgreOpcode
TARGET = OgreOpcodeExample

OBJECT = main.o
SOURCES = main.cpp 
HEADERS = ../include/ogreOpcodeExample.h

all: $(TARGET)

static: TARGET_STATIC

clean:
	rm *.o $(TARGET)*

TARGET_STATIC: $(OBJECT)
	$(CC) -o $(TARGET).static $(OBJECT) $(LIBS)

$(TARGET): $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(LIBS_DYNAMIC)

$(OBJECT) : $(SOURCES) $(HEADERS)
	$(CC) -c $(SOURCES) $(CFLAGS) $(INCLUDES)
