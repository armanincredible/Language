#include "stack.h"
#if defined (NO_PROTECT) || defined (FIRST_LEVEL) || defined (SECOND_LEVEL)

int main()
{
    Stack st = {};
    elem_t variable = 0;
    stack_ctor (&st, BASIC_SIZE);
    int i = 0;
    for (; i < 30; i ++)
    {
        stack_push (&st, 15.2);
        printf ("%lf\n", st.data[i]);
    }
    for (; i > 15; i --)
    {
        stack_pop (&st, &variable);
        printf ("%lf\n", variable);
    }
    /**(elem_t*)((char*)st.data + (st.size - 1)* sizeof(elem_t) + sizeof(st.left_canary)) = 1;*/
    stack_dump (&st, __FILE__, __func__, __LINE__);
    stack_dtor (&st);
}
#endif