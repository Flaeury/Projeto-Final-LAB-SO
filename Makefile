# Variáveis
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread
LDFLAGS = -lncurses

# Nome do executável
TARGET = snake

# Arquivo fonte
SRC = snake.cpp

# Regra padrão
all: $(TARGET)

# Regra para compilar o executável
$(TARGET): $(SRC)
    $(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Limpeza dos arquivos gerados
clean:
    rm -f $(TARGET)

# Regra para executar o programa
run: $(TARGET)
    ./$(TARGET)