#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	// AB3: regresa si se presiono P desde el ultimo cuadro y la limpia (asi un solo golpe arranca un solo despegue)
	bool getPelicanPresionado() { bool p = pelicanPresionado; pelicanPresionado = false; return p; }
	bool getComaPresionada() { bool p = comaPresionada; comaPresionada = false; return p; }	// coma: toma o suelta el Arwing de Venom
	bool getCoheteVenomPresionado() { bool p = coheteVenomPresionado; coheteVenomPresionado = false; return p; }	// tecla F: cohete de Venom
	GLfloat getArwingGiro() { return (keys[GLFW_KEY_A] ? 1.0f : 0.0f) - (keys[GLFW_KEY_D] ? 1.0f : 0.0f); }	// A gira a la izquierda, D a la derecha
	GLfloat getArwingPitch() { return (keys[GLFW_KEY_W] ? 1.0f : 0.0f) - (keys[GLFW_KEY_S] ? 1.0f : 0.0f); }	// W sube la nariz, S la baja
	bool getcamaraLibre() { return camaraLibre; }	// tecla C: sale del avatar (camara libre) o regresa a el
	bool getwarthogAndando() { return warthogAndando; }	// tecla O: el Warthog patrulla o se detiene
	bool getplumaArriba() { return plumaArriba; }		// tecla J: la pluma del puesto de control sube o baja
	// teclas de un solo golpe (L, K, X, R, G, H, B, V, M, T, espacio, retroceso, F5 y F6): regresa si se presiono desde la ultima vez que se pregunto y la limpia
	bool getTeclaGolpe(int tecla) { bool p = teclaGolpe[tecla]; teclaGolpe[tecla] = false; return p; }
	int getCamaraPedida() { int c = camaraPedida; camaraPedida = 0; return c; }	// teclas 1 a 6: numero de la ultima camara pedida (0 si no hubo ninguna)
	GLfloat getAvatarAdelante() { return (keys[GLFW_KEY_W] ? 1.0f : 0.0f) - (keys[GLFW_KEY_S] ? 1.0f : 0.0f); }	// W camina hacia adelante y S hacia atras
	GLfloat getAvatarLado() { return (keys[GLFW_KEY_D] ? 1.0f : 0.0f) - (keys[GLFW_KEY_A] ? 1.0f : 0.0f); }	// D camina a la derecha y A a la izquierda

	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	bool mouseFirstMoved;
	bool pelicanPresionado;
	bool comaPresionada;
	bool coheteVenomPresionado;
	bool camaraLibre;
	bool warthogAndando;
	bool plumaArriba;
	bool teclaGolpe[1024];
	int camaraPedida;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

