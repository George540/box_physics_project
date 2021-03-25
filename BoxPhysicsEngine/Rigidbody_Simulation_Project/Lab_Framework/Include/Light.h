#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


// The Light's vertices are manually computed and has the simplest form of functionality
// The work is done in the shaders
// The purpose of the light is to provide etter visuals for the cube's orientation
// Shadow mapping is NOT applied to this project, aas light direction is good enough to tell if a collision with other surfaces has been detected
class Light
{
private:
	// ORIENTATION
	glm::vec3 mPosition;
	glm::vec3 mScaling;
	glm::mat4 mWorldTransformationMatrix;

	unsigned int mVAO;
	unsigned int mVBO;
	std::vector<glm::vec3> mVertexPositions;

public:
	Light(glm::vec3& position, glm::vec3& scale);
	~Light();

	void Draw();
	void SetPosition(glm::vec3& position) { mPosition = position; }
	void SetScaling(glm::vec3& scaling) { mScaling = scaling; }
	glm::vec3 GetPosition() const { return mPosition; }
	glm::vec3 GetScaling() const { return mScaling; }
	glm::mat4 GetWorldTransformationMatrix() const { return mWorldTransformationMatrix; }

	unsigned int GetVAO() { return mVAO; }

	void BuildVertexBuffer();
};
