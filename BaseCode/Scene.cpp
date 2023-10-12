#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GL/glut.h>
#define GLM_FORCE_RADIANS

#include "Scene.h"


using namespace std;

Scene::Scene()
{
	cube = NULL;
	bunny = NULL;
	new_mesh = NULL; 
	moai = NULL;
	dragon = NULL;
}

Scene::~Scene()
{
	if(cube != NULL)
		delete cube;
	if(bunny != NULL)
		delete bunny;
	if(new_mesh != NULL)
		delete new_mesh;
	if(moai != NULL)
		delete moai;
	if(dragon != NULL)
		delete dragon;
}


// Initialize the scene. This includes the cube we will use to render
// the floor and walls, as well as the camera.
void Scene::init()
{
	// Initializations
	initShaders();								// Initialize the shaders
	camera.init(glm::vec3(2.5f, 0.5f, 5.f));	// Initialize the camera
	currentTime = 0.0f;							// Initialize current rendering time
	tileCounter = 0;							// Set the tileCounter to 0 and start reading the Grid from the .txt file
	cube = new TriangleMesh();					// Initialize the cube mesh, used for floor and wall tiles
	cube->buildCube();
	cube->sendToOpenGL(basicProgram);
	simplification_mode = REPRESENTATIVE_MEAN;	// Simplification modes REPRESENTATIVE_MEAN, REPRESENTATIVE_QEM
	rendering_mode = false;
	renderLOD = false;

	// Load my meshes
	loadMesh("../../extras/models/bunny.ply", bunny);
	loadMesh("../../extras/models/moai.ply", moai);
	loadMesh("../../extras/models/dragon.ply", dragon);

	// Set the triangle count of the TC_Models
	for (unsigned int i=0; i<4; i++)
	{
		tc_bunny.triangleCount[i] = bunny->getTriangleSize(i);
		tc_moai.triangleCount[i] = moai->getTriangleSize(i);
		tc_dragon.triangleCount[i] = dragon->getTriangleSize(i);
	}


	// Reading the grid layout from .txt file
	tiles = readGrid("../../extras/grid/grid_layout.txt");

	// Reading the museum visibility from .txt file
	visibilitySimplified = readVisibility("../../extras/grid/visibilitySimplified.txt");
	visibilitySupercover = readVisibility("../../extras/grid/visibilitySupercover.txt");

}

// Loads the mesh into CPU memory and sends it to GPU memory (using GL)
bool Scene::loadMesh(const char* filename)
{
	return loadMesh(filename, new_mesh);
}

bool Scene::loadMesh(const char *filename, TriangleMesh* &mesh)
{
#pragma warning( push )
#pragma warning( disable : 4101)
	PLYReader reader;
#pragma warning( pop ) 

	if(mesh != NULL)
	{
		mesh->free();
		delete mesh;
	}
	mesh = new TriangleMesh();

	// If LOD files exists then for each LOD file, read it and store their params in the array
	std::string str_filename = filename;
	std::string str_simplification_mode_mean = "Mean";
	std::string str_simplification_mode_qem = "QEM";
	std::string str_simplification_lod0 = "LOD0";
	std::string str_simplification_lod1 = "LOD1";
	std::string str_simplification_lod2 = "LOD2";
	std::string str_simplification_lod3 = "LOD3";
	std::string str_extension_file = ".ply";
	str_filename = str_filename.substr(0, str_filename.size() - 4);

	if (simplification_mode == REPRESENTATIVE_MEAN &&
		reader.fileExists(str_filename + str_simplification_mode_mean + str_simplification_lod0 + str_extension_file) &&
		reader.fileExists(str_filename + str_simplification_mode_mean + str_simplification_lod1 + str_extension_file)&&
		reader.fileExists(str_filename + str_simplification_mode_mean + str_simplification_lod2 + str_extension_file)&&
		reader.fileExists(str_filename + str_simplification_mode_mean + str_simplification_lod3 + str_extension_file)) {

		bool bSuccess = reader.readMesh(str_filename + str_simplification_mode_mean + str_simplification_lod0 + str_extension_file, *mesh);
		if (bSuccess) {
			mesh->generateLOD(basicProgram, 0);
		}

		bSuccess = reader.readMesh(str_filename + str_simplification_mode_mean + str_simplification_lod1 + str_extension_file, *mesh);
		if (bSuccess) {
			mesh->generateLOD(basicProgram, 1);
		}

		bSuccess = reader.readMesh(str_filename + str_simplification_mode_mean + str_simplification_lod2 + str_extension_file, *mesh);
		if (bSuccess) {
			mesh->generateLOD(basicProgram, 2);
		}

		bSuccess = reader.readMesh(str_filename + str_simplification_mode_mean + str_simplification_lod3 + str_extension_file, *mesh);
		if (bSuccess) {
			mesh->generateLOD(basicProgram, 3);
		}
	}
	else if (simplification_mode == REPRESENTATIVE_QEM &&
		reader.fileExists(str_filename + str_simplification_mode_qem + str_simplification_lod0 + str_extension_file) &&
		reader.fileExists(str_filename + str_simplification_mode_qem + str_simplification_lod1 + str_extension_file) &&
		reader.fileExists(str_filename + str_simplification_mode_qem + str_simplification_lod2 + str_extension_file) &&
		reader.fileExists(str_filename + str_simplification_mode_qem + str_simplification_lod3 + str_extension_file)) {
		
		bool bSuccess = reader.readMesh(str_filename + str_simplification_mode_qem + str_simplification_lod0 + str_extension_file, *mesh);
		if (bSuccess) {
			mesh->generateLOD(basicProgram, 0);
		}

		bSuccess = reader.readMesh(str_filename + str_simplification_mode_qem + str_simplification_lod1 + str_extension_file, *mesh);
		if (bSuccess) {
			mesh->generateLOD(basicProgram, 1);
		}

		bSuccess = reader.readMesh(str_filename + str_simplification_mode_qem + str_simplification_lod2 + str_extension_file, *mesh);
		if (bSuccess) {
			mesh->generateLOD(basicProgram, 2);
		}

		bSuccess = reader.readMesh(str_filename + str_simplification_mode_qem + str_simplification_lod3 + str_extension_file, *mesh);
		if (bSuccess) {
			mesh->generateLOD(basicProgram, 3);
		}
	}
	else {
		// Else generate the LODs from base file, store their params in the array and write LOD files
		bool bSuccess = reader.readMesh(filename, *mesh);
		if (bSuccess) {
			mesh->generateLODs(basicProgram, simplification_mode, filename);
		}
		return bSuccess;
	}

	return false;
}

// Update scene time
void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
}

// Render the scene. First the room, then the mesh it there is one loaded.
void Scene::render()
{
	// ImGui
	// ImGui UI window
	// Set the next window position using normalized coordinates
    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiCond_Always);
	if (ImGui::Begin("Settings")) {
		ImGui::Text("Key Commands:");
        ImGui::Text("F1: Toggle application focus");
		ImGui::Text("F5: Fullscreen");
		ImGui::Text("Navigation: WASD/arrows, Q: Up, E: Down");
		ImGui::Separator();
		ImGui::Text("Level of Detail (LOD)");
		ImGui::SliderInt(" ", &renderLOD, 0, 3);
		ImGui::Separator();
		ImGui::Text("Rendering Technique");
		ImGui::RadioButton("Forward Rendering", &rendering_mode, FORWARD);
		ImGui::RadioButton("Visibility - Simplified Bresenham", &rendering_mode, VISIBILITY_BRESENHAM_SIMPLIFIED);
		ImGui::RadioButton("Visibility - Supercover Bresenham", &rendering_mode, VISIBILITY_BRESENHAM_SUPERCOVER);
		ImGui::Separator();
        ImGui::Text("Performance");
        ImGui::Text("Frames per second (FPS): %g", 	  sceneFps);
        ImGui::Text("Triangles per Second(TPS): %g",  TPS_display);
        ImGui::Text("Max Cost: %g", 				  maxCost_display);
    }
    ImGui::End();

	// Debugging via wireframe ...
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	
	// clear the tc_models vector, to fill it during rendering and compute TPS, MaxCost later.
	tc_models.clear();

	// Render room
 	renderRoom();
}

// Fetch the camera
VectorCamera &Scene::getCamera()
{
  return camera;
}

// Compute the mesh's distance to camera
float Scene::distanceToCamera(const glm::ivec2 &meshPosition) const
{
    return glm::length(camera.getPosition() - glm::vec3(meshPosition.x, 0.0f, meshPosition.y));
}

// Render the room. Both the floor and the walls are instances of the
// same initial cube scaled and translated to build the room.
void Scene::renderRoom()
{	
	tileCounter = 0;	// Reset the tileCounter

	// Get the camera's current cell index 
	camera_x = static_cast<int>(camera.getPosition().x + 18.5f);
	camera_z = static_cast<int>(-camera.getPosition().z + 18.5f);
	currentCellIndex = camera_x * gridSize + camera_z;

	// Select Visibility algorithm
	switch(rendering_mode)
	{
		case FORWARD:
			// Do nothing, just continue the rendering. Case for Readability
			break;
		case VISIBILITY_BRESENHAM_SIMPLIFIED:
			// Visibility vector - Simplified
			currentVisibility = visibilitySimplified[currentCellIndex];
			break;
		case VISIBILITY_BRESENHAM_SUPERCOVER:
			// Visibility vector - Supercover
			currentVisibility = visibilitySupercover[currentCellIndex];
		default:
			break;
	};

	// Rendering loop
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			if(rendering_mode != FORWARD) // --> This means that it is either of the Bresenham modes
			{
				//First visibility check
				tileIndex = i * gridSize + j;
				if (!currentVisibility[tileIndex]) {
					// This tile is not visible from the current cell, skip it
					tileCounter++;
					continue;
				}
			}

			// Tile check
			if (tiles[tileCounter] == 0)		// 0 -- > Blank space
			{
				// just increment the tile counter
				tileCounter++;
			}
			else if (tiles[tileCounter] == 1)	// 1 -- > Floor tile
			{
				renderFloor(i,j);
				tileCounter++;
			}
			else if (tiles[tileCounter] == 2)	// 2 --> Wall tile
			{
				renderWall(i,j);
				tileCounter++;
			}
			else if (tiles[tileCounter] == 3)	// 3 --> Bunny model
			{
				// Model
				renderModel(bunny, i, j, renderLOD);
				tc_models.push_back(tc_bunny);
				// Floor
				renderFloor(i,j);
				tileCounter++;
			}
			else if (tiles[tileCounter] == 4)	// 4 --> Moai model
			{
				// Model
				renderModel(moai, i, j, renderLOD);
				tc_models.push_back(tc_moai);
				// Floor
				renderFloor(i,j);
				tileCounter++;
			}
			else if (tiles[tileCounter] == 5)	// 5 --> Dragon model
			{
				// Model
				renderModel(dragon, i, j, renderLOD);
				tc_models.push_back(tc_dragon);
				// Floor
				renderFloor(i,j);
				tileCounter++;
			}
		}
	}
}

// Render Floor tile
void Scene::renderFloor(int i, int j)
{
	// Floor render
	if(cube != NULL)
	{
		basicProgram.use();
		modelview = camera.getModelViewMatrix();
		modelview = glm::translate(modelview, glm::vec3(static_cast<float>(i - gridSize/2), -0.5f, static_cast<float>(-j + gridSize/2)));
		modelview = glm::scale(modelview, glm::vec3(1.f, 1.f, 1.f));
		normalMatrix = glm::inverseTranspose(modelview);
		basicProgram.setUniform4f("color", 0.5f, 0.5f, 0.5f, 1.0f);
		basicProgram.setUniformMatrix4f("modelview", modelview);
		basicProgram.setUniformMatrix4f("projection", camera.getProjectionMatrix());
		basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
		cube->render();
	}
	else
	{
		cout << "Floor null." << endl;
	}
}

// Render Wall tile
void Scene::renderWall(int i, int j)
{
	// Wall render
	if(cube != NULL)
	{
		basicProgram.use();
		modelview = camera.getModelViewMatrix();
		modelview = glm::translate(modelview, glm::vec3(static_cast<float>(i - gridSize / 2), -0.5f, static_cast<float>(-j + gridSize / 2)));
		modelview = glm::scale(modelview, glm::vec3(1.f, 3.f, 1.f));
		normalMatrix = glm::inverseTranspose(modelview);
		basicProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);
		basicProgram.setUniformMatrix4f("modelview", modelview);
		basicProgram.setUniformMatrix4f("projection", camera.getProjectionMatrix());
		basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
		cube->render();
	}
	else
	{
		cout << "Wall null." << endl;
	}
}

// Render Model
void Scene::renderModel(TriangleMesh* currentMesh, int i, int j, int renderLOD)
{
	// Mesh render
	if(currentMesh != NULL)
	{
		basicProgram.use();
		modelview = camera.getModelViewMatrix();
		modelview = glm::translate(modelview, glm::vec3(static_cast<float>(i - gridSize/2), 0.0f, static_cast<float>(-j + gridSize/2)));
		modelview = glm::scale(modelview, glm::vec3(1.f, 1.f, 1.f));
		normalMatrix = glm::inverseTranspose(modelview);
		basicProgram.setUniform4f("color", 0.5f, 0.5f, 0.5f, 1.0f);
		basicProgram.setUniformMatrix4f("modelview", modelview);
		basicProgram.setUniformMatrix4f("projection", camera.getProjectionMatrix());
		basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
		currentMesh->render(renderLOD);
	}
	else
	{
		cout << "Model null." << endl;
	}
	
	// Set the LOD of the tc_models, will be used to push the correct tc_models into a vector
	// to be used in the time-critical part
	if (currentMesh == bunny)
	{
		tc_bunny.LOD = renderLOD;
	}
	else if (currentMesh == moai)
	{
		tc_moai.LOD = renderLOD;
	}
	else if (currentMesh == dragon)
	{
		tc_dragon.LOD = renderLOD;
	}
}

// Load, compile, and link the vertex and fragment shader
void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/basic.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/basic.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	basicProgram.init();
	basicProgram.addShader(vShader);
	basicProgram.addShader(fShader);
	basicProgram.link();
	if(!basicProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << basicProgram.log() << endl << endl;
	}
	basicProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

// Helper function to render the AABB cube
void Scene::renderAABBCube(const glm::vec3& minPoint, const glm::vec3& maxPoint)
{
    // Calculate the center and size of the AABB
    center = (minPoint + maxPoint) * 0.5f;
    scale = maxPoint - minPoint;

	// Calculate the modelview matrix
	modelCube = glm::mat4(1.0f);
	modelCube = glm::translate(modelCube, center);
	modelCube = glm::scale(modelCube, scale);
    //glm::mat4 modelCube = glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), size);
	modelview = camera.getModelViewMatrix() * modelCube;

	// Rendering as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    basicProgram.setUniform4f("color", 0.0f, 1.0f, 0.0f, 1.0f);		// Set the color to green
    basicProgram.setUniformMatrix4f("modelview", modelview);
    cube->render();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Read the grid layout from the .txt file
std::vector<int> Scene::readGrid(const std::string& filename) {
    std::ifstream myfile;
    myfile.open(filename);
    char mychar;
    int myint;

    std::vector<int> tiles(gridSize * gridSize);

    // Open grid layout file
    if (myfile.is_open()) {
        int tileCounter = 0;
        while (myfile.good())
        {
            myfile >> std::noskipws >> mychar; // pipe file's content into stream

            if (!isspace(mychar))	// Line-break counts as space
            {
                myint = int(mychar) - 48;	// 48 is keycode for 0, 49 keycode for 1, etc.
                tiles[tileCounter] = myint;
                tileCounter += 1;
                // I break here, because if not it will try to add the last character of the .txt file doubled.
                if (tileCounter == gridSize * gridSize)
                    break;
            }
        }
    }
    else
    {
        std::cout << "Failed to read the grid layout file." << std::endl;
        std::cout << "It should be located in: '../../extras/grid/grid_layout.txt'" << std::endl;
        std::cout << "Make sure the path is correct, or just try again." << std::endl;
    }
    myfile.close();

    return tiles;
}

// Read the museum visibility from the .txt file
std::vector<std::vector<bool>> Scene::readVisibility(const std::string& filename) {
    std::ifstream myfile;
    myfile.open(filename);
    if (!myfile.is_open()) {
        std::cout << "Unable to open visibility file." << endl;
        std::cout << "It should be located in: '../../extras/grid/visibility____.txt'" << endl;
		std::cout << "Make sure the path is correct, or just try again." << std::endl;
        return {};
    }

    std::vector<std::vector<bool>> visibility;

    std::string line;
    while (std::getline(myfile, line)) {
        std::istringstream iss(line);
        std::vector<bool> row;

        int value;
        while (iss >> value) {
            row.push_back(value == 1);
        }

        visibility.push_back(row);
    }

    myfile.close();
    return visibility;
}
