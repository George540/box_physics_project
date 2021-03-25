#pragma once

#include "../Include/Object.h"

bool CubeCubeCollisionCheck(const Cube& box0,
	const Cube& box1,
	glm::vec3* hitPoints,
	int* numHitPoints,
	float* penetration,
	glm::vec3* hitNormal);
