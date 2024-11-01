# Jogo da Cobrinha em C++ com ncurses e Semáforos

Este projeto é uma implementação do clássico jogo da cobrinha em C++ usando a biblioteca `ncurses` para exibir o jogo no terminal e semáforos para sincronização. Ele explora conceitos de concorrência com threads e o uso de semáforos para garantir uma execução segura e coordenada entre as partes do código.

## Funcionalidades

- **Movimento fluido da cobra** com controle contínuo de direção.
- **Sincronização com threads e semáforo** para garantir uma execução paralela e responsiva.
- **Geração de comida** em posições aleatórias no campo de jogo.
- **Verificação de colisões** com as bordas e com o próprio corpo da cobra.

## Estrutura do Código

O projeto é organizado em várias funções principais:

- **`main()`**: inicializa o jogo, cria as threads e inicia a sincronização com o semáforo.
- **`desenhar()`**: responsável por renderizar o campo de jogo no terminal, desenhando bordas, a cobra e a comida.
- **`moverCobra()`**: executado em uma thread separada, atualiza a posição da cobra conforme a direção atual e verifica colisões. Se a cobra colide com comida, aumenta de tamanho e gera nova comida.
- **`lerEntrada()`**: executado em outra thread, captura as teclas pressionadas pelo usuário e ajusta a direção da cobra.
- **`checarColisao()`**: verifica se a cobra colidiu com a parede ou com seu próprio corpo.
- **`gerarComida()`**: posiciona a comida em um local aleatório dentro do campo, evitando a posição da cabeça da cobra.

## Dependências

Para compilar e rodar o código, é necessário ter o `ncurses` instalado, uma biblioteca para manipulação de interfaces de texto.

### Instalação do `ncurses`

No Linux:

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

No MacOS (usando Homebrew):

```bash
brew install ncurses
```

## Compilação e Execução

Compile o programa com o seguinte comando:

```bash
g++ -o jogo_cobrinha jogo_cobrinha.cpp -lncurses -pthread
```

E então execute o programa:

```bash
./jogo_cobrinha
```

## Explicação Técnica

### Threads e Sincronização

Este jogo utiliza duas threads principais:

- Uma para **movimentar a cobra** e verificar as colisões.
- Outra para **capturar a entrada do usuário**.

As threads operam em paralelo, garantindo que a cobra continue se movendo enquanto o usuário pode alterar a direção a qualquer momento. Isso é essencial para a responsividade do jogo.

### Uso de Semáforo

O semáforo (`sem_t semaforo`) garante que apenas uma thread acesse e modifique a posição da cobra e sua direção de cada vez, evitando conflitos. Quando a `moverCobra()` ou a `lerEntrada()` precisa alterar a posição ou a direção, primeiro espera pelo semáforo (`sem_wait(&semaforo)`), executa a operação e, em seguida, libera o semáforo (`sem_post(&semaforo)`). Sem o uso de semáforo, o programa poderia apresentar condições de corrida, levando a comportamentos imprevisíveis e falhas de execução.

### Função `desenhar()`

A função `desenhar()` é chamada após cada movimento para atualizar o campo de jogo no terminal. Ela limpa a tela, desenha as bordas, exibe a posição atual da cobra e da comida, e atualiza o terminal. Isso cria o efeito visual do movimento da cobra.

### Função `checarColisao()`

A `checarColisao()` verifica se a cobra colidiu com as bordas ou com seu próprio corpo. Se ocorrer uma colisão, o jogo exibe uma mensagem e finaliza.

## Controles

Use as teclas a seguir para controlar a direção da cobra:

- **W**: Mover para cima
- **S**: Mover para baixo
- **A**: Mover para a esquerda
- **D**: Mover para a direita
