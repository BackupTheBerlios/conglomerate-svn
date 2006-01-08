include Makefile.global

CFLAGS =
INCLUDES = 
LIBS = 

OPCODE_DIR = opcode132/
OGREOPCODE_DIR = src/
EXAMPLE_DIR = example/src/



.PHONY : all
all: opcode ogreopcode

opcode:
	$(MAKE) -C $(OPCODE_DIR)

ogreopcode: opcode
	$(MAKE) -C $(OGREOPCODE_DIR)

example: ogreopcode
	$(MAKE) -C $(EXAMPLE_DIR)


static: opcode_static ogreopcode_static


opcode_static:
	$(MAKE) -C $(OPCODE_DIR) static

ogreopcode_static: opcode_static
	$(MAKE) -C $(OGREOPCODE_DIR) static

example_static: ogreopcode_static
	$(MAKE) -C $(EXAMPLE_DIR) static


.PHONY : clean
clean:
	rm $(OPCODE_DIR)*.a $(OPCODE_DIR)*.o $(OPCODE_DIR)*.so* \
			$(OGREOPCODE_DIR)*.a $(OGREOPCODE_DIR)*.o $(OGREOPCODE_DIR)*.so* 
	$(MAKE) -C $(EXAMPLE_DIR) clean
