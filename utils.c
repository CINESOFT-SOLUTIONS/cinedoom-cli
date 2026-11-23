#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar() {
    printf("\nPressione Enter para continuar...");
    limparBuffer();
    getchar();
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerString(char *dest, int max) {
    fgets(dest, max, stdin);
    dest[strcspn(dest, "\n")] = '\0';
}

int lerInt() {
    int valor;
    while (scanf("%d", &valor) != 1) {
        limparBuffer();
        printf("Entrada invalida. Digite um numero: ");
    }
    limparBuffer();
    return valor;
}

float lerFloat() {
    float valor;
    while (scanf("%f", &valor) != 1) {
        limparBuffer();
        printf("Entrada invalida. Digite um numero: ");
    }
    limparBuffer();
    return valor;
}
