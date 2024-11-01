#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>

using namespace std;

class PedraPapelTesoura {
private:
    mutex mtx;
    condition_variable cv;
    int escolha_jogador1;
    int escolha_jogador2;
    int derrotas_jogador1 = 0;
    int derrotas_jogador2 = 0;
    bool rodada_em_andamento = false;
    bool fim_do_jogo = false;
    bool modo_automatico; // Indica se o jogo é automático ou manual

public:
    PedraPapelTesoura(bool auto_mode) : escolha_jogador1(0), escolha_jogador2(0), modo_automatico(auto_mode) {}

    // Função para gerar uma escolha aleatória: 1 = Pedra, 2 = Papel, 3 = Tesoura
    int escolhaAleatoria() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 3);  // Valores entre 1 e 3
        return dis(gen);
    }

    // Função para traduzir escolha em texto
    string escolhaParaTexto(int escolha) {
        return (escolha == 1) ? "Pedra" : (escolha == 2) ? "Papel" : "Tesoura";
    }

    // Função para o jogador fazer sua escolha (manual para jogador 1 e automático para jogador 2)
    void escolher(int jogador) {
        unique_lock<mutex> lock(mtx);

        int escolha;
        if (jogador == 1) {
            if (modo_automatico) {
                escolha = escolhaAleatoria(); // Escolha aleatória no modo automático
            } else {
                // Modo manual: jogador 1 escolhe
                cout << "Jogador 1, escolha sua jogada (1 = Pedra, 2 = Papel, 3 = Tesoura): ";
                cin >> escolha;
                while (escolha < 1 || escolha > 3) {
                    cout << "Escolha inválida. Tente novamente (1 = Pedra, 2 = Papel, 3 = Tesoura): ";
                    cin >> escolha;
                }
            }
            escolha_jogador1 = escolha;
            cout << "Jogador 1 escolheu: " << escolhaParaTexto(escolha_jogador1) << endl;
        } else if (jogador == 2) {
            escolha_jogador2 = escolhaAleatoria(); // Escolha aleatória para o jogador 2
            cout << "Jogador 2 (Computador) escolheu: " << escolhaParaTexto(escolha_jogador2) << endl;
        }

        // Se ambos os jogadores fizeram suas escolhas, inicia a rodada
        if (escolha_jogador1 != 0 && escolha_jogador2 != 0) {
            rodada_em_andamento = true;
            cv.notify_all();
        } else {
            cv.wait(lock, [this] { return rodada_em_andamento; });
        }
    }

    // Função para comparar escolhas e decidir o vencedor
    void compararEscolhas() {
        unique_lock<mutex> lock(mtx);

        // Espera até que ambos os jogadores tenham feito suas escolhas
        cv.wait(lock, [this] { return rodada_em_andamento; });

        cout << "Comparando escolhas..." << endl;

        if (escolha_jogador1 == escolha_jogador2) {
            cout << "Empate!" << endl;
        } else if ((escolha_jogador1 == 1 && escolha_jogador2 == 3) ||
                   (escolha_jogador1 == 2 && escolha_jogador2 == 1) ||
                   (escolha_jogador1 == 3 && escolha_jogador2 == 2)) {
            cout << "Jogador 1 vence a rodada!" << endl;
            derrotas_jogador2++;
        } else {
            cout << "Jogador 2 vence a rodada!" << endl;
            derrotas_jogador1++;
        }

        // Reseta escolhas para a próxima rodada
        escolha_jogador1 = 0;
        escolha_jogador2 = 0;
        rodada_em_andamento = false;

        // Verifica se alguém perdeu o jogo (3 derrotas)
        if (derrotas_jogador1 == 3) {
            cout << "Jogador 2 é o vencedor do jogo! Jogador 1 perdeu com 3 derrotas." << endl;
            fim_do_jogo = true;
            cv.notify_all();
        } else if (derrotas_jogador2 == 3) {
            cout << "Jogador 1 é o vencedor do jogo! Jogador 2 perdeu com 3 derrotas." << endl;
            fim_do_jogo = true;
            cv.notify_all();
        } else {
            cv.notify_all();
        }
    }

    // Função principal para rodar o jogo
    void jogar() {
        while (!fim_do_jogo) {
            thread jogador1(&PedraPapelTesoura::escolher, this, 1);
            thread jogador2(&PedraPapelTesoura::escolher, this, 2);

            compararEscolhas();

            jogador1.join();
            jogador2.join();

            if (!fim_do_jogo) {
                // Pausa para próxima rodada
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
    }
};

int main() {
    char modo;
    cout << "Deseja jogar manualmente ou deixar o computador jogar sozinho? (m = manual, a = automático): ";
    cin >> modo;
    bool auto_mode = (modo == 'a');

    PedraPapelTesoura jogo(auto_mode);
    jogo.jogar();
    return 0;
}
