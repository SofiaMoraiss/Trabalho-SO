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

# Compila o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compila .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para executar o programa
run: $(TARGET)
	./$(TARGET)

val: $(TARGET)
	valgrind ./$(TARGET)

# Limpa os arquivos compilados
clean:
	rm -f $(OBJS) $(TARGET)

# Recompila do zero
rebuild: clean all

.PHONY: all clean rebuild run
