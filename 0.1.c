#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definição da estrutura para as sessões da caverna
typedef struct Session {
    char description[100];
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
    Session* entrance = createSession("Você está na entrada da caverna.");
    Session* middleStart = createSession("Você entrou mais fundo na caverna.");
    Session* middleEnd = createSession("Você está na parte central da caverna.");
    Session* exit = createSession("Você encontrou a saída da caverna.");

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
            printf("Parabéns, você encontrou a saída!\n");
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
