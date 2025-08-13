#include "famine.h"

void print_tab(char **tab) {
    printf("\nAll file modifie:\n");
    int i = 0;
    while (tab[i]) {
        printf("%s\n", tab[i]);
        i++;
    }
    return ;
}

void free_tab(char **tab, int index) {
    for (int i = 0; i < index; i++)
        free(tab[i]);
    free(tab);
}
