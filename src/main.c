#include <stdio.h>

void menu() {
    printf("\n");
    printf("  +==============================================================+\n");
    printf("  |   ____ ___ _   _ _____ ____   ___   ___  __  __              |\n");
    printf("  |  / ___|_ _| \\ | | ____|  _ \\ / _ \\ / _ \\|  \\/  |             |\n");
    printf("  | | |    | ||  \\| |  _| | | | | | | | | | | |\\/| |             |\n");
    printf("  | | |___ | || |\\  | |___| |_| | |_| | |_| | |  | |             |\n");
    printf("  |  \\____|___|_| \\_|_____|____/ \\___/ \\___/|_|  |_|             |\n");
    printf("  |                                                              |\n");
    printf("  +==============================================================+\n");
    printf("  |                       MENU PRINCIPAL                         |\n");
    printf("  +==============================================================+\n");
    printf("  |                                                              |\n");
    printf("  |   [1] - Consultar Filmes                                     |\n");
    printf("  |   [2] - Consultar Sessões, Horários e Salas                  |\n");
    printf("  |   [3] - Consultar Disponibilidade de Ingressos               |\n");
    printf("  |   [4] - Comprar Ingressos                                    |\n");
    printf("  |   [5] - Consultar Ingressos                                  |\n");
    printf("  |   [6] - Cancelar Ingresso Adquirido                          |\n");
    printf("  |                                                              |\n");
    printf("  |   [9] - Sair do Sistema                                      |\n");
    printf("  |                                                              |\n");
    printf("  +==============================================================+\n");
    printf("   >> Escolha uma opção: ");
}

//PQP não sei escrever cód mais kakakakakakakaka

int main() {
int opcao;

    do
    {
        menu();
        scanf("%d", &opcao);
        switch (opcao){
            case 1:
                printf("teste caso 1\n");
                break;
            case 2:
                printf("teste caso 2\n");
                break;
            case 3:
                printf("teste caso 3\n");
                break;
            case 4:
                printf("teste caso 4\n");
                break;
            case 5:
                printf("teste caso 5\n");
                break;
            case 6:
                printf("teste caso 6\n");
                break;
            case 9:
                printf("OBRIGADO POR USAR NOSSO SISTEMA!\n");
                break;
            default:
                printf("Digite a opção desejada");
                break;
            
        }

    } while (!(opcao == 9));
    


    return 0;
}