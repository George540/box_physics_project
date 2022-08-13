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

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include "../Include/Object.h"
#include "../Include/Renderer.h"
#include "../Include/OBJLoader.h"
#include "../Include/World.h"
#include "../Include/CollisionHelper.h"

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

// these namespaces are included during implementation
using namespace glm;
using namespace std;

//class CollisionHelper; 

/*
	Default constructor for Cube: used to create surface for world scene
*/
Cube::Cube() : mPosition(vec3(0.0f, -0.5f, 0.0f)),
	mRotation(vec3(0.0f, 1.0f, 0.0f)),
	mRotationAngleInDegrees(0.0f),
	mScaling(vec3(50.0f, 1.0f, 50.0f)),
	mHalfWidths(mScaling * 0.5f),
	// Radius: equation taken from Reference Source #2, but did not finally implement
	mRadius(sqrt(mHalfWidths.x* mHalfWidths.x + mHalfWidths.y * mHalfWidths.y + mHalfWidths.z * mHalfWidths.z)),
	mGravity(0.0f),
	mMass(11000.0f), // set mass incredibly high so the object is labeled as static
	mLinearVelocity(vec3(0.0f, 0.0f, 0.0f)), // surface is always asleep
	mAngularVelocity(vec3(0.0f, 0.0f, 0.0f)), // surface is still a box
	mInvInersia(mat4(1.0f)),
	mIsAsleep(true),
	mKineticEnergy(2 * SLEEPEPSILON), // start with minimal kinetic energy
	mIsBox(true) // set a default value for inertia tensor
{
	// Initialize VAO and VBO until we build them at the end of constructor
	mVAO = NULL;
	mVBO = NULL;

	// Set shader program compiled during the Renderer's initialization
	Renderer::SetShader(0);
	mLightLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColor"); // Light Color


	CalculateInverseInertia();

	// Load proper .obj model
	if (mIsBox)
		BuildVertexBuffer("../Source/cube.obj", mNumOfVertices);
	else
		BuildVertexBuffer("../Source/sphere.obj", mNumOfVertices);

	// Initialize Angular Velocity Axis same as rotational axis
	// They must be the same in the beginning
	mAngularVelocity = mRotation;
	// Normalize rotational axis
	normalize(mRotation);

	// Set World Transformation Matrix based on the orientation calculated above
	UpdateWorldMatrix();
};

/*
	Cube's Destructor: used to deallocate memory
*/
Cube::~Cube()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

/*
	Cube's copy constructor: used to create an exact copy of a given cube (used a lot during testing alongside its accessor functions)
*/
Cube::Cube(const Cube& other)
{
	mPosition = other.mPosition;
	mRotation = other.mRotation;
	mScaling = other.mScaling;
	mHalfWidths = other.mHalfWidths;
	mRadius = other.mRadius;
	mGravity = other.mGravity;
	mMass = other.mMass;
	mIsAsleep = other.mIsAsleep;
	mLinearVelocity = other.mLinearVelocity;
	mAngularVelocity = other.mAngularVelocity;
	mKineticEnergy = other.mKineticEnergy;
	mInvInersia = other.mInvInersia;
	mVAO = other.mVAO;
	mVBO = other.mVBO;
	mVertexPositions = other.mVertexPositions;
	mIsAsleep = other.mIsAsleep;
	mIsBox = other.mIsBox;
	mRotationAngleInDegrees = other.mRotationAngleInDegrees;

	mAngularVelocity = mRotation;
	normalize(mRotation);
};

/*
	Cube's parametized constructor:
	Parameters:
		1. Position
		2. Rotational Axis
		3. Scale
		4. Mass
		5. Sleeping State
		6. Object Type
		7. Texture ID
*/
Cube::Cube(glm::vec3& position, glm::vec3& rotation, glm::vec3& scaling, float mass, bool isAsleep, bool isBox, GLuint texture) :
	mPosition(position),
	mRotation(rotation),
	mRotationAngleInDegrees(0.0f), // initial angle set to 0 degrees
	mScaling(scaling),
	mHalfWidths(mScaling * 0.5f),
	mRadius(sqrt(mHalfWidths.x* mHalfWidths.x + mHalfWidths.y * mHalfWidths.y + mHalfWidths.z * mHalfWidths.z)),
	mMass(mass),
	mAngularVelocityInDegrees(0.0f),
	mIsAsleep(isAsleep),
	mTexture(texture),
	mIsBox(isBox)
{
	mLinearVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	mKineticEnergy = SLEEPEPSILON + (isAsleep * SLEEPEPSILON); // depending on the sleeping state initialized, kinetic energy is calculated, even if it's extremely small
	mLinearVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	mInvInersia = glm::mat4(1.0f);
	mVAO = NULL;
	mVBO = NULL;
	//mVertexPositions[36];

	Renderer::SetShader(0);
	mLightLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColor"); // Light Color

	// Box is automatically not asleep when initialized
	mIsAsleep = false;

	if (mIsBox)
		BuildVertexBuffer("../Source/cube.obj", mNumOfVertices);
	else
		BuildVertexBuffer("../Source/sphere.obj", mNumOfVertices);

	mAngularVelocity = mRotation;
	normalize(mRotation);

	CalculateInverseInertia();

	// World Matrix Update
	UpdateWorldMatrix();
};

/*
	Draw cube with its corresponding VAO and texture
	Also light lovation is computed depending on the angle of the surface
*/
void Cube::Draw()
{
	// Draw the Vertex Buffer
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVAO);
	
	// Bind texture
	glBindTexture(GL_TEXTURE_2D, mTexture);
	const GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "worldMatrix");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldTransformationMatrix()[0][0]);
	glUniform3fv(mLightLocation, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, mNumOfVertices); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
}

// TODO - FIX ROTATION STUFF
void Cube::Update(float dt)
{

	// Immovable or asleep objects do not move
	if (mMass > MAXIMUMMASS || mIsAsleep)
		return;

	// Add gravity
	if (mMass > 0 && mMass < MAXIMUMMASS && !mIsAsleep)
		AddForceAcceleration(vec3(0.0f, -mGravity * mMass, 0.0f), dt);
		//AddForce(mPosition, vec3(0.0f, -9.807f * mMass, 0.0f), vec3(0.0f));

	// Reference 2: attempted energy calculations for updating sleeping state of cube
	// Update Kinetic Energy
	// KE = 0.5 * mass * (lv^2) + 0.5 * Inertia * (av^2)
	const float kE = dot(mLinearVelocity, mLinearVelocity) + dot(mAngularVelocity, mAngularVelocity);
	mKineticEnergy = mix(kE, mKineticEnergy, 0.98f);
	// Clamp kinetic energy for not getting absurd results
	//if (mKineticEnergy > 50.0f) { mKineticEnergy = 5.0f; }

	// POSITION AND ORIENTATION
	mPosition += dt * mLinearVelocity;
	mRotation = mix(mRotation, mAngularVelocity, dt);
	mRotationAngleInDegrees += dt * mAngularVelocityInDegrees;

	// Updating World Tranasformation Matrix with new orientation values
	UpdateWorldMatrix();
}

/*
	Updating World Transformation Matrix used for the Model-View-Projection transformation model on vertex shader
*/
void Cube::UpdateWorldMatrix()
{
	// World Matrix Update
	auto model = mat4(1.0f);
	mWorldTransformationMatrix = translate(model, mPosition) *
		rotate(model, radians(mRotationAngleInDegrees), mRotation) *
		scale(model, mScaling);
}

/*
	Rference 2: Setting Sleeping State based on kinetic energy
*/
void Cube::SetSleepingState(bool isAsleep)
{
	mIsAsleep = isAsleep;
	mKineticEnergy = SLEEPEPSILON + (isAsleep * SLEEPEPSILON);
}

/*
	Original used to update the net force and torque of object
*/
/*
void Cube::AddForce(vec3 position, vec3 force, vec3 torque)
{
	if (mMass <= MAXIMUMMASS || mIsAsleep) {
		mForces += force;
		mAngularVelocity = torque;
		mAngularVelocityInDegrees += dot(mAngularVelocity, mRotation);
	}
}
*/

/*
	Reference 2: Calculating Inverse Inertia using the Inertia Tensor
*/
void Cube::CalculateInverseInertia()
{
	vec3 size = mHalfWidths * 2.0f;

	auto x2 = (size.x * size.x);
	auto y2 = (size.y * size.y);
	auto z2 = (size.z * size.z);
	auto ix = (y2 + z2) * mMass / 12.0f;
	auto iy = (x2 + z2) * mMass / 12.0f;
	auto iz = (x2 + y2) * mMass / 12.0f;


	const mat4 inertiaTensor(ix, 0, 0, 0,
	                         0, iy, 0, 0,
	                         0, 0, iz, 0,
	                         0, 0, 0, 1);

	mInvInersia = inverse(inertiaTensor);
}

/*
	Accelerating and Angulating object using their velocities to update orientation
*/
void Cube::AddForceAcceleration(glm::vec3 force, float dt)
{
	if (mMass > 0 && mMass < MAXIMUMMASS && !mIsAsleep) {
		mLinearVelocity += force * dt;
	}
}

void Cube::AddTorqueAcceleration(glm::vec3 torque)
{
	if (mMass > 0 && mMass < MAXIMUMMASS && !mIsAsleep) {
		mAngularVelocity = torque;
		mAngularVelocityInDegrees += dot(mAngularVelocity, mRotation);
	}
}

// Reference 2 and 3: IMPULSE FORMULA FOR COLLISION RESOLUTION
//----------------------------------------------------------
// 
// j =			                           -(1+Cor)(relv.norm)
//	     -----------------------------------------------------------------------------------
//	     norm.norm(1/Mass0 + 1/Mass1) + (sqr(r0 x norm) / Inertia0) + (sqr(r1 x norm) / Inertia1)
//
//----------------------------------------------------------

void Cube::AddCollisionImpulse(Cube& c0, Cube& c1, vec3& hitPoint, vec3 normal, float dt, float penetration)
{
	// Some simple check code.
	if (dt <= 0.0) { return; }

	auto invMass0 = (c0.GetMass() > 1000.0f) ? 0.0f : (1.0f / c0.GetMass());
	auto invMass1 = (c1.GetMass() > 1000.0f) ? 0.0f : (1.0f / c1.GetMass());

	invMass0 = (c0.IsAsleep()) ? 0.0f : invMass0;
	invMass1 = (c1.IsAsleep()) ? 0.0f : invMass1;

	// Both objects are non movable
	if ((invMass0 + invMass1) == 0.0) return;


	vec3 r0 = hitPoint - c0.GetPosition();
	vec3 r1 = hitPoint - c1.GetPosition();

	vec3 v0 = c0.GetLinearVelocity() + cross(c0.GetAngularVelocity(), r0);
	vec3 v1 = c1.GetLinearVelocity() + cross(c1.GetAngularVelocity(), r1);

	// Relative Velocity
	vec3 dv = v0 - v1;

	// NORMAL Impulse Code

	// Compute Normal Impulse
	float vn = dot(dv, normal);

	// Works out the bias to prevent Prevents sinking!
	const auto allowedPenetration = 0.1f;
	const auto biasFactor = 0.1f; // 0.1 to 0.3
	bool g_positionCorrection = true;
	float biasFactorValue = g_positionCorrection ? biasFactor : 0.0f;

	float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;
	float bias = biasFactorValue * inv_dt * glm::max(0.0f, penetration - allowedPenetration);


	float kNormal = invMass0 + invMass1 +

		dot(normal,
			cross(vec3((vec4(cross(r0, normal), 1.0f) * c0.GetInverseInersia())), r0) +
			cross(vec3((vec4(cross(r1, normal), 1.0f) * c1.GetInverseInersia())), r1)
		);

	float massNormal = 1.0f / kNormal;


	float dPn = massNormal * (-vn + bias);

	dPn = glm::max(dPn, 0.0f);


	// Apply normal contact impulse
	vec3 P = dPn * normal;

	c0.GetLinearVelocity() += invMass0 * P;
	c0.GetAngularVelocity() += vec3(vec4(cross(r0, P), 1.0f) * c0.GetInverseInersia());

	c1.GetLinearVelocity() -= invMass1 * P;
	c1.GetAngularVelocity() -= vec3(vec4(cross(r1, P), 1.0f) * c1.GetInverseInersia());
	// NORMAL



	// TANGENT Impulse Code
	{
		// Work out our tangent vector, with is perpendicular
		// to our collision normal
		auto tangent = vec3(0.0f, 0.0f, 0.0f);
		tangent = dv - (dot(dv, normal) * normal);
		tangent = normalize(tangent);

		float kTangent = invMass0 + invMass1 +
			
			//cross(vec3((vec4(cross(r0, normal), 1.0f) * c0.GetInverseInersia())), r0)
			dot(tangent,
				cross(vec3((vec4(cross(r0, tangent), 1.0f) * c0.GetInverseInersia())), r0) +
				cross(vec3((vec4(cross(r1, tangent), 1.0f) * c1.GetInverseInersia())), r1)
			);

		float massTangent = 1.0f / kTangent;


		float vt = dot(dv, tangent);
		float dPt = massTangent * (-vt);


		float maxPt = 0.6f * dPn;
		//dPt = Clamp(dPt, -maxPt, maxPt);
		dPt = dPt > maxPt ? maxPt : dPt;
		dPt = dPt < -maxPt ? -maxPt : dPt;


		// Apply contact impulse
		vec3 P = dPt * tangent;

		c0.GetLinearVelocity() += invMass0 * P;
		c0.GetAngularVelocity() += vec3(vec4(cross(r0, P), 1.0f) * c0.GetInverseInersia());

		c1.GetLinearVelocity() -= invMass1 * P;
		c1.GetAngularVelocity() -= vec3(vec4(cross(r1, P), 1.0f) * c1.GetInverseInersia());
	}
	// TANGENT
}

/*
	Building Vertex Buffer using the OBJLoader: creates vectors for vertices, normals and UVs for VBO setup
*/
void Cube::BuildVertexBuffer(std::string path, GLuint& vertexCount) {
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> UVs;

	//read the vertex data from the model's OBJ file
	loadOBJ(path.c_str(), vertices, normals, UVs);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Vertex VBO setup
	GLuint vertices_VBO;
	glGenBuffers(1, &vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Normals VBO setup
	GLuint normals_VBO;
	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	//UVs VBO setup
	GLuint uvs_VBO;
	glGenBuffers(1, &uvs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	vertexCount = vertices.size();
	mNumOfVertices = vertexCount;
	mVAO = VAO;

	for (int i = 0; i < mNumOfVertices; i++) {
		mVertexPositions.push_back(vertices.at(i));
	}
}

/*
	Simple Collision Resolution for bouncing off infinite ground surface
	It simply reverces the velocity on y-axis
	There are two options: either simulate gravity by reducing velocity OR reverse velocity.y for infinite bouncing
	Clever idea used on Reference #1
*/
void Cube::BounceOffGround(bool isTestingSleep)
{
	if (mMass > 0.0f && mMass < 10000.0f && !mIsAsleep) {
		if (isTestingSleep == true) {
			//cout << "mLinearVelocity.y: " << mLinearVelocity.y << endl;
			if (abs(mLinearVelocity.y) < 0.1f) {
				mLinearVelocity = vec3(0.0f, 0.0f, 0.0f);
				mIsAsleep = true;
			}
			else
				mLinearVelocity.y = abs(mLinearVelocity.y * 0.75f);
		}
		else {
			if (abs(mLinearVelocity.y) < 0.1f) {
				mLinearVelocity = vec3(0.0f, 0.0f, 0.0f);
				mIsAsleep = true;
			}
			else
				mLinearVelocity.y = abs(mLinearVelocity.y);
		}
	}
}

/*
	Reference #1: raycasting on a cube model and applying impulses based on the ray's intersection point
*/
float Cube::IntersectsRay(glm::vec3 source, glm::vec3 direction)
{
	float intersection = std::numeric_limits<float>::infinity() * -1.0f;
	for (int i = 0; i != mVertexPositions.size(); i += 3)
	{
		//Three corners of a triangle
		glm::vec3 p0 = mWorldTransformationMatrix * glm::vec4(mVertexPositions.at(i), 1.0f);
		glm::vec3 p1 = mWorldTransformationMatrix * glm::vec4(mVertexPositions.at(i + 1), 1.0f);
		glm::vec3 p2 = mWorldTransformationMatrix * glm::vec4(mVertexPositions.at(i + 2), 1.0f);

		//define a plane based on triangle vertices
		glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));


		//Check plane is facing us
		if (glm::dot(normal, direction) < 0) {
			float t = (glm::dot(normal, p0) - glm::dot(normal, source)) / glm::dot(normal, direction);
			//check collision happens forward in time
			if (t >= 0) {
				//Plane contains point, now what about the triangle?
				glm::vec3 intersectionInPlane = source + t * direction;

				//Projection onto a plane, we must pick a plane not perpendicular to triangle, to avoid collapsing the triangle into a single line
				//At least one of these is guaranteed to be non-perpendicular. All are centered at the origin
				auto xNormal = glm::vec3(1, 0, 0);
				auto yNormal = glm::vec3(0, 1, 0);
				auto zNormal = glm::vec3(0, 0, 1);

				glm::vec2 a;
				glm::vec2 b;
				glm::vec2 c;
				glm::vec2 r; //Ray intersection point				

				if (glm::dot(xNormal, normal) != 0.0f)
				{ //Project on yz-plane
					a = glm::vec2(p0.y, p0.z);
					b = glm::vec2(p1.y, p1.z);
					c = glm::vec2(p2.y, p2.z);
					r = glm::vec2(intersectionInPlane.y, intersectionInPlane.z);
				}
				else if (glm::dot(yNormal, normal) != 0.0f)
				{ //Project on xz-plane
					a = glm::vec2(p0.z, p0.x);
					b = glm::vec2(p1.z, p1.x);
					c = glm::vec2(p2.z, p2.x);
					r = glm::vec2(intersectionInPlane.z, intersectionInPlane.x);
				}
				else
				{ //Project on xy-plane
					a = glm::vec2(p0.x, p0.y);
					b = glm::vec2(p1.x, p1.y);
					c = glm::vec2(p2.x, p2.y);
					r = glm::vec2(intersectionInPlane.x, intersectionInPlane.y);
				}
				float areaABC = glm::length(glm::cross(glm::vec3(b - a, 0.0f), glm::vec3(c - a, 0.0f))) / 2.0f;

				float areaABP = glm::length(glm::cross(glm::vec3(b - a, 0.0f), glm::vec3(r - a, 0.0f))) / 2.0f;
				float areaACP = glm::length(glm::cross(glm::vec3(r - a, 0.0f), glm::vec3(c - a, 0.0f))) / 2.0f;
				float areaBCP = glm::length(glm::cross(glm::vec3(b - r, 0.0f), glm::vec3(c - r, 0.0f))) / 2.0f;

				float alpha = areaBCP / areaABC;
				float beta = areaACP / areaABC;
				float gamma = areaABP / areaABC; //Recalculate this!!!!

				if ((alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f) && (alpha + beta + gamma == 1.0f))
				{
					intersection = t;
				}
			}
		}
	}

	return intersection;
}

/*
	Does the cube penetrate the ground's infinite surface?
*/
bool Cube::PenetratesGround(glm::vec3 planePoint, glm::vec3 planeNormal)
{
	// Iterate through all vertices of the cube
	for (int i = 0; i < mVertexPositions.size(); i++) {
		// Convert to world coordinates
		vec3 vertexWorldCoordinates = mWorldTransformationMatrix * vec4(mVertexPositions.at(i), 1.0f);
		// Are they located bellow the ground?
		if (vertexWorldCoordinates.y <= planePoint.y) {
			vec3 a = vertexWorldCoordinates - mPosition;
			vec3 v = mPosition;

			// ATTEMPTED: Used the projected version of the position-to-vertex projection to reposition cube outside of the penetration at time t
			// This would ultimately avoid continuous collisions when cubes penetrate the surface
			// Problem: improper projection of vector positions cube in wrong position
			vec3 mNewPosition = ((a * v) / normalize(v) * normalize(v)) * v;
			//mPosition = mNewPosition;

			return true;
		}
	}
	return false;
}