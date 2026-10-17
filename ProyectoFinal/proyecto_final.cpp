/*
Proyecto final CGeIHC 2027-1 · Sistema Tres Mundos
Base: codigo de la practica 7 (iluminacion).
Secciones del main (cada quien trabaja en la suya y en su rama):
  1. Geometria hecha en codigo ................ comun
  2. Iluminacion: sol, luminarias, spotlights  comun
  3. Camaras C1 a C4 .......................... comun
  4. Escenario comun: planetas, pista, estacion, plataforma, cuerpos celestes, nave viajera
  5. P1 Mundo Forja (Ratchet & Clank) ......... 419049898
  6. P2 Reach (Halo) .......................... 317136421
  7. P3 Venom (Star Fox 64) ................... 318337872
  8. Avatar ................................... pendiente

Teclas que ya funcionan: 1-4 camaras, WASD + mouse en la camara libre, T viajar al siguiente planeta,
R cohete (AB1), G torreta (AB2), H Ratchet vuela con el Heli-Pack (KF1), L reflector S1, K luces de la estacion S2,
X acelera el ciclo de dia (solo pruebas)
*/
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <stdlib.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;
const float PI = 3.14159265f;

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

// texturas (las de terreno y skybox las hicimos con generar_texturas.py)
Texture plainTexture;
Texture forjaTexture;
Texture reachTexture;
Texture venomTexture;
Texture rocaTexture;
Texture metalTexture;
Texture pistaTexture;
Texture neonTexture;	// franjas de luz de las orillas de la pista

// modelos reunidos de Kenney (Space Kit, licencia CC0), en Models/kenney
Model naveRacer;		// N1
Model naveSpeederA;		// N2
Model naveSpeederB;		// N3
Model antenaSatelite;	// satelites del espacio
Model meteoro;			// asteroides
Model hangarReach;		// HR-01
Model torretaDoble;		// SF-02
Model chimenea;			// MF-01
Model generador;		// junto a las refinerias de Mundo Forja
// modelos de Ratchet & Clank (The Models Resource, de los juegos), en Models/rc
Model ratchetModelo;	// MF-11 figura y KF1 vuelo con el Heli-Pack
Model vendedorGadgetron;	// MF-06

// un skybox por zona: espacio y uno por planeta
Skybox skyEspacio;
Skybox skyForja;
Skybox skyReach;
Skybox skyVenom;

// materiales (tabla 1.7.5 de la propuesta)
Material metalPulido;
Material metalPintado;
Material metalNave;
Material concreto;
Material roca;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

// indices de las mallas en meshListModel
#define CUBO 0
#define TORNILLO 1
#define PISTA 2
#define ESFERA 3
#define HEMISFERIO 4
#define CILINDRO 5
#define CONO 6
#define DISCO 7
#define MURO_PISTA 8

// la pista: elipse de semiejes 60 y 30 con centro en (0, 12, 22); sube y baja con un seno para que se vea mas futurista
const float PISTA_A = 60.0f, PISTA_B = 30.0f, PISTA_Z = 22.0f, PISTA_ANCHO = 4.0f;
float alturaPista(float t) { return 12.0f + 2.5f * sin(2.0f * t); }



// 1. GEOMETRIA HECHA EN CODIGO
// ojo: este shader pide las normales al reves (hacia adentro), por eso todas van con signo menos


// cubo de lado 1 centrado en el origen, cada cara con sus uv completas
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2,	2, 3, 0,
		4, 5, 6,	6, 7, 4,
		8, 9, 10,	10, 11, 8,
		12, 13, 14,	14, 15, 12,
		16, 17, 18,	18, 19, 16,
		20, 21, 22,	22, 23, 20,
	};
	GLfloat cubo_vertices[] = {
		//x		y		z		S		T		NX		NY		NZ
		// frente (+z)
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		0.0f,	0.0f,	-1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f,	0.0f,	-1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		0.0f,	0.0f,	-1.0f,
		// derecha (+x)
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		-1.0f,	0.0f,	0.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		-1.0f,	0.0f,	0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		-1.0f,	0.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		-1.0f,	0.0f,	0.0f,
		// atras (-z)
		 0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f,	0.0f,	1.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		0.0f,	0.0f,	1.0f,
		 0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f,	0.0f,	1.0f,
		// izquierda (-x)
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		1.0f,	0.0f,	0.0f,
		// abajo (-y)
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 1.0f,		0.0f,	1.0f,	0.0f,
		// arriba (+y)
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		0.0f,	-1.0f,	0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f,	-1.0f,	0.0f,
	};
	MeshModel* cubo = new MeshModel();
	cubo->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(cubo);
}

// tornillo (bolt): prisma hexagonal de radio 0.5 y alto 0.3, acostado en el plano XY para que se vea de frente
void CrearTornillo()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	// tapas: centro + 6 vertices en z = +0.15 y z = -0.15
	for (int tapa = 0; tapa < 2; tapa++) {
		float z = tapa == 0 ? 0.15f : -0.15f;
		float nz = tapa == 0 ? -1.0f : 1.0f;
		unsigned int base = v.size() / 8;
		v.insert(v.end(), { 0.0f, 0.0f, z,	0.5f, 0.5f,	0.0f, 0.0f, nz });
		for (int i = 0; i < 6; i++) {
			float a = i * PI / 3.0f;
			v.insert(v.end(), { 0.5f * cos(a), 0.5f * sin(a), z,	0.5f + 0.5f * cos(a), 0.5f + 0.5f * sin(a),	0.0f, 0.0f, nz });
		}
		for (int i = 0; i < 6; i++) {
			ind.push_back(base); ind.push_back(base + 1 + i); ind.push_back(base + 1 + (i + 1) % 6);
		}
	}
	// los 6 lados, cada uno con su propia normal
	for (int i = 0; i < 6; i++) {
		float a1 = i * PI / 3.0f, a2 = (i + 1) * PI / 3.0f, am = (a1 + a2) / 2.0f;
		unsigned int base = v.size() / 8;
		v.insert(v.end(), { 0.5f * cos(a1), 0.5f * sin(a1),  0.15f,	0.0f, 0.0f,	-cos(am), -sin(am), 0.0f });
		v.insert(v.end(), { 0.5f * cos(a2), 0.5f * sin(a2),  0.15f,	1.0f, 0.0f,	-cos(am), -sin(am), 0.0f });
		v.insert(v.end(), { 0.5f * cos(a2), 0.5f * sin(a2), -0.15f,	1.0f, 1.0f,	-cos(am), -sin(am), 0.0f });
		v.insert(v.end(), { 0.5f * cos(a1), 0.5f * sin(a1), -0.15f,	0.0f, 1.0f,	-cos(am), -sin(am), 0.0f });
		ind.insert(ind.end(), { base, base + 1, base + 2, base + 2, base + 3, base });
	}
	MeshModel* tornillo = new MeshModel();
	tornillo->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(tornillo);
}

// pista E2: tira de triangulos entre dos elipses con PISTA_ANCHO de cada lado; la altura sale de alturaPista(t)
void CrearPista()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int n = 320;
	for (int i = 0; i <= n; i++) {
		float t = 2.0f * PI * i / n;
		float vt = i * 0.5f;   // la textura avanza a lo largo de la pista y se repite
		float y = alturaPista(t);
		v.insert(v.end(), { (PISTA_A - PISTA_ANCHO) * cos(t), y, (PISTA_B - PISTA_ANCHO) * sin(t),	0.0f, vt,	0.0f, -1.0f, 0.0f });
		v.insert(v.end(), { (PISTA_A + PISTA_ANCHO) * cos(t), y, (PISTA_B + PISTA_ANCHO) * sin(t),	1.0f, vt,	0.0f, -1.0f, 0.0f });
	}
	for (int i = 0; i < n; i++) {
		unsigned int a = 2 * i;
		ind.insert(ind.end(), { a, a + 1, a + 3, a + 3, a + 2, a });
	}
	MeshModel* pista = new MeshModel();
	pista->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(pista);
}

// muros bajos de las dos orillas de la pista (la franja de luz va en la textura neon)
void CrearMuroPista()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int n = 320;
	for (int lado = 0; lado < 2; lado++) {
		float d = lado == 0 ? -PISTA_ANCHO : PISTA_ANCHO;
		unsigned int base = v.size() / 8;
		for (int i = 0; i <= n; i++) {
			float t = 2.0f * PI * i / n;
			float y = alturaPista(t);
			float nx = (lado == 0 ? 1.0f : -1.0f) * cos(t), nz = (lado == 0 ? 1.0f : -1.0f) * sin(t);	// normal al reves, como pide el shader
			v.insert(v.end(), { (PISTA_A + d) * cos(t), y - 0.3f, (PISTA_B + d) * sin(t),	i * 0.25f, 0.0f,	nx, 0.0f, nz });
			v.insert(v.end(), { (PISTA_A + d) * cos(t), y + 0.9f, (PISTA_B + d) * sin(t),	i * 0.25f, 1.0f,	nx, 0.0f, nz });
		}
		for (int i = 0; i < n; i++) {
			unsigned int a = base + 2 * i;
			ind.insert(ind.end(), { a, a + 1, a + 3, a + 3, a + 2, a });
		}
	}
	MeshModel* muro = new MeshModel();
	muro->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(muro);
}

// esfera de radio 1; con lat0 y lat1 se puede sacar solo un pedazo (el hemisferio de abajo de los planetas)
void CrearEsfera(float lat0, float lat1)
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int pisos = 16, gajos = 32;
	for (int i = 0; i <= pisos; i++) {
		float lat = lat0 + (lat1 - lat0) * i / pisos;   // 0 = polo de arriba, PI = polo de abajo
		for (int j = 0; j <= gajos; j++) {
			float lon = 2.0f * PI * j / gajos;
			float x = sin(lat) * cos(lon), y = cos(lat), z = sin(lat) * sin(lon);
			v.insert(v.end(), { x, y, z,	(float)j / gajos * 2.0f, (float)i / pisos,	-x, -y, -z });
		}
	}
	for (int i = 0; i < pisos; i++)
		for (int j = 0; j < gajos; j++) {
			unsigned int a = i * (gajos + 1) + j, b = a + gajos + 1;
			ind.insert(ind.end(), { a, b, a + 1, a + 1, b, b + 1 });
		}
	MeshModel* esfera = new MeshModel();
	esfera->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(esfera);
}

// cilindro de y = 0 a y = 1 con radio de abajo 0.5; con rArriba = 0 sale un cono
void CrearCilindro(float rArriba)
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int n = 24;
	for (int i = 0; i <= n; i++) {
		float a = 2.0f * PI * i / n;
		v.insert(v.end(), { 0.5f * cos(a), 0.0f, 0.5f * sin(a),		(float)i / n, 0.0f,	-cos(a), 0.0f, -sin(a) });
		v.insert(v.end(), { rArriba * cos(a), 1.0f, rArriba * sin(a),	(float)i / n, 1.0f,	-cos(a), 0.0f, -sin(a) });
	}
	for (int i = 0; i < n; i++) {
		unsigned int a = 2 * i;
		ind.insert(ind.end(), { a, a + 2, a + 1, a + 1, a + 2, a + 3 });
	}
	// tapas
	for (int tapa = 0; tapa < 2; tapa++) {
		float y = tapa == 0 ? 0.0f : 1.0f, r = tapa == 0 ? 0.5f : rArriba, ny = tapa == 0 ? 1.0f : -1.0f;
		unsigned int c = v.size() / 8;
		v.insert(v.end(), { 0.0f, y, 0.0f,	0.5f, 0.5f,	0.0f, ny, 0.0f });
		for (int i = 0; i <= n; i++) {
			float a = 2.0f * PI * i / n;
			v.insert(v.end(), { r * cos(a), y, r * sin(a),	0.5f + 0.5f * cos(a), 0.5f + 0.5f * sin(a),	0.0f, ny, 0.0f });
		}
		for (int i = 0; i < n; i++) { ind.push_back(c); ind.push_back(c + 1 + i); ind.push_back(c + 2 + i); }
	}
	MeshModel* cil = new MeshModel();
	cil->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(cil);
}

// disco de radio 1 en y = 0 (piso de los planetas y de la plataforma); la textura se repite 12 veces (los planetas ya son el doble de grandes)
void CrearDisco()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> ind;
	int n = 96;
	v.insert(v.end(), { 0.0f, 0.0f, 0.0f,	0.0f, 0.0f,	0.0f, -1.0f, 0.0f });
	for (int i = 0; i <= n; i++) {
		float a = 2.0f * PI * i / n;
		v.insert(v.end(), { cos(a), 0.0f, sin(a),	12.0f * cos(a), 12.0f * sin(a),	0.0f, -1.0f, 0.0f });
	}
	for (int i = 0; i < n; i++) { ind.push_back(0); ind.push_back(1 + i); ind.push_back(2 + i); }
	MeshModel* disco = new MeshModel();
	disco->CreateMeshModel(v.data(), ind.data(), v.size(), ind.size());
	meshListModel.push_back(disco);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();
	CrearCubo();			// 0
	CrearTornillo();		// 1
	CrearPista();			// 2
	CrearEsfera(0.0f, PI);	// 3 esfera completa
	CrearEsfera(PI / 2, PI);	// 4 hemisferio de abajo
	CrearCilindro(0.5f);	// 5
	CrearCilindro(0.0f);	// 6 cono
	CrearDisco();			// 7
	CrearMuroPista();		// 8
	CreateShaders();

	// camara libre: arranca viendo Mundo Forja desde arriba
	camera = Camera(glm::vec3(-90.0f, 38.0f, 22.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -32.0f, 0.6f, 0.5f);
	// si existe la variable PF_CAM ("x,y,z,yaw,pitch") la camara libre arranca ahi; sirve para tomar capturas de un punto exacto
	char textoCam[128]; size_t largoCam = 0;
	if (getenv_s(&largoCam, textoCam, sizeof(textoCam), "PF_CAM") == 0 && largoCam > 0) {
		float cx, cy, cz, cyaw, cpitch;
		if (sscanf_s(textoCam, "%f,%f,%f,%f,%f", &cx, &cy, &cz, &cyaw, &cpitch) == 5)
			camera = Camera(glm::vec3(cx, cy, cz), glm::vec3(0.0f, 1.0f, 0.0f), cyaw, cpitch, 0.6f, 0.5f);
	}

	plainTexture = Texture("Textures/plain.png");			plainTexture.LoadTextureA();
	forjaTexture = Texture("Textures/terreno_forja.png");	forjaTexture.LoadTextureA();
	reachTexture = Texture("Textures/terreno_reach.png");	reachTexture.LoadTextureA();
	venomTexture = Texture("Textures/terreno_venom.png");	venomTexture.LoadTextureA();
	rocaTexture = Texture("Textures/roca.png");				rocaTexture.LoadTextureA();
	metalTexture = Texture("Textures/metal.png");			metalTexture.LoadTextureA();
	pistaTexture = Texture("Textures/pista_espacial.png");	pistaTexture.LoadTextureA();
	neonTexture = Texture("Textures/neon_pista.png");		neonTexture.LoadTextureA();

	naveRacer = Model();		naveRacer.LoadModel("Models/kenney/craft_racer.obj");
	naveSpeederA = Model();		naveSpeederA.LoadModel("Models/kenney/craft_speederA.obj");
	naveSpeederB = Model();		naveSpeederB.LoadModel("Models/kenney/craft_speederB.obj");
	antenaSatelite = Model();	antenaSatelite.LoadModel("Models/kenney/satelliteDish_large.obj");
	meteoro = Model();			meteoro.LoadModel("Models/kenney/meteor_detailed.obj");
	hangarReach = Model();		hangarReach.LoadModel("Models/kenney/hangar_largeA.obj");
	torretaDoble = Model();		torretaDoble.LoadModel("Models/kenney/turret_double.obj");
	chimenea = Model();			chimenea.LoadModel("Models/kenney/chimney_detailed.obj");
	generador = Model();		generador.LoadModel("Models/kenney/machine_generatorLarge.obj");
	ratchetModelo = Model();	ratchetModelo.LoadModel("Models/rc/ratchet.obj");
	vendedorGadgetron = Model();	vendedorGadgetron.LoadModel("Models/rc/vendedor.obj");

	// skyboxes: orden rt, lf, up, dn, bk, ft
	std::vector<std::string> caras;
	const char* nombresSky[4] = { "espacio", "forja", "reach", "venom" };
	Skybox* skies[4] = { &skyEspacio, &skyForja, &skyReach, &skyVenom };
	for (int s = 0; s < 4; s++) {
		caras.clear();
		const char* lado[6] = { "rt", "lf", "up", "dn", "bk", "ft" };
		for (int k = 0; k < 6; k++) caras.push_back(std::string("Textures/Skybox/") + nombresSky[s] + "_" + lado[k] + ".png");
		*skies[s] = Skybox(caras);
	}

	metalPulido = Material(1.0f, 128);
	metalPintado = Material(0.5f, 32);
	metalNave = Material(0.8f, 64);
	concreto = Material(0.2f, 8);
	roca = Material(0.1f, 4);


	//=========================================================================================
	// 2. ILUMINACION (las luces fijas se crean aqui, el sol y los apagados se calculan en el ciclo)
	//=========================================================================================
	// luminarias L1 a L6, dos por planeta (posiciones de la tabla 1.13.1); alcance ~13 con 1.0, 0.35, 0.44
	PointLight luminarias[6];
	// (los planetas crecieron al doble el 17-oct, por eso las posiciones son el doble de las de la tabla)
	luminarias[0] = PointLight(1.0f, 0.8f, 0.4f, 0.0f, 1.0f, -104.0f, 5.0f, -50.0f, 1.0f, 0.22f, 0.02f);	// L1 forja, amarillo
	luminarias[1] = PointLight(1.0f, 0.8f, 0.4f, 0.0f, 1.0f, -76.0f, 5.0f, -48.0f, 1.0f, 0.22f, 0.02f);	// L2 forja
	luminarias[2] = PointLight(0.8f, 0.9f, 1.0f, 0.0f, 1.0f, 76.0f, 5.0f, -52.0f, 1.0f, 0.22f, 0.02f);	// L3 reach, blanco azulado
	luminarias[3] = PointLight(0.8f, 0.9f, 1.0f, 0.0f, 1.0f, 104.0f, 5.0f, -50.0f, 1.0f, 0.22f, 0.02f);	// L4 reach
	luminarias[4] = PointLight(0.3f, 1.0f, 0.3f, 0.0f, 1.0f, -14.0f, 5.0f, 76.0f, 1.0f, 0.22f, 0.02f);	// L5 venom, verde
	luminarias[5] = PointLight(0.3f, 1.0f, 0.3f, 0.0f, 1.0f, 14.0f, 5.0f, 76.0f, 1.0f, 0.22f, 0.02f);	// L6 venom
	glm::vec3 posLuminaria[6] = { glm::vec3(-104, 0, -50), glm::vec3(-76, 0, -48), glm::vec3(76, 0, -52), glm::vec3(104, 0, -50), glm::vec3(-14, 0, 76), glm::vec3(14, 0, 76) };

	// S1 reflector de la plataforma (tecla L) y S2 luces de la estacion (tecla K)
	SpotLight reflectorS1 = SpotLight(1.0f, 1.0f, 0.8f, 0.0f, 3.0f, -90.0f, 16.0f, -67.0f, 0.0f, -1.0f, 0.35f, 1.0f, 0.02f, 0.001f, 25.0f);
	SpotLight estacionS2 = SpotLight(0.4f, 0.8f, 1.0f, 0.0f, 3.0f, 0.0f, 54.0f, -46.0f, 0.0f, 0.3f, -1.0f, 1.0f, 0.02f, 0.001f, 20.0f);
	bool s1Prendido = true, s2Prendido = true;
	bool teclaL = false, teclaK = false;	// para que el toggle no se repita mientras se deja presionada

	// ciclo de dia y noche: 120 s; con X se acelera para probarlo (la N es para KF2)
	float tiempoDia = 30.0f;	// arranca a media manana
	{ char textoHora[32]; size_t largoHora = 0; float horaIni;	// PF_HORA (segundos del ciclo) sirve para ver el escenario de dia o de noche sin esperar
	  if (getenv_s(&largoHora, textoHora, sizeof(textoHora), "PF_HORA") == 0 && largoHora > 0 && sscanf_s(textoHora, "%f", &horaIni) == 1) tiempoDia = horaIni; }
	bool teclaX = false; float velocidadDia = 1.0f;


	//=========================================================================================
	// 3. CAMARAS (1 libre por ahora, 2 nave N1, 3 nave viajera, 4 cinematica)
	//=========================================================================================
	int modoCamara = 1;
	glm::vec3 camPos = camera.getCameraPosition(), camMira = camPos + camera.getCameraDirection();
	glm::vec3 camPosAntes = camPos, camMiraAntes = camMira;
	float tTransicion = 1.0f;	// 1 = ya llego; cuando cambia de camara se reinicia y se interpola 0.5 s
	float anguloC4 = 0.0f;
	// puntos de llegada para la tecla T (centro de cada planeta, en el orden P1 -> P2 -> P3)
	glm::vec3 llegadas[3] = { glm::vec3(-104.0f, 7.0f, -14.0f), glm::vec3(72.0f, 7.0f, -16.0f), glm::vec3(0.0f, 7.0f, 62.0f) };	// dentro de la zona del skybox de cada planeta
	int planetaActual = 0; bool teclaT = false;


	//=========================================================================================
	// variables de las animaciones
	//=========================================================================================
	// AC2 naves de la pista: angulo de cada una y velocidad base distinta para que se rebasen
	float angNave[3] = { 0.0f, 2.1f, 4.2f };
	float w0Nave[3] = { 0.30f, 0.28f, 0.26f };
	glm::vec3 colorNave[3] = { glm::vec3(0.9f, 0.45f, 0.15f), glm::vec3(0.65f, 0.7f, 0.75f), glm::vec3(0.95f, 0.55f, 0.65f) };	// N1 hoverbike, N2 sabre, N3 catspaw
	// nave viajera NV (Pelican): avanza sobre una curva Catmull-Rom P1 -> P2 -> P3 -> P1
	glm::vec3 rutaNV[3] = { glm::vec3(-90.0f, 32.0f, -36.0f), glm::vec3(90.0f, 32.0f, -36.0f), glm::vec3(0.0f, 32.0f, 86.0f) };
	float uNV = 0.5f;	// arranca a medio camino entre Mundo Forja y Reach, a la vista de la camara 3
	// anillo de la estacion
	float giroAnillo = 0.0f;
	// AB1 cohete: 0 en la plataforma, 1 cuenta regresiva, 2 sube, 3 va a la estacion, 4 acoplado
	int faseCohete = 0; float tCohete = 0.0f; glm::vec3 posCohete = glm::vec3(-90.0f, 0.0f, -60.0f); float brazoServicio = 0.0f; float inclinaCohete = 0.0f;
	// KF1 Ratchet vuela con el Heli-Pack alrededor de la refineria (tecla H): cuadros clave guardados e interpolacion lineal
	// cada cuadro es la posicion local en Mundo Forja y el giro en Y; entre cuadro y cuadro hay i_max_steps pasos
	// (las posiciones son desde el pedestal de la figura MF-11, asi el vuelo empieza y termina ahi)
	glm::vec3 kfPos[9] = { glm::vec3(0, 0, 0), glm::vec3(4, 5, -4), glm::vec3(10, 10, -14), glm::vec3(16, 14, -24), glm::vec3(26, 13, -18),
		glm::vec3(26, 14, -6), glm::vec3(16, 10, 2), glm::vec3(6, 5, 2), glm::vec3(0, 0, 0) };
	float kfGiro[9] = { 0.0f, 140.0f, 149.0f, 100.0f, 30.0f, -25.0f, -70.0f, -100.0f, 0.0f };
	// ajustes de los modelos del juego: hacia donde miran y donde va Clank en la espalda de Ratchet (en unidades del modelo)
	float giroBaseRatchet = -90.0f, giroVendedor = 0.0f;
	glm::vec3 clankEspalda = glm::vec3(-0.32f, 0.85f, 0.0f);
	const int i_max_steps = 90;
	int kfCuadro = 0, i_curr_steps = 0; bool kfReproduciendo = false, teclaH = false;
	glm::vec3 kfActual = kfPos[0]; float kfGiroActual = kfGiro[0]; float kfAcum = 0.0f;
	// AB2 torreta: 0 quieta, 1 gira hacia la camara, 2 dispara
	int faseTorreta = 0; float giroTorre = 0.0f, angCanon = 0.0f, retroceso = 0.0f, giroObjetivo = 0.0f; glm::vec3 posBala; glm::vec3 dirBala; float tBala = 0.0f;
	bool teclaR = false, teclaG = false;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		float dt = deltaTime / (1.0f + 1.0f / (float)limitFPS);	// segundos reales del cuadro (el deltaTime de la base viene multiplicado por 61)
		if (dt > 0.1f) dt = 0.1f;

		glfwPollEvents();
		bool* teclas = mainWindow.getsKeys();

		// ---------------- teclas de un solo golpe ----------------
		if (teclas[GLFW_KEY_L] && !teclaL) s1Prendido = !s1Prendido;	teclaL = teclas[GLFW_KEY_L];
		if (teclas[GLFW_KEY_K] && !teclaK) s2Prendido = !s2Prendido;	teclaK = teclas[GLFW_KEY_K];
		if (teclas[GLFW_KEY_X] && !teclaX) velocidadDia = velocidadDia > 1.0f ? 1.0f : 10.0f;	teclaX = teclas[GLFW_KEY_X];
		if (teclas[GLFW_KEY_R] && !teclaR && faseCohete == 0) { faseCohete = 1; tCohete = 0.0f; }	teclaR = teclas[GLFW_KEY_R];
		if (teclas[GLFW_KEY_G] && !teclaG && faseTorreta == 0) {
			faseTorreta = 1;
			glm::vec3 t = glm::vec3(-98.0f, 0.0f, -56.0f);	// torreta de la izquierda de la plataforma
			giroObjetivo = glm::degrees(atan2(camPos.x - t.x, camPos.z - t.z));
		}
		teclaG = teclas[GLFW_KEY_G];
		if (teclas[GLFW_KEY_H] && !teclaH) {	// H arranca o detiene el vuelo de KF1
			kfReproduciendo = !kfReproduciendo;
			if (kfReproduciendo && kfCuadro >= 8) { kfCuadro = 0; i_curr_steps = 0; }
		}
		teclaH = teclas[GLFW_KEY_H];
		int modoNuevo = modoCamara;
		if (teclas[GLFW_KEY_1]) modoNuevo = 1;
		if (teclas[GLFW_KEY_2]) modoNuevo = 2;
		if (teclas[GLFW_KEY_3]) modoNuevo = 3;
		if (teclas[GLFW_KEY_4]) modoNuevo = 4;
		if (modoNuevo != modoCamara) { camPosAntes = camPos; camMiraAntes = camMira; tTransicion = 0.0f; modoCamara = modoNuevo; }
		if (teclas[GLFW_KEY_T] && !teclaT && modoCamara == 1) {
			// T: viaja al siguiente planeta (por ahora mueve la camara libre; despues movera al avatar)
			planetaActual = (planetaActual + 1) % 3;
			glm::vec3 centro = planetaActual == 0 ? glm::vec3(-90, 0, -36) : planetaActual == 1 ? glm::vec3(90, 0, -36) : glm::vec3(0, 0, 86);
			glm::vec3 llega = llegadas[planetaActual];
			float yaw = glm::degrees(atan2(centro.z - llega.z, centro.x - llega.x));
			camera = Camera(llega, glm::vec3(0.0f, 1.0f, 0.0f), yaw, -15.0f, 0.6f, 0.5f);
		}
		teclaT = teclas[GLFW_KEY_T];

		if (modoCamara == 1) {
			camera.keyControl(teclas, deltaTime);
			// el primer movimiento del mouse a veces trae un salto enorme y deja la camara en NaN; esos se ignoran
			float dx = mainWindow.getXChange(), dy = mainWindow.getYChange();
			if (fabs(dx) < 200.0f && fabs(dy) < 200.0f) camera.mouseControl(dx, dy);
		}
		else { mainWindow.getXChange(); mainWindow.getYChange(); }

		//=====================================================================================
		// ANIMACIONES (se calculan antes de dibujar)
		//=====================================================================================
		// AC2: x = a cos(t), z = b sin(t), con w(t) = w0 + 0.2 sin(t + i) en proporcion para que se rebasen
		// (estuvieron quietas para la revision del 16-oct; ya vimos animacion y se volvieron a activar)
		bool navesEnMovimiento = true;
		if (navesEnMovimiento) for (int i = 0; i < 3; i++) angNave[i] += (w0Nave[i] + 0.06f * sin(now + i)) * dt;
		// NV: avanza un tramo de la ruta cada 16 s (la ruta es el doble de larga que antes)
		if (navesEnMovimiento) { uNV += dt / 16.0f; if (uNV >= 3.0f) uNV -= 3.0f; }
		giroAnillo += 10.0f * dt;
		// KF1: avanza un paso por cuadro (60 cuadros por segundo) e interpola entre el cuadro actual y el siguiente
		if (kfReproduciendo) {
			kfAcum += dt * 60.0f;	// el ciclo no esta limitado a 60 fps, asi que cuento los pasos con el tiempo real
			while (kfAcum >= 1.0f) { kfAcum -= 1.0f; i_curr_steps++; }
			if (i_curr_steps >= i_max_steps) { i_curr_steps = 0; kfCuadro++; }
			if (kfCuadro >= 8) { kfCuadro = 8; i_curr_steps = 0; kfReproduciendo = false; }	// termina en el ultimo cuadro (que es igual al primero)
		}
		{
			int sig = kfCuadro < 8 ? kfCuadro + 1 : 8;
			float f = (float)i_curr_steps / i_max_steps;
			kfActual = kfPos[kfCuadro] + (kfPos[sig] - kfPos[kfCuadro]) * f;
			kfGiroActual = kfGiro[kfCuadro] + (kfGiro[sig] - kfGiro[kfCuadro]) * f;
		}
		// AB1 cohete
		if (faseCohete == 1) { tCohete += dt; brazoServicio = glm::min(tCohete / 3.0f, 1.0f) * 70.0f; if (tCohete >= 3.0f) { faseCohete = 2; tCohete = 0.0f; } }
		else if (faseCohete == 2) {	// sube con aceleracion constante: y = 1/2 a t^2
			tCohete += dt; posCohete.y = 0.5f * 2.5f * tCohete * tCohete;
			if (posCohete.y >= 45.0f) { posCohete.y = 45.0f; faseCohete = 3; tCohete = 0.0f; }
		}
		else if (faseCohete == 3) {	// se inclina y va hacia la bahia de la estacion
			tCohete += dt; float u = glm::min(tCohete / 6.0f, 1.0f);
			posCohete = glm::mix(glm::vec3(-90.0f, 45.0f, -60.0f), glm::vec3(-6.4f, 56.0f, -51.6f), u);
			inclinaCohete = -60.0f * sin(u * PI);
			if (u >= 1.0f) { faseCohete = 4; tCohete = 0.0f; }
		}
		else if (faseCohete == 4) {	// acoplado: desaparece y a los 4 s regresa a la plataforma
			tCohete += dt;
			if (tCohete > 4.0f) { faseCohete = 0; posCohete = glm::vec3(-90.0f, 0.0f, -60.0f); brazoServicio = 0.0f; inclinaCohete = 0.0f; }
		}
		// AB2 torreta
		if (faseTorreta == 1) {
			float dif = giroObjetivo - giroTorre;
			giroTorre += glm::clamp(dif, -90.0f * dt, 90.0f * dt);
			angCanon = glm::min(angCanon + 20.0f * dt, 15.0f);
			if (fabs(dif) < 1.0f && angCanon >= 15.0f) {
				faseTorreta = 2; tBala = 0.0f; retroceso = 0.3f;
				posBala = glm::vec3(-98.0f, 2.2f, -56.0f);
				dirBala = glm::vec3(sin(glm::radians(giroTorre)), -sin(glm::radians(angCanon)) * 0.3f, cos(glm::radians(giroTorre)));
			}
		}
		else if (faseTorreta == 2) {
			tBala += dt; posBala += dirBala * 25.0f * dt; retroceso = glm::max(retroceso - 0.6f * dt, 0.0f);
			if (tBala > 1.5f) { faseTorreta = 3; }
		}
		else if (faseTorreta == 3) {	// regresa a su posicion
			angCanon = glm::max(angCanon - 20.0f * dt, 0.0f);
			if (angCanon <= 0.0f) faseTorreta = 0;
		}

		// ---------------- posicion de las naves para las camaras ----------------
		glm::vec3 posN1 = glm::vec3(PISTA_A * cos(angNave[0]), alturaPista(angNave[0]) + 0.9f, PISTA_Z + PISTA_B * sin(angNave[0]));
		glm::vec3 tanN1 = glm::normalize(glm::vec3(-PISTA_A * sin(angNave[0]), 5.0f * cos(2.0f * angNave[0]), PISTA_B * cos(angNave[0])));	// derivada de la curva (tambien en y porque la pista sube y baja)
		int tramo = (int)uNV; float u = uNV - tramo;
		// Catmull-Rom con los 3 puntos del circuito: p0 anterior, p1 y p2 del tramo, p3 siguiente (con 3 puntos p0 y p3 son el mismo)
		glm::vec3 p0 = rutaNV[(tramo + 2) % 3], p1 = rutaNV[tramo], p2 = rutaNV[(tramo + 1) % 3], p3 = rutaNV[(tramo + 2) % 3];
		glm::vec3 posNV = 0.5f * ((2.0f * p1) + (-p0 + p2) * u + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * u * u + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * u * u * u);
		glm::vec3 tanNV = glm::normalize(0.5f * ((-p0 + p2) + 2.0f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * u + 3.0f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * u * u));

		// ---------------- camara activa ----------------
		glm::vec3 destPos, destMira;
		if (modoCamara == 1) { destPos = camera.getCameraPosition(); destMira = destPos + camera.getCameraDirection(); }
		else if (modoCamara == 2) { destPos = posN1 - 7.0f * tanN1 + glm::vec3(0.0f, 2.5f, 0.0f); destMira = posN1 + tanN1; }
		else if (modoCamara == 3) { destPos = posNV - 8.0f * tanNV + glm::vec3(0.0f, 2.5f, 0.0f); destMira = posNV + tanNV; }
		else {	// C4: orbita el punto entre la plataforma y la estacion
			anguloC4 += 0.06f * dt;
			glm::vec3 centro = glm::vec3(-45.0f, 28.0f, -58.0f);
			destPos = centro + glm::vec3(70.0f * cos(anguloC4), 14.0f, 70.0f * sin(anguloC4)); destMira = centro;
		}
		if (tTransicion < 1.0f) {	// cambio suave de camara en 0.5 s
			tTransicion = glm::min(tTransicion + dt / 0.5f, 1.0f);
			camPos = glm::mix(camPosAntes, destPos, tTransicion); camMira = glm::mix(camMiraAntes, destMira, tTransicion);
		}
		else { camPos = destPos; camMira = destMira; }
		glm::mat4 vista = glm::lookAt(camPos, camMira, glm::vec3(0.0f, 1.0f, 0.0f));

		// ---------------- sol: ciclo de 120 s ----------------
		tiempoDia += dt * velocidadDia;
		float theta = 2.0f * PI * fmod(tiempoDia, 120.0f) / 120.0f;
		float s = sin(theta);
		glm::vec3 dirSol = glm::normalize(glm::vec3(-cos(theta), -s, -0.3f));
		glm::vec3 colSol;
		if (s > 0.0f) colSol = glm::mix(glm::vec3(1.0f, 0.6f, 0.3f), glm::vec3(1.0f, 1.0f, 0.95f), glm::min(s * 2.0f, 1.0f));	// amanecer -> mediodia
		else colSol = glm::vec3(0.45f, 0.55f, 0.9f);	// noche: luz de luna azulada
		float difusa = glm::max(0.0f, s) * 0.8f;
		if (s < 0.0f) difusa = 0.6f;	// de noche queda la luna para que se siga viendo el escenario
		float ambiental = 0.3f + 0.05f * glm::max(0.0f, s);
		if (s < 0.0f) dirSol = glm::vec3(0.0f, -1.0f, 0.0f);	// de noche la luna cae desde arriba
		mainLight = DirectionalLight(colSol.x, colSol.y, colSol.z, ambiental, difusa, dirSol.x, dirSol.y, dirSol.z);
		bool esDeNoche = s < 0.0f;

		// luminarias: todas a la vez segun el ciclo
		unsigned int pointLightCount = 0;
		if (esDeNoche) for (int i = 0; i < 6; i++) pointLights[pointLightCount++] = luminarias[i];
		// spotlights: solo se mandan los que estan prendidos
		unsigned int spotLightCount = 0;
		if (s1Prendido) { reflectorS1.SetPos(glm::vec3(-90.0f, 16.0f, -67.0f)); spotLights[spotLightCount++] = reflectorS1; }
		if (s2Prendido) spotLights[spotLightCount++] = estacionS2;

		// ---------------- skybox segun la zona ----------------
		Skybox* sky = &skyEspacio;
		if (modoCamara == 1) {
			glm::vec2 c = glm::vec2(camPos.x, camPos.z);
			if (glm::distance(c, glm::vec2(-90.0f, -36.0f)) < 34.0f && camPos.y < 30.0f) sky = &skyForja;
			else if (glm::distance(c, glm::vec2(90.0f, -36.0f)) < 34.0f && camPos.y < 30.0f) sky = &skyReach;
			else if (glm::distance(c, glm::vec2(0.0f, 86.0f)) < 32.0f && camPos.y < 30.0f) sky = &skyVenom;
		}

		// si se maximiza la ventana el dibujo se quedaba en una esquina: ajusto el viewport y la proyeccion al tamaño real
		int anchoVentana, altoVentana;
		glfwGetFramebufferSize(glfwGetCurrentContext(), &anchoVentana, &altoVentana);
		if (anchoVentana > 0 && altoVentana > 0) {
			glViewport(0, 0, anchoVentana, altoVentana);
			projection = glm::perspective(45.0f, (GLfloat)anchoVentana / altoVentana, 0.1f, 1000.0f);
		}
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sky->DrawSkybox(vista, projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(vista));
		glUniform3f(uniformEyePosition, camPos.x, camPos.y, camPos.z);

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelaux2(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);


		//=====================================================================================
		// 4. ESCENARIO COMUN
		//=====================================================================================
		// ---- planetas: disco caminable en y = 0 y hemisferio de roca abajo ----
		// el profe pidio mundos mas grandes: radio y separacion al doble (17-oct)
		glm::vec3 centroPlaneta[3] = { glm::vec3(-90.0f, 0.0f, -36.0f), glm::vec3(90.0f, 0.0f, -36.0f), glm::vec3(0.0f, 0.0f, 86.0f) };
		float radioPlaneta[3] = { 28.0f, 28.0f, 26.0f };
		Texture* texPlaneta[3] = { &forjaTexture, &reachTexture, &venomTexture };
		for (int i = 0; i < 3; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, centroPlaneta[i]);
			model = glm::scale(model, glm::vec3(radioPlaneta[i], 1.0f, radioPlaneta[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			texPlaneta[i]->UseTexture();
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[DISCO]->RenderMeshModel();

			model = glm::mat4(1.0);
			model = glm::translate(model, centroPlaneta[i] - glm::vec3(0.0f, 0.02f, 0.0f));
			model = glm::scale(model, glm::vec3(radioPlaneta[i], radioPlaneta[i] * 0.7f, radioPlaneta[i]));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i == 0 ? glm::vec3(0.55f, 0.5f, 0.45f) : i == 1 ? glm::vec3(0.6f, 0.6f, 0.6f) : glm::vec3(0.6f, 0.4f, 0.3f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			rocaTexture.UseTexture();
			meshListModel[HEMISFERIO]->RenderMeshModel();
		}

		// ---- E2 pista de carreras: flota entre los planetas, sube y baja, con muros de luz y anillos de energia ----
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, PISTA_Z));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pistaTexture.UseTexture();
		metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[PISTA]->RenderMeshModel();
		// muros de las orillas: la franja de luz late despacio (color arriba de 1 para que brille aunque sea de noche)
		color = glm::vec3(1.0f, 1.0f, 1.0f) * (1.3f + 0.3f * sin(3.0f * now));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		neonTexture.UseTexture();
		meshListModel[MURO_PISTA]->RenderMeshModel();
		// soportes: 16 columnas delgadas debajo de la pista con un emisor de energia en la punta
		for (int i = 0; i < 16; i++) {
			float t = i * 2.0f * PI / 16.0f;
			glm::vec3 c = glm::vec3(PISTA_A * cos(t), alturaPista(t), PISTA_Z + PISTA_B * sin(t));
			model = glm::mat4(1.0);
			model = glm::translate(model, c - glm::vec3(0.0f, 7.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.6f, 6.7f, 0.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.35f, 0.38f, 0.45f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
			model = glm::mat4(1.0);
			model = glm::translate(model, c - glm::vec3(0.0f, 7.3f, 0.0f));
			model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f) * (1.0f + 0.15f * sin(4.0f * now + i)));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i % 2 ? glm::vec3(0.4f, 2.2f, 2.6f) : glm::vec3(2.4f, 0.5f, 1.8f);	// cian y magenta como la nebulosa
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
		}
		// 4 anillos de control: 20 segmentos alrededor de la pista que giran sobre la tangente
		for (int i = 0; i < 4; i++) {
			float t = i * PI / 2.0f + 0.4f;
			glm::vec3 c = glm::vec3(PISTA_A * cos(t), alturaPista(t) + 1.0f, PISTA_Z + PISTA_B * sin(t));
			float giro = glm::degrees(atan2(-PISTA_A * sin(t), PISTA_B * cos(t)));	// el eje Z local queda sobre la tangente
			for (int k = 0; k < 20; k++) {
				float fi = k * 360.0f / 20.0f + now * 25.0f * (i % 2 ? 1.0f : -1.0f);
				model = glm::mat4(1.0);
				model = glm::translate(model, c);
				model = glm::rotate(model, glm::radians(giro), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(fi), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::translate(model, glm::vec3(0.0f, 6.5f, 0.0f));
				model = glm::scale(model, glm::vec3(2.1f, 0.45f, 0.7f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = k % 2 ? glm::vec3(0.3f, 0.32f, 0.38f) : (i % 2 ? glm::vec3(0.4f, 2.0f, 2.4f) : glm::vec3(2.2f, 0.45f, 1.6f));
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshListModel[CUBO]->RenderMeshModel();
			}
		}
		// ---- AC2 naves N1, N2, N3 en la pista ----
		for (int i = 0; i < 3; i++) {
			float t = angNave[i], carril = (i - 1) * 2.4f;	// cada una en su carril
			glm::vec3 p = glm::vec3((PISTA_A + carril) * cos(t), alturaPista(t) + 0.9f + 0.15f * sin(6.0f * now + i), PISTA_Z + (PISTA_B + carril) * sin(t));	// flotan un poco
			float giro = glm::degrees(atan2(-PISTA_A * sin(t), PISTA_B * cos(t)));	// orientada con la tangente
			float pendiente = glm::degrees(atan2(5.0f * cos(2.0f * t), glm::length(glm::vec2(PISTA_A * sin(t), PISTA_B * cos(t)))));	// sube y baja con la pista
			model = glm::mat4(1.0);
			model = glm::translate(model, p);
			model = glm::rotate(model, glm::radians(giro), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-pendiente), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0f, 0.0f, 1.0f));	// se inclina hacia adentro de la curva
			// modelos de Kenney (Space Kit, CC0): craft_racer, craft_speederA y craft_speederB
			model = glm::scale(model, glm::vec3(2.4f, 2.4f, 2.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = colorNave[i];
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
			if (i == 0) naveRacer.RenderModel();
			else if (i == 1) naveSpeederA.RenderModel();
			else naveSpeederB.RenderModel();
		}
		// ---- E1 estacion espacial (jerarquia: nucleo -> anillo -> brazos -> bahias) ----
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 60.0f, -58.0f));
		model = glm::scale(model, glm::vec3(1.6f, 1.6f, 1.6f));	// crecio junto con los planetas
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.2f, 2.2f, 2.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.82f, 0.88f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalTexture.UseTexture();
		metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[ESFERA]->RenderMeshModel();
		model = modelaux;
		model = glm::rotate(model, glm::radians(giroAnillo), glm::vec3(0.0f, 1.0f, 0.0f));	// el anillo gira con todo y brazos
		modelaux2 = model;
		for (int i = 0; i < 16; i++) {	// anillo hecho de 16 bloques
			model = modelaux2;
			model = glm::rotate(model, glm::radians(i * 22.5f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.9f, 0.8f, 2.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[CUBO]->RenderMeshModel();
		}
		for (int i = 0; i < 4; i++) {	// cuatro brazos
			model = modelaux2;
			model = glm::rotate(model, glm::radians(i * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(3.5f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(4.0f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.55f, 0.58f, 0.62f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CUBO]->RenderMeshModel();
		}
		for (int i = 0; i < 2; i++) {	// dos bahias de acoplamiento, fijas debajo del nucleo
			model = modelaux;
			model = glm::translate(model, glm::vec3(i == 0 ? -4.0f : 4.0f, -2.5f, 4.0f));
			model = glm::scale(model, glm::vec3(2.5f, 1.5f, 3.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.35f, 0.6f, 0.85f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CUBO]->RenderMeshModel();
		}

		// ---- E3 plataforma de lanzamiento y cohete (AB1) ----
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-90.0f, 0.03f, -60.0f));
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.72f, 0.66f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[DISCO]->RenderMeshModel();
		// torre de servicio con su brazo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-85.5f, 0.0f, -60.0f));
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 6.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 13.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.45f, 0.45f, 0.48f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[CUBO]->RenderMeshModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 8.5f, 0.0f));
		model = glm::rotate(model, glm::radians(brazoServicio), glm::vec3(0.0f, 1.0f, 0.0f));	// se retira antes del despegue
		model = glm::translate(model, glm::vec3(-1.7f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshListModel[CUBO]->RenderMeshModel();
		// cohete: cuerpo -> nariz, 3 propulsores y 3 aletas
		if (faseCohete != 4) {
			model = glm::mat4(1.0);
			model = glm::translate(model, posCohete);
			model = glm::rotate(model, glm::radians(inclinaCohete), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(1.4f, 1.4f, 1.4f));	// mas grande ahora que el planeta crecio
			modelaux = model;
			model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.6f, 5.0f, 1.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.9f, 0.45f, 0.15f);	// naranja como la Aphelion
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 6.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.6f, 2.0f, 1.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.7f, 0.72f, 0.76f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CONO]->RenderMeshModel();
			for (int i = 0; i < 3; i++) {
				model = modelaux;
				model = glm::rotate(model, glm::radians(i * 120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				modelaux2 = model;
				model = glm::translate(model, glm::vec3(0.85f, 0.0f, 0.0f));	// propulsor
				model = glm::scale(model, glm::vec3(0.6f, 2.2f, 0.6f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshListModel[CILINDRO]->RenderMeshModel();
				model = modelaux2;
				model = glm::translate(model, glm::vec3(1.1f, 1.3f, 0.0f));	// aleta
				model = glm::scale(model, glm::vec3(1.0f, 1.6f, 0.12f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.9f, 0.45f, 0.15f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CUBO]->RenderMeshModel();
				color = glm::vec3(0.7f, 0.72f, 0.76f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			}
		}

		// ---- CE1 a CE5 cuerpos celestes ----
		glm::vec3 posCuerpo[5] = { glm::vec3(-46, 30, -8), glm::vec3(46, 34, -8), glm::vec3(-50, 24, 50), glm::vec3(50, 30, 54), glm::vec3(0, 42, -24) };
		float radioCuerpo[5] = { 5.0f, 3.6f, 2.6f, 4.2f, 3.0f };
		for (int i = 0; i < 5; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, posCuerpo[i]);
			model = glm::rotate(model, glm::radians(now * (8.0f + 3.0f * i)), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(radioCuerpo[i], radioCuerpo[i] * (i % 2 ? 0.8f : 1.0f), radioCuerpo[i]));	// los asteroides achatados
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.85f, 0.83f, 0.8f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			rocaTexture.UseTexture();
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[ESFERA]->RenderMeshModel();
		}
		// satelites (antenas de Kenney) cerca de la estacion, giran despacio
		glm::vec3 posSatelite[3] = { glm::vec3(22, 52, -44), glm::vec3(-26, 68, -70), glm::vec3(36, 58, -76) };
		for (int i = 0; i < 3; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, posSatelite[i]);
			model = glm::rotate(model, glm::radians(now * (12.0f + 5.0f * i)), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(4.5f, 4.5f, 4.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.9f, 0.92f, 0.95f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
			antenaSatelite.RenderModel();
		}
		// asteroides (meteoros de Kenney) entre los planetas
		glm::vec3 posMeteoro[6] = { glm::vec3(-20, 36, -12), glm::vec3(28, 26, 4), glm::vec3(12, 46, -36), glm::vec3(-8, 30, 60), glm::vec3(-60, 40, 20), glm::vec3(62, 44, 24) };
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, posMeteoro[i] + glm::vec3(0.0f, 1.5f * sin(0.5f * now + i), 0.0f));	// flotan despacio
			model = glm::rotate(model, glm::radians(now * (9.0f + 4.0f * i)), glm::vec3(0.3f, 1.0f, 0.2f));
			model = glm::scale(model, glm::vec3(4.5f + i * 0.6f, 4.5f + i * 0.6f, 4.5f + i * 0.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.55f, 0.5f, 0.45f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meteoro.RenderModel();
		}

		// ---- NV nave viajera (Pelican provisional) ----
		model = glm::mat4(1.0);
		model = glm::translate(model, posNV);
		model = glm::rotate(model, atan2(tanNV.x, tanNV.z), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.4f, 1.2f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.42f, 0.5f, 0.28f);	// verde olivo UNSC
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[CUBO]->RenderMeshModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -0.8f));
		model = glm::scale(model, glm::vec3(5.0f, 0.2f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshListModel[CUBO]->RenderMeshModel();


		//=====================================================================================
		// 5. P1 MUNDO FORJA (Ratchet & Clank) · 419049898
		// centro (-90, 0, -36), radio 28; coordenadas locales: norte = plataforma, este = refineria,
		// oeste = corral de Protopets, centro = vendedor y cajas, sur = garage, figura y plataformas flotantes
		//=====================================================================================
		glm::vec3 cMF = glm::vec3(-90.0f, 0.0f, -36.0f);
		// MF-01 torres de refineria x3: chimenea de Kenney (Space Kit, CC0) con un generador al lado
		glm::vec3 refineria[3] = { glm::vec3(16, 0, -6), glm::vec3(21, 0, 1), glm::vec3(15, 0, 8) };
		for (int i = 0; i < 3; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + refineria[i]);
			modelaux = model;
			model = glm::scale(model, glm::vec3(6.5f + 0.7f * i, 6.5f + 0.7f * i, 6.5f + 0.7f * i));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.62f, 0.6f, 0.5f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			chimenea.RenderModel();
			model = modelaux;	// generador junto a la chimenea
			model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.85f, 0.5f, 0.2f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			generador.RenderModel();
		}
		// tuberias entre las torres (cilindro acostado de una torre a la siguiente) y dos tanques
		for (int i = 0; i < 2; i++) {
			glm::vec3 a = cMF + refineria[i] + glm::vec3(0.0f, 3.5f + i, 0.0f), b = cMF + refineria[i + 1] + glm::vec3(0.0f, 3.5f + i, 0.0f);
			glm::vec3 d = b - a;
			model = glm::mat4(1.0);
			model = glm::translate(model, a);
			model = glm::rotate(model, atan2(d.x, d.z), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.9f, glm::length(d), 0.9f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.55f, 0.52f, 0.46f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalTexture.UseTexture();
			meshListModel[CILINDRO]->RenderMeshModel();
		}
		glm::vec3 tanque[2] = { glm::vec3(25, 0, -12), glm::vec3(25, 0, 12) };
		for (int i = 0; i < 2; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + tanque[i]);
			modelaux = model;
			model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.7f, 0.68f, 0.6f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CILINDRO]->RenderMeshModel();
			model = modelaux;	// tapa redonda
			model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.5f, 1.2f, 2.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[ESFERA]->RenderMeshModel();
			model = modelaux;	// franja naranja de peligro
			model = glm::translate(model, glm::vec3(0.0f, 2.2f, 0.0f));
			model = glm::scale(model, glm::vec3(5.1f, 0.6f, 5.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.95f, 0.5f, 0.1f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CILINDRO]->RenderMeshModel();
		}

		// MF-03 grind rails x2: curvas cuadraticas (A -> control C -> B) hechas con tramos de cilindro
		glm::vec3 rielA[2] = { glm::vec3(-24, 1.6f, -8), glm::vec3(4, 1.6f, 22) };
		glm::vec3 rielC[2] = { glm::vec3(-18, 1.6f, -26), glm::vec3(18, 1.6f, 28) };
		glm::vec3 rielB[2] = { glm::vec3(-6, 1.6f, -16), glm::vec3(24, 1.6f, 13) };
		for (int r = 0; r < 2; r++) {
			for (int k = 0; k < 30; k++) {
				float t1 = k / 30.0f, t2 = (k + 1) / 30.0f;
				glm::vec3 q1 = (1 - t1) * (1 - t1) * rielA[r] + 2 * (1 - t1) * t1 * rielC[r] + t1 * t1 * rielB[r];
				glm::vec3 q2 = (1 - t2) * (1 - t2) * rielA[r] + 2 * (1 - t2) * t2 * rielC[r] + t2 * t2 * rielB[r];
				glm::vec3 d = q2 - q1;
				model = glm::mat4(1.0);
				model = glm::translate(model, cMF + q1);
				model = glm::rotate(model, atan2(d.x, d.z), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// el cilindro nace parado, lo acuesto
				model = glm::scale(model, glm::vec3(0.3f, glm::length(d), 0.3f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.85f, 0.7f, 0.2f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshListModel[CILINDRO]->RenderMeshModel();
				if (k % 5 == 0) {	// poste cada pocos tramos
					model = glm::mat4(1.0);
					model = glm::translate(model, cMF + glm::vec3(q1.x, 0.0f, q1.z));
					model = glm::scale(model, glm::vec3(0.2f, 1.6f, 0.2f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(0.35f, 0.3f, 0.2f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CILINDRO]->RenderMeshModel();
				}
			}
		}
		// MF-04 tornillos x12 con AC1: 8 sobre los rieles y 4 en el centro; giran en Y a 90 grados/s y flotan con un seno desfasado
		glm::vec3 tornillo[12];
		for (int r = 0; r < 2; r++)
			for (int k = 0; k < 4; k++) {
				float t = 0.15f + 0.23f * k;
				tornillo[r * 4 + k] = (1 - t) * (1 - t) * rielA[r] + 2 * (1 - t) * t * rielC[r] + t * t * rielB[r] + glm::vec3(0.0f, -0.4f, 0.0f);
			}
		tornillo[8] = glm::vec3(2, 1.2f, -4); tornillo[9] = glm::vec3(5, 1.2f, -1); tornillo[10] = glm::vec3(-1, 1.2f, -7); tornillo[11] = glm::vec3(8, 1.2f, -5);
		for (int i = 0; i < 12; i++) {
			float y = tornillo[i].y + 0.25f * sin(2.0f * PI * now / 2.0f + i * 0.7f);
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + glm::vec3(tornillo[i].x, y, tornillo[i].z));
			model = glm::rotate(model, glm::radians(90.0f * now), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.95f, 0.75f, 0.1f);	// dorado
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[TORNILLO]->RenderMeshModel();
		}
		// MF-05 cajas x8 junto al vendedor (de tornillos naranjas y de nanotech azules)
		glm::vec3 caja[8] = { glm::vec3(-9, 0, 0), glm::vec3(-9, 0, 2), glm::vec3(-9, 1.1f, 1), glm::vec3(-10.5f, 0, 4), glm::vec3(-7.5f, 0, 5),
			glm::vec3(1, 0, 3), glm::vec3(2.3f, 0, 4.6f), glm::vec3(1.4f, 1.1f, 3.6f) };
		for (int i = 0; i < 8; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + caja[i] + glm::vec3(0.0f, 0.55f, 0.0f));
			model = glm::rotate(model, glm::radians(i * 23.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = i % 2 ? glm::vec3(0.85f, 0.48f, 0.12f) : glm::vec3(0.25f, 0.6f, 0.85f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CUBO]->RenderMeshModel();
		}
		// MF-06 vendedor Gadgetron (modelo del juego, mide 66 de alto, lo dejo en ~6)
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + glm::vec3(-4.0f, 0.0f, 2.0f));
		model = glm::rotate(model, glm::radians(giroVendedor), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.09f, 0.09f, 0.09f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		vendedorGadgetron.RenderModel();

		// MF-07 torretas x2 junto a la plataforma (jerarquia: base -> torre en Y -> cañon en X); la de la izquierda hace AB2
		glm::vec3 torreta[2] = { cMF + glm::vec3(-8.0f, 0.0f, -20.0f), cMF + glm::vec3(8.0f, 0.0f, -20.0f) };
		for (int i = 0; i < 2; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, torreta[i]);
			modelaux = model;
			model = glm::scale(model, glm::vec3(1.8f, 1.0f, 1.8f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.42f, 0.42f, 0.42f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 1.6f, 0.0f));
			model = glm::rotate(model, glm::radians(i == 0 ? giroTorre : 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			modelaux = model;
			model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.6f, 0.18f, 0.18f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.5f - (i == 0 ? retroceso : 0.0f)));
			model = glm::rotate(model, glm::radians(90.0f - (i == 0 ? angCanon : 0.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.35f, 1.8f, 0.35f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.25f, 0.25f, 0.27f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CILINDRO]->RenderMeshModel();
		}
		if (faseTorreta == 2) {	// proyectil de AB2
			model = glm::mat4(1.0);
			model = glm::translate(model, posBala);
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 0.85f, 0.3f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
		}

		// MF-08 plataformas flotantes x3 afuera de la orilla sur: suben y bajan con un seno y giran despacio
		for (int i = 0; i < 3; i++) {
			glm::vec3 p = cMF + glm::vec3(-10.0f + 10.0f * i, 2.5f + 0.9f * sin(0.8f * now + i * 2.0f), 33.0f + (i == 1 ? 4.0f : 0.0f));
			model = glm::mat4(1.0);
			model = glm::translate(model, p);
			model = glm::rotate(model, glm::radians(10.0f * now + i * 40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			modelaux = model;
			model = glm::scale(model, glm::vec3(7.0f, 0.6f, 7.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.5f, 0.52f, 0.55f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalTexture.UseTexture();
			metalNave.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
			for (int k = 0; k < 8; k++) {	// orilla de franjas amarillas y negras
				model = modelaux;
				model = glm::rotate(model, glm::radians(k * 45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::translate(model, glm::vec3(3.4f, 0.6f, 0.0f));
				model = glm::scale(model, glm::vec3(0.3f, 0.25f, 2.6f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = k % 2 ? glm::vec3(0.12f, 0.12f, 0.12f) : glm::vec3(0.95f, 0.8f, 0.1f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				meshListModel[CUBO]->RenderMeshModel();
			}
			model = modelaux;	// propulsor que brilla abajo
			model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
			model = glm::scale(model, glm::vec3(1.2f, 0.5f, 1.2f) * (1.0f + 0.1f * sin(8.0f * now + i)));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.6f, 2.0f, 2.4f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
		}

		// MF-09 Blargs x4 trabajando en la refineria (jerarquia: cadera -> torso -> cabeza con pico; brazos que martillan; piernas)
		glm::vec3 blarg[4] = { glm::vec3(10, 0, -10), glm::vec3(11, 0, 3), glm::vec3(20, 0, 13), glm::vec3(24, 0, -4) };
		float giroBlarg[4] = { 60.0f, 90.0f, 160.0f, -90.0f };
		for (int i = 0; i < 4; i++) {
			float golpe = 45.0f * sin(5.0f * now + i * 1.3f);	// el brazo derecho sube y baja con la llave
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + blarg[i] + glm::vec3(0.0f, 1.1f, 0.0f));
			model = glm::rotate(model, glm::radians(giroBlarg[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			modelaux = model;	// cadera
			for (int lado = -1; lado <= 1; lado += 2) {	// piernas
				model = modelaux;
				model = glm::translate(model, glm::vec3(lado * 0.3f, -1.1f, 0.0f));
				model = glm::scale(model, glm::vec3(0.35f, 1.1f, 0.35f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.25f, 0.28f, 0.36f);	// overol
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshListModel[CILINDRO]->RenderMeshModel();
			}
			model = modelaux;	// torso
			model = glm::translate(model, glm::vec3(0.0f, 0.6f, 0.0f));
			modelaux2 = model;
			model = glm::scale(model, glm::vec3(0.8f, 1.0f, 0.6f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.3f, 0.34f, 0.44f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
			model = modelaux2;	// cabeza
			model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.1f));
			glm::mat4 cabeza = model;
			model = glm::scale(model, glm::vec3(0.4f, 0.45f, 0.45f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.45f, 0.62f, 0.32f);	// piel verde
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
			model = cabeza;	// pico largo hacia el frente
			model = glm::translate(model, glm::vec3(0.0f, -0.05f, 0.3f));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.35f, 0.8f, 0.25f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[CONO]->RenderMeshModel();
			for (int lado = -1; lado <= 1; lado += 2) {	// ojos
				model = cabeza;
				model = glm::translate(model, glm::vec3(lado * 0.2f, 0.15f, 0.3f));
				model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(1.0f, 0.85f, 0.2f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[ESFERA]->RenderMeshModel();
			}
			for (int lado = -1; lado <= 1; lado += 2) {	// brazos: el derecho martilla, el izquierdo queda quieto
				model = modelaux2;
				model = glm::translate(model, glm::vec3(lado * 0.85f, 0.7f, 0.0f));
				model = glm::rotate(model, glm::radians(lado > 0 ? -60.0f + golpe : 10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				glm::mat4 brazo = model;
				model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
				model = glm::scale(model, glm::vec3(0.25f, 0.9f, 0.25f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.45f, 0.62f, 0.32f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CILINDRO]->RenderMeshModel();
				if (lado > 0) {	// llave en la mano
					model = brazo;
					model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.3f));
					model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.8f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(0.6f, 0.6f, 0.62f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
					meshListModel[CUBO]->RenderMeshModel();
					concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
				}
			}
		}

		// MF-10 Protopets x3 en el oeste: dos normales y uno mutado (mas grande); brincan con |sin| y se aplastan al caer
		glm::vec3 protopet[3] = { glm::vec3(-20, 0, -1), glm::vec3(-23, 0, 4), glm::vec3(-18, 0, 7) };
		for (int i = 0; i < 3; i++) {
			bool mutado = i == 2;
			float tam = mutado ? 1.8f : 0.8f;
			float brinco = fabs(sin(3.5f * now + i * 1.7f));
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + protopet[i] + glm::vec3(0.0f, tam * (1.0f + 0.9f * brinco), 0.0f));
			model = glm::rotate(model, glm::radians(30.0f * sin(0.7f * now + i)), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(tam, tam * (0.85f + 0.15f * brinco), tam));
			modelaux = model;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = mutado ? glm::vec3(0.6f, 0.2f, 0.45f) : glm::vec3(0.98f, 0.66f, 0.2f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[ESFERA]->RenderMeshModel();
			for (int lado = -1; lado <= 1; lado += 2) {
				model = modelaux;	// ojo grande
				model = glm::translate(model, glm::vec3(lado * 0.35f, 0.3f, 0.8f));
				model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = mutado ? glm::vec3(1.0f, 0.3f, 0.2f) : glm::vec3(1.0f, 1.0f, 1.0f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[ESFERA]->RenderMeshModel();
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.7f));	// pupila
				model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.05f, 0.05f, 0.05f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[ESFERA]->RenderMeshModel();
				model = modelaux;	// cuernito
				model = glm::translate(model, glm::vec3(lado * 0.45f, 0.75f, 0.0f));
				model = glm::rotate(model, glm::radians(-lado * 25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(0.25f, 0.5f, 0.25f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = mutado ? glm::vec3(0.9f, 0.9f, 0.8f) : glm::vec3(0.75f, 0.4f, 0.12f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CONO]->RenderMeshModel();
			}
			if (mutado)	// el mutado tiene dientes
				for (int k = -2; k <= 2; k++) {
					model = modelaux;
					model = glm::translate(model, glm::vec3(k * 0.15f, -0.25f, 0.9f));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.1f, 0.2f, 0.1f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					color = glm::vec3(1.0f, 1.0f, 0.95f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					meshListModel[CONO]->RenderMeshModel();
				}
		}
		// cerca del corral de los Protopets: postes en circulo
		for (int k = 0; k < 18; k++) {
			float a = k * 2.0f * PI / 18.0f;
			model = glm::mat4(1.0);
			model = glm::translate(model, cMF + glm::vec3(-20.5f + 6.5f * cos(a), 0.0f, 3.0f + 6.5f * sin(a)));
			model = glm::scale(model, glm::vec3(0.2f, 1.2f, 0.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.5f, 0.45f, 0.35f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
		}

		// MF-12 garage de Ratchet: caja con techo de medio cilindro, porton abierto y franjas naranjas
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + glm::vec3(-12.0f, 0.0f, 17.0f));
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 4.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.78f, 0.74f, 0.66f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalTexture.UseTexture();
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[CUBO]->RenderMeshModel();
		model = modelaux;	// techo: cilindro acostado de lado a lado, medio enterrado en la caja
		model = glm::translate(model, glm::vec3(-4.0f, 4.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(7.0f, 8.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.6f, 0.62f, 0.66f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[CILINDRO]->RenderMeshModel();
		model = modelaux;	// porton (hueco oscuro en el frente)
		model = glm::translate(model, glm::vec3(0.0f, 1.6f, 3.52f));
		model = glm::scale(model, glm::vec3(5.0f, 3.2f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.08f, 0.08f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		meshListModel[CUBO]->RenderMeshModel();
		for (int k = 0; k < 2; k++) {	// franjas naranjas a los lados del porton
			model = modelaux;
			model = glm::translate(model, glm::vec3(k == 0 ? -3.2f : 3.2f, 2.0f, 3.53f));
			model = glm::scale(model, glm::vec3(0.6f, 4.0f, 0.05f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.95f, 0.5f, 0.12f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CUBO]->RenderMeshModel();
		}

		// MF-11 figura de Ratchet y Clank en su pedestal; con H despega y vuela con el Heli-Pack (KF1) y regresa al pedestal
		glm::vec3 pedestal = glm::vec3(2.0f, 0.0f, 14.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + pedestal);
		model = glm::scale(model, glm::vec3(2.6f, 0.6f, 2.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.95f, 0.75f, 0.15f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[CILINDRO]->RenderMeshModel();
		bool volando = kfReproduciendo || (kfCuadro > 0 && kfCuadro < 8);
		model = glm::mat4(1.0);
		model = glm::translate(model, cMF + pedestal + kfActual + glm::vec3(0.0f, 0.6f, 0.0f));
		model = glm::rotate(model, glm::radians(kfGiroActual + giroBaseRatchet), glm::vec3(0.0f, 1.0f, 0.0f));
		if (volando) model = glm::rotate(model, glm::radians(8.0f * sin(3.0f * now)), glm::vec3(1.0f, 0.0f, 0.0f));	// se balancea en el aire
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
		ratchetModelo.RenderModel();
		// Clank en la espalda (hijo de Ratchet): cuerpo, cabeza, antena y, si va volando, el Heli-Pack girando
		model = modelaux;
		model = glm::translate(model, clankEspalda);
		glm::mat4 clank = model;
		model = glm::scale(model, glm::vec3(0.22f, 0.3f, 0.26f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.78f, 0.82f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		metalPulido.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[CUBO]->RenderMeshModel();
		model = clank;
		model = glm::translate(model, glm::vec3(0.0f, 0.27f, 0.0f));
		glm::mat4 cabezaClank = model;
		model = glm::scale(model, glm::vec3(0.2f, 0.17f, 0.28f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshListModel[ESFERA]->RenderMeshModel();
		model = cabezaClank;	// antena con la bolita roja
		model = glm::translate(model, glm::vec3(0.0f, 0.15f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.15f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshListModel[CILINDRO]->RenderMeshModel();
		model = cabezaClank;
		model = glm::translate(model, glm::vec3(0.0f, 0.32f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.6f, 0.2f, 0.15f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[ESFERA]->RenderMeshModel();
		if (volando)
			for (int k = 0; k < 2; k++) {	// aspas del Heli-Pack: giran rapido en Y sobre la cabeza de Clank
				model = cabezaClank;
				model = glm::translate(model, glm::vec3(0.0f, 0.25f, 0.0f));
				model = glm::rotate(model, glm::radians(now * 1080.0f + k * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.1f, 0.02f, 0.09f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.95f, 0.8f, 0.2f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				meshListModel[CUBO]->RenderMeshModel();
			}
		// pendiente en Mundo Forja: audio AMB-MF y SFX1 a SFX4 (en teoria, con irrKlang)

		//=====================================================================================
		// 6. P2 REACH (Halo) · 317136421
		// centro (45, 0, -18); coordenadas locales de la lamina 3. Por ahora son bloques en su lugar,
		// se cambian por los modelos: HR-01 a HR-14, AB3 Pelican (P), AC3 antena, AC4 escudos, AC5 plataformas MAC
		//=====================================================================================
		glm::vec3 cHR = glm::vec3(90.0f, 0.0f, -36.0f);	// (los planetas crecieron al doble: las posiciones locales van al doble)
		// HR-01 base UNSC: hangar de Kenney (Space Kit, CC0) mientras llega el modelo de Blender
		model = glm::mat4(1.0);
		model = glm::translate(model, cHR + glm::vec3(-6.0f, 0.0f, -8.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// el lado largo del hangar va a lo ancho
		model = glm::scale(model, glm::vec3(3.6f, 3.6f, 3.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.62f, 0.65f, 0.63f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		concreto.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hangarReach.RenderModel();
		plainTexture.UseTexture();
		// HR-02 barricadas x4
		glm::vec2 barricada[4] = { glm::vec2(-16, -6), glm::vec2(-16, 6), glm::vec2(0, 6), glm::vec2(6, -4) };
		for (int i = 0; i < 4; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cHR + glm::vec3(barricada[i].x, 0.4f, barricada[i].y));
			model = glm::scale(model, glm::vec3(2.0f, 0.8f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshListModel[CUBO]->RenderMeshModel();
		}
		// HR-04a pinos x6
		glm::vec2 pino[6] = { glm::vec2(-18, -18), glm::vec2(-10, -22), glm::vec2(10, -20), glm::vec2(20, -14), glm::vec2(-20, 14), glm::vec2(14, 20) };
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cHR + glm::vec3(pino[i].x, 0.0f, pino[i].y));
			modelaux = model;
			model = glm::scale(model, glm::vec3(0.4f, 1.5f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.4f, 0.28f, 0.16f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CILINDRO]->RenderMeshModel();
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.0f));
			model = glm::scale(model, glm::vec3(2.4f, 4.5f, 2.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.18f, 0.42f, 0.24f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CONO]->RenderMeshModel();
		}
		// HR-09 zona de aterrizaje
		model = glm::mat4(1.0);
		model = glm::translate(model, cHR + glm::vec3(14.0f, 0.03f, 6.0f));
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.72f, 0.74f, 0.68f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[DISCO]->RenderMeshModel();
		// pendiente en Reach: HR-03 torre + antena (AC3), HR-05 Warthog, HR-06 Pelican (AB3), HR-07 cajas, HR-08 escudos (AC4),
		// HR-10 plataformas MAC (AC5), NPCs HR-11 a HR-14, audio AMB-HR y SFX5


		//=====================================================================================
		// 7. P3 VENOM (Star Fox 64) · 318337872
		// centro (0, 0, 43); coordenadas locales de la lamina 4. Por ahora son bloques en su lugar,
		// se cambian por los modelos: SF-01 a SF-11, AB4 Arwing (F), KF2 acoplamiento en la estacion (N)
		//=====================================================================================
		glm::vec3 cSF = glm::vec3(0.0f, 0.0f, 86.0f);	// (posiciones locales al doble, como en los otros planetas)
		// SF-01 base de Andross (domo)
		model = glm::mat4(1.0);
		model = glm::translate(model, cSF + glm::vec3(0.0f, 0.0f, 12.0f));
		model = glm::scale(model, glm::vec3(5.5f, 4.5f, 5.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// el hemisferio es el de abajo, lo volteo
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.55f, 0.24f, 0.17f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		metalTexture.UseTexture();
		metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[HEMISFERIO]->RenderMeshModel();
		// SF-06 columnas de roca x6
		glm::vec2 columna[6] = { glm::vec2(-20, -12), glm::vec2(-8, -18), glm::vec2(8, -20), glm::vec2(20, -10), glm::vec2(-22, 12), glm::vec2(22, 14) };
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cSF + glm::vec3(columna[i].x, 0.0f, columna[i].y));
			model = glm::rotate(model, glm::radians(i * 40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.4f, 7.0f + (i % 3) * 2.0f, 1.8f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.7f, 0.48f, 0.3f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			rocaTexture.UseTexture();
			roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
		}
		// SF-02 torres defensivas x3
		glm::vec2 torreSF[3] = { glm::vec2(-16, 2), glm::vec2(16, 2), glm::vec2(10, 18) };
		for (int i = 0; i < 3; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cSF + glm::vec3(torreSF[i].x, 0.0f, torreSF[i].y));
			model = glm::rotate(model, atan2(-torreSF[i].x, 12.0f - torreSF[i].y), glm::vec3(0.0f, 1.0f, 0.0f));	// apuntan hacia la base
			model = glm::scale(model, glm::vec3(3.5f, 3.5f, 3.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.45f, 0.45f, 0.5f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			torretaDoble.RenderModel();	// modelo de Kenney (Space Kit, CC0)
		}
		plainTexture.UseTexture();
		// SF-07 contenedores x6
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, cSF + glm::vec3(-12.0f + (i % 3) * 2.0f, 0.5f, i < 3 ? -8.0f : -10.0f));
			model = glm::scale(model, glm::vec3(1.6f, 1.0f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.25f, 0.36f, 0.24f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[CUBO]->RenderMeshModel();
		}
		// pendiente en Venom: SF-03 Arwing (AB4), SF-04 Wolfen II, SF-05 entrada a tuneles, NPCs SF-08 a SF-11,
		// KF2 en la estacion, audio AMB-SF, SFX6 y SFX7


		// ---- postes de las luminarias L1 a L6 (el foco brilla de noche) ----
		for (int i = 0; i < 6; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, posLuminaria[i]);
			model = glm::scale(model, glm::vec3(0.2f, 4.0f, 0.2f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.25f, 0.25f, 0.25f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			metalPintado.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshListModel[CILINDRO]->RenderMeshModel();
			model = glm::mat4(1.0);
			model = glm::translate(model, posLuminaria[i] + glm::vec3(0.0f, 4.1f, 0.0f));
			model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			// de noche el foco se ve encendido (color mas fuerte para que resalte con la luz ambiental baja)
			color = esDeNoche ? glm::vec3(4.0f, 4.0f, 3.0f) : glm::vec3(0.8f, 0.8f, 0.7f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			meshListModel[ESFERA]->RenderMeshModel();
		}


		//=====================================================================================
		// 8. AVATAR (pendiente: el equipo va a definir el personaje)
		// jerarquico, no humanoide, con textura + material adicional y caminata con A sin(wt + fase)
		//=====================================================================================


		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
