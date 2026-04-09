#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "wordle_solver.h"



static void to_lower_str(char *s) {
    for (; *s; s++) *s = (char)tolower((unsigned char)*s);
}


static void remove_word(WordList *wl, int i) {
    wl->count--;
    if (i != wl->count)
        memcpy(wl->words[i], wl->words[wl->count], WORD_LEN + 1);
}



int wl_load(WordList *wl, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", path);
        return -1;
    }
    wl->count = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f) && wl->count < MAX_WORDS) {
        /* Strip newline / carriage return */
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) != WORD_LEN) continue;
        to_lower_str(line);
        memcpy(wl->words[wl->count], line, WORD_LEN + 1);
        wl->count++;
    }
    fclose(f);
    return wl->count;
}



void wl_print(const WordList *wl) {
    for (int i = 0; i < wl->count; i++)
        printf("  %s\n", wl->words[i]);
    printf("  (%d mot(s))\n", wl->count);
}

int wl_count(const WordList *wl) {
    return wl->count;
}



void filter_include(WordList *wl, const char *letters) {
    char low[27];
    strncpy(low, letters, 26);
    low[26] = '\0';
    to_lower_str(low);

    int i = 0;
    while (i < wl->count) {
        int keep = 1;
        for (int k = 0; low[k]; k++) {
            if (!strchr(wl->words[i], low[k])) { keep = 0; break; }
        }
        if (!keep) remove_word(wl, i);
        else       i++;
    }
}



void filter_exclude(WordList *wl, const char *letters) {
    char low[27];
    strncpy(low, letters, 26);
    low[26] = '\0';
    to_lower_str(low);

    int i = 0;
    while (i < wl->count) {
        int remove = 0;
        for (int k = 0; low[k]; k++) {
            if (strchr(wl->words[i], low[k])) { remove = 1; break; }
        }
        if (remove) remove_word(wl, i);
        else        i++;
    }
}



void filter_position(WordList *wl, char letter, int pos) {
    if (pos < 0 || pos >= WORD_LEN) return;
    char c = (char)tolower((unsigned char)letter);
    int i = 0;
    while (i < wl->count) {
        if (wl->words[i][pos] != c) remove_word(wl, i);
        else                        i++;
    }
}



void filter_misplaced(WordList *wl, char letter, int pos) {
    if (pos < 0 || pos >= WORD_LEN) return;
    char c = (char)tolower((unsigned char)letter);
    int i = 0;
    while (i < wl->count) {
        /* Must contain the letter but NOT at this position */
        if (!strchr(wl->words[i], c) || wl->words[i][pos] == c)
            remove_word(wl, i);
        else
            i++;
    }
}



void filter_substring(WordList *wl, const char *substr) {
    char low[WORD_LEN + 1];
    strncpy(low, substr, WORD_LEN);
    low[WORD_LEN] = '\0';
    to_lower_str(low);

    int i = 0;
    while (i < wl->count) {
        if (!strstr(wl->words[i], low)) remove_word(wl, i);
        else                            i++;
    }
}


void filter_wordle_result(WordList *wl, const char *guess, const char *result) {
    char g[WORD_LEN + 1], r[WORD_LEN + 1];
    strncpy(g, guess,  WORD_LEN); g[WORD_LEN] = '\0'; to_lower_str(g);
    strncpy(r, result, WORD_LEN); r[WORD_LEN] = '\0'; to_lower_str(r);

    for (int k = 0; k < WORD_LEN; k++) {
        if (r[k] == 'v') {
            filter_position(wl, g[k], k);
        } else if (r[k] == 'o') {
            filter_misplaced(wl, g[k], k);
        } else { 
           
            int elsewhere = 0;
            for (int j = 0; j < WORD_LEN; j++) {
                if (j != k && g[j] == g[k] && (r[j] == 'v' || r[j] == 'o'))
                    elsewhere = 1;
            }
            if (!elsewhere) filter_exclude(wl, (char[]){g[k], '\0'});
        }
    }
}


void suggest(const WordList *wl, int n) {
    int limit = (n <= 0 || n > wl->count) ? wl->count : n;
    printf("Suggestions (%d/%d mot(s) restants) :\n", limit, wl->count);
    for (int i = 0; i < limit; i++)
        printf("  %s\n", wl->words[i]);
    if (limit < wl->count)
        printf("  ... et %d autre(s)\n", wl->count - limit);
}
