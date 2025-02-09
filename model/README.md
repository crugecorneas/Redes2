# Requisitos previos
Descargar la carpeta *media.zip* y descomprimirla dentro de la carpeta *htmlfiles*

# Arrancar el servidor
make all

./server

# Mandar peticiones del cliente
Desde otra terminal diferente al servidor ejecutar el comando que corresponda
## Verbo OPTIONS
curl -i -X OPTIONS http://localhost:8080
## Verbo GET de algún fichero sin guardarlo
curl -i http://localhost:8080/carpeta/fichero.tipo

Por ejemplo: curl -i http://localhost:8080/media/texto.txt
## Verbo GET de algún fichero y guardarlo
curl -o nombrenuevo.tipo http://localhost:8080/carpeta/fichero.tipo

Por ejemplo: curl -o guardar1.jpg http://localhost:8080/media/img1.jpg
## Verbo GET de algún fichero con una variable
curl -i http://localhost:8080/carpeta/fichero.tipo?una=abcd

Por ejemplo: curl -i http://localhost:8080/scripts/test.py?line=hola
## Verbo GET de algún fichero con dos variables
curl -i http://localhost:8080/carpeta/fichero.tipo?una=abcd&dos=efgh

## Verbo POST sin parámetros (devuelve *index.html*)
curl -i -X POST http://localhost:8080/
## Verbo POST de algún fichero sin guardarlo
curl -i -X POST http://localhost:8080/carpeta/fichero.tipo

Por ejemplo: curl -i -X POST http://localhost:8080/media/texto.txt
## Verbo POST de algún fichero y guardarlo
curl -o nombrenuevo.tipo -X POST http://localhost:8080/carpeta/fichero.tipo

Por ejemplo: curl -o guardar2.jpg -X POST http://localhost:8080/media/img1.jpg
## Verbo POST de algún fichero con una variable
curl -i -d "una=abcd" -X POST http://localhost:8080/carpeta/fichero.tipo

Por ejemplo: curl -i -d "line=hola" -X POST http://localhost:8080/scripts/test.py

## Verbo POST de algún fichero con dos variables
curl -i -d "una=abcd&dos=efgh" -X POST http://localhost:8080/carpeta/fichero.tipo
# Troubleshooting
## lib_bind(): Address already in use
Este error aparece si se ejecuta el servidor de manera muy seguida; aun no se ha cerrado
el puerto 8080 para poder usarlo de nuevo. Unicamente hay que esperar un rato para volver
a ejecutarlo.
## curl: (35) Recv failure: Connection reset by peer
Este error puede darse si se escribe *https* en vez de *http* en la url *http://localhost:8080*.