#add make file
# This makefile is for compiling a simple SDL OpenGL program
# is using mingw
# and g++ compiler
# It assumes that SDL2 and OpenGL are installed and properly configured
# in your system
# It also assumes that the SDL2 and OpenGL libraries are in the default library path
# and the SDL2 include files are in the default include path

windows:
	gcc main.c src/glad.c -o main -Iinclude -Iinclude/glad -Iinclude/KHR -IC:/SDL3/x86_64-w64-mingw32/include -LC:/SDL3/x86_64-w64-mingw32/lib -lSDL3 -lopengl32 -lm

linux:
	gcc main.c src/glad.c -Iinclude -Iinclude/glad -Iinclude/KHR -o main -lSDL3 -lm
#gcc main.c src/glad.c -o main -lSDL3 -lm
