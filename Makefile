cudir = c:\Users\pc\Desktop\SDL_Projects\SDL2_NewProject

ALL:
	gcc -Ic:\Users\pc\Desktop\SDL_Projects\SDL2_NewProject\dependencies\include\ .\sources\main.c .\sources\glad.c .\sources\ShaderFileHandler.c .\sources\ErrorHandler.c -o .\release\prog -L.\dependencies\libs\ -lSDL2
RUN:
	.\release\prog
