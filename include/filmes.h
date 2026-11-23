#ifndef FILMES_H
#define FILMES_H

#include "tipos.h"

NoFilme *carregarFilmes();
void     salvarFilmes(NoFilme *lista);
void     liberarFilmes(NoFilme *lista);

void cadastrarFilme(NoFilme **lista);
void listarFilmes(NoFilme *lista);
void atualizarFilme(NoFilme *lista);
void excluirFilme(NoFilme **lista);

NoFilme *buscarFilmePorCodigo(NoFilme *lista, int codigo);
int      filmeTemSessao(int cod_filme);

#endif
