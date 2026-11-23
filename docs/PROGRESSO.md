# Progresso — CineDoom CLI

## 17/11

Definimos o escopo do projeto: sistema de cinema em C com 12 funcionalidades distribuídas entre filmes, sessões, salas e ingressos.

Escolhemos as estruturas de dados: lista simplesmente encadeada pra filmes e ingressos, lista duplamente encadeada pra sessões (porque precisamos verificar conflito de horário nos dois sentidos) e matriz estática pra assentos dentro de cada sessão.

Ficou decidido também que os dados vão ser salvos em arquivos `.bin` e reconstruídos em memória toda vez que o programa iniciar.

## 22/11

Definimos a estrutura de arquivos do projeto. Separamos o código em módulos: cada entidade (filmes, sessões, ingressos) tem seu próprio `.c` com as funções relacionadas. Um `tipos.h` central guarda todos os enums e structs pra evitar problema de inclusão circular, já que as entidades se referenciam entre si. Os docs foram pra uma pasta `docs/` pra não misturar com o código.

Criamos o `tipos.h` com todas as definições: enums de gênero, classificação e tipo de ingresso, structs das entidades e os nós das listas encadeadas, e as 4 salas fixas como constante.

Cada módulo do projeto tem dois arquivos: um `.h` e um `.c`. O `.h` só declara quais funções existem, o `.c` tem o código de verdade. Quando outro módulo precisa usar uma função, ele inclui o `.h` com `#include` — assim não precisa copiar nada, e se a função mudar basta atualizar em um lugar só.

Criamos `utils.h` e `utils.c` com as funções auxiliares que todos os módulos vão usar: limpar tela, pausar, ler texto e ler números com validação. A leitura com validação foi necessária porque `scanf` sozinho quebra quando o usuário digita texto no lugar de número — a função repete a pergunta até receber uma entrada válida.
