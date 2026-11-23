#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/tipos.h"
#include "../include/sessoes.h"
#include "../include/filmes.h"
#include "../include/utils.h"

#define ARQUIVO_SESSOES "dados/sessoes.bin"

int proximoCodigoSessao = 1;

/* ── Persistência ───────────────────────────────────────────────────────── */

typedef struct {
    int codigo;
    int cod_filme;
    int id_sala;
    int dia, mes, ano;
    int hora, minuto;
} SessaoReg;

NoSessao *carregarSessoes(NoFilme *filmes) {
    FILE *f = fopen(ARQUIVO_SESSOES, "rb");
    if (!f) return NULL;

    NoSessao *lista = NULL;
    SessaoReg reg;
    int maior = 0;

    while (fread(&reg, sizeof(SessaoReg), 1, f) == 1) {
        NoSessao *novo = (NoSessao *)malloc(sizeof(NoSessao));
        novo->dado.codigo    = reg.codigo;
        novo->dado.cod_filme = reg.cod_filme;
        novo->dado.id_sala   = reg.id_sala;
        novo->dado.dia       = reg.dia;
        novo->dado.mes       = reg.mes;
        novo->dado.ano       = reg.ano;
        novo->dado.hora      = reg.hora;
        novo->dado.minuto    = reg.minuto;
        memset(novo->dado.assentos, 0, sizeof(novo->dado.assentos));
        novo->ant  = NULL;
        novo->prox = NULL;

        /* insere ordenado por data/hora */
        if (!lista) {
            lista = novo;
        } else {
            NoSessao *atual = lista;
            NoSessao *anterior = NULL;
            while (atual) {
                if (reg.ano < atual->dado.ano ||
                   (reg.ano == atual->dado.ano && reg.mes < atual->dado.mes) ||
                   (reg.ano == atual->dado.ano && reg.mes == atual->dado.mes && reg.dia < atual->dado.dia) ||
                   (reg.ano == atual->dado.ano && reg.mes == atual->dado.mes && reg.dia == atual->dado.dia &&
                    (reg.hora * 60 + reg.minuto) < (atual->dado.hora * 60 + atual->dado.minuto))) {
                    break;
                }
                anterior = atual;
                atual = atual->prox;
            }
            novo->prox = atual;
            novo->ant  = anterior;
            if (anterior) anterior->prox = novo;
            else lista = novo;
            if (atual) atual->ant = novo;
        }

        if (reg.codigo > maior) maior = reg.codigo;
    }

    fclose(f);
    proximoCodigoSessao = maior + 1;

    /* remonta matrizes de assentos a partir dos ingressos — feito em ingressos.c */
    return lista;
}

void salvarSessoes(NoSessao *lista) {
    FILE *f = fopen(ARQUIVO_SESSOES, "wb");
    if (!f) {
        printf("Erro ao salvar sessoes.\n");
        return;
    }

    NoSessao *atual = lista;
    while (atual) {
        SessaoReg reg;
        reg.codigo    = atual->dado.codigo;
        reg.cod_filme = atual->dado.cod_filme;
        reg.id_sala   = atual->dado.id_sala;
        reg.dia       = atual->dado.dia;
        reg.mes       = atual->dado.mes;
        reg.ano       = atual->dado.ano;
        reg.hora      = atual->dado.hora;
        reg.minuto    = atual->dado.minuto;
        fwrite(&reg, sizeof(SessaoReg), 1, f);
        atual = atual->prox;
    }

    fclose(f);
}

void liberarSessoes(NoSessao *lista) {
    while (lista) {
        NoSessao *tmp = lista;
        lista = lista->prox;
        free(tmp);
    }
}

/* ── Busca ──────────────────────────────────────────────────────────────── */

NoSessao *buscarSessaoPorCodigo(NoSessao *lista, int codigo) {
    NoSessao *atual = lista;
    while (atual) {
        if (atual->dado.codigo == codigo) return atual;
        atual = atual->prox;
    }
    return NULL;
}

int filmeTemSessao(int cod_filme) {
    NoSessao *atual = listaSessoes;
    while (atual) {
        if (atual->dado.cod_filme == cod_filme) return 1;
        atual = atual->prox;
    }
    return 0;
}

/* funcoes auxiliares */

int hojeEmMinutos(int *diaH, int *mesH, int *anoH) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    *diaH = tm->tm_mday;
    *mesH = tm->tm_mon + 1;
    *anoH = tm->tm_year + 1900;
    return tm->tm_hour * 60 + tm->tm_min;
}

int sessaoNoPassado(Sessao s) {
    int diaH, mesH, anoH, agoraMin;
    agoraMin = hojeEmMinutos(&diaH, &mesH, &anoH);

    if (s.ano < anoH) return 1;
    if (s.ano == anoH && s.mes < mesH) return 1;
    if (s.ano == anoH && s.mes == mesH && s.dia < diaH) return 1;
    if (s.ano == anoH && s.mes == mesH && s.dia == diaH &&
        (s.hora * 60 + s.minuto) <= agoraMin) return 1;
    return 0;
}

int haConflito(NoSessao *lista, int id_sala, int dia, int mes, int ano,
                      int hora, int minuto, int duracao, int ignorarCodigo) {
    int inicioNovo = hora * 60 + minuto;
    int fimNovo    = inicioNovo + duracao;

    NoSessao *atual = lista;
    while (atual) {
        if (atual->dado.codigo == ignorarCodigo) {
            atual = atual->prox;
            continue;
        }
        if (atual->dado.id_sala == id_sala &&
            atual->dado.dia == dia &&
            atual->dado.mes == mes &&
            atual->dado.ano == ano) {

            NoFilme *nf = buscarFilmePorCodigo(listaFilmes, atual->dado.cod_filme);
            int durExist = nf ? nf->dado.duracao : 120;

            int inicioExist = atual->dado.hora * 60 + atual->dado.minuto;
            int fimExist    = inicioExist + durExist;

            if (inicioNovo < fimExist && inicioExist < fimNovo) return 1;
        }
        atual = atual->prox;
    }
    return 0;
}

void exibirSessao(Sessao s, NoFilme *filmes) {
    NoFilme *nf = buscarFilmePorCodigo(filmes, s.cod_filme);
    char *titulo = nf ? nf->dado.titulo : "Filme desconhecido";
    int fim = s.hora * 60 + s.minuto + (nf ? nf->dado.duracao : 0);

    printf("\n  Codigo:  %d\n", s.codigo);
    printf("  Filme:   %s\n",   titulo);
    printf("  Sala:    %s\n",   SALAS[s.id_sala - 1].nome);
    printf("  Data:    %02d/%02d/%04d\n", s.dia, s.mes, s.ano);
    printf("  Inicio:  %02d:%02d\n", s.hora, s.minuto);
    printf("  Termino: %02d:%02d\n", fim / 60, fim % 60);
}

int lerSala() {
    printf("  Sala:\n");
    for (int i = 0; i < NUM_SALAS; i++)
        printf("  [%d] %s (%d assentos)\n", i + 1, SALAS[i].nome, SALAS[i].fileiras * SALAS[i].colunas);
    printf("  Opcao: ");
    int op = lerInt();
    if (op < 1 || op > NUM_SALAS) return -1;
    return op;
}

/* ── CRUD ───────────────────────────────────────────────────────────────── */

void cadastrarSessao(NoSessao **lista, NoFilme *filmes) {
    limparTela();
    printf("=== Cadastrar Sessao ===\n\n");

    printf("  Codigo do filme: ");
    int cod_filme = lerInt();
    NoFilme *nf = buscarFilmePorCodigo(filmes, cod_filme);
    if (!nf) {
        printf("Filme nao encontrado.\n");
        pausar();
        return;
    }

    int id_sala = lerSala();
    if (id_sala < 0) {
        printf("Sala invalida.\n");
        pausar();
        return;
    }

    printf("  Data (dd mm aaaa): ");
    int dia = lerInt(), mes = lerInt(), ano = lerInt();

    printf("  Horario de inicio (hh mm): ");
    int hora = lerInt(), minuto = lerInt();

    /* RN12: horario entre 06:00 e 01:00 */
    int inicioMin = hora * 60 + minuto;
    if (inicioMin < 360 && inicioMin > 60) {
        printf("Horario fora do intervalo permitido (06:00 ate 01:00).\n");
        pausar();
        return;
    }

    /* RN06: nao cadastrar no passado */
    Sessao tmp = {0, cod_filme, id_sala, dia, mes, ano, hora, minuto};
    if (sessaoNoPassado(tmp)) {
        printf("Nao e possivel cadastrar sessao no passado.\n");
        pausar();
        return;
    }

    /* RN02: conflito de horario */
    if (haConflito(*lista, id_sala, dia, mes, ano, hora, minuto, nf->dado.duracao, -1)) {
        printf("Conflito de horario nesta sala.\n");
        pausar();
        return;
    }

    NoSessao *novo = (NoSessao *)malloc(sizeof(NoSessao));
    novo->dado.codigo    = proximoCodigoSessao;
    novo->dado.cod_filme = cod_filme;
    novo->dado.id_sala   = id_sala;
    novo->dado.dia       = dia;
    novo->dado.mes       = mes;
    novo->dado.ano       = ano;
    novo->dado.hora      = hora;
    novo->dado.minuto    = minuto;
    memset(novo->dado.assentos, 0, sizeof(novo->dado.assentos));
    novo->ant  = NULL;
    novo->prox = NULL;

    /* insere ordenado por data/hora */
    if (!*lista) {
        *lista = novo;
    } else {
        NoSessao *atual = *lista;
        NoSessao *anterior = NULL;
        while (atual) {
            int minAtual = atual->dado.hora * 60 + atual->dado.minuto;
            if (ano < atual->dado.ano ||
               (ano == atual->dado.ano && mes < atual->dado.mes) ||
               (ano == atual->dado.ano && mes == atual->dado.mes && dia < atual->dado.dia) ||
               (ano == atual->dado.ano && mes == atual->dado.mes && dia == atual->dado.dia && inicioMin < minAtual))
                break;
            anterior = atual;
            atual = atual->prox;
        }
        novo->prox = atual;
        novo->ant  = anterior;
        if (anterior) anterior->prox = novo;
        else *lista = novo;
        if (atual) atual->ant = novo;
    }

    proximoCodigoSessao++;
    printf("\nSessao cadastrada com sucesso!\n");
    pausar();
}

void listarSessoes(NoSessao *lista, NoFilme *filmes) {
    limparTela();
    printf("=== Sessoes ===\n\n");

    if (!lista) {
        printf("Nenhuma sessao cadastrada.\n");
        pausar();
        return;
    }

    NoSessao *atual = lista;
    int i = 1;
    while (atual) {
        NoFilme *nf = buscarFilmePorCodigo(filmes, atual->dado.cod_filme);
        char *titulo = nf ? nf->dado.titulo : "?";
        printf("  [%d] %02d/%02d/%04d %02d:%02d — %s (%s)\n",
            i, atual->dado.dia, atual->dado.mes, atual->dado.ano,
            atual->dado.hora, atual->dado.minuto,
            titulo, SALAS[atual->dado.id_sala - 1].nome);
        atual = atual->prox;
        i++;
    }

    printf("\n  [0] Voltar\n  Opcao: ");
    int op = lerInt();
    if (op == 0) return;

    atual = lista;
    for (int j = 1; j < op && atual; j++)
        atual = atual->prox;

    if (!atual) {
        printf("Opcao invalida.\n");
        pausar();
        return;
    }

    limparTela();
    printf("=== Detalhes da Sessao ===\n");
    exibirSessao(atual->dado, filmes);

    printf("\n  [1] Atualizar\n  [2] Excluir\n  [0] Voltar\n  Opcao: ");
    int acao = lerInt();
    if (acao == 1) atualizarSessao(lista, filmes);
}

void atualizarSessao(NoSessao *lista, NoFilme *filmes) {
    limparTela();
    printf("=== Atualizar Sessao ===\n\n");
    printf("  Codigo da sessao: ");
    int codigo = lerInt();

    NoSessao *no = buscarSessaoPorCodigo(lista, codigo);
    if (!no) {
        printf("Sessao nao encontrada.\n");
        pausar();
        return;
    }

    /* RN10: nao altera sala/data/hora se tiver ingressos */
    int temIngresso = sessaoTemIngresso(codigo);
    if (temIngresso)
        printf("  (!) Esta sessao possui ingressos — sala, data e horario nao podem ser alterados.\n\n");

    printf("  Novo codigo do filme [%d]: ", no->dado.cod_filme);
    int cod_filme = lerInt();
    NoFilme *nf = buscarFilmePorCodigo(filmes, cod_filme);
    if (!nf) {
        printf("Filme nao encontrado.\n");
        pausar();
        return;
    }
    no->dado.cod_filme = cod_filme;

    if (!temIngresso) {
        int id_sala = lerSala();
        if (id_sala > 0) no->dado.id_sala = id_sala;

        printf("  Nova data (dd mm aaaa) [%02d %02d %04d]: ",
            no->dado.dia, no->dado.mes, no->dado.ano);
        int dia = lerInt(), mes = lerInt(), ano = lerInt();

        printf("  Novo horario (hh mm) [%02d %02d]: ", no->dado.hora, no->dado.minuto);
        int hora = lerInt(), minuto = lerInt();

        Sessao tmp = {0, cod_filme, no->dado.id_sala, dia, mes, ano, hora, minuto};
        if (sessaoNoPassado(tmp)) {
            printf("Nao e possivel definir sessao no passado.\n");
            pausar();
            return;
        }

        if (haConflito(lista, no->dado.id_sala, dia, mes, ano, hora, minuto, nf->dado.duracao, codigo)) {
            printf("Conflito de horario nesta sala.\n");
            pausar();
            return;
        }

        no->dado.dia    = dia;
        no->dado.mes    = mes;
        no->dado.ano    = ano;
        no->dado.hora   = hora;
        no->dado.minuto = minuto;
    }

    printf("\nSessao atualizada com sucesso!\n");
    pausar();
}

void excluirSessao(NoSessao **lista) {
    limparTela();
    printf("=== Excluir Sessao ===\n\n");
    printf("  Codigo da sessao: ");
    int codigo = lerInt();

    /* RN04 */
    if (sessaoTemIngresso(codigo)) {
        printf("Nao e possivel excluir: sessao possui ingressos vendidos.\n");
        pausar();
        return;
    }

    NoSessao *atual = *lista;
    while (atual && atual->dado.codigo != codigo)
        atual = atual->prox;

    if (!atual) {
        printf("Sessao nao encontrada.\n");
        pausar();
        return;
    }

    if (atual->ant) atual->ant->prox = atual->prox;
    else *lista = atual->prox;
    if (atual->prox) atual->prox->ant = atual->ant;

    free(atual);
    printf("Sessao excluida com sucesso!\n");
    pausar();
}

