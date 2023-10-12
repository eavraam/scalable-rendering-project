#ifndef _VECTOR_CAMERA_INCLUDE
#define _VECTOR_CAMERA_INCLUDE


#include <glm/glm.hpp>


// VectorCamera contains the properies of the camera the scene is using
// It is responsible for computing the associated GL matrices


class VectorCamera
{

public:
	VectorCamera();
	~VectorCamera();
	
	void init(const glm::vec3 &initPosition);
	
	void resizeCameraViewport(int width, int height);
	void rotateCamera(float rotation);
	void changePitch(float rotation);
	void moveForward(float distance);
	void moveUpward(float distance);
	void strafe(float distance);
	
	const glm::vec3 &getPosition() const {return position;}

	glm::mat4 &getProjectionMatrix();
	glm::mat4 &getModelViewMatrix();

private:
  void computeModelViewMatrix();

private:
	// Projection params
	float fov;
	float zNear;
	float zFar;
	float aspectRatio;

	// Camera parameters
	glm::vec3 position;
	float movementSpeed;
	float angleDirection, anglePitch;
	glm::mat4 projection, modelview;	// OpenGL matrices

};


#endif // _VECTOR_CAMERA_INCLUDE

