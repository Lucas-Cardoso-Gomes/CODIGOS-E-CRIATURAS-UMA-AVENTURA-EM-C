#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// Definição da estrutura para as sessões da caverna
typedef struct Session {
    char description[250];
    int enemyDefeated; // Nova variável para controlar se o inimigo foi derrotado
    struct Session* next;
    struct Session* prev;
} Session;

// Estrutura para o jogador
typedef struct Player {
    int lives;
    int mp;
    int shieldActive; // Nova variável para indicar se o Escudo está ativo
    int shieldRemaining;
} Player;

void displayPlayerStatus(Player* player) {
    printf("Vidas: %d | Pontos de Magia (MP): %d", player->lives, player->mp);
    if (player->shieldActive) {
        printf(" | Escudo ativo (Duração restante: %d turnos)\n", player->shieldRemaining);
    } else {
        printf("\n");
    }
}

// Função para criar uma nova sessão
Session* createSession(char description[]) {
    Session* newSession = (Session*)malloc(sizeof(Session));
    strcpy(newSession->description, description);
    newSession->enemyDefeated = 0; // Inicializa como inimigo não derrotado
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

// Função para simular um lançamento de dado de 20 lados (d20)
int rollD20() {
    return rand() % 20 + 1; // Gera um número entre 1 e 20
}

// Função para lidar com a sessão de combate
void handleCombat(Player* player, Session* currentSession) {
    printf("Você encontra um terrível monstro! Ele está prestes a atacar.\n");

    // Verifica se o inimigo já foi derrotado
    if (currentSession->enemyDefeated) {
        printf("O monstro já foi derrotado anteriormente. Você avança com cuidado.\n");
        return;
    }

    int choice;
    int defesa = 0;
    do {
        // Opções de magia
        displayPlayerStatus(player);
        printf("Escolha sua ação:\n1. Atacar normalmente\n2. Bola de Fogo (4 MP)\n3. Escudo (3 MP)\n4. Cura (3 MP)\n");

        scanf("%d", &choice);

        switch (choice) {
            case 1: // Atacar normalmente
                if (player->shieldActive) {
                    printf("O monstro ataca, mas seu Escudo absorve parte do dano.\n");
                    defesa = 1;
                }
                // Lança o d20 para determinar o resultado do combate
                printf("Rolando o dado d20...\n");
                int diceRoll = rollD20();
                printf("Resultado do dado: %d\n", diceRoll);

                if (diceRoll > 15) {
                    printf("Você derrotou o monstro e avança com cuidado.\n");
                    currentSession->enemyDefeated = 1; // Marca o inimigo como derrotado
                } else if (diceRoll >= 7) {
                    printf("Você errou o ataque e o monstro contra-atacou! Você perde uma vida.\n");
                    int dano = 1;
                    // Reduz uma vida do jogador
                    player->lives -= (dano - defesa);

                    if (player->lives <= 0) {
                        printf("Você perdeu todas as vidas. Fim do jogo.\n");
                        exit(0); // Encerra o jogo
                    }
                } else {
                    printf("Você escorregou e o monstro te ataca brutalmente! Você perde duas vidas.\n");
                    int dano = 2;
                    // Reduz duas vidas do jogador
                    player->lives -= (dano - defesa);

                    if (player->lives <= 0) {
                        printf("Você perdeu todas as vidas. Fim do jogo.\n");
                        exit(0); // Encerra o jogo
                    }
                }
                break;
            case 2: // Bola de Fogo
                if (player->mp >= 4) {
                    printf("Você conjura a Bola de Fogo!\n");
                    if (diceRoll > 10) {
                        printf("Você acertou o monstro e o derrotou.\n");
                        currentSession->enemyDefeated = 1; // Marca o inimigo como derrotado
                    } else {
                        printf("A Bola de Fogo errou o alvo.\n");
                    }
                    player->mp -= 4; // Consome 4 MP
                } else {
                    printf("Você não tem MP suficiente para conjurar a Bola de Fogo.\n");
                }
                break;
            case 3: // Escudo
                if (player->mp >= 3) {
                    if (player->shieldActive) {
                        printf("Você já conjurou o Escudo. Escolha outra ação.\n");
                    } else {
                        printf("Você conjura o Escudo! Sua defesa está melhorada.\n");
                        player->shieldActive = 1; // Ativa o Escudo
                        player->shieldRemaining = 2; // Define a duração do Escudo
                        player->mp -= 3; // Consome 3 MP
                    }
                } else {
                    printf("Você não tem MP suficiente para conjurar o Escudo.\n");
                }
                break;
            case 4: // Cura
                if (player->mp >= 3) {
                    printf("Você conjura a Cura! Você recuperou 1 ponto de vida.\n");
                    player->lives++; // Recupera 1 vida
                    player->mp -= 3; // Consome 3 MP
                } else {
                    printf("Você não tem MP suficiente para conjurar a Cura.\n");
                }
                break;
            default:
                printf("Escolha inválida. Você ataca normalmente.\n");
                // Lógica de ataque normal aqui
                break;
        }

        if (player->shieldActive) {
            player->shieldRemaining--;
            if (player->shieldRemaining == 0) {
                printf("O Escudo dissipou-se. Sua defesa retorna ao normal.\n");
                player->shieldActive = 0; // Desativa o Escudo
            }
        }

    } while (!currentSession->enemyDefeated && choice == 1);
}

// Função para lidar com a sessão da armadilha
void handleTrap() {
    printf("Você se depara com uma armadilha complexa no caminho.\n");
    printf("Escolha:\n1. Tentar desativar a armadilha\n2. Procurar um desvio\n");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Você consegue desativar a armadilha com sucesso e avança com cuidado.\n");
    } else {
        printf("Ao procurar um desvio, você encontra uma passagem secreta que leva a um tesouro escondido!\n");
        printf("Você decide pegar o tesouro antes de continuar sua jornada.\n");
        exit(0); // Encerra o jogo
    }
}

// Função para lidar com a sessão da entrada da caverna
void entranceSession(Session* currentSession) {
    printf("%s\n", currentSession->description);
}

// Função para lidar com a sessão do meio da caverna
void middleSession(Session* currentSession, Player* player) {
    if (currentSession->next == NULL) {
        return; // Não há próxima sessão, encerra a função
    }

    do {
        handleCombat(player, currentSession);
    } while (!currentSession->enemyDefeated && player->lives > 0);
}

// Função para lidar com a sessão da saída da caverna
void exitSession() {
    printf("Parabéns, você encontrou a saída e descobriu o tesouro lendário!\n");
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

    // Inicializa o jogador com 3 vidas, 10 pontos de magia (MP) e sem escudo
    Player player = {3, 10, 0, 0};

    // Loop principal do jogo
    while (1) {
        if (currentSession == entrance) {
            entranceSession(currentSession);
        } else if (currentSession == middleStart || currentSession == middleEnd) {
            middleSession(currentSession, &player);
        } else if (currentSession == exit) {
            exitSession();
            break;
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
    }

    // Libera a memória alocada
    destroySessions(entrance);
}

int main() {
    // Configuração da localidade para lidar com caracteres especiais
    setlocale(LC_ALL,"");

    // Inicializa o gerador de números aleatórios
    srand((unsigned int)time(NULL));

    // Inicia o jogo
    playGame();

    return 0;
}
