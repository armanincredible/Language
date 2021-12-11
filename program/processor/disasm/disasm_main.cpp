#include "disasm.h"

#define CHECK_ERROR_(name, type_error, ret)                                             \
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
    FILE* input_file = fopen ("../calc/code.rb", "rb");
    CHECK_ERROR_(input_file, "NOT OPENED", -1);

    char* array_rb = make_array (input_file);
    CHECK_ERROR_(array_rb, "NULL ADRESS", -1);

    write_file (input_file, array_rb);

    if (ferror(input_file))
    {
        printf ("ERROR in function : %s \n"
                "reading input_file falled\n", __func__);
        return -1;
    }

    free (array_rb);
    fclose (input_file);

    return 0;
}

#undef CHECK_ERROR_