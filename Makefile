# Nome do executável
TARGET = programa

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Todos os arquivos .c no diretório
SRCS := $(wildcard *.c)

# Arquivos .o correspondentes
OBJS := $(SRCS:.c=.o)

# Regra padrão
all: $(TARGET)

# Compila o executável, incluindo a biblioteca de threads
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -pthread

# Compila .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra principal para executar o programa para todas as entradas
run: $(TARGET)
	@echo "Executando o programa para todas as 7 entradas..."
	@for i in `seq 1 7`; do \
		echo "Processando entradas/$$i.txt..."; \
		./$(TARGET) "entradas/$$i.txt" > "saidas/saida$$i.txt"; \
	done
	@echo "Todas as execuções foram concluídas."
	$(MAKE) clean

# Limpa os arquivos compilados e o executável
clean:
	rm -f $(OBJS) $(TARGET)

# Recompila do zero
rebuild: clean all

.PHONY: all clean rebuild run