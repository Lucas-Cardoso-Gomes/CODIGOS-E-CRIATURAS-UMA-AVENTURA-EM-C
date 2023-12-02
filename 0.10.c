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
    struct Sessao* proxima;
    struct Sessao* anterior;
} Sessao;

typedef struct Jogador {
    int PV;
    int PM;
    int escudoAtivo;
    int duracaoEscudo;
} Jogador;

void mostrarOpcoes(Sessao* sessaoAtual) {
        printf("**************************************************\n");
        printf("Escolha sua ação:\n");
        printf("1. Atacar normalmente\n");
        printf("2. Bola de Fogo (%d PM)\n", CUSTO_BOLA_DE_FOGO);
        printf("3. Escudo (%d PM)\n", CUSTO_ESCUDO);
        printf("4. Cura (%d PM)\n", CUSTO_CURA);
        printf("**************************************************\n");
}

void mostrarInformacoesCombate(Jogador* jogador) {
    printf("**************************************************\n");
    printf("Vidas: %d | Pontos de Magia (PM): %d", jogador->PV, jogador->PM);
    if (jogador->escudoAtivo) {
        printf(" | Escudo ativo (Duração restante: %d turnos)\n", jogador->duracaoEscudo);
    } else {
        printf("\n");
    }
}

Sessao* criarSessao(char descricao[]) {
    Sessao* novaSessao = (Sessao*)malloc(sizeof(Sessao));
    if (novaSessao == NULL) {
        printf("Erro ao alocar memória para a sessão.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(novaSessao->descricao, descricao);
    novaSessao->inimigo = 0;
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

void executarAtaque(Jogador* jogador, Sessao* sessaoAtual) {
    int defesa = 0;

    if (jogador->escudoAtivo) {
        printf("O monstro ataca, mas seu Escudo absorve parte do dano.\n");
        defesa = 1;
    }

    printf("Rolando o dado d20...\n");
    int resultadoDado = rolarD20();
    printf("Resultado do dado: %d\n", resultadoDado);

    int dano = 0;
    if (resultadoDado > 15) {
        printf("Você derrotou o monstro e avança com cuidado.\n");
        sessaoAtual->inimigo = 1;
    } else if (resultadoDado >= 7) {
        printf("Você errou o ataque!\n");
    } if (resultadoDado == 1){
        printf("Você escorregou e o monstro te ataca! Você perde 1 HP.\n");
        dano = 1;
    }

    jogador->PV -= (dano - defesa);

    if (jogador->PV <= 0) {
        printf("Você perdeu todas as vidas. Fim do jogo.\n");
        exit(EXIT_SUCCESS);
    }

    if (!sessaoAtual->inimigo) {
        printf("O monstro contra-ataca!\n");
        int resultadoDadoInimigo = rolarD20();
        printf("Resultado do dado do monstro: %d\n", resultadoDadoInimigo);

        if (resultadoDadoInimigo > 19) {
            printf("O monstro acertou você e causou 2 de dano!\n");
            jogador->PV -= 2;
        } else if (resultadoDadoInimigo >= 12) {
            printf("O monstro acertou você e causou 1 de dano.\n");
            jogador->PV -= 1;
        } else if (resultadoDadoInimigo == 1) {
            printf("O monstro escorregou e você contra-atacou!\n");
            sessaoAtual->inimigo = 0;
        } else {
            printf("O monstro errou o ataque..\n");
        }
    }
}

void executarBolaDeFogo(Jogador* jogador, Sessao* sessaoAtual) {
    if (jogador->PM >= CUSTO_BOLA_DE_FOGO) {
        printf("Você conjura a Bola de Fogo!\n");
        int resultadoDado = rolarD20();

        if (resultadoDado > 10) {
            printf("Você acertou o monstro e o derrotou.\n");
            sessaoAtual->inimigo = 1;
        } else {
            printf("A Bola de Fogo errou o alvo.\n");
        }

        jogador->PM -= CUSTO_BOLA_DE_FOGO;
    } else {
        printf("Você não tem PM suficiente para conjurar a Bola de Fogo.\n");
    }
}

void executarEscudo(Jogador* jogador, Sessao* sessaoAtual) {
    if (jogador->PM >= CUSTO_ESCUDO) {
        if (jogador->escudoAtivo) {
            printf("Você já conjurou o Escudo. Escolha outra ação.\n");
        } else {
            printf("Você conjura o Escudo! Sua defesa está melhorada.\n");
            jogador->escudoAtivo = 1;
            jogador->duracaoEscudo = 2;
            jogador->PM -= CUSTO_ESCUDO;
        }
    } else {
        printf("Você não tem PM suficiente para conjurar o Escudo.\n");
    }
}

void executarCura(Jogador* jogador) {
    if (jogador->PM >= CUSTO_CURA) {
        printf("Você conjura a Cura! Você recuperou 1 ponto de vida.\n");
        jogador->PV++;
        jogador->PM -= CUSTO_CURA;
    } else {
        printf("Você não tem PM suficiente para conjurar a Cura.\n");
    }
}

void executarAcao(Jogador* jogador, Sessao* sessaoAtual) {
    int escolha;

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
                executarEscudo(jogador, sessaoAtual);
                break;
            case 4:
                executarCura(jogador);
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }

    } while (escolha < 1 || escolha > 4);
}

void jogarJogo(Sessao* inicioSessao) {
    Jogador jogador = {5, 10, 0, 0};
    Sessao* sessaoAtual = inicioSessao;

    printf("%s\n", sessaoAtual->descricao);

    while (1) {
        if (sessaoAtual->inimigo) {
            if (sessaoAtual->proxima != NULL) {
                sessaoAtual = sessaoAtual->proxima;
                printf("%s\n", sessaoAtual->descricao);
            } else {
                printf("Parabéns! Você explorou todas as cavernas com sucesso!\n");
                break;
            }
        } else {
            executarAcao(&jogador, sessaoAtual);
            if (jogador.PV <= 0) {
                printf("Você perdeu todas as vidas. Fim do jogo.\n");
                break;
            }
        }
    }

    destruirSessoes(inicioSessao);
}

void inicializarCavernaIluminada(Sessao* inicioSessao) {
    Sessao* sessao1 = criarSessao("Você entra em uma caverna iluminada por cristais brilhantes. O ar é carregado com uma energia mística.");
    Sessao* sessao2 = criarSessao("Você se depara com um caminho dividido. À esquerda, uma passagem estreita; à direita, uma sala iluminada por cristais pulsantes.");
    Sessao* sessao3 = criarSessao("Uma criatura feita de sombras emerge das sombras. Ela parece hostil!");
    Sessao* sessao4 = criarSessao("Você derrotou a criatura de sombras. Seu caminho está livre novamente.");
    Sessao* sessao5 = criarSessao("Você entra na sala iluminada por cristais pulsantes. Uma sensação de calma toma conta de você.");

    sessao1->proxima = sessao2;
    sessao2->anterior = sessao1;
    sessao2->proxima = sessao3;
    sessao3->anterior = sessao2;
    sessao3->proxima = sessao4;
    sessao4->anterior = sessao3;
    sessao4->proxima = sessao5;
    sessao5->anterior = sessao4;

    *inicioSessao = *sessao1;
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    srand((unsigned int)time(NULL));

    int opcao;

    printf("**************************************************\n");
    printf("Bem-vindo ao Jogo de Exploração de Cavernas!\n");
    printf("**************************************************\n");
    printf("Hoje é o seu aniversário e você decide se presentear com uma aventura única.\n");
    printf("Escolha uma opção:\n1. Iniciar jogo\n2. Sair\n");
    scanf("%d", &opcao);

    if (opcao == 1) {
        Sessao inicioSessao;
        inicializarCavernaIluminada(&inicioSessao);
        jogarJogo(&inicioSessao);
    } if (opcao == 2) {
        printf("**************************************************\n");
        printf("Obrigado por jogar! Até a próxima aventura!\n");
        printf("**************************************************\n");
        return 0;
    }
}
