| [Español](NamingConvention_es.md) | English |
| :--: | :--: |

# Naming convention
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

