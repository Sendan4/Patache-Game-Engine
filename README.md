| Español | [English](README_en.md) |
| :--: | :--: |

<p align = "center"><img draggable = false src = "data/assets/patache.svg?ref_type=heads&inline=false" width=64></p>

<h1 align = "center">Motor Patache</h1>

_Motor tonto y simple de videojuegos hecho en C++._

<p><strong>Es un motor 2D sencillo y ligero</strong> con el enfoque en el desarrollo de videojuegos utilizando C++.</p>

<p>Este motor esta hecho para ser utilizado junto con <a href = "http://www.libsdl.org/">SDL3</a>. Se abstrae la creación de la ventana, el renderizado de graficos, el sonido y la configuración en una API sencilla de utilizar.</p>

<p><strong>Es portable</strong>, funciona en varios sistemas operativos.</p>

| Windows | GNU/Linux |
| :-----: | :-----: |
|<b>x86_64</b> | <b>x86_64</b> |

<p>⚠️ <strong>Advertencia</strong>: Esta en un estado muy temprano de desarrollo y no es estable.</p>

## Gamescope
If the engine or game does not run inside gamescope try with ```--expose-wayland```.

## Dependencias usadas en este proyecto
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

## Enlaces
- [Documentación](#)
#### Espejos Oficiales
- [Github](https://github.com/Sendan4/Patache-Game-Engine)
- [Codeberg](https://codeberg.org/PatacheEngine/Patache.git)

<hr>

Patache esta bajo la licencia [MIT](LICENSE.txt)

