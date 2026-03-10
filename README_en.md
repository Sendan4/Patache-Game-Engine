| [Español](README.md) | English |
| :--: | :--: |

<p align = "center"><img draggable = false src = "data/patache.svg?ref_type=heads&inline=false" width=64></p>

<h1 align = "center">Patache Engine</h1>

_Dumb and simple 2D game engine made in C++. The goal is to do fast, lightweight and portable._

> [!IMPORTANT]
> It is at a very early stage of development.

## Links
<!-- - [Documentation](#)-->

| [Naming convention](docs/NamingConvention_es.md) |
| :--: |

<b>Mirror repositories</b>

| [Github](https://github.com/Sendan4/Patache-Game-Engine) | [Codeberg](https://codeberg.org/PatacheEngine/Patache.git) |
| :--: | :--: |

## Platforms
<b>PC</b>
|     Windows    |   GNU/Linux    |            Unix BSD           |
| :------------: | :------------: | :---------------------------: |
| x86_64 / AMD64 <span style="color: #FF6347;">(64-bits)</span> | x86_64 / AMD64 <span style="color: #FF6347;">(64-bits)</span> | x86_64 / AMD64 <span style="color: #FF6347;">(64-bits)</span> <br> <span style="color: yellow;">_Only GNU GCC_</span> |

## System requirements
- Multi-core CPU with instructions AVX y SSE4.2 (MMX if you use GCC). basically a second-generation Intel Sandy Bridge CPU or AMD Bulldozer known as FX.
- Vulkan compatible GPU.
- At least 4 GB of RAM.
- At least 14 GB of free space for source code and binaries (includes debug with all features).

## Dependencies used
- [SDL3](http://www.libsdl.org/)
- [Vulkan](https://www.vulkan.org/)
	- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers.git)
	- [Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader.git)
	- [Vulkan-Validation-Layers](https://github.com/KhronosGroup/Vulkan-ValidationLayers.git)
	- [Vulkan-Utility-Libraries](https://github.com/KhronosGroup/Vulkan-Utility-Libraries.git) *modified*
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
If the engine or game does not run inside gamescope, try with ```--expose-wayland```.

<hr>

Patache is under the [GPLv3.0](LICENSE) license
