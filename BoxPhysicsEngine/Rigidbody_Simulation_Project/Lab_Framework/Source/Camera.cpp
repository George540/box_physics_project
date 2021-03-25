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

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "../Include/Camera.h"
#include "../Include/EventManager.h"
#include <GLFW/glfw3.h>
#include <algorithm>

using namespace std;
using namespace glm;


/*
	Camera's Constructor: Camera is initialized in the world scene
*/
Camera::Camera(glm::vec3 position, glm::vec3 lookAtPoint, glm::vec3 upVector) : 
	mPosition(position), 
	mLookAt(lookAtPoint),
	mUpVector(upVector),
	mHorizontalAngle(90.0f), 
	mVerticalAngle(-30.0f),
	mSpeed(10.0f),
	mAngularSpeed(15.0f)
{

};

Camera::~Camera() {};
/*
	Reference #1: Camera's Update: Camera is first-person with WASD movement and Mouse Input rotation
*/
void Camera::Update(float dt)
{
	// Prevent from having the camera move only when the cursor is within the windows
	EventManager::DisableMouseCursor();


	// The Camera moves based on the User inputs
	// - You can access the mouse motion with EventManager::GetMouseMotionXY()
	// - For mapping A S D W, you can look in World.cpp for an example of accessing key states
	// - Don't forget to use dt to control the speed of the camera motion

	// Mouse motion to get the variation in angle
	mHorizontalAngle -= EventManager::GetMouseMotionX() * mAngularSpeed * dt;
	mVerticalAngle -= EventManager::GetMouseMotionY() * mAngularSpeed * dt;

	// Clamp vertical angle to [-85, 85] degrees
	mVerticalAngle = std::max(-85.0f, std::min(85.0f, mVerticalAngle));
	if (mHorizontalAngle > 360)
	{
		mHorizontalAngle -= 360;
	}
	else if (mHorizontalAngle < -360)
	{
		mHorizontalAngle += 360;
	}

	float theta = radians(mHorizontalAngle);
	float phi = radians(mVerticalAngle);

	mLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));

	vec3 sideVector = glm::cross(mLookAt, mUpVector);
	glm::normalize(sideVector);

	// A S D W for motion along the camera basis vectors
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
	{
		mPosition += mLookAt * dt * mSpeed;
	}

	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
	{
		mPosition -= mLookAt * dt * mSpeed;
	}

	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
	{
		mPosition += sideVector * dt * mSpeed;
	}

	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
	{
		mPosition -= sideVector * dt * mSpeed;
	}
}

mat4 Camera::GetViewProjectionMatrix() const
{
	return GetProjectionMatrix() * GetViewMatrix();
}

mat4 Camera::GetViewMatrix() const
{
	// When camera used to be tested in static mode, this snippet was made for the view matrix
	/*
	return glm::lookAt(mPosition,		// Camera position
		mLookAt,	// Look towards this point
		mUpVector		// Up vector
	);
	*/
	return glm::lookAt(mPosition, mPosition + mLookAt, mUpVector);
}

mat4 Camera::GetProjectionMatrix() const
{
	return perspective(radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
}

