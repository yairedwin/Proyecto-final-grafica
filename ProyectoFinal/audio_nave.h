#pragma once
// Sonido de las naves de Venom, hecho con miniaudio (miniaudio.h).
// El main solo llama a estas funciones; todo lo de miniaudio queda en audio_nave.cpp.

// true: empieza el loop del motor del Arwing (Audio/motor_arwing.wav) con un fundido corto;
// false: lo apaga con otro fundido. Se puede llamar en cada cuadro con el mismo valor,
// solo hace algo cuando cambia.
void audioNaveMotor(bool encendido);

// Motor de los Wolfen (Audio/motor_wolfen.wav). Se llama en cada cuadro con la distancia de la camara al Wolfen
// mas cercano y con su posicion a los lados de la camara (-1 izquierda, 0 enfrente, 1 derecha).
// Cerca se oye fuerte y lejos se apaga solo.
void audioNaveWolfen(float distancia, float pan);

// Libera los sonidos y el motor de audio. Llamar antes de glfwTerminate().
void audioNaveCerrar();
