#include "asm.h"

#define CHECK_ERROR_(name, type_error, ret)                                         \
    do{                                                                             \
        if (name == NULL)                                                           \
        {                                                                           \
            printf ("\nERROR in function : %s \n"                                   \
                    "%s have %s\n", __func__,                                       \
                    #name, type_error);                                             \
            return ret;                                                             \
        }                                                                           \
    } while (0)


int main (void) 
{
    int amount_operations = 0; 

    char* buffer = copy_in_buffer_from_file_asm (&amount_operations); 

    Onegin* line = (Onegin*) calloc (amount_operations, sizeof (Onegin));
    CHECK_ERROR_(line, "ZERO ADRESS (Not Enough Memory)", -1);

    Labels* label = (Labels*) calloc (AMMOUNT_LABELS, sizeof (Labels));
    CHECK_ERROR_(label, "ZERO ADRESS (Not Enough Memory)", -1);

    make_array_adress (buffer, amount_operations, line);

    make_code_rb (line, label, amount_operations);

    free (buffer);

    free (label);

    return 0;
}

#undef CHECK_ERROR_