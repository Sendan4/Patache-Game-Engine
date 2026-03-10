| Español | [English](README_en.md) |
| :--: | :--: |

<p align = "center"><img draggable = false src = "data/patache.svg?ref_type=heads&inline=false" width=64></p>

<h1 align = "center">Motor Patache</h1>

_Motor de videojuegos 2D tonto y simple hecho con C++. El objetivo es hacerlo rapido, ligero y portable._

> [!IMPORTANT]
> Esta en un estado muy temprano de desarrollo.

## Enlaces
<!-- - [Documentation](#)-->

| [Convencion de nombres](docs/NamingConvention_es.md) |
| :--: |

<b>Repositorios espejo</b>

| [Github](https://github.com/Sendan4/Patache-Game-Engine) | [Codeberg](https://codeberg.org/PatacheEngine/Patache.git) |
| :--: | :--: |

## Plataformas
<b>PC</b>
|     Windows    |    GNU/Linux   |           Unix BSD            |
| :------------: | :------------: | :---------------------------: |
| x86_64 / AMD64 <span style="color: #FF6347;">(64-bits)</span> | x86_64 / AMD64 <span style="color: #FF6347;">(64-bits)</span> | x86_64 / AMD64 <span style="color: #FF6347;">(64-bits)</span> <br> <span style="color: yellow;">_Solo GNU GCC_</span> |

## Requerimentos de sistema
- CPU multinucleo x86_64 con las extensiones AVX y SSE4.2 (MMX si usas GCC). basicamente una CPU Intel Sandy Bridge de segunda generacion o AMD Bulldozer conocido como FX.
- GPU compatible con vulkan.
- almenos 200 MiB de memoria RAM disponible.
- almenos 14 GiB de espacio libre para los binarios y el codigo fuente (incluido todas las caracteristicas de depuracion).

## Dependencias
- [SDL3](http://www.libsdl.org/)
- [Vulkan](https://www.vulkan.org/)
	- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers.git)
	- [Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader.git)
	- [Vulkan-Validation-Layers](https://github.com/KhronosGroup/Vulkan-ValidationLayers.git)
	- [Vulkan-Utility-Libraries](https://github.com/KhronosGroup/Vulkan-Utility-Libraries.git) *modificado*
    - [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git)
	- [Glslang](https://github.com/KhronosGroup/glslang.git)
- [fast_io](https://github.com/cppfastio/fast_io.git)
- [RapidYaml](https://github.com/biojppm/rapidyaml.git)
- [FreeType](https://gitlab.freedesktop.org/freetype/freetype.git)
- [ImGui](https://github.com/ocornut/imgui.git)
- [cglm](https://github.com/recp/cglm.git)

#### GNU/Linux | Unix BSD
 - [Wayland](https://wayland.freedesktop.org/)
	- [Wayland Client](https://gitlab.freedesktop.org/wayland/wayland)
	- [Wayland Protocols](https://gitlab.freedesktop.org/wayland/wayland-protocols.git)

## Gamescope
Si el motor o juego no inicia dentro de gamescope, prueba con ```--expose-wayland```.

<hr>

Patache esta bajo la licencia [GPLv3.0](LICENSE)
