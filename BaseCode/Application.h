#ifndef _APPLICATION_INCLUDE
#define _APPLICATION_INCLUDE


#include "Scene.h"


// Application is a singleton (a class with a single instance) that represents our whole app


class Application
{

public:
	Application() {}
	
	
	static Application &instance()
	{
		static Application G;
	
		return G;
	}
	
	void init();
	bool loadMesh(const char *filename);
	bool update(int deltaTime);
	void render();
	
	void resize(int width, int height);
	
	// Input callback methods
	void keyPressed(int key);
	void keyReleased(int key);
	void specialKeyPressed(int key);
	void specialKeyReleased(int key);
	void mouseMove(int x, int y);
	void mousePress(int button);
	void mouseRelease(int button);
	
	bool getKey(int key) const;
	bool getSpecialKey(int key) const;

	bool bNavigation;					// Navigation enabled?
	float fps = 0.0f;						// Initialize FPS

private:
	bool bPlay;							// Continue?
	Scene scene;						// Scene to render
	bool keys[256], specialKeys[256];	// Store key states so that 
										// we can have access at any time

	glm::ivec2 previousMousePos; 		// Previous mouse position
	bool mouseButtons[3];				// State of mouse buttons

	bool bFullscreen; 					// Fullscreen enabled?
	glm::ivec2 windowSize; 				// Window size before going fullscreen
	           
	int frameCount = 0;					// Frame counter for FPS
	float totalTime = 0.0f;				// Total time for FPS

	double TPS, maxCost; 				// Triangles per Second, Max Cost
};


#endif // _APPLICATION_INCLUDE


