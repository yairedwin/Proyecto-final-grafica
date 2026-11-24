// miniaudio trae todo su codigo en el .h; este define hace que se compile aqui (solo puede ir en un .cpp)
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "Audio.h"
#include <stdio.h>
#include <string.h>

#define MAX_SONIDOS 16

static ma_engine motor;				// el motor mezcla todos los sonidos y los manda a la bocina
static ma_sound sonidos[MAX_SONIDOS];
static char nombres[MAX_SONIDOS][32];
static int cuantos = 0;
static bool motorListo = false;

bool Audio::Abrir(const char* alias, const char* archivo)
{
	if (!motorListo) {
		if (ma_engine_init(NULL, &motor) != MA_SUCCESS) { printf("No se pudo iniciar el motor de audio\n"); return false; }
		motorListo = true;
	}
	if (cuantos >= MAX_SONIDOS) return false;
	// DECODE: el archivo se decodifica una vez al abrirlo y queda en memoria, asi reproducirlo no cuesta en cada cuadro
	// NO_SPATIALIZATION: el volumen segun la distancia lo calculamos nosotros en el main
	ma_result r = ma_sound_init_from_file(&motor, archivo, MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &sonidos[cuantos]);
	if (r != MA_SUCCESS) { printf("No se pudo abrir el audio %s\n", archivo); return false; }
	strncpy_s(nombres[cuantos], alias, 31);
	cuantos++;
	return true;
}

void Audio::Reproducir(const char* alias, bool enBucle)
{
	for (int i = 0; i < cuantos; i++)
		if (strcmp(nombres[i], alias) == 0) {
			ma_sound_set_looping(&sonidos[i], enBucle ? MA_TRUE : MA_FALSE);
			ma_sound_seek_to_pcm_frame(&sonidos[i], 0);	// siempre desde el principio
			ma_sound_start(&sonidos[i]);
		}
}

void Audio::Detener(const char* alias)
{
	for (int i = 0; i < cuantos; i++)
		if (strcmp(nombres[i], alias) == 0) ma_sound_stop(&sonidos[i]);
}

void Audio::Volumen(const char* alias, int volumen)
{
	if (volumen < 0) volumen = 0;
	if (volumen > 1000) volumen = 1000;
	for (int i = 0; i < cuantos; i++)
		if (strcmp(nombres[i], alias) == 0) ma_sound_set_volume(&sonidos[i], volumen / 1000.0f);
}

void Audio::CerrarTodo()
{
	for (int i = 0; i < cuantos; i++) ma_sound_uninit(&sonidos[i]);
	if (motorListo) ma_engine_uninit(&motor);
	cuantos = 0;
	motorListo = false;
}
