inc = -I./dependencies/include/ -I./dependencies/myinclude
src =  ./sources/main.c ./sources/glad.c ./sources/fileutils.c ./sources/mymath.c ./sources/myglobals.c ./sources/shaderutils.c ./sources/configutils.c ./sources/mystring.c ./sources/uiutils.c
src2 =  ./sources/main3.c ./sources/glad.c ./sources/mymath.c ./sources/sdffont.c ./sources/fileutils.c ./sources/myglobals.c  ./sources/mystring.c
tar = ./release/prog.exe
loc = -L./dependencies/libs/
lib = -lSDL3 -lSDL3_ttf

proj1 =./release/projects/proj1/
proj2 =./release/projects/proj2/

arg = $(proj2)


TEST:
	clang $(inc) -o $(tar) $(src2) $(loc) $(lib)
ALL:
	clang $(inc) -o $(tar) $(src) $(loc) $(lib)
RUN:
	$(tar) $(arg)
