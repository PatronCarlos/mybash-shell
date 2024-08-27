# Sistemas Operativos 2024 - Laboratorio 1: MyBash

## Introducción
Este proyecto tiene como objetivo la implementación de un intérprete de línea de comandos, `mybash`, inspirado en `bash`. A través de este laboratorio, se busca que los estudiantes comprendan y apliquen los mecanismos de concurrencia y comunicación en UNIX, además de seguir buenas prácticas de programación.

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
