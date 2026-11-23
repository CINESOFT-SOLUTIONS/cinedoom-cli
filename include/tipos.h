#ifndef TIPOS_H
#define TIPOS_H

/* ── Enums ─────────────────────────────────────────────────────────────── */

typedef enum {
    ACAO = 0,
    COMEDIA,
    DRAMA,
    TERROR,
    ANIMACAO,
    ROMANCE,
    FICCAO_CIENTIFICA,
    DOCUMENTARIO
} Genero;

typedef enum {
    LIVRE     =  0,
    DEZ       = 10,
    DOZE      = 12,
    QUATORZE  = 14,
    DEZESSEIS = 16,
    DEZOITO   = 18
} Classificacao;

typedef enum {
    INTEIRA      = 0,
    MEIA_ENTRADA = 1
} TipoIngresso;

/* ── Structs ────────────────────────────────────────────────────────────── */

typedef struct {
    int           codigo;
    char          titulo[101];
    Genero        genero;
    int           duracao;
    Classificacao classificacao;
    char          descricao[301];
} Filme;

typedef struct NoFilme {
    Filme           dado;
    struct NoFilme *prox;
} NoFilme;

typedef struct {
    int codigo;
    int cod_filme;
    int id_sala;
    int dia, mes, ano;
    int hora, minuto;
    int assentos[10][12];
} Sessao;

typedef struct NoSessao {
    Sessao           dado;
    struct NoSessao *ant;
    struct NoSessao *prox;
} NoSessao;

typedef struct {
    int          codigo;
    int          cod_sessao;
    int          fileira;
    int          coluna;
    char         nome_cliente[101];
    TipoIngresso tipo;
    float        valor;
} Ingresso;

typedef struct NoIngresso {
    Ingresso           dado;
    struct NoIngresso *prox;
} NoIngresso;

typedef struct {
    int  codigo;
    char nome[20];
    int  fileiras;
    int  colunas;
} Sala;

/* ── Salas fixas ────────────────────────────────────────────────────────── */

#define NUM_SALAS 4

static const Sala SALAS[NUM_SALAS] = {
    {1, "Sala 1",   10, 12},
    {2, "Sala 2",   10, 12},
    {3, "Sala 3",   10, 12},
    {4, "Sala VIP",  6, 10}
};

/* listas globais acessiveis por todos os modulos */
extern NoFilme    *listaFilmes;
extern NoSessao   *listaSessoes;
extern NoIngresso *listaIngressos;

#endif
