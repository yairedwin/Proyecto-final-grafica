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
	bool getwarthogAndando() { return warthogAndando; }	// tecla O: el Warthog patrulla o se detiene
	bool getplumaArriba() { return plumaArriba; }		// tecla J: la pluma del puesto de control sube o baja

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
	bool warthogAndando;
	bool plumaArriba;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

