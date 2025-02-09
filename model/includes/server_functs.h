/**
 * @file server_functs.h
 * @author Alejandro Monterrubio & Esther Perez
 * @brief Funciones del servidor.
 * @version 0.5
 * @date 11/03/24
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _SERVER_FUNCTS
#define _SERVER_FUNCTS

#include "../includes/picohttpparser.h"
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h> /* Para read() */
#include <sys/socket.h> /* Para send() */
#include <time.h> /* Para time() */
#include <sys/stat.h>

/* Definiciones de distintos tamanos para guardar campos */
#define SIZE_32 32
#define SIZE_128 128
#define SIZE_512 512
#define SIZE_1024 1024
#define SIZE_2048 2048

/* Extensiones de los tipos de ficheros que soporta el servidor */
#define FILE_TYPES 11

/* Los que tienen const delante es porque daba fallo*/
typedef struct {
    const char *verb; /* Verbo a usar */
    size_t verb_length; /* Longitud del verbo*/
    const char *object_path; /* Ruta del objeto solicitado */
    size_t path_length; /* Tamano de la ruta*/
    int http_version; /* Dentro de HTTP1 el 1.1 */
    struct phr_header header[SIZE_128]; /* Cabecera de picohttpparser */
    size_t header_size; /* Tamano de la cabecera */
    const char *request_body; /* Cuerpo de la peticion */
} request_struct;

/* Estructura que relaciona tipo de fichero y su terminacion */
typedef struct {
    char *type; /* Por ejemplo text/plain */
    char *extension; /* Por ejemplo .txt */
} content_type;

/* Estructura que trata var=algo */
typedef struct {
    char *varX; /* Por ejemplo var1 */
    char *value; /* Por ejemplo abcd */
} variable_struct;

/**
 * @brief Procesa la peticion y llama a metodo correspondiente.
 *
 * @param connection Conexion del socket.
 * @param client_request Puntero a la peticion.
 * @return 0 si correcto, -1 si error.
 */
int request(int connection, request_struct *client_request);

/**
 * @brief Procesa una peticion con verbo GET.
 *
 * @param connection Conexion del socket.
 * @param client_request Puntero a la peticion.
 * @param path Ruta del fichero.
 * @param server_signature MyCoolServer1.1.
 * @return 0 si correcto, -1 si error.
 */
int get_request(int connection, request_struct *client_request, char *path, char *server_signature);

/**
 * @brief Procesa una peticion con verbo POST.
 *
 * @param connection Conexion del socket.
 * @param client_request Puntero a la peticion.
 * @param path Ruta del fichero.
 * @param server_signature MyCoolServer1.1.
 * @param body Cuerpo de la peticion.
 * @return 0 si correcto, -1 si error.
 */
int post_request(int connection, request_struct *client_request, char *path, char *server_signature, char *body);

/**
 * @brief Procesa una peticion con verbo OPTIONS.
 *
 * @param connection Conexion del socket.
 * @param client_request Puntero a la peticion.
 * @param server_signature MyCoolServer1.1.
 * @return 0 si correcto, -1 si error.
 */
int options_request(int connection, request_struct *client_request, char *server_signature);

/**
 * @brief Procesa el error 404.
 *
 * @param connection Conexion del socket.
 * @param server_signature MyCoolServer1.1.
 * @param http_version 1.1.
 * @return 0 si correcto, -1 si error.
 */
int error_404(int connection, char *server_signature, int http_version);

/**
 * @brief Procesa un verbo no implementado.
 *
 * @param connection Conexion del socket.
 * @param server_signature MyCoolServer1.1.
 * @param http_version 1.1.
 * @return 0 si correcto, -1 si error.
 */
int not_implemented_501(int connection, char *server_signature, int http_version);

/**
 * @brief Imprime todo menos content length y content type si OK.
 *
 * @param connection Conexion del socket.
 * @param server_signature MyCoolServer1.1.
 * @param http_version 1.1.
 * @return 0 si correcto, -1 si error.
 */
int ok_200_general(int connection, char *server_signature, int http_version);

/**
 * @brief Procesa una peticion sintacticamente incorrecta.
 *
 * @param connection Conexion del socket.
 * @param server_signature MyCoolServer1.1.
 * @param http_version 1.1.
 * @return 0 si correcto, -1 si error.
 */
int bad_request_400(int connection, char *server_signature, int http_version);

/**
 * @brief Ejecuta py o php.
 *
 * @param path Ruta del objeto.
 * @param variables Asociacion de variables y su valor.
 * @param number_variables Numero de variables usado (min 0, max 2).
 * @param file_type .py o .php
 * @param ok_response Respuesta del servidor a peticion.
 * @return 0 si correcto, -1 si error.
 */
int exe_script(char *path, variable_struct *variables, int number_variables, char *file_type, char *ok_response);

/**
 * @brief Da el fichero al cliente tras la peticion.
 *
 * @param connection Conexion del socket.
 * @param path Ruta del objeto.
 * @return 0 si correcto, -1 si error.
 */
int give_file(int connection, char *path);

#endif
