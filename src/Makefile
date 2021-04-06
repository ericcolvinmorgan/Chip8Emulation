#We try to detect the OS we are running on, and adjust commands as needed
ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	RMDIR = rmdir
	MKDIR = mkdir
  else # in a bash-like shell, like msys
	CLEANUP = rm -f
	MKDIR = mkdir -p
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=.out
endif

BUILDDIR = build
EXE_FILE = Chip8Emu

#SRCS specifies which source files to compile as part of the project
SRCS = ./ui/main.cpp ./emulator/cpu.cpp

#OBJS specifies which objects files to compile as part of the project
#OBJS=$(SRCS:.cpp=.o)
OBJS= ./$(BUILDDIR)/main.o ./$(BUILDDIR)/cpu.o

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I../dependencies/x86_64-w64-mingw32/include/

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L../dependencies/x86_64-w64-mingw32/lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
# COMPILER_FLAGS = -w -Wl,-subsystem,windows -g
COMPILER_FLAGS = -w -g

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

all: $(SRCS)
	$(CC) $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BUILDDIR)/$(EXE_FILE)

# Remember to run "source $SETUPEMSDKENV" where $SETUPEMSDKENV is the path to the emsdk_env.sh file.
# Start testing server with emrun index.html --no_browser
wasmdev:
	emcc \
	-g \
	./ui/main.cpp \
	./emulator/cpu.cpp \
	-s WASM=1 \
	-s USE_SDL=2 \
	-s USE_SDL_TTF=2 \
	--preload-file ./build/FreeSans.ttf@fontset.ttf \
	-s EMIT_PRODUCERS_SECTION=1 \
	-s EXPORTED_FUNCTIONS='["_main", "_loadROM", "_setTargetSpeed", "_dumpCPUDetail"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
	-o ./build/index.html \
	--shell-file ./web/interface.html

wasm:
	cp ./web/interface.js ../docs/
	emcc \
	-O3 \
	./ui/main.cpp \
	./emulator/cpu.cpp \
	-s WASM=1 \
	-s USE_SDL=2 \
	-s USE_SDL_TTF=2 \
	--preload-file ./build/FreeSans.ttf@fontset.ttf \
	-s EXPORTED_FUNCTIONS='["_main", "_loadROM", "_setTargetSpeed", "_dumpCPUDetail"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
	-o ../docs/index.html \
	--shell-file ./web/interface.html	

clean:
	$(CLEANUP) $(BUILDDIR) $(EXE_FILE)