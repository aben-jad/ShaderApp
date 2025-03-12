inc = -I./dependencies/include/
src =  ./sources/main.c ./sources/glad.c ./sources/ShaderFileHandler.c 
tar = ./release/prog.exe
loc = -L./dependencies/libs/
lib = -lSDL3

arg ?=

ALL:
	gcc $(inc) -o $(tar) $(src) $(loc) $(lib)
RUN:
	$(tar) $(arg)
