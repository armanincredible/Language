#include "calc.h"

int main (void)
{
    Proc_str proc = {};
    proc.st = {};
    proc.rtn = {};
    stack_ctor (&proc.st, BASIC_SIZE);
    
    proc.Regs = (elem_t*) calloc (AMMOUNT_REGS, sizeof (elem_t));
    if (proc.Regs == NULL)
    {
        printf ("ERROR in function : %s \n"
        "havent memory for regs\n", __func__);
        return -1;
    }
    memset (proc.Regs, 0, AMMOUNT_REGS);

    calculate (&proc.st, &proc);

    free (proc.Regs);

    stack_dump (&proc.st, __FILE__, __func__, __LINE__);

    stack_dtor (&proc.st);

    return 0;
}