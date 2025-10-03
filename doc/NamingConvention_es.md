| Español | [English](NamingConvention_en.md) |
| :--: | :--: |

# Convencion de nombres
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
- Los archivos fuente usan extensiones .cpp/.hpp.

