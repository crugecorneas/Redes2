#!/bin/bash

# Verifica si se pasó el mensaje de commit como argumento
if [ -z "$1" ]; then
    echo "Por favor, proporciona un mensaje de commit."
    exit 1
fi

# Agrega todos los cambios
git add .

# Realiza el commit con el mensaje proporcionado
git commit -m "$1"

# Realiza el push al repositorio remoto
git push

# Mensaje de éxito
echo "Cambios enviados con éxito con el mensaje: $1"
