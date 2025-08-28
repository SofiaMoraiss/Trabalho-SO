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
	@mkdir -p nossas_saidas/mono nossas_saidas/multi
	@for i in `seq 1 7`; do \
		echo "Processando entradas/$$i.txt no modo MONO..."; \
		./$(TARGET) "entradas/$$i.txt" "mono" > "nossas_saidas/mono/saida$$i.txt"; \
		echo "Processando entradas/$$i.txt no modo MULTI..."; \
		./$(TARGET) "entradas/$$i.txt" "multi" > "nossas_saidas/multi/saida$$i.txt"; \
	done
	@echo "Todas as execuções foram concluídas."
	$(MAKE) clean

# Versão com Valgrind
run-valgrind: $(TARGET)
	@echo "Executando o programa com Valgrind para todas as 7 entradas..."
	@mkdir -p nossas_saidas/mono nossas_saidas/multi
	@for i in `seq 1 7`; do \
		echo "Valgrind -> entradas/$$i.txt (MONO)"; \
		valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) "entradas/$$i.txt" "mono" > "nossas_saidas/mono/valgrind_saida$$i.txt" 2> "nossas_saidas/mono/valgrind_log$$i.txt"; \
		echo "Valgrind -> entradas/$$i.txt (MULTI)"; \
		valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) "entradas/$$i.txt" "multi" > "nossas_saidas/multi/valgrind_saida$$i.txt" 2> "nossas_saidas/multi/valgrind_log$$i.txt"; \
	done
	@echo "Execução com Valgrind concluída."
	$(MAKE) clean

# Limpa os arquivos compilados e o executável
clean:
	rm -f $(OBJS) $(TARGET)

# Limpa apenas os arquivos de saída do Valgrind
clean-valgrind:
	rm -f nossas_saidas/mono/valgrind_saida*.txt \
	      nossas_saidas/mono/valgrind_log*.txt \
	      nossas_saidas/multi/valgrind_saida*.txt \
	      nossas_saidas/multi/valgrind_log*.txt

# Recompila do zero
rebuild: clean all

.PHONY: all clean clean-valgrind rebuild run run-valgrind
