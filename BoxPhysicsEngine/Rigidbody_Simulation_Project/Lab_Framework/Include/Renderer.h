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

struct GLFWwindow;

// Include GLEW - OpenGL Extension Wrangler
#define GLEW_STATIC 1
#include <GL/glew.h>

#include <string>
#include <vector>
#include <glm/glm.hpp>


class Renderer
{
public:
	static void Initialize();
	static void Shutdown();

	static void BeginFrame();
	static void EndFrame();

	static GLuint LoadShaders(std::string vertex_shader_path, std::string fragment_shader_path);

	static unsigned int GetShaderProgramID() { return sCurrentShader; }
	//static unsigned int GetCurrentShader() { return sCurrentShader; }
	static void SetShader(unsigned int index) { sCurrentShader = sShaderProgramID.at(index); } // index is 0 or 1 (going between main and light shader)

	static void CheckForErrors();
	static bool PrintError();

private:
	static GLFWwindow* spWindow;

	static std::vector<GLuint> sShaderProgramID;
	static unsigned int sCurrentShader;
};
