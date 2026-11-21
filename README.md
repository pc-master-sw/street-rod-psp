# Street Rod para PSP

Port nativo de **Street Rod 1** para Sony PSP. El juego se ejecuta desde un
`EBOOT.PBP` compilado para Allegrex (MIPS). La imagen VGA de 320 × 200 se
presenta en la pantalla de PSP y el audio del altavoz de PC se reproduce a
través de la salida PCM de la consola.

El port requiere `SR.EXE`, `LIB1`, `LIB2` y `HOT_DATA` del juego DOS. El motor
C lee las tablas y recursos de tu copia durante la ejecución.

## Empezar a jugar

1. Descargá `Street-Rod-PSP.zip` desde [Releases](https://github.com/pc-master-sw/street-rod-psp/releases).
2. Seguí el [manual de instalación](docs/INSTALACION.md) para copiar el EBOOT y tus datos.
3. Consultá el [manual de uso](docs/USO.md) para iniciar una partida, correr y guardar.

## Desarrollo

La [guía de compilación](docs/COMPILACION.md) explica cómo generar el EBOOT
con PSPSDK, probar el port en PC y armar un ZIP. Los resultados y el alcance
de las pruebas están en [Validación](docs/VALIDACION.md). El
[registro de cambios](CHANGELOG.md) documenta las versiones publicadas.

## Créditos y licencias

- **Motor C.** [Street Rod SDL3](https://github.com/kylofon/street-rod-sdl3) de
  Krzysztof Kania (kylofon). Se conserva el [aviso MIT](vendor/street-rod-sdl3/LICENSE)
  y la revisión de origen está fijada en [dependencies.lock.json](dependencies.lock.json).
- **Adaptador y runtime de plataforma.** DOS2PSP Native, bajo [GPL-2.0 o posterior](LICENSE).
- **Juego original.** California Dreams / Logical Design Works. Cada jugador
  aporta sus propios archivos DOS.
