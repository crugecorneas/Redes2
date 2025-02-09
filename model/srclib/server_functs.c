#include "../includes/server_functs.h"

/* Asociacion entre fichero y terminacion */
content_type types [] = {
    {"text/plain", ".txt"},
    {"text/html", ".html"},
    {"text/html", ".htm"},
    {"text/gif", ".gif"},
    {"text/jpeg", ".jpeg"},
    {"text/jpeg", ".jpg"},
    {"text/mpeg", ".mpeg"},
    {"text/mpeg", ".mpg"},
    {"application/msword", ".doc"},
    {"application/msword", ".docx"},
    {"application/pdf", ".pdf"},
};

int request(int connection, request_struct *client_request){
	ssize_t read_request; /* Si no va al principio sale error */
    size_t reading_buffer = 0; /* Si no va al principio sale error */
    FILE *fd = NULL;
    int parse_return;
    char *value = NULL;
    char verb_requested[SIZE_32];
	char server_root[SIZE_128], server_signature[SIZE_128], aux[SIZE_128], buffer[SIZE_1024];
    char server_path[SIZE_512], server_path2[SIZE_512];

	/* Abrir fichero de configuracion */
    if(!(fd = fopen("server.conf", "r"))){
        perror("fopen");
        return -1;
    }

	/* Leer server_root y server_signature */
    while (fgets(aux, SIZE_128, fd)) {
        if (strncmp("server_root", aux, strlen("server_root")) == 0){
            strtok(aux, " \n");
            strtok(NULL, " \n");
            sprintf(server_root, "%s", strtok(NULL, " \n"));
        }
        if (strncmp("server_signature", aux, strlen("server_signature")) == 0) {
            strtok(aux, "=");
            value = strtok(NULL, "\n");
            sscanf(value, " %[^\n]", server_signature);
        }
    }

	fclose(fd);

    client_request->header_size = SIZE_128;

	/* Leer peticion */
	while(true){
		if((read_request = read(connection, buffer+reading_buffer, SIZE_1024-reading_buffer))<1){
			perror("read request");
			return -1;
		}
        /* Funcion de httppicoparser */
        parse_return = phr_parse_request(buffer, reading_buffer+read_request, &(client_request->verb),
         &(client_request->verb_length), &(client_request->object_path), &(client_request->path_length),
         &(client_request->http_version), client_request->header, &(client_request->header_size), reading_buffer);

        if( (reading_buffer += read_request)  == sizeof(buffer) ){
            perror("request is too long");
            return -1;
        }

        /* Comprueba el resultado de parsear la peticion */
        if(parse_return == -1){ /* Partial request o failed request*/
            bad_request_400(connection, server_signature, client_request->http_version);
            return -1;
        }
        else if(parse_return > 0){
            break; /* Se consume un numero correcto de bytes */
        }
	}

    if(client_request->path_length > 1){
        sprintf(server_path, "%s%.*s", server_root, (int)client_request->path_length, client_request->object_path);
    }
    else{
        strcpy(server_path, server_root);
        strcat(server_path, "/index.html");
    }
    /* Al hacer post el path cambiaba no se por que*/
    strcpy(server_path2, server_path);

    /* SI NO SE PONE ESTO SALE CURL(52) EMPTY REPLY FROM SERVER */
    sprintf(verb_requested, "%.*s", (int)client_request->verb_length, client_request->verb);
    sprintf(aux, "%.*s", (int)reading_buffer, buffer);
    value = strrchr(aux, '\n')+1;
    /* SI NO SE PONE ESTO SALE CURL(52) EMPTY REPLY FROM SERVERS */

    /* Cliente solicita GET */
    if(strcmp(verb_requested, "GET") == 0){
        get_request(connection, client_request, server_path, server_signature);
    }
    /* Cliente solicita POST */
    else if(strcmp(verb_requested, "POST") == 0){
        post_request(connection, client_request, server_path2, server_signature, value);
    }
    /* Cliente solicita OPTIONS */
    else if(strcmp(verb_requested, "OPTIONS") == 0)
        options_request(connection, client_request, server_signature);
    /* Cliente solicita verbo no soportado */
    else if((strcmp(verb_requested, "HEAD")==0)||(strcmp(verb_requested, "PUT")==0)||(strcmp(verb_requested, "DELETE")==0))
        not_implemented_501(connection, server_signature, client_request->http_version);
    /* Cliente escribe mal la peticion */
    else
        bad_request_400(connection, server_signature, client_request->http_version);

    return 0;
}

int get_request(int connection, request_struct *client_request, char *path, char *server_signature){
    char *get_variables = NULL, *aux_var = NULL, *aux = NULL, *file_type = NULL;
    char ok_response[SIZE_1024], html_response[SIZE_2048], html_response2[SIZE_512], aux2[SIZE_32];
    int number_variables = 0, i;
    variable_struct variables[SIZE_32];
    struct stat in_server;
    content_type *type = types;

    if(connection<0 || !client_request || !path || !server_signature){
        perror("arguments not valid");
        return -1;
    }

    get_variables = strrchr(path, '?') +1;

    /* GET puede tener 1 o 2 variables */
    if(get_variables > path){
        /* strchr necesita '' en vez de "" porque da fallo sino */
        /* una variable*/
        if(strchr(get_variables, '=') != NULL){
            number_variables++;
            variables[0].varX = strtok(get_variables, "="); /* Antes del = */
            variables[0].value = strtok(NULL, "\0"); /* El resto */
        }
        /* GET /scripts/backend.py?var1=abcd&var2=efgh */
        else if(strchr(get_variables, '&') != NULL){
            aux_var = strtok_r(get_variables, "&", &aux);
            while(aux_var != NULL){
                variables[number_variables].varX = strtok(aux_var, "="); /* Antes del = */
                variables[number_variables].value = strrchr(aux_var, '=')+1; /* El resto */
                number_variables++;
                aux_var = strtok_r(NULL, "&\0", &aux);
            }
        }
        path = strtok(path, "?");
    }

    /* Si no esta el recurso, llamada a 404 */
    if(stat(path, &in_server) != 0){
        error_404(connection, server_signature, client_request->http_version);
        return 0;
    }
    /* Si esta el recurso, llamada a 200 */
    else{
        file_type = strrchr(path, '.'); /* Con '' que sino da fallo */
        /* Ver extension del fichero */
        for(i=0; i!=FILE_TYPES; i++){
            if(strcmp(file_type, (type+1)->extension) == 0){
                strcpy(aux2, (type+1)->type);
                break;
            }
        }
        /* Imprime todo menos content length y content type */
        ok_200_general(connection, server_signature, client_request->http_version);

        /* El fichero es ejecutable -> .py o .php*/
        if( (strcmp(file_type, ".py") == 0) || (strcmp(file_type, ".php") == 0) ){
            /* Imprime lo que queda de imprimir del 200 OK */
            strcpy(ok_response, "");
            /* FUNCION DE EJECUTAR FICHERO */
            exe_script(path, variables, number_variables, file_type, ok_response);
            sprintf(html_response, "<!DOCTYPE HTML PUBLIC>\n<html><head>\n</head>\n<body>\n%s\n</body>\n</html>\n", ok_response);
            /* html_response2 es porque daba warnings si se usaba el mismo */
            strcpy(html_response2, html_response);
            sprintf(ok_response, "Content-Length: %ld\r\n", strlen(html_response2));
            send(connection, ok_response, strlen(ok_response), 0);
            sprintf(ok_response, "Content-Type: text/html\r\n\r\n%s", html_response2);
            send(connection, ok_response, strlen(ok_response), 0);
        }
        /* El fichero no es ejecutable */
        else{
            /* Imprime lo que queda de imprimir del 200 OK */
            sprintf(ok_response, "Content-Length: %ld\r\n", in_server.st_size);
            send(connection, ok_response, strlen(ok_response), 0);
            sprintf(ok_response, "Content-Type: %s\r\n\r\n", file_type);
            send(connection, ok_response, strlen(ok_response), 0);
            /* FUNCION DE DAR FICHERO AL CLIENTE */
            give_file(connection, path);
        }
    }

    return 0;
}

int post_request(int connection, request_struct *client_request, char *path, char *server_signature, char *body){
    char *aux_var = NULL, *aux = NULL, *file_type = NULL;
    char ok_response[SIZE_1024], html_response[SIZE_2048], html_response2[SIZE_512], aux2[SIZE_32];
    int number_variables = 0, i;
    variable_struct variables[SIZE_32];
    struct stat in_server;
    content_type *type = types;

    if(connection<0 || !client_request || !path || !server_signature || !body){
        perror("arguments not valid");
        return -1;
    }

    /* strchr necesita '' en vez de "" porque da fallo sino */
    /* una variable */
    if(strchr(body, '=') != NULL){
        number_variables++;
        variables[0].varX = strtok(body, "="); /* Antes del = */
        variables[0].value = strtok(NULL, "\0"); /* El resto */
    }
    /* dos variables */
    else if(strchr(body, '&') != NULL){
        aux_var = strtok_r(body, "&", &aux);
        while(aux_var != NULL){
            variables[number_variables].varX = strtok(aux_var, "="); /* Antes del = */
            variables[number_variables].value = strrchr(aux_var, '=')+1; /* El resto */
            number_variables++;
            aux_var = strtok_r(NULL, "&\0", &aux);
        }
    }

    /* Si no esta el recurso, llamada a 404 */
    if(stat(path, &in_server) != 0){
        error_404(connection, server_signature, client_request->http_version);
        return 0;
    }
    /* Si esta el recurso, llamada a 200 */
    else{
        file_type = strrchr(path, '.'); /* Con '' que sino da fallo */
        /* Ver extension del fichero */
        for(i=0; i<FILE_TYPES; i++){
            if(strcmp(file_type, (type+1)->extension) == 0){
                strcpy(aux2, (type+1)->type);
                break;
            }
        }
        /* Imprime todo menos content length y content type */
        ok_200_general(connection, server_signature, client_request->http_version);

        /* El fichero es ejecutable -> .py o .php*/
        if( (strcmp(file_type, ".py") == 0) || (strcmp(file_type, ".php") == 0) ){
            /* Imprime lo que queda de imprimir del 200 OK */
            strcpy(ok_response, "");
            /* FUNCION DE EJECUTAR FICHERO */
            exe_script(path, variables, number_variables, file_type, ok_response);
            sprintf(html_response, "<!DOCTYPE HTML PUBLIC>\n<html><body>\n<h1>%s</h1>\n</body></html>\n", ok_response);
            strcpy(html_response2, html_response);
            sprintf(ok_response, "Content-Length: %ld\r\n", strlen(html_response2));
            send(connection, ok_response, strlen(ok_response), 0); /* Obligatorio del enunciado: Content-Length */
            sprintf(ok_response, "Content-Type: text/html\r\n\r\n%s", html_response2);
            send(connection, ok_response, strlen(ok_response), 0);
        }
        /* El fichero no es ejecutable */
        else{
            /* Imprime lo que queda de imprimir del 200 OK */
            sprintf(ok_response, "Content-Length: %ld\r\n", in_server.st_size);
            send(connection, ok_response, strlen(ok_response), 0);
            sprintf(ok_response, "Content-Type: %s\r\n\r\n", file_type);
            send(connection, ok_response, strlen(ok_response), 0);
            /* FUNCION DE DAR FICHERO AL CLIENTE */
            give_file(connection, path);
        }
    }
    
    return 0;
}

int options_request(int connection, request_struct *client_request, char *server_signature){
    char options_response[SIZE_128];
    time_t date = time(NULL);

    if(!client_request || !server_signature){
        perror("arguments not valid");
        return -1;
    }

    sprintf(options_response, "HTTP/1.%d 204 No Content\r\n", client_request->http_version);
    send(connection, options_response, strlen(options_response), 0); /* HTTP/1.1 204 No Content */
    sprintf(options_response, "Allow: OPTIONS, GET, POST\r\n");
    send(connection, options_response, strlen(options_response), 0); /* Allow: OPTIONS, GET, POST */
    sprintf(options_response, "Date: %.*s\r\n", (int)strlen(ctime(&date))-1, ctime(&date));
    send(connection, options_response, strlen(options_response), 0); /* Obligatorio del enunciado: Date */
    sprintf(options_response, "Server: %s\r\n", server_signature);
    send(connection, options_response, strlen(options_response), 0); /* Obligatorio del enunciado: Server */
    sprintf(options_response, "Last-Modified: No data\r\n");
    send(connection, options_response, strlen(options_response), 0); /* Obligatorio del enunciado: Last-Modified */
    sprintf(options_response, "Content-Length: No data\r\n");
    send(connection, options_response, strlen(options_response), 0); /* Obligatorio del enunciado: Content-Length */
    sprintf(options_response, "Content-Type: No data\r\n\r\n");
    send(connection, options_response, strlen(options_response), 0); /* Obligatorio del enunciado: Content-type */    

    return 0;
}

int error_404(int connection, char *server_signature, int http_version){
    char error_response[SIZE_512];
    char html_response[SIZE_128] =
    "<!DOCTYPE HTML>\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Resource not found</h1>\n</body></html>\n";
    time_t date = time(NULL);

    if(!server_signature || http_version<0){
        perror("error_404");
        return 1;
    }

    sprintf(error_response, "HTTP/1.%d 404 Not Found\r\n", http_version);
    send(connection, error_response, strlen(error_response), 0); /* HTTP/1.1 404 Not Found */
    sprintf(error_response, "Date: %.*s\r\n", (int)strlen(ctime(&date))-1, ctime(&date));
    send(connection, error_response, strlen(error_response), 0); /* Obligatorio del enunciado: Date */
    sprintf(error_response, "Server: %s\r\n", server_signature);
    send(connection, error_response, strlen(error_response), 0); /* Obligatorio del enunciado: Server */
    /* Last modified no incluido*/
    sprintf(error_response, "Content-Length: %ld\r\n", strlen(html_response));
    send(connection, error_response, strlen(error_response), 0); /* Obligatorio del enunciado: Content-Length */
    sprintf(error_response, "Content-Type: text/html\r\n\r\n");
    send(connection, error_response, strlen(error_response), 0); /* Obligatorio del enunciado: Content-type */
    send(connection, html_response, strlen(html_response), 0); /* Mensaje html */

    return 0;
}

int not_implemented_501(int connection, char *server_signature, int http_version){
    char not_implemented_response[SIZE_512];
    char html_response[SIZE_512] =
    "<!DOCTYPE HTML>\n<html><head>\n<title>501 Method Not Implemented</title>\n</head><body>\n<h1>Method Not Implemented</h1>\n</body></html>\n";
    time_t date = time(NULL);

    if(!server_signature || http_version<0){
        perror("not_implemented_501");
        return 1;
    }

    sprintf(not_implemented_response, "HTTP/1.%d 501 Method Not Implemented\r\n", http_version);
    send(connection, not_implemented_response, strlen(not_implemented_response), 0); /* HTTP/1.1 501 Method Not Implemented */
    sprintf(not_implemented_response, "Date: %.*s\r\n", (int)strlen(ctime(&date))-1, ctime(&date));
    send(connection, not_implemented_response, strlen(not_implemented_response), 0); /* Obligatorio del enunciado: Date */
    sprintf(not_implemented_response, "Server: %s\r\n", server_signature);
    send(connection, not_implemented_response, strlen(not_implemented_response), 0); /* Obligatorio del enunciado: Server */
    /* Last modified no incluido*/
    sprintf(not_implemented_response, "Content-Length: %ld\r\n", strlen(html_response));
    send(connection, not_implemented_response, strlen(not_implemented_response), 0); /* Obligatorio del enunciado: Content-Length */
    sprintf(not_implemented_response, "Content-Type: text/html\r\n\r\n");
    send(connection, not_implemented_response, strlen(not_implemented_response), 0); /* Obligatorio del enunciado: Content-type */
    send(connection, html_response, strlen(html_response), 0); /* Mensaje html */

    return 0;
}

int ok_200_general(int connection, char *server_signature, int http_version){
    char ok_response[SIZE_512];
    time_t date = time(NULL);
    struct stat time;
    
    if(!server_signature || http_version<0){
        perror("ok_200_general");
        return 1;
    }

    sprintf(ok_response, "HTTP/1.%d 200 OK\r\n", http_version);
    send(connection, ok_response, strlen(ok_response), 0); /* HTTP/1.1 200 OK */
    sprintf(ok_response, "Date: %.*s\r\n", (int)strlen(ctime(&date))-1, ctime(&date));
    send(connection, ok_response, strlen(ok_response), 0); /* Obligatorio del enunciado: Date */
    sprintf(ok_response, "Server: %s\r\n", server_signature);
    send(connection, ok_response, strlen(ok_response), 0); /* Obligatorio del enunciado: Server */
    sprintf(ok_response, "Last-Modified: %.*s\r\n", (int) strlen(ctime(&time.st_mtime))-1, ctime(&time.st_mtime));
    send(connection, ok_response, strlen(ok_response), 0);

    return 0;
}

int bad_request_400(int connection, char *server_signature, int http_version){
    char bad_request_response[SIZE_512];
    char html_response[SIZE_512] =
    "<!DOCTYPE HTML>\n<html><head>\n<title>400 Bad Request</title>\n</head><body>\n<h1>Your browser sent a request that this server could not understand</h1>\n</body></html>\n";
    time_t date = time(NULL);

    if(!server_signature || http_version<0){
        perror("bad_request_400");
        return 1;
    }

    sprintf(bad_request_response, "HTTP/1.%d 400 Bad Request\r\n", http_version);
    send(connection, bad_request_response, strlen(bad_request_response), 0); /* HTTP/1.1 400 Bad Request */
    sprintf(bad_request_response, "Date: %.*s\r\n", (int)strlen(ctime(&date))-1, ctime(&date));
    send(connection, bad_request_response, strlen(bad_request_response), 0); /* Obligatorio del enunciado: Date */
    sprintf(bad_request_response, "Server: %s\r\n", server_signature);
    send(connection, bad_request_response, strlen(bad_request_response), 0); /* Obligatorio del enunciado: Server */
    /* Last modified no incluido*/
    sprintf(bad_request_response, "Content-Length: %ld\r\n", strlen(html_response));
    send(connection, bad_request_response, strlen(bad_request_response), 0); /* Obligatorio del enunciado: Content-Length */
    sprintf(bad_request_response, "Content-Type: text/html\r\n\r\n");
    send(connection, bad_request_response, strlen(bad_request_response), 0); /* Obligatorio del enunciado: Content-type */
    send(connection, html_response, strlen(html_response), 0); /* Mensaje html */

    return 0;
}

int exe_script(char *path, variable_struct *variables, int number_variables, char *file_type, char *ok_response){
    char terminal[SIZE_512] = "python3 ", buffer[SIZE_1024];
    int i, read_buffer;
    FILE *f = NULL;
    
    if(!path || !variables || number_variables<0 || !file_type || !ok_response){
        perror("exe_script()");
        return -1;
    }

    /* Ver que tipo es */
    /*CON ESTO FALLABA if(strcmp(file_type, ".py") == 0) strcpy(terminal, "py ");*/
    if(strcmp(file_type, ".php") == 0) strcpy(terminal, "php ");
    
    strcat(terminal, path);

    for(i=0; i<number_variables; i++){
        strcat(terminal, " ");
        strcat(terminal, variables[i].value);
    }

    /* Abrir fichero a ejecutar */
    if(!(f = popen(terminal, "r"))){
        perror("open file");
        return -1;
    }

    while(!feof(f)){
        read_buffer = fread(buffer, sizeof(char), SIZE_1024, f);
        if(read_buffer > 0){
            strncat(ok_response, buffer, read_buffer);
            strcat(ok_response, "\n\0");
            
        }
        else if(read_buffer < 0){ /* Fichero vacio*/
            pclose(f);
            return -1;
        }
    }

    pclose(f);
    return 0;
}

int give_file(int connection, char *path){
    FILE *f = NULL;
    int read_buffer;
    char buffer[SIZE_1024];

    if(connection<0 || !path){
        perror("give_file()");
        return -1;
    }

    if(!(f = fopen(path, "r"))){
        perror("open file");
        return -1;
    }

    while(!feof(f)){
        read_buffer = fread(buffer, sizeof(char), SIZE_1024, f);
        if(read_buffer > 0)
            send(connection, buffer, read_buffer, 0);
        else if(read_buffer < 0){ /* Fichero vacio */
            fclose(f);
            return -1;
        }
    }

    fclose(f);
    return 0;
}