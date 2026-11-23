# Especificação do Sistema — CineDoom CLI

## 0. Estrutura de Arquivos

```
cinedoom-cli/
├── include/
│   ├── tipos.h          ← enums, structs e constante SALAS (compartilhado por todos)
│   ├── utils.h
│   ├── filmes.h
│   ├── sessoes.h
│   └── ingressos.h
├── src/
│   ├── main.c           ← menu principal
│   ├── utils.c          ← funções auxiliares
│   ├── filmes.c
│   ├── sessoes.c
│   └── ingressos.c
├── dados/
│   ├── filmes.bin
│   ├── sessoes.bin
│   └── ingressos.bin
└── docs/
```

---

## 1. Funcionalidades

### A. Gerenciamento de Filmes

| ID  | Funcionalidade |
|-----|----------------|
| F01 | Cadastrar filme |
| F02 | Listar filmes (seleciona para ver detalhes) |
| F03 | Atualizar dados do filme |
| F04 | Excluir filme |

---

### B. Gerenciamento de Sessões

| ID  | Funcionalidade |
|-----|----------------|
| F05 | Cadastrar sessão (com validação de conflito de horário) |
| F06 | Listar sessões (seleciona para ver detalhes) |
| F07 | Atualizar dados da sessão |
| F08 | Excluir sessão |

---

### C. Gerenciamento de Salas (pré-definidas no código)

| ID  | Funcionalidade |
|-----|----------------|
| F09 | Listar salas disponíveis |

---

### D. Gerenciamento de Ingressos

| ID  | Funcionalidade |
|-----|----------------|
| F10 | Vender ingresso (escolhe sessão + assento, exibe mapa) |
| F11 | Cancelar ingresso (libera o assento) |
| F12 | Listar ingressos de uma sessão |

---

## 2. Fluxo de Menus

```
[1] Filmes
    [1] Cadastrar filme
    [2] Listar filmes
        → seleciona filme → exibe detalhes
            [1] Atualizar
            [2] Excluir
            [0] Voltar
    [0] Voltar

[2] Sessões
    [1] Cadastrar sessão
    [2] Listar sessões
        → seleciona sessão → exibe detalhes
            [1] Atualizar
            [2] Excluir
            [0] Voltar
    [0] Voltar

[3] Salas
    [1] Listar salas disponíveis
    [0] Voltar

[4] Ingressos
    [1] Vender ingresso
        → lista sessões → seleciona → exibe mapa de assentos → escolhe assento → confirma
    [2] Cancelar ingresso
        → lista sessões → seleciona → lista ingressos → seleciona → confirma
    [3] Listar ingressos de uma sessão
        → lista sessões → seleciona → exibe ingressos
    [0] Voltar

[0] Sair
```

---

## 3. Estruturas de Dados (TADs)

| Entidade | TAD | Justificativa |
|----------|-----|---------------|
| **Filmes** | Lista Simplesmente Encadeada Ordenada | Alocação dinâmica conforme o catálogo cresce; mantém listagem ordenada (código/título); permite validar existência antes de cadastrar sessão ou excluir filme. |
| **Sessões** | Lista Duplamente Encadeada Ordenada (por data/hora) | Grade cronológica; ponteiros anterior/próximo facilitam checar choque de horário na mesma sala e remover sessões sem varrer desde o início. |
| **Ingressos** | Lista Simplesmente Encadeada | Número imprevisível de vendas; insere no fim (venda), remove específico (cancelamento), filtra por sessão; sem desperdício de memória. |
| **Assentos** | Matriz de Ocupação (dentro de cada Sessão) | `fileiras × colunas` (0 = livre, 1 = ocupado); verificar/ocupar assento é O(1); facilita mapa visual na venda. |
| **Salas** | Vetor Estático Constante (`const`) | 4 salas fixas no código; acesso direto por índice O(1); sem necessidade de CRUD ou arquivos extras. |

---

## 4. Definição dos Dados e Structs C

> Fonte de verdade dos campos, tipos e enums. Structs prontos para uso no código.

---

### Filme

| Campo | Tipo C | Restrição |
|-------|--------|-----------|
| `codigo` | `int` | único, auto-incremento, > 0 |
| `titulo` | `char[101]` | não vazio |
| `genero` | `enum Genero` | ver enum abaixo |
| `duracao` | `int` | em minutos; entre 30 e 360 |
| `classificacao` | `enum Classificacao` | ver enum abaixo |
| `descricao` | `char[301]` | pode ser vazio |

```c
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
```

---

### Sessão

| Campo | Tipo C | Restrição |
|-------|--------|-----------|
| `codigo` | `int` | único, auto-incremento, > 0 |
| `cod_filme` | `int` | deve existir na lista de filmes |
| `id_sala` | `int` | 1 a 4 |
| `dia` | `int` | 1–31 |
| `mes` | `int` | 1–12 |
| `ano` | `int` | >= ano atual |
| `hora` | `int` | 0–23 |
| `minuto` | `int` | 0 ou 30 |

> `horario_fim` não é armazenado — calculado em tempo de execução: `inicio + duracao_do_filme`.

```c
typedef struct {
    int codigo;
    int cod_filme;
    int id_sala;
    int dia, mes, ano;
    int hora, minuto;
    int assentos[10][12]; /* 0 = livre, 1 = ocupado; dimensões máximas */
} Sessao;

typedef struct NoSessao {
    Sessao           dado;
    struct NoSessao *ant;
    struct NoSessao *prox;
} NoSessao;
```

> A matriz `assentos[10][12]` usa as dimensões máximas fixas para evitar alocação dinâmica. Apenas as posições dentro de `fileiras × colunas` da sala vinculada são válidas.

---

### Sala (constante no código)

```c
typedef struct {
    int  codigo;
    char nome[20];
    int  fileiras;
    int  colunas;
} Sala;

const Sala SALAS[4] = {
    {1, "Sala 1",   10, 12},  /* 120 assentos */
    {2, "Sala 2",   10, 12},  /* 120 assentos */
    {3, "Sala 3",   10, 12},  /* 120 assentos */
    {4, "Sala VIP",  6, 10}   /*  60 assentos */
};
```

---

### Ingresso

| Campo | Tipo C | Restrição |
|-------|--------|-----------|
| `codigo` | `int` | único, auto-incremento, > 0 |
| `cod_sessao` | `int` | deve existir na lista de sessões |
| `fileira` | `int` | 0 a `(fileiras - 1)` da sala |
| `coluna` | `int` | 0 a `(colunas - 1)` da sala |
| `nome_cliente` | `char[101]` | não vazio |
| `tipo` | `enum TipoIngresso` | ver enum abaixo |
| `valor` | `float` | calculado conforme tipo |

```c
typedef enum {
    INTEIRA      = 0,  /* R$ 30,00 */
    MEIA_ENTRADA = 1   /* R$ 15,00 */
} TipoIngresso;

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
```

> `valor` é derivado do tipo no momento da venda e gravado no `.bin` para fins de relatório futuro.

---

## 5. Regras de Negócio

| ID   | Regra |
|------|-------|
| RN01 | Não excluir filme que tenha sessão cadastrada. |
| RN02 | Não cadastrar/atualizar sessão em sala com horário sobreposto (fim = início + duração do filme). |
| RN03 | Não vender assento já ocupado nem fora dos limites da sala. |
| RN04 | Não excluir sessão que tenha ingressos vendidos (cancelá-los antes, com confirmação). |
| RN05 | Códigos de filme, sessão e ingresso não podem se repetir. |
| RN06 | Não cadastrar sessão com data/hora no passado. |
| RN07 | Não vender ingresso para sessão que já ocorreu. |
| RN08 | Não vender ingresso se a sala estiver com todos os assentos ocupados. |
| RN09 | Não alterar a duração de um filme que já possua sessões vinculadas (evita conflito retroativo de horário na grade). |
| RN10 | Não alterar a sala, data ou horário de uma sessão que já possua ingressos vendidos (evita assentos fora dos limites da nova sala). |
| RN11 | Não cadastrar filme com duração menor que 30 minutos ou maior que 360 minutos. |
| RN12 | Não cadastrar sessão com horário fora do intervalo 06:00–01:00 (madrugada do dia seguinte). |
| RN13 | Não cancelar ingresso de sessão que já ocorreu. |
| RN14 | Ao carregar os arquivos `.bin`, o próximo código gerado deve continuar do maior código existente + 1 (evita colisão com RN05 após reinício do programa). |

---

## 6. Persistência em Disco

| Arquivo | Conteúdo |
|---------|----------|
| `filmes.bin` | Registros de filmes sem ponteiros |
| `sessoes.bin` | Registros de sessões sem ponteiros nem matriz |
| `ingressos.bin` | Registros de ingressos sem ponteiros |

**Ordem de carregamento obrigatória:**
1. `filmes.bin` → reconstrói lista de filmes
2. `sessoes.bin` → reconstrói lista de sessões (referencia filme por código)
3. `ingressos.bin` → reconstrói lista de ingressos + remonta matrizes de assentos (referencia sessão por código)

> A matriz de assentos **não é gravada** — ela é remontada a partir dos ingressos no carregamento.

### Formato dos registros

```c
typedef struct {
    int  codigo;
    char titulo[101];
    int  genero;
    int  duracao;
    int  classificacao;
    char descricao[301];
} FilmeReg;               /* 418 bytes */

typedef struct {
    int codigo;
    int cod_filme;
    int id_sala;
    int dia, mes, ano;
    int hora, minuto;
} SessaoReg;              /* 32 bytes — sem a matriz */

typedef struct {
    int   codigo;
    int   cod_sessao;
    int   fileira;
    int   coluna;
    char  nome_cliente[101];
    int   tipo;
    float valor;
} IngressoReg;            /* 125 bytes */
```

---

## 7. Lógica de Conflito de Horário (RN02)

Duas sessões **conflitam** na mesma sala se seus intervalos se sobrepõem:

```
inicio_A = hora_A * 60 + minuto_A
fim_A    = inicio_A + duracao_filme_A

inicio_B = hora_B * 60 + minuto_B
fim_B    = inicio_B + duracao_filme_B

conflito = (inicio_A < fim_B) && (inicio_B < fim_A)
           && mesma sala && mesma data
```

> Intervalo considera apenas o dia — sessões em datas diferentes nunca conflitam. Adicionar 30 min de limpeza entre sessões é opcional: `fim_A + 30 > inicio_B`.
