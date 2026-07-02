| Español | [English](Building_en.md) |
| :--: | :--: |

# Integrar la biblioteca en tu programa

```cmake
cmake_minimum_required(VERSION 3.18)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("Test" LANGUAGES CXX)

add_subdirectory(Patache)

if (${PATACHE_USE_BIN_DIR})
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PATACHE_OUTPUT_DIR})
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PATACHE_OUTPUT_DIR})
endif()

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE PatacheEngine)
```

# Linux
Instalar los paquetes de desarrollo necesarios para [SDL](https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md)
- Algunas caracteristicas de SDL han sido desactivadas. por lo tanto no todas las dependencias listadas son necesarias

Instalar las dependencias necesarias
- GCC o Clang
- Makefiles o Ninja
- CMake
- Git
- Python3 (Necesario para configurar varias depedencias de vulkan)

### Fedora Linux
```bash
dnf install gcc gcc-c++ cmake ninja-build python3 git
```

### Debian
```bash
apt install gcc cmake ninja-build python3 git
```


### Compilar

```bash
git clone --recurse-submodules https://codeberg.org/PatacheEngine/Patache.git
cd Patache
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel $(nproc)
```

Usando Ninja-build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G Ninja
```

# Windows

## Mingw | MSYS2
Instalar las dependencias necesarias

- [MSYS2](https://www.msys2.org/) o descarga y coloca en el PATH del sistema el compilador [Mingw](https://www.winlibs.com/)

- [CMake](https://cmake.org/download/) o ```winget install Kitware.CMake```

- [Git](https://git-scm.com/install/windows) o ```winget install --id Git.Git -e --source winget```

- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) o ```winget install -e --id KhronosGroup.VulkanSDK```

### Compilar
```pswh
git clone --recurse-submodules https://codeberg.org/PatacheEngine/Patache.git
cd Patache
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G Ninja
cmake --build build --parallel $(nproc)
```

## MSVC - Visual Studio
Instalar las dependencias necesarias

- [Visual Studio Community](https://visualstudio.microsoft.com/es/vs/community/) o ```winget install -e --id Microsoft.VisualStudio.2022.Community``` y procure tener instaldo la carga de trabajo <b>Desarrollo para el escritorio con C++</b>.

- [CMake](https://cmake.org/download/) o ```winget install Kitware.CMake```

- [Git](https://git-scm.com/install/windows) o ```winget install --id Git.Git -e --source winget```

- [Python](https://www.python.org/downloads/) o ```winget install -e --id Python.Python.3.11 --scope machine``` (Necesario para configurar varias depedencias de vulkan)

### Compilar
```pswh
git clone --recurse-submodules https://codeberg.org/PatacheEngine/Patache.git
cd Patache
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 18 2026"
cmake --build build --parallel $(nproc) --config Debug
```

# FreeBSD
Escribire esto en otra ocasion

# Opciones
Estas opciones pueden ser modificadas durante la configuracion de CMake.

```-DFORMAT_IN_BUILD_TIME=ON```: Si esta activado usa clang-format con una configuracion para formatear todo el codigo del motor, util para el desarrollo del motor.

```-DUSE_VVL=ON```: Activa el uso de la capa de validacion de vulkan, util para desarrollar el renderizador del motor.

```-DBUILD_SHARED_LIBS=ON```: Si esta activado las bibliotecas se compilan como compartidas, si esta desactivado se compilan como bibliotecas estaticas.

```-DUSE_GIT=OFF```: Recoje informacion de git durante el tiempo de configuracion para incluirlo y mostrarlo en el motor.

```-DBUILD_EXAMPLES=ON```: Si es activado se construiran programas de ejemplo, esta opcion es mas util para el desarrollo del motor.

```-DUSE_LTO=ON```: Es manejado automaticamente, en muchos casos no es necesario modificar esta opcion. LTO puede provocar bugs en el compilador. LTO esta activado para los casos donde el compilador no ha producido fallos.
