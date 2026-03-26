| Español | [English](Contributing_en.md) |
| :--: | :--: |

El proposito de este documento es el ayudar a entender el estilo del codigo que debe llevar el proyecto.

# Clang Format
Este proyecto usa una de configuracion modificada del estilo [GNU](https://en.wikipedia.org/wiki/GNU_coding_standards) para clang-format que corrije los problemas de estilos. puede utilizar el target que se incluye en este proyecto de CMake para formatear el codigo explicitamente ``` --target PatacheClangFormat```. o puede utilizar la opcion ```-DFORMAT_IN_BUILD_TIME=ON``` al configurar para establecer que todo el codigo sera formateado cuando se vaya a compilar el motor.

*clang-format debe estar instalado y ser visible en el PATH.*

# General
- Si el compilador genera advertencias para tu codigo, por favor corrigelo.
- No subas una pull request sin antes formatear el codigo.
- Por favor, no codigo generado con IA.
- El uso de [nullptr](https://en.cppreference.com/w/cpp/language/nullptr.html) sobre NULL o 0 es preferido.
- No usar la STL.
- No usar auto, especifica siempre el tipo explicitamente.
- No usar ```using namespace x``` en ningun lado.

# Estilo

## Variables
Se usa el estilo [lowerCamelCase](https://wiki.c2.com/?LowerCamelCase) para las varaibles de cualquiera tipo y clase, tambien preferiblemente se utiliza la inicializacion con ```{}```,  que tambien ayuda a evitar el [narrowing](https://www.learncpp.com/cpp-tutorial/narrowing-conversions-list-initialization-and-constexpr-initializers/).
```cpp
int var{ 0 };
```

## Funciones
Se usa el estilo [UpperCamelCase](https://wiki.c2.com/?UpperCamelCase) para las funciones y el estilo clasico antes de C++11 por consistencia y simplicidad.
```cpp
void FuncMake (void);

namespace Patache
{
void FuncDelete (void);
}

void
Patache::FuncDelete ()
{
}
``` 

## Macros
Las macros deben tener el nombre del proyecto al principio.

```cpp
#define PATACHE_MACRO
```


## Enumeraciones
Tambien se utiliza [UpperCamelCase](https://wiki.c2.com/?UpperCamelCase) tanto para el nombre en clase de la enumeracion como para el valor.

```cpp
Patache::Enum::Value;
``` 

## Estructuras y clases
El nombre sigue el estilo [UpperCamelCase](https://wiki.c2.com/?UpperCamelCase) y la instancia [lowerCamelCase](https://wiki.c2.com/?LowerCamelCase).

Una recomendacion aqui!, utiliza una clase solo cuando realmente vayas a usar una caracteristica de clase, para un objeto que solo tiene miembros de datos o funciones utilice una estructura.

```cpp
namespace Patache
{
struct Example
{
  void * pData{ nullptr };
};
}

Patache::Example var{};
``` 

Evita
- subrayados '_' en los nombres de las estructuras/clases.
- simbolos especiales en los nombres de las variables.

## Tipos de variables
- Las varibles puntero comienzan con 'p'.

```cpp
int * pPointer{ nullptr };
int ** ppDoublePointer{ nullptr };
``` 

- Variables referencia comienzan con 'r'.
```cpp
int & rReference{ something };
``` 

- Variables estaticas comienzan con 's'.
```cpp
static int sVar{ 0 };
```

## Condicionales
El uso de ```{}``` es preferido por legibilidad, sin embargo si tu instruccion o funcion a llamar es muy larga, usa ```{}```.

- Si
```cpp
if (var == true)
  Patache::Func ();

if (var == true)
  {
    Patache::Func ();
  }

if (var == true)
  {
    Patache::Func ();
  }
else
  {
    Patache::AnotherFunc ();
  }

if (var == true)
  Patache::Func ();
else
  Patache::AnotherFunc ();
```

- No
```cpp
if (var == true)
  rVar = Patache::Func (arg1, &arg2, arg3, &arg4, arg5);
else
  rVar = Patache::Func (arg1, &arg2, arg3, &arg4, arg5);

if (var == true)
  {
    rVar = Patache::Func (arg1, &arg2, arg3, &arg4, arg5);
  }
else
  rVar = Patache::Func (arg1, &arg2, arg3, &arg4, arg5);
```

## Archivos fuente
Los nombres pueden estar separados por ```_``` como tambien seguir la regla [UpperCamelCase](https://wiki.c2.com/?UpperCamelCase).

- Name.cpp/.hpp
- LongName.cpp/.hpp
- ImportantName_LongName.cpp/.hpp
- ImportantName_LongName_OtherName.cpp/.hpp
- ImportantName_LongName_Abbreviation.cpp/.hpp
- Abbreviation_LongName_OtherName.cpp/.hpp

Evitar
- No caracteres especiales en nombres de archivos, excepto por subrayados '_'.
