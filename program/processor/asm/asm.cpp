#include "asm.h"

static void   arg_jump                      (Asm_values* asm_values, Bit_Type_Comand* bit_field, Labels* label, const char* lb_name);

static void   arg                           (Asm_values* asm_values, Bit_Type_Comand* bit_field,  const char reg, const elem_t value);

static int    ctor_asm_values               (Asm_values* asm_value, const int amount_operations);

static int    parsing_push_pop              (Asm_values* asm_values, Bit_Type_Comand* bit_field);

static int    parsing_label                 (Asm_values* asm_values, Labels* label);

static int    parsing_call_jump             (Asm_values* asm_values, Bit_Type_Comand* bit_field,  Labels* label);

static int    parsing_default               (Asm_values* asm_values, const Bit_Type_Comand* bit_field);


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

char *copy_in_buffer_from_file_asm (int* amount_operations)
{
    CHECK_ERROR_(amount_operations, "NULL_ADRESS", NULL);

    char* buffer = NULL; 
   
    FILE* asm_file = fopen ("../library/txt_library/asm.txt", "r");
    if (asm_file != NULL)
    {
        buffer = remove_trash_and_copy_in_buffer (amount_operations, asm_file);
    }

    CHECK_ERROR_(asm_file, "NOT OPENED", NULL);

    if (ferror(asm_file))
    {
        printf ("ERROR in function : %s \n"
                "reading file falled\n", __func__);
        return NULL;
    }
    fclose (asm_file);

    return buffer;
}

#define _STR_EQ_(first_str, sec_str, size)      \
        strncmp (first_str, sec_str, size) == 0


int make_code_rb (Onegin *line, Labels* label, const int amount_operations)
{
    CHECK_ERROR_(line, "NULL ADRESS", -1);

    CHECK_ERROR_(label, "NULL ADRESS", -1);

    FILE* output_file = fopen ("../calc/code.rb", "wb"); ///////argument command str
    
    CHECK_ERROR_(output_file, "NOT OPENED", -1);

    Asm_values asm_values = {};

    ctor_asm_values (&asm_values, amount_operations);


    Bit_Type_Comand bit_field = {0, 0, 0, 0};      


    for (asm_values.cur_operation = 0; asm_values.cur_operation < amount_operations; asm_values.cur_operation++)
    {
        bit_field.ram = 0;
        bit_field.reg = 0;
        bit_field.inm = 0;

        int status_work = 0;

        asm_values.line_operation = line[asm_values.cur_operation].adress;
   
        /*printf("%s\n", line[asm_values.cur_operation].adress);*/

        #define _parsing_default    parsing_default     (&asm_values, &bit_field)   

        #define _parsing_push_pop   parsing_push_pop    (&asm_values, &bit_field) 

        #define _parsing_call_jump  parsing_call_jump   (&asm_values, &bit_field, label)

        #define _parsing_label      parsing_label       (&asm_values, label)

        #define _cmp_status_work_                                                       \
            if (status_work == 1)                                                       \
            {                                                                           \
                continue;                                                               \
            }                                                                           \
            if (status_work == -1)                                                      \
            {                                                                           \
                break;                                                                  \
            }   

        #define DEF_CMD(num, name, pars, ...)                                           \
            bit_field.cmd = (num);                                                      \
            asm_values.size_name = sizeof (#name);                                      \
            asm_values.name_of_operaion = #name;                                        \
                                                                                        \
            status_work = _parsing_label;                                               \
            _cmp_status_work_                                                           \
            if (_STR_EQ_(asm_values.line_operation, #name, sizeof (#name) - 1))         \
            {                                                                           \
                status_work = pars;                                                     \
                _cmp_status_work_                                                       \
            }                                                                           

        #include "..\library\headers\commands.def"

        #undef DEF_CMD
        #undef _cmp_status_work_
        #undef _parsing_default
        #undef _parsing_push_pop
        #undef _parsing_call_jump
        #undef _parsing_

        printf ("BAD ASM");
        return -1;
    }

    /*for (int i = 0; i < AMMOUNT_LABELS; i++)
    {
        printf ("\n\n label %s has ip %d", label[i].name, label[i].ip_save);
    }*/

    if (fwrite (asm_values.code_rb, sizeof (char), asm_values.ip, output_file) == 0)
    {
        printf ("ERROR in function : %s \n"
                "writing file falled\n", __func__);
        return -1;
    }

    if (ferror(output_file))
    {
        printf ("ERROR in function : %s \n"
                "writing file falled\n", __func__);
        return -1;
    }
    fclose (output_file);

    return 0;
}

#define security_function                                    \
        do {                                                 \
            CHECK_ERROR_(asm_values, "NULL ADRESS", -1);     \
            CHECK_ERROR_(bit_field, "NULL ADRESS", -1);      \
            CHECK_ERROR_(label, "NULL ADRESS", -1);          \
        }while (0)

static int parsing_default (Asm_values* asm_values, const Bit_Type_Comand* bit_field)
{   
    CHECK_ERROR_(asm_values, "NULL ADRESS", -1);
    CHECK_ERROR_(bit_field, "NULL ADRESS", -1);

    int size_str = asm_values->size_name - 1;

    if (_STR_EQ_(asm_values->name_of_operaion, asm_values->line_operation, size_str))
    {
        *((Bit_Type_Comand*)(asm_values->code_rb + asm_values->ip)) = *bit_field;
        asm_values->ip = asm_values->ip + 1;
        return 1;
    }
    return 0;
}

static int parsing_call_jump (Asm_values* asm_values, Bit_Type_Comand* bit_field,  Labels* label)
{   
    CHECK_ERROR_(asm_values, "NULL ADRESS", -1);
    CHECK_ERROR_(bit_field, "NULL ADRESS", -1); 
    CHECK_ERROR_(label, "NULL ADRESS", -1);  

    char lb_name [MAX_AMOUNT_SMB] = {0};
    int size_str = asm_values->size_name - 1;

    if (sscanf (asm_values->line_operation, "CALL %s", lb_name) == 1)                  
    {                                                                       
        printf ("CALL %s\n", lb_name);                                      
        bit_field->cmd = CALL;                                               
        arg_jump (asm_values, bit_field, label, lb_name);                                                    
        return 1;                                                         
    }

    if ((_STR_EQ_(asm_values->line_operation, asm_values->name_of_operaion, size_str))      &&                
        (sscanf (asm_values->line_operation + asm_values->size_name, "%s", lb_name) == 1))          
    {                                                                     
        arg_jump (asm_values, bit_field, label, lb_name);  
        return 1;                                             
    }                                                                   
    
    return 0;  
}

static int parsing_push_pop (Asm_values* asm_values, Bit_Type_Comand* bit_field)
{
    CHECK_ERROR_(asm_values, "NULL ADRESS", -1);
    CHECK_ERROR_(bit_field, "NULL ADRESS", -1);

    elem_t value = 0;
    char reg = 0;

    char* pointer_on_str = asm_values->line_operation + asm_values->size_name; 

    if ((*(asm_values->line_operation + asm_values->size_name) == '[') && 
        (*(asm_values->line_operation + strlen (asm_values->line_operation) - 1) == ']')) 
    {
        bit_field->ram = 1;
        /*add_pointer = 1;*/
        pointer_on_str = pointer_on_str + 1;
    }
    if ((sscanf (pointer_on_str,
                "%cX + %lf", &reg, &value) == 2) &&
                (isalpha (reg)))
    {
        bit_field->reg = 1;
        bit_field->inm = 1;      
        arg (asm_values, bit_field, reg, value);
        return 1;                                               
    }
    if ((sscanf (pointer_on_str,                                        
                "%cX", &reg) == 1) &&                                   
                (isalpha (reg)))                                        
    {
        bit_field->reg = 1;
        bit_field->inm = 0; 
        arg (asm_values, bit_field, reg, value);
        return 1;                                                   
    }                                                                   
    if (sscanf (pointer_on_str,                                         
                "%lf", &value) == 1)                                  
    {                                                                
        bit_field->reg = 0;
        bit_field->inm = 1;                                     
        arg (asm_values, bit_field, reg, value);
        return 1;                                 
    }                                                                  
    return 0; 
}

static int parsing_label (Asm_values* asm_values, Labels* label)
{
    CHECK_ERROR_(asm_values, "NULL ADRESS", -1);
    CHECK_ERROR_(label, "NULL ADRESS", -1);

    char lb_name [MAX_AMOUNT_SMB] = {0};

    if (sscanf (asm_values->line_operation, "$ %s :", lb_name) == 1)
    {
        printf ("$ %s :\n", lb_name);
        for (int am = 0; am < asm_values->ammount_lbls; am++)
        {
            if (_STR_EQ_(label[am].name, lb_name,  
                         strlen (lb_name)))
            {
                label[am].ip_save =  asm_values->ip;
                return 1;
            }
        }
        
        asm_values->not_need_reasm = 0;

        label[asm_values->ammount_lbls].name    =   asm_values->line_operation + 2;
        label[asm_values->ammount_lbls].ip_save =   asm_values->ip;

        asm_values->ammount_lbls = asm_values->ammount_lbls + 1;

        return 1;                                                         
    }

    if ((asm_values->am_asm < 2)                                            &&
        (asm_values->not_need_reasm == false)                               && 
        (asm_values->cur_operation == asm_values->amount_operations - 1))
    {
        asm_values->ip = 0;
        asm_values->not_need_reasm = 1;
        asm_values->cur_operation = -1;
        asm_values->am_asm = asm_values->am_asm + 1;
        return 1; 
    }
    return 0;                                                                          
}

static void arg (Asm_values* asm_values, Bit_Type_Comand* bit_field, const char reg, const elem_t value)
{
    *((Bit_Type_Comand*)(asm_values->code_rb + asm_values->ip)) = *bit_field;
    asm_values->ip = asm_values->ip + 1;
    if (bit_field->reg == 1)                                           
    {
        *(asm_values->code_rb + asm_values->ip) = reg - 'A';
        asm_values->ip = asm_values->ip + 1;           
    }
    if (bit_field->inm == 1)
    {
        *((elem_t*)(asm_values->code_rb + asm_values->ip)) = value;                                                   
        asm_values->ip = asm_values->ip + sizeof (elem_t);
    }
    return;
}

static void arg_jump (Asm_values* asm_values, Bit_Type_Comand* bit_field, Labels* label, const char* lb_name)
{
    bit_field->inm = 1;
    *((Bit_Type_Comand*)(asm_values->code_rb + asm_values->ip)) = *bit_field;
    asm_values->ip = asm_values->ip + 1;

    for (int am = 0; am < asm_values->ammount_lbls; am++)
    {
        if (_STR_EQ_(label[am].name, lb_name,
                    strlen (lb_name)))
        {
            printf ("in %s write it %d with name %s\n",                         
                    lb_name, label[am].ip_save,                                 
                    label[am].name);                                            
            *((elem_t*)(asm_values->code_rb + asm_values->ip)) = label[am].ip_save;     
            break;                                                              
        }                                                                       
        printf ("in %s dont write it %d with name %s \n",                       
                lb_name, label[am].ip_save,                                     
                label[am].name);                                                
        asm_values->not_need_reasm = 0;                                             
    }
    asm_values->ip = asm_values->ip + sizeof (elem_t);     
    return;     
}                               


static int ctor_asm_values (Asm_values* asm_values, const int amount_operations)
{
    asm_values->code_rb = (char*) calloc (amount_operations, sizeof (elem_t) + 2 * sizeof (char));

    CHECK_ERROR_(asm_values->code_rb, "ZERO ADRESS (Not Enough Memory)", -1);

    asm_values->ip = 0;
    asm_values->ammount_lbls = 0;
    asm_values->not_need_reasm = 1;
    asm_values->am_asm = 1;
    asm_values->size_name = 0;
    asm_values->amount_operations = amount_operations;

    return 0;
}


#undef security_function

#undef CHECK_ERROR_

#undef _STR_EQ_