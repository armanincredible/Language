#include "dora.h"

int make_array_adress (char* buffer, const int str_amount, Onegin* line)
{
    if (line == NULL)
    {
        printf ("ERROR in function : %s \nline has zero adress\n", __func__);
        return -1;  
    }
    if (buffer == NULL)
    {
        printf ("ERROR in function : %s \nbuffer has zero adress\n", __func__);
        return -1;
    }
    
    char *adress_new_str = buffer;
    char *past_adress = buffer;
    line[0].adress = buffer;
    int amount = 1;
    int one = 1; 

    for (; amount != str_amount; amount++)
    {
        adress_new_str = strchr (adress_new_str, '\0') + one;

        line [amount      ].adress = adress_new_str;
        line [amount - one].length = adress_new_str - past_adress - one;
        
        past_adress = adress_new_str;
    }

    adress_new_str = strchr (adress_new_str, '\0') + one;
    //*(adress_new_str - one) = '\n';//
    
    line[amount      ].adress = adress_new_str;
    line[amount - one].length = line[amount].adress - line[amount - one].adress - one;

    return 1;
}

int size_file (FILE *filestream)
{
    if (filestream == NULL)
    {
        printf ("ERROR in function : %s \nfilestream have zero adress\n", __func__);
        return -1;
    }

    fseek (filestream, 0, SEEK_END);

    int size = ftell (filestream);

    fseek (filestream, 0, SEEK_SET);
    
    return (size);
}

char* remove_trash_and_copy_in_buffer (int* str_amount, FILE* file)
{
    if (str_amount == NULL)
    {
        printf ("ERROR in function : %s \nstr_amount have zero adress\n", __func__);
        return NULL;
    }

    if (file == NULL)
    {
        printf ("ERROR in function : %s \ndora.txt didnt open\n", __func__);
        return NULL;
    }

    int size_buffer = size_file (file);

    char* buffer = (char*)calloc (size_buffer, sizeof (char));
    if (buffer == NULL)
    {
        printf ("ERROR in function : %s \nhaven't memory for buffer open\n", __func__);
        return NULL;
    }
    
    char curSymbol = '0';
    //bool new_string = true;
    *str_amount = 0;
    int i = 0;

    for (;(curSymbol = getc (file)) != EOF;)
    {
        //symbol_past = curSymbol;
        if (curSymbol != ' ')
        {
            buffer [i] = curSymbol;
            i++;
            if (curSymbol == '\n')
            {
               *str_amount = *str_amount + 1; 
               buffer [i-1] = '\0';
               //buffer [*str_amount] = '\0';
            }
        }
        //amount_of_necessary_symbols++;
    }
    //printf ("%s", buffer);
    
    return buffer;
}

int compare_char (const char first_symb, const char second_symb) //!!!!!!!!!!!!!!!comp do universal and better without if and swap//
{
    return (tolower (first_symb) - tolower (second_symb));
}