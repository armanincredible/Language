#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <cstdlib>
#include <string.h> 

struct Onegin
{
    char* adress;
    int length;
};

char** read_text            (FILE *filestream, char** array, const int str_amount, char *buffer);
int    size_file            (FILE *filestream);
int    make_array_adress    (char* buffer, const int str_amount, Onegin* line);
char*  remove_trash_and_copy_in_buffer (int* str_amount, FILE* file);

int    comp_typical         (const void* first_line_void, const void* second_line_void);
int    comp_reverse         (const void* first_line_void, const void* second_line_void);
void   swap                 (void* line, size_t size, int i, int j);
void   qsort                (void* line, size_t size, int left, int right, int (*comp)(const void*, const void*));
int    compare_char         (const char first_symb, const char second_symb);


int    sort_and_output_text (Onegin* line, const int str_amount, char *buffer);
int    output               (Onegin* line, const int str_amount, FILE* fileoutput);