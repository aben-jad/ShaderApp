#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mystring.h>
#include <myglobals.h>

char* read_file(char* _path)
{
	FILE* fp = fopen(_path, "r");
	if (fp == NULL)
	{
		printf("file doesn't exist anymore! %s\n", _path);
		fclose(fp);

		return NULL;
	}
	int len = 0;
	char c;
	while (fread(&c, sizeof(char), 1, fp))
		len++;

	fseek(fp, SEEK_SET, SEEK_SET);

	char* arr = (char*)malloc(sizeof(char) * (len + 1));
	arr[len] = 0;

	fread(arr, sizeof(char), len, fp);
	fclose(fp);

	return arr;
}

char* read_main_file()
{
	char* filePath = concatenate(g_project_path, "\\main.glsl");

	char* res = read_file(filePath);

	free(filePath);

	return res;
}

unsigned int shader_file_exist()
{
	char* filePath = concatenate(g_project_path, "\\main.glsl");
	FILE* fp = fopen(filePath, "r");

	free(filePath);
	fclose(fp);

	return (fp != NULL);
}

char* get_next_line(FILE* _f)
{ 
	char tmp;
	char* buffer;

	int line_size = 0;
	if (fread(&tmp, sizeof(char), 1, _f) == 0)
		return NULL;
	line_size++;

	while(tmp != '\n')
	{
		fread(&tmp, sizeof(char), 1, _f);
		line_size++;
	}

	fseek(_f, -(line_size+1), SEEK_CUR);
	buffer = (char*)malloc((line_size) * sizeof(char));
	fread(buffer, sizeof(char), line_size, _f);
	buffer[line_size-1] = 0;

	return buffer;
}

char* get_next_line_with_new_line(FILE* _f)
{ 
	char tmp;
	char* buffer;

	int line_size = 0;
	if (fread(&tmp, sizeof(char), 1, _f) == 0)
		return NULL;
	line_size++;

	while(tmp != '\n')
	{
		fread(&tmp, sizeof(char), 1, _f);
		line_size++;
	}

	fseek(_f, -(line_size+1), SEEK_CUR);
	buffer = (char*)malloc((line_size+1) * sizeof(char));
	fread(buffer, sizeof(char), line_size, _f);
	buffer[line_size] = 0;

	return buffer;
}
