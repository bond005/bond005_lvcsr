#ifndef BACKTRACK_POINTERS_LIB_H
#define BACKTRACK_POINTERS_LIB_H

#define CAPACITY_INC 10

/* Structure for representation of backtrack pointer. The backtrack pointer is
 * stack of word-time pairs which defines best words sequence (see X.Huang,
 * Spoken Language Processing, pp.618-620). */
typedef struct _TBacktrackPointer
{
    int size;    /* number of the backtrack pointer items */
    int capacity;/* capacity, i.e. maximal number of the backtrack pointer
                    items, for which the memory is allocated. */
    int *words;  /* sequence of word indexes in the word lexicon */
    int *times;  /* sequence time moments */
} TBacktrackPointer;
typedef TBacktrackPointer* PBacktrackPointer;

/* This function creates the new TBacktrackPointer structure and initializes
 * fields of this structure by zeros. Thus, after the backtrack pointer
 * creation its length is null. */
void create_backtrack_pointer(PBacktrackPointer *btp);

/* This function frees memory which was allocated for TBacktrackPointer
 * structure and its fields. */
void free_backtrack_pointer(PBacktrackPointer *btp);

/* This function adds new word-time pair into the tail of backtrack pointer. */
void add_to_backtrack_pointer(PBacktrackPointer btp, int word_i, int time_i);

/* This function removes one word-time pair from the tail of backtrack
 * pointer. */
void remove_from_backtrack_pointer(PBacktrackPointer btp);

/* This function removes all word-time pairs from the backtrack pointer. */
void remove_all_from_backtrack_pointer(PBacktrackPointer btp);

/* This function copies contents of fields from one backtrack pointer to
 * another one. */
void copy_backtrack_pointers(PBacktrackPointer dst, PBacktrackPointer src);

#endif // BACKTRACK_POINTERS_LIB_H
