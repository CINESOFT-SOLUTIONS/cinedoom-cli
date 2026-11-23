# Progresso — CineDoom CLI

## 17/11

Definimos o escopo: sistema de gerenciamento de cinema em C com 12 funcionalidades cobrindo filmes, sessões, salas e ingressos.

Escolhemos as estruturas de dados. Filmes e ingressos ficam em listas simplesmente encadeadas. Sessões ficam em lista duplamente encadeada porque precisamos percorrer nos dois sentidos pra verificar conflito de horário. Os assentos de cada sessão ficam numa matriz estática dentro da própria struct, evitando alocação dinâmica.

Os dados serão salvos em arquivos `.bin` e reconstruídos em memória toda vez que o programa iniciar.

## 22/11

Organizamos a estrutura de arquivos. Os headers ficam em `include/` e os arquivos de código em `src/`. Cada entidade tem seu próprio par de arquivos. Um `tipos.h` central reúne todos os enums e structs pra evitar dependências circulares entre os módulos.

Implementamos `utils.c` com as funções auxiliares que o projeto inteiro vai usar: limpar tela, pausar, ler strings e ler números com validação de entrada.

Implementamos `filmes.c` com o CRUD completo e persistência em `filmes.bin`. As regras RN01, RN09 e RN14 já estão aplicadas: o módulo não deixa excluir filme com sessão vinculada, não deixa alterar duração se já houver sessão, e ao carregar do arquivo o próximo código continua de onde parou.
