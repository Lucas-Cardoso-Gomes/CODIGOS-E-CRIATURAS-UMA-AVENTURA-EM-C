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
    Sessao* sessao1 = criarSessao("\nVocê está a bordo de um carroça, sendo levado para sua execução em Helgen.");
    Sessao* sessao2 = criarSessao("Antes de sua execução, um dragão ataca Helgen, causando caos e destruição.");
    sessao2->armadilha = 1;
    Sessao* sessao3 = criarSessao("\n\tDurante o caos, um soldado Imperial liderado por Hadvar oferece ajuda para escapar.");
    Sessao* sessao4 = criarSessao("\nVocês dois correm por escombros para se abrigar enquanto o dragão queima a cidade nas suas costas.");
    Sessao* sessao5 = criarSessao("\nMais adiante o Hadvar diz para se abrigarem nas masmorras, onde o dragão não irá alcançar.");
    Sessao* sessao6 = criarSessao("\nAo entrar nas masmorras, Hadvar te solta das algemas e diz que para sobreviver, vocês teram que trabalhar juntos");
    Sessao* sessao7 = criarSessao("\nVocê vasculha alguns baus dentro da masmorra e percebe equipamentos da guarda, você se equipa para fugir");
    Sessao* sessao8 = criarSessao("\nEnquanto fogem, você percebem os stormcloaks um exercito que luta pela libertação do seu povo e uma batalha inicia!");
    sessao8->inimigo = 1;
    Sessao* sessao9 = criarSessao("\nApós a batalha, vocês andam mais discretamente, porém são muitos inimigos!");
    sessao9->inimigo = 1;
    Sessao* sessao10 = criarSessao("\nAvançando mais a masmorra, você encontram a camara de tortura, onde um mago e seu assitente batalham contra os stormcloaks, você deve ajuda-los!");
    sessao10->inimigo = 1;
    Sessao* sessao11 = criarSessao("\nApós salva-los, eles fornecem mais equipamento e poções de cura para seguir com a fuga.");
    Sessao* sessao12 = criarSessao("\nVocê segue masmorra a dentro, agora não parece mais como uma construção, e sim uma caverna.");
    Sessao* sessao13 = criarSessao("\nMais inimigos aparecem no seu caminho, a batalha é inevitavel");
    sessao13->inimigo = 1;
    Sessao* sessao14 = criarSessao("\nSeguindo a caverna, não tem mais presenã humana, agora você deve ter cuidado com as bestas que a habitam");
    Sessao* sessao15 = criarSessao("\nDurante a caminha, vocês notam teias de aranha cada vez mais presente onde vocês estão, quando derrepente, aranha gigantes surgem nas paredes...");
    sessao15->inimigo = 1;
    Sessao* sessao16 = criarSessao("\nSem tempo para descanso, após a batalha com a aranha, urso aparece na saida da caverna!");
    sessao16->inimigo = 1;
    Sessao* sessao17 = criarSessao("\nFinalmente, a Luz do dia nunca foi tão boa, vocês forem sã em salvo!");
    Sessao* sessao18 = criarSessao("\nAgora cada um de vocês segue sem rumo, você tem uma grande historia pela frente!");

    sessao1->proxima = sessao2;
    sessao2->anterior = sessao1;
    sessao2->proxima = sessao3;
    sessao3->anterior = sessao2;
    sessao3->proxima = sessao4;
    sessao4->anterior = sessao3;
    sessao4->proxima = sessao5;
    sessao5->anterior = sessao4;
    sessao5->proxima = sessao6;
    sessao6->anterior = sessao5;
    sessao6->proxima = sessao7;
    sessao7->anterior = sessao6;
    sessao7->proxima = sessao8;
    sessao8->anterior = sessao7;
    sessao8->proxima = sessao9;
    sessao9->anterior = sessao8;
    sessao9->proxima = sessao10;
    sessao10->anterior = sessao9;
    sessao10->proxima = sessao11;
    sessao11->anterior = sessao10;
    sessao11->proxima = sessao12;
    sessao12->anterior = sessao11;
    sessao12->proxima = sessao13;
    sessao13->anterior = sessao12;
    sessao13->proxima = sessao14;
    sessao14->anterior = sessao13;
    sessao14->proxima = sessao15;
    sessao15->anterior = sessao14;
    sessao15->proxima = sessao16;
    sessao16->anterior = sessao15;
    sessao16->proxima = sessao17;
    sessao17->anterior = sessao16;
    sessao17->proxima = sessao18;
    sessao18->anterior = sessao17;

    *inicioSessao = *sessao1;
}

void campanha2(Sessao* inicioSessao)/*Caverna Escura*/ {
    Sessao* sessao1 = criarSessao("\nVocê entra em uma caverna iluminada por cristais brilhantes. O ar é carregado com uma energia mística.");
    Sessao* sessao2 = criarSessao("Você se depara em um caminho longo, cada passo que você dá, menos brilho os cristais parecem prover.\nNa escuridão você percebe algo...");
    sessao2->armadilha = 1;
    Sessao* sessao3 = criarSessao("\n\tNo escuro, surge uma criatura. Ela parece hostil!");
    sessao3->inimigo = 1;
    Sessao* sessao4 = criarSessao("\nVocê derrotou a criatura. Seu caminho está livre novamente.");
    Sessao* sessao5 = criarSessao("\nVocê entra na sala iluminada por cristais pulsantes. Uma sensação de calma toma conta de você.");
    Sessao* sessao6 = criarSessao("\n... porém não por muito tempo, após ficar um tempo nessa sala, você escuta passos vindo da entrada da caverda, você está encuralado");
    Sessao* sessao7 = criarSessao("\nApós alguns minutos, o som chega mais perto e você percebe, um urso surge da entrada, a batalha por sua salvação é inevitável!");
    sessao7->inimigo = 1;
    Sessao* sessao8 = criarSessao("\nCom a grande vitória sobre o urso, você sai as pressas da caverna, sua exploração te levou para um lugar ""inesquecivel"", porém perigoso!");

    sessao1->proxima = sessao2;
    sessao2->anterior = sessao1;
    sessao2->proxima = sessao3;
    sessao3->anterior = sessao2;
    sessao3->proxima = sessao4;
    sessao4->anterior = sessao3;
    sessao4->proxima = sessao5;
    sessao5->anterior = sessao4;
    sessao5->proxima = sessao6;
    sessao6->anterior = sessao5;
    sessao6->proxima = sessao7;
    sessao7->anterior = sessao6;
    sessao7->proxima = sessao8;
    sessao8->anterior = sessao7;


    *inicioSessao = *sessao1;
}

void campanha3(Sessao* inicioSessao)/*Caverda dos goblins*/ {
    Sessao* sessao1 = criarSessao("\nVocê adentra a Caverna dos Goblins, usa missão é massacrar esses pequenos monstros.");
    Sessao* sessao2 = criarSessao("\nOs goblins espreitam você de longe, mas não tentam aproximar, parecem tramar algo...");
    sessao2->armadilha = 1;
    Sessao* sessao3 = criarSessao("\n\tVocê se livra da armadilha e segue a caverna.");
    Sessao* sessao4 = criarSessao("\n\tVocê encontra uma encruzilhada. Um grupo de goblins surge de uma passagem lateral.");
    sessao4->inimigo = 1;
    Sessao* sessao5 = criarSessao("\nApós derrotar os goblins, você encontra uma sala cheia de tesouros pequenos e reluzentes.");
    Sessao* sessao6 = criarSessao("\nContinuando sua exploração, você se depara com um goblin no fim de uma sala, ao tentar se aproximar você é surpreendido.");
    sessao6->armadilha = 1;
    Sessao* sessao7 = criarSessao("\n\tO goblin no fim sala é o líder goblin desta caverna, ele tenta correr para para outra passagem na caverna e você o segue...");
    sessao7->armadilha = 1;
    Sessao* sessao8 = criarSessao("\nApós se safar da armadilha, seu combate com o lider começa!");
    sessao8->inimigo = 1;
    Sessao* sessao9 = criarSessao("\n\tMais goblins surgem aos montes, todas as fisuras da caverna parecem ser passagem para eles...");
    sessao9->inimigo = 1;
    Sessao* sessao10 = criarSessao("\nVocê, finalmente, derrota todos os goblins, mas a caverna começa a desmoronar. É hora de escapar o mais rápido possível.");
    sessao10->armadilha = 1;
    Sessao* sessao11 = criarSessao("\nVocê escapa da Caverna dos Goblins a tempo. Agradecido, o líder goblin lhe presenteia com um item valioso como sinal de respeito.");

    sessao1->proxima = sessao2;
    sessao2->anterior = sessao1;
    sessao2->proxima = sessao3;
    sessao3->anterior = sessao2;
    sessao3->proxima = sessao4;
    sessao4->anterior = sessao3;
    sessao4->proxima = sessao5;
    sessao5->anterior = sessao4;
    sessao5->proxima = sessao6;
    sessao6->anterior = sessao5;
    sessao6->proxima = sessao7;
    sessao7->anterior = sessao6;
    sessao7->proxima = sessao8;
    sessao8->anterior = sessao7;
    sessao8->proxima = sessao9;
    sessao9->anterior = sessao8;
    sessao9->proxima = sessao10;
    sessao10->anterior = sessao9;
    sessao10->proxima = sessao11;
    sessao11->anterior = sessao10;

    *inicioSessao = *sessao1;
}

void campanha4(Sessao* inicioSessao)/*Caverna dos Orcs*/ {
    Sessao* sessao1 = criarSessao("\nVocê entra na Caverna dos Orcs, o cheiro de suor e metal impregna o ar.");
    Sessao* sessao2 = criarSessao("\nVocê se depara com um grupo de orcs treinando. Eles notam sua presença e se preparam para o combate.");
    sessao2->inimigo = 1;
    Sessao* sessao3 = criarSessao("\nCom os orcs derrotados, porém mais orcs chegam para enfrentar você!");
    sessao3->inimigo = 1;
    Sessao* sessao4 = criarSessao("\nVocê derrota os orcs e avança na caverna.\nNessa sela você encontra mais orcs, a luta é inevitavel.");
    sessao4->inimigo = 1;
    Sessao* sessao5 = criarSessao("\nApós a batalha, ao revistar os Orcs, você acham um Mapa no bolso de um deles, contendo informações da caverda.\nSeguindo o mapa, você chega à sala do líder orc. Ele está pronto para a batalha.");
    sessao5->inimigo = 1;
    Sessao* sessao6 = criarSessao("\n\tA batalha contra o líder orc é intensa, mas você consegue derrotá-lo. A caverna dos orcs agora está sob seu controle.");
    Sessao* sessao7 = criarSessao("\nVocê encontra uma passagem secreta que leva a uma sala com todos os tesouros roubados pelos orcs, porém ela está protegida.");
    Sessao* sessao8 = criarSessao("\nVocê decide enfrentar os perigos e recuperar o tesouro. Orcs guardiões desafiam sua bravura.");
    sessao8->inimigo = 1;
    Sessao* sessao9 = criarSessao("\nApós vencer, o tesouro está ao seu alcance. Você retorna à entrada da caverna com suas recompensas.");
    Sessao* sessao10 = criarSessao("\nVocê sai vitorioso da Caverna dos Orcs, sendo aclamado como herói. Sua jornada continua em terras desconhecidas.");

    sessao1->proxima = sessao2;
    sessao2->anterior = sessao1;
    sessao2->proxima = sessao3;
    sessao3->anterior = sessao2;
    sessao3->proxima = sessao4;
    sessao4->anterior = sessao3;
    sessao4->proxima = sessao5;
    sessao5->anterior = sessao4;
    sessao5->proxima = sessao6;
    sessao6->anterior = sessao5;
    sessao6->proxima = sessao7;
    sessao7->anterior = sessao6;
    sessao7->proxima = sessao8;
    sessao8->anterior = sessao7;
    sessao8->proxima = sessao9;
    sessao9->anterior = sessao8;
    sessao9->proxima = sessao10;
    sessao10->anterior = sessao9;

    *inicioSessao = *sessao1;
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    srand((unsigned int)time(NULL));

    int gamemode;
    do{
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
        printf("Escolha uma opção:\n1. Main Quest (Longa-Hístoria)\n2. Caverna Escura (Fácil)\n3. Caverna dos goblins (Média)\n4. Caverna dos Orcs (Dificil) \n5. Sair\n");
        scanf("%d", &gamemode);

        if (gamemode == 1) {
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha1(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 2) {
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha2(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 3) {
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha3(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 4) {
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha4(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 5) {
            printf("**************************************************\n");
            printf("Obrigado por jogar! Até a próxima aventura!\n");
            printf("**************************************************\n");
            return 0;
        }
    system("cls");
    } while (gamemode > 0 || gamemode <6);

    system("pause");
    return 0;
}
