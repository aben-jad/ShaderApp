#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** ParseShader(char* _filePath)
{
	FILE* fp = fopen(_filePath, "r");
	char **s = (char **)malloc(sizeof(char**));

	if (s == NULL)
	{
		printf("heap s\n");
		exit(0);
	}

	if (fp == NULL)
	{
		printf("file not found\n");
		exit(0);
	}


	fseek(fp, SEEK_SET, SEEK_END);

	int len = ftell(fp);

	fseek(fp, SEEK_SET, SEEK_SET);

	char arr[len + 1];
	arr[len] = 0;

	fread(arr, sizeof(char), len, fp);

	char * vs = strstr(arr, "#shader vertex");
	char * vsBegin = strstr(vs, "#version");
	char * vsEnd = strstr(vsBegin, "#end");

	char* myvsh = (char *)malloc(vsEnd - vsBegin + 1);
	if (myvsh == NULL)
	{
		printf("heap v\n");
		exit(0);
	}
	myvsh[vsEnd - vsBegin] = 0;

	memcpy(myvsh, vsBegin, vsEnd - vsBegin);

	s[0] = myvsh;

	char *fs = strstr(vsEnd, "#shader frag");
	char * fsBegin = strstr(fs, "#version");
	char * fsEnd = strstr(fsBegin, "#end");

	char *myfsh = (char *)malloc(fsEnd - fsBegin + 1);
	if (myfsh == NULL)
	{
		printf("heap f\n");
		exit(0);
	}
	myfsh[fsEnd - fsBegin] = 0;

	memcpy(myfsh, fsBegin, fsEnd - fsBegin);

	fclose(fp);


	s[1] = myfsh;

	return s;

}

void FreeShaderParserHeap(char **_ptr)
{
	free(_ptr[0]);
	free(_ptr[1]);
	free(_ptr);

}

