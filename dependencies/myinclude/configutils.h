#ifndef _CONFIG_UTILS_H
#define _CONFIG_UTILS_H
#include <stdio.h>

unsigned int config_file_exist();
void* generate_ui_shader();
void* generate_uniforms(char** _uniforms_names, char** _vals, int _len);
char** get_next_attribute(FILE* _f);

#endif
