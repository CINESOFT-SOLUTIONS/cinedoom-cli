#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tipos.h"
#include "../include/filmes.h"
#include "../include/utils.h"

#define ARQUIVO_FILMES "dados/filmes.bin"

static int proximoCodigoFilme = 1;

/* ── Persistência ───────────────────────────────────────────────────────── */

typedef struct {
    int  codigo;
    char titulo[101];
    int  genero;
    int  duracao;
    int  classificacao;
    char descricao[301];
} FilmeReg;

NoFilme *carregarFilmes() {
    FILE *f = fopen(ARQUIVO_FILMES, "rb");
    if (!f) return NULL;

    NoFilme *lista = NULL;
    NoFilme *ultimo = NULL;
    FilmeReg reg;
    int maior = 0;

    while (fread(&reg, sizeof(FilmeReg), 1, f) == 1) {
        NoFilme *novo = (NoFilme *)malloc(sizeof(NoFilme));
        novo->dado.codigo        = reg.codigo;
        strcpy(novo->dado.titulo, reg.titulo);
        novo->dado.genero        = (Genero)reg.genero;
        novo->dado.duracao       = reg.duracao;
        novo->dado.classificacao = (Classificacao)reg.classificacao;
        strcpy(novo->dado.descricao, reg.descricao);
        novo->prox = NULL;

        if (!lista) {
            lista = novo;
        } else {
            ultimo->prox = novo;
        }
        ultimo = novo;

        if (reg.codigo > maior) maior = reg.codigo;
    }

    fclose(f);
    proximoCodigoFilme = maior + 1;
    return lista;
}

void salvarFilmes(NoFilme *lista) {
    FILE *f = fopen(ARQUIVO_FILMES, "wb");
    if (!f) {
        printf("Erro ao salvar filmes.\n");
        return;
    }

    NoFilme *atual = lista;
    while (atual) {
        FilmeReg reg;
        reg.codigo        = atual->dado.codigo;
        strcpy(reg.titulo, atual->dado.titulo);
        reg.genero        = (int)atual->dado.genero;
        reg.duracao       = atual->dado.duracao;
        reg.classificacao = (int)atual->dado.classificacao;
        strcpy(reg.descricao, atual->dado.descricao);
        fwrite(&reg, sizeof(FilmeReg), 1, f);
        atual = atual->prox;
    }

    fclose(f);
}

void liberarFilmes(NoFilme *lista) {
    while (lista) {
        NoFilme *tmp = lista;
        lista = lista->prox;
        free(tmp);
    }
}

/* ── Busca ──────────────────────────────────────────────────────────────── */

NoFilme *buscarFilmePorCodigo(NoFilme *lista, int codigo) {
    NoFilme *atual = lista;
    while (atual) {
        if (atual->dado.codigo == codigo) return atual;
        atual = atual->prox;
    }
    return NULL;
}

/* ── Helpers de exibição ────────────────────────────────────────────────── */

static const char *nomeGenero(Genero g) {
    switch (g) {
        case ACAO:             return "Acao";
        case COMEDIA:          return "Comedia";
        case DRAMA:            return "Drama";
        case TERROR:           return "Terror";
        case ANIMACAO:         return "Animacao";
        case ROMANCE:          return "Romance";
        case FICCAO_CIENTIFICA: return "Ficcao Cientifica";
        case DOCUMENTARIO:     return "Documentario";
        default:               return "Desconhecido";
    }
}

static const char *nomeClassificacao(Classificacao c) {
    switch (c) {
        case LIVRE:     return "Livre";
        case DEZ:       return "10 anos";
        case DOZE:      return "12 anos";
        case QUATORZE:  return "14 anos";
        case DEZESSEIS: return "16 anos";
        case DEZOITO:   return "18 anos";
        default:        return "Desconhecida";
    }
}

static void exibirFilme(Filme f) {
    printf("\n  Codigo:         %d\n", f.codigo);
    printf("  Titulo:         %s\n",  f.titulo);
    printf("  Genero:         %s\n",  nomeGenero(f.genero));
    printf("  Duracao:        %d min\n", f.duracao);
    printf("  Classificacao:  %s\n",  nomeClassificacao(f.classificacao));
    if (strlen(f.descricao) > 0)
        printf("  Descricao:      %s\n", f.descricao);
}

static void menuGenero(Genero *g) {
    printf("  Genero:\n");
    printf("  [0] Acao\n");
    printf("  [1] Comedia\n");
    printf("  [2] Drama\n");
    printf("  [3] Terror\n");
    printf("  [4] Animacao\n");
    printf("  [5] Romance\n");
    printf("  [6] Ficcao Cientifica\n");
    printf("  [7] Documentario\n");
    printf("  Opcao: ");
    int op = lerInt();
    if (op < 0 || op > 7) op = 0;
    *g = (Genero)op;
}

static void menuClassificacao(Classificacao *c) {
    printf("  Classificacao indicativa:\n");
    printf("  [0] Livre\n");
    printf("  [1] 10 anos\n");
    printf("  [2] 12 anos\n");
    printf("  [3] 14 anos\n");
    printf("  [4] 16 anos\n");
    printf("  [5] 18 anos\n");
    printf("  Opcao: ");
    int op = lerInt();
    const int valores[] = {0, 10, 12, 14, 16, 18};
    if (op < 0 || op > 5) op = 0;
    *c = (Classificacao)valores[op];
}

/* ── CRUD ───────────────────────────────────────────────────────────────── */

void cadastrarFilme(NoFilme **lista) {
    limparTela();
    printf("=== Cadastrar Filme ===\n\n");

    Filme f;
    f.codigo = proximoCodigoFilme;

    printf("  Titulo: ");
    lerString(f.titulo, sizeof(f.titulo));
    if (strlen(f.titulo) == 0) {
        printf("Titulo nao pode ser vazio.\n");
        pausar();
        return;
    }

    menuGenero(&f.genero);

    printf("  Duracao (min): ");
    f.duracao = lerInt();
    if (f.duracao < 30 || f.duracao > 360) {
        printf("Duracao invalida. Deve ser entre 30 e 360 minutos.\n");
        pausar();
        return;
    }

    menuClassificacao(&f.classificacao);

    printf("  Descricao (opcional): ");
    lerString(f.descricao, sizeof(f.descricao));

    NoFilme *novo = (NoFilme *)malloc(sizeof(NoFilme));
    novo->dado = f;
    novo->prox = NULL;

    /* insere ordenado por codigo */
    if (!*lista || (*lista)->dado.codigo > f.codigo) {
        novo->prox = *lista;
        *lista = novo;
    } else {
        NoFilme *atual = *lista;
        while (atual->prox && atual->prox->dado.codigo < f.codigo)
            atual = atual->prox;
        novo->prox = atual->prox;
        atual->prox = novo;
    }

    proximoCodigoFilme++;
    printf("\nFilme cadastrado com sucesso!\n");
    pausar();
}

void listarFilmes(NoFilme *lista) {
    limparTela();
    printf("=== Filmes ===\n\n");

    if (!lista) {
        printf("Nenhum filme cadastrado.\n");
        pausar();
        return;
    }

    NoFilme *atual = lista;
    int i = 1;
    while (atual) {
        printf("  [%d] %s (%d min) — %s\n",
            i, atual->dado.titulo, atual->dado.duracao,
            nomeGenero(atual->dado.genero));
        atual = atual->prox;
        i++;
    }

    printf("\n  [0] Voltar\n  Opcao: ");
    int op = lerInt();
    if (op == 0) return;

    /* navega ate o filme escolhido */
    atual = lista;
    for (int j = 1; j < op && atual; j++)
        atual = atual->prox;

    if (!atual) {
        printf("Opcao invalida.\n");
        pausar();
        return;
    }

    limparTela();
    printf("=== Detalhes do Filme ===\n");
    exibirFilme(atual->dado);

    printf("\n  [1] Atualizar\n  [2] Excluir\n  [0] Voltar\n  Opcao: ");
    int acao = lerInt();
    if (acao == 1) atualizarFilme(lista);  /* passa lista pra buscar por codigo */
    /* excluir sera chamado pelo main com ponteiro duplo */
}

void atualizarFilme(NoFilme *lista) {
    limparTela();
    printf("=== Atualizar Filme ===\n\n");
    printf("  Codigo do filme: ");
    int codigo = lerInt();

    NoFilme *no = buscarFilmePorCodigo(lista, codigo);
    if (!no) {
        printf("Filme nao encontrado.\n");
        pausar();
        return;
    }

    /* RN09: nao altera duracao se tiver sessoes */
    if (filmeTemSessao(codigo)) {
        printf("  (!) Este filme possui sessoes — duracao nao pode ser alterada.\n\n");
    }

    printf("  Novo titulo [%s]: ", no->dado.titulo);
    char tmp[101];
    lerString(tmp, sizeof(tmp));
    if (strlen(tmp) > 0) strcpy(no->dado.titulo, tmp);

    menuGenero(&no->dado.genero);

    if (!filmeTemSessao(codigo)) {
        printf("  Nova duracao (min) [%d]: ", no->dado.duracao);
        int dur = lerInt();
        if (dur >= 30 && dur <= 360) no->dado.duracao = dur;
    }

    menuClassificacao(&no->dado.classificacao);

    printf("  Nova descricao [%s]: ", no->dado.descricao);
    lerString(tmp, sizeof(tmp));
    if (strlen(tmp) > 0) strcpy(no->dado.descricao, tmp);

    printf("\nFilme atualizado com sucesso!\n");
    pausar();
}

void excluirFilme(NoFilme **lista) {
    limparTela();
    printf("=== Excluir Filme ===\n\n");
    printf("  Codigo do filme: ");
    int codigo = lerInt();

    /* RN01 */
    if (filmeTemSessao(codigo)) {
        printf("Nao e possivel excluir: filme possui sessoes cadastradas.\n");
        pausar();
        return;
    }

    NoFilme *atual = *lista;
    NoFilme *anterior = NULL;

    while (atual && atual->dado.codigo != codigo) {
        anterior = atual;
        atual = atual->prox;
    }

    if (!atual) {
        printf("Filme nao encontrado.\n");
        pausar();
        return;
    }

    if (!anterior) {
        *lista = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }

    free(atual);
    printf("Filme excluido com sucesso!\n");
    pausar();
}

/* ── Stub: implementado em sessoes.c ────────────────────────────────────── */
int __attribute__((weak)) filmeTemSessao(int cod_filme) {
    (void)cod_filme;
    return 0;
}
