CC = gcc
CFLAGS = -Wall -Wextra -pthread -I../includes

# Archivos fuente del servidor y cliente
SRC_SERVER_ITERATIVO = ../src/serverIterativo.c ../includes/picohttpparser/picohttpparser.c
SRC_SERVER_CONCURRENTE = ../src/serverHTTPConcurrente.c ../includes/picohttpparser/picohttpparser.c
SRC_CLIENT = ../src/clients.c

# Objetos generados de los archivos fuente
OBJ_SERVER_ITERATIVO = $(SRC_SERVER_ITERATIVO:.c=.o)
OBJ_SERVER_CONCURRENTE = $(SRC_SERVER_CONCURRENTE:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)

# Ejecutables generados
TARGET_SERVER_ITERATIVO = serverIterativo
TARGET_SERVER_CONCURRENTE = serverConcurrente
TARGET_CLIENT = cliente

# Regla principal (compila todos los ejecutables)
all: $(TARGET_SERVER_ITERATIVO) $(TARGET_SERVER_CONCURRENTE) $(TARGET_CLIENT)

# Compilación del servidor iterativo
$(TARGET_SERVER_ITERATIVO): $(OBJ_SERVER_ITERATIVO)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER_ITERATIVO) $(OBJ_SERVER_ITERATIVO)

# Compilación del servidor concurrente
$(TARGET_SERVER_CONCURRENTE): $(OBJ_SERVER_CONCURRENTE)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER_CONCURRENTE) $(OBJ_SERVER_CONCURRENTE)

# Compilación del cliente
$(TARGET_CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT) $(OBJ_CLIENT)

# Regla para compilar archivos .c a .o (servidores)
../src/%.o: ../src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJ_SERVER_ITERATIVO) $(OBJ_SERVER_CONCURRENTE) $(OBJ_CLIENT) $(TARGET_SERVER_ITERATIVO) $(TARGET_SERVER_CONCURRENTE) $(TARGET_CLIENT)
