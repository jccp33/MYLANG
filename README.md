# MYLANG

MYLANG es un lenguaje experimental implementado en C que ejecuta programas escritos en archivos `.ml`. El ejecutable lee el código, realiza análisis léxico y sintáctico, construye un árbol de sintaxis abstracta (AST), aplica comprobaciones semánticas y lo interpreta.

Aunque la documentación inicial lo denomina compilador, actualmente la ejecución se realiza mediante un intérprete del AST. `make` compila el intérprete de C, no convierte los programas `.ml` en ejecutables independientes.

## Estado actual

Estado documentado al 7 de octubre de 2026:

- Compilación y enlace confirmados en Windows mediante MSYS2 MINGW64, según la ejecución registrada durante el desarrollo.
- El ejecutable requiere una ruta a un archivo fuente; no incluye una consola interactiva (REPL).
- Implementación de variables, asignaciones, números almacenados como `double`, cadenas y `print` con varios argumentos.
- Operaciones aritméticas `+`, `-`, `*`, `/`, signo negativo y agrupación con paréntesis.
- Comparaciones numéricas `<`, `<=`, `>`, `>=`, `==`, `!=` y operadores lógicos `&&`, `||`. Los resultados booleanos se representan como números `0` y `1`.
- Estructuras `if`/`else`, ciclos `while`, funciones con parámetros y llamadas. Hay un ejemplo de recursión en `tests/test7.ml`.
- Comprobaciones semánticas básicas sobre declaraciones y símbolos, y comprobaciones de tipos durante la ejecución. El análisis semántico aún no recorre explícitamente los nodos `if` y `while`.
- Registro de errores léxicos, sintácticos, semánticos, de ejecución y de entrada/salida en `errors/errors.txt`.

Los siete archivos de `tests/` son ejemplos para pruebas manuales; no existe una suite automática con resultados esperados. Su presencia no garantiza que todas las combinaciones del lenguaje estén verificadas.

### Trabajo pendiente

La ruta técnica de [docs/COMPILADOR (MyLang, .ml).txt](<docs/COMPILADOR (MyLang, .ml).txt>) contempla:

1. `return` con valor en funciones.
2. Análisis estático de tipos.
3. Máquina virtual y bytecode.
4. Recolección de basura.
5. Optimización del AST.

La dependencia se mantiene en el repositorio independiente [STRUCTS](https://github.com/jccp33/STRUCTS). Más abajo se describe cómo reconstruirla para Windows y Linux. Queda pendiente validar MYLANG en Ubuntu 24.04 y registrar el commit exacto de STRUCTS utilizado en cada versión. Actualmente la memoria se administra manualmente.

## Estructura

```text
MYLANG/
├── main.c                 # Entrada del intérprete
├── Makefile               # Compilación, enlace y limpieza
├── include/               # Cabeceras de MYLANG
├── src/                   # Lexer, parser, AST, semántica, intérprete y errores
├── tests/                 # Programas de ejemplo test1.ml a test7.ml
├── docs/                  # Ruta técnica
├── errors/                # Registros de errores
├── structs/
│   ├── include/           # Cabeceras de estructuras de datos
│   ├── lib/libstructs.a   # Biblioteca de importación de Windows
│   └── bin/               # Binarios de la dependencia
├── structs.dll            # Dependencia disponible junto al ejecutable
└── build/                 # Objetos generados por make
```

## Requisitos y dependencia `structs`

Se necesitan Git para clonar, GCC para compilar, GNU Make y una biblioteca `structs` compatible con el sistema operativo y la arquitectura elegidos. El Makefile utiliza además comandos de shell como `mkdir -p` y `rm -rf`.

MYLANG utiliza tablas hash y árboles de [STRUCTS, mantenida por jccp33](https://github.com/jccp33/STRUCTS). Esta copia contiene sus cabeceras y binarios; sus fuentes se respaldan por separado en ese repositorio.

Para cualquier duda sobre la biblioteca, su API, compilación o distribución, consulta el [README de STRUCTS](https://github.com/jccp33/STRUCTS#readme), el [manual](https://github.com/jccp33/STRUCTS/blob/main/manual.md) y su [Makefile](https://github.com/jccp33/STRUCTS/blob/main/Makefile). Los pasos siguientes se basan en los objetivos `dll` y `so` de ese Makefile, revisados el 7 de octubre de 2026. `make` por sí solo genera el ejecutable de prueba de STRUCTS.

La inspección de los archivos incluidos muestra:

| Archivo | Contenido actual |
| --- | --- |
| `structs/lib/libstructs.a` | Archivo de importación con objetos PE x86-64 para Windows; no es una biblioteca estática nativa de Linux. |
| `structs/bin/structs.dll` y `structs.dll` | Biblioteca de Windows. |
| `structs/bin/libstructs.so` | Binario PE de Windows x86-64, a pesar de su extensión `.so`. No es una biblioteca ELF de Linux. |

Conserva estos binarios y las cabeceras al respaldar la versión actual. Una regla global que ignore `*.a`, `*.dll` o `*.so` dejaría el respaldo sin su dependencia precompilada. También puedes reconstruirla desde STRUCTS siguiendo los pasos siguientes. Registra `git rev-parse HEAD` dentro de STRUCTS para identificar la versión utilizada y copia siempre cabeceras y binarios de la misma compilación. La rama `main` puede cambiar; el commit de los binarios antiguos incluidos no está identificado.

### Reconstruir STRUCTS en Windows

Con las herramientas MINGW64 indicadas en la sección de Windows, ejecuta desde la raíz de MYLANG. Se clona en una carpeta hermana para conservar la carpeta local `structs/` destinada a los archivos de integración:

```bash
git clone https://github.com/jccp33/STRUCTS.git ../STRUCTS
cd ../STRUCTS
git rev-parse HEAD
make clean
make dll
cd ../MYLANG
mkdir -p structs/include structs/lib structs/bin
cp ../STRUCTS/include/*.h structs/include/
cp ../STRUCTS/lib/libstructs.a structs/lib/
cp ../STRUCTS/bin/structs.dll structs/bin/
cp ../STRUCTS/bin/structs.dll ./structs.dll
cp ../STRUCTS/LICENSE structs/LICENSE
make clean
make
```

`make dll` genera `bin/structs.dll` y `lib/libstructs.a`; este último es una **biblioteca de importación**, no una copia estática de la implementación. No necesitas ejecutar primero `make`, ya que `make dll` compila los objetos necesarios. Usa el mismo entorno y arquitectura para STRUCTS y MYLANG.

Los ejemplos suponen que la carpeta de este proyecto se llama `MYLANG` y que `../STRUCTS` aún no existe. Si ya tienes un clon de STRUCTS, utiliza su ruta y omite `git clone`. `make clean` debe ejecutarse dentro de ese clon, no dentro de la carpeta local de binarios `MYLANG/structs/`.

## Clonar desde GitHub

Después de publicar el repositorio, reemplaza `TU_USUARIO` por el propietario real:

```bash
git clone https://github.com/TU_USUARIO/MYLANG.git
cd MYLANG
```

La URL es un ejemplo; esta documentación no presupone que ya exista un repositorio remoto. En la revisión actual, la carpeta local no estaba inicializada como repositorio Git.

## Windows: recompilar con MSYS2 MINGW64

Esta es la ruta de compilación confirmada para la copia actual y sus binarios de `structs`.

1. Instala [MSYS2](https://www.msys2.org/).
2. Abre **MSYS2 MINGW64** desde el menú Inicio. El prompt debe indicar `MINGW64`.
3. Actualiza los paquetes:

   ```bash
   pacman -Syu
   ```

   Si el actualizador pide cerrar la terminal, ciérrala, vuelve a abrir MINGW64 y repite el comando.

4. Instala las herramientas:

   ```bash
   pacman -S --needed mingw-w64-x86_64-gcc make git
   ```

5. Clona el repositorio y, desde su raíz, verifica y compila:

   ```bash
   which gcc
   gcc --version
   make --version
   make clean
   make
   ```

   `which gcc` debería mostrar `/mingw64/bin/gcc`. `make` genera `mylang.exe` y los objetos en `build/`.

6. Asegura la presencia de la DLL junto al ejecutable y ejecuta un ejemplo:

   ```bash
   cp structs/bin/structs.dll ./structs.dll
   mkdir -p errors
   ./mylang.exe tests/test1.ml
   ```

La biblioteca de importación se busca mediante `-L./structs/lib -lstructs`. La DLL debe estar disponible al ejecutar; colocarla junto al ejecutable evita depender de una ruta externa para encontrarla. Si Windows informa de otras DLL ausentes, comprueba las dependencias con `objdump -p mylang.exe` y `objdump -p structs.dll` desde MINGW64.

MSYS2 distingue entre MSYS, MINGW64 y UCRT64; consulta sus [entornos](https://www.msys2.org/docs/environments/). Cambiar de entorno requiere revisar la compatibilidad de las bibliotecas. UCRT64 no se ha validado con los binarios incluidos.

### ¿Se puede usar PowerShell o CMD?

Sí, MSYS2 no es un requisito del lenguaje: el intérprete compilado puede ejecutarse desde otras consolas de Windows si están disponibles sus DLL:

```powershell
.\mylang.exe .\tests\test1.ml
```

Para **recompilar**, PowerShell o CMD por sí solos no proporcionan GCC, Make ni los comandos Unix del Makefile. Se puede configurar un toolchain MinGW-w64 compatible y un shell adecuado, o compilar directamente con GCC sin usar Make. Por ejemplo, desde PowerShell, con GCC x86-64 compatible en `PATH` y desde la raíz del proyecto:

```powershell
gcc -Wall -Wextra -Iinclude -I./structs/include src/error.c src/token.c src/lexer.c src/parser.c src/ast.c src/semantics.c src/io.c src/interpreter.c main.c -L./structs/lib -lstructs -o mylang.exe
```

Esta alternativa omite la compilación incremental; no fue validada en esta revisión. Tener un `gcc.exe` instalado no garantiza que su arquitectura coincida con las bibliotecas x86-64 incluidas. Para reproducir la compilación conocida, utiliza MINGW64.

## Ubuntu 24.04 y WSL

**Ubuntu no necesita MSYS2.** Puede compilar C directamente con GCC y Make. Sin embargo, **esta copia del proyecto no se puede enlazar de forma nativa en Ubuntu usando los binarios de `structs` incluidos**, porque son de Windows. WSL con Ubuntu tiene la misma necesidad de una biblioteca Linux para una compilación nativa.

Instala las herramientas en Ubuntu:

```bash
sudo apt update
sudo apt install build-essential git
```

Consulta también la [guía de GCC de Ubuntu](https://ubuntu.com/developers/docs/howto/gcc-setup/).

### Reconstruir STRUCTS y MYLANG en Linux

Desde la raíz de MYLANG, clona STRUCTS en una carpeta hermana y utiliza su objetivo `so`:

```bash
git clone https://github.com/jccp33/STRUCTS.git ../STRUCTS
cd ../STRUCTS
git rev-parse HEAD
make clean
make so
cd ../MYLANG
mkdir -p structs/include structs/bin
cp ../STRUCTS/include/*.h structs/include/
cp ../STRUCTS/bin/libstructs.so structs/bin/
cp ../STRUCTS/LICENSE structs/LICENSE
mkdir -p errors
make clean
make BIN_NAME=mylang LDFLAGS='-L./structs/bin -lstructs'
LD_LIBRARY_PATH="$PWD/structs/bin${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" ./mylang tests/test1.ml
```

El Makefile de STRUCTS compila los objetos con `-fPIC` y `make so` genera `bin/libstructs.so`. Se copia esa biblioteca Linux sobre el archivo Windows con el mismo nombre incluido en la copia original. El objetivo `so` no genera una biblioteca estática `.a`.

La opción `LDFLAGS='-L./structs/bin -lstructs'` hace que MYLANG enlace la biblioteca Linux recién generada en lugar de la biblioteca de importación Windows de `structs/lib/`. `LD_LIBRARY_PATH` permite encontrarla durante la ejecución; utiliza esa misma configuración al ejecutar otros ejemplos.

El Makefile de MYLANG fija por defecto el nombre `mylang.exe`, pero permite sobrescribirlo mediante `BIN_NAME=mylang`. La extensión `.exe` por sí sola no determina el formato del ejecutable.

Los objetivos de compilación se verificaron leyendo el Makefile de STRUCTS y sus cabeceras de tablas hash y árboles. No se ejecutó una compilación conjunta con ese repositorio en esta revisión; la compilación nativa de MYLANG en Ubuntu 24.04 aún debe validarse. Para dudas o cambios futuros en la biblioteca, consulta directamente [STRUCTS](https://github.com/jccp33/STRUCTS).

## Uso del lenguaje

Crea un archivo `ejemplo.ml`:

```text
var precio = 9.99;
var cantidad = 3;
print("Total:", precio * cantidad);

function contar(limite) {
    var contador = 0;
    while (contador < limite) {
        print(contador);
        contador = contador + 1;
    }
}

contar(3);

if (cantidad > 0) {
    print("Hay productos");
} else {
    print("Sin productos");
}
```

Ejecuta desde la raíz del proyecto:

```bash
./mylang.exe ejemplo.ml
```

En Linux, tras completar la preparación anterior, utiliza `./mylang ejemplo.ml`. Las declaraciones, asignaciones y llamadas terminan con `;`. Las cadenas usan comillas dobles y los bloques usan llaves. La extensión `.ml` es la convención del proyecto; el programa recibe una ruta y no comprueba la extensión.

Para recorrer los ejemplos manualmente desde Bash:

```bash
for archivo in tests/test*.ml; do
    echo "Ejecutando $archivo"
    ./mylang.exe "$archivo" || break
done
```

En Linux sustituye `./mylang.exe` por `./mylang`. Este recorrido no compara la salida con resultados esperados.

## Problemas frecuentes

| Mensaje o situación | Causa y acción |
| --- | --- |
| `make: gcc: No such file or directory`, error 127 | GCC no está disponible en el `PATH` de esa terminal. En Windows abre MINGW64 e instala el paquete correspondiente. |
| `USAGE: ./mylang.exe <<source_file>>` | Falta el argumento del archivo: ejecuta `./mylang.exe tests/test1.ml`. No escribas los símbolos `<< >>`. |
| `cannot find -lstructs` | Falta la biblioteca en `structs/lib/`, o no es compatible con el compilador. |
| `file format not recognized` o biblioteca incompatible en Ubuntu | Se está intentando enlazar un binario Windows o de otra arquitectura. Reconstruye `structs` para Linux. |
| Windows informa que falta `structs.dll` | Copia la DLL compatible junto a `mylang.exe`. |
| Error al abrir el archivo fuente | Revisa la ruta; usa comillas si contiene espacios. |
| Error del lenguaje | Consulta `errors/errors.txt`. Crea `errors/` y ejecuta desde la raíz del proyecto, ya que la ruta del registro es relativa. |

Ejecuta `make clean` al cambiar de compilador o sistema operativo para evitar reutilizar objetos incompatibles.

## Respaldo en GitHub

Incluye el código C, cabeceras, Makefile, README, ejemplos y documentación, además de las cabeceras y binarios de `structs` necesarios para esta versión. Sus fuentes están respaldadas en [jccp33/STRUCTS](https://github.com/jccp33/STRUCTS); registra también el commit utilizado para poder reproducir la dependencia a largo plazo. Al redistribuirla, incluye su archivo `LICENSE`.

Puedes crear un `.gitignore` con estas reglas para excluir los resultados de MYLANG y los registros, conservando las bibliotecas necesarias:

```gitignore
/build/
/mylang.exe
/mylang
/errors/*.txt
/structs/errors/*.txt
```

Tras crear un repositorio vacío en GitHub, desde la carpeta del proyecto:

```bash
git init
git add .
git status
git commit -m "Respaldo inicial de MYLANG"
git branch -M main
git remote add origin https://github.com/TU_USUARIO/MYLANG.git
git push -u origin main
```

Antes del commit, revisa `git status` para confirmar que se incluyen las dependencias y que no se incluyen archivos personales. El envío requiere autenticación en GitHub. Estos comandos son instrucciones; la creación y publicación del repositorio no se han ejecutado como parte de esta documentación.

No se encontró un archivo de licencia de MYLANG en esta copia; queda pendiente definirla. STRUCTS se distribuye bajo [licencia MIT](https://github.com/jccp33/STRUCTS/blob/main/LICENSE). Conserva su aviso de copyright y licencia al redistribuir sus archivos; los pasos de integración anteriores copian esa licencia a `structs/LICENSE`.
