#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define CUSTO_BOLA_DE_FOGO 4
#define CUSTO_ESCUDO 3
#define CUSTO_CURA 3

typedef struct Sessao {
    char descricao[500];
    int inimigo;
    int armadilha;
    int mod;
    struct Sessao* proxima;
    struct Sessao* anterior;
} Sessao;

typedef struct Jogador {
    int HP;
    int MP;
    int escudoAtivo;
    int duracaoEscudo;
} Jogador;

void mostrarOpcoes(Sessao* sessaoAtual) {
    printf("**************************************************\n");
    printf("Escolha sua ação:\n");
    printf("1. Atacar normalmente\n");
    printf("2. Bola de Fogo (%d MP)\n", CUSTO_BOLA_DE_FOGO);
    printf("3. Escudo (%d MP)\n", CUSTO_ESCUDO);
    printf("4. Cura (%d MP)\n", CUSTO_CURA);
    printf("**************************************************\n");
}

void mostrarInformacoesCombate(Jogador* jogador) {
    printf("**************************************************\n");
    printf("Vidas: %d | Pontos de Magia (PM): %d", jogador->HP, jogador->MP);
    if (jogador->escudoAtivo) {
        printf(" | Escudo ativo (Duração restante: %d turnos)\n", jogador->duracaoEscudo);
    } else {
        printf("\n");
    }
}

Sessao* criarSessao(const char descricao[500]) {
    Sessao* novaSessao = (Sessao*)malloc(sizeof(Sessao));
    if (novaSessao == NULL) {
        printf("Erro ao alocar memória para a sessão.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(novaSessao->descricao, descricao, sizeof(novaSessao->descricao));
    novaSessao->inimigo = 0;
    novaSessao->armadilha = 0;
    novaSessao->proxima = NULL;
    novaSessao->anterior = NULL;

    return novaSessao;
}

void destruirSessoes(Sessao* inicio) {
    while (inicio != NULL) {
        Sessao* temp = inicio;
        inicio = inicio->proxima;
        free(temp);
    }
}

int rolarD20() {
    return rand() % 20 + 1;
}

void dano(Jogador* jogador, int valorDano) {
    int dano = valorDano;

    if (jogador->escudoAtivo == 1) {
        dano -= 1;
    }

    jogador->HP -= dano;

    if (jogador->escudoAtivo) {
        jogador->duracaoEscudo--;
        if (jogador->duracaoEscudo == 0) {
            printf("O Escudo dissipou-se. Sua defesa retorna ao normal.\n");
            jogador->escudoAtivo = 0;
        }
    }
}

void executarAtaque(Jogador* jogador, Sessao* sessaoAtual) {
    printf("Rolando o dado d20...\n");
    int resultadoDado = rolarD20();
    printf("Resultado do dado: %d\n", resultadoDado);

    if (resultadoDado > 14) {
        printf("Você derrotou o monstro e avança com cuidado.\n");
        sessaoAtual->inimigo = 0;
    } else if (resultadoDado >= 3 && resultadoDado <= 14) {
        printf("Você errou o ataque!\n");
    } else {
        printf("Você escorregou e o monstro te ataca! Você perde 1 HP.\n");
        dano(jogador, 1);
    }

    if (jogador->HP <= 0) {
        printf("Você perdeu todas as vidas. Fim do jogo.\n");
        exit(EXIT_SUCCESS);
    }

    printf("**************************************************\n");

    if (sessaoAtual->inimigo == 1) {
        printf("O monstro contra-ataca!\n");
        int resultadoDadoInimigo = rolarD20();
        printf("Resultado do dado do monstro: %d\n", resultadoDadoInimigo);

        if (resultadoDadoInimigo >= 19) {
            printf("O monstro acertou você e causou 2 de dano!\n");
            dano(jogador, 2);
        } else if (resultadoDadoInimigo >= 12) {
            printf("O monstro acertou você e causou 1 de dano.\n");
            dano(jogador, 1);
        } else if (resultadoDadoInimigo <= 2) {
            printf("O monstro escorregou e você contra-atacou!\n");
            sessaoAtual->inimigo = 0;
        } else {
            printf("O monstro errou o ataque..\n");
        }
    }

    if (jogador->HP <= 0) {
        printf("Você perdeu todas as vidas. Fim do jogo.\n");
        exit(EXIT_SUCCESS);
    }
}

void executarBolaDeFogo(Jogador* jogador, Sessao* sessaoAtual) {
    if (jogador->HP >= CUSTO_BOLA_DE_FOGO) {
        printf("Você conjura a Bola de Fogo!\n");
        int resultadoDado = rolarD20();
        printf("Resultado do dado %d\n", resultadoDado);

        if (resultadoDado > 10) {
            printf("Você acertou o monstro e o derrotou.\n");
            sessaoAtual->inimigo = 0;
        } else {
            printf("A Bola de Fogo errou o alvo.\n");
        }

        jogador->HP -= CUSTO_BOLA_DE_FOGO;
    } else {
        printf("Você não tem PM suficiente para conjurar a Bola de Fogo.\n");
    }
}

void executarEscudo(Jogador* jogador) {
    if (jogador->MP >= CUSTO_ESCUDO) {
        if (jogador->escudoAtivo) {
            printf("Você já conjurou o Escudo. Escolha outra ação.\n");
        } else {
            printf("Você conjura o Escudo! Sua defesa está melhorada.\n");
            jogador->escudoAtivo = 1;
            jogador->duracaoEscudo = 2;
            jogador->MP -= CUSTO_ESCUDO;
        }
    } else {
        printf("Você não tem PM suficiente para conjurar o Escudo.\n");
    }
}

void executarCura(Jogador* jogador) {
    if (jogador->MP >= CUSTO_CURA) {
        printf("Você conjura a Cura! Você recuperou 1 ponto de vida.\n");
        jogador->HP++;
        jogador->MP -= CUSTO_CURA;
    } else {
        printf("Você não tem PM suficiente para conjurar a Cura.\n");
    }
}

void executarAcao(Jogador* jogador, Sessao* sessaoAtual) {
    int escolha;

    if (sessaoAtual->inimigo == 1) {
        do {
            mostrarInformacoesCombate(jogador);
            mostrarOpcoes(sessaoAtual);

            scanf("%d", &escolha);
            switch (escolha) {
                case 1:
                    executarAtaque(jogador, sessaoAtual);
                    break;
                case 2:
                    executarBolaDeFogo(jogador, sessaoAtual);
                    break;
                case 3:
                    executarEscudo(jogador);
                    break;
                case 4:
                    executarCura(jogador);
                    break;
                default:
                    printf("Opção inválida. Tente novamente.\n");
            }

        } while (escolha < 1 || escolha > 4);

        // Se o inimigo for derrotado após a escolha do jogador, atualize a sessão
        if (sessaoAtual->inimigo == 0) {
            printf("**************************************************\n");
            printf("Você derrotou a criatura. Seu caminho está livre novamente.\n");
        }
    }
}

void executarArmadilha(Jogador* jogador, Sessao* sessaoAtual) {
    mostrarInformacoesCombate(jogador);
    printf("Você se depara com uma armadilha!\n");
    
    int opcao;
    printf("Escolha uma opção:\n1. Usar escudo e tentar passar\n2. Se curar\n3. Tentar passar");
    scanf("%d", &opcao);
    
    switch (opcao)
    {
        case 1:
            executarEscudo(jogador);
            printf("Você conjura o escudo e tenta passar pela armadilha...\n");
            break;
        case 2:
            executarCura(jogador);
            printf("Você conjura o cura e tenta passar pela armadilha...\n");
            break;
        case 3:
            printf("Você tenta passar pela armadilha...\n");
            break;
    }

    int resultadoDado = rolarD20();
    printf("Resultado do dado %d\n", resultadoDado);

    if (resultadoDado >= 15) {
        printf("Você conseguiu desarmar a armadilha e sair ileso!\n");
        sessaoAtual->armadilha=0;
    } else if (resultadoDado >= 7) {
        printf("Você passou pela armadilha, mas sofreu um pequeno ferimento. Perdeu 1 ponto de vida.\n");
        jogador->HP--;
        sessaoAtual->armadilha=0;
    } else {
        printf("Você foi pego pela armadilha! Sofreu um ferimento grave. Perdeu 1 ponto de vida.\n");
        jogador->HP--;
    }
    if (jogador->HP <= 0) {
            printf("Você perdeu todas as vidas. Fim do jogo.\n");
            exit(EXIT_SUCCESS);
    }
    printf("**************************************************\n");
}

void jogarJogo(Sessao* inicioSessao, Jogador* jogador) {

    Sessao* sessaoAtual = inicioSessao;

    while (sessaoAtual != NULL) {
        
        printf("%s\n", sessaoAtual->descricao);

        while (sessaoAtual->inimigo) {
            executarAcao(jogador, sessaoAtual);
            if (jogador->HP <= 0) {
                printf("Você perdeu todas as vidas. Fim do jogo.\n");
                break;
            }
        }

        while (sessaoAtual->armadilha) {
            executarArmadilha(jogador, sessaoAtual);
            if (jogador->HP <= 0) {
                printf("Você perdeu todas as vidas. Fim do jogo.\n");
                break;
            }
        }
        sessaoAtual = sessaoAtual->proxima;
    }

    printf("Parabéns! Você explorou a caverna com sucesso!\n");
    destruirSessoes(inicioSessao);
}


void campanha1(Sessao* inicioSessao)/*Main Quest*/ {

}

void campanha2(Sessao* inicioSessao)/*Caverna Escura*/ {
    Sessao* sessao1 = criarSessao("\nVocê entra em uma caverna iluminada por cristais brilhantes. O ar é carregado com uma energia mística.");
    Sessao* sessao2 = criarSessao("Você se depara em um caminho longo, cada passo que você dá, menos brilho os cristais parecem prover.\nNa escuridão você percebe algo...");
    sessao2->armadilha = 1;
    Sessao* sessao3 = criarSessao("\n\tNo escuro, surge uma criatura. Ela parece hostil!");
    sessao3->inimigo = 1;
    Sessao* sessao4 = criarSessao("\nVocê derrotou a criatura. Seu caminho está livre novamente.");
    Sessao* sessao5 = criarSessao("\nVocê entra na sala iluminada por cristais pulsantes. Uma sensação de calma toma conta de você.");

    sessao1->proxima = sessao2;
    sessao2->anterior = sessao1;
    sessao2->proxima = sessao3;
    sessao3->anterior = sessao2;
    sessao3->proxima = sessao4;
    sessao4->anterior = sessao3;
    sessao4->proxima = sessao5;
    sessao5->anterior = sessao4;

    inicioSessao = sessao1;
}

void campanha3(Sessao* inicioSessao)/*Caverda dos goblins*/ {
    
}

void campanha4(Sessao* inicioSessao)/*Caverna dos Orcs*/ {
    
}

int main() {
    setlocale(LC_ALL, "");
    srand((unsigned int)time(NULL));

    int opcao;

    printf("**************************************************\n");
    printf("Códigos e Criaturas: uma aventura em C!\n");
    printf("**************************************************\n");
    printf("Bem-vindo ao um jogo de RPG Textual\n");
    printf("⠀⠀⠀⡄⠀⠀⠀⠀⠀⠀⣠⠀ ⢀⠀⠀⢠⠀⠀⠀\n");
    printf("⠀⠀⢸⣧⠀⠀⠀⠀⢠⣾⣇⣀⣴⣿⠀⠀⣼⡇⠀⠀\n");
    printf("⠀⠀⣾⣿⣧⠀⠀⢀⣼⣿⣿⣿⣿⣿⠀⣼⣿⣷⠀⠀\n");
    printf("⠀⢸⣿⣿⣿⡀⠀⠸⠿⠿⣿⣿⣿⡟⢀⣿⣿⣿⡇⠀\n");
    printf("⠀⣾⣿⣿⣿⣿⡀⠀⢀⣼⣿⣿⡿⠁⣿⣿⣿⣿⣷⠀\n");
    printf("⢸⣿⣿⣿⣿⠁⣠⣤⣾⣿⣿⣯⣤⣄⠙⣿⣿⣿⣿⡇\n");
    printf("⣿⣿⣿⣿⣿⣶⣿⣿⣿⣿⣿⣿⣿⣿⣶⣿⣿⣿⣿⣿\n");
    printf("⠘⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏\n");
    printf("⠀⠘⢿⣿⣿⣿⠛⠻⢿⣿⣿⣿⠹⠟⣿⣿⣿⣿⣿⠀\n");
    printf("⠀⠀⠘⢿⣿⣿⣦⡄⢸⣿⣿⣿⡇⠠⣿⣿⣿⣿⡇⠀\n");
    printf("⠀⠀⠀⠘⢿⣿⣿⠀⣸⣿⣿⣿⠇⠀⠙⣿⣿⣿⠁⠀\n");
    printf("⠀⠀⠀⠀⠘⣿⠃⢰⣿⣿⣿⡇⠀⠀⠀⠈⢻⡇⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠈⠀⠈⢿⣿⣿⣿⣶⡶⠂⠀⠀⠁⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀\n");
    printf("Escolha uma opção:\n1. Main Quest\n2. Caverna Escura\n3. Caverna dos goblins\n4. Caverna dos Orcs \n5. Sair\n");
    scanf("%d", &opcao);
    
    if (opcao == 1) {
        Jogador jogador = {5, 10, 0, 0};
        Sessao* inicioSessao = criarSessao("");
        campanha1(inicioSessao);
        jogarJogo(inicioSessao, &jogador);
    } else if (opcao == 2) {
        Jogador jogador = {5, 10, 0, 0};
        Sessao* inicioSessao = criarSessao("");
        campanha2(inicioSessao);
        jogarJogo(inicioSessao, &jogador);
    } else if (opcao == 3) {
        Jogador jogador = {5, 10, 0, 0};
        Sessao* inicioSessao = criarSessao("");
        campanha3(inicioSessao);
        jogarJogo(inicioSessao, &jogador);
    } else if (opcao == 4) {
        Jogador jogador = {5, 10, 0, 0};
        Sessao* inicioSessao = criarSessao("");
        campanha4(inicioSessao);
        jogarJogo(inicioSessao, &jogador);
    } else if (opcao == 5) {
        printf("**************************************************\n");
        printf("Obrigado por jogar! Até a próxima aventura!\n");
        printf("**************************************************\n");
        return 0;
    }
    return 0;
}   