#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Include/Object.h"

typedef unsigned int  GLuint;

class Camera;
class Cube;
class Light;

class World
{
public:
	World();
	World(float grav, bool isTest);
	~World();

	static World* GetInstance();

	void Update(float dt);
	void Draw();
	void SetMatrix(unsigned int shaderProgram, glm::mat4 worldMatrix);

	Cube* GetCube(int index);
	void AddCube(Cube* cube);
	void RemoveCube(Cube* cube);
	void ClearCubes();
	GLuint LoadTexture(const char* filename);
	GLuint GetWoodTexture() { return mWoodTexture; }

	const Camera* GetCamera() const;

	void SetProjectionMatrix(int shaderProgram, glm::mat4 projectionMatrix);
	void SetViewMatrix(int shaderProgram, glm::mat4 viewMatrix);
	void SetWorldMatrix(int shaderProgram, glm::mat4 worldMatrix);

	void CollisionDetection();
	void CheckSleepingState(Cube* c);
	void ApplyImpulses(Cube* o1, Cube* o2);
	bool CheckPenetration(Cube* o1, Cube* o2);
	glm::vec3 FindPenetrationPoint(Cube* o1, Cube* o2);
	void ApplyImpulses(float dt);
private:
	static World* instance;

	std::vector<Cube*> mCubes; // vector that contains pointers to all the cubes' details
	Cube* mFloor; // visual representation of wooden floor, uses same class as boxes
	Camera* mCamera; // Camera pointer
	Light* mLight; // light object

	GLuint mWoodTexture; // wood texture
	GLuint mBoxTexture; // box texture (specifically drawn for the cube.obj model's UV map)
	GLuint mBoxRedTexture; // used to give a visual for box's sleepingstates

	bool isTestingSleeping; // is the program testing the sleeping state feature?
	float gravity; // setting the world's gravity
	float friction = 0.6f; // friction constant (attempted to try cube friction)

};