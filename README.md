| Español | [English](README_en.md) |
| :--: | :--: |

<p align = "center"><img draggable = false src = "data/assets/patache.svg?ref_type=heads&inline=false" width=64></p>

<h1 align = "center">Motor Patache</h1>

_Motor de videojuegos 2D tonto y simple hecho con C++. El objetivo es hacerlo rapido, ligero y portable._

<p>⚠️ Esta en un estado muy temprano de desarrollo.</p>

## Enlaces
<!-- - [Documentation](#)-->
<b>Repositorios espejo</b>

| [Github](https://github.com/Sendan4/Patache-Game-Engine) | [Codeberg](https://codeberg.org/PatacheEngine/Patache.git) |
| :--: | :--: |

## Plataformas
<b>PC</b>
|    Windows   |  GNU/Linux   |
| :----------: | :----------: |
| x86_64/AMD64 | x86_64/AMD64 |

## Requerimentos de sistema
- CPU multinucleo con extensiones de instrucciones x86_64 AVX y SSE (MMX si usas GCC).
- GPU compatible con vulkan.
- almenos 200 MiB de memoria RAM disponible.
- almenos 14 GiB de espacio libre para los binarios y el codigo fuente (incluido todas las caracteristicas de depuracion)

## Dependencias
- [SDL3](http://www.libsdl.org/)
- [Vulkan](https://www.vulkan.org/)
	- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers.git)
	- [Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader.git)
	- [Vulkan-Validation-Layers](https://github.com/KhronosGroup/Vulkan-ValidationLayers.git)
	- [Glslang](https://github.com/KhronosGroup/glslang.git)
- [fast_io](https://github.com/cppfastio/fast_io.git)
- [RapidYaml](https://github.com/biojppm/rapidyaml.git)
- [FreeType](https://gitlab.freedesktop.org/freetype/freetype.git)
- [ImGui](https://github.com/ocornut/imgui.git)
- [cglm](https://github.com/recp/cglm.git)

#### GNU/Linux
 - [Wayland](https://wayland.freedesktop.org/)
	- [Wayland Client](https://gitlab.freedesktop.org/wayland/wayland)
	- [Wayland Protocols](https://gitlab.freedesktop.org/wayland/wayland-protocols.git)

## Gamescope
Si el motor o juego no inicia dentro de gamescope, prueba con ```--expose-wayland```.

## Convencion de nombres
<b>Variables</b>
```cpp
int var = 0;
int longVar = 0;
int veryLongVarABBREVIATION = 0;
```

<b>Funciones</b>
```cpp
void Func(void);
void LongFunc(void);
void VeryLongFuncABBREVIATION(void);
``` 

<b>Macros</b>

Las abreviaciones son alineadas hacia la derecha en media de lo posible.

```cpp
#define PATACHE_MACRO
#define PATACHE_LONGMACRO
#define PATACHE_LONGMACRO_MACRO
#define PATACHE_VERYLONGMACRO_ABBREVIATION
#define PATACHE_MACRO_LONGMACRO_ABBREVIATION_MACRO
```

<b>Enumeraciones</b>
```cpp
Patache::Enum::Value;
Patache::LongEnum::LongValue;
Patache::VeryLongEnumABBREVIATION::VeryLongValueABBREVIATION;
``` 

<b>Estructuras/Clases</b>
```cpp
Patache::Class var;
Patache::LongClass longVar;
Patache::VeryLongClassABBREVIATION veryLongVarABBREVIATION;
``` 

<b>Archivos fuente</b>
- Name.cpp/.hpp
- LongName.cpp/.hpp
- ImportantName_LongName.cpp/.hpp
- ImportantName_LongName_OtherName.cpp/.hpp
- ImportantName_LongName_Abbreviation.cpp/.hpp
- Abbreviation_LongName_OtherName.cpp/.hpp

<b>Otras reglas:</b>

- No subrayados '_' en los nombres de las estructuras/clases.
- No simbolos especiales en los nombres de las variables.
- Las varibles puntero comienzan con 'p'.
```cpp
int * pPointer = nullptr;
int ** ppDoublePointer = nullptr;
``` 

- Variables referencia comienzan con 'r'.
```cpp
int & rReference = something;
``` 

- Variables estaticas comienzan con 's'.
```cpp
static int sVar = 0;
```

- No caracteres especiales en nombres de archivos, excepto por subrayados '_'.
- Los archivos fuente usan extensiones .cpp/.hpp

<hr>

Patache esta bajo la licencia [MIT](LICENSE.txt)
