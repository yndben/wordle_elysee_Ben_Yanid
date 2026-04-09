#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "wordle_solver.h"



static void to_lower_str(char *s) {
    for (; *s; s++) *s = (char)tolower((unsigned char)*s);
}

static void trim(char *s) {
    char *end = s + strlen(s) - 1;
    while (end >= s && isspace((unsigned char)*end)) { *end = '\0'; end--; }
}

static void print_help(void) {
    printf("\n"
           "=== WORDLE SOLVER — commandes disponibles ===\n"
           "  help                    Afficher cette aide\n"
           "  list                    Lister tous les mots restants\n"
           "  suggest [n]             Afficher les n meilleures suggestions (défaut: 10)\n"
           "  count                   Afficher le nombre de mots restants\n"
           "  reset                   Recharger la liste complète\n"
           "\n"
           "--- Filtres ---\n"
           "  include <lettres>       Garder les mots qui contiennent ces lettres\n"
           "                          ex: include ae\n"
           "  exclude <lettres>       Supprimer les mots contenant ces lettres\n"
           "                          ex: exclude bcd\n"
           "  pos <lettre> <pos>      Garder les mots avec la lettre à la position (1-5)\n"
           "                          ex: pos a 1\n"
           "  misplace <lettre> <pos> Lettre présente mais pas à cette position (jaune)\n"
           "                          ex: misplace e 2\n"
           "  sub <substr>            Garder les mots contenant cette sous-chaîne\n"
           "                          ex: sub ain\n"
           "  result <mot> <code>     Appliquer un résultat Wordle complet\n"
           "                          code: v=vert o=orange x=gris\n"
           "                          ex: result adieu oxovx\n"
           "  quit                    Quitter le programme\n"
           "=============================================\n\n");
}



int main(int argc, char *argv[]) {
    const char *default_path = "../ressource/bdd_wordle.txt";
    const char *path = (argc >= 2) ? argv[1] : default_path;

    WordList full, current;

    if (wl_load(&full, path) < 0) {
        fprintf(stderr, "Impossible de charger la base de données : %s\n", path);
        return EXIT_FAILURE;
    }
    current = full;   

    printf("\n\n");
    printf("       WORDLE SOLVER         \n");
    printf("\n");
    printf("%d mots chargés depuis %s\n", full.count, path);
    print_help();

    char line[256];
    while (1) {
        printf("[%d mots] > ", current.count);
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) break;
        trim(line);
        if (line[0] == '\0') continue;

        
        char cmd[64] = {0}, arg1[64] = {0}, arg2[64] = {0};
        int n = sscanf(line, "%63s %63s %63s", cmd, arg1, arg2);
        to_lower_str(cmd);

       
        if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0 || strcmp(cmd, "q") == 0) {
            printf("Au revoir !\n");
            break;

        } else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0) {
            print_help();

        } else if (strcmp(cmd, "reset") == 0) {
            current = full;
            printf("Liste réinitialisée (%d mots).\n", current.count);

        } else if (strcmp(cmd, "count") == 0) {
            printf("%d mot(s) restant(s).\n", current.count);

        } else if (strcmp(cmd, "list") == 0) {
            wl_print(&current);

        } else if (strcmp(cmd, "suggest") == 0) {
            int nb = 10;
            if (n >= 2) nb = atoi(arg1);
            suggest(&current, nb);

        } else if (strcmp(cmd, "include") == 0) {
            if (n < 2) { printf("Usage: include <lettres>\n"); continue; }
            to_lower_str(arg1);
            int before = current.count;
            filter_include(&current, arg1);
            printf("Filtre 'include %s' : %d → %d mots\n", arg1, before, current.count);

        } else if (strcmp(cmd, "exclude") == 0) {
            if (n < 2) { printf("Usage: exclude <lettres>\n"); continue; }
            to_lower_str(arg1);
            int before = current.count;
            filter_exclude(&current, arg1);
            printf("Filtre 'exclude %s' : %d → %d mots\n", arg1, before, current.count);

        } else if (strcmp(cmd, "pos") == 0) {
            if (n < 3) { printf("Usage: pos <lettre> <position 1-5>\n"); continue; }
            int pos = atoi(arg2) - 1;
            if (pos < 0 || pos >= WORD_LEN) { printf("Position doit être entre 1 et 5.\n"); continue; }
            int before = current.count;
            filter_position(&current, arg1[0], pos);
            printf("Filtre 'pos %c %d' : %d → %d mots\n", arg1[0], pos+1, before, current.count);

        } else if (strcmp(cmd, "misplace") == 0) {
            if (n < 3) { printf("Usage: misplace <lettre> <position 1-5>\n"); continue; }
            int pos = atoi(arg2) - 1;
            if (pos < 0 || pos >= WORD_LEN) { printf("Position doit être entre 1 et 5.\n"); continue; }
            int before = current.count;
            filter_misplaced(&current, arg1[0], pos);
            printf("Filtre 'misplace %c %d' : %d → %d mots\n", arg1[0], pos+1, before, current.count);

        } else if (strcmp(cmd, "sub") == 0) {
            if (n < 2) { printf("Usage: sub <sous-chaîne>\n"); continue; }
            to_lower_str(arg1);
            int before = current.count;
            filter_substring(&current, arg1);
            printf("Filtre 'sub %s' : %d → %d mots\n", arg1, before, current.count);

        } else if (strcmp(cmd, "result") == 0) {
            if (n < 3 || strlen(arg1) != WORD_LEN || strlen(arg2) != WORD_LEN) {
                printf("Usage: result <mot-5-lettres> <code-5-chars>\n");
                printf("       code: v=vert(correct) o=orange(mal placé) x=gris(absent)\n");
                printf("       ex:   result adieu oxovx\n");
                continue;
            }
            to_lower_str(arg1); to_lower_str(arg2);
            int before = current.count;
            filter_wordle_result(&current, arg1, arg2);
            printf("Résultat '%s' / '%s' : %d → %d mots\n", arg1, arg2, before, current.count);
            if (current.count <= 10)
                suggest(&current, 10);

        } else {
            printf("Commande inconnue : '%s'. Tapez 'help' pour l'aide.\n", cmd);
        }
    }

    return EXIT_SUCCESS;
}
