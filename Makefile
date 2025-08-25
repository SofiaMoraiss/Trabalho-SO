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
all: $(TARGET) clean

# Compila o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compila .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para executar o programa
run: $(TARGET)
	./$(TARGET) >out1.txt

val: $(TARGET) clean
	valgrind --leak-check=full ./$(TARGET)

# Limpa os arquivos compilados
clean:
	rm -f $(OBJS)

# Recompila do zero
rebuild: clean all

.PHONY: all clean rebuild run val
