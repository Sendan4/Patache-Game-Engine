| [Español](Building_es.md) | English |
| :--: | :--: |

# Integrate the library into your program

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
Install the necessary development packages to [SDL](https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md)
- Some SDL features have been disabled; therefore, not all listed dependencies are required.

Install the necessary dependencies
- GCC o Clang
- Makefiles o Ninja
- CMake
- Git
- Python3 (Necessary to configure various Vulkan dependencies)

### Fedora Linux
```bash
dnf install gcc gcc-c++ cmake ninja-build python3 git
```

### Debian
```bash
apt install gcc cmake ninja-build python3 git
```

### Build

```bash
git clone --recurse-submodules https://codeberg.org/PatacheEngine/Patache.git
cd Patache
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel $(nproc)
```

Using Ninja-build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G Ninja
```

# Windows

## Mingw | MSYS2
Install the necessary dependencies

- [MSYS2](https://www.msys2.org/) or download and place the compiler in the system PATH [Mingw](https://www.winlibs.com/)

- [CMake](https://cmake.org/download/) or ```winget install Kitware.CMake```

- [Git](https://git-scm.com/install/windows) or ```winget install --id Git.Git -e --source winget```

- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) or ```winget install -e --id KhronosGroup.VulkanSDK```

### Build
```pswh
git clone --recurse-submodules https://codeberg.org/PatacheEngine/Patache.git
cd Patache
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G Ninja
cmake --build build --parallel $(nproc)
```

## MSVC - Visual Studio
Install the necessary dependencies

- [Visual Studio Community](https://visualstudio.microsoft.com/es/vs/community/) or ```winget install -e --id Microsoft.VisualStudio.2022.Community``` and make sure you have the workload installed <b>Desktop development with C++</b>.

- [CMake](https://cmake.org/download/) or ```winget install Kitware.CMake```

- [Git](https://git-scm.com/install/windows) or ```winget install --id Git.Git -e --source winget```

- [Python](https://www.python.org/downloads/) or ```winget install -e --id Python.Python.3.11 --scope machine``` (Necessary to configure various Vulkan dependencies)

### Build
```pswh
git clone --recurse-submodules https://codeberg.org/PatacheEngine/Patache.git
cd Patache
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 18 2026"
cmake --build build --parallel $(nproc) --config Debug
```

# FreeBSD
not yet

# Opciones
These options can be modified during CMake configuration.

```-DFORMAT_IN_BUILD_TIME=ON```: If enabled, use clang-format with a configuration to format all engine code, useful for engine development.

```-DUSE_VVL=ON```: Enables the use of the Vulkan validation layer, useful for developing the engine's renderer.

```-DBUILD_SHARED_LIBS=ON```: If enabled, the libraries are compiled as shared libraries; if disabled, they are compiled as static libraries.

```-DUSE_GIT=OFF```: It gathers information from Git during setup time to include and display it in the engine.

```-DBUILD_EXAMPLES=ON```: If activated, example programs are built; this option is more useful for engine development.

```-DUSE_LTO=ON```: It is handled automatically; in many cases, it is not necessary to modify this option. LTO can cause compiler bugs. LTO is enabled for cases where the compiler has not produced errors.
