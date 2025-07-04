| [Español](README.md) | English |
| :--: | :--: |

<p align = "center"><img draggable = false src = "data/assets/patache.svg?ref_type=heads&inline=false" width=64></p>

<h1 align = "center">Patache Engine</h1>

_Dumb and simple game engine made in C++._

<p><strong>Is a 2D engine, simple and lightweight</strong> with a focus on game development using C++.</p>

<p>This engine is intended to be used in conjunction with <a href = "http://www.libsdl.org/">SDL3</a>. Abstracts window creation, graphics rendering, sound and configuration into one easy-to-use API.</p>

<p><strong>It is portable</strong>, works on various operating systems.</p>

| Windows | GNU/Linux |
| :-----: | :-----: |
|<b>x86_64</b> | <b>x86_64</b> |

<p>⚠️ <strong>WARNING</strong>: It is at a very early stage of development and is not stable.</p>

## Gamescope
If the engine or game does not run inside gamescope try with ```--expose-wayland```.

## Dependencies used in this project
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

## Links
- [Documentation](#)
#### Official Mirrors
- [Github](https://github.com/Sendan4/Patache-Game-Engine)
- [Codeberg](https://codeberg.org/PatacheEngine/Patache.git)

<hr>

Patache is under the [MIT](LICENSE.txt) license
