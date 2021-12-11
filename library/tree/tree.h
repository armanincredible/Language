#ifndef TREEHEADER
#define TREEHEADER

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 

typedef char* node_t;

const double EPS = 10e-6;

struct Arg_node
{
    node_t data;
};

struct Tree
{
    Arg_node value;
    Tree* left_ptr;
    Tree* right_ptr;
    Tree* pre_ptr;
    int size_value;
    int level;
};

enum Orientation
{
    LEFT,
    RIGHT,
    IN
};

const int NAME_LENGTH = 16;

int tree_verificate (Tree* tree_node);

int tree_ctor (Tree** tree_node);

int tree_dtor (Tree** tree_node);

int tree_push (Tree* tree_node, int side, node_t name);

int tree_dump_visual (Tree* tree_node);

Tree* New_Node (const node_t value, Tree* left_ptr, Tree* right_ptr);

Tree* copy_tree_node (Tree* tree_node);


#endif