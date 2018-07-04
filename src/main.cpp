#ifdef _MSC_VER
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include "ugine.h"
#include <glfw3.h>
#include <iostream>

#define FULLSCREEN false
#define WIDTH 800
#define HEIGHT 600

struct PlayerData {
	World&  world;
	Vec2    mouseCoords;
	Vec2    screenSize;

	PlayerData(World& world) : world(world) {}
};

Vec2 centerInScreen(const Vec2& pos, const Vec2& screenSize, const Vec2& worldMin, const Vec2& worldMax) {
	Vec2 centerPos = pos - screenSize / 2;
	centerPos.x = std::min(std::max(centerPos.x, worldMin.x), worldMax.x - screenSize.x);
	centerPos.y = std::min(std::max(centerPos.y, worldMin.y), worldMax.y - screenSize.y);
	return centerPos;
}

void updatePlayer(Sprite& sprite, float deltaTime) {
	PlayerData& playerData = *reinterpret_cast<PlayerData*>(sprite.getUserData());
	// movemos el sprite
	Vec2 dirVec = playerData.mouseCoords + playerData.world.getCameraPosition() - sprite.getPosition();
	Vec2 destDir;
	if (dirVec.length() >= 1) {
		destDir = dirVec.norm();
		sprite.setPosition(sprite.getPosition() + destDir * 128 * deltaTime);
	}

	// rotamos el sprite
	float destAngle = 0;
	if (destDir.x > 0) destAngle = -15;
	else if (destDir.x < 0) destAngle = 15;
	if (destAngle > sprite.getAngle()) {
		sprite.setAngle(sprite.getAngle() + 32 * deltaTime);
		if (sprite.getAngle() > destAngle) sprite.setAngle(destAngle);
	}
	else {
		sprite.setAngle(sprite.getAngle() - 32 * deltaTime);
		if (sprite.getAngle() < destAngle) sprite.setAngle(destAngle);
	}

	// centramos la camara en el sprite
	playerData.world.setCameraPosition(centerInScreen(sprite.getPosition(), playerData.screenSize, Vec2(0, 0), Vec2(static_cast<float>(playerData.world.getBackground(0)->width), static_cast<float>(playerData.world.getBackground(0)->height))));
}

int main() {
	// inicializamos glfw
	if (!glfwInit()) {
		std::cout << "no se ha podido inicializar glfw" << std::endl;
		return -1;
	}

	// creamos ventana
#if FULLSCREEN
	GLFWwindow* window = glfwCreateWindow(glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, "", glfwGetPrimaryMonitor(), nullptr);
#else
	glfwWindowHint(GLFW_RESIZABLE, true);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Scroll Paralax", nullptr, nullptr);
#endif
	if (!window) {
		std::cout << "no se pudo crear la ventana" << std::endl;
		glfwTerminate();
		return -1;
	}

	// activamos contexto de opengl
	glfwMakeContextCurrent(window);

	// obtenemos dimensiones de la pantalla
	int screenWidth, screenHeight;
	glfwGetWindowSize(window, &screenWidth, &screenHeight);

	// inicializamos litegfx
	lgfx_setup2d(screenWidth, screenHeight);

	// cargamos texturas
	ltex_t* spriteTex = loadTexture("data/bee_anim.png");
	ltex_t* back0 = loadTexture("data/level.png");
	ltex_t* back1 = loadTexture("data/trees1.png");
	ltex_t* back2 = loadTexture("data/trees2.png");
	ltex_t* clouds = loadTexture("data/clouds.png");

	// creamos mundo
	World world(0.87f, 0.83f, 0.87f, back0, back1, back2, clouds);
	world.setScrollRatio(1, 0.8f);
	world.setScrollRatio(2, 0.6f);
	world.setScrollRatio(3, 0.4f);
	world.setScrollSpeed(3, Vec2(-16, -8));

	// creamos datos del jugador
	PlayerData playerData(world);

	// creamos sprite
	Sprite sprite(spriteTex, 8);
	sprite.setUserData(&playerData);
	sprite.setCallback(updatePlayer);
	sprite.setFps(8);
	world.addSprite(sprite);

	// bucle principal
	double lastTime = glfwGetTime();
	float deltaTime = 0.0f;
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		// actualizamos delta time
		deltaTime = static_cast<float>(glfwGetTime() - lastTime);
		lastTime = glfwGetTime();

		// actualizamos dimensiones de la pantalla
		glfwGetWindowSize(window, &screenWidth, &screenHeight);
		//playerData.screenSize = Vec2(screenWidth, screenHeight);
		playerData.screenSize = Vec2(800, 600); // realmente, utilizamos la resolucion virtual como tamaño de pantalla

		// obtenemos coordenadas del raton
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		playerData.mouseCoords = Vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		playerData.mouseCoords = playerData.mouseCoords * playerData.screenSize / Vec2(static_cast<float>(screenWidth), static_cast<float>(screenHeight)); // ajustamos coordenadas a la resolucion virtual

		// actualizamos el mundo
		world.update(deltaTime);

		// establecemos viewport
		lgfx_setviewport(0, 0, screenWidth, screenHeight);
		lgfx_setresolution(800, 600); // establecemos resolucion virtual fija

		// dibujamos el mundo
		world.draw(playerData.screenSize);

		// intercambiamos buffers
		glfwSwapBuffers(window);

		// actualizamos eventos
		glfwPollEvents();
	}

	// terminamos
	glfwTerminate();
	return 0;
}
