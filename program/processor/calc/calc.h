#ifndef CALCHEADER
#define CALCHEADER

#include "..\library\Onegin\dora.h"
#include "..\library\Stack\stack.h"
#include "sys\stat.h"
#include "..\library\headers\codes_operations.h"

struct Proc_str
{
    struct Stack st;
    struct Stack rtn;
    int Ip = 0;
    elem_t* Regs;
    char* Code;
    elem_t* Ram;
};

const int SIZE_VRAM = 25;
const int SIZE_RAM = 10000;
const int AMMOUNT_REGS = 10;
const int AMMOUNT_SAVES_IP = 1000;

elem_t  calculate       (Stack* stack, Proc_str* proc);

#endif