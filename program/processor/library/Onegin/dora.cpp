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
        adress_new_str = strchr (adress_new_str, '\n') + one;
        *(adress_new_str - 1) = '\0';

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
        printf ("ERROR in function : %s \nfile have NULL adress\n", __func__);
        return NULL;
    }

    int size_buffer = size_file (file);

    char* buffer = (char*)calloc (size_buffer, sizeof (char));
    if (buffer == NULL)
    {
        printf ("ERROR in function : %s \nhaven't memory for buffer open\n", __func__);
        return NULL;
    }
    
    int amount_of_necessary_symbols = 0;
    char curSymbol = '0';
    char symbol_past = '\n';
    bool new_string = true;
    *str_amount = 1;

    for (;(curSymbol = getc (file)) != EOF;)
    {
        if ((new_string == 1) && (curSymbol == ' '))
        {
            continue;
        }
        new_string = false;

        if ((curSymbol == symbol_past) &&
           ((curSymbol == ' ') || (curSymbol == '\t') || 
            (curSymbol == ',') || (curSymbol == '\n')))
        {
            continue;
        }

        if (curSymbol == '\t')
        {
            continue;
        }

        if (curSymbol == '\n')
        {
            new_string = true;
            if (symbol_past != '\n')
            {
                *str_amount = *str_amount + 1;
            }
        }

        symbol_past = curSymbol;
        buffer [amount_of_necessary_symbols] = curSymbol;
        amount_of_necessary_symbols++;
    }

    for (int amount = amount_of_necessary_symbols; amount < size_buffer; amount++)
    {
        buffer [amount] = '\0';
    }

    if (ferror(file))
    {
        printf ("ERROR in function : %s \nreading file falled\n", __func__);
        return NULL;
    }

    fclose (file);
    
    return buffer;
}