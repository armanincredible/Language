#include "stack.h"
#if defined (NO_PROTECT) || defined (FIRST_LEVEL) || defined (SECOND_LEVEL)

#define data_right_canary_          *(elem_t*)((char*)stack->data + stack->size* sizeof(elem_t))
#define data_left_canary_           *(elem_t*)(stack->data)

#define data_ponter_element_        (elem_t *)((char*)stack->data + sizeof (elem_t))
#define data_pointer_default_       (elem_t *)((char*)stack->data - sizeof (elem_t))

static int     stack_resize            (Stack* stack, const int mode);
static int     stack_status            (Stack* stack);
static int     check_stack_on_errors   (Stack* stack, const char* file, const char* funct, const int line);

int error_type = 0;

int stack_ctor (Stack* stack, const int size_st)
{
    if (stack == NULL)
    {
        return STACK_ADRESS_ERROR;
    }
    
    #ifdef NO_PROTECT
        stack->data = (elem_t*) calloc (1, size_st * sizeof (elem_t));
    #else
        stack->data = (elem_t*) calloc (1, size_st * sizeof (elem_t) + 2 * sizeof(stack->left_canary));
    #endif

    if (stack->data == NULL)
    {
        return DATA_ADRESS_ERROR;
    }
    stack->capacity = size_st;
    stack->size = 0;


    #ifndef NO_PROTECT
        data_left_canary_ = POISON_VALUE;
        stack->data = data_ponter_element_;

        stack->left_canary  = POISON_VALUE;
        stack->right_canary = POISON_VALUE;
    #endif

    #ifdef SECOND_LEVEL
        stack->hash = 0;
    #endif
    
    return check_stack_on_errors_(stack);

}

int stack_dtor (Stack* stack)
{
    int error = 0;
    if ((error = check_stack_on_errors_(stack)) > 0)
    {
        return error;
    }

    #ifdef NO_PROTECT
        memset (stack->data, 0, stack->size * sizeof (elem_t));
    #else
        stack->data = data_pointer_default_;
        memset (stack->data, 0, stack->size * sizeof (elem_t) + 2 * sizeof(stack->left_canary));
    #endif

    free (stack->data);

    #ifdef SECOND_LEVEL
        stack->hash = 0;
    #endif
    
    stack->size = -1;
    stack->capacity = -1;
    stack->data = (elem_t*)POISON_ADRESS;

    #ifndef NO_PROTECT
        stack->left_canary = 0;
        stack->right_canary = 0;
    #endif
    
    return error;
}

int stack_push (Stack* stack, elem_t value)
{
    int error = 0;
    if ((error = check_stack_on_errors_(stack)) > 0)
    {
        return error;
    }

    if ((error = stack_resize (stack, PUSH_MODE)) > 0)
    {
        return error;
    }
    
    stack->data [stack->size] = value;

    stack->size = stack->size + 1;

    #ifndef NO_PROTECT
        data_right_canary_ = POISON_VALUE;
    #endif

    #ifdef SECOND_LEVEL
        stack->hash = MurmurHash2 ((char*)stack->data, stack->size);
    #endif
    
    return check_stack_on_errors_(stack);
}

int stack_pop (Stack* stack, elem_t *variable)
{
    int error = 0;
    if (stack->size <= 0)
    {
        error_type = MEMORY_SIZE_ERROR;
    }
    if ((error = check_stack_on_errors_(stack)) > 0)
    {
        return error;
    }

    *variable = *(stack->data + stack->size - 1);

    if ((error = stack_resize (stack, POP_MODE)) > 0)
    {
        return error;
    }

    stack->size = stack->size - 1;

    #ifndef NO_PROTECT
        data_right_canary_ = POISON_VALUE;
    #endif

    #ifdef SECOND_LEVEL
        stack->hash = MurmurHash2 ((char*)stack->data, stack->size);
    #endif

    return check_stack_on_errors_(stack);
}

static int stack_resize (Stack* stack, const int mode)
{
    int error = 0;
    bool need_resize = 0;
    if ((error = check_stack_on_errors_(stack)) > 0)
    {
        return error;
    }

    if ((stack->size >= stack->capacity) && (mode == PUSH_MODE))
    {
        stack->capacity = stack->capacity * COMMON_RATIO;
        need_resize = 1;
    }

    if ((stack->size - stack->capacity / COMMON_RATIO <= Value_Reduce_Memory) &&
        (stack->size > BASIC_SIZE)                                            && 
        (mode == POP_MODE))
    {
        stack->capacity = stack->capacity / COMMON_RATIO;
        need_resize = 1;
    }

    if (need_resize)
    {   
        #ifdef NO_PROTECT
            stack->data = (elem_t*) realloc (stack->data, stack->capacity * sizeof (elem_t));
        #else
            stack->data = data_pointer_default_;
            stack->data = (elem_t*) realloc (stack->data, stack->capacity * sizeof (elem_t) + 2 * sizeof(stack->left_canary));
        #endif

        if (stack->data == NULL)
        {
            return DATA_ADRESS_ERROR;
        }

        #ifndef NO_PROTECT
            data_left_canary_ = POISON_VALUE;
            stack->data = data_ponter_element_;

            data_right_canary_ = POISON_VALUE;
        #endif

        #ifdef SECOND_LEVEL
            stack->hash = MurmurHash2 ((char*)stack->data, stack->size);
        #endif
    }
    
    return check_stack_on_errors_(stack);
}

static int stack_status (Stack* stack)
{
    if ((stack->data == (elem_t*)POISON_ADRESS) || 
        (stack->data == NULL))
    {
        stack->status = "DATA_ADRESS_ERROR";
        return DATA_ADRESS_ERROR;
    }

    if (stack == NULL)
    {
        stack->status = "STACK_ADRESS_ERROR";
        return STACK_ADRESS_ERROR;
    }

    if (error_type == MEMORY_SIZE_ERROR)
    {
        stack->status = "MEMORY_SIZE_ERROR";
        return MEMORY_SIZE_ERROR;
    }

    if ((stack->capacity <= 0) ||
        (stack->size < 0))
    {
        stack->status = "MOD_VALUE_ERROR";
        return MOD_VALUE_ERROR;
    }

    #ifndef NO_PROTECT

        if ((data_right_canary_ != POISON_VALUE) &&
            (stack->size != 0))
        {
            stack->status = "DATA_RIGHT_CANARY_ERROR";
            return DATA_RIGHT_CANARY_ERROR;
        }

        if (*data_pointer_default_ != POISON_VALUE)
        {
            stack->status = "DATA_LEFT_CANARY_ERROR";
            return DATA_LEFT_CANARY_ERROR;
        }

        if (stack->left_canary != POISON_VALUE)
        {
            stack->status = "STACK_LEFT_CANARY_ERROR";
            return STACK_LEFT_CANARY_ERROR;
        }

        if (stack->right_canary != POISON_VALUE)
        {
            stack->status = "STACK_RIGHT_CANARY_ERROR";
            return STACK_RIGHT_CANARY_ERROR;
        }
    #endif

    #ifdef SECOND_LEVEL
        unsigned int hash = 0;
        if (stack->size > 0)
        {
            hash = MurmurHash2 ((char*)stack->data, stack->size);
        }
        if ((hash != stack->hash) && (stack->size > 0))
        {
            stack->status = "HASH_ERROR";
            return HASH_ERROR;
        }   
    #endif

    stack->status = "NO_ERRORS";
    return NO_ERRORS;
}

static int check_stack_on_errors (Stack* stack, const char* file, const char* funct, const int line)
{   
    const int status = stack_status ((Stack*)stack);
    if (status > 0)
    {
        stack_dump (stack, file, funct, line);
    }
    return status;
}

unsigned int MurmurHash2 (char * key, unsigned int len)
{
  const unsigned int m = 0x5bd1e995;
  const unsigned int seed = 0;
  const int r = 24;

  unsigned int h = seed ^ len;

  const unsigned char * data = (const unsigned char *)key;

  while (len >= 4)
  {
      unsigned int k = 0;
      
      k  = data[0];
      k |= data[1] << 8;
      k |= data[2] << 16;
      k |= data[3] << 24;

      k *= m;
      k ^= k >> r;
      k *= m;

      h *= m;
      h ^= k;

      data += 4;
      len -= 4;
  }

  switch (len)
  {
    case 3:
    {
        h ^= data[2] << 16;
        break;
    }
    case 2:
    {
        h ^= data[1] << 8;
        break;
    }
    case 1:
    {
        h ^= data[0];
        h *= m;
        break;
    }
    default:
    {
        break;
    }
  }

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

int stack_dump (const Stack* stack, const char* file, const char* funct, const int line)
{
    FILE* stack_status_file = fopen ("stackstatus.txt", "w");
    if (stack_status_file == NULL)
    {
        return FILE_ERROR;
    }
    #ifdef NO_PROTECT
        fprintf (stack_status_file, "Stack haven't Protect\n\n");
    #endif

    #ifdef FIRST_LEVEL
        fprintf (stack_status_file, "Stack have FIRST_LEVEL Protect (it means that canarey works)\n\n");
    #endif

    #ifdef SECOND_LEVEL
        fprintf (stack_status_file, "Stack have SECOND_LEVEL Protect (it means that hash and canary works)\n\n");
    #endif


    fprintf (stack_status_file,  "Stack <%s> Have %s at %s at (%s:%d) \n\n",
             typeid(stack).name(), stack->status, file, funct, line);


    if (stack->data != (elem_t*)POISON_ADRESS)
    {          
            fprintf (stack_status_file,
                    "Capacity = %d\n" "Size = %d\n" "Pointer of data <%s> = %p\n\n",
                    stack->capacity, stack->size, typeid(*(stack->data)).name(), (void*)stack->data);

            #ifdef SECOND_LEVEL

                fprintf (stack_status_file, "hash = %u\n", stack->hash);
            #endif

            #ifndef NO_PROTECT

                fprintf (stack_status_file,
                        "left_canary_stack = %lf\n" "right_canary_stack = %lf\n"
                        "left_canary_data = %p\n" "right_canary_data = %p\n\n",
                        stack->left_canary, stack->right_canary,
                        (void*)((char*)stack->data - sizeof(stack->left_canary)),
                        (void*)(&data_right_canary_));
            #endif
    }

    for (int i = 0; i < stack->size; i++) 
    {
        fprintf (stack_status_file, "\tdata[%d] = %s \n", i, stack->data[i]);
    }

    fprintf (stack_status_file, "\nEND OF TXT\n \t\t\t\\Thanks for using our program\\");

    fclose (stack_status_file);

    return NO_ERRORS;                                                               
}

#endif