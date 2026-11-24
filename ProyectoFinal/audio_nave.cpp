// Sonido de las naves de Venom con miniaudio: motor del Arwing (mientras se pilota) y motor de los Wolfen (por cercania).
//
// MINIAUDIO_IMPLEMENTATION debe estar en UN SOLO .cpp de todo el proyecto.
// - Si este es el unico archivo que lo define (caso de este proyecto): no hay que hacer nada.
// - Si el proyecto final ya lo define en otro .cpp (por ejemplo en Audio.cpp): agregar
//   AUDIO_NAVE_SIN_IMPLEMENTACION en Propiedades > C/C++ > Preprocesador (el miniaudio.h es el mismo).
#ifndef AUDIO_NAVE_SIN_IMPLEMENTACION
#define MINIAUDIO_IMPLEMENTATION
#endif
#include "miniaudio.h"
#include <stdio.h>
#include <math.h>
#include "audio_nave.h"

// las rutas son relativas a la carpeta del proyecto, igual que las de Models y Textures
static const char* ARCHIVO_MOTOR = "Audio/motor_arwing.wav";
static const char* ARCHIVO_WOLFEN = "Audio/motor_wolfen.wav";
static const float VOLUMEN_MOTOR = 0.8f;		// Arwing, de 0 a 1
static const float VOLUMEN_WOLFEN = 0.7f;		// volumen de un Wolfen pegado a la camara
static const float WOLFEN_DIST_CERCA = 8.0f;	// a esta distancia o menos suena al maximo
static const float WOLFEN_DIST_LEJOS = 75.0f;	// a esta distancia o mas ya no se oye
static const ma_uint64 FADE_ENTRA_MS = 250;
static const ma_uint64 FADE_SALE_MS = 400;

static ma_engine motor;
static bool motorIniciado = false;	// el ma_engine ya existe

static ma_sound sonidoMotor;
static bool sonidoCargado = false;	// el wav del Arwing ya se abrio
static bool motorSonando = false;	// estado pedido por el main
static bool motorFallo = false;		// si algo fallo, no se reintenta ni se repite el aviso hasta que se vuelva a pilotar

static ma_sound sonidoWolfen;
static bool wolfenCargado = false;
static bool wolfenFallo = false;	// si no se pudo abrir, no se reintenta en cada cuadro
static float wolfenVolumen = 0.0f;

// crea el motor de audio la primera vez que algo lo pide (el main no tiene que iniciar nada)
static bool iniciarEngine()
{
	if (motorIniciado) return true;
	if (ma_engine_init(NULL, &motor) != MA_SUCCESS) {
		printf("No se pudo iniciar el motor de audio (miniaudio)\n");
		return false;
	}
	motorIniciado = true;
	return true;
}

// abre un wav en loop y sin espacializacion (el volumen y el paneo los calcula el main)
static bool abrirLoop(const char* archivo, ma_sound* s)
{
	if (!iniciarEngine()) return false;
	// DECODE: el wav es corto, se decodifica una vez y se queda en memoria (el loop no se corta)
	// NO_SPATIALIZATION: no usa la posicion 3D de miniaudio; la distancia la calcula el main
	ma_uint32 flags = MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION;
	if (ma_sound_init_from_file(&motor, archivo, flags, NULL, NULL, s) != MA_SUCCESS) {
		printf("No se pudo abrir %s (revisa que la carpeta Audio este junto al proyecto)\n", archivo);
		return false;
	}
	ma_sound_set_looping(s, MA_TRUE);
	return true;
}

void audioNaveMotor(bool encendido)
{
	if (!encendido) motorFallo = false;	// al soltar la nave se permite reintentar la proxima vez
	if (motorFallo || encendido == motorSonando) return;	// solo actua cuando cambia (el main lo llama en cada cuadro)

	if (encendido) {
		if (!sonidoCargado) {
			if (!abrirLoop(ARCHIVO_MOTOR, &sonidoMotor)) { motorFallo = true; return; }
			ma_sound_set_volume(&sonidoMotor, VOLUMEN_MOTOR);
			sonidoCargado = true;
		}
		ma_sound_reset_stop_time_and_fade(&sonidoMotor);	// cancela el apagado programado si venia saliendo
		ma_sound_set_fade_in_milliseconds(&sonidoMotor, 0.0f, 1.0f, FADE_ENTRA_MS);
		ma_sound_seek_to_pcm_frame(&sonidoMotor, 0);
		ma_sound_start(&sonidoMotor);
		motorSonando = true;
	}
	else if (sonidoCargado) {
		// baja el volumen y se detiene solo al terminar el fundido
		ma_sound_stop_with_fade_in_milliseconds(&sonidoMotor, FADE_SALE_MS);
		motorSonando = false;
	}
	else {
		motorSonando = false;
	}
}

void audioNaveWolfen(float distancia, float pan)
{
	if (wolfenFallo) return;
	if (!wolfenCargado) {
		if (distancia >= WOLFEN_DIST_LEJOS) return;	// mientras no haya ninguno cerca no se abre nada
		if (!abrirLoop(ARCHIVO_WOLFEN, &sonidoWolfen)) { wolfenFallo = true; return; }
		ma_sound_set_volume(&sonidoWolfen, 0.0f);
		ma_sound_start(&sonidoWolfen);	// queda sonando en loop y su volumen lo sube o baja la distancia
		wolfenCargado = true;
	}
	// 1 cerca, 0 lejos, con curva suave (smoothstep)
	float t = (WOLFEN_DIST_LEJOS - distancia) / (WOLFEN_DIST_LEJOS - WOLFEN_DIST_CERCA);
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;
	float objetivo = VOLUMEN_WOLFEN * t * t * (3.0f - 2.0f * t);
	wolfenVolumen += (objetivo - wolfenVolumen) * 0.2f;	// se acerca poco a poco al objetivo para que no haya saltos
	if (pan < -1.0f) pan = -1.0f;
	if (pan > 1.0f) pan = 1.0f;
	ma_sound_set_volume(&sonidoWolfen, wolfenVolumen);
	ma_sound_set_pan(&sonidoWolfen, 0.7f * pan);	// no se manda todo a un solo lado para que no moleste con audifonos
}

void audioNaveCerrar()
{
	if (sonidoCargado) { ma_sound_uninit(&sonidoMotor); sonidoCargado = false; }
	if (wolfenCargado) { ma_sound_uninit(&sonidoWolfen); wolfenCargado = false; }
	if (motorIniciado) { ma_engine_uninit(&motor); motorIniciado = false; }
	motorSonando = false;
}
