//
// COMP 477 Project
//
// ORIGINAL OPENGL FRAMEWORK PREPARED BY:
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//
// List of all references will be listed on READ.ME and Project Report

// DISCLAIMER: A portion of the physics studied and implemented for this project are found in the following sources:
// 1. Lab08 from COMP 371 - Collisions and Raycasting
// 2. XBDEV.net - Physics: RigidBody Impulse Cubes (by Ben Kenwright: bkenwright@xbdev.net)
// 3. Online Siggraph 2001 Course Notes for Physically Based Modeling - Rigid Body Simulations (by David Baraff, Pixar Animation Studios)

// Please refer to README.txt, project website or project report for more information

#pragma once

#include <array>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

const static float MAXIMUMMASS = 10000.0f; // MAXIMUM MASS TO MAKE AN OBJECT MOVABLE OR NOT
const static float SLEEPEPSILON = 0.05f; // Initially used for disabling object using its kinetic energy

typedef unsigned int  GLuint;

class Renderer;

class Cube
{
private:
	// ORIENTATION
	glm::vec3 mPosition;                                    // position of object in world coordinates
	glm::vec3 mRotation;                                    // rotation axis of object
	float mRotationAngleInDegrees;							// rotation of object in angle
															// mRotation and MrotationAngleInDegrees simulate the function of a Quaternion
															// but are expressed in a form of a 4x4 matrix
	glm::vec3 mScaling;										// scale of object (length x width x height)
	glm::vec3 mHalfWidths;									//positive halfwidths along each axis
	float mRadius;											// half of the box's diagonal:
															//length= sqrt( mHalfWidths.x*mHalfWidths.x + mHalfWidths.y*mHalfWidths.y + mHalfWidths.z*mHalfWidths.z ) - original used for collision detection
	glm::mat4 mWorldTransformationMatrix;					// World transformation Matrix of object (used for updating orientation and vertex shader work)

	// PHYSICS:
	float mGravity;											// gravity value passed by the World instance
	float mMass;											// mass of object in kg, remains constant (mostly assumed to be 1kg)
	glm::vec3 mLinearVelocity;								// Linear velocity of object (relates to mPosition)
	glm::vec3 mAngularVelocity;								// angular velocity axis of object  (relates to mRotation)
	float mAngularVelocityInDegrees;						// mangnitude of rotational speed of object (relates to mRotationAngleInDegrees)
	glm::mat4 mInvInersia;									// matrix inverse of the inerta tensor calculated at the Box's constructor

	//MISCELLANEOUS
	float mIsAsleep;										// is the box in a stationary state? used to stop the object from moving during unecessary collisions
	float mKineticEnergy;									// box's kinetic energy - initially used to calculate the sleeping state of the box
	GLuint mLightLocation;									// used for the shader's light reflection on box's surface
	GLuint mTexture;										// texture loaded form World Instance
	bool mIsBox;											// is this object a moving box or a stationary surface?
	GLuint mVAO;											// Vertex Array Object
	GLuint mVBO;											// Vertex Buffer Object
	GLuint mNumOfVertices;									// Number of vertices of the object
															// (a box's vertices are 36, but since the program was also tested in spheres, the OBJLoader could calculate the number of vertices)
	std::vector<glm::vec3> mVertexPositions;				// here the object's 

public:

	Cube();
	Cube(const Cube& other);
	Cube(glm::vec3& position, glm::vec3& rotation, glm::vec3& scale, float mass, bool isAsleep, bool isBox, GLuint texture);
	~Cube();

	// Updates
	void Update(float dt); // Updates position, velocity and matrix
	void UpdateWorldMatrix();
	void Draw();

	void SetPosition(glm::vec3& position) { mPosition = position; }
	void SetRotation(glm::vec3& rotation, float angle) { mRotation = rotation; mRotationAngleInDegrees = angle; }
	void SetScaling(glm::vec3& scaling) { mScaling = scaling; }
	glm::vec3 GetPosition() const { return mPosition; }
	glm::vec3 GetRotation() const { return mRotation; }
	float GetRotationalAngle() const { return mRotationAngleInDegrees; }
	glm::vec3 GetScaling() const { return mScaling; }
	glm::vec3 GetHalfWidths() const { return mHalfWidths; }
	glm::mat4 GetWorldTransformationMatrix() const { return mWorldTransformationMatrix; }

	void SetGravity(float gravity) { mGravity = gravity; }
	void SetMass(float mass) { mMass = mass; }
	void SetLinearVelocity(glm::vec3 linearVelocity) { mLinearVelocity = linearVelocity; }
	void SetAngularVelocity(glm::vec3 angularVelocity) { mAngularVelocity = angularVelocity; }
	void AddForceAcceleration(glm::vec3 force, float dt);
	void AddTorqueAcceleration(glm::vec3 torque);
	void CalculateInverseInertia();
	float GetMass() { return mMass; }
	glm::vec3 GetLinearVelocity() const { return mLinearVelocity; }
	glm::vec3 GetAngularVelocity() const { return mAngularVelocity; }
	glm::mat4 GetInverseInersia() const { return mInvInersia; }

	static void AddCollisionImpulse(Cube& c0, Cube& c1, glm::vec3& hitPoint, glm::vec3 normal, float dt, float penetration);
	void BounceOffGround(bool isTestingSleep);
	float IntersectsRay(glm::vec3 source, glm::vec3 direction);
	bool PenetratesGround(glm::vec3 planePoint, glm::vec3 planeNormal);
	std::vector<glm::vec3> GetVertexPositions() { return mVertexPositions; }

	void SetSleepingState(bool isAsleep);
	void CheckIfAsleep();
	bool IsAsleep() { return mIsAsleep; }
	bool IsBox() { return mIsBox; }
	void SetTexture(GLuint texture) { mTexture = texture; }

	unsigned int GetVAO() { return mVAO; }

	void BuildVertexBuffer(std::string path, GLuint& vertexCount);
};