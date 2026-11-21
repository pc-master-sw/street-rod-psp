# Validación

| Nivel | Evidencia |
|---|---|
| T0 · PC | Compilación con ASan y UBSan y cuatro pruebas CTest de plataforma aprobadas en el repositorio independiente. El final del Rey pasó como escena aislada en la validación previa. |
| T1 · PPSSPP 1.20.4 | Arranque, licencia, compra de un auto, garaje, salida a Bob’s Drive-In, carrera de ruta, carrera de aceleración, audio PCM, guardado, carga y salida del EBOOT. |
| T2 · PSP real | El usuario confirmó el funcionamiento del EBOOT en su PSP el 19 de noviembre de 2026. El modelo, el firmware y el recorrido exacto de esa prueba quedaron sin registrar. |

En una instalación limpia en PPSSPP, el EBOOT extraído del ZIP de lanzamiento
arrancó y abrió los cuatro archivos DOS en `Game`.

La compilación del repositorio independiente con `psp-gcc` 15.2.0 produjo el
mismo EBOOT que se usó en las pruebas de integración y en PSP física. Su
SHA-256 es `5433b2113bda8f9e9ecb947ea67e126a6d586980f003b283ca97bffa67a0e679`.
Queda pendiente registrar una campaña continua hasta vencer al Rey.
