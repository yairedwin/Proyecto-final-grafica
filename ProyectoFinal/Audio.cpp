#include "Audio.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>
#pragma comment(lib, "winmm.lib")

// ultimo volumen mandado a cada alias, para no mandar el mismo comando en cada cuadro
static char nombres[16][32];
static int volumenes[16];
static int cuantos = 0;

bool Audio::Abrir(const char* alias, const char* archivo)
{
	char cmd[512];
	// "mpegvideo" (DirectShow) deja reproducir varios sonidos a la vez, repetirlos y cambiarles el volumen
	snprintf(cmd, sizeof(cmd), "open \"%s\" type mpegvideo alias %s", archivo, alias);
	MCIERROR error = mciSendStringA(cmd, NULL, 0, NULL);
	if (error != 0) { printf("No se pudo abrir el audio %s\n", archivo); return false; }
	if (cuantos < 16) { strncpy_s(nombres[cuantos], alias, 31); volumenes[cuantos] = -1; cuantos++; }
	return true;
}

void Audio::Reproducir(const char* alias, bool enBucle)
{
	char cmd[128];
	snprintf(cmd, sizeof(cmd), enBucle ? "play %s from 0 repeat" : "play %s from 0", alias);
	mciSendStringA(cmd, NULL, 0, NULL);
}

void Audio::Detener(const char* alias)
{
	char cmd[128];
	snprintf(cmd, sizeof(cmd), "stop %s", alias);
	mciSendStringA(cmd, NULL, 0, NULL);
}

void Audio::Volumen(const char* alias, int volumen)
{
	if (volumen < 0) volumen = 0;
	if (volumen > 1000) volumen = 1000;
	for (int i = 0; i < cuantos; i++)
		if (strcmp(nombres[i], alias) == 0) {
			if (volumenes[i] >= 0 && abs(volumenes[i] - volumen) < 15) return;	// casi igual, no vale la pena
			volumenes[i] = volumen;
		}
	char cmd[128];
	snprintf(cmd, sizeof(cmd), "setaudio %s volume to %d", alias, volumen);
	mciSendStringA(cmd, NULL, 0, NULL);
}

void Audio::CerrarTodo()
{
	mciSendStringA("close all", NULL, 0, NULL);
}
