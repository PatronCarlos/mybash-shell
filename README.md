# Sistemas Operativos 2024 - Laboratorio 1: MyBash

## Introducción
Este proyecto tiene como objetivo la implementación de un intérprete de línea de comandos, `mybash`, inspirado en `bash`, pudiendo asi ejecutar en el mismo algunos comandos basicos. A través de este laboratorio, se busca que los estudiantes comprendan y apliquen los mecanismos de concurrencia y comunicación en UNIX, además de seguir buenas prácticas de programación.



## Funcionalidades Generales
- Ejecución de comandos en **foreground** y **background**.
- Redirección de **entrada** y **salida estándar**.
- Soporte para **pipes** entre comandos.
- Implementación de comandos internos: `cd`, `help`, y `exit`.
- Robustez ante entradas incompletas o inválidas.

## Modularización
El proyecto se organiza en los siguientes módulos:
1. **mybash**: Módulo principal.
2. **command**: Definición de TADs para comandos.
3. **parsing**: Procesamiento de la entrada del usuario.
4. **parser**: Implementación del TAD `parser`.
5. **execute**: Ejecución de comandos y administración de llamadas al sistema.
6. **builtin**: Implementación de comandos internos.

## Requisitos
- Implementar y probar los módulos según las especificaciones provistas.
- Usar la biblioteca **GLib** para manejo de listas y secuencias.
- Seguir buenas prácticas de programación: modularización, uso de TADs opacos, manejo de memoria y estilo de código.

## Compilación y Pruebas
- Compilar y ejecutar las pruebas unitarias con:
  ```bash
  $ make test

## Metodo de trabajo
Para el desarrollo de nuestro bash, utilizamos un enfoque de trabajo colaborativo y asincrónico, para eso se uso herramientas de gestión y control de versiones como:

- Bitbucket: Para el control de versiones, se creó un repositorio privado en Bitbucket. Cada integrante trabajó en ramas independientes, asegurando la modularidad y facilitando la integración de cambios mediante pull requests. Esto permitió que el equipo pudiera trabajar simultáneamente sin conflictos de código.

- Jira: La planificación y asignación de tareas se realizó a través de Jira, donde organizamos el proyecto en sprints. Definimos tareas claras para cada integrante, lo que nos permitió distribuir el desarrollo de los módulos de manera eficiente.

- División de Módulos: El equipo dividió el trabajo asignando uno o varios modulos a los integrantes del grupo, de igual manera cada uno pudo aportar ideas y soluciones a los demas cuando se presentaron problemas/errores.

## Dependencias
Para la implementación de mybash, se utilizaron las siguientes bibliotecas:

- GLib: Utilizada para el manejo de colas. Esta biblioteca fue clave para la implementación de los TADs (Tipos Abstractos de Datos) en nuestro proyecto.

- Check: Herramienta utilizada para ejecutar las pruebas unitarias provistas por la cátedra, permitiendo validar el correcto funcionamiento de los distintos módulos implementados..

Antes de compilar el proyecto, asegúrate de tener instaladas estas dependencias:

para Glib: 
$ sudo apt-get install libglib2.0-dev
para Check:
$ sudo apt-get install check

para poder ejecutar nuestro bash, basta con utilizar el comando ./mybash en la terminal (siempre que estemos posicionados en la carpeta donde hicimos el make)



## Observaciones personales
- Para algunos miembros del grupo, tuvimos que familiarizarnos con la herramientas como bitbucket y jira, ya sea por falta de experiencia o bien por equivocarnos en algun comando "git", cabe destacar que una vez que se comprendío el uso de las mismas, se le dio gran valor ya que permitío trabjar de una manera mas comoda y organizada.

- Pair Programming: Durante el desarrollo de este proyecto, trabajamos la metodología de pair programming (no estamos acostumbrados a trabajar de esta manera). A través de esta práctica, un miembro codificaba mientras el otro revisaba y sugería mejoras en tiempo real. Esto nos facilitó la detección temprana de errores y promovió un aprendizaje más colaborativo. Además, mejoró nuestra comunicación y nos permitió resolver problemas más rápidamente.

