#ifndef LANGHEADER
#define LANGHEADER

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include <assert.h>

#include "..\..\library\tree\tree.h"
#include "..\..\library\stack\stack.h"
#include "..\..\library\onegin\dora.h"

const int VALUE_CONST_NAMES_NODE = 20;

const int AMOUNT_VAR = 1000;

enum System_regs
{
    RET_REG = 'J',
    R_COND = 'K',
    L_COND = 'L',
    V_COND = 'G',
    FREE_MEM_REG = 'F',
    SAVE_MEM_REG = 'E',
    ARG_SUM_REG = 'C'
};

enum CONST_NAMES_NODE
{
    CALC_COND,
    DEFINE,
    CALL,
    STATEMENT,
    DECISION,
    PARAMETER,
    ARRAY,
    DORA,/////
    DURA, ///;/////
    VTURILAS,///if
    VKRASHILAS,////else
    VLYAPALAS,///while
    POSHLU,////return///////
    FUNCTION
};

enum Type_Error
{
    NO_ERROR,
    NO_PARENTHESIS,
    NO_NUMBER,
    NO_END_DOLLAR,
    NO_END_OP,
    NO_END_ARRAY_PAR
};

struct Variables
{
    char* name;
    int size;
    int ptr;
};

struct Arrays
{
    char* name;
    int size;
    int ptr;
};

struct Asm_values
{
    FILE* output_file;
    int cur_am_var;///cur amount variables
    int cur_field;
    int cur_am_if;
    int cur_am_wh;
    int size_arr;
};


struct node_st
{
    Tree** array;
    int size;
    int capacity;
    int cur_node;
};

const int BASIC_SIZE_NODES_ARRAY = 30;

int tree_nodes_ctr(node_st* nodes_st);

int tree_nodes_dtor(node_st* nodes_st);

int make_nodes_arr (node_st* nodes_st, char* str);

Tree* get_general (node_st* nodes_st);

int make_tree_asm (node_st* nodes_st, Tree* tree_node);

int make_code (Tree* tree_node);

Onegin* make_tree_from_library (Tree* tree_node, char** buffer);

#endif 