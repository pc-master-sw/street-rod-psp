# Compilación y pruebas

El repositorio incluye las fuentes del adaptador, el runtime y la revisión
fijada del motor. El preparador materializa una copia para la compilación y
aplica las adaptaciones de plataforma en ese directorio.

## Generar el EBOOT

Usá Linux o WSL2 con Git, Bash, Python 3, Make y PSPDEV con PSPSDK. Cloná el
repositorio en una ruta sin espacios. Si PSPDEV está en `PATH`, basta con
ejecutar `bash scripts/build_psp.sh`. De otro modo, indicá la ruta de instalación.

```bash
PSPDEV=/ruta/a/pspdev bash scripts/build_psp.sh
```

El script genera el `EBOOT.PBP`, el PRX, sus hashes y la versión del compilador
en `build/psp/street-rod-*`. `build/psp/latest.txt` apunta a la última compilación
exitosa. El [proyecto PSPDEV](https://pspdev.github.io/) documenta el SDK.

## Probar en PC

Instalá GCC, CMake 3.16 o posterior, Python 3 y SDL2 con sus cabeceras.
Las pruebas de plataforma pueden ejecutarse sin datos DOS.

```bash
cmake -S . -B build/host -DNATIVE_SANITIZERS=ON
cmake --build build/host -j2
ctest --test-dir build/host --output-on-failure
```

Para abrir el juego en PC, indicá la carpeta de los cuatro archivos DOS y una
ruta de guardado independiente. La carpeta padre de la ruta de guardado debe existir.

```bash
./build/host/native_street_rod --game-dir /ruta/a/StreetRod --save-dir ./Save
```

## Crear un ZIP instalable

```bash
build=$(cat build/psp/latest.txt)
python3 scripts/package_psp.py "$build/EBOOT.PBP" dist/Street-Rod-PSP.zip
```

El ZIP contiene el EBOOT, manuales, licencias y un archivo con el SHA-256 del
EBOOT. El usuario coloca sus datos DOS en `STREETROD/Game` al instalarlo.
