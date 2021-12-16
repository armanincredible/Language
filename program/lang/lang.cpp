#include "lang.h"

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

//const char* CALC_COND_STR = "CALC_COND";

static int is_op (const char arg);

static void log_make (const char* function, node_st* nodes_st);

static int nodes_array_resize (node_st* node_st);

static void syntax_error (int type, const int line);

static Tree* get_num (node_st* nodes_st);

static Tree* get_exp (node_st* nodes_st);

static Tree* get_t (node_st* nodes_st);

static Tree* get_p (node_st* nodes_st);

static Tree* get_var (node_st* nodes_st);

static Tree* get_op (node_st* nodes_st);

static Tree* get_if (node_st* nodes_st);

static Tree* get_ret (node_st* nodes_st);

static Tree* get_while (node_st* nodes_st);

static Tree* get_as (node_st* nodes_st);

static Tree* get_dis (node_st* nodes_st);

static Tree* get_func (node_st* nodes_st);

static int make_code_recurse (Tree* tree_node, FILE* file_output);

//static int how_many_arg (Tree* tree_node, int* num);

static int make_asm (Asm_values* asm_st, Tree* tree_node, Variables* var_arr);
static int make_asm_recurse (Asm_values* asm_st, Tree* tree_node, Variables* var_arr);

static void print_val (FILE* output_file, Tree* tree_node);
static int take_var_ptr (Asm_values* asm_st, Tree* tree_node, Variables* var_arr);

int operation_array_ctor (char** array);
int operation_array_dtor (char** array);

static int var_arr_ctor (Variables** var_arr);
static int var_arr_dtor (Variables** var_arr);

static int check_stat (Asm_values* asm_st, Tree* tree_node, Variables* var_arr);

#define SIZE_VAL_                 ( nodes_st->array[nodes_st->cur_node]      ->size_value)
#define NODE_VAL_(arg)            ( nodes_st->array[nodes_st->cur_node + arg]->value.data)
#define ADD_CUR_NODE_(arg)        ( nodes_st->cur_node = nodes_st->cur_node + arg)

#define STR_EQ_(arg1, arg2, size) ( strncmp (arg1, arg2, size) == 0 )

#define _LOG_                     log_make (__func__, nodes_st);

char** OPERTAIONS_ARR = (char**) calloc (VALUE_CONST_NAMES_NODE, sizeof (char*));

/*int make_str_from_file (char** str, FILE* file_input)
{

}
int dtor_str_from_file ()*/

#define MAKE_STR_(name)                                                 \
        do                                                              \
        {                                                               \
            str = (char*) calloc (sizeof (#name), sizeof(char));        \
            CHECK_ERROR_(str, "NULL ADDRESS", NULL);                    \
            strcpy (str, #name);                                        \
                                                                        \
            OPERTAIONS_ARR[name] = str;                                 \
            str = NULL;                                                 \
        }while (0)

int operation_array_ctor (char** array)
{
    CHECK_ERROR_(array, "NULL ADDRESS", -1);

    char* str = NULL;

    MAKE_STR_(CALC_COND);

    MAKE_STR_(CALL);

    MAKE_STR_(DEFINE);

    MAKE_STR_(STATEMENT);

    MAKE_STR_(DECISION);

    MAKE_STR_(PARAMETER);

    MAKE_STR_(ARRAY);

    MAKE_STR_(DORA);

    MAKE_STR_(DURA);

    //printf ("%s", OPERTAIONS_ARR[DURA]);

    MAKE_STR_(VKRASHILAS);

    MAKE_STR_(VTURILAS);

    MAKE_STR_(VLYAPALAS);

    MAKE_STR_(POSHLU);

    MAKE_STR_(FUNCTION);

    return 0;
}

#undef MAKE_STR_

int operation_array_dtor (char** array)
{
    for (int i = 0; i < FUNCTION; i++)
    {
        memset (array[i], 0, strlen (array[i]));
        free (array[i]);
    }
    return 0;
}

FILE* log_output = fopen ("../../output/log.txt", "w");

static void log_make (const char* function, node_st* nodes_st)
{
    fprintf (log_output, "%s : cur_node = %d, value = ", function, nodes_st->cur_node);
    for (int i = 0; i < nodes_st->array[nodes_st->cur_node]->size_value; i++)
    {
        fprintf (log_output, "%c", *(nodes_st->array[nodes_st->cur_node]->value.data + i));
    }
    fprintf (log_output, "\n");
}

int make_tree_asm (node_st* nodes_st, Tree* tree_node)
{
    CHECK_ERROR_(nodes_st, "NUL ADDRESS", -1);
    CHECK_ERROR_(tree_node, "NUL ADDRESS", -1);

    /*Tree* tree_node = get_general (nodes_st);
    tree_dump_visual (tree_node);*/

    Asm_values asm_st = {};
    asm_st.output_file = fopen ("../processor/library/txt_library/asm.txt", "w");
    CHECK_ERROR_(asm_st.output_file, "NOT OPENED", -1);

    Variables* var_arr = {};
    var_arr_ctor (&var_arr);

    make_asm (&asm_st, tree_node, var_arr);

    for (int i = 0; i < nodes_st->size; i++)
    {
        int symb = *(nodes_st->array[i]->value.data);
        if  ((nodes_st->array != NULL)  && 
            ((symb == '}')              || 
            (symb == '{')               || 
            (symb == '(')               ||
            (symb == '[')               ||
            (symb == ']')               ||
            (symb == ')')))
        {
            tree_dtor (&nodes_st->array[i]);
        }
    }

    /*tree_dtor (&tree_node);*/
    var_arr_dtor (&var_arr);

    if (ferror (asm_st.output_file) != 0)
    {
        printf ("ERROR WITH file_asmT\n");
        return -1;
    }

    fclose (asm_st.output_file);

    return 0;
}

int var_arr_ctor (Variables** var_arr)
{
    CHECK_ERROR_(var_arr, "NULL ADDRESS", -1);

    *var_arr = (Variables*) calloc (AMOUNT_VAR, sizeof (Variables));
    CHECK_ERROR_(*var_arr, "NULL ADDRESS", -1);

    memset (*var_arr, NULL, AMOUNT_VAR * sizeof (Variables));

    return 0;
}

int var_arr_dtor (Variables** var_arr)
{
    CHECK_ERROR_(var_arr, "NULL ADDRESS", -1);

    memset (*var_arr, NULL, AMOUNT_VAR * sizeof (Variables));

    free (*var_arr);

    return 0;
}


Tree* get_general (node_st* nodes_st)
{
    CHECK_ERROR_(nodes_st, "NUL ADDRESS", NULL);

    Tree* val = get_op (nodes_st);

    if (*NODE_VAL_(0) != '$')
    {
        syntax_error (NO_END_DOLLAR, __LINE__);
    }

    return val;
}

#define prove_end                                       \
        do                                              \
        {                                               \
            if (*NODE_VAL_(0) != ';')                   \
            {                                           \
                if (*(NODE_VAL_(0)) != ';')             \
                {                                       \
                    syntax_error (NO_END_OP, __LINE__); \
                }                                       \
            }                                           \
            ADD_CUR_NODE_(1);                           \
        }while(0)                                   

Tree* get_op (node_st* nodes_st)
{
    _LOG_

    //Tree* val = NULL;
    Tree* val = New_Node (OPERTAIONS_ARR [STATEMENT], NULL, NULL);
    val->size_value = strlen ("STATEMENT");

    Tree* val_op = val;

    while ((*NODE_VAL_(0) != '$') && (*NODE_VAL_(1) != '$') && (*NODE_VAL_(0) != '}'))
    {
        val_op->left_ptr = New_Node (OPERTAIONS_ARR [STATEMENT], NULL, NULL);
        val_op->left_ptr->size_value = strlen ("STATEMENT");

        if (STR_EQ_(NODE_VAL_(0), OPERTAIONS_ARR [VTURILAS], sizeof ("VTURILAS") - 1))
        {
            val_op->right_ptr = get_if (nodes_st);
        }
        else if (STR_EQ_(NODE_VAL_(0), OPERTAIONS_ARR [POSHLU], sizeof ("POSHLU") - 1))
        {
            val_op->right_ptr = get_ret (nodes_st);
            prove_end;
        }
        else if (STR_EQ_(NODE_VAL_(0), OPERTAIONS_ARR [VLYAPALAS], sizeof ("VLYAPALAS") - 1))
        {
            val_op->right_ptr = get_while (nodes_st);
        }
        else if ((*(NODE_VAL_(1)) == '=') && (nodes_st->array[nodes_st->cur_node + 1]->size_value == 1))
        {
            val_op->right_ptr = get_as (nodes_st);
            prove_end;
        }
        else
        {
            val_op->right_ptr = get_exp (nodes_st);
            prove_end;
        }

        val_op = val_op->left_ptr;
    }

    return val;
}

#undef prove_end

Tree* get_if (node_st* nodes_st)
{
    _LOG_
    
    int save_node_ptr = nodes_st->cur_node;
    Tree* val = nodes_st->array[save_node_ptr];

    val->left_ptr = New_Node (OPERTAIONS_ARR [DECISION], NULL, NULL);
    val->left_ptr->size_value = strlen ("DECISION");
    
    ADD_CUR_NODE_(1);

    if (*NODE_VAL_(0) != '(')
    {
        syntax_error (NO_PARENTHESIS, __LINE__);
    }
    ADD_CUR_NODE_(1);

    Tree* val_cond = get_dis (nodes_st);

    if ((*NODE_VAL_(0) != ')') || (*NODE_VAL_(1) != '{'))
    {
        syntax_error (NO_PARENTHESIS, __LINE__);
    }

    ADD_CUR_NODE_(2);

    Tree* val_true = get_op (nodes_st);

    if (*NODE_VAL_(0) != '}')
    {
        syntax_error (NO_PARENTHESIS, __LINE__);
    }

    ADD_CUR_NODE_(1);

    val->left_ptr->right_ptr = val_true;
    val->right_ptr = val_cond;
    
    if (STR_EQ_(NODE_VAL_(0), OPERTAIONS_ARR [VKRASHILAS], sizeof ("VKRAHILAS") - 1))
    {
        ADD_CUR_NODE_(1);

        if (*NODE_VAL_(0) != '{')
        {
            syntax_error (NO_PARENTHESIS, __LINE__);
        }

        ADD_CUR_NODE_(1);

        Tree* else_val = get_op (nodes_st);

        if (*NODE_VAL_(0) != '}')
        {
            syntax_error (NO_PARENTHESIS, __LINE__);
        }
        
        ADD_CUR_NODE_(1);

        val->left_ptr->left_ptr = else_val;
    }

    return val;
}

Tree* get_ret (node_st* nodes_st)
{
    int save_ptr = nodes_st->cur_node;
    Tree* val = nodes_st->array[save_ptr];

    ADD_CUR_NODE_(1);

    val->right_ptr = get_exp (nodes_st);

    return val;
}

Tree* get_as (node_st* nodes_st)
{
    _LOG_

    Tree* val_left = get_var (nodes_st);

    int save_node_ptr = nodes_st->cur_node;

    ADD_CUR_NODE_(1);

    Tree* val_right = get_exp (nodes_st);

    nodes_st->array[save_node_ptr]->left_ptr = val_left;
    nodes_st->array[save_node_ptr]->right_ptr = val_right;

    return nodes_st->array[save_node_ptr];
}

Tree* get_while (node_st* nodes_st)
{
    _LOG_
    
    int save_node_ptr = nodes_st->cur_node;
    Tree* val = nodes_st->array[save_node_ptr];

    ADD_CUR_NODE_(1);

    if (*NODE_VAL_(0) != '(')
    {
        syntax_error (NO_PARENTHESIS, __LINE__);
    }

    ADD_CUR_NODE_(1);

    Tree* cond_node = get_dis (nodes_st);

    if ((*NODE_VAL_(0) != ')') || (*NODE_VAL_(1) != '{'))
    {
        syntax_error (NO_PARENTHESIS, __LINE__);
    }

    ADD_CUR_NODE_(2);

    Tree* arg_node = get_op (nodes_st);

    if (*NODE_VAL_(0) != '}')
    {
        $
        syntax_error (NO_PARENTHESIS, __LINE__);
    }

    ADD_CUR_NODE_(1);

    val->left_ptr = arg_node;
    val->right_ptr = cond_node;

    return val;
}

Tree* get_dis (node_st* nodes_st)
{
    _LOG_

    Tree* val_left = get_exp (nodes_st);

    if ((*(NODE_VAL_(0)) == '=') || 
        (*(NODE_VAL_(0)) == '>') || 
        (*(NODE_VAL_(0)) == '<') || 
        (*(NODE_VAL_(0)) == '!'))
    {
        int save_node_ptr = nodes_st->cur_node;

        ADD_CUR_NODE_(1);

        Tree* val_right = get_exp (nodes_st);

        nodes_st->array[save_node_ptr]->left_ptr = val_left;
        nodes_st->array[save_node_ptr]->right_ptr = val_right;

        return nodes_st->array[save_node_ptr];
    }
    else
    {
        return val_left;
    }
}

Tree* get_num (node_st* nodes_st)
{
    _LOG_

    Tree* val = nodes_st->array[nodes_st->cur_node];
    ADD_CUR_NODE_(1);

    return val;
}

Tree* get_var (node_st* nodes_st)
{
    _LOG_

    Tree* val = nodes_st->array[nodes_st->cur_node];
    ADD_CUR_NODE_(1);

    if (*NODE_VAL_(0) == '[')
    {
        ADD_CUR_NODE_(1);

        Tree* val_2 = get_exp (nodes_st);
        
        Tree* val_array = New_Node (OPERTAIONS_ARR [ARRAY], NULL, NULL);
        val_array->size_value = strlen ("ARRAY");

        val_array->left_ptr = val;
        val_array->right_ptr = val_2;

        if (*NODE_VAL_(0) != ']')
        {
            syntax_error (NO_END_ARRAY_PAR, __LINE__);
        }
        ADD_CUR_NODE_(1);

        return val_array;
    }

    return val;
}

Tree* get_func (node_st* nodes_st) //////////////TODO
{
    _LOG_


    Tree* val_call = New_Node (OPERTAIONS_ARR [CALL], NULL, NULL);
    val_call->size_value = strlen ("CALL");
    
    val_call->right_ptr = New_Node (OPERTAIONS_ARR [FUNCTION], NULL, NULL);
    val_call->right_ptr->size_value = strlen ("FUNCTION");

    val_call->right_ptr->left_ptr = get_var (nodes_st);
    
    val_call->right_ptr->right_ptr = New_Node (OPERTAIONS_ARR [PARAMETER], NULL, NULL);
    val_call->right_ptr->right_ptr->size_value = strlen ("PARAMETER");

    if (*NODE_VAL_(0) == '(')
    {
        ADD_CUR_NODE_(1);
        val_call->right_ptr->right_ptr->right_ptr = get_exp (nodes_st);
        
        _LOG_

        Tree* param_node = val_call->right_ptr->right_ptr;

        while (*NODE_VAL_(0) == ',')
        {
            ADD_CUR_NODE_(1);

            param_node->left_ptr = New_Node (OPERTAIONS_ARR [PARAMETER], NULL, NULL);
            param_node->left_ptr->size_value = strlen ("PARAMETER");

            param_node->left_ptr->right_ptr = get_exp (nodes_st);

            param_node = param_node->left_ptr;
        }
    }
    else
    {
        syntax_error (NO_PARENTHESIS, __LINE__);
    }

    if (*NODE_VAL_(0) != ')')
    {
        syntax_error (NO_PARENTHESIS, __LINE__);
    }
    ADD_CUR_NODE_(1);

    if (*NODE_VAL_(0) == '{')
    {
        ADD_CUR_NODE_(1);

        Tree* val_def = New_Node (OPERTAIONS_ARR [DEFINE], NULL, NULL);
        val_def->size_value = strlen ("DEFINE");

        Tree* val_stat = get_op (nodes_st);

        ADD_CUR_NODE_(1);

        val_def->left_ptr  = val_call->right_ptr;
        val_def->right_ptr = val_stat;

        val_call->right_ptr = NULL;
        val_call->left_ptr = NULL;
        tree_dtor(&val_call);

        return val_def;
    }

    _LOG_

    return val_call;
}

Tree* get_exp (node_st* nodes_st)
{
    _LOG_

    Tree* val = get_t (nodes_st);

    while ((*NODE_VAL_(0) == '+') || (*NODE_VAL_(0) == '-'))
    {
        int save_node_ptr = nodes_st->cur_node;

        ADD_CUR_NODE_(1);
        Tree* sec_val = get_t (nodes_st);

        nodes_st->array[save_node_ptr]->left_ptr = val;
        nodes_st->array[save_node_ptr]->right_ptr = sec_val;

        val = nodes_st->array[save_node_ptr];
    }

    return val;
}

Tree* get_t (node_st* nodes_st)
{
    _LOG_
    Tree* val = get_p (nodes_st);

    while ((*NODE_VAL_(0) == '*') || (*NODE_VAL_(0) == '/') || (*NODE_VAL_(0) == '^'))
    {
        int save_node_ptr = nodes_st->cur_node;

        ADD_CUR_NODE_(1);
        Tree* sec_val = get_p (nodes_st);

        nodes_st->array[save_node_ptr]->left_ptr = val;
        nodes_st->array[save_node_ptr]->right_ptr = sec_val;

        val = nodes_st->array[save_node_ptr];
    }

    return val;
}

Tree* get_p (node_st* nodes_st)
{
    _LOG_

    Tree* val = NULL;

    if (*NODE_VAL_(0) == '(')
    {
        ADD_CUR_NODE_(1);
        val = get_exp (nodes_st);

        if (*NODE_VAL_(0) != ')')
        {
            printf ("\n%c\n", *NODE_VAL_(0));
            syntax_error (NO_PARENTHESIS, __LINE__); ////////////
        }

        ADD_CUR_NODE_(1);
    }
    else if (isdigit (*NODE_VAL_(0)))
    {
        val = get_num (nodes_st);
    }
    else if (*NODE_VAL_(1) == '(')
    {
        val = get_func (nodes_st);
    }
    else
    {
        val = get_var (nodes_st);
    }

    return val;
}

#undef SIZE_VAL_ 
#undef NODE_VAL_ 
#undef ADD_CUR_NODE_

#undef _LOG_


static void syntax_error (int type, const int line)
{
    switch (type)
    {
        case NO_PARENTHESIS :
        {
            printf ("NO_PARENTHESIS ERROR, %d\n", line);
            assert (0);
            break;
        }
        case NO_END_DOLLAR :
        {
            printf ("NO_END_DOLLAR ERROR %d\n", line);
            assert (0);
            break;
        }
        case NO_NUMBER :
        {
            printf ("NO_NUMBER ERROR %d\n", line);
            assert (0);
            break;
        }
        case NO_END_OP :
        {
            printf ("NO_END_OP ERROR %d\n", line);
            assert (0);
            break;
        }
        case NO_END_ARRAY_PAR :
        {
            printf ("NO_END_ARRAY_PARENTHESIS ERROR %d\n", line);
            assert (0);
            break;
        }
        default:
        {
            break;
        }
    }
}

int tree_nodes_ctr (node_st* nodes_st)
{
    nodes_st->array = (Tree**) calloc (BASIC_SIZE_NODES_ARRAY, sizeof (Tree*));
    CHECK_ERROR_(nodes_st->array, "NULL ADRESS", -1);

    nodes_st->size = 0;
    nodes_st->cur_node = 0;
    nodes_st->capacity = BASIC_SIZE_NODES_ARRAY;

    return 0;
}

static int nodes_array_resize (node_st* node_st)
{
    CHECK_ERROR_(node_st, "NULL ADRESS", -1);
    
    node_st->capacity = node_st->capacity * 2;

    node_st->array = (Tree**) realloc (node_st->array, sizeof(Tree) * node_st->capacity);
    CHECK_ERROR_(node_st->array, "HAVE NULL ADDRESS", -1);

    return 0;
}

int tree_nodes_dtor (node_st* nodes_st)
{
    CHECK_ERROR_(nodes_st, "NULL ADDRESS", -1);
    if (nodes_st->capacity == 0)
    {
        printf ("size error\n");
        return -1;
    }

    memset (nodes_st->array, 0, nodes_st->capacity * sizeof (char*));
    free (nodes_st->array);

    nodes_st->cur_node = 0;
    nodes_st->size = 0;
    nodes_st->capacity = 0;

    return 0;
}

#define RENAME_(name, new_name) \
        do\
        {\
            if (STR_EQ_(str + cur_symb, OPERTAIONS_ARR[name], strlen (OPERTAIONS_ARR[name])))\
            {\
                cur_symb += strlen (OPERTAIONS_ARR[name]) - 1;\
                symb = new_name;\
            }\
        }while (0)

#define symb *(str + cur_symb)

int make_nodes_arr (node_st* nodes_st, char* str)
{
    operation_array_ctor (OPERTAIONS_ARR);

    CHECK_ERROR_(nodes_st, "NULL ADDRESS", -1);
    CHECK_ERROR_(str, "NULL ADDRESS", -1);
    
    int cur_symb = 0;

    while (1)
    {
        if (nodes_st->size == nodes_st->capacity - 1)
        {
            nodes_array_resize(nodes_st);
        }

        while ((isspace (symb)) || (symb == '\n'))
        {
            cur_symb++;
        }

        RENAME_(DURA, '}');
        RENAME_(DORA, '{');

        //printf ("%s\n", str);

        if (isalpha(symb))
        {
            int save_ptr = cur_symb;
            //int length = 1;
            cur_symb++;
            while ((isalpha (symb)) || (isdigit (symb)))
            {
                //length++;
                cur_symb++;
            }

            Tree* tree_node = New_Node (str + save_ptr, NULL, NULL);

            tree_node->size_value = cur_symb - save_ptr;

            nodes_st->array[nodes_st->size] = tree_node;
            nodes_st->size = nodes_st->size + 1;

            continue;
        }
        
        if (isdigit (symb))
        {
            int save_ptr = cur_symb;
            cur_symb++;
            while (isdigit (symb))
            {
                cur_symb++;
            }

            Tree* tree_node = New_Node (str + save_ptr, NULL, NULL);

            tree_node->size_value = cur_symb - save_ptr;

            nodes_st->array[nodes_st->size] = tree_node;
            nodes_st->size = nodes_st->size + 1;

            continue;
        }

        if (is_op (symb))
        {
            Tree* tree_node = New_Node (str + cur_symb, NULL, NULL);

            tree_node->size_value = 1;

            nodes_st->array[nodes_st->size] = tree_node;
            nodes_st->size = nodes_st->size + 1;

            cur_symb++;

            if (symb == '=')
            {
                cur_symb++;
                tree_node->size_value = tree_node->size_value + 1;
            }

            continue;
        }

        while ((isspace (symb)) || (symb == '\n'))
        {
            cur_symb++;
        }

        if (symb != '$')
        {
            syntax_error (NO_END_DOLLAR, __LINE__);
            return -1;
        }
        else
        {
            if (nodes_st->size == nodes_st->capacity - 1)
            {
                nodes_array_resize(nodes_st);
            }
            nodes_st->array[nodes_st->size] = New_Node (str + cur_symb, NULL, NULL);
            nodes_st->size = nodes_st->size + 1;

            return 0;
        }
    }

    return 0;
}

#undef RENAME_

static int is_op (const char arg)
{
    if ((arg == '+') || (arg == '-') || (arg == '*') ||
        (arg == '/') || (arg == '^') || (arg == ';') ||
        (arg == '=') || (arg == '>') || (arg == '<') ||
        (arg == '(') || (arg == ')') || (arg == '{') ||
        (arg == '}') || (arg == ',') || (arg == '!') ||
        (arg == '[') || (arg == ']'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int make_asm (Asm_values* asm_st, Tree* tree_node, Variables* var_arr)
{
    CHECK_ERROR_(asm_st, "NULL ADDRESS", -1);
    CHECK_ERROR_(asm_st->output_file, "NULL ADDRESS", -1);
    CHECK_ERROR_(tree_node, "NULL ADDRESS", -1);
    CHECK_ERROR_(var_arr, "NULL ADDRESS", -1);

    FILE* file_output = asm_st->output_file;
    
    fprintf (file_output,   "PUSH 0\n"
                            "POP %cX\n", V_COND);

    make_asm_recurse (asm_st, tree_node, var_arr);

    fprintf (file_output, "HLT");

    fprintf (file_output,  "\n$ %s :\n" 
                            "PUSH 1\n"
                            "POP %cX\n"
                            "RET", OPERTAIONS_ARR[CALC_COND], V_COND);


    return 0;
}

#define ARG_NODE_           (tree_node->value.data)
#define ARG_LEFT_NODE_      (tree_node->left_ptr->value.data)
#define ARG_RIGHT_NODE_     (tree_node->right_ptr->value.data)

#define SIZE_ARG_NODE_      (tree_node->size_value)
#define SIZE_ARG_LEFT_NODE  (tree_node->left_ptr->size_value)
#define SIZE_ARG_RIGHT_NODE (tree_node->right_ptr->size_value)

#define LEFT_NODE_  (tree_node->left_ptr)
#define RIGHT_NODE_ (tree_node->right_ptr)

#define TAKE_VAR_(node) take_var_ptr (asm_st, node, var_arr)

#define RECURSE_(node)                                              \
            do{                                                     \
                if (node != NULL)                                   \
                {                                                   \
                    make_asm_recurse (asm_st, node, var_arr);       \
                }                                                   \
            }while(0)


#define PRINT_BASIC_OP_(str, arg)                   \
        do{                                         \
            if (*ARG_NODE_ == arg)                  \
            {                                       \
                fprintf (file_output, "%s\n", str); \
                return 0;                           \
            }                                       \
        }while(0)

#define STR_EQ_NODE_(arg) STR_EQ_(ARG_NODE_, OPERTAIONS_ARR [arg], SIZE_ARG_NODE_)

bool need_push = 1;

static int make_asm_recurse (Asm_values* asm_st, Tree* tree_node, Variables* var_arr)
{
    FILE* file_output = asm_st->output_file;

    if (STR_EQ_NODE_(PARAMETER))
    {
        if (need_push == 1)
        {
            RECURSE_(RIGHT_NODE_);

            RECURSE_(LEFT_NODE_);
        }
        else
        {
            RECURSE_(LEFT_NODE_);

            int var_ptr = TAKE_VAR_(RIGHT_NODE_);
            fprintf (file_output, "POP [%cX + %d]\n", FREE_MEM_REG, var_ptr);

            //RECURSE_(LEFT_NODE_);
        }

        return 0;
    }

    if (STR_EQ_NODE_(FUNCTION))
    {
        print_val (file_output, LEFT_NODE_);

        if (need_push == 0)
        {
            fprintf (file_output, " :\n");
        }

        RECURSE_(RIGHT_NODE_);


        return 0;
    }

    if (STR_EQ_NODE_(DEFINE))
    {
        need_push = 0;
/*
        asm_st->cur_field = asm_st->cur_am_var;
        int save_field = asm_st->cur_field;
*/
        memset (var_arr, 0, (asm_st->cur_am_var) * sizeof (char*));
        asm_st->cur_am_var = 0;
        asm_st->cur_field = 0;

        fprintf (file_output, "HLT\n");

        fprintf (file_output, "$ ");

        RECURSE_(LEFT_NODE_);

        RECURSE_(RIGHT_NODE_);

        need_push = 1;
/*
        memset (var_arr + save_field, 0, (asm_st->cur_am_var - save_field + 1) * sizeof (char*));/////////
        asm_st->cur_field = save_field;
*/
        return 0;
    }

    if (STR_EQ_NODE_(CALL))
    {
        char* name_tree = RIGHT_NODE_->left_ptr->value.data;

        if (STR_EQ_ (name_tree, "print", sizeof ("print") - 1))
        {
            need_push = 1;
            RECURSE_(RIGHT_NODE_->right_ptr);
            fprintf (file_output, "OUT\n");
            need_push = 0;
            return 0;
        }

        if (STR_EQ_ (name_tree, "scan", sizeof ("scan") - 1))
        {
            fprintf (file_output, "IN\n");
            need_push = 0;
            RECURSE_(RIGHT_NODE_->right_ptr);
            need_push = 1;
            return 0;
        }

        if (STR_EQ_ (name_tree, "sqrt", sizeof ("sqrt") - 1))
        {
            need_push = 1;
            RECURSE_(RIGHT_NODE_->right_ptr);
            fprintf (file_output, "SQRT\n");
            need_push = 0;
            return 0;
        }

        need_push = 1;

        int delta = asm_st->cur_am_var;

        RECURSE_(RIGHT_NODE_->right_ptr);

        #define PRINT_STR_(str, arg) fprintf (file_output, str, arg);

        PRINT_STR_("PUSH %cX\n", FREE_MEM_REG);
        PRINT_STR_("PUSH %d\n", delta );
        fprintf (file_output, "ADD\n");
        PRINT_STR_("POP %cX\n", FREE_MEM_REG);

        fprintf (file_output, "CALL ");

        print_val(file_output, RIGHT_NODE_->left_ptr);
        fprintf (file_output, "\n");

        PRINT_STR_("PUSH %d\n", delta );
        PRINT_STR_("PUSH %cX\n", FREE_MEM_REG);
        fprintf (file_output, "SUB\n");
        PRINT_STR_("POP %cX\n", FREE_MEM_REG);

        PRINT_STR_("PUSH %cX\n", RET_REG);

        #undef PRINT_STR_

        need_push = 0;

        return 0;
    }

    if (STR_EQ_NODE_(DECISION))
    {
        if (LEFT_NODE_ != NULL)
        {
            RECURSE_ (LEFT_NODE_);
            asm_st->cur_am_if = asm_st->cur_am_if + 1;
        }
        //RECURSE_(LEFT_NODE_);/////else

        fprintf (file_output, "JMP skip_if%d\n", asm_st->cur_am_if);

        fprintf (file_output, "$ if%d :\n", asm_st->cur_am_if);

        RECURSE_(RIGHT_NODE_);

        fprintf (file_output, "$ skip_if%d :\n", asm_st->cur_am_if);

        return 0;
        
    }

    if (STR_EQ_NODE_(POSHLU))
    {
        RECURSE_(RIGHT_NODE_);

        fprintf (file_output, "POP %cX\n", RET_REG);
        fprintf (file_output, "RET\n");

        return 0;
    }

    if (STR_EQ_NODE_(VTURILAS))
    {
        asm_st->cur_am_if = asm_st->cur_am_if + 1;

        check_stat (asm_st, RIGHT_NODE_, var_arr);

        fprintf (file_output, "PUSH 0\n"
                                        "PUSH %cX\n", V_COND);

        fprintf (file_output, "JNE if%d\n", asm_st->cur_am_if);

        RECURSE_(LEFT_NODE_);

        fprintf (file_output, "PUSH 0\n"
                              "POP %cX\n", V_COND); /////// reg condition = 0 after all operations

        return 0;
    }

    if (STR_EQ_NODE_(VLYAPALAS))//////////////////////////////
    {
        asm_st->cur_am_if = asm_st->cur_am_if + 1;

        int am_if = asm_st->cur_am_if;

        fprintf (file_output, "$ wh_cycle%d :\n", am_if);

        check_stat (asm_st, RIGHT_NODE_, var_arr);
        fprintf (file_output, "PUSH 0\n"
                                      "PUSH %cX\n", V_COND);

        fprintf (file_output, "JE skip_wh%d\n", am_if);
        
        RECURSE_(LEFT_NODE_);

        fprintf (file_output,   "PUSH 0\n"
                                "POP %cX\n", V_COND);

        fprintf (file_output, "JMP wh_cycle%d\n", am_if);
        fprintf (file_output, "$ skip_wh%d :\n", am_if);

        return 0;
    }


    if ((*ARG_NODE_ == '=') && (SIZE_ARG_NODE_ == 1))
    {
        int var_ptr = TAKE_VAR_(LEFT_NODE_);

        RECURSE_(RIGHT_NODE_);
        
        fprintf (file_output, "POP [%cX + %d]\n", FREE_MEM_REG ,var_ptr);

        return 0;
    }

    RECURSE_(RIGHT_NODE_);
    RECURSE_(LEFT_NODE_);

    if (STR_EQ_NODE_(STATEMENT))
    {
        return 0;
    }

    PRINT_BASIC_OP_("ADD", '+');
    PRINT_BASIC_OP_("MUL", '*');
    PRINT_BASIC_OP_("DIV", '/');
    PRINT_BASIC_OP_("SUB", '-');
    PRINT_BASIC_OP_("POWER", '^');

    if (isdigit (*ARG_NODE_))
    {
        fprintf (file_output, "PUSH ");
        print_val (file_output, tree_node);
        fprintf (file_output, "\n");
        return 0;
    }

    if (isalpha (*ARG_NODE_))
    {
        if ((LEFT_NODE_ == NULL) && (RIGHT_NODE_ == NULL))
        { 
            int var_ptr_new = TAKE_VAR_(tree_node);
            fprintf (file_output, "PUSH [%cX + %d]\n", FREE_MEM_REG, var_ptr_new );
        }
        return 0;
    }

    return 0;
}

#define PRINT_IFS_(type)                                                                        \
        do                                                                                      \
        {                                                                                       \
            fprintf (asm_st->output_file, "%s jump_to_call%d\n", #type, cur_am_if);             \
            fprintf (asm_st->output_file, "JMP skip_call%d\n", cur_am_if);                      \
            fprintf (asm_st->output_file, "$ jump_to_call%d :\n"                                \
                                          "CALL %s\n", cur_am_if, OPERTAIONS_ARR[CALC_COND]);   \
            fprintf (asm_st->output_file, "$ skip_call%d :\n", cur_am_if);                      \
        }while(0)\

static int check_stat (Asm_values* asm_st, Tree* tree_node, Variables* var_arr)
{
    int cur_am_if = asm_st->cur_am_if;

    char arg = *ARG_NODE_;

    RECURSE_(RIGHT_NODE_);
    RECURSE_(LEFT_NODE_);

    if (SIZE_ARG_NODE_ == 2)
    {
        switch (arg)
        {
        case '!':
            PRINT_IFS_(JNE);
            return 0;
        case '=':
            PRINT_IFS_(JE);
            return 0;
        case '<':
            PRINT_IFS_(JBL);
            return 0;
        case '>':
            PRINT_IFS_(JAL);
            return 0;
        default:
            break;
        }
    }
    else
    {
        if (arg == '>')
        {
            PRINT_IFS_(JA);
            return 0;
        }

        if (arg == '<')
        {
            PRINT_IFS_(JB);
            return 0;
        }
    }

    fprintf (asm_st->output_file, "PUSH 0\n");

    RECURSE_(tree_node);
    PRINT_IFS_(JAL);

    return 0;
}

#undef PRINT_IFS_

static void print_val (FILE* output_file, Tree* tree_node)
{
    for (int i = 0; i < tree_node->size_value; i++)
    {
        fprintf (output_file, "%c", *(tree_node->value.data + i));
    }
}

static int take_var_ptr (Asm_values* asm_st, Tree* tree_node, Variables* var_arr)
{
    for (int i = 0; i < asm_st->cur_am_var; i++)
    {
        if (STR_EQ_(ARG_NODE_, var_arr[i].name, var_arr[i].size))
        {
            return var_arr[i].ptr - asm_st->cur_field;
        }
    }

    int am_var = asm_st->cur_am_var;

    var_arr[am_var].ptr = am_var;
    var_arr[am_var].name = ARG_NODE_;
    var_arr[am_var].size = SIZE_ARG_NODE_;

    asm_st->cur_am_var = asm_st->cur_am_var + 1;

    return am_var;
}

int make_code (Tree* tree_node)
{
    CHECK_ERROR_(tree_node, "NULL ADDRESS", -1);

    FILE* file_output = fopen ("../../output/lang_from_tree.txt", "w");
    CHECK_ERROR_(file_output, "FILE NOT OPENED", -1);

    make_code_recurse (tree_node, file_output);

    operation_array_dtor (OPERTAIONS_ARR);

    if (ferror (file_output) != 0)
    {
        printf ("ERROR WITH file_asmT\n");
        return -1;
    }
    fclose (file_output);

    return 0;
}

#undef RECURSE_
#undef PRINT_BASIC_OP_

#define RECURSE_(node)                                              \
            do{                                                     \
                if (node != NULL)                                   \
                {                                                   \
                    make_code_recurse (node, file_output);          \
                }                                                   \
            }while(0)

#define PRINT_BASIC_OP_(arg)                        \
        do{                                         \
            if (*ARG_NODE_ == arg)                  \
            {                                       \
                RECURSE_(LEFT_NODE_);                \
               /* fprintf (file_output, "%c", arg);*/ \
                print_val(file_output, tree_node);\
                RECURSE_(RIGHT_NODE_);              \
                return 0;                           \
            }                                       \
        }while(0)

#undef PRINT_
#define PRINT_(arg) fprintf (file_output, "%c", arg);

static int make_code_recurse (Tree* tree_node, FILE* file_output)
{
    if (STR_EQ_NODE_(STATEMENT))
    {
        RECURSE_(RIGHT_NODE_);
        if ((RIGHT_NODE_ != NULL) && 
            (!STR_EQ_(ARG_RIGHT_NODE_, OPERTAIONS_ARR [VTURILAS], SIZE_ARG_RIGHT_NODE)) && 
            (!STR_EQ_(ARG_RIGHT_NODE_, OPERTAIONS_ARR [VLYAPALAS], SIZE_ARG_RIGHT_NODE)))
        {
            PRINT_(';');
            //fprintf (file_output, "\n");
        }
        fprintf (file_output, "\n");
        RECURSE_(LEFT_NODE_);
        return 0;
    }

    if (STR_EQ_NODE_(CALL))
    {
        RECURSE_(RIGHT_NODE_);
        return 0;
    }

    if (STR_EQ_NODE_(DEFINE))
    {
        RECURSE_(LEFT_NODE_);
        fprintf (file_output, "\n");
        fprintf (file_output, "%s\n", OPERTAIONS_ARR [DORA]);
        RECURSE_(RIGHT_NODE_);
        fprintf (file_output, "%s\n", OPERTAIONS_ARR [DURA]);
        return 0;
    }

    if (STR_EQ_NODE_(FUNCTION))
    {
        print_val (file_output, LEFT_NODE_);
        PRINT_('(');
        RECURSE_(RIGHT_NODE_);
        PRINT_(')');
        //fprintf (file_output, "\n");
        return 0;
    }

    if (STR_EQ_NODE_(PARAMETER))
    {
        RECURSE_(RIGHT_NODE_);
        if (LEFT_NODE_!= NULL)
        {
            PRINT_(',');
            RECURSE_(LEFT_NODE_);
        }
        return 0;

    }

    if (STR_EQ_NODE_(VTURILAS))
    {
        print_val (file_output, tree_node);
        
        PRINT_('(');
        RECURSE_(RIGHT_NODE_);
        PRINT_(')');
        fprintf (file_output, "\n");

        RECURSE_(LEFT_NODE_);

        return 0;
    }

    if (STR_EQ_NODE_(DECISION))
    {
        fprintf (file_output, "%s\n", OPERTAIONS_ARR [DORA]);
        RECURSE_(RIGHT_NODE_);
        fprintf (file_output, "%s", OPERTAIONS_ARR [DURA]);

        if (LEFT_NODE_ != NULL)
        {
            fprintf (file_output, "\n");
            fprintf (file_output, "%s\n", OPERTAIONS_ARR [VKRASHILAS]);
            fprintf (file_output, "%s\n", OPERTAIONS_ARR [DORA]);
            RECURSE_(LEFT_NODE_);
            fprintf (file_output, "%s", OPERTAIONS_ARR [DURA]);
        }

        return 0;
    }

    if (STR_EQ_NODE_(VLYAPALAS))
    {
        print_val (file_output, tree_node);
        
        PRINT_('(');
        RECURSE_(RIGHT_NODE_);
        PRINT_(')');
        fprintf (file_output, "\n");

        fprintf (file_output, "%s\n", OPERTAIONS_ARR [DORA]);
        RECURSE_(LEFT_NODE_);
        fprintf (file_output, "%s", OPERTAIONS_ARR [DURA]);

        return 0;
    }

    if (STR_EQ_NODE_(POSHLU))
    {
        print_val (file_output, tree_node);

        fprintf (file_output, " ");
        
        RECURSE_(RIGHT_NODE_);

        return 0;
    }

    if ((isdigit (*ARG_NODE_)) || (isalpha (*ARG_NODE_)))
    {
        print_val (file_output, tree_node);

        return 0;
    }
    

    PRINT_BASIC_OP_('+');
    PRINT_BASIC_OP_('=');
    PRINT_BASIC_OP_('!');
    PRINT_BASIC_OP_('>');
    PRINT_BASIC_OP_('<');
    PRINT_BASIC_OP_('*');
    PRINT_BASIC_OP_('/');
    PRINT_BASIC_OP_('-');
    PRINT_BASIC_OP_('^');


    /*RECURSE_(RIGHT_NODE_);
    RECURSE_(LEFT_NODE_);*/

    return 0;
}

#undef STR_EQ_NODE_

#undef PRINT_

#undef RECURSE_

#undef PRINT_BASIC_OP_

#undef ARG_NODE_
#undef ARG_LEFT_NODE_
#undef ARG_RIGHT_NODE_

#undef SIZE_ARG_NODE_
#undef SIZE_ARG_LEFT_NODE
#undef SIZE_ARG_RIGHT_NODE

#undef LEFT_NODE_
#undef RIGHT_NODE_
#undef TAKE_VAR_

#undef symb

#undef STR_EQ_ 

#undef CHECK_ERROR_

#undef $