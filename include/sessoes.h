#ifndef SESSOES_H
#define SESSOES_H

#include "tipos.h"

NoSessao *carregarSessoes(NoFilme *filmes);
void      salvarSessoes(NoSessao *lista);
void      liberarSessoes(NoSessao *lista);

void cadastrarSessao(NoSessao **lista, NoFilme *filmes);
void listarSessoes(NoSessao *lista, NoFilme *filmes);
void atualizarSessao(NoSessao *lista, NoFilme *filmes);
void excluirSessao(NoSessao **lista);

NoSessao *buscarSessaoPorCodigo(NoSessao *lista, int codigo);
int       filmeTemSessao(int cod_filme);
int       sessaoTemIngresso(int cod_sessao);

#endif
