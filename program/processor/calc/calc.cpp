#include "calc.h"

static char*   make_array      (FILE* input_file);

static int     take_arguments  (char * array, int* ip, int * reg_value, elem_t* inm_value, Bit_Type_Comand* bit_field);

#define proc_dump_write(void)                                                               \
        fprintf (calc_dump, "ram_status = %d, reg_status = %d, inm_status %d, cmd = %d\n"   \
                            "IP %d, REG_VALUE %d, INM_VALUE %lg\n\n\n",                     \
                            bit_field.ram, bit_field.reg, bit_field.inm, bit_field.cmd,     \
                            proc->Ip, reg_value, inm_value);

#define CHECK_ERROR_(name, type_error, ret)                                             \
        do{                                                                             \
            if (name == NULL)                                                           \
            {                                                                           \
                printf ("\nERROR in function : %s \n"                                   \
                        "%s have %s\n", __func__,                                       \
                        #name, type_error);                                             \
                return ret;                                                             \
            }                                                                           \
        } while (0)

elem_t calculate (Stack* stack, Proc_str* proc)
{
    CHECK_ERROR_(stack, "ZERO ADRESS", -1);

    CHECK_ERROR_(proc, "ZERO ADRESS", -1);


    FILE* input_file = fopen ("code.rb", "rb");
    CHECK_ERROR_(input_file, "NOT OPENED", -1);

    FILE* calc_dump = fopen ("calc_status.txt", "w");
    CHECK_ERROR_(calc_dump, "NOT OPENED", -1);
    
    int rtr_pointer [AMMOUNT_SAVES_IP] = {0};
    int rtr_ip = 0;

    proc->Code = make_array (input_file);
    CHECK_ERROR_(proc->Code, "ZERO ADRESS", -1);

    proc->Ram = (elem_t*) calloc (SIZE_RAM + SIZE_VRAM, sizeof(elem_t));
    CHECK_ERROR_(proc->Ram, "ZERO ADRESS (Not Enough Memory)", -1);

    memset (proc->Ram, 0, SIZE_RAM);

    int operation = 0;
    proc->Ip = 0;

    int reg_value = 0;
    elem_t inm_value = 0;

    Bit_Type_Comand bit_field = {0, 0, 0, 0};

    while (1)
    {   
        bit_field = *((Bit_Type_Comand*)(proc->Code + proc->Ip));

        inm_value = 0;
        reg_value = 0;

        if (take_arguments (proc->Code, &(proc->Ip), &reg_value, &inm_value, &bit_field) == -1)
        {
            break;
        }

        proc_dump_write(void) 

        operation = bit_field.cmd;

        if (operation == HLT)
        {
            break;
        }

        #define DEF_CMD(num, name, hz, arg)     \
                case name:                      \
                {                               \
                    arg                         \
                    break;                      \
                }                               

        switch (operation)
        {
            #include "..\library\headers\commands.def"
        }
        
        #undef DEF_CMD

    }

    /*for (int i = 0; i < 10; i++)
    {
        printf ("%d : %lg\n", i, proc->Ram[i]);
    }*/

    free(proc->Ram);
    free(proc->Code);
    
    if (ferror(input_file))
    {
        printf ("ERROR in function : %s \n"
                "reading input_file falled\n", __func__);
        return -1;
    }
    fclose (input_file);

    if (ferror(calc_dump))
    {
        printf ("ERROR in function : %s \n"
                "writing calc_dump falled\n", __func__);
        return -1;
    }
    fclose (calc_dump);

    return 0;
}

static char* make_array (FILE* input_file)
{
    CHECK_ERROR_(input_file, "ZERO ADRESS", NULL);

    int size = size_file (input_file);

    char* array = (char*) calloc (size, sizeof(char));
    CHECK_ERROR_(array, "ZERO ADRESS (Not Enough Memory)", NULL);

    if (fread (array, sizeof (char), size, input_file) == 0)
    {
        printf ("ERROR in function : %s \n"
                "writing falled\n", __func__);
        return NULL;
    }

    return array;
}


static int take_arguments (char * array, int* ip, int * reg_value, elem_t* inm_value, Bit_Type_Comand* bit_field)
{
    CHECK_ERROR_(array, "ZERO ADRESS", -1);

    CHECK_ERROR_(ip, "ZERO ADRESS", -1);

    CHECK_ERROR_(reg_value, "ZERO ADRESS", -1);

    CHECK_ERROR_(inm_value, "ZERO ADRESS", -1);

    CHECK_ERROR_(bit_field, "ZERO ADRESS", -1);


    if ((bit_field->reg == 0) && (bit_field->inm == 0))
    {
        *ip += 1;
        return 0;
    }
    if (bit_field->reg == 1)
    {
        *reg_value = array [*ip + 1];
        if (bit_field -> inm == 1)
        {
            *inm_value = *((elem_t*)(array + *ip + 2));
            *ip += 2 + sizeof (elem_t);
            return 0;
        }
        *ip += 2;
    }
    else if (bit_field -> inm == 1)
    {
        *inm_value = *((elem_t*)(array + *ip + 1));
        *ip += 1 + sizeof (elem_t);
    }
    return 0;
}

#undef CHECK_ERROR_

#undef proc_dump_write