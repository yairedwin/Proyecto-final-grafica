# Proyecto final · Sistema Tres Mundos

Computación Gráfica e Interacción Humano-Computadora, semestre 2027-1.
Escena en OpenGL 4.3 (C++, GLFW, GLEW, GLM, Assimp) con tres planetas conectados por una pista circular y ciclo día/noche.

| Nº de cuenta | Universo · planeta |
|---|---|
| 318337872 | Star Fox 64 · Venom |
| 317136421 | Halo · Reach |
| 419049898 | Ratchet & Clank · Mundo Forja |

## Cómo ejecutarlo

Todo lo necesario está en `ProyectoFinal/`: código, modelos, texturas, shaders, los archivos de keyframes y las bibliotecas.

- **Sin compilar:** abrir `ProyectoFinal/ProyectoFinalLab.exe` (hace falta el Visual C++ Redistributable x64 si Windows pide `MSVCP140.dll` o `VCRUNTIME140.dll`: https://aka.ms/vc14/vc_redist.x64.exe). Debe ejecutarse desde esa misma carpeta, junto a sus `.dll`, `Models`, `Textures` y `shaders`.
- **Compilando:** abrir `ProyectoFinal/ProyectoFinalLab.vcxproj` en Visual Studio y compilar en Release x64 (conjunto de herramientas v145; en Visual Studio 2022 se cambia a v143). Para ejecutar desde Visual Studio, el directorio de trabajo debe ser la carpeta `ProyectoFinal`.

El programa principal es `proyecto_final.cpp`, dividido en secciones por integrante. Los archivos `keyframes_cohete_reach.txt`, `keyframes_cohete_venom.txt` y `keyframescohete.txt` se leen al iniciar y deben quedar junto al ejecutable.

## Versiones

| Etiqueta | Qué es |
|---|---|
| `v1` | Primera versión del proyecto, lo que había en el repositorio antes de la versión del laboratorio. |
| `v2` | Planetas de radio 80, cohete Saturn V con humo, llama y separación de etapas, y Reach integrado. |
| `v3` | Versión final del laboratorio: avatar jerárquico con caminata, Venom completo (domo, Venomianos, Wolfen y Arwing), cohetes de los tres planetas por keyframes (Forja, Reach y Venom), Sabre en la pista, todas las teclas guardadas en `Window` y modelos centrados en el origen. |

Cada integrante trabaja en su rama (`419049898`, `317136421`, `318337872`) y lo terminado se junta en `main`.

## Modelos y texturas

Los modelos y las texturas van incluidos en este repositorio. Los modelos de Ratchet & Clank y de Halo provienen de los juegos (ver `ProyectoFinal/Models/halo/FUENTES.txt`) y se usan con fines académicos. Una copia de respaldo de los modelos está en Google Drive:

**Enlace a Drive:** https://drive.google.com/drive/folders/1Awm0dTTBCjZd4GlclVRkpVnjBDm1YaTk?usp=sharing
