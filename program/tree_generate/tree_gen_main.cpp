#include "tree_gen.h"

#define CHECK_ERROR_(name, type_error, ret)                                             \
        do{                                                                             \
            if ((name) == NULL)                                                         \
            {                                                                           \
                printf ("\nERROR in function : %s \n"                                   \
                        "%s have %s on line %d\n", __func__,                            \
                        #name, type_error, __LINE__);                                   \
                return ret;                                                             \
            }                                                                           \
        } while (0)

int main ()
{
    FILE* code_input = fopen ("../../input/code.txt", "r");
    CHECK_ERROR_(code_input, "NOT OPENED", -1);

    int file_size = size_file (code_input);
    char* str = (char*) calloc (file_size, sizeof(char));
    if (str == NULL)
    {
        printf ("ERROR STR ADDRESS\n");
        return -1;
    }
    fread (str, sizeof(char), file_size, code_input);

    node_st nodes_st = {};
    tree_nodes_ctr (&nodes_st);

    make_nodes_arr (&nodes_st, str);

    Tree* tree_node = get_general (&nodes_st);
    tree_dump_visual (tree_node);

    tree_dtor (&tree_node);
    tree_nodes_dtor (&nodes_st);

    memset (str, 0, file_size * sizeof (char));
    free (str);

    if (ferror (code_input))
    {
        printf ("ERROR WITH CODE_INPUT\n");
        return -1;
    }
    fclose (code_input);

    return 0;
}

#undef CHECK_ERROR_