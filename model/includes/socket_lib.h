/**
 * @file socket_lib.h
 * @author Alejandro Monterrubio & Esther Perez
 * @brief Biblioteca con funciones de sockets.
 * @version 0.1
 * @date 27-02-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _SOCKET_LIB
#define _SOCKET_LIB

#include <stdio.h>
#include <errno.h> /* para perror */
#include <unistd.h>
#include <stdlib.h> /* para exit() */
#include <sys/socket.h> /* para llamar a funciones de sockets */
#include <netinet/in.h> /* para htons y htol */
#include <strings.h> /* para bzero */

/**
 * @brief Crea el socket.
 *
 * @param domain Dominio de comunicacion.
 * @param type Protocolo de comunicacion (TCP).
 * @param protocol Protocolo de internet.
 * @return Descriptor del socket.
 */
int lib_socket(int domain, int type, int protocol);

/**
 * @brief Une direccion con puerto.
 * 
 * @param sockfd Descriptor del socket.
 * @param address Direccion del socket.
 * @param port Puerto del socket.
 * @return Descriptor del socket.
 */
int lib_bind(int sockfd, int address, int port);

/**
 * @brief Acepta peticiones.
 *
 * @param sockfd Descriptor del socket.
 * @return Descriptor del socket.
 */
int lib_accept(int sockfd);

#endif
