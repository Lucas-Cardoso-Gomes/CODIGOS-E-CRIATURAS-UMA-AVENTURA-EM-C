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
        printf("Erro ao alocar mem�ria para a sess�o.\n");
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
    printf("Vidas: %d | Pontos de Magia (PM): %d", jogador->HP, jogador->MP);
    if (jogador->escudoAtivo) {
        printf(" | Escudo ativo (Dura��o restante: %d turnos)\n", jogador->duracaoEscudo);
    } else {
        printf("\n");
    }
}

void mostrarOpcoes(Sessao* sessaoAtual) {
    printf("Escolha sua a��o:\n");
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

    if ((resultadoDado-= sessaoAtual->mod_inimigo) >= 14) {
        printf("Voc� derrotou o inimigo e avan�a com cuidado.\n");
        sessaoAtual->inimigo = 0;
    } else if ((resultadoDado-= sessaoAtual->mod_inimigo) >= 3 && (resultadoDado-= sessaoAtual->mod_inimigo) <= 13) {
        printf("Voc� errou o ataque!\n");
        
        if (sessaoAtual->mod_inimigo > 1){
            printf("O Inimigo � poderoso, voc� ir� precisar de mais precis�o em seus ataques!\n");
        }
    } else {
        printf("Voc� escorregou e o inimigo te ataca! Voc� perde 1 HP.\n");
        dano(jogador, 1);

        if (sessaoAtual->mod_inimigo > 1){
            printf("O Inimigo � poderoso, voc� ir� precisar de mais precis�o em seus ataques!\n");
        }
    }

    if (jogador->HP <= 0) {
        printf("Voc� perdeu todas as vidas. Fim do jogo.\n");
        exit(EXIT_SUCCESS);
    }

    printf("**************************************************\n");

    if (sessaoAtual->inimigo == 1) {
        printf("O inimigo contra-ataca!\n");
        int resultadoDadoInimigo = rolarD20();

        if (sessaoAtual->mod_inimigo > 1){
            resultadoDadoInimigo += sessaoAtual->mod_inimigo;
        }

        printf("Resultado do dado inimigo: %d\n", resultadoDadoInimigo);

        if (resultadoDadoInimigo >= 19) {
            printf("O inimigo acertou voc� e causou 2 de dano!\n");
            dano(jogador, 2);
        } else if (resultadoDadoInimigo >= 12) {
            printf("O inimigo acertou voc� e causou 1 de dano.\n");
            dano(jogador, 1);
        } else if (resultadoDadoInimigo <= 2) {
            printf("O inimigo escorregou e voc� contra-atacou!\n");
            sessaoAtual->inimigo = 0;
        } else {
            printf("O inimigo errou o ataque..\n");
        }
    }

    if (jogador->HP <= 0) {
        printf("Voc� perdeu todas as vidas. Fim do jogo.\n");
        exit(EXIT_SUCCESS);
    }
    
    if (jogador->escudoAtivo) {
        jogador->duracaoEscudo--;
        if (jogador->duracaoEscudo == 0) {
            printf("O Escudo dissipou-se. Sua defesa retorna ao normal.\n");
            jogador->escudoAtivo = 0;
        }
    }

    printf("**************************************************\n");

}

void executarBolaDeFogo(Jogador* jogador, Sessao* sessaoAtual) {
    if (jogador->MP >= CUSTO_BOLA_DE_FOGO) {
        printf("Voc� conjura a Bola de Fogo!\n");
        int resultadoDado = rolarD20();
        printf("Resultado do dado %d\n", resultadoDado);

        if (sessaoAtual->mod_inimigo > 1){
            resultadoDado -= sessaoAtual->mod_inimigo;
        }

        if (resultadoDado > 10) {
            printf("Voc� acertou o inimigo e o derrotou.\n");
            sessaoAtual->inimigo = 0;
        } else {
            printf("A Bola de Fogo errou o alvo.\n");
            
            if (sessaoAtual->mod_inimigo > 1){
                printf("O Inimigo � poderoso, voc� ir� precisar de mais precis�o em seus ataques!\n");
            }
        }

        jogador->MP -= CUSTO_BOLA_DE_FOGO;
    } else {
        printf("Voc� n�o tem PM suficiente para conjurar a Bola de Fogo.\n");
    }

    printf("**************************************************\n");

    if (sessaoAtual->inimigo == 1) {
        printf("O inimigo contra-ataca!\n");
        int resultadoDadoInimigo = rolarD20();
        printf("Resultado do dado inimigo: %d\n", resultadoDadoInimigo);

        if (resultadoDadoInimigo >= 19) {
            printf("O inimigo acertou voc� e causou 2 de dano!\n");
            dano(jogador, 2);
        } else if (resultadoDadoInimigo >= 12) {
            printf("O inimigo acertou voc� e causou 1 de dano.\n");
            dano(jogador, 1);
        } else if (resultadoDadoInimigo <= 2) {
            printf("O inimigo escorregou e voc� contra-atacou!\n");
            sessaoAtual->inimigo = 0;
        } else {
            printf("O inimigo errou o ataque..\n");
        }
    }

    if (jogador->HP <= 0) {
        printf("Voc� perdeu todas as vidas. Fim do jogo.\n");
        exit(EXIT_SUCCESS);
    }
    
    if (jogador->escudoAtivo) {
        jogador->duracaoEscudo--;
        if (jogador->duracaoEscudo == 0) {
            printf("O Escudo dissipou-se. Sua defesa retorna ao normal.\n");
            jogador->escudoAtivo = 0;
        }
    }

    printf("**************************************************\n");

}

void executarEscudo(Jogador* jogador) {
    if (jogador->MP >= CUSTO_ESCUDO) {
        if (jogador->escudoAtivo) {
            printf("Voc� j� conjurou o Escudo. Escolha outra a��o.\n");
        } else {
            printf("Voc� conjura o Escudo! Sua defesa est� melhorada.\n");
            jogador->escudoAtivo = 1;
            jogador->duracaoEscudo = 3;
            jogador->MP -= CUSTO_ESCUDO;
        }
    } else {
        printf("Voc� n�o tem PM suficiente para conjurar o Escudo.\n");
    }
}

void executarCura(Jogador* jogador) {
    if (jogador->MP >= CUSTO_CURA) {
        printf("Voc� conjura a Cura! Voc� recupera 1 ponto de vida.\n");
        jogador->HP++;
        jogador->MP -= CUSTO_CURA;
    } else {
        printf("Voc� n�o tem PM suficiente para conjurar a Cura.\n");
    }
}

void executarCombate(Jogador* jogador, Sessao* sessaoAtual) {
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
                    printf("Op��o inv�lida. Tente novamente.\n");
            }

        } while (escolha < 1 || escolha > 4);
        printf("**************************************************\n");
    }

    if (sessaoAtual->horda_inimigo > 0)
    {
        printf("Eles s�o muitos, continue lutando por usa vida!\n");
        sessaoAtual->inimigo = 1;
        sessaoAtual->horda_inimigo -= 1;
    }

}

void executarArmadilha(Jogador* jogador, Sessao* sessaoAtual) {
    mostrarInformacoesCombate(jogador);
    printf("Voc� se depara com uma armadilha!\n");

    int opcao;
    printf("Escolha uma op��o:\n1. Usar escudo e tentar passar\n2. Se curar\n3. Tentar passar\n");
    scanf("%d", &opcao);

    printf("**************************************************\n");

    switch (opcao)
    {
        case 1:
            executarEscudo(jogador);
            printf("Voc� conjura o escudo e tenta passar pela armadilha...\n");
            break;
        case 2:
            executarCura(jogador);
            printf("Voc� conjura o cura e tenta passar pela armadilha...\n");
            break;
        case 3:
            printf("Voc� tenta passar pela armadilha...\n");
            break;
    }

    int resultadoDado = rolarD20();
    printf("Resultado do dado %d\n", resultadoDado);

    if (resultadoDado >= 15) {
        printf("Voc� conseguiu desarmar a armadilha e sair ileso!\n");
        sessaoAtual->armadilha=0;
    } else if (resultadoDado >= 7) {
        printf("Voc� passou pela armadilha, mas sofreu um pequeno ferimento. Perdeu 1 ponto de vida.\n");
        dano(jogador, 1);
        sessaoAtual->armadilha=0;
    } else if (resultadoDado >= 2) {
        printf("Voc� est� preso na armadilha e mas sofreu um pequeno ferimento. Perdeu 1 ponto de vida.\n");
        dano(jogador, 1);
    } else {
        printf("Voc� est� preso na armadilha e sofreu um ferimento grave. Perdeu 2 ponto de vida.\n");
        dano(jogador, 2);
    }
    if (jogador->HP <= 0) {
            printf("Voc� perdeu todas as vidas. Fim do jogo.\n");
            exit(EXIT_SUCCESS);
    }
    printf("**************************************************\n");

    if (jogador->escudoAtivo) {
        jogador->duracaoEscudo--;
        if (jogador->duracaoEscudo == 0) {
            printf("O Escudo dissipou-se. Sua defesa retorna ao normal.\n");
            jogador->escudoAtivo = 0;
        }
    }
}

void executarSuprimentos(Jogador* jogador, Sessao* sessaoAtual) {

    printf("**************************************************\n");

    printf("Voc� encontra suprimentos durante sua jornada, que tenham boa utilidade!\n");
    
    int resultadoDado = rolarD20();
    printf("Sorte no dado %d\n", resultadoDado);
    
    if (resultadoDado >= 15) {
        printf("Voc� recebe um Buff de +3 HP e 10 MP!\n");
        jogador->HP += 3;
        jogador->MP += 10;
    } else if (resultadoDado >= 7) {
        printf("Voc� recebe um Buff de +2 HP e 6 MP!\n");
        jogador->HP += 2;
        jogador->MP += 6;
    } else if (resultadoDado >= 2) {
        printf("Voc� recebe um Buff de +1 HP e 3 MP!\n");
        jogador->HP += 1;
        jogador->MP += 3;
    } else if (resultadoDado == 2) {
        printf("Voc� recebe um Buff de +2 MP!\n");
        jogador->MP += 2;
    }
    else {
        printf("Os suprimentos est�o estragados, nada poder� ser usado.\n");
    }
    
    printf("**************************************************\n");

    sessaoAtual->suprimentos=0;
}

void jogarJogo(Sessao* inicioSessao, Jogador* jogador) {

    Sessao* sessaoAtual = inicioSessao;

    while (sessaoAtual != NULL) {

        printf("%s\n", sessaoAtual->descricao);

        while (sessaoAtual->inimigo) {
            executarCombate(jogador, sessaoAtual);
            if (jogador->HP <= 0) {
                printf("Voc� perdeu todas as vidas. Fim do jogo.\n");
                break;
            }
        }

        while (sessaoAtual->armadilha) {
            executarArmadilha(jogador, sessaoAtual);
            if (jogador->HP <= 0) {
                printf("Voc� perdeu todas as vidas. Fim do jogo.\n");
                break;
            }
        }

        while (sessaoAtual->suprimentos) {
            executarSuprimentos(jogador, sessaoAtual);
            if (jogador->HP <= 0) {
                printf("Voc� perdeu todas as vidas. Fim do jogo.\n");
                break;
            }
        }

        sessaoAtual = sessaoAtual->proxima;
    }

    printf("Parab�ns! Voc� explorou a caverna com sucesso!\n");
    destruirSessoes(inicioSessao);
}

void intro1(){

} 

void campanha1(Sessao* inicioSessao)/*Main Quest*/ {
    Sessao* sessao1 = criarSessao("\nVoc� est� a bordo de um carro�a, sendo levado para sua execu��o em Helgen.");
    Sessao* sessao2 = criarSessao("Antes de sua execu��o, um drag�o ataca Helgen, causando caos e destrui��o.");
    sessao2->armadilha = 1;
    Sessao* sessao3 = criarSessao("\nDurante o caos, um soldado Imperial, chamado Hadvar oferece ajuda para escapar.");
    Sessao* sessao4 = criarSessao("\nVoc�s dois correm por escombros para se abrigar enquanto o drag�o queima a cidade nas suas costas.");
    Sessao* sessao5 = criarSessao("\nMais adiante o Hadvar diz para se abrigarem nas masmorras, onde o drag�o n�o ir� alcan�ar.");
    Sessao* sessao6 = criarSessao("\nAo entrar nas masmorras, Hadvar te solta das algemas e diz que para sobreviver, voc�s ter�o que trabalhar juntos");
    Sessao* sessao7 = criarSessao("\nVoc� vasculha alguns ba�s dentro da masmorra e percebe equipamentos da guarda, voc� se equipa para fugir");
    Sessao* sessao8 = criarSessao("\nEnquanto fogem, voc� percebem os stormcloaks estavam invadindo a cidade para resgatar Ulfric Stormcloaks, que estava preso na cidade! (eles formam um exercito que luta pela liberta��o do seu povo pela invas�o do imp�rio)\nVoc�s travam um combate com os Stormcloaks!");
    sessao8->inimigo = 1;
    Sessao* sessao9 = criarSessao("\nAp�s a batalha, voc�s andam mais discretamente, por�m s�o muitos inimigos em seu caminho!");
    sessao9->inimigo = 1;
    Sessao* sessao10 = criarSessao("\nAvan�ando mais a masmorra, voc� encontram a c�mara de tortura, onde um mago e seu assistente batalham contra os stormcloaks, voc� deve ajuda-los!");
    sessao10->inimigo = 1;
    Sessao* sessao11 = criarSessao("\nAp�s salv�-los, eles fornecem mais equipamento e po��es para que possam seguir com a fuga.");
    sessao11->suprimentos = 1;
    Sessao* sessao12 = criarSessao("\nVoc� segue masmorra adentro, agora n�o parece mais como uma constru��o, e sim uma caverna.");
    Sessao* sessao13 = criarSessao("\nMais inimigos aparecem no seu caminho, a batalha � inevit�vel");
    sessao13->inimigo = 1;
    Sessao* sessao14 = criarSessao("\nSeguindo a caverna, n�o tem mais presen�a humana, agora voc� deve ter cuidado com as bestas que a habitam");
    Sessao* sessao15 = criarSessao("\nDurante a caminhada, voc�s notam teias de aranha cada vez mais presente onde voc�s est�o, quando de repente, aranha gigantes surgem nas paredes...");
    sessao15->inimigo = 1;
    Sessao* sessao16 = criarSessao("\nSem tempo para descanso, ap�s a batalha com a aranha, urso aparece na sa�da da caverna!");
    sessao16->inimigo = 1;
    Sessao* sessao17 = criarSessao("\nFinalmente, a Luz do dia nunca foi t�o boa, voc�s forem s� em salvo!");
    Sessao* sessao18 = criarSessao("\nAgora cada um de voc�s segue sem rumo, voc� tem uma grande hist�ria pela frente!");

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

}

void campanha2(Sessao* inicioSessao)/*Caverna Escura*/ {
    Sessao* sessao1 = criarSessao("\nVoc� entra em uma caverna iluminada por cristais brilhantes. O ar � carregado com uma energia m�stica.");
    Sessao* sessao2 = criarSessao("Voc� se depara em um caminho longo, cada passo que voc� d�, menos brilho os cristais parecem prover.\nNa escurid�o voc� percebe algo...");
    sessao2->armadilha = 1;
    Sessao* sessao3 = criarSessao("\nDevido ao barulho da armadilha, surge uma criatura do fundo da caverna. Ela parece hostil!");
    sessao3->inimigo = 1;
    Sessao* sessao4 = criarSessao("\nVoc� derrotou a criatura, que no final, era apenas um urso. Seu caminho est� livre novamente.");
    Sessao* sessao5 = criarSessao("\nVoc� entra na sala iluminada por cristais pulsantes. Uma sensa��o de calma toma conta de voc�.\nVoc� aproveita para olhar em volta, e percebe equipamentos e suprimentos espalhado no ch�o.");
    sessao5->suprimentos = 1;
    Sessao* sessao6 = criarSessao("\n... por�m n�o por muito tempo, ap�s ficar um tempo nessa sala, voc� escuta passos vindo da entrada da caverna, voc� est� encurralado");
    Sessao* sessao7 = criarSessao("\nAp�s alguns minutos, o som chega mais perto e voc� percebe, um grande Urso das Cavernas surge da entrada, a batalha por sua salva��o � inevit�vel!");
    sessao7->inimigo = 1;
    sessao7->mod_inimigo = 3;
    Sessao* sessao8 = criarSessao("\nCom a grande vit�ria sobre o urso, voc� sai �s pressas da caverna, sua explora��o te levou para um lugar ""inesquec�vel"", por�m perigoso!");

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
    printf("                ..         =                                \n");
    printf("              .*        :*=                                 \n");
    printf("              ##-+**#*-=#*                                  \n");
    printf("             :#++**##%%@%%#*.                                 \n");
    printf("            .****#*##@%%#@@%%**=-:.                          \n");
    printf("            *###***%%%#%#@%##%###%%=                        \n");
    printf("           +*#*%##%%##+%%%@###***#%%#@+                       \n");
    printf("            #%%#@@@*%%#*+%%@@%%###***%#%@+                      \n");
    printf("            -*#%%*#@%##%%%@%##%###**#@@%%#:                    \n");
    printf("             =*%%@@%%%%#@@%%***%%+**##@@%%%%@.                   \n");
    printf("              -#%%%%#@@@%%#*##*##**@@%%#%%@                   \n");
    printf("              +*%%@%#%*@###+*%%*##*#@@##%%@.                  \n");
    printf("             -+*#@%%-  :==#%%%%***%%@%%#%%@%%-                 \n");
    printf("             +**%%#        .=#@#**@@@%%@@@@@@@%%#              \n");
    printf("           :+##*@-        .-*####%%@@@@@@@@@@@@:             \n");
    printf("          -+++#%%@:       =*++*#%%@@@@##%%@@%%@@*              \n");
    printf("         :+++#@#     :-+*++*#%%%%@@@%%*#%%@@%%@@@+              \n");
    printf("         +***%%#   :=***#+*#%%@%%#@@#%%+*##%%@@@@@=              \n");
    printf("        -##*%%*  -**###%#*%@@@%%@@#*+###%%@@@%%@-              \n");
    printf("        *##%%= .*#%#%%%###@@@@%%@@@%%+*###@@@%%:+=              \n");
    printf("       +##%%=  :%#%#%%@%%###*    -@@%%*###@@@@:  :              \n");
    printf("       +*%%#   :=%%#*##%%*#%%+    +@#*##@@--%%@:                \n");
    printf("      -####*%%    =***#####%#%#+::*#*#*#%%. -@#              \n");
    printf("      :#*%%=##     =**#%%#%%*+#-:=-*%%****%%@#@:               \n");
    printf("       +##.*+      -**#@-**+*:-+.. :*#*%%@%%@-               \n");
    printf("        -#*=.       -**%%* -*+#-       =%%##@+               \n");
    printf("          -#-.       #**%%*.            .+##%%@:              \n");
    printf("            .    .-+#####%%@....:.:.      =###%%+.            \n");
    printf("            =*#%%##%%#+=-==- . .          -%%#%%@@            \n");
    printf("           .. .:.--.                  :=#%%%%*:             \n");
    printf("                                      :::::.                \n");
    Sleep(3000);
}

void campanha3(Sessao* inicioSessao)/*Caverda dos goblins*/ {
    Sessao* sessao1 = criarSessao("\nVoc� adentra a Caverna dos Goblins, sua miss�o � massacrar esses pequenos monstros.");
    Sessao* sessao2 = criarSessao("\nOs goblins espreitam voc� de longe, mas n�o tentam aproximar, parecem tramar algo...");
    sessao2->armadilha = 1;
    Sessao* sessao3 = criarSessao("\nVoc� consegue se livrar da armadilha e segue a caverna.");
    Sessao* sessao4 = criarSessao("\nVoc� encontra uma encruzilhada. Um grupo de goblins surge de uma passagem lateral.");
    sessao4->inimigo = 1;
    Sessao* sessao5 = criarSessao("\nAp�s derrotar os goblins, voc� encontra uma sala cheia de tesouros pequenos e reluzentes e encontra alguns suprimentos que parecem ter sido roubados de outros aventureiros.");
    sessao5->suprimentos = 1;
    Sessao* sessao6 = criarSessao("\nContinuando sua explora��o, voc� se depara com um goblin no fim de uma sala, ao tentar se aproximar voc� � surpreendido.");
    sessao6->armadilha = 1;
    Sessao* sessao7 = criarSessao("\n\tO goblin no fim sala � o l�der goblin desta caverna, ele tenta correr para outra passagem na caverna e voc� o segue...");
    sessao7->armadilha = 1;
    Sessao* sessao8 = criarSessao("\nAp�s se safar da armadilha, seu combate com o lider come�a!");
    sessao8->inimigo = 1;
    Sessao* sessao9 = criarSessao("\n\tMais goblins surgem aos montes, todas as fissuras da caverna parecem ser passagem para eles...");
    sessao9->inimigo = 1;
    sessao9->horda_inimigo = 3;
    Sessao* sessao10 = criarSessao("\nVoc�, finalmente, derrota todos os goblins, mas a caverna come�a a desmoronar. � hora de escapar o mais r�pido poss�vel.");
    sessao10->armadilha = 1;
    Sessao* sessao11 = criarSessao("\nVoc� escapa da Caverna dos Goblins a tempo. Agradecido, o l�der goblin lhe presenteia com um item valioso como sinal de respeito.");

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
    printf("????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("????????????????????????????????????????????????\n");
    printf("????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    printf("????????????????????????????????????????????????\n");
    printf("????????????????????????????????????????????????\n");
    printf("?????????????????????????????????????????????????\n");
    Sleep(3000);
}

void campanha4(Sessao* inicioSessao)/*Caverna dos Orcs*/ {
    Sessao* sessao1 = criarSessao("\nVoc� entra na Caverna dos Orcs, o cheiro de suor e metal impregna o ar.");
    Sessao* sessao2 = criarSessao("\nVoc� se depara com um grupo de orcs treinando. Eles notam sua presen�a e se preparam para o combate.");
    sessao2->inimigo = 1;
    Sessao* sessao3 = criarSessao("\nCom os orcs derrotados voc� se sente mais tranquilizado, mas n�o por muito tempo, pois mais orcs chegam para enfrentar voc�!");
    sessao3->inimigo = 1;
    Sessao* sessao4 = criarSessao("\nVoc� derrota os orcs e avan�a na caverna.\nNessa parte da caverna, voc� encontra mais orcs, a luta � inevit�vel.");
    sessao4->inimigo = 1;
    Sessao* sessao5 = criarSessao("\nAp�s a batalha, ao revistar os Orcs, voc� acham um Mapa no bolso de um deles, contendo informa��es da caverna.\nSeguindo o mapa, voc� chega � sala do l�der orc. Ele est� pronto para a batalha.\n\tEle parece mais forte que os demais Orcs! Ele � um Uruk");
    sessao5->mod_inimigo = 3;
    sessao5->inimigo = 1;
    Sessao* sessao6 = criarSessao("\n\tA batalha contra o l�der orc � intensa, mas voc� consegue derrot�-lo. A caverna dos orcs agora est� sob seu controle.");
    Sessao* sessao7 = criarSessao("\nVoc� encontra uma passagem secreta que leva a uma sala com todos os tesouros roubados pelos orcs, por�m ela est� protegida.");
    Sessao* sessao8 = criarSessao("\nVoc� decide enfrentar os perigos e recuperar o tesouro. Orcs guardi�es desafiam sua bravura.");
    sessao8->inimigo = 1;
    Sessao* sessao9 = criarSessao("\nAp�s vencer, o tesouro est� ao seu alcance. Voc� retorna � entrada da caverna com suas recompensas.");
    Sessao* sessao10 = criarSessao("\nVoc� sai vitorioso da Caverna dos Orcs, sendo aclamado como her�i. Sua jornada continua em terras desconhecidas.");

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
    printf("Bem-vindo a sess�o de ajuda, segue dados para melhor compreender o game!\n");
    printf("1. Combate\n\t No combate voc� tera 4 op��es, sendo elas:\n\t\t1. Atacar normalmente (Usando a sorte nos dados, voc� tenta atacar o inimigo).\n\t\t2. Bola de fogo (Usando a sorte nos dados, voc� tenta acertar o inimigo).\n\t\t3. Escudo (Cria um escudo magico ao seu redor, ele � capaz de absorver 1 de dano, ele ficar� ativo por 2 turnos!).\n\t\t4. Cura (Use magia para se curar, restaurando 1 HP).\n");
    printf("2. Armadilhas\n\t Ao passar por armadilhas, voc� ter� 3 op��es, sendo elas:\n\t\t1. Usar escudo e tentar passar (Mesmas vantagens ao utilizar em combate).\n\t\t2. Se curar (� possivel se curar antes de tentar passar pela armadilha).\n\t\t3. Tentar passar (Tenta passar pela armadilha sem nenhum auxilio magico).\n");
    printf("3. Suprimentos\n\t Voc� utiliza de suprimentos disponiveis naquele momento, a quantidade de HP e MP recuperada ir� depender da sua sorte!\n");
    printf("4. Campanhas\n\t O Jogo fornece 4 campanhas, sendo elas:\n\t\t1. Main Quest (Hist�ria longa, baseada no pr�logo do jogo TES V - Skyrim)\n\t\t2. Caverna Escura (Miss�o curta, dificuldade: F�cil)\n\t\t3. Caverna dos goblins (Miss�o de tamanho m�dio, dificuldade: M�dia)\n\t\t4. Caverna dos Orcs (Miss�o de tamanho m�dio, dificuldade: Dif�cil)\n");
    printf("\nPor fim, confie nos dados! ??\n\n");
}

int main() {
    setlocale(LC_ALL, "Portuguese_Brasil");
    srand((unsigned int)time(NULL));

    int gamemode;

    inicio:

    do{
        printf("**************************************************\n");
        printf("C�digos e Criaturas: uma aventura em C!\n");
        printf("**************************************************\n");
        printf("Bem-vindo ao um jogo de RPG Textual\n");

        printf("Escolha uma op��o:\n\t1. Main Quest (Longa-Hist�ria)\n\t2. Caverna Escura (F�cil)\n\t3. Caverna dos goblins (M�dia)\n\t4. Caverna dos Orcs (Dif�cil) \n\t5. Ajuda\n\t6. Sair\n");
        scanf("%d", &gamemode);
        
        printf("**************************************************\n");
        if (gamemode == 1) {
            intro1();
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha1(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 2) {
            intro2();
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha2(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 3) {
            intro3();
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha3(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 4) {
            intro4();
            Jogador jogador = {5, 10, 0, 0};
            Sessao* inicioSessao = criarSessao("");
            campanha4(inicioSessao);
            jogarJogo(inicioSessao, &jogador);
        } else if (gamemode == 5){
            help();
            Sleep(10000);
            goto inicio;
        } else if (gamemode == 6) {
            printf("**************************************************\n");
            printf("Obrigado por jogar! At� a pr�xima aventura!\n");
            printf("**************************************************\n");
            return 0;
        }
    } while (gamemode > 0 || gamemode <6);

    system("pause");
    return 0;
}