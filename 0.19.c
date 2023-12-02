#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <windows.h>
#include <conio.h>

#define CUSTO_BOLA_DE_FOGO 4
#define CUSTO_ESCUDO 3
#define CUSTO_CURA 3

typedef struct Sessao {
    char descricao[500];
    int inimigo;
    int mod_inimigo;
    int horda_inimigo;
    int armadilha;
    int suprimentos;

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
        printf("\nErro ao alocar memória para iniciar a sessão.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(novaSessao->descricao, descricao, sizeof(novaSessao->descricao));
    novaSessao->inimigo = 0;
    novaSessao->mod_inimigo = 0;
    novaSessao->horda_inimigo = 0;
    novaSessao->armadilha = 0;
    novaSessao->suprimentos = 0;
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

void mostrarInformacoesCombate(Jogador* jogador) {
    printf("**************************************************\n");
    printf("\nVidas: %d | Pontos de Magia (PM): %d", jogador->HP, jogador->MP);
    if (jogador->escudoAtivo) {
        printf(" | Escudo ativo (Duração restante: %d turnos)\n", jogador->duracaoEscudo);
    } else {
        printf("\n");
    }
}

void mostrarOpcoes(Sessao* sessaoAtual) {
    printf("Escolha sua ação:\n");
    printf("1. Atacar normalmente\n");
    printf("2. Bola de Fogo (%d MP)\n", CUSTO_BOLA_DE_FOGO);
    printf("3. Escudo (%d MP)\n", CUSTO_ESCUDO);
    printf("4. Cura (%d MP)\n", CUSTO_CURA);
}

int rolarD20() {
    return rand() % 20 + 1;
}

void dano(Jogador* jogador, int valorDano) {
    int dano = valorDano;

    if (jogador->escudoAtivo == 1) {
        dano -= 1;
        printf("\nO Escudo dissipou-se. Sua defesa retorna ao normal.\n");
    }

    jogador->HP -= dano;

    jogador->escudoAtivo = 0;
    jogador->duracaoEscudo = 0;
}

void executarAtaque(Jogador* jogador, Sessao* sessaoAtual) {

    printf("**************************************************\n");

    printf("\nRolando o dado D20...\n");
    int resultadoDado = rolarD20();
    printf("Resultado do dado: %d\n", resultadoDado);

    if ((resultadoDado-= sessaoAtual->mod_inimigo) >= 14) {
        printf("\nVocê derrotou o inimigo e avança com cuidado.\n");
        sessaoAtual->inimigo = 0;
    } else if ((resultadoDado-= sessaoAtual->mod_inimigo) >= 3 && (resultadoDado-= sessaoAtual->mod_inimigo) <= 13) {
        printf("\nVocê errou o ataque!\n");

        if (sessaoAtual->mod_inimigo > 1){
            printf("\nO Inimigo é poderoso, você irá precisar de mais precisão em seus ataques!\n");
        }
    } else {
        printf("\nVocê escorregou e o inimigo te ataca! Você perde 1 HP.\n");
        dano(jogador, 1);

        if (sessaoAtual->mod_inimigo > 1){
            printf("\nO Inimigo é poderoso, você irá precisar de mais precisão em seus ataques!\n");
        }
    }

    if (jogador->HP <= 0) {
        printf("\nVocê perdeu todas as vidas. Fim do jogo.\n");
        exit(EXIT_SUCCESS);
    }

    printf("**************************************************\n");

    if (sessaoAtual->inimigo == 1) {
        printf("\nO inimigo contra-ataca!\n");
        int resultadoDadoInimigo = rolarD20();

        if (sessaoAtual->mod_inimigo > 1){
            resultadoDadoInimigo += sessaoAtual->mod_inimigo;
        }

        printf("\nResultado do dado inimigo: %d\n", resultadoDadoInimigo);

        if (resultadoDadoInimigo >= 19) {
            printf("\nO inimigo acertou você e causou 2 de dano!\n");
            dano(jogador, 2);
        } else if (resultadoDadoInimigo >= 12) {
            printf("\nO inimigo acertou você e causou 1 de dano.\n");
            dano(jogador, 1);
        } else if (resultadoDadoInimigo <= 2) {
            printf("\nO inimigo escorregou e você contra-atacou!\n");
            sessaoAtual->inimigo = 0;
        } else {
            printf("\nO inimigo errou o ataque..\n");
        }
    }

    if (jogador->HP <= 0) {
        printf("\nVocê perdeu todas as vidas. Fim do jogo.\n");
        exit(EXIT_SUCCESS);
    }

    if (jogador->escudoAtivo) {
        jogador->duracaoEscudo--;
        if (jogador->duracaoEscudo == 0) {
            printf("\nO Escudo dissipou-se. Sua defesa retorna ao normal.\n");
            jogador->escudoAtivo = 0;
        }
    }
}

void executarBolaDeFogo(Jogador* jogador, Sessao* sessaoAtual) {

    printf("**************************************************\n");

    if (jogador->MP >= CUSTO_BOLA_DE_FOGO) {
        printf("\nVocê conjura a Bola de Fogo!\n");
        int resultadoDado = rolarD20();
        printf("\nResultado do dado %d\n", resultadoDado);

        if (sessaoAtual->mod_inimigo > 1){
            resultadoDado -= sessaoAtual->mod_inimigo;
        }

        if (resultadoDado > 10) {
            printf("\nVocê acertou o inimigo e o derrotou.\n");
            sessaoAtual->inimigo = 0;
        } else {
            printf("\nA Bola de Fogo errou o alvo.\n");

            if (sessaoAtual->mod_inimigo > 1){
                printf("\nO Inimigo é poderoso, você irá precisar de mais precisão em seus ataques!\n");
            }
        }

        jogador->MP -= CUSTO_BOLA_DE_FOGO;
    } else {
        printf("\nVocê não possui PM suficiente para conjurar a Bola de Fogo.\n");
    }

    printf("**************************************************\n");

    if (sessaoAtual->inimigo == 1) {
        printf("\nO inimigo contra-ataca!\n");
        int resultadoDadoInimigo = rolarD20();
        printf("Resultado do dado inimigo: %d\n", resultadoDadoInimigo);

        if (resultadoDadoInimigo >= 19) {
            printf("\nO inimigo acertou você e causou 2 de dano!\n");
            dano(jogador, 2);
        } else if (resultadoDadoInimigo >= 12) {
            printf("\nO inimigo acertou você e causou 1 de dano.\n");
            dano(jogador, 1);
        } else if (resultadoDadoInimigo <= 2) {
            printf("\nO inimigo escorregou e você contra-atacou!\n");
            sessaoAtual->inimigo = 0;
        } else {
            printf("\nO inimigo errou o ataque..\n");
        }
    }

    if (jogador->HP <= 0) {
        printf("\nVocê perdeu todas as vidas. Fim do jogo.\n");
        exit(EXIT_SUCCESS);
    }

    if (jogador->escudoAtivo) {
        jogador->duracaoEscudo--;
        if (jogador->duracaoEscudo == 0) {
            printf("\nO Escudo dissipou-se. Sua defesa retorna ao normal.\n");
            jogador->escudoAtivo = 0;
        }
    }

}

void executarEscudo(Jogador* jogador) {

    printf("**************************************************\n");

    if (jogador->MP >= CUSTO_ESCUDO) {
        if (jogador->escudoAtivo) {
            printf("\nVocê já conjurou o Escudo. Escolha outra ação.\n");
        } else {
            printf("\nVocê conjura o Escudo! Sua defesa está melhorada.\n");
            jogador->escudoAtivo = 1;
            jogador->duracaoEscudo = 3;
            jogador->MP -= CUSTO_ESCUDO;
        }
    } else {
        printf("\nVocê não possui PM suficiente para conjurar o Escudo.\n");
    }
}

void executarCura(Jogador* jogador) {

    printf("**************************************************\n");

    if (jogador->MP >= CUSTO_CURA) {
        printf("\nVocê conjura a Cura! e recupera 1 ponto de vida.\n");
        jogador->HP++;
        jogador->MP -= CUSTO_CURA;
    } else {
        printf("\nVocê não possui PM suficiente para conjurar a Cura.\n");
    }
}

void executarCombate(Jogador* jogador, Sessao* sessaoAtual) {

    int escolha;

    if (sessaoAtual->inimigo == 1) {
        do {
            mostrarInformacoesCombate(jogador);
            mostrarOpcoes(sessaoAtual);

            scanf("%d", &escolha);

            system("cls");

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
                    printf("\nOpção inválida. Tente novamente.\n");
            }

        } while (escolha < 1 || escolha > 4);
    }

    if (sessaoAtual->horda_inimigo > 0)
    {
        printf("\nEles são muitos, continue lutando por sua vida!\n");
        sessaoAtual->inimigo = 1;
        sessaoAtual->horda_inimigo -= 1;
    }

}

void executarArmadilha(Jogador* jogador, Sessao* sessaoAtual) {

    mostrarInformacoesCombate(jogador);

    printf("**************************************************\n");

    printf("\nVocê se depara com uma armadilha!\n");

    int opcao;
    printf("\nEscolha uma opção:\n1. Usar escudo e tentar passar\n2. Se curar\n3. Tentar passar\n");
    scanf("%d", &opcao);

    system("cls");

    switch (opcao)
    {
        case 1:
            executarEscudo(jogador);
            printf("\nVocê conjura o escudo e tenta passar pela armadilha...\n");
            break;
        case 2:
            executarCura(jogador);
            printf("\nVocê conjura o cura e tenta passar pela armadilha...\n");
            break;
        case 3:
            printf("\nVocê tenta passar pela armadilha...\n");
            break;
    }

    int resultadoDado = rolarD20();
    printf("\nResultado do dado %d\n", resultadoDado);

    if (resultadoDado >= 15) {
        printf("\nVocê conseguiu desarmar a armadilha e sair ileso!\n");
        sessaoAtual->armadilha=0;
    } else if (resultadoDado >= 7) {
        printf("\nVocê passou pela armadilha, mas sofreu um pequeno ferimento. Perdeu 1 ponto de vida.\n");
        dano(jogador, 1);
        sessaoAtual->armadilha=0;
    } else if (resultadoDado >= 2) {
        printf("\nVocê está preso na armadilha e sofreu um pequeno ferimento. Perdeu 1 ponto de vida.\n");
        dano(jogador, 1);
    } else {
        printf("\nVocê está preso na armadilha e sofreu um ferimento grave. Perdeu 2 ponto de vida.\n");
        dano(jogador, 2);
    }
    if (jogador->HP <= 0) {
            printf("\nVocê perdeu todas as vidas. Fim do jogo.\n");
            exit(EXIT_SUCCESS);
    }

    if (jogador->escudoAtivo) {
        jogador->duracaoEscudo--;
        if (jogador->duracaoEscudo == 0) {
            printf("\nO Escudo dissipou-se. Sua defesa retorna ao normal.\n");
            jogador->escudoAtivo = 0;
        }
    }
}

void executarSuprimentos(Jogador* jogador, Sessao* sessaoAtual) {

    printf("\nVocê encontrou suprimentos durante sua jornada, que tenham boa utilidade!\n");

    int resultadoDado = rolarD20();
    printf("\nSorte no dado %d\n", resultadoDado);

    if (resultadoDado >= 15) {
        printf("\nVocê recebe um Buff de +3 HP e 10 MP!\n");
        jogador->HP += 3;
        jogador->MP += 10;
    } else if (resultadoDado >= 7) {
        printf("\nVocê recebe um Buff de +2 HP e 6 MP!\n");
        jogador->HP += 2;
        jogador->MP += 6;
    } else if (resultadoDado >= 2) {
        printf("\nVocê recebe um Buff de +1 HP e 3 MP!\n");
        jogador->HP += 1;
        jogador->MP += 3;
    } else if (resultadoDado == 2) {
        printf("\nVocÊ recebe um Buff de +2 MP!\n");
        jogador->MP += 2;
    }
    else {
        printf("\nOs suprimentos estão estragados, nada poderá ser usado.\n");
    }

    sessaoAtual->suprimentos=0;
}

void jogarJogo(Sessao* inicioSessao, Jogador* jogador) {

    Sessao* sessaoAtual = inicioSessao;

    while (sessaoAtual != NULL) {

        printf("%s\n", sessaoAtual->descricao);

        while (sessaoAtual->inimigo) {
            executarCombate(jogador, sessaoAtual);
            if (jogador->HP <= 0) {
                printf("\nVocê perdeu todas as vidas. Fim do jogo.\n");
                break;
            }
        }

        while (sessaoAtual->armadilha) {
            executarArmadilha(jogador, sessaoAtual);
            if (jogador->HP <= 0) {
                printf("\nVocê perdeu todas as vidas. Fim do jogo.\n");
                break;
            }
        }

        while (sessaoAtual->suprimentos) {
            executarSuprimentos(jogador, sessaoAtual);
            if (jogador->HP <= 0) {
                printf("\nVocê perdeu todas as vidas. Fim do jogo.\n");
                break;
            }
        }

        sessaoAtual = sessaoAtual->proxima;
    }

    printf("\nParabéns! Você explorou a caverna com sucesso!\n");
    destruirSessoes(inicioSessao);

    system("pause");
}

void intro0(){
    printf(R"EOF(

                                                         / ___| /_/    __| |(_)  __ _   ___   ___    ___   / ___| _ __ (_)  __ _ | |_  _   _  _ __  __ _  ___  _
                                                        | |    / _ \  / _` || | / _` | / _ \ / __|  / _ \ | |    | '__|| | / _` || __|| | | || '__|/ _` |/ __|(_)
                                                        | |___| (_) || (_| || || (_| || (_) |\__ \ |  __/ | |___ | |   | || (_| || |_ | |_| || |  | (_| |\__ \ _
                                                         \____|\___/  \__,_||_| \__, | \___/ |___/  \___|  \____||_|   |_| \__,_| \__| \__,_||_|   \__,_||___/(_)
                                                                                |___/
                                                         _   _                                               _                                           ____
                                                        | | | | _ __ ___    __ _    __ _ __   __ ___  _ __  | |_  _   _  _ __  __ _    ___  _ __ ___    / ___|
                                                        | | | || '_ ` _ \  / _` |  / _` |\ \ / // _ \| '_ \ | __|| | | || '__|/ _` |  / _ \| '_ ` _ \  | |
                                                        | |_| || | | | | || (_| | | (_| | \ V /|  __/| | | || |_ | |_| || |  | (_| | |  __/| | | | | | | |___
                                                         \___/ |_| |_| |_| \__,_|  \__,_|  \_/  \___||_| |_| \__| \__,_||_|   \__,_|  \___||_| |_| |_|  \____|

                                                                                                                _   __,----'~~~~~~~~~`-----.__
                                                                                                            .  .    `//====-              ____,-'~`
                                                                                            -.            \_|// .   /||\\  `~~~~`---.___./
                                                                                      ______-==.       _-~o  `\/    |||  \\           _,'`
                                                                                __,--'   ,=='||\=_    ;_,_,/ _-'|-   |`\   \\        ,'
                                                                             _-'      ,='    | \\`.    '',/~7  /-   /  ||   `\.     /
                                                                           .'       ,'       |  \\  \_  "  /  /-   /   ||      \   /
                                                                          / _____  /         |     \\.`-_/  /|- _/   ,||       \ /
                                                                         ,-'     `-|--'~~`--_ \     `==-/  `| \'--===-'       _/`
                                                                                   '         `-|      /|    )-'\~'      _,--"'
                                                                                               '-~^\_/ |    |   `\_   ,^             /\
                                                                                                    /  \     \__   \/~               `\__
                                                                                                _,-' _/'\ ,-'~____-'`-/                 ``===\
                                                                                               ((->/'    \|||' `.     `\.  ,                _||
                                                                                 ./                       \_     `\      `~---|__i__i__\--~'_/
                                                                                <_n_                     __-^-_    `)  \-.______________,-~'
                                                                                 `B'\)                  ///,-'~`__--^-  |-------~~~~^'
                                                                                 /^>                           ///,--~`-\
                                                                                `  `
)EOF");

    Sleep(3300);
}

void intro1(){
printf(R"EOF(

                                                             _______  _______ _________ _          _______           _______  _______ _________
                                                            (       )(  ___  )\__   __/( (    /|  (  ___  )|\     /|(  ____ \(  ____ \\__   __/
                                                            | () () || (   ) |   ) (   |  \  ( |  | (   ) || )   ( || (    \/| (    \/   ) (
                                                            | || || || (___) |   | |   |   \ | |  | |   | || |   | || (__    | (_____    | |
                                                            | |(_)| ||  ___  |   | |   | (\ \) |  | |   | || |   | ||  __)   (_____  )   | |
                                                            | |   | || (   ) |   | |   | | \   |  | | /\| || |   | || (            ) |   | |
                                                            | )   ( || )   ( |___) (___| )  \  |  | (_\ \ || (___) || (____/\/\____) |   | |
                                                            |/     \||/     \|\_______/|/    )_)  (____\/_)(_______)(_______/\_______)   )_(
                                                                                               ,--,  ,.-.
                                                                                ,                   \,       '-,-`,'-.' | ._
                                                                               /|           \    ,   |\         }  )/  / `-,',
                                                                               [ '          |\  /|   | |        /  \|  |/`  ,`
                                                                               | |       ,.`  `,` `, | |  _,...(   (      _',
                                                                               \  \  __ ,-` `  ,  , `/ |,'      Y     (   \_L\
                                                                                \  \_\,``,   ` , ,  /  |         )         _,/
                                                                                 \  '  `  ,_ _`_,-,<._.<        /         /
                                                                                  ', `>.,`  `  `   ,., |_      |         /
                                                                                    \/`  `,   `   ,`  | /__,.-`    _,   `\
                                                                                -,-..\  _  \  `  /  ,  / `._) _,-\`       \
                                                                                 \_,,.) /\    ` /  / ) (-,, ``    ,        |
                                                                                ,` )  | \_\       '-`  |  `(               \
                                                                               /  /```(   , --, ,' \   |`<`    ,            |
                                                                              /  /_,--`\   <\  V /> ,` )<_/)  | \      _____)
                                                                        ,-, ,`   `   (_,\ \    |   /) / __/  /   `----`
                                                                       (-, \           ) \ ('_.-._)/ /,`    /
                                                                       | /  `          `/ \\ V   V, /`     /
                                                                    ,--\(        ,     <_/`\\     ||      /
                                                                   (   ,``-     \/|         \-A.A-`|     /
                                                                  ,>,_ )_,..(    )\          -,,_-`  _--`
                                                                 (_ \|`   _,/_  /  \_            ,--`
                                                                  \( `   <.,../`     `-.._   _,-`
                                                                   `                      ```


)EOF");
Sleep(3300);
system("cls");
}

void campanha1(Sessao* inicioSessao)/*Main Quest*/ {
    Sessao* sessao1 = criarSessao("\nVocê está a bordo de um carroça, sendo levado para sua execução em Helgen.");
    Sessao* sessao2 = criarSessao("Antes de sua execução, um dragão ataca Helgen, causando caos e destruição.");
    sessao2->armadilha = 0;
    Sessao* sessao3 = criarSessao("\nDurante o caos, um soldado Imperial, chamado Hadvar oferece ajuda para escapar.");
    Sessao* sessao4 = criarSessao("\nVocês dois correm por escombros para se abrigar enquanto o dragão queima a cidade nas suas costas.");
    Sessao* sessao5 = criarSessao("\nMais adiante o Hadvar diz para se abrigarem nas masmorras, onde o dragão não irá alcaçar.");
    Sessao* sessao6 = criarSessao("\nAo entrar nas masmorras, Hadvar te solta das algemas e diz que para sobreviver, vocês terão que trabalhar juntos");
    Sessao* sessao7 = criarSessao("\nVocê vasculha alguns baús dentro da masmorra e percebe equipamentos da guarda, você se equipa para fugir");
    Sessao* sessao8 = criarSessao("\nEnquanto fogem, você percebem os stormcloaks estavam invadindo a cidade para resgatar Ulfric Stormcloaks, que estava preso na cidade! (eles formam um exercito que luta pela libertação do seu povo pela invasão do império)\nVocês travam um combate com os Stormcloaks!");
    sessao8->inimigo = 1;
    Sessao* sessao9 = criarSessao("\nApós a batalha, vocês andam mais discretamente, porém são muitos inimigos em seu caminho!");
    sessao9->inimigo = 1;
    Sessao* sessao10 = criarSessao("\nAvançando mais a masmorra, você encontram a câmara de tortura, onde um mago e seu assistente batalham contra os stormcloaks, você deve ajuda-los!");
    sessao10->inimigo = 1;
    Sessao* sessao11 = criarSessao("\nApós salvá-los, eles fornecem mais equipamento e poções para que possam seguir com a fuga.");
    sessao11->suprimentos = 1;
    Sessao* sessao12 = criarSessao("\nVocê segue masmorra adentro, agora não parece mais como uma construção, e sim uma caverna.");
    Sessao* sessao13 = criarSessao("\nMais inimigos aparecem no seu caminho, a batalha é inevitável");
    sessao13->inimigo = 1;
    Sessao* sessao14 = criarSessao("\nSeguindo a caverna, não tem mais presença humana, agora você deve ter cuidado com as bestas que a habitam");
    Sessao* sessao15 = criarSessao("\nDurante a caminhada, vocês notam teias de aranha cada vez mais presente onde vocês estão, quando de repente, aranha gigantes surgem nas paredes...");
    sessao15->inimigo = 1;
    Sessao* sessao16 = criarSessao("\nSem tempo para descanso, após a batalha com a aranha, urso aparece na saída da caverna!");
    sessao16->inimigo = 1;
    Sessao* sessao17 = criarSessao("\nFinalmente, a Luz do dia nunca foi tão boa, vocês fogem sá em salvo!");
    Sessao* sessao18 = criarSessao("\nAgora cada um de vocês segue sem rumo, você tem uma grande história pela frente!");

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

void intro2(){
    system("cls");
    printf(R"EOF(
                              ___  _     _                           _                                 _                                      __                 _
                             / _ \| |   (_)                         | |                               | |                                    / _|               | |
                            / /_\ \ |__  _ ___ _ __ ___   ___     __| | __ _ ___   ___  ___  _ __ ___ | |__  _ __ __ _ ___   _ __  _ __ ___ | |_ _   _ _ __   __| | __ _ ___
                            |  _  | '_ \| / __| '_ ` _ \ / _ \   / _` |/ _` / __| / __|/ _ \| '_ ` _ \| '_ \| '__/ _` / __| | '_ \| '__/ _ \|  _| | | | '_ \ / _` |/ _` / __|
                            | | | | |_) | \__ \ | | | | | (_) | | (_| | (_| \__ \ \__ \ (_) | | | | | | |_) | | | (_| \__ \ | |_) | | | (_) | | | |_| | | | | (_| | (_| \__ \
                            \_| |_/_.__/|_|___/_| |_| |_|\___/   \__,_|\__,_|___/ |___/\___/|_| |_| |_|_.__/|_|  \__,_|___/ | .__/|_|  \___/|_|  \__,_|_| |_|\__,_|\__,_|___/
                                                                                                                            | |
                                                                                      _                                     |_|
                                                                                     / \
                                                                                    /   \              /'\       _
                                                               /\_..           /'.,/     \_         .,'   \     / \_
                                                              /     \         /            \      _/       \_  /    \     _
                                                             /        \__,.   /              \    /           \/.,   _|  _/ \
                                                            /             \_/                \  /',.,''\      \_ \_/  \/    \
                                                                                           _  \/   /    ',../',.\    _/      \
                                                                             /           _/M\  \  /    |         \  /.,/'\   _\
                                                                           _/           /MMMM\  \_     |          \/      \_/  \
                                                                          /      \     |MM  MM|   \__   \          \_       \   \_
                                                                                  \   /MM   MM|      \   \           \       \    \
                                                                                   \  |MM    MM\      \___            \_      \_   \
                                                                                    \|MMM_____MM|____.'  /\_            \       \   \_
                                                                                    /'.,___________...,,'   \            \   \        \
                                                                                   /       \          |      \    |__     \   \_       \
                                                                                 _/        |           \      \_     \     \    \       \_
                                                                                /                               \     \     \_   \        \
                                                                                                                 \     \      \   \__      \
                                                                                                                  \     \_     \     \      \
                                                                                                                   |      \     \     \      \
                                                                                                                    \            |            \

)EOF");
Sleep(3300);
system("cls");
}

void campanha2(Sessao* inicioSessao)/*Caverna Escura*/ {
    Sessao* sessao1 = criarSessao("\nVocê entra em uma caverna iluminada por cristais brilhantes. O ar é carregado com uma energia mística.");
    Sessao* sessao2 = criarSessao("Você se depara em um caminho longo, cada passo que você dá, menos brilho os cristais parecem prover.\nNa escuridão você percebe algo...");
    sessao2->armadilha = 1;
    Sessao* sessao3 = criarSessao("\nDevido ao barulho da armadilha, surge uma criatura do fundo da caverna. Ela parece hostil!");
    sessao3->inimigo = 1;
    Sessao* sessao4 = criarSessao("\nVocê derrotou a criatura, que no final, era apenas um urso. Seu caminho está livre novamente.");
    Sessao* sessao5 = criarSessao("\nVocê entra na sala iluminada por cristais pulsantes. Uma sensasão de calma toma conta de você.\nVocê aproveita para olhar em volta, e percebe equipamentos e suprimentos espalhado no chão.");
    sessao5->suprimentos = 1;
    Sessao* sessao6 = criarSessao("\n... porém não por muito tempo, após ficar um tempo nessa sala, você escuta passos vindo da entrada da caverna, você está encurralado");
    Sessao* sessao7 = criarSessao("\nApós alguns minutos, o som chega mais perto e você percebe, um grande Urso das Cavernas surge da entrada, a batalha por sua salvação é inevitível!");
    sessao7->inimigo = 1;
    sessao7->mod_inimigo = 3;
    Sessao* sessao8 = criarSessao("\nCom a grande vitória sobre o urso, você sai ás pressas da caverna, sua exploração te levou para um lugar ""inesquecível"", porém perigoso!");

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

void intro3(){
    system("cls");
printf(R"EOF(
                      ____        __      _   ________   ______       ____        ______       ____      _____         _____      ____     _____          _____      ____     ______        _____     _____
                     (    )      /  \    / ) (___  ___) (   __ \     / __ \      (_  __ \     / __ \    / ____\       / ___ \    / __ \   (_   _)        / ___ \    / __ \   (   __ \      / ___ \   / ____\
                     / /\ \     / /\ \  / /      ) )     ) (__) )   / /  \ \       ) ) \ \   / /  \ \  ( (___        / /   \_)  / /  \ \    | |         / /   \_)  / /  \ \   ) (__) )    / /   \_) ( (___
                    ( (__) )    ) ) ) ) ) )     ( (     (    __/   ( ()  () )     ( (   ) ) ( ()  () )  \___ \      ( (  ____  ( ()  () )   | |        ( (  ____  ( ()  () ) (    __/    ( (  ____   \___ \
                     )    (    ( ( ( ( ( (       ) )     ) \ \  _  ( ()  () )      ) )  ) ) ( ()  () )      ) )     ( ( (__  ) ( ()  () )   | |   __   ( ( (__  ) ( ()  () )  ) \ \  _   ( ( (__  )      ) )
                    /  /\  \   / /  \ \/ /      ( (     ( ( \ \_))  \ \__/ /      / /__/ /   \ \__/ /   ___/ /       \ \__/ /   \ \__/ /  __| |___) )   \ \__/ /   \ \__/ /  ( ( \ \_))   \ \__/ /   ___/ /
                   /__(  )__\ (_/    \__/       /__\     )_) \__/    \____/      (______/     \____/   /____/         \____/     \____/   \________/     \____/     \____/    )_) \__/     \____/   /____/

                                                                                                     _..
                                                                                                    .'   `",
                                                                                                   ;        \
                                                                                            .---._; ^,       ;
                                                                                         .-'      ;{ :  .-. ._;
                                                                                    .--""          \*'   o/ o/
                                                                                   /   ,  /         :    _`";
                                                                                  ;     \;          `.   `"+'
                                                                                  |      }    /    _.'T"--"\
                                                                                  :     /   .'.--""-,_ \    ;
                                                                                   \   /   /_         `,\   ;
                                                                                    : /   /  `-.,_      \`.  :
                                                                                    |;   {     .' `-     ; `, \
                                                                                    : \  `;   {  `-,__..-'   \ `}+=,
                                                                                     : \  ;    `.   `,        `-,\"
                                                                                     ! |\ `;     \}?\|}
                                                                                  .-'  | \ ;
                                                                                .'}/ i.'  \ `,
                                                                                ``''-'    /   \
                                                                                         /J|/{/
                                                                                           `'
                                                                                                                       ___
                                                                                                                     .';:;'.
                                                                                                                    /_' _' /\   __
                                                                                                                    ;a/ e= J/-'"  '.
                                                                                                                    \ ~_   (  -'  ( ;_ ,.
                                                                                                                     L~"'_.    -.  \ ./  )
                                                                                                                     ,'-' '-._  _;  )'   (
                                                                                                                   .' .'   _.'")  \  \(  |
                                                                                                                  /  (  .-'   __\{`', \  |
                                                                                                                 / .'  /  _.-'   "  ; /  |
                                                                                                                / /    '-._'-,     / / \ (
                                                                                                             __/ (_    ,;' .-'    / /  /_'-._
                                                                                                            `"-'` ~`  ccc.'   __.','     \j\L\
                                                                                                                             .='/|\7
                                                                                                                                ' `
)EOF");
Sleep(3300);
system("cls");
}

void campanha3(Sessao* inicioSessao)/*Caverna dos Golgorgs*/ {
    Sessao* sessao1 = criarSessao("\nVocê adentra a Caverna dos Golgorgs, sua missão é massacrar esses pequenos monstros.");
    Sessao* sessao2 = criarSessao("\nOs Golgorgs espreitam você de longe, mas não tentam aproximar, parecem tramar algo...");
    sessao2->armadilha = 1;
    Sessao* sessao3 = criarSessao("\nVocê consegue se livrar da armadilha e segue a caverna.");
    Sessao* sessao4 = criarSessao("\nVocê encontra uma encruzilhada. Um grupo de Golgorgs surge de uma passagem lateral.");
    sessao4->inimigo = 1;
    Sessao* sessao5 = criarSessao("\nApós derrotar os Golgorgs, você encontra uma sala cheia de tesouros pequenos e reluzentes e encontra alguns suprimentos que parecem ter sido roubados de outros aventureiros.");
    sessao5->suprimentos = 1;
    Sessao* sessao6 = criarSessao("\nContinuando sua exploração, você se depara com um Golgorgs no fim de uma sala, ao tentar se aproximar você é surpreendido.");
    sessao6->armadilha = 1;
    Sessao* sessao7 = criarSessao("\n\tO Golgorgs no fim sala é o líder Golgorgs desta caverna, ele tenta correr para outra passagem na caverna e você o segue...");
    sessao7->armadilha = 1;
    Sessao* sessao8 = criarSessao("\nApós se safar da armadilha, seu combate com o lider começa!");
    sessao8->inimigo = 1;
    sessao8->mod_inimigo = 2;
    Sessao* sessao9 = criarSessao("\n\tMais Golgorgs surgem aos montes, todas as fissuras da caverna parecem ser passagem para eles...");
    sessao9->inimigo = 1;
    sessao9->horda_inimigo = 3;
    Sessao* sessao10 = criarSessao("\nVocê finalmente, derrota todos os Golgorgs, mas a caverna começa a desmoronar. É hora de correr o mais rápido possível.");
    sessao10->armadilha = 1;
    Sessao* sessao11 = criarSessao("\nVocê escapa da Caverna dos Golgorgs a tempo. Sua missão de eliminação foi concluída com sucesso!.");

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

void intro4(){
    printf(R"EOF(

                              .,-:::::     ...    :::      .::.::: :::         :::::::-.      ...      .::::::.         ...    :::::::..     .,-::::: .::::::.
                            ,;;;'````'  .;;;;;;;. ';;,   ,;;;' ;;; ;;;          ;;,   `';, .;;;;;;;.  ;;;`    `      .;;;;;;;. ;;;;``;;;;  ,;;;'````';;;`    `
                            [[[        ,[[     \[[,\[[  .[[/   [[[ [[[          `[[     [[,[[     \[[,'[==/[[[[,    ,[[     \[[,[[[,/[[['  [[[       '[==/[[[[,
                            $$$        $$$,     $$$ Y$c.$$"    $$$ $$'           $$,    $$$$$,     $$$  '''    $    $$$,     $$$$$$$$$c    $$$         '''    $
                            `88bo,__,o,"888,_ _,88P  Y88P      888o88oo,.__      888_,o8P'"888,_ _,88P 88b    dP    "888,_ _,88P888b "88bo,`88bo,__,o,88b    dP
                              "YUMMMMMP" "YMMMMMP"    MP       MMM""""YUMMM      MMMMP"`    "YMMMMMP"   "YMmMY"       "YMMMMMP" MMMM   "W"   "YUMMMMMP""YMmMY"

                                                                                  __.--|~|--.__                               ,,;/;
                                                                                 /~     | |    ;~\                         ,;;;/;;'
                                                                                /|      | |    ;~\\                      ,;;;;/;;;'
                                                                               |/|      \_/   ;;;|\                    ,;;;;/;;;;'
                                                                               |/ \          ;;;/  )                 ,;;;;/;;;;;'
                                                                           ___ | ______     ;_____ |___....__      ,;;;;/;;;;;'
                                                                     ___.-~ \\(| \  \.\ \__/ /./ /:|)~   ~   \   ,;;;;/;;;;;'
                                                                 /~~~    ~\    |  ~-.     |   .-~: |//  _.-~~--,;;;;/;;;;;'
                                                                (.-~___     \.'|    | /-.__.-\|::::| //~     ,;;;;/;;;;;'
                                                                /      ~~--._ \|   /          `\:: |/      ,;;;;/;;;;;'
                                                             .-|             ~~|   |  /V""""V\ |:  |     ,;;;;/;;;;;' \
                                                            /                   \  |  ~`^~~^'~ |  /    ,;;;;/;;;;;'    ;
                                                           (        \             \|`\._____./'|/    ,;;;;/;;;;;'      '\
                                                          / \        \                             ,;;;;/;;;;;'     /    |
                                                         |            |                          ,;;;;/;;;;;'      |     |
                                                        |`-._          |                       ,;;;;/;;;;;'              \
                                                        |             /                      ,;;;;/;;;;;'  \              \__________
                                                        (             )                 |  ,;;;;/;;;;;'      |        _.--~
                                                         \          \/ \              ,  ;;;;;/;;;;;'       /(     .-~_..--~~~~~~~~~~
                                                         \__         '  `       ,     ,;;;;;/;;;;;'    .   /  \   / /~
                                                         /          \'  |`._______ ,;;;;;;/;;;;;;'    /   :    \/'/'       /|_/|   ``|
                                                        | _.-~~~~-._ |   \ __   .,;;;;;;/;;;;;;' ~~~~'   .'    | |       /~ (/\/    ||
                                                        /~ _.-~~~-._\    /~/   ;;;;;;;/;;;;;;;'          |    | |       / ~/_-'|-   /|
                                                        (/~         \| /' |   ;;;;;;/;;;;;;;;            ;   | |       (.-~;  /-   / |
                                                        |            /___ `-,;;;;;/;;;;;;;;'            |   | |      ,/)  /  /-   /  |
                                                         \            \  `-.`---/;;;;;;;;;' |          _'   |T|    /'('  /  /|- _/  //
                                                           \           /~~/ `-. |;;;;;''    ______.--~~ ~\  |u|  ,~)')  /   | \~-==//
                                                             \      /~(   `-\  `-.`-;   /|    ))   __-####\ |a|   (,   /|    |  \
                                                               \  /~.  `-.   `-.( `-.`~~ /##############'~~)| |   '   / |    |   ~\
                                                                \(   \    `-._ /~)_/|  /############'       |X|      /  \     \_\  `\
                                                                ,~`\  `-._  / )#####|/############'   /     |i|  _--~ _/ | .-~~____--'
                                                               ,'\  `-._  ~)~~ `################'           |o| ((~>/~   \ (((' -_
                                                             ,'   `-.___)~~      `#############             |n|           ~-_     ~\_
                                                         _.,'        ,'           `###########              |g|            _-~-__    (
                                                        |  `-.     ,'              `#########       \       | |          ((.-~~~-~_--~
                                                        `\    `-.;'                  `#####"                | |           "     ((.-~~
                                                          `-._   )               \     |   |        .       |  \                 "
                                                              `~~  _/                  |    \               |   `---------------------
                                                                |/~                `.  |     \        .     |  O    __.---------------
                                                                 |                   \ ;      \             |   _.-~
                                                                 |                    |        |            |  /  |
                                                                  |                   |         |           |/'
)EOF");
    Sleep(3300);
    system("cls");
}

void campanha4(Sessao* inicioSessao)/*Caverna dos Orcs*/ {
    Sessao* sessao1 = criarSessao("\nVocê entra na Caverna dos Orcs, o cheiro de suor e metal impregna o ar.");
    Sessao* sessao2 = criarSessao("\nVocê se depara com um grupo de orcs treinando. Eles notam sua presença e se preparam para o combate.");
    sessao2->inimigo = 1;
    sessao2->mod_inimigo = 1;
    Sessao* sessao3 = criarSessao("\nCom os orcs derrotados você se sente mais tranquilizado, mas não por muito tempo, pois mais orcs chegam para enfrentar você!");
    sessao3->inimigo = 1;
    Sessao* sessao4 = criarSessao("\nVocê derrota os orcs e avança na caverna.\nNessa parte da caverna, você encontra mais orcs, a luta é inevitável.");
    sessao4->inimigo = 1;
    Sessao* sessao5 = criarSessao("\nApós a batalha, ao revistar os Orcs, você acham um Mapa no bolso de um deles, contendo informações da caverna.\nSeguindo o mapa, você chega na sala do líder orc. Ele está pronto para a batalha.\n\tEle parece mais forte que os demais Orcs! Ele é um Uruk");
    sessao5->mod_inimigo = 3;
    sessao5->inimigo = 1;
    Sessao* sessao6 = criarSessao("\nA batalha contra o líder orc é intensa, mas você consegue derrotá-lo. A caverna dos orcs agora está sob seu controle.");
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

void help(){
    printf("Bem-vindo a sessão de ajuda, segue dados para melhor compreender o game!\n");
    printf("\n1. Combate\n\t No combate você terá 4 opções, sendo elas:\n\t\t1. Atacar normalmente (Usando a sorte nos dados, você tenta atacar o inimigo).\n\t\t2. Bola de fogo (Usando a sorte nos dados, você tenta acertar o inimigo).\n\t\t3. Escudo (Cria um escudo mágico ao seu redor, ele é capaz de absorver 1 de dano, ele ficará ativo por 2 turnos!).\n\t\t4. Cura (Use magia para se curar, restaurando 1 HP).\n");
    printf("\n2. Armadilhas\n\t Ao passar por armadilhas, você terá 3 opções, sendo elas:\n\t\t1. Usar escudo e tentar passar (Mesmas vantagens ao utilizar em combate).\n\t\t2. Se curar (É possível se curar antes de tentar passar pela armadilha).\n\t\t3. Tentar passar (Tenta passar pela armadilha sem nenhum auxilio mágico).\n");
    printf("\n3. Suprimentos\n\t Você utiliza de suprimentos disponíveis naquele momento, a quantidade de HP e MP recuperada irá depender da sua sorte!\n");
    printf("\n4. Campanhas\n\t O Jogo fornece 4 campanhas, sendo elas:\n\t\t1. Main Quest (História longa, baseada no prólogo do jogo TES V - Skyrim)\n\t\t2. Abismo das Sombras Profundas (Missão curta, dificuldade: Fácil)\n\t\t3. Antro dos Golgorgs (Missão de tamanho médio, dificuldade: Média)\n\t\t4. Caverna dos Orcs (Missão de tamanho médio, dificuldade: Difícil)\n");
    printf("\nPor fim, confie nos dados!\n\n");
}

int main() {
    intro0();
    system("cls");
    printf(" \n",setlocale(LC_ALL,""));
    srand((unsigned int)time(NULL));
    int gamemode;
    inicio:
    do{
        printf("\nBem-vindo aventureiro\n");
        printf(R"EOF(
Escolha uma opção abaixo para iniciar uma jornada:
1. Main Quest - Jornada principal
2. Abismo das Sombras Profundas - Jornada para aventureiros iniciantes
3. Antro dos Golgorgs - Jornada para aventureiros experientes
4. Covil dos Orcs - Jornada para aventureiros que não temem a morte
5. Ajuda
6. Sair)EOF");
printf("\n");
        scanf("\n%d", &gamemode);
        system("cls");
        if (gamemode == 1) {
            system("cls");
            intro1();
            system("cls");
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha1(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 2) {
            system("cls");
            intro2();
            system("cls");
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha2(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 3) {
            system("cls");
            intro3();
            system("cls");
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha3(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 4) {
            system("cls");
            intro4();
            system("cls");
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha4(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 5){
            system("cls");
            help();
            system("pause");
            system("cls");
            goto inicio;
        } else if (gamemode == 6) {
            printf("**************************************************\n");
            printf("Obrigado por jogar! Até a próxima aventura!\n");
            printf("**************************************************\n");
            return 0;
        }
    } while (gamemode > 0 || gamemode <6);

    system("pause");
    return 0;
}
