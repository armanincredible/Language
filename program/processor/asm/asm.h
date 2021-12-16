#ifndef ASMHEADER
#define ASMHEADER

#include "..\library\Onegin\dora.h"
#include "..\library\Stack\stack.h"
#include "..\library\headers\codes_operations.h"

const int AMMOUNT_LABELS = 50;
const int MAX_AMOUNT_SMB = 20;

struct Labels
{
       int ip_save;
       char* name;
};

struct Asm_values ///asm_values
{
       char* code_rb;
       int ip;
       int ammount_lbls;
       int not_need_reasm;
       int size_name;
       int am_asm;
       const char* name_of_operaion;
       char* line_operation;
       int cur_operation;
       int amount_operations;
};

int    make_code_rb                   (Onegin *line, Labels* label, const int amount_operations);

char*  copy_in_buffer_from_file_asm   (int* amount_operations);

#endif