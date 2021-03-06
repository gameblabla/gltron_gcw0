CC = gcc-6
CXX = g++-6
LD = g++-6
CFLAGS = -O0 -g -Iutil -D_PC -Ilua/include -Isrc/include/video -Isrc/include/video/back -DVERSION="'1.0'" $(shell sdl-config --cflags) -Inebu/include -Isrc/include \
-DDATA_DIR=\".\" -DSNAP_DIR=\".\" -DPREF_DIR=\"~\" -DSEPARATOR="'/'" -D_PC
LDFLAGS     = -lSDLmain -lSDL -lSDL_mixer -lSDL_sound -lpng -lm -lEGL -lGLESv1_CM
EXE = gltron
OBJS = $(patsubst %.c, %.o, $(shell find . -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -name \*.cpp))
OBJS += $(patsubst %.S, %.o, $(shell find . -name \*.S))

all: $(EXE).elf

%.o: %.cpp
	@echo Compiling $<...
	@$(CC) $(CFLAGS) -c $< -o $@

$(EXE).elf: $(OBJS)
	+$(LD) $^ -o $@ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f `find . -name \*.o`
	rm -f $(EXE).tns $(EXE).elf
