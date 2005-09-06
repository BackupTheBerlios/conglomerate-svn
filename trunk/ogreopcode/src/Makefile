include ../Makefile.global

CFLAGS = $(OGRE_CFLAGS)
INCLUDES = -I../include/ -I../opcode131
LIBS = 

TARGET_DYNAMIC_SO = libOgreOpcode.so
VERSION_DYNAMIC = .0
SUBVERSION_DYNAMIC = .3

CFLAGS_DYNAMIC = $(CFLAGS) -fPIC
LIBS_DYNAMIC = $(LIBS) -shared -W1,-soname,$(TARGET_DYNAMIC_SO)$(VERSION_DYNAMIC)

TARGET_STATIC = libOgreOpcode.a
TARGET_DYNAMIC = $(TARGET_DYNAMIC_SO)$(VERSION_DYNAMIC)$(SUBVERSION_DYNAMIC)

SOURCES = OgreCollisionContext.cpp OgreCollisionManager.cpp \
					OgreCollisionShape.cpp OgreOpcodeDebugObject.cpp
HEADERS = ../include/OgreCollisionContext.h \
			../include/OgreCollisionManager.h ../include/OgreCollisionObject.h \
			../include/OgreCollisionReporter.h ../include/OgreCollisionShape.h \
			../include/OgreCollisionTypes.h ../include/OgreHash.h \
			../include/OgreKeyArray.h ../include/OgreNodes.h \
			../include/OgreOpcode.h ../include/OgreOpcodeDebugObject.h \
			../include/OgreOpcodeExports.h ../include/OgreOpcodeMath.h

OBJECTS = OgreCollisionContext.o OgreCollisionManager.o \
			OgreCollisionShape.o OgreOpcodeDebugObject.o


.PHONY : all
all: $(TARGET_DYNAMIC)

.PHONY : static
static: $(TARGET_STATIC)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CC) -c $(SOURCES) $(CFLAGS) $(INCLUDES)

OBJECTS_DYNAMIC: $(SOURCES) $(HEADERS)
	$(CC) -c $(SOURCES) $(CFLAGS_DYNAMIC) $(INCLUDES)

$(TARGET_STATIC): $(OBJECTS)
	ar rcs $(TARGET_STATIC) $(OBJECTS) ../opcode131/*.o

$(TARGET_DYNAMIC): OBJECTS_DYNAMIC
	$(CC) $(LIBS_DYNAMIC) -o $(TARGET_DYNAMIC) $(OBJECTS)
	ln -sf $(TARGET_DYNAMIC) $(TARGET_DYNAMIC_SO)$(VERSION_DYNAMIC)
	ln -sf $(TARGET_DYNAMIC_SO)$(VERSION_DYNAMIC) $(TARGET_DYNAMIC_SO)
	
