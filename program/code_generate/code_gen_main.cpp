#include "code_gen.h"

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
    Tree tree = {};
    Tree* tree_node = &tree;

    char* buffer = 0;
    Onegin* line = make_tree_from_library (tree_node, &buffer);

    make_tree_asm (tree_node);

    tree_dtor (&(tree_node->left_ptr));
    tree_dtor (&(tree_node->right_ptr));
    
    free (buffer);
    free (line);

    printf ("AAA");

    return 0;
}

#undef CHECK_ERROR_