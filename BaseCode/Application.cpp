#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Application.h"


#define MOVEMENT_DELTA 0.015f
#define ROTATION_DELTA_HEAD 0.30f
#define ROTATION_DELTA_PITCH 0.30f


// Initialize GL and the attributes of Application
void Application::init()
{
	bPlay = true;
	glClearColor(0.04f, 0.05f, 0.28f, 1.0f); 	// Background = Blue-ish color
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	// Initialize the scene object
	scene.init();
	
	// State attributes needed to track keyboard & mouse
	for(unsigned int i=0; i<256; i++)
	{
	  keys[i] = false;
	  specialKeys[i] = false;
	}
	mouseButtons[0] = false;
	mouseButtons[1] = false;
	bNavigation = false;
	
	previousMousePos = glm::ivec2(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	glutWarpPointer(previousMousePos.x, previousMousePos.y);
}

// Load the mesh into the scene
bool Application::loadMesh(const char *filename)
{
  return scene.loadMesh(filename);
}

// Update any animations or state in the scene
// Also move using the keyboard if in navigation mode
bool Application::update(int deltaTime)
{
	// Update scene time and fps
	scene.update(deltaTime);
	scene.sceneFps = fps;

	// Update the frame counter and total time
	frameCount++;
	totalTime += deltaTime / 1000.0f;

	// Compute the FPS of the application
	if (totalTime >= 1.0f)
	{
		// Compute TPS (Triangles per second)
		TPS = calculateTPS(scene.tc_models, totalTime);
		scene.TPS_display = TPS;
		//cout << "Total Triangles per Second: " + to_string(TPS) << endl;

		// Compute Max Cost (TPS / FPS)
		maxCost = calculateMaxCost(TPS, fps);
		scene.maxCost_display = maxCost;
		//cout << "Max Cost:" + to_string(maxCost) << endl;

		// Reset fps, frame count, total time
		fps = static_cast<float>(frameCount) / totalTime;
		frameCount = 0;
		totalTime = 0.0f;
	}
	
	if(bNavigation)
	{
		if(specialKeys[GLUT_KEY_UP] || keys['W'] || keys['w'])
			scene.getCamera().moveForward(MOVEMENT_DELTA);
		if(specialKeys[GLUT_KEY_DOWN] || keys['S'] || keys['s'])
			scene.getCamera().moveForward(-MOVEMENT_DELTA);
		if(specialKeys[GLUT_KEY_RIGHT] || keys['D'] || keys['d'])
			scene.getCamera().strafe(MOVEMENT_DELTA);
		if(specialKeys[GLUT_KEY_LEFT] || keys['A'] || keys['a'])
			scene.getCamera().strafe(-MOVEMENT_DELTA);
		if(keys['Q'] || keys['q'])
			scene.getCamera().moveUpward(MOVEMENT_DELTA);
		if (keys['E'] || keys['e'])
			scene.getCamera().moveUpward(-MOVEMENT_DELTA);
	}
	
	return bPlay;
}

// Render the scene and warp the mouse pointer to the center
// so that it remains inside the window as much as possible
void Application::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
	
	if(bNavigation)
	{
		previousMousePos = glm::ivec2(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		glutWarpPointer(previousMousePos.x, previousMousePos.y);
	}
}

// Resizing the window should change the size GL viewport
// as well as adapt the camera to the new aspect ratio
void Application::resize(int width, int height)
{
  glViewport(0, 0, width, height);
  scene.getCamera().resizeCameraViewport(width, height);
}

// Process keyboard events. Escape exits the application
void Application::keyPressed(int key)
{
	if (key == 27) // Escape code
	{
		bPlay = false;
		cout << "Final FPS: " + std::to_string(fps) << endl;
	}

	keys[key] = true;
}

void Application::keyReleased(int key)
{
	keys[key] = false;
}

void Application::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

// F1 enters and exits the navigation mode
// F5 does the same for fullscreen mode
void Application::specialKeyReleased(int key)
{
	specialKeys[key] = false;
	if(key == GLUT_KEY_F1)
	{
		bNavigation = !bNavigation;
		if(bNavigation)
		{
			previousMousePos = glm::ivec2(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
			glutWarpPointer(previousMousePos.x, previousMousePos.y);
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		else
			glutSetCursor(GLUT_CURSOR_INHERIT);
	}
	if(key == GLUT_KEY_F5)
	{
		bFullscreen = !bFullscreen;
		if(bFullscreen)
		{
			windowSize = glm::ivec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			glutFullScreen();
		}
		else
			glutReshapeWindow(windowSize.x, windowSize.y);
	}
}

// Moving the mouse while in navigation mode rotates the camera
void Application::mouseMove(int x, int y)
{
	if(bNavigation)
	{
		glm::ivec2 move = glm::ivec2(previousMousePos.x - x, y - previousMousePos.y);
		if((move.x != 0) || (move.y != 0))
		{
			if(move.x != 0)
				scene.getCamera().rotateCamera(ROTATION_DELTA_HEAD * move.x);
			if(move.y != 0)
				scene.getCamera().changePitch(ROTATION_DELTA_PITCH * move.y);
			previousMousePos = glm::ivec2(x, y);
		}
	}
}

// Process mouse events
void Application::mousePress(int button)
{
  mouseButtons[button] = true;
}

void Application::mouseRelease(int button)
{
  mouseButtons[button] = false;
}

// Return a given key state
bool Application::getKey(int key) const
{
	return keys[key];
}

bool Application::getSpecialKey(int key) const
{
	return specialKeys[key];
}





