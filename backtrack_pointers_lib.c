#include <stdlib.h>
#include <string.h>
#include "backtrack_pointers_lib.h"

#define CAPACITY_INC 10

void create_backtrack_pointer(PBacktrackPointer *btp)
{
    if (btp == NULL)
    {
        return;
    }
    *btp = malloc(sizeof(TBacktrackPointer));
    (*btp)->size = 0;
    (*btp)->capacity = 0;
    (*btp)->words = NULL;
    (*btp)->times = NULL;
}

void free_backtrack_pointer(PBacktrackPointer *btp)
{
    if (btp == NULL)
    {
        return;
    }
    if (*btp == NULL)
    {
        return;
    }
    if ((*btp)->words != NULL)
    {
        free((*btp)->words);
        (*btp)->words = NULL;
    }
    if ((*btp)->times != NULL)
    {
        free((*btp)->times);
        (*btp)->times = NULL;
    }
    (*btp)->size = 0;
    (*btp)->capacity = 0;
}

void add_to_backtrack_pointer(PBacktrackPointer btp, int word_i, int time_i)
{
    if (btp == NULL)
    {
        return;
    }

    btp->size++;
    if (btp->size < btp->capacity)
    {
        do {
            btp->capacity += CAPACITY_INC;
        } while (btp->size < btp->capacity);

        if (btp->words == NULL)
        {
            btp->words = malloc(sizeof(int) * btp->capacity);
        }
        else
        {
            btp->words = realloc(btp->words, sizeof(int) * btp->capacity);
        }

        if (btp->times == NULL)
        {
            btp->times = malloc(sizeof(int) * btp->capacity);
        }
        else
        {
            btp->times = realloc(btp->times, sizeof(int) * btp->capacity);
        }
    }

    btp->words[btp->size-1] = word_i;
    btp->times[btp->size-1] = time_i;
}

void remove_from_backtrack_pointer(PBacktrackPointer btp)
{
    if (btp == NULL)
    {
        return;
    }
    if (btp->size > 0)
    {
        btp->size--;
    }
}

void remove_all_from_backtrack_pointer(PBacktrackPointer btp)
{
    if (btp == NULL)
    {
        return;
    }
    if (btp->size > 0)
    {
        btp->size = 0;
    }
}

void copy_backtrack_pointers(PBacktrackPointer dst, PBacktrackPointer src)
{
    if ((dst == NULL) || (src == NULL))
    {
        return;
    }
    if (dst->capacity < src->capacity)
    {
        do {
            dst->capacity += CAPACITY_INC;
        } while (dst->capacity < src->capacity);

        if (dst->words == NULL)
        {
            dst->words = malloc(sizeof(int) * dst->capacity);
        }
        else
        {
            dst->words = realloc(dst->words, sizeof(int) * dst->capacity);
        }

        if (dst->times == NULL)
        {
            dst->times = malloc(sizeof(int) * dst->capacity);
        }
        else
        {
            dst->times = realloc(dst->times, sizeof(int) * dst->capacity);
        }
    }
    dst->size = src->size;
    if (dst->size > 0)
    {
        memcpy(dst->words, src->words, sizeof(int) * dst->size);
        memcpy(dst->times, src->times, sizeof(int) * dst->size);
    }
}
