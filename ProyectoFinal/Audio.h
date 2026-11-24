#pragma once
// Clase para el audio del proyecto: usa la biblioteca miniaudio (un solo archivo, miniaudio.h, junto a este).
// Cada sonido se abre con un nombre (alias) y despues se reproduce, se detiene o se le cambia el volumen por ese nombre.
class Audio
{
public:
	static bool Abrir(const char* alias, const char* archivo);	// la primera vez tambien enciende el motor de audio
	static void Reproducir(const char* alias, bool enBucle);	// enBucle = true para musica y ambientes
	static void Detener(const char* alias);
	static void Volumen(const char* alias, int volumen);	// 0 a 1000
	static void CerrarTodo();
};
