#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

typedef struct Session {
    char description[500];
    int enemyDefeated;
    struct Session* next;
    struct Session* prev;
} Session;

typedef struct Player {
    int HP;
    int MP;
    int shieldActive;
    int shieldRemaining;
} Player;

void displayOptions() {
    printf("**************************************************\n");
    printf("Escolha sua ação:\n");
    printf("1. Atacar normalmente\n");
    printf("2. Bola de Fogo (4 MP)\n");
    printf("3. Escudo (3 MP)\n");
    printf("4. Cura (3 MP)\n");
    printf("**************************************************\n");
}

void displayCombatInfo(Player* player) {
    printf("**************************************************\n");
    printf("Vidas: %d | Pontos de Magia (MP): %d", player->HP, player->MP);
    if (player->shieldActive) {
        printf(" | Escudo ativo (Duração restante: %d turnos)\n", player->shieldRemaining);
    } else {
        printf("\n");
    }
}

Session* createSession(char description[]) {
    Session* newSession = (Session*)malloc(sizeof(Session));
    strcpy(newSession->description, description);
    newSession->enemyDefeated = 0;
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

int rollD20() {
    return rand() % 20 + 1;
}

void performAction(Player* player, Session* currentSession) {
    int choice;
    int defesa = 0;

    do {
        displayCombatInfo(player);
        displayOptions();

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
                    player->HP -= (dano - defesa);

                    if (player->HP <= 0) {
                        printf("Você perdeu todas as vidas. Fim do jogo.\n");
                        exit(0); // Encerra o jogo
                    }
                } else {
                    printf("Você escorregou e o monstro te ataca brutalmente! Você perde duas vidas.\n");
                    int dano = 2;
                    // Reduz duas vidas do jogador
                    player->HP -= (dano - defesa);

                    if (player->HP <= 0) {
                        printf("Você perdeu todas as vidas. Fim do jogo.\n");
                        exit(0); // Encerra o jogo
                    }
                }

                // Verifica se o inimigo está vivo antes de contra-atacar
                if (!currentSession->enemyDefeated) {
                    // Inimigo ataca após o turno do jogador
                    printf("O monstro contra-ataca!\n");
                    int enemyRoll = rollD20();
                    printf("Resultado do dado do monstro: %d\n", enemyRoll);

                    if (enemyRoll > 19) {
                        printf("O monstro acertou você e causou 2 de dano!\n");
                        player->HP -= 2;
                    } else if (enemyRoll >= 12) {
                        printf("O monstro acertou você e causou 1 de dano.\n");
                        player->HP -= 1;
                    } else {
                        printf("O monstro errou o ataque. Você está a salvo por enquanto.\n");
                    }
                }
                break;
            case 2: // Bola de Fogo
                if (player->MP >= 4) {
                    printf("Você conjura a Bola de Fogo!\n");
                    diceRoll = rollD20(); // Movido para dentro do if para garantir uma nova rolagem
                    if (diceRoll > 10) {
                        printf("Você acertou o monstro e o derrotou.\n");
                        currentSession->enemyDefeated = 1; // Marca o inimigo como derrotado
                    } else {
                        printf("A Bola de Fogo errou o alvo.\n");
                    }
                    player->MP -= 4; // Consome 4 MP
                } else {
                    printf("Você não tem MP suficiente para conjurar a Bola de Fogo.\n");
                }
                break;
            case 3: // Escudo
                if (player->MP >= 3) {
                    if (player->shieldActive) {
                        printf("Você já conjurou o Escudo. Escolha outra ação.\n");
                    } else {
                        printf("Você conjura o Escudo! Sua defesa está melhorada.\n");
                        player->shieldActive = 1; // Ativa o Escudo
                        player->shieldRemaining = 2; // Define a duração do Escudo
                        player->MP -= 3; // Consome 3 MP
                    }
                } else {
                    printf("Você não tem MP suficiente para conjurar o Escudo.\n");
                }
                break;
            case 4: // Cura
                if (player->MP >= 3) {
                    printf("Você conjura a Cura! Você recuperou 1 ponto de vida.\n");
                    player->HP++; // Recupera 1 vida
                    player->MP -= 3; // Consome 3 MP
                } else {
                    printf("Você não tem MP suficiente para conjurar a Cura.\n");
                }
                break;
            default:
                printf("Escolha inválida. Você ataca normalmente.\n");
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

void entranceSession(Session* currentSession) {
    printf("%s\n", currentSession->description);
}

void middleSession(Session* currentSession, Player* player) {
    if (currentSession->next == NULL) {
        return;
    }

    do {
        performAction(player, currentSession);
    } while (!currentSession->enemyDefeated && player->HP > 0);
}

void exitSession() {
    printf("Parabéns, você encontrou a saída e descobriu o tesouro lendário!\n");
}

void displayStory(Session* currentSession) {
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
}

void playGame() {
    int choice;

    printf("Escolha qual sessão você quer jogar:\n");
    printf("\t1. Uma passagem sinuosa se estende para as profundezas\n");
    printf("\t2. Uma caverna iluminada por cristais brilhantes\n");
    printf("\t3. Uma caverna repleta de cristais cintilantes\n");
    printf("\t4. Uma caverna são adornadas com runas antigas\n");
    printf("\t5. Uma caverna com inscrições nas paredes\n");

    scanf("%d", &choice);

    switch (choice) {
        case 1: {
            Session* entrance1 = createSession("Você está na entrada da caverna escura. A frente, uma passagem sinuosa se estende para as profundezas.");
            Session* middleStart1 = createSession("Você avançou mais fundo na caverna. O ar fica mais frio e você ouve murmúrios distantes.");
            Session* middle1 = createSession("Você avançou mais fundo na caverna. O ar fica mais frio e você ouve murmúrios distantes.");
            Session* middleEnd1 = createSession("Você chegou à parte central da caverna. Uma bifurcação se estende para a esquerda e para a direita.");
            Session* exit1 = createSession("Você encontrou a saída da caverna. A luz do sol banha você, revelando um cenário deslumbrante.");

            displayStory(entrance1);
            displayStory(middleStart1);
            displayStory(middle1);
            displayStory(middleEnd1);
            displayStory(exit1);
            break;
        }
        case 2: {
            Session* entrance2 = createSession("Você entra em uma caverna iluminada por cristais brilhantes. O ar é carregado com uma energia mística.");
            Session* middleStart2 = createSession("Os murmúrios nesta parte da caverna parecem ser canções antigas. Símbolos estranhos adornam as paredes.");
            Session* middle2 = createSession("Criaturas mágicas guardam o caminho à frente. Elas parecem curiosas, mas cautela é sempre necessária.");
            Session* middleEnd2 = createSession("Uma fenda dimensional se abre, revelando uma paisagem além da compreensão. A bifurcação se aproxima.");
            Session* exit2 = createSession("A luz no final da caverna parece conter segredos arcanos. Você está prestes a descobrir seu destino.");

            displayStory(entrance2);
            displayStory(middleStart2);
            displayStory(middle2);
            displayStory(middleEnd2);
            displayStory(exit2);
            break;
        }
        case 3: {
            Session* entrance3 = createSession("Você entra em uma caverna repleta de cristais cintilantes, iluminando o caminho à frente.");
            Session* middleStart3 = createSession("Um eco misterioso preenche o ar, ecoando canções esquecidas. Sombras dançam nas paredes da caverna.");
            Session* middle3 = createSession("Aqui, criaturas mágicas pacíficas residem, oferecendo conhecimentos arcanos em troca de respeito.");
            Session* middleEnd3 = createSession("Você se depara com uma porta mágica, marcando o final desta parte da caverna. A bifurcação se revela.");
            Session* exit3 = createSession("Uma luz etérea indica a saída, revelando segredos que transcendem a compreensão humana.");

            displayStory(entrance3);
            displayStory(middleStart3);
            displayStory(middle3);
            displayStory(middleEnd3);
            displayStory(exit3);
            break;
        }
        case 4: {
            Session* entrance4 = createSession("As paredes desta caverna são adornadas com runas antigas, revelando uma história esquecida pelo tempo.");
            Session* middleStart4 = createSession("Uma bruma mística paira no ar, obscurecendo visões e aumentando a tensão ao seu redor.");
            Session* middle4 = createSession("Criaturas sombrias espreitam nas sombras, testando sua coragem e habilidade. O desconhecido aguarda adiante.");
            Session* middleEnd4 = createSession("Você se encontra diante de um portal dimensional, sugerindo que realidades alternativas se entrelaçam aqui.");
            Session* exit4 = createSession("A luz no fim do túnel revela a saída, mas algo sinistro parece acompanhá-lo.");

            displayStory(entrance4);
            displayStory(middleStart4);
            displayStory(middle4);
            displayStory(middleEnd4);
            displayStory(exit4);
            break;
        }
        case 5: {
            Session* entrance5 = createSession("Esta caverna exala uma atmosfera ancestral, com inscrições nas paredes contando a história dos antigos exploradores.");
            Session* middleStart5 = createSession("O som de água corrente ecoa, indicando a proximidade de um rio subterrâneo. Criaturas aquáticas espreitam nas sombras.");
            Session* middle5 = createSession("Aqui, o equilíbrio entre luz e escuridão cria um cenário surreal. O desconhecido se revela de maneiras inesperadas.");
            Session* middleEnd5 = createSession("Um portal interdimensional aguarda, oferecendo vislumbres de reinos além da imaginação.");
            Session* exit5 = createSession("A luz natural invade a caverna, marcando o fim da jornada subterrânea. Um novo começo se vislumbra no horizonte.");

            displayStory(entrance5);
            displayStory(middleStart5);
            displayStory(middle5);
            displayStory(middleEnd5);
            displayStory(exit5);
            break;
        }
        default:
            printf("**************************************************\n");
            printf("Opção inválida, tente novamente!\n");
            printf("**************************************************\n");
            break;
    }
}


int main() {
    setlocale(LC_ALL, "");

    srand((unsigned int)time(NULL));

    Player player = {5, 10, 0, 0};

    playGame();

    return 0;
}
