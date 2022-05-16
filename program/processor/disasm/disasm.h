#ifndef DISASMHEADER
#define DISASMHEADER

#include "..\library\Onegin\dora.h"
#include "..\library\Stack\stack.h"
#include "..\library\headers\codes_operations.h"

char* make_array (FILE* input_file);

int write_file (FILE* input_file, char* array_rb);

#endif