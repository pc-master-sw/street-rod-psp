# Manual de instalación

## Requisitos

- Una Sony PSP preparada para ejecutar homebrew y una Memory Stick con espacio libre.
- `Street-Rod-PSP.zip` de la sección Releases del repositorio.
- Los cuatro archivos de una copia DOS de **Street Rod 1**. El sitio de Street Rod
  ofrece [descargas del juego original](https://www.streetrodonline.com/downloads/).

## Instalación en PSP

1. Extraé el ZIP en tu PC.
2. Copiá la carpeta `STREETROD` a `PSP/GAME` de la Memory Stick.
3. Copiá `SR.EXE`, `LIB1`, `LIB2` y `HOT_DATA` a `PSP/GAME/STREETROD/Game`.
   Usá esos nombres. `LIB1`, `LIB2` y `HOT_DATA` van sin extensión.
4. Desconectá la PSP y abrí **Street Rod** desde **Juego → Memory Stick**.

```text
PSP/GAME/STREETROD/
├── EBOOT.PBP
├── Game/
│   ├── SR.EXE
│   ├── LIB1
│   ├── LIB2
│   └── HOT_DATA
└── Save/                  creada al iniciar
```

## Actualización y partidas

Hacé una copia de `Save` antes de actualizar. Con el juego cerrado, reemplazá
`EBOOT.PBP` por el de la nueva versión. La carpeta `Game` puede seguir en su sitio.
Para importar una partida DOS, copiá el archivo `HOTROD1.SAV` a `HOTROD15.SAV`
que corresponda a tu ranura dentro de `Save`. Cargala desde el menú del juego.

## Si el juego no arranca

- **`MISSING GAME/...`**. Revisá el nombre y la ubicación del archivo que indica la pantalla.
- **`COULD NOT CREATE THE SAVE FOLDER`**. Revisá que haya espacio y que la Memory Stick permita escribir.
- **El icono no aparece en Juego.** Comprobá la ruta `PSP/GAME/STREETROD/EBOOT.PBP` y que la PSP pueda ejecutar homebrew.

En PPSSPP podés usar la misma estructura dentro de la Memory Stick emulada y
abrir `EBOOT.PBP`. Los mandos de la tabla del [manual de uso](USO.md) son los de PSP.
