#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* concatenate(char* _str1, char* _str2)
{
	int s_len1 = strlen(_str1);
	int s_len2 = strlen(_str2);
	char* res = (char*)malloc((s_len1 + s_len2 + 1) * sizeof(char));

	int i = 0;
	while (*_str1)
	{
		res[i] = *_str1;
		_str1++;
		i++;
	}

	while (*_str2)
	{
		res[i] = *_str2;
		_str2++;
		i++;
	}

	res[i] = 0;
	return res;
}

int get_cursor_from_line_number(char* _src, int _line)
{
	int len = 0;
	while (_line)
	{
		if (_src[len] == '\n')
			_line--;
		len++;
	}
	return len;
}

char* insert_string_at_line(char* _src, char* _str, int _line)
{
	int srcLength = strlen(_src);
	int startCursor = get_cursor_from_line_number(_src, _line);
	int strLength = strlen(_str);

	char* res = (char*)malloc((srcLength + strLength + 2) * sizeof(char));

	res[startCursor + strLength] = '\n';
	strncpy(res, _src, startCursor);
	strncpy(res + startCursor, _str, strLength);
	strcpy(res + startCursor + strLength + 1, _src + startCursor);

	return res;
}

char* insert_strings_at_line(char* _src, char** _strs, int _str_size,  int _line)
{
	int srcLength = strlen(_src);
	int startCursor = get_cursor_from_line_number(_src, _line);
	int finalLength = srcLength + 1;
	int strs_index = 0;
	int strsLength[_str_size];
	while (strs_index < _str_size)
	{
		//printf("test : %d\n", strlen(_strs[0]));
		strsLength[strs_index] = strlen(_strs[strs_index]);
		//printf("index : %d\n", strs_index);
		finalLength += strsLength[strs_index];
		strs_index++;
		//printf("index : %d\n", strs_index);
	}

	//printf("en\n");
	strs_index = 0;

	char* res = (char*)malloc(finalLength * sizeof(char));

	//res[startCursor + strLength] = '\n';
	strncpy(res, _src, startCursor);
	
	int currentCursor = startCursor;

	while (strs_index < _str_size)
	{
		strncpy(res + currentCursor, _strs[strs_index], strsLength[strs_index]);
		currentCursor += strsLength[strs_index];
		strs_index++;
	}
	strcpy(res + currentCursor, _src + startCursor);

	return res;
	
}

char* generate_function_call(char* _prefix, char* _name, int _params_len, char** _params_value)
{
	int finalLength = 5;

	int prefixLength = strlen(_prefix);
	int nameLength = strlen(_name);
	int paramsLength[_params_len];

	finalLength += prefixLength;
	finalLength += nameLength;

	int paramsIndex = 0;
	while (paramsIndex < _params_len)
	{
		paramsLength[paramsIndex] = strlen(_params_value[paramsIndex]);
		finalLength += paramsLength[paramsIndex];
		paramsIndex++;
	}

	if (_params_len > 1)
		finalLength += 2 * (_params_len-1);
	char* res = (char*)malloc(finalLength * sizeof(char));
	res[finalLength-1] = 0;
	res[finalLength-2] = '\n';
	res[finalLength-3] = ';';
	res[finalLength-4] = ')';

	paramsIndex = 0;
	int currentCursor = 0;
	strncpy(res, _prefix, prefixLength);
	currentCursor += prefixLength;
	strncpy(res + currentCursor, _name, nameLength);
	currentCursor += nameLength;
	res[currentCursor] = '(';
	currentCursor += 1;

	while (paramsIndex < _params_len)
	{
		strncpy(res + currentCursor, _params_value[paramsIndex], paramsLength[paramsIndex]);
		currentCursor += paramsLength[paramsIndex];
		if (paramsIndex < _params_len - 1)
		{
			res[currentCursor] = ',';
			res[currentCursor + 1] = ' ';
			currentCursor += 2;
		}
		paramsIndex++;
	}

	//printf("%s\n", res);
	return res;

}

char* generate_text_from_strings(char** _strs, int _str_size)
{
	int finalLength = 1;
	int strs_index = 0;
	int strsLength[_str_size];
	while (strs_index < _str_size)
	{
		strsLength[strs_index] = strlen(_strs[strs_index]);
		finalLength += strsLength[strs_index];
		strs_index++;
	}

	strs_index = 0;

	char* res = (char*)malloc(finalLength * sizeof(char));
	
	int currentCursor = 0;

	while (strs_index < _str_size)
	{
		strncpy(res + currentCursor, _strs[strs_index], strsLength[strs_index]);
		currentCursor += strsLength[strs_index];
		strs_index++;
	}

	res[finalLength-1] = 0;

	return res;
	
}

char* ftostr(float _f)
{
	int entier_part = (int)_f;
	int decimal_part = (int)((_f - (float)entier_part) * 10000.0f);
	int sign = 0;
	int i = 1;
        int r = entier_part;

	if (_f < 0)
	{
		i++;
		sign++;
		r *= -1;
		decimal_part *= -1;
	}

        while (r >= 10)
        {
                r = r / 10;
                i++;
        }

        char *res = (char*)malloc((i+6) * sizeof(char));
        res[i+5] = 0;
        res[i+4] = (decimal_part%10) + 48;
        res[i+3] = ((decimal_part%100))/10 + 48;
        res[i+2] = ((decimal_part%1000))/100 + 48;
        res[i+1] = decimal_part/1000 + 48;
        res[i] = '.';
        i--;

        r = entier_part * ((sign == 1) ? -1 : 1);
        int q;
        while (i >= sign)
        {
                q = r % 10;
                r = r / 10;
                res[i] = q + 48;
                i--;
        }
	if (sign)
		res[0] = '-';

        return res;
}
