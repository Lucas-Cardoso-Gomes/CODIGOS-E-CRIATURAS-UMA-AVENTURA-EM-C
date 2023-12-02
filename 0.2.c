#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definição da estrutura para as sessões da caverna
typedef struct Session {
    char description[200];
    struct Session* next;
    struct Session* prev;
} Session;

// Função para criar uma nova sessão
Session* createSession(char description[]) {
    Session* newSession = (Session*)malloc(sizeof(Session));
    strcpy(newSession->description, description);
    newSession->next = NULL;
    newSession->prev = NULL;
    return newSession;
}

// Função para destruir a lista de sessões
void destroySessions(Session* start) {
    while (start != NULL) {
        Session* temp = start;
        start = start->next;
        free(temp);
    }
}

// Função principal do jogo
void playGame() {
    // Criação das sessões de entrada, meio e fim
    Session* entrance = createSession("Você está na entrada da caverna escura. A frente, uma passagem sinuosa se estende para as profundezas.");
    Session* middleStart = createSession("Você avançou mais fundo na caverna. O ar fica mais frio e você ouve murmúrios distantes.");
    Session* middleEnd = createSession("Você chegou à parte central da caverna. Uma bifurcação se estende para a esquerda e para a direita.");
    Session* exit = createSession("Você encontrou a saída da caverna. A luz do sol banha você, revelando um cenário deslumbrante.");

    // Conecta as sessões
    entrance->next = middleStart;
    middleStart->prev = entrance;
    middleStart->next = middleEnd;
    middleEnd->prev = middleStart;
    middleEnd->next = exit;
    exit->prev = middleEnd;

    Session* currentSession = entrance;

    // Loop principal do jogo
    while (1) {
        printf("%s\n", currentSession->description);

        // Introdução de desafios
        if (currentSession == middleStart) {
            printf("Você percebe que a caverna está infestada de morcegos assustadores!\n");
            printf("Escolha:\n1. Enfrentar os morcegos\n2. Tentar passar despercebido\n");
            int choice;
            scanf("%d", &choice);

            if (choice == 1) {
                printf("Os morcegos atacam, mas você consegue se esquivar e seguir em frente.\n");
            } else {
                printf("Você tenta passar despercebido, mas um morcego te ataca! Você consegue se livrar dele, mas sofre alguns arranhões.\n");
            }
        } else if (currentSession == middleEnd) {
            printf("Você se depara com uma armadilha complexa no caminho.\n");
            printf("Escolha:\n1. Tentar desativar a armadilha\n2. Procurar um desvio\n");
            int choice;
            scanf("%d", &choice);

            if (choice == 1) {
                printf("Você consegue desativar a armadilha com sucesso e avança com cuidado.\n");
            } else {
                printf("Ao procurar um desvio, você encontra uma passagem secreta que leva a um tesouro escondido!\n");
                printf("Você decide pegar o tesouro antes de continuar sua jornada.\n");
                break;
            }
        }

        printf("Escolha:\n1. Voltar\n2. Prosseguir\n");

        int choice;
        scanf("%d", &choice);

        if (choice == 1 && currentSession->prev != NULL) {
            currentSession = currentSession->prev;
        } else if (choice == 2 && currentSession->next != NULL) {
            currentSession = currentSession->next;
        } else {
            printf("Escolha inválida.\n");
        }

        if (currentSession == exit) {
            printf("Parabéns, você encontrou a saída e descobriu o tesouro lendário!\n");
            break;
        }
    }

    // Libera a memória alocada
    destroySessions(entrance);
}

int main() {
    // Inicializa o gerador de números aleatórios
    srand((unsigned int)time(NULL));

    // Inicia o jogo
    playGame();

    return 0;
}
