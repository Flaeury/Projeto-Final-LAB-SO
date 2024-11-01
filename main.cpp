#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <condition_variable>
#include <random>
#include <map>

using namespace std;

class PedraPapelTesoura {
private:
    binary_semaphore semaforo_rodada{1}; // Controla o início da rodada
    mutex mtx;
    condition_variable cv;
    map<int, int> derrotas; // Contador de derrotas de cada jogador
    map<int, int> escolhas; // Escolhas de cada jogador (1: Pedra, 2: Papel, 3: Tesoura)
    bool fim_do_jogo = false;
    int jogadores_prontos = 0;

public:
    PedraPapelTesoura() {
        derrotas[1] = 0;
        derrotas[2] = 0;
        derrotas[3] = 0;
    }

    // Função para gerar uma escolha aleatória: 1 = Pedra, 2 = Papel, 3 = Tesoura
    int escolhaAleatoria() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 3);  // Valores entre 1 e 3
        return dis(gen);
    }

    // Inicializa a escolha de cada jogador
    void inicializarEscolhas() {
        escolhas[1] = escolhaAleatoria();
        escolhas[2] = escolhaAleatoria();
        escolhas[3] = escolhaAleatoria();

        cout << "Jogador 1 escolheu: " << escolhaParaTexto(escolhas[1]) << endl;
        cout << "Jogador 2 escolheu: " << escolhaParaTexto(escolhas[2]) << endl;
        cout << "Jogador 3 escolheu: " << escolhaParaTexto(escolhas[3]) << endl;
    }

    // Função para traduzir escolha em texto
    string escolhaParaTexto(int escolha) {
        return (escolha == 1) ? "Pedra" : (escolha == 2) ? "Papel" : "Tesoura";
    }

    // Função para comparar escolhas de dois jogadores
    int comparar(int jogador1, int jogador2) {
        int escolha1 = escolhas[jogador1];
        int escolha2 = escolhas[jogador2];

        if (escolha1 == escolha2) {
            cout << "Empate entre jogador " << jogador1 << " e jogador " << jogador2 << "!" << endl;
            return jogador1; // Empate, retorna o primeiro
        }
        if ((escolha1 == 1 && escolha2 == 3) || (escolha1 == 2 && escolha2 == 1) || (escolha1 == 3 && escolha2 == 2)) {
            cout << "Jogador " << jogador1 << " vence contra jogador " << jogador2 << endl;
            derrotas[jogador2]++; // Jogador 2 perde
            return jogador1;
        } else {
            cout << "Jogador " << jogador2 << " vence contra jogador " << jogador1 << endl;
            derrotas[jogador1]++; // Jogador 1 perde
            return jogador2;
        }
    }

    // Função principal da rodada
    void rodada() {
        while (!fim_do_jogo) {
            // Controle de início da rodada
            semaforo_rodada.acquire();
            inicializarEscolhas();

            unique_lock<mutex> lock(mtx);

            // Compara jogadores na ordem: 1 com 2, depois o ganhador com 3
            int vencedor = comparar(1, 2);
            vencedor = comparar(vencedor, 3);

            // Verifica eliminações
            verificarEliminados();

            jogadores_prontos++;
            cv.notify_all(); // Libera as threads para a próxima rodada

            if (!fim_do_jogo) {
                lock.unlock();
                this_thread::sleep_for(chrono::seconds(2));
                semaforo_rodada.release(); // Libera o semáforo para a próxima rodada
            }
        }
    }

    // Função para verificar eliminações e declarar o vencedor final
    void verificarEliminados() {
        for (auto it = derrotas.begin(); it != derrotas.end();) {
            if (it->second >= 3) {
                cout << "Jogador " << it->first << " foi eliminado com 3 derrotas." << endl;
                it = derrotas.erase(it);
            } else {
                ++it;
            }
        }

        // Verifica se restou apenas um jogador
        if (derrotas.size() == 1) {
            fim_do_jogo = true;
            cout << "Jogador " << derrotas.begin()->first << " é o vencedor final!" << endl;
        }
    }
};

int main() {
    PedraPapelTesoura jogo;

    // Inicia uma thread para rodadas
    thread t(&PedraPapelTesoura::rodada, &jogo);

    // Espera o fim do jogo
    t.join();

    return 0;
}
