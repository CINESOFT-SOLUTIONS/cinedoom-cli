# Progresso — CineDoom CLI

## 17/11

Definimos o escopo do projeto: sistema de cinema em C com 12 funcionalidades distribuídas entre filmes, sessões, salas e ingressos.

Escolhemos as estruturas de dados: lista simplesmente encadeada pra filmes e ingressos, lista duplamente encadeada pra sessões (porque precisamos verificar conflito de horário nos dois sentidos) e matriz estática pra assentos dentro de cada sessão.

Ficou decidido também que os dados vão ser salvos em arquivos `.bin` e reconstruídos em memória toda vez que o programa iniciar.
