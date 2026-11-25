# Proyecto final · Sistema Tres Mundos

Computación Gráfica e Interacción Humano-Computadora, semestre 2027-1.
Escenario interactivo en OpenGL 4.3 (C++, GLFW, GLEW, GLM, Assimp y miniaudio) sobre viajes espaciales y evolución de especies intergalácticas: tres planetas de universos distintos, unidos por una pista de carreras elíptica, una estación espacial y naves que viajan entre ellos, con un avatar que camina por cada mundo, especies en tres etapas de su evolución, animaciones por keyframes, audio (soundtrack, ambientes y efectos) y ciclo de día y noche.

> **Versión final: `v5`** (rama `version-5-final`). Es la que se entrega; el detalle de cada versión está en la sección [Versiones](#versiones).

| Nº de cuenta | Universo · planeta |
|---|---|
| 318337872 | Star Fox 64 · Venom |
| 317136421 | Halo · Reach |
| 419049898 | Ratchet & Clank · Mundo Forja |

## Cómo ejecutarlo

Todo lo necesario está en `ProyectoFinal/`: código, modelos, texturas, shaders, audio, los archivos de keyframes y las bibliotecas.

- **Sin compilar:** abrir `ProyectoFinal/ProyectoFinalTeoria.exe` (hace falta el Visual C++ Redistributable x64 si Windows pide `MSVCP140.dll` o `VCRUNTIME140.dll`: https://aka.ms/vc14/vc_redist.x64.exe). Debe ejecutarse desde esa misma carpeta, junto a sus `.dll`, `Models`, `Textures`, `shaders` y `audio`.
- **Compilando:** abrir `ProyectoFinal/ProyectoFinalTeoria.vcxproj` en Visual Studio y compilar en Release x64 (conjunto de herramientas v145; en Visual Studio 2022 se cambia a v143). Para ejecutar desde Visual Studio, el directorio de trabajo debe ser la carpeta `ProyectoFinal`.
- **Audio:** usa la biblioteca miniaudio (`miniaudio.h`, un solo archivo, ya incluido) a través de la clase `Audio`; no hay que instalar ni enlazar nada más.

El programa principal es `proyecto_final.cpp`, dividido en secciones por integrante. Los archivos `keyframes_cohete_reach.txt`, `keyframes_cohete_venom.txt` y `keyframescohete.txt` se leen al iniciar y deben quedar junto al ejecutable.

## Versiones

| Etiqueta | Qué es |
|---|---|
| `v1` | Primera versión del proyecto, lo que había en el repositorio antes de la versión del laboratorio. |
| `v2` | Planetas de radio 80, cohete Saturn V con humo, llama y separación de etapas, y Reach integrado. |
| `v3` | Versión final del laboratorio: avatar jerárquico con caminata, Venom completo (domo, Venomianos, Wolfen y Arwing), cohetes de los tres planetas por keyframes (Forja, Reach y Venom), Sabre en la pista, todas las teclas guardadas en `Window` y modelos centrados en el origen. |
| `v4` | Versión de teoría (rama `version-4-teoria`): audio con miniaudio (soundtrack, ambientes por planeta y efectos con condición, incluidos los de Reach y el motor del Arwing), evolución de especies en 3 etapas con su nombre flotante (Protopets, Moa, Venomianos, ballenas espaciales y aves de fuego), keyframes nuevos (asteroide, estrella fugaz y satélite que sale de la estación), Mundo Forja repartido, plasma de las naves de Venom y texturas en todos los elementos. |
| `v5` | **Versión final del proyecto** (rama `version-5-final`), la que se entrega en teoría: todo lo de la v4 más tres estrellas fugaces de colores por keyframes, en Venom los personajes de Star Fox que caminan (Fox, Peppy, Wolf y Pigma) y una zona de lava con dos luces que titilan, sonido de las naves de Venom con fundidos y paneo (motor de los Wolfen con audio CC0), modelos de los personajes centrados en el origen y cada luz puntual con su propia atenuación. |

Cada integrante trabaja en su rama (`419049898`, `317136421`, `318337872`) y lo terminado se junta en `main`.

## Modelos y texturas

Los modelos y las texturas van incluidos en este repositorio. Los modelos de Ratchet & Clank y de Halo provienen de los juegos (ver `ProyectoFinal/Models/halo/FUENTES.txt`) y se usan con fines académicos. Una copia de respaldo de los modelos está en Google Drive:

**Enlace a Drive:** https://drive.google.com/drive/folders/1Awm0dTTBCjZd4GlclVRkpVnjBDm1YaTk?usp=sharing
