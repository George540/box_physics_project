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

#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 lookAtPoint, glm::vec3 upVector);
	~Camera();

	void Update(float dt);

	glm::mat4 GetViewMatrix() const;;
	glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetViewProjectionMatrix() const;

private:
	glm::vec3 mPosition;
	float mHorizontalAngle; // horizontal angle
	float mVerticalAngle;   // vertical angle
	glm::vec3 mLookAt;
	glm::vec3 mUpVector;

	float mSpeed;			// World units per second
	float mAngularSpeed;    // Degrees per pixel
};
