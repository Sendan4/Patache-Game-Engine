| [Español](README.md) | English |
| :--: | :--: |

<p align = "center"><img draggable = false src = "data/assets/patache.svg?ref_type=heads&inline=false" width=64></p>

<h1 align = "center">Patache Engine</h1>

_Dumb and simple 2D game engine made in C++. The goal is to do fast, lightweight and portable._

<p>⚠️ It is at a very early stage of development.</p>

## Links
<!-- - [Documentation](#)-->
<b>Mirror repositories</b>

| [Github](https://github.com/Sendan4/Patache-Game-Engine) | [Codeberg](https://codeberg.org/PatacheEngine/Patache.git) |
| :--: | :--: |

## Platforms
<b>PC</b>
|    Windows   |  GNU/Linux   |
| :----------: | :----------: |
| x86_64/AMD64 | x86_64/AMD64 |

## System requirements
- Multi-core CPU with instructions AVX y SSE (MMX if you use GCC)
- Vulkan compatible GPU
- At least 4 GB of RAM
- At least 14 GB of free space for source code and binaries (includes debug with all features)

## Dependencies used
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
If the engine or game does not run inside gamescope, try with ```--expose-wayland```.

## Naming convention
<b>Variables</b>
```cpp
int var = 0;
int longVar = 0;
int veryLongVarABBREVIATION = 0;
```

<b>Functions</b>
```cpp
void Func(void);
void LongFunc(void);
void VeryLongFuncABBREVIATION(void);
``` 

<b>Macros</b>
Abbreviation is more right aligned
```cpp
#define PATACHE_MACRO
#define PATACHE_LONGMACRO
#define PATACHE_LONGMACRO_MACRO
#define PATACHE_VERYLONGMACRO_ABBREVIATION
#define PATACHE_MACRO_LONGMACRO_ABBREVIATION_MACRO
```

<b>Enumerations</b>
```cpp
Patache::Enum::Value;
Patache::LongEnum::LongValue;
Patache::VeryLongEnumABBREVIATION::VeryLongValueABBREVIATION;
``` 

<b>Struct/Class</b>
```cpp
Patache::Class var;
Patache::LongClass longVar;
Patache::VeryLongClassABBREVIATION veryLongVarABBREVIATION;
``` 

<b>Source file</b>
- Name.cpp/.hpp
- LongName.cpp/.hpp
- ImportantName_LongName.cpp/.hpp
- ImportantName_LongName_OtherName.cpp/.hpp
- ImportantName_LongName_Abbreviation.cpp/.hpp
- Abbreviation_LongName_OtherName.cpp/.hpp

<b>Other rules:</b>

- No underscores '_' in the struct/class name.
- No special symbols in variable names.
- Pointer variables begin with 'p'.
```cpp
int * pPointer = nullptr;
int ** ppDoublePointer = nullptr;
``` 

- Reference variables begin with 'r'.
```cpp
int & rReference = something;
``` 

- Static variables begin with 's'.
```cpp
static int sVar = 0;
```

- No special characters in file names, except for underscores '_'.
- The source file name must end with the extension .cpp/.hpp.

<hr>

Patache is under the [MIT](LICENSE.txt) license
