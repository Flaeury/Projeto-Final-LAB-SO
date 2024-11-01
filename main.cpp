#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ncurses.h>
#include <semaphore.h>

const int WIDTH = 20;  // Largura do campo
const int HEIGHT = 10; // Altura do campo
const char FOOD = '*';
const char SNAKE_BODY = 'O';
const char EMPTY = ' ';
const char WALL = '#';

struct Snake {
    int x, y;        // Posição da cabeça da cobra
    int length;      // Comprimento da cobra
    std::vector<std::pair<int, int>> body; // Corpo da cobra
    char direction;  // Direção atual
};

Snake snake = {WIDTH / 2, HEIGHT / 2, 1, {{WIDTH / 2, HEIGHT / 2}}};
int foodX, foodY; // Posição da comida
sem_t semaphore; // Semáforo para controle de acesso

void generateFood() {
    foodX = rand() % WIDTH;
    foodY = rand() % HEIGHT;
}

void draw() {
    clear(); // Limpa a tela

    for (int i = 0; i < WIDTH + 2; i++) printw("%c", WALL); // Desenha o topo
    printw("\n");

    for (int y = 0; y < HEIGHT; y++) {
        printw("%c", WALL); // Desenha a parede à esquerda
        for (int x = 0; x < WIDTH; x++) {
            if (x == snake.x && y == snake.y) {
                printw("%c", SNAKE_BODY); // Desenha a cabeça da cobra
            } else if (x == foodX && y == foodY) {
                printw("%c", FOOD); // Desenha a comida
            } else {
                bool isBody = false;
                for (const auto& segment : snake.body) {
                    if (segment.first == x && segment.second == y) {
                        printw("%c", SNAKE_BODY);
                        isBody = true;
                        break;
                    }
                }
                if (!isBody) printw("%c", EMPTY); // Espaço vazio
            }
        }
        printw("%c", WALL); // Desenha a parede à direita
        printw("\n");
    }

    for (int i = 0; i < WIDTH + 2; i++) printw("%c", WALL); // Desenha a base
    printw("\n");
    refresh(); // Atualiza a tela
}

void moveSnake() {
    while (true) {
        sem_wait(&semaphore); // Aguarda o semáforo

        // Lê a tecla pressionada
        int key = getch();
        if (key == 'w' && snake.direction != 's') snake.direction = 'w';
        else if (key == 's' && snake.direction != 'w') snake.direction = 's';
        else if (key == 'a' && snake.direction != 'd') snake.direction = 'a';
        else if (key == 'd' && snake.direction != 'a') snake.direction = 'd';

        // Move a cobra
        if (snake.direction == 'w') snake.y--;
        else if (snake.direction == 's') snake.y++;
        else if (snake.direction == 'a') snake.x--;
        else if (snake.direction == 'd') snake.x++;

        // Checa se comeu a comida
        if (snake.x == foodX && snake.y == foodY) {
            snake.length++;
            snake.body.push_back({snake.x, snake.y});
            generateFood(); // Gera nova comida
        } else {
            // Atualiza o corpo da cobra
            if (!snake.body.empty()) {
                snake.body.push_back({snake.x, snake.y});
                if (snake.body.size() > snake.length) {
                    snake.body.erase(snake.body.begin());
                }
            }
        }

        draw(); // Desenha o jogo
        sem_post(&semaphore); // Libera o semáforo
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Controle de velocidade
    }
}

int main() {
    srand(time(0));
    initscr(); // Inicializa o modo ncurses
    cbreak(); // Desativa o buffer de linha
    noecho(); // Não mostra a entrada do usuário
    keypad(stdscr, TRUE); // Habilita leitura de teclas de função

    generateFood(); // Gera a primeira comida
    sem_init(&semaphore, 0, 1); // Inicializa o semáforo

    std::thread snakeThread(moveSnake);

    // Espera o thread da cobra terminar (nunca vai, apenas para exemplificação)
    snakeThread.join();

    sem_destroy(&semaphore); // Destrói o semáforo
    endwin(); // Finaliza o modo ncurses
    return 0;
}
