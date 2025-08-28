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
	@echo "Executando o programa para todas as entradas..."
	@mkdir -p nossas_saidas/mono nossas_saidas/multi
	@for entrada in $(wildcard entradas/*.txt); do \
		nome_base=$$(basename $$entrada .txt); \
		echo "Processando $$entrada no modo MONO..."; \
		./$(TARGET) "$$entrada" "mono" > "nossas_saidas/mono/log_execucao_minikernel$$nome_base.txt"; \
		echo "Processando $$entrada no modo MULTI..."; \
		./$(TARGET) "$$entrada" "multi" > "nossas_saidas/multi/log_execucao_minikernel$$nome_base.txt"; \
	done
	@echo "Todas as execuções foram concluídas."
	$(MAKE) clean

# Versão com Valgrind
run-valgrind: $(TARGET)
	@echo "Executando o programa com Valgrind para todas as entradas..."
	@mkdir -p nossas_saidas/mono nossas_saidas/multi
	@for entrada in $(wildcard entradas/*.txt); do \
		nome_base=$$(basename $$entrada .txt); \
		echo "Valgrind -> $$entrada (MONO)"; \
		valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) "$$entrada" "mono" > "nossas_saidas/mono/valgrind_log_execucao_minikernel$$nome_base.txt" 2> "nossas_saidas/mono/valgrind_log_$$nome_base.txt"; \
		echo "Valgrind -> $$entrada (MULTI)"; \
		valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) "$$entrada" "multi" > "nossas_saidas/multi/valgrind_log_execucao_minikernel$$nome_base.txt" 2> "nossas_saidas/multi/valgrind_log_$$nome_base.txt"; \
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