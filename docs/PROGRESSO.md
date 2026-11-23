# Progresso — CineDoom CLI

## 17/11

Definimos o escopo do projeto: sistema de cinema em C com 12 funcionalidades distribuídas entre filmes, sessões, salas e ingressos.

Escolhemos as estruturas de dados: lista simplesmente encadeada pra filmes e ingressos, lista duplamente encadeada pra sessões (porque precisamos verificar conflito de horário nos dois sentidos) e matriz estática pra assentos dentro de cada sessão.

Ficou decidido também que os dados vão ser salvos em arquivos `.bin` e reconstruídos em memória toda vez que o programa iniciar.

Definimos a estrutura de arquivos do projeto. Separamos o código em módulos: cada entidade (filmes, sessões, ingressos) tem seu próprio `.c` com as funções relacionadas. Os headers ficam em `include/` e os arquivos de código em `src/`. Um `tipos.h` central guarda todos os enums e structs pra evitar problema de inclusão circular, já que as entidades se referenciam entre si. Os docs foram pra uma pasta `docs/` pra não misturar com o código.

## 22/11

Criamos o `tipos.h` com todas as definições: enums de gênero, classificação e tipo de ingresso, structs das entidades e os nós das listas encadeadas, e as 4 salas fixas como constante.

Cada módulo do projeto tem dois arquivos: um `.h` e um `.c`. O `.h` só declara quais funções existem, o `.c` tem o código de verdade. Quando outro módulo precisa usar uma função, ele inclui o `.h` com `#include`, assim não precisa copiar nada, e se a função mudar basta atualizar em um lugar só.

Criamos `utils.h` e `utils.c` com as funções auxiliares que todos os módulos vão usar: limpar tela, pausar, ler texto e ler números com validação. A leitura com validação foi necessária porque `scanf` sozinho quebra quando o usuário digita texto no lugar de número, a função repete a pergunta até receber uma entrada válida.

Criamos `filmes.h` e `filmes.c` com o módulo completo de filmes: cadastro, listagem, atualização e exclusão, mais as funções de carregar e salvar no `.bin`. O código mantém os filmes ordenados por código na lista. Algumas regras de negócio já estão aplicadas aqui: não deixa excluir filme com sessão vinculada (RN01), não deixa alterar a duração se já tiver sessão (RN09), e ao carregar do arquivo o próximo código continua de onde parou pra não repetir (RN14).

Criamos `sessoes.h` e `sessoes.c` com o módulo completo de sessões: lista duplamente encadeada ordenada por data/hora, CRUD com persistência em `sessoes.bin` e todas as validações do escopo. A detecção de conflito de horário compara os intervalos de duas sessões na mesma sala e na mesma data convertendo tudo pra minutos. O módulo também implementa a verificação real de `filmeTemSessao`, que antes era um stub em `filmes.c`.

Criamos `ingressos.h` e `ingressos.c` com venda, cancelamento e listagem de ingressos. O carregamento do `.bin` já remonta a matriz de assentos de cada sessão, que não é gravada em disco. O mapa de assentos é exibido na venda com `O` pra livre e `X` pra ocupado. O módulo implementa a verificação real de `sessaoTemIngresso`, que antes era stub em `sessoes.c`.

