#include "tree.h"

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

#define $ printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);

#define _TREE_VERIFICATE_(name)                         \
        do{                                             \
            if (tree_verificate (name) == -1)           \
            {                                           \
                return -1;                              \
            }                                           \
        }while(0)

static int write_tree_element_vis (Tree* tree_node, FILE* output_file);

static int tree_ver_recurs (Tree* tree_node);

static Tree* copy_tree_node_recurse (Tree* new_tree_node, Tree* tree_node);

#define LEFT_NODE_ tree_node->left_ptr

#define RIGHT_NODE_ tree_node->right_ptr

int tree_verificate (Tree* tree_node)
{
    CHECK_ERROR_(tree_node, "HAVE ZERO ADRESS", -1);

    tree_node->level = tree_node->level + 1;

    if (tree_ver_recurs (tree_node) == -1)
    {
        return -1;
    }
    
    return 0;
}

#define tree_ver_(arg)                                                      \
        do                                                                  \
        {                                                                   \
            Tree* node_ptr = tree_node->arg;                                \
            if (node_ptr != NULL)                                           \
            {                                                               \
                node_ptr->level = node_ptr->level + 1;                      \
                                                                            \
                if (node_ptr->level > tree_node->level)                     \
                {                                                           \
                    printf ("TREE HAVE CYCLE ERROR\n");                     \
                    return -1;                                              \
                }                                                           \
                if (node_ptr->pre_ptr != tree_node)                         \
                {                                                           \
                    /*printf ("TREE HAVE PARRENT CONNECT ERROR\n");         \
                    return -1;*/                                            \
                    return 0;                                              \
                }                                                           \
                if (tree_ver_recurs(node_ptr) == -1)                        \
                {                                                           \
                    return -1;                                              \
                }                                                           \
            }                                                               \
        } while (0)
        
static int tree_ver_recurs (Tree* tree_node)
{
    tree_ver_(left_ptr);
    tree_ver_(right_ptr);
    return 0;
}

#undef tree_ver_

int tree_ctor (Tree** tree_node)
{
    CHECK_ERROR_(tree_node, "NULL ADRESS", -1);

    *tree_node = (Tree*) calloc (1, sizeof (Tree));
    CHECK_ERROR_(*tree_node, "NULL ADRESS (NOT FREE MEMORY)", -1);

    Tree* node_ptr = *tree_node;

    node_ptr->left_ptr =  NULL;
    node_ptr->right_ptr = NULL;
    node_ptr->pre_ptr = NULL;

    node_ptr->level = 0;

    node_ptr->value.data = NULL;

    node_ptr->size_value = 0;

    return 0;
}

#define _tree_dtor_elem_(name)                                      \
        do {                                                        \
            if ((node_ptr)->name != NULL)                           \
            {                                                       \
                tree_dtor (&((node_ptr)->name));                    \
            }                                                       \
        }while (0)

int tree_dtor (Tree** tree_node)
{
    CHECK_ERROR_(tree_node, "NULL ADRESS", -1);

    Tree* node_ptr = *tree_node;

    _TREE_VERIFICATE_(*tree_node);

    _tree_dtor_elem_(left_ptr);

    _tree_dtor_elem_(right_ptr);

    node_ptr->level = 0;

    node_ptr->size_value = 0;
    
    node_ptr->value.data = NULL;

    node_ptr->left_ptr = NULL;
    node_ptr->right_ptr = NULL;
    
    node_ptr->pre_ptr = NULL;/////////////////////////////////////////////////////////CHECK IT

    free (*(tree_node));    
    //*tree_node = NULL;

    return 0;
}

#undef _tree_dtor_elem_
        
int tree_push (Tree* tree_node, int side, node_t name)
{
    if (tree_node == NULL)
    {
        CHECK_ERROR_(tree_node, "NULL ADRESS", -1);
    }

    if (side == IN)
    {
        tree_node->value.data = name;
    }

    if (side == LEFT)
    {
        Tree* node_ptr = tree_node->left_ptr;

        tree_ctor (&(node_ptr));

        node_ptr->value.data = name;
        
        node_ptr->pre_ptr = tree_node;

        tree_node->left_ptr = node_ptr;
    }
    
    if (side == RIGHT)
    {
        Tree* node_ptr = tree_node->right_ptr;

        tree_ctor (&(node_ptr));

        node_ptr->value.data = name;
        
        node_ptr->pre_ptr = tree_node;

        tree_node->right_ptr = node_ptr;
    }

    return 0;
}


Tree* copy_tree_node (Tree* tree_node)
{
    CHECK_ERROR_(tree_node, "HAVE NULL ADRESS", NULL);

    Tree* new_tree_node = NULL;
    tree_ctor (&new_tree_node);

    new_tree_node->pre_ptr = tree_node->pre_ptr;
    new_tree_node->level = tree_node->level;

    tree_push (new_tree_node, IN, tree_node->value.data);

    copy_tree_node_recurse (new_tree_node, tree_node);

    return new_tree_node;
}

static Tree* copy_tree_node_recurse (Tree* new_tree_node, Tree* tree_node)
{
    if (LEFT_NODE_ != NULL)
    {
        tree_push(new_tree_node, LEFT, LEFT_NODE_->value.data);

        copy_tree_node_recurse (new_tree_node->left_ptr, LEFT_NODE_);
    }

    if (RIGHT_NODE_ != NULL)
    {
        tree_push(new_tree_node, LEFT, LEFT_NODE_->value.data);

        copy_tree_node_recurse (new_tree_node->right_ptr, RIGHT_NODE_);
    }

    return 0;
}

Tree* New_Node (const node_t value, Tree* left_ptr, Tree* right_ptr)
{
    Tree* tree_node = NULL;
    tree_ctor (&tree_node);

    tree_push (tree_node, IN, value);

    Tree* new_left_ptr = NULL;
    Tree* new_right_ptr = NULL; 

    if (left_ptr != NULL)
    {
        new_left_ptr = copy_tree_node (left_ptr);
        new_left_ptr->pre_ptr = tree_node;

        //tree_node->level = new_left_ptr->level - 1;
    }
    if (right_ptr != NULL)
    {
        new_right_ptr = copy_tree_node (right_ptr); 
        new_right_ptr->pre_ptr = tree_node;

        //tree_node->level = new_right_ptr->level - 1;
    }

    //new_left_ptr->pre_ptr = tree_node;
    LEFT_NODE_  =  new_left_ptr;
    RIGHT_NODE_ =  new_right_ptr;
    
    return tree_node;
}


#define print_all_                                                                                                                                  \
        do {                                                                                                                                        \
            fprintf (output_file, "\tnode[color = \"blue\",fontsize=14];\n");                                                                       \
            fprintf (output_file, "\tINFO%p [shape = record, label = \" <left> LEFT :\\n%p | <name> ", (void*)node_ptr, (void*)node_ptr->left_ptr); \
            for (int i = 0; i < node_ptr->size_value; i++)                                                                                          \
            {                                                                                                                                       \
                if ((*(node_ptr->value.data) == '{') || (*(node_ptr->value.data) == '}') ||                                                         \
                     (*(node_ptr->value.data) == '>') || (*(node_ptr->value.data) == '<'))                                                          \
                {                                                                                                                                   \
                    fprintf (output_file, "\\");                                                                                                    \
                }                                                                                                                                   \
                fprintf (output_file, "%c", *(node_ptr->value.data + i));                                                                           \
            }                                                                                                                                       \
            fprintf (output_file, " | <right> RIGHT :\\n%p\"];\n", (void*)node_ptr->right_ptr);                                                     \
        }while(0)



int tree_dump_visual (Tree* tree_node)
{
    _TREE_VERIFICATE_(tree_node);

    FILE* output_file = fopen ("../../output/tree_dump_vis.dot", "w");

    //FILE* output_file = fopen ("tree_dump.dot", "w");

    CHECK_ERROR_(output_file, "NOT CREATED", -1);

    fprintf (output_file, "\tdigraph dump_graph{\n"
                          "\trankdir=HR;\n");

    Tree* node_ptr = tree_node;
    print_all_;

    write_tree_element_vis (tree_node, output_file);

    fprintf(output_file, "\t}");


    if (ferror(output_file))
    {
        printf ("ERROR in function : %s \n"
                "writing file falled\n", __func__);
        return -1;
    }
    
    fclose (output_file);


    system ("\"C:/Program Files/Graphviz/bin/dot.exe\" -Tpng ../../output/tree_dump_vis.dot -o ../../output/tree_graph.png");

    return 0;
}

#define write_define_vis_(arg, side)                                                                                                                    \
        do{                                                                                                                                             \
            if (tree_node->arg != NULL)                                                                                                                 \
            {                                                                                                                                           \
                Tree* node_ptr = tree_node->arg;                                                                                                        \
                print_all_;\
                fprintf (output_file, "\tINFO%p: <%s> -> INFO%p: <name>\n", (void*)tree_node, #side, (void*)node_ptr);                                  \
                /*fprintf (output_file, "\tINFO%p: <name> -> INFO%p: <%s>\n", (void*)node_ptr, (void*)node_ptr->pre_ptr, #side);*/                          \
                write_tree_element_vis (node_ptr, output_file);                                                                                         \
            }                                                                                                                                           \
        }while(0)



static int write_tree_element_vis (Tree* tree_node, FILE* output_file)
{
    write_define_vis_(left_ptr, left);
    write_define_vis_(right_ptr, right);
    return 0;
}

#undef print_all_

#undef write_define_vis_

#undef _write_tree_elem_

#undef CHECK_ERROR_

#undef _TREE_VERIFICATE_

#undef LEFT_NODE_

#undef RIGHT_NODE_

#undef $