#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ncurses.h>

const int LARGURA = 30;  // Largura do campo
const int ALTURA = 20;   // Altura do campo
const char COMIDA = '*';
const char CORPO_COBRA = 'O';
const char VAZIO = ' ';
const char PAREDE = '#';

struct Cobra {
    int x, y;                // Posição da cabeça da cobra
    int comprimento;         // Comprimento da cobra
    std::vector<std::pair<int, int>> corpo; // Corpo da cobra
    char direcao;            // Direção atual
};

Cobra cobra = {LARGURA / 2, ALTURA / 2, 1, {{LARGURA / 2, ALTURA / 2}}};
int comidaX, comidaY; // Posição da comida

void gerarComida() {
    do {
        comidaX = rand() % LARGURA;
        comidaY = rand() % ALTURA;
    } while (comidaX == cobra.x && comidaY == cobra.y); // Garante que a comida não aparece na cabeça da cobra
}

void desenhar() {
    clear(); // Limpa a tela

    for (int i = 0; i < LARGURA + 2; i++) printw("%c", PAREDE); // Desenha o topo
    printw("\n");

    for (int y = 0; y < ALTURA; y++) {
        printw("%c", PAREDE); // Desenha a parede à esquerda
        for (int x = 0; x < LARGURA; x++) {
            if (x == cobra.x && y == cobra.y) {
                printw("%c", CORPO_COBRA); // Desenha a cabeça da cobra
            } else if (x == comidaX && y == comidaY) {
                printw("%c", COMIDA); // Desenha a comida
            } else {
                bool isCorpo = false;
                for (const auto& segmento : cobra.corpo) {
                    if (segmento.first == x && segmento.second == y) {
                        printw("%c", CORPO_COBRA);
                        isCorpo = true;
                        break;
                    }
                }
                if (!isCorpo) printw("%c", VAZIO); // Espaço vazio
            }
        }
        printw("%c", PAREDE); // Desenha a parede à direita
        printw("\n");
    }

    for (int i = 0; i < LARGURA + 2; i++) printw("%c", PAREDE); // Desenha a base
    printw("\n");
    refresh(); // Atualiza a tela
}

bool checarColisao() {
    // Verifica colisão com as paredes
    if (cobra.x < 0 || cobra.x >= LARGURA || cobra.y < 0 || cobra.y >= ALTURA) {
        return true; // Colidiu com a parede
    }

    // Verifica colisão com o corpo da cobra
    for (const auto& segmento : cobra.corpo) {
        if (segmento.first == cobra.x && segmento.second == cobra.y) {
            return true; // Colidiu com o corpo
        }
    }

    return false; // Sem colisão
}

void moverCobra() {
    while (true) {
        // Move a cobra
        if (cobra.direcao == 'w') cobra.y--;
        else if (cobra.direcao == 's') cobra.y++;
        else if (cobra.direcao == 'a') cobra.x--;
        else if (cobra.direcao == 'd') cobra.x++;

        // Checa se colidiu
        if (checarColisao()) {
            printw("O jogo acabou! Pressione alguma tecla para sair.\n");
            refresh();
            getch(); // Espera por uma tecla para sair
            break; // Sai do loop
        }

        // Checa se comeu a comida
        if (cobra.x == comidaX && cobra.y == comidaY) {
            cobra.comprimento++;
            cobra.corpo.push_back({cobra.x, cobra.y});
            gerarComida(); // Gera nova comida
        } else {
            // Atualiza o corpo da cobra
            if (!cobra.corpo.empty()) {
                cobra.corpo.push_back({cobra.x, cobra.y});
                if (cobra.corpo.size() > cobra.comprimento) {
                    cobra.corpo.erase(cobra.corpo.begin());
                }
            }
        }

        desenhar(); // Desenha o jogo
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Controle de velocidade
    }
}

void lerEntrada() {
    while (true) {
        // Lê a tecla pressionada
        int tecla = getch();
        if (tecla == 'w' && cobra.direcao != 's') cobra.direcao = 'w';
        else if (tecla == 's' && cobra.direcao != 'w') cobra.direcao = 's';
        else if (tecla == 'a' && cobra.direcao != 'd') cobra.direcao = 'a';
        else if (tecla == 'd' && cobra.direcao != 'a') cobra.direcao = 'd';
    }
}

int main() {
    srand(static_cast<unsigned>(time(0))); // Semente para o gerador de números aleatórios
    initscr(); // Inicializa o modo ncurses
    cbreak(); // Desativa o buffer de linha
    noecho(); // Não mostra a entrada do usuário
    keypad(stdscr, TRUE); // Habilita leitura de teclas de função

    gerarComida(); // Gera a primeira comida
    cobra.direcao = 'd'; // Inicializa a direção da cobra

    std::thread cobraThread(moverCobra);
    std::thread entradaThread(lerEntrada); // Nova thread para ler a entrada

    cobraThread.join(); // Espera o thread da cobra terminar
    entradaThread.detach(); // Desanexa a thread de entrada

    endwin(); // Finaliza o modo ncurses
    return 0;
}
