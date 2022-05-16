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

#define $ printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);


static int make_asm (Asm_values* asm_st, Tree* tree_node, Variables* var_arr);
static int make_asm_recurse (Asm_values* asm_st, Tree* tree_node, Variables* var_arr);

static void print_val (FILE* output_file, Tree* tree_node);
static int take_var_ptr (Asm_values* asm_st, Tree* tree_node, Variables* var_arr);

int operation_array_ctor (char** array);
int operation_array_dtor (char** array);

static int var_arr_ctor (Variables** var_arr);
static int var_arr_dtor (Variables** var_arr);

static int make_tree_recursion (Tree* tree_node, Onegin* line, int* ammount, const int amount_str);
static char *copy_in_buffer_from_file (int* amount_str);

static int check_stat (Asm_values* asm_st, Tree* tree_node, Variables* var_arr);


#define STR_EQ_(arg1, arg2, size) ( strcmp (arg1, arg2) == 0 )

#define _LOG_                     log_make (__func__, nodes_st);

char** OPERTAIONS_ARR = (char**) calloc (VALUE_CONST_NAMES_NODE, sizeof (char*));

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

int make_tree_asm (Tree* tree_node)
{
    //CHECK_ERROR_(nodes_st, "NUL ADDRESS", -1);
    CHECK_ERROR_(tree_node, "NUL ADDRESS", -1);

    /*Tree* tree_node = get_general (nodes_st);
    tree_dump_visual (tree_node);*/

    Asm_values asm_st = {};
    asm_st.size_arr = 0;

    asm_st.output_file = fopen ("../processor/library/txt_library/asm.txt", "w");
    CHECK_ERROR_(asm_st.output_file, "NOT OPENED", -1);

    Variables* var_arr = {};
    var_arr_ctor (&var_arr);

    operation_array_ctor (OPERTAIONS_ARR);

    make_asm (&asm_st, tree_node, var_arr);

    operation_array_dtor (OPERTAIONS_ARR);

    /*for (int i = 0; i < nodes_st->size; i++)
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
    }*/

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

    if (STR_EQ_NODE_(ARRAY))
    {
        RECURSE_(RIGHT_NODE_);

        int var_ptr = TAKE_VAR_(LEFT_NODE_);

        fprintf (file_output, "PUSH %cX\n", FREE_MEM_REG);
        fprintf (file_output, "ADD\n");
        fprintf (file_output, "POP %cX\n", ARG_SUM_REG);

        fprintf (file_output, "PUSH [%cX + %d]\n", ARG_SUM_REG , var_ptr);

        asm_st->size_arr = 0;

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
        int var_ptr = 0;

        if ((LEFT_NODE_!= NULL) && (STR_EQ_(ARG_LEFT_NODE_, OPERTAIONS_ARR [ARRAY], strlen (OPERTAIONS_ARR [ARRAY]))))
        {
            if (isdigit (*(tree_node->left_ptr->right_ptr->value.data)))
            {
                int val = 0;

                val = atoi (tree_node->left_ptr->right_ptr->value.data);


                asm_st->size_arr = val - 1;
            }

            var_ptr = TAKE_VAR_(LEFT_NODE_->left_ptr);
            RECURSE_(LEFT_NODE_->right_ptr);

            fprintf (file_output, "PUSH %cX\n", FREE_MEM_REG);
            fprintf (file_output, "ADD\n");
            fprintf (file_output, "POP %cX\n", ARG_SUM_REG);

            RECURSE_(RIGHT_NODE_);

            fprintf (file_output, "POP [%cX + %d]\n", ARG_SUM_REG ,var_ptr);
            asm_st->size_arr = 0;
            return 0;
        }

        var_ptr = TAKE_VAR_(LEFT_NODE_);

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
        if ((var_arr[i].size != 0) && (STR_EQ_(ARG_NODE_, var_arr[i].name, var_arr[i].size)))
        {
            return var_arr[i].ptr - asm_st->cur_field;
        }
    }

    //printf ("%c", *ARG_NODE_);

    int am_var = asm_st->cur_am_var;

    var_arr[am_var].ptr = am_var;
    var_arr[am_var].name = ARG_NODE_;
    var_arr[am_var].size = SIZE_ARG_NODE_;

    asm_st->cur_am_var = asm_st->cur_am_var + 1 + asm_st->size_arr;

    return am_var;
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

Onegin* make_tree_from_library (Tree* tree_node, char** buffer)
{
    CHECK_ERROR_(tree_node, "NULL ADRESS", NULL);

    printf ("%s\n", OPERTAIONS_ARR[CALC_COND]);

    int amount_str = 0; 

    *buffer = copy_in_buffer_from_file (&amount_str);

    Onegin* line = (Onegin*) calloc (amount_str, sizeof (Onegin));
    CHECK_ERROR_(line, "NULL ADRESS (Not Enough Memory)", NULL);

    make_array_adress (*buffer, amount_str, line);

    //printf ("%s", *buffer);


    tree_push (tree_node, IN, line[1].adress);
    tree_node->size_value = line[1].length;
    int ammount = 2;
    
    make_tree_recursion (tree_node, line, &ammount, amount_str);
    
    return line;
}

#define _pars_tree_def_(arg, side)                                              \
        do{                                                                     \
            if ((*(line[*ammount].adress) == '{') &&                            \
                (*(line[1 + *ammount].adress) != '}'))                          \
            {                                                                   \
                tree_push (tree_node, side, line[*ammount + 1].adress);         \
                tree_node->arg->size_value = line[*ammount + 1].length;         \
                *ammount = *ammount + 2;                                        \
                make_tree_recursion (tree_node->arg, line, ammount, amount_str);\
            }                                                                   \
            else                                                                \
            {                                                                   \
                *ammount = *ammount + 2;                                        \
            }                                                                   \
        }while(0)

static int make_tree_recursion (Tree* tree_node, Onegin* line, int* ammount, const int amount_str)
{   
    while (*ammount < amount_str)
    {
        if (*(line[*ammount].adress) == '}')
        {
            *ammount = *ammount + 1;
            return 0;
        }

        _pars_tree_def_(left_ptr, LEFT);

        _pars_tree_def_(right_ptr, RIGHT);


    }
    return 0;
}
#undef _pars_tree_def_

static char *copy_in_buffer_from_file (int* amount_str)
{
    CHECK_ERROR_(amount_str, "NULL_ADRESS", NULL);

    char* buffer = NULL; 
   
    FILE* input_file = fopen ("../../output/tree_dump_text.txt", "r");
    CHECK_ERROR_(input_file, "NOT OPENED", NULL);

    buffer = remove_trash_and_copy_in_buffer (amount_str, input_file);

    if (ferror(input_file))
    {
        printf ("ERROR in function : %s \n"
                "reading file falled\n", __func__);
        return NULL;
    }
    fclose (input_file);

    return buffer;
}


#undef CHECK_ERROR_