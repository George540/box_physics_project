#include "../Include/World.h"
#include "../Include/Renderer.h"
#include "../Include/Camera.h"
#include "../Include/Object.h"
#include "../Include/Light.h"
#include "../Include/CollisionHelper.h"

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#define STB_IMAGE_IMPLEMENTATION
#include "../Include/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtx/transform.hpp>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <limits>
#include <vector>
#include <iostream>
#include "../Include/EventManager.h"

using namespace std;
using namespace glm;

World* World::instance;


World::World()
{
	instance = this;

	gravity = 9.07f;
	isTestingSleeping = false;

	// Setup Camera
	mCamera = new Camera(vec3(0.0f, 1.0f, 10.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));

	// Setup light source
	mLight = new Light(vec3(0.0f, 7.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	
	mWoodTexture = LoadTexture("../Source/Wood.jpg");
	mBoxTexture = LoadTexture("../Source/Crate.png");
	mFloor = new Cube();
	mFloor->SetTexture(mWoodTexture);

	/*
	Cube parameters:
		1. vec3 Position
		2. vec3 Rotation
		3. vec3 Scale
		4. float mass
		5. bool Is it asleep?
		6. bool Is it a movable box?
		7. GLuint texture
	*/

	AddCube(new Cube(vec3(-4.0f, 5.0f, 1.5f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(3.0f, 4.0f, 2.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(6.0f, 3.0f, 2.5f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(-4.0f, 10.0f, 1.5f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));

	for (int i = 0; i < mCubes.size(); i++) {
		mCubes.at(i)->SetGravity(gravity);
	}

}

World::World(float grav, bool isTest)
{
	instance = this;

	gravity = grav;
	isTestingSleeping = isTest;

	// Setup Camera
	mCamera = new Camera(vec3(0.0f, 10.0f, 20.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));

	// Setup light source
	mLight = new Light(vec3(0.0f, 7.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));

	mWoodTexture = LoadTexture("../Source/Wood.jpg");
	mBoxTexture = LoadTexture("../Source/Crate.png");
	mBoxRedTexture = LoadTexture("../Source/CrateRed.png");

	mFloor = new Cube();
	mFloor->SetTexture(mWoodTexture);

	/*
	Cube parameters:
		1. vec3 Position
		2. vec3 Rotation
		3. vec3 Scale
		4. float mass
		5. bool Is it asleep?
		6. bool Is it a movable box?
		7. GLuint texture
	*/

	/*AddCube(new Cube(vec3(-4.5f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(4.0f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));*/
	//AddCube(new Cube(vec3(-3.5f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));

	
	
	AddCube(new Cube(vec3(-4.5f, 6.0f, -4.5f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(-0.0f, 5.0f, -5.5f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(0.0f, 6.0f, 4.5f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(-4.0f, 4.0f, 4.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(4.5f, 5.0f, 6.5f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	AddCube(new Cube(vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, false, true, mBoxTexture));
	

	for (int i = 0; i < mCubes.size(); i++) {
		mCubes.at(i)->SetGravity(gravity);
	}

}

void World::Update(float dt)
{
	mCamera->Update(dt);
	
	/*
	mcubes.at(0)->setrotation(vec3(1.0f, 1.0f, 0.0f), 30.0f);
	mcubes.at(0)->addforceacceleration(vec3(2.0f, 0.0f, 0.0f), dt);
	//mcubes.at(0)->addtorqueacceleration(vec3(0.2f, 0.2f, 0.2f));
    mcubes.at(1)->addtorqueacceleration(vec3(0.2f, 0.0f, 0.1f));
	mcubes.at(1)->addforceacceleration(vec3(-2.0f, 0.0f, 0.0f), dt);
	mcubes.at(2)->addtorqueacceleration(vec3(-0.2f, 0.0f, 0.1f));
	mcubes.at(2)->addforceacceleration(vec3(-1.0f, 0.0f, 0.0f), dt);
	*/
	
	for (vector<Cube*>::iterator it = mCubes.begin(); it < mCubes.end(); ++it) {
		(*it)->AddForceAcceleration((vec3(0.0f, 1.5f, 0.0f) - (*it)->GetPosition()) * 0.4f, dt);
		CheckSleepingState((*it));
	}
	

	/*
		Reference #1: Ray-Cube intersection
	*/
	if (glfwGetMouseButton(EventManager::GetWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		glm::mat4 inverseView = glm::inverse(mCamera->GetViewMatrix());
		glm::vec3 rayOrigin = inverseView[3]; //the camera position
		glm::vec3 rayDirection = -1.0f * glm::normalize(inverseView[2]); //the camera lookat

		//Try intersecting ray with every model
		//We are shooting a ray from the camera, directly forward
		float closestIntersection = std::numeric_limits<float>::infinity();

		Cube* intersectingModel = nullptr;
		for (vector<Cube*>::iterator it = mCubes.begin(); it < mCubes.end(); ++it)
		{
			Cube* currentTarget = *it;
			float intersectionPoint = currentTarget->IntersectsRay(rayOrigin, rayDirection);
			if (intersectionPoint >= 0.0f && intersectionPoint < closestIntersection) {
				closestIntersection = currentTarget->IntersectsRay(rayOrigin, rayDirection);
				intersectingModel = currentTarget;
			}
		}
		
		if (intersectingModel != nullptr)
		{
			if (intersectingModel->IsAsleep() == true) {
				intersectingModel->SetLinearVelocity(vec3(0.0f, 2.0f, 0.0f));
				intersectingModel->SetSleepingState(false);
			}
			//cout << intersectingModel->IsAsleep() << endl;
			//CheckSleepingState(intersectingModel);
			glm::vec3 intersectingPoint = rayOrigin + closestIntersection * rayDirection;
			glm::vec3 force = 10.0f * glm::normalize(intersectingModel->GetPosition() - intersectingPoint);
			intersectingModel->AddForceAcceleration(force, dt);

			glm::vec3 torque = 3.0f * (glm::cross(glm::normalize(force), glm::normalize(intersectingPoint)));
			intersectingModel->AddTorqueAcceleration(torque);
		}


	}
	

	/*
		Iterate through cubes in the scene inserted in the mCubes Vector
	*/
	for (vector<Cube*>::iterator it = mCubes.begin(); it < mCubes.end(); ++it)
	{
		glm::vec3 groundPoint = glm::vec3(0.0f);
		glm::vec3 groundUp = glm::vec3(0.0f, 1.0f, 0.0f);
		
		if ((*it)->PenetratesGround(groundPoint, groundUp)) {
			(*it)->BounceOffGround(isTestingSleeping);
		}

		/*
			HIGHLIGHT: CUBE-CUBE INTERSECTION
			More details in the methods implemented bellow
			Total Algorithm time complexity every frame: O(n^3)
		*/
		for (vector<Cube*>::iterator it2 = it; it2 < mCubes.end(); ++it2) {
			ApplyImpulses(*it, *it2);
		}

		(*it)->Update(dt);
	}
}

/*
	Deallocate memory
*/
World::~World()
{
	ClearCubes();
	delete mCamera;
	delete mLight;
}

World* World::GetInstance()
{
	return instance;
}

/*
	Render world and its objects
*/
void World::Draw()
{
	Renderer::BeginFrame();

	// Draw Light Source
	Renderer::SetShader(1);
	glUseProgram(Renderer::GetShaderProgramID());

	SetProjectionMatrix(Renderer::GetShaderProgramID(), mCamera->GetProjectionMatrix());
	SetViewMatrix(Renderer::GetShaderProgramID(), mCamera->GetViewMatrix());

	mLight->Draw();

	// Draw Cube Floor (switch shader)
	Renderer::SetShader(0);
	glUseProgram(Renderer::GetShaderProgramID());

	SetProjectionMatrix(Renderer::GetShaderProgramID(), mCamera->GetProjectionMatrix());
	SetViewMatrix(Renderer::GetShaderProgramID(), mCamera->GetViewMatrix());

	mFloor->Draw();

	// Draw Cubes
	for (vector<Cube*>::iterator it = mCubes.begin(); it < mCubes.end(); ++it)
	{
		(*it)->Draw();
	}

	Renderer::SetShader(1);
	glUseProgram(Renderer::GetShaderProgramID());

	SetProjectionMatrix(Renderer::GetShaderProgramID(), mCamera->GetProjectionMatrix());
	SetViewMatrix(Renderer::GetShaderProgramID(), mCamera->GetViewMatrix());

	// Draw Path Lines
	Renderer::CheckForErrors();

	Renderer::EndFrame();
}

/*
	M-V-P accessors for vertex shaders
*/
void World::SetProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
	glUseProgram(shaderProgram);
	GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}
void World::SetViewMatrix(int shaderProgram, mat4 viewMatrix)
{
	glUseProgram(shaderProgram);
	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}
void World::SetWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
	glUseProgram(shaderProgram);
	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

const Camera* World::GetCamera() const
{
	return mCamera;
}

Cube* World::GetCube(int index) 
{
	return mCubes.at(index);
}

void World::AddCube(Cube* cube)
{
	// Checking if there is a duplicate of the exact same object instance
	// We do not need two of the same instance in the vector, but rather just copies of it
	for (auto it = mCubes.begin(); it != mCubes.end(); it++) {
		if (*it == cube) {
			cout << "Cube has already been added" << endl;
		}
	}
	mCubes.push_back(cube);
}

void World::RemoveCube(Cube* cube)
{
	for (auto it = mCubes.begin(); it != mCubes.end(); it++) {
		if (*it == cube) {
			it = mCubes.erase(it);
			break;
		}
	}
	delete cube;
}

void World::ClearCubes()
{
	for (auto it = mCubes.begin(); it != mCubes.end(); it++) {
		delete(*it);
	}
	mCubes.clear();
}

/*
	Using stb_image.h to load texture files to program
*/
GLuint World::LoadTexture(const char* filename)
{
	// Step1 Create and bind textures
	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	assert(textureId != 0);


	glBindTexture(GL_TEXTURE_2D, textureId);

	// Step2 Set filter parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Step3 Load Textures with dimension data
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
		return 0;
	}

	// Step4 Upload the texture to the GPU
	GLenum format = 0;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
		0, format, GL_UNSIGNED_BYTE, data);

	// Step5 Free resources
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

/*
	IMPORTANT: Applying impulses between cube models
	Reference #1: Built uppon the collision detection and resolution code used for Sphere-Sphere collision

// Reference 1, 2 and 3: IMPULSE FORMULA FOR COLLISION RESOLUTION
//----------------------------------------------------------
//
// j =			                           -(1+Cor)(relv.norm)
//	     -----------------------------------------------------------------------------------
//	     norm.norm(1/Mass0 + 1/Mass1) + (sqr(r0 x norm) / Inertia0) + (sqr(r1 x norm) / Inertia1)
//
//----------------------------------------------------------

*/
void World::ApplyImpulses(Cube* c1, Cube* c2)
{
	// Check if the two boxes are not from the same pointer
	// Check if boxes are NOT static by their mass
	if (c1 != c2 && c1->IsBox() && c2->IsBox() && c1->GetMass() < 10000.0f && c2->GetMass() < 10000.0) {
		
		//c1->SetSleepingState(false);
		//c2->SetSleepingState(false);

		float Distance = distance(c1->GetPosition(), c2->GetPosition());

		// If penetrated, apply impulses based on the direction of their penetrated faces from the collision Point
		if (CheckPenetration(c1, c2) && FindPenetrationPoint(c1, c2) != vec3(-10000.0f, -10000.0f, -10000.0f)) {

			vec3 collisionNormal = normalize(c1->GetPosition() - c2->GetPosition());
			vec3 collisionPoint = FindPenetrationPoint(c1, c2); // returns penetration point - check function for more information

			float m1 = c1->GetMass();
			float m2 = c2->GetMass();

			glm::vec3 normalVelocity1 = glm::dot(c1->GetLinearVelocity(), collisionNormal) * collisionNormal;
			glm::vec3 normalVelocity2 = glm::dot(c2->GetLinearVelocity(), collisionNormal) * collisionNormal;

			glm::vec3 tangentMomentum1 = c1->GetLinearVelocity() - normalVelocity1;
			glm::vec3 tangentMomentum2 = c2->GetLinearVelocity() - normalVelocity2;

			glm::vec3 newNormalVelocity1 = ((m1 - m2) / (m1 + m2)) * normalVelocity1 + ((2 * m2) / (m1 + m2) * normalVelocity2);
			glm::vec3 newNormalVelocity2 = ((2 * m1) / (m1 + m2)) * normalVelocity1 + ((m2 - m1) / (m1 + m2) * normalVelocity2);

			c1->SetLinearVelocity(newNormalVelocity1 + tangentMomentum1);
			c2->SetLinearVelocity(newNormalVelocity2 + tangentMomentum2);

			vec3 force1 = 5.0f * normalize(c2->GetPosition() - collisionPoint) * -1.0F;
			vec3 torque1 = 2.0f * cross(normalize(force1), normalize(collisionPoint));

			vec3 force2 = 5.0f * normalize(c1->GetPosition() - collisionPoint) * -1.0F;
			vec3 torque2 = 2.0f * cross(normalize(force2), normalize(collisionPoint));

			c1->AddTorqueAcceleration(torque1);
			c2->AddTorqueAcceleration(torque2);
		}
	}
}

/*
	IMPORTANT: OBB Collision detection between two cubes, returns boolean
*/
bool World::CheckPenetration(Cube* o1, Cube* o2)
{
	// Iterate through second cube and turn each vertex into world coordinates
	for (int i = 0; i < o2->GetVertexPositions().size(); i++) {
		vec3 vertexWorldCoordinates = o2->GetWorldTransformationMatrix() * vec4(o2->GetVertexPositions().at(i), 1.0f);
		
		vec4 worldSpacePoint(vertexWorldCoordinates, 1.0f);

		//We will first transform the position into Cube Model Space
		mat4 mTranslate = translate(o1->GetPosition());
		mat4 mRotation = rotate(o1->GetRotationalAngle(), o1->GetRotation());
		mat4 mScale = scale(o1->GetScaling());

		// Put vertex coordinates from o2 into local space of o1
		vec3 cubeModelSpacePoint = glm::inverse(mTranslate * mRotation * mScale) * worldSpacePoint;
		vec3 halfSize = o1->GetScaling() * 0.5f;

		// Check if the vertex from o2 is contained within the first cube's bounding box
		bool containsPoint =	cubeModelSpacePoint.x >= -halfSize.x && cubeModelSpacePoint.x <= halfSize.x &&
								cubeModelSpacePoint.y >= -halfSize.y && cubeModelSpacePoint.y <= halfSize.y &&
								cubeModelSpacePoint.z >= -halfSize.z && cubeModelSpacePoint.z <= halfSize.z;

		if (containsPoint) {
			return true;
		}
	}
	return false;
}

/*
	IMPORTANT: Same as OBB collision detection, but this time returns the contact point
*/
vec3 World::FindPenetrationPoint(Cube* o1, Cube* o2)
{
	for (int i = 0; i < o2->GetVertexPositions().size(); i++) {
		vec3 vertexWorldCoordinates = o2->GetWorldTransformationMatrix() * vec4(o2->GetVertexPositions().at(i), 1.0f);

		vec4 worldSpacePoint(vertexWorldCoordinates, 1.0f);

		//We will first transform the position into Cube Model Space
		mat4 mTranslate = translate(o1->GetPosition());
		mat4 mRotation = rotate(o1->GetRotationalAngle(), o1->GetRotation());
		mat4 mScale = scale(o1->GetScaling());

		vec3 cubeModelSpacePoint = glm::inverse(mTranslate * mRotation * mScale) * worldSpacePoint;
		vec3 halfSize = o1->GetScaling() * 0.5f;

		bool containsPoint = cubeModelSpacePoint.x >= -halfSize.x && cubeModelSpacePoint.x <= halfSize.x &&
			cubeModelSpacePoint.y >= -halfSize.y && cubeModelSpacePoint.y <= halfSize.y &&
			cubeModelSpacePoint.z >= -halfSize.y && cubeModelSpacePoint.z <= halfSize.y;

		if (containsPoint) {
			return vertexWorldCoordinates;
		}
	}
	// we assume no penetration point was found so we set an infinity distance relative to this world
	return glm::vec3(-10000.0f, -10000.0f, -10000.0f);
}

void World::CheckSleepingState(Cube* c)
{
	if (c->IsAsleep()) {
		c->SetTexture(mBoxRedTexture);
	}
	else {
		c->SetTexture(mBoxTexture);
	}
}

