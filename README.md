# CineDoom - CLI

> Sistema de gerenciamento de cinema via linha de comando, desenvolvido em C como trabalho da disciplina de Algoritmos e Estruturas de Dados 1.

## Equipe

| # | Nome |
|---|------|
| 1 | Carlos Eduardo Freire da Silva |
| 2 | Gabriel Henrique Lourenço Dias |
| 3 | Kayque Vicente Silva |
| 4 | Yury Nogueira de Oliveira Silva |

- **Disciplina:** Algoritmos e Estruturas de Dados 1  
- **Professor:** Edmundo  
- **Instituição:** Universidade Federal de Goiás — Instituto de Informática  
- **Prazo:** Primeira quinzena de dezembro  
- **Peso:** 0.2 na média final  

---

## Especificação do Projeto

### Tema

Desenvolver um trabalho contendo TADs (Tipos Abstratos de Dados) definidas pelo grupo. O tema deve ser apresentado ao professor antes de iniciar o desenvolvimento.

### Objetivo

Desenvolver um projeto compreendendo uma área real de mercado, contendo:

- Pelo menos **7 funcionalidades** principais
- Armazenamento de dados em disco (persistência entre execuções)
- TADs bem definidas

---

## Funcionalidades

- [ ] Cadastro de filmes
- [ ] Cadastro de salas
- [ ] Cadastro de sessões
- [ ] Venda de ingressos
- [ ] Consulta de programação
- [ ] Cancelamento de reservas
- [ ] Relatórios de ocupação

---

## Progresso

**[0%]** — Em desenvolvimento

---

## Como compilar

```bash
gcc src/main.c src/filmes.c src/sessoes.c src/ingressos.c src/utils.c -I include -o cinedoom
./cinedoom
```
