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

#include <iostream>
#include "../Include/Renderer.h"
#include "../Include/World.h"
#include "../Include/EventManager.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

	float gravity = 0.0f;
	bool isTestingSleeping = false;
	
	// Choose gravity
	int input = -1;
	while (input < 0 || input > 1) {
		cout << "Enter gravity state. Earth or Moon? (0 for Earth, 1 for Moon): ";
		cin >> input;
		if (input < 0 || input > 1) {
			cout << "Invalid input. try again: ";
		}
	}

	cin.clear();
	cin.ignore();

	// Choose Sleeping State Test
	int input2 = -1;
	while (input2 < 0 || input2 > 1) {
		cout << "Are you testing sleeping State?(0 for no, 1 for yes): ";
		cin >> input2;
		if (input2 < 0 || input2 > 1) {
			cout << "Invalid input. try again: ";
		}
	}
	if (input == 0)
		gravity = 9.807;
	else
		gravity = 1.623;

	if (input2 == 1)
		isTestingSleeping = true;
	else
		isTestingSleeping = false;


	EventManager::Initialize();
	Renderer::Initialize();

	World world = World(gravity, isTestingSleeping);

	// Main Loop
	do
	{
		// Update Event Manager - Frame time / input / events processing 
		EventManager::Update();

		// Update World
		float dt = EventManager::GetFrameTime();
		world.Update(dt);

		// Draw World
		world.Draw();
	} while (EventManager::ExitRequested() == false);

	Renderer::Shutdown();
	EventManager::Shutdown();

	return 0;
}