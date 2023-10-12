#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <vector>

#include "VectorCamera.h"
#include "ShaderProgram.h"
#include "TriangleMesh.h"
#include "PLYReader.h"
#include "Octree.h"
#include "TimeCritical.h"

#include "backends/imgui_impl_glut.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.

class Scene
{

public:
	Scene();
	~Scene();

	void init();
	bool loadMesh(const char *filename);
	bool loadMesh(const char* filename, TriangleMesh* &mesh);
	void update(int deltaTime);
	void render();

  	VectorCamera &getCamera();
	float distanceToCamera(const glm::ivec2 &gridCoordinates) const;

  	float sceneFps;						 	// public fps
	std::vector<TC_Model> tc_models;	 	// public tc_models  vector
	double TPS_display, maxCost_display; 	// TPS / Cost / Benefit ImGui display

private:
	void initShaders();
	void computeModelViewMatrix();
	
	// Rendering functions
	void renderAABBCube(const glm::vec3& minPoint, const glm::vec3& maxPoint);
	void renderRoom();
	void renderFloor(int i, int j);
	void renderWall(int i, int j);
	void renderModel(TriangleMesh* currentMesh, int i, int j, int renderLod);

	//
	std::vector<int> readGrid(const std::string& filename);
	std::vector<std::vector<bool>> readVisibility(const std::string& filename);

private:

	// rendering variables
	// Initialize normal and model-view matrices for rendering
	glm::mat3 normalMatrix;
	glm::mat4 modelview;

	// General
	VectorCamera camera;
	ShaderProgram basicProgram;
	float currentTime;
	int rendering_mode;
	enum renderingTechnique
	{
		FORWARD,
		VISIBILITY_BRESENHAM_SIMPLIFIED,
		VISIBILITY_BRESENHAM_SUPERCOVER
	};

	// Meshes / Models
	TriangleMesh *cube, *new_mesh, *bunny, *moai, *dragon;
	TC_Model tc_bunny, tc_moai, tc_dragon;
	int simplification_mode;
	int renderLOD;

	// AABB Cube params
	glm::vec3 center;
	glm::vec3 scale;
	glm::mat4 modelCube;

	// Grid Layout
	int tileCounter;
	int gridSize = 37;
	std::vector<int> tiles;
	//int tiles[1369];	// Explicitly declaring the tile array, using a gridSize*gridSize (37*37) = 1369 grid
	float floorScale = 20.f;

	// Visibility
	std::vector<std::vector<bool>> visibilitySimplified;
	std::vector<std::vector<bool>> visibilitySupercover;
	std::vector<bool> currentVisibility;
	int camera_x, camera_z, currentCellIndex, tileIndex;
	//const int VISIBILITY_RADIUS = 2;

};


#endif // _SCENE_INCLUDE

