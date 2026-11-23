#include <stdio.h>
#include <stdlib.h>
#include "../include/tipos.h"
#include "../include/utils.h"
#include "../include/filmes.h"
#include "../include/sessoes.h"
#include "../include/ingressos.h"

/* listas globais — acessadas via extern em sessoes.c e ingressos.c */
NoFilme   *listaFilmes   = NULL;
NoSessao  *listaSessoes  = NULL;
NoIngresso *listaIngressos = NULL;

/* ── Banner ─────────────────────────────────────────────────────────────── */

static void exibirBanner() {
    printf("\n");
    printf("  ==================\n");
    printf("    CINEDOOM CLI\n");
    printf("  ==================\n");
}

/* ── Menus ──────────────────────────────────────────────────────────────── */

static void menuFilmes() {
    int op;
    do {
        limparTela();
        exibirBanner();
        printf("  |                        FILMES                                |\n");
        printf("  +==============================================================+\n");
        printf("  |   [1] Cadastrar filme                                        |\n");
        printf("  |   [2] Listar filmes                                          |\n");
        printf("  |   [0] Voltar                                                 |\n");
        printf("  +==============================================================+\n");
        printf("  >> ");
        op = lerInt();
        switch (op) {
            case 1: cadastrarFilme(&listaFilmes); salvarFilmes(listaFilmes); break;
            case 2: listarFilmes(listaFilmes); break;
        }
    } while (op != 0);
}

static void menuSessoes() {
    int op;
    do {
        limparTela();
        exibirBanner();
        printf("  |                        SESSOES                               |\n");
        printf("  +==============================================================+\n");
        printf("  |   [1] Cadastrar sessao                                       |\n");
        printf("  |   [2] Listar sessoes                                         |\n");
        printf("  |   [0] Voltar                                                 |\n");
        printf("  +==============================================================+\n");
        printf("  >> ");
        op = lerInt();
        switch (op) {
            case 1: cadastrarSessao(&listaSessoes, listaFilmes); salvarSessoes(listaSessoes); break;
            case 2: listarSessoes(listaSessoes, listaFilmes); break;
        }
    } while (op != 0);
}

static void menuSalas() {
    limparTela();
    exibirBanner();
    printf("  |                         SALAS                                |\n");
    printf("  +==============================================================+\n\n");
    for (int i = 0; i < NUM_SALAS; i++)
        printf("  %s — %d fileiras x %d colunas (%d assentos)\n",
            SALAS[i].nome, SALAS[i].fileiras, SALAS[i].colunas,
            SALAS[i].fileiras * SALAS[i].colunas);
    pausar();
}

static void menuIngressos() {
    int op;
    do {
        limparTela();
        exibirBanner();
        printf("  |                       INGRESSOS                              |\n");
        printf("  +==============================================================+\n");
        printf("  |   [1] Vender ingresso                                        |\n");
        printf("  |   [2] Cancelar ingresso                                      |\n");
        printf("  |   [3] Listar ingressos de uma sessao                         |\n");
        printf("  |   [0] Voltar                                                 |\n");
        printf("  +==============================================================+\n");
        printf("  >> ");
        op = lerInt();
        switch (op) {
            case 1:
                venderIngresso(&listaIngressos, listaSessoes, listaFilmes);
                salvarIngressos(listaIngressos);
                break;
            case 2:
                cancelarIngresso(&listaIngressos, listaSessoes);
                salvarIngressos(listaIngressos);
                break;
            case 3:
                listarIngressosDaSessao(listaIngressos, listaSessoes, listaFilmes);
                break;
        }
    } while (op != 0);
}

/* ── Main ───────────────────────────────────────────────────────────────── */

int main() {
    /* garante que a pasta de dados existe */
#ifdef _WIN32
    system("mkdir dados 2>nul");
#else
    system("mkdir -p dados");
#endif

    /* carrega dados do disco */
    listaFilmes    = carregarFilmes();
    listaSessoes   = carregarSessoes(listaFilmes);
    listaIngressos = carregarIngressos(listaSessoes);

    int op;
    do {
        limparTela();
        exibirBanner();
        printf("  |                     MENU PRINCIPAL                           |\n");
        printf("  +==============================================================+\n");
        printf("  |   [1] Filmes                                                 |\n");
        printf("  |   [2] Sessoes                                                |\n");
        printf("  |   [3] Salas                                                  |\n");
        printf("  |   [4] Ingressos                                              |\n");
        printf("  |   [0] Sair                                                   |\n");
        printf("  +==============================================================+\n");
        printf("  >> ");
        op = lerInt();
        switch (op) {
            case 1: menuFilmes();   break;
            case 2: menuSessoes();  break;
            case 3: menuSalas();    break;
            case 4: menuIngressos(); break;
            case 0:
                limparTela();
                printf("\n  Obrigado por usar o CineDoom!\n\n");
                break;
            default:
                printf("  Opcao invalida.\n");
                pausar();
        }
    } while (op != 0);

    /* libera memoria */
    liberarFilmes(listaFilmes);
    liberarSessoes(listaSessoes);
    liberarIngressos(listaIngressos);

    return 0;
}
