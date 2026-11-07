#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	pelicanPresionado = false;
	comaPresionada = false;
	coheteVenomPresionado = false;
	camaraLibre = false;
	warthogAndando = false;
	plumaArriba = false;
	camaraPedida = 0;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
		teclaGolpe[i] = false;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	pelicanPresionado = false;
	comaPresionada = false;
	coheteVenomPresionado = false;
	camaraLibre = false;
	warthogAndando = false;
	plumaArriba = false;
	camaraPedida = 0;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
		teclaGolpe[i] = false;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Proyecto final CGeIHC - Sistema Tres Mundos", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)	// AB3 Pelican de Reach
	{
		theWindow->pelicanPresionado = true;
	}
	if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)	// el Arwing de Venom se pilota o se suelta
	{
		theWindow->comaPresionada = true;
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)	// el cohete de Venom despega o regresa a la cima del domo
	{
		theWindow->coheteVenomPresionado = true;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS)	// sale del avatar y deja la camara libre, o regresa al avatar
	{
		theWindow->camaraLibre = !theWindow->camaraLibre;
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS)	// el Warthog de Reach patrulla o se detiene
	{
		theWindow->warthogAndando = !theWindow->warthogAndando;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)	// la pluma del puesto de control de Reach sube o baja
	{
		theWindow->plumaArriba = !theWindow->plumaArriba;
	}
	if (action == GLFW_PRESS && key >= GLFW_KEY_1 && key <= GLFW_KEY_6)	// camaras 1 a 6 (la 5 y la 6 son las del cohete de Reach)
	{
		theWindow->camaraPedida = key - GLFW_KEY_1 + 1;
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 1.0;
	}



	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			theWindow->teclaGolpe[key] = true;	// queda anotada hasta que el main pregunte con getTeclaGolpe
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
