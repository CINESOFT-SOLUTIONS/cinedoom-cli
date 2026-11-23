#ifndef INGRESSOS_H
#define INGRESSOS_H

#include "tipos.h"

NoIngresso *carregarIngressos(NoSessao *sessoes);
void        salvarIngressos(NoIngresso *lista);
void        liberarIngressos(NoIngresso *lista);

void venderIngresso(NoIngresso **lista, NoSessao *sessoes, NoFilme *filmes);
void cancelarIngresso(NoIngresso **lista, NoSessao *sessoes);
void listarIngressosDaSessao(NoIngresso *lista, NoSessao *sessoes, NoFilme *filmes);

int sessaoTemIngresso(int cod_sessao);

#endif
