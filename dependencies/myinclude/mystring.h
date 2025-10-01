#ifndef _MY_STRING_H_
#define _MY_STRING_H_

char* concatenate(char* _str1, char* _str2);
int get_cursor_from_line_number(char* _src, int _line);
char* insert_string_at_line(char* _src, char* _str, int _line);
char* insert_strings_at_line(char* _src, char** _strs, int _str_size, int _line);
char* generate_text_from_strings(char** _strs, int _str_size);
char* generate_function_call(char* _prefix, char* _name, int _params_len, char** _params_value);
char* ftostr(float _f);

#endif
