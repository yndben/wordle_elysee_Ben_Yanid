#ifndef WORDLE_SOLVER_H
#define WORDLE_SOLVER_H

#define MAX_WORDS     3000
#define WORD_LEN      5
#define MAX_LINE      64

typedef struct {
    char words[MAX_WORDS][WORD_LEN + 1];
    int  count;
} WordList;

int  wl_load(WordList *wl, const char *path);
void wl_print(const WordList *wl);
int  wl_count(const WordList *wl);




void filter_include(WordList *wl, const char *letters);


void filter_exclude(WordList *wl, const char *letters);


void filter_position(WordList *wl, char letter, int pos);


void filter_misplaced(WordList *wl, char letter, int pos);


void filter_substring(WordList *wl, const char *substr);


void filter_wordle_result(WordList *wl, const char *guess, const char *result);


void suggest(const WordList *wl, int n);

#endif
