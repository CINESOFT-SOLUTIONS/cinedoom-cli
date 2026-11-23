#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/tipos.h"
#include "../include/ingressos.h"
#include "../include/sessoes.h"
#include "../include/filmes.h"
#include "../include/utils.h"

#define ARQUIVO_INGRESSOS "dados/ingressos.bin"

int proximoCodigoIngresso = 1;

/* ── Persistência ───────────────────────────────────────────────────────── */

typedef struct {
    int   codigo;
    int   cod_sessao;
    int   fileira;
    int   coluna;
    char  nome_cliente[101];
    int   tipo;
    float valor;
} IngressoReg;

NoIngresso *carregarIngressos(NoSessao *sessoes) {
    FILE *f = fopen(ARQUIVO_INGRESSOS, "rb");
    if (!f) return NULL;

    NoIngresso *lista = NULL;
    NoIngresso *ultimo = NULL;
    IngressoReg reg;
    int maior = 0;

    while (fread(&reg, sizeof(IngressoReg), 1, f) == 1) {
        NoIngresso *novo = (NoIngresso *)malloc(sizeof(NoIngresso));
        novo->dado.codigo       = reg.codigo;
        novo->dado.cod_sessao   = reg.cod_sessao;
        novo->dado.fileira      = reg.fileira;
        novo->dado.coluna       = reg.coluna;
        strcpy(novo->dado.nome_cliente, reg.nome_cliente);
        novo->dado.tipo         = (TipoIngresso)reg.tipo;
        novo->dado.valor        = reg.valor;
        novo->prox = NULL;

        if (!lista) lista = novo;
        else ultimo->prox = novo;
        ultimo = novo;

        /* remonta matriz de assentos da sessao correspondente */
        NoSessao *ns = buscarSessaoPorCodigo(sessoes, reg.cod_sessao);
        if (ns) ns->dado.assentos[reg.fileira][reg.coluna] = 1;

        if (reg.codigo > maior) maior = reg.codigo;
    }

    fclose(f);
    proximoCodigoIngresso = maior + 1;
    return lista;
}

void salvarIngressos(NoIngresso *lista) {
    FILE *f = fopen(ARQUIVO_INGRESSOS, "wb");
    if (!f) {
        printf("Erro ao salvar ingressos.\n");
        return;
    }

    NoIngresso *atual = lista;
    while (atual) {
        IngressoReg reg;
        reg.codigo     = atual->dado.codigo;
        reg.cod_sessao = atual->dado.cod_sessao;
        reg.fileira    = atual->dado.fileira;
        reg.coluna     = atual->dado.coluna;
        strcpy(reg.nome_cliente, atual->dado.nome_cliente);
        reg.tipo  = (int)atual->dado.tipo;
        reg.valor = atual->dado.valor;
        fwrite(&reg, sizeof(IngressoReg), 1, f);
        atual = atual->prox;
    }

    fclose(f);
}

void liberarIngressos(NoIngresso *lista) {
    while (lista) {
        NoIngresso *tmp = lista;
        lista = lista->prox;
        free(tmp);
    }
}

/* ── Helpers ────────────────────────────────────────────────────────────── */

int sessaoTemIngresso(int cod_sessao) {
    NoIngresso *atual = listaIngressos;
    while (atual) {
        if (atual->dado.cod_sessao == cod_sessao) return 1;
        atual = atual->prox;
    }
    return 0;
}

int sessaoJaOcorreu(Sessao s) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    int anoH  = tm->tm_year + 1900;
    int mesH  = tm->tm_mon + 1;
    int diaH  = tm->tm_mday;
    int agoraMin = tm->tm_hour * 60 + tm->tm_min;

    if (s.ano < anoH) return 1;
    if (s.ano == anoH && s.mes < mesH) return 1;
    if (s.ano == anoH && s.mes == mesH && s.dia < diaH) return 1;
    if (s.ano == anoH && s.mes == mesH && s.dia == diaH &&
        (s.hora * 60 + s.minuto) <= agoraMin) return 1;
    return 0;
}

void exibirMapaAssentos(Sessao s) {
    Sala sala = SALAS[s.id_sala - 1];
    printf("\n  Mapa de assentos — %s\n\n", sala.nome);
    printf("     ");
    for (int c = 0; c < sala.colunas; c++) printf("%2d", c + 1);
    printf("\n");
    for (int f = 0; f < sala.fileiras; f++) {
        printf("  %2d ", f + 1);
        for (int c = 0; c < sala.colunas; c++)
            printf(" %c", s.assentos[f][c] ? 'X' : 'O');
        printf("\n");
    }
    printf("\n  O = livre   X = ocupado\n");
}

int salaLotada(Sessao s) {
    Sala sala = SALAS[s.id_sala - 1];
    for (int f = 0; f < sala.fileiras; f++)
        for (int c = 0; c < sala.colunas; c++)
            if (!s.assentos[f][c]) return 0;
    return 1;
}

/* ── Operações ──────────────────────────────────────────────────────────── */

void venderIngresso(NoIngresso **lista, NoSessao *sessoes, NoFilme *filmes) {
    limparTela();
    printf("=== Vender Ingresso ===\n\n");

    /* lista sessoes disponiveis */
    if (!sessoes) {
        printf("Nenhuma sessao cadastrada.\n");
        pausar();
        return;
    }

    NoSessao *ns = sessoes;
    int i = 1;
    while (ns) {
        NoFilme *nf = buscarFilmePorCodigo(filmes, ns->dado.cod_filme);
        char *titulo = nf ? nf->dado.titulo : "?";
        printf("  [%d] %02d/%02d/%04d %02d:%02d — %s (%s)\n",
            i, ns->dado.dia, ns->dado.mes, ns->dado.ano,
            ns->dado.hora, ns->dado.minuto,
            titulo, SALAS[ns->dado.id_sala - 1].nome);
        ns = ns->prox;
        i++;
    }

    printf("\n  [0] Voltar\n  Opcao: ");
    int op = lerInt();
    if (op == 0) return;

    ns = sessoes;
    for (int j = 1; j < op && ns; j++) ns = ns->prox;
    if (!ns) { printf("Opcao invalida.\n"); pausar(); return; }

    /* RN07: sessao ja ocorreu */
    if (sessaoJaOcorreu(ns->dado)) {
        printf("Esta sessao ja ocorreu.\n");
        pausar();
        return;
    }

    /* RN08: sala lotada */
    if (salaLotada(ns->dado)) {
        printf("Sala lotada.\n");
        pausar();
        return;
    }

    exibirMapaAssentos(ns->dado);

    Sala sala = SALAS[ns->dado.id_sala - 1];
    printf("  Fileira (1-%d): ", sala.fileiras);
    int fileira = lerInt() - 1;
    printf("  Coluna  (1-%d): ", sala.colunas);
    int coluna = lerInt() - 1;

    /* RN03: fora dos limites */
    if (fileira < 0 || fileira >= sala.fileiras || coluna < 0 || coluna >= sala.colunas) {
        printf("Assento fora dos limites da sala.\n");
        pausar();
        return;
    }

    /* RN03: ja ocupado */
    if (ns->dado.assentos[fileira][coluna]) {
        printf("Assento ja ocupado.\n");
        pausar();
        return;
    }

    printf("  Nome do cliente: ");
    char nome[101];
    lerString(nome, sizeof(nome));
    if (strlen(nome) == 0) { printf("Nome nao pode ser vazio.\n"); pausar(); return; }

    printf("  Tipo:\n  [0] Inteira  — R$ 30,00\n  [1] Meia     — R$ 15,00\n  Opcao: ");
    int tipo = lerInt();
    if (tipo < 0 || tipo > 1) tipo = 0;

    float valor = (tipo == INTEIRA) ? 30.0f : 15.0f;

    NoIngresso *novo = (NoIngresso *)malloc(sizeof(NoIngresso));
    novo->dado.codigo     = proximoCodigoIngresso;
    novo->dado.cod_sessao = ns->dado.codigo;
    novo->dado.fileira    = fileira;
    novo->dado.coluna     = coluna;
    strcpy(novo->dado.nome_cliente, nome);
    novo->dado.tipo  = (TipoIngresso)tipo;
    novo->dado.valor = valor;
    novo->prox = NULL;

    /* marca assento na matriz */
    ns->dado.assentos[fileira][coluna] = 1;

    /* insere no fim da lista */
    if (!*lista) {
        *lista = novo;
    } else {
        NoIngresso *atual = *lista;
        while (atual->prox) atual = atual->prox;
        atual->prox = novo;
    }

    proximoCodigoIngresso++;
    printf("\nIngresso vendido! Codigo: %d\n", novo->dado.codigo);
    pausar();
}

void cancelarIngresso(NoIngresso **lista, NoSessao *sessoes) {
    limparTela();
    printf("=== Cancelar Ingresso ===\n\n");
    printf("  Codigo do ingresso: ");
    int codigo = lerInt();

    NoIngresso *atual = *lista;
    NoIngresso *anterior = NULL;
    while (atual && atual->dado.codigo != codigo) {
        anterior = atual;
        atual = atual->prox;
    }

    if (!atual) { printf("Ingresso nao encontrado.\n"); pausar(); return; }

    /* RN13: sessao ja ocorreu */
    NoSessao *ns = buscarSessaoPorCodigo(sessoes, atual->dado.cod_sessao);
    if (ns && sessaoJaOcorreu(ns->dado)) {
        printf("Nao e possivel cancelar: sessao ja ocorreu.\n");
        pausar();
        return;
    }

    /* libera assento */
    if (ns) ns->dado.assentos[atual->dado.fileira][atual->dado.coluna] = 0;

    if (!anterior) *lista = atual->prox;
    else anterior->prox = atual->prox;

    free(atual);
    printf("Ingresso cancelado com sucesso!\n");
    pausar();
}

void listarIngressosDaSessao(NoIngresso *lista, NoSessao *sessoes, NoFilme *filmes) {
    limparTela();
    printf("=== Ingressos por Sessao ===\n\n");

    if (!sessoes) { printf("Nenhuma sessao cadastrada.\n"); pausar(); return; }

    NoSessao *ns = sessoes;
    int i = 1;
    while (ns) {
        NoFilme *nf = buscarFilmePorCodigo(filmes, ns->dado.cod_filme);
        printf("  [%d] %02d/%02d/%04d %02d:%02d — %s\n",
            i, ns->dado.dia, ns->dado.mes, ns->dado.ano,
            ns->dado.hora, ns->dado.minuto,
            nf ? nf->dado.titulo : "?");
        ns = ns->prox;
        i++;
    }

    printf("\n  [0] Voltar\n  Opcao: ");
    int op = lerInt();
    if (op == 0) return;

    ns = sessoes;
    for (int j = 1; j < op && ns; j++) ns = ns->prox;
    if (!ns) { printf("Opcao invalida.\n"); pausar(); return; }

    limparTela();
    NoFilme *nf = buscarFilmePorCodigo(filmes, ns->dado.cod_filme);
    printf("Ingressos — %s %02d/%02d/%04d %02d:%02d\n\n",
        nf ? nf->dado.titulo : "?",
        ns->dado.dia, ns->dado.mes, ns->dado.ano,
        ns->dado.hora, ns->dado.minuto);

    NoIngresso *atual = lista;
    int encontrou = 0;
    while (atual) {
        if (atual->dado.cod_sessao == ns->dado.codigo) {
            printf("  [%d] Fileira %d / Coluna %d — %s — %s — R$ %.2f\n",
                atual->dado.codigo,
                atual->dado.fileira + 1,
                atual->dado.coluna  + 1,
                atual->dado.nome_cliente,
                atual->dado.tipo == INTEIRA ? "Inteira" : "Meia",
                atual->dado.valor);
            encontrou = 1;
        }
        atual = atual->prox;
    }

    if (!encontrou) printf("  Nenhum ingresso vendido para esta sessao.\n");
    pausar();
}
