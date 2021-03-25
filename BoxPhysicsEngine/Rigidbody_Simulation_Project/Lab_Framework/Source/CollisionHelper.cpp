#include "../Include/World.h"
#include "../Include/Object.h"
#include "../Include/CollisionHelper.h"

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

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

using namespace std;
using namespace glm;

// Helper methods

/*
inline float Clamp(float v, float min, float max)
{
	float res = v;
	res = v > max ? max : v;
	res = v < min ? min : v;
	return res;
};

// Calculates the box's boundaries via its vertices, converts to world coordinates
void CalculateInterval(const Cube& box,
	const vec3 axis,
	float& min,
	float& max)
{
	float x = box.GetHalfWidths().x;
	float y = box.GetHalfWidths().y;
	float z = box.GetHalfWidths().z;

	// 
	vec3 vertices[8] =
	{
		vec3(x, y, z),
		vec3(-x, y, z),
		vec3(x, -y, z),
		vec3(-x, -y, z),

		vec3(x, y, -z),
		vec3(-x, y, -z),
		vec3(x, -y, -z),
		vec3(-x, -y, -z),
	};

	for (int i = 0; i < 8; i++)
	{
		vertices[i] = vec3(vec4(vertices[i], 1.0f) * box.GetWorldTransformationMatrix());
	}

	min = max = glm::dot(vertices[0], axis);

	for (int i = 0; i < 8; i++)
	{
		float d = glm::dot(vertices[i], axis);

		if (d < min) min = d;
		if (d > max) max = d;
	}
};

// Check if box's vertices penetrate another box
bool SpanIntersect(const Cube& box1,
	const Cube& box2,
	const glm::vec3 collisionAxis,
	float* minPenetration = NULL,
	glm::vec3* axisPenetration = NULL,
	float* penetration = NULL)
{
	glm::vec3 axis = collisionAxis;
	float squaredVectorLength = glm::length(axis) * glm::length(axis);

	if (squaredVectorLength <= 0.2f) {
		if (penetration) *penetration = 100000.0f;
		return true;
	}

	axis = normalize(axis);

	float mina, maxa;
	float minb, maxb;
	CalculateInterval(box1, axis, mina, maxa);
	CalculateInterval(box1, axis, minb, maxb);

	float length_a = maxa - mina;
	float length_b = maxb - minb;

	float minv = glm::min(mina, minb);
	float maxv = glm::max(maxa, maxb);
	float length_v = maxv - minv;

	if (length_v > (length_a + length_b))
	{
		// No collision occured
		return false;
	}

	float pen = (length_a + length_b) - length_v;

	if (penetration)
		*penetration = pen;

	if (minPenetration && axisPenetration) {
		if (pen < *minPenetration) {
			*minPenetration = pen;
			*axisPenetration = axis;

			// BoxA pushes BoxB away in the correct Direction
			if (minb < mina)
			{
				// Switch direction of penetrated normal
				*axisPenetration *= -1;
			}
		}
	}

	return true; //collision occured
}

int GetNumOfHitPoints(const Cube& box,
	const vec3 hitNormal,
	const float penetration,
	vec3 verts[8],
	int* vertIndices)
{
	float x = box.GetHalfWidths().x;
	float y = box.GetHalfWidths().y;
	float z = box.GetHalfWidths().z;

	// 
	vec3 vertices[8] =
	{
		vec3(x, y, z),
		vec3(-x, y, z),
		vec3(x, -y, z),
		vec3(-x, -y, z),

		vec3(x, y, -z),
		vec3(-x, y, -z),
		vec3(x, -y, -z),
		vec3(-x, -y, -z),
	};

	for (int i = 0; i < 8; i++)
	{
		vertices[i] = vec3(vec4(vertices[i], 1.0f) * box.GetWorldTransformationMatrix());
	}

	// Find furthest plane point within vertices
	vec3 planePoint = vertices[0];
	float maxDistance = dot(vertices[0], hitNormal);

	for (int i = 0; i < 8; i++)
	{
		float d = dot(vertices[i], hitNormal);
		if (d > maxDistance)
		{
			maxDistance = d;
			planePoint = vertices[i];
		}
	}

	// a*x + b*y + c*z = 0
	// Plane Equation (A dot N) - d = 0;
	
	float d = dot(planePoint, hitNormal);
	d -= penetration + 0.01f;


	int numVerts = 0;
	for (int i = 0; i < 8; i++)
	{
		float side = dot(vertices[i], hitNormal) - d;

		if (side > 0)
		{
			verts[numVerts] = vertices[i];
			vertIndices[numVerts] = i;
			numVerts++;
		}
	}

	// Return vertices that are forming the hit point
	return numVerts;
}

void SortVertices(vec3* verts,
	int* vertIndices)
{
	int faces[6][4] =
	{
		{4,0,3,7},
		{1,5,6,2},
		{0,1,2,3},
		{7,6,5,4},
		{5,1,0,4},
		{6,7,3,2}
	};

	int faceSet = -1;
	vec3 temp[4]; // New correct clockwise order

	// Work out which face to use
	for (int i = 0; i < 6 && faceSet == -1; i++)
	{
		int numFound = 0;
		for (int j = 0; j < 4; j++)
		{
			if (vertIndices[j] == faces[i][j])
			{
				temp[numFound] = verts[j];
				numFound++;

				if (numFound == 4)
				{
					faceSet = i;
					break;
				}
			}
		}
	}

	if (faceSet < 0)
	{
		int errorHasOccured = 1;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			verts[i] = temp[i];
		}
	}
}

bool VertInsideFace(vec3* verts0, vec3& p0, float planeErr = 0.0f)
{
	// Work out the normal for the face
	vec3 v0 = verts0[1] - verts0[0];
	vec3 v1 = verts0[2] - verts0[0];
	vec3 n = glm::cross(v1, v0);
	n = normalize(n);

	for (int i = 0; i < 4; i++)
	{
		vec3 s0 = verts0[i];
		vec3 s1 = verts0[(i + 1) % 4];
		vec3 sx = s0 + n * 10.0f;

		// Work out the normal for the face
		vec3 sv0 = s1 - s0;
		vec3 sv1 = sx - s0;
		vec3 sn = glm::cross(sv1, sv0);
		sn = normalize(sn);

		float d = glm::dot(s0, sn);
		float d0 = glm::dot(p0, sn) - d;

		// Outside the plane
		if (d0 > planeErr)
		{
			return false;
		}
	}

	return true;
}

void ClipFaceFaceVerts(vec3* verts0,
	int* vertIndexs0,
	vec3* verts1,
	int* vertIndexs1,
	vec3* vertsX,
	int* numVertsX)
{
	SortVertices(verts0, vertIndexs0);
	SortVertices(verts1, vertIndexs1);

	// Work out the normal for the face
	vec3 v0 = verts0[1] - verts0[0];
	vec3 v1 = verts0[2] - verts0[0];
	vec3 n = glm::cross(v1, v0);
	n = normalize(n);

	// Project all the vertices onto a shared plane, plane0
	vec3 vertsTemp1[4];
	for (int i = 0; i < 4; i++)
	{
		vertsTemp1[i] = verts1[i] + (n * dot(n, verts0[0] - verts1[i]));
	}


	static vec3 temp[50];
	int numVerts = 0;

	for (int c = 0; c < 2; c++)
	{
		vec3* vertA = vertsTemp1;
		vec3* vertB = verts0;
		if (c == 1)
		{
			vertA = verts0;
			vertB = vertsTemp1;
		}

		// Work out the normal for the face
		vec3 v0 = vertA[1] - vertA[0];
		vec3 v1 = vertA[2] - vertA[0];
		vec3 n = cross(v1, v0);
		n = normalize(n);

		for (int i = 0; i < 4; i++)
		{
			vec3 s0 = vertA[i];
			vec3 s1 = vertA[(i + 1) % 4];
			vec3 sx = s0 + n * 10.0f;

			// Work out the normal for the face
			vec3 sv0 = s1 - s0;
			vec3 sv1 = sx - s0;
			vec3 sn = cross(sv1, sv0);
			sn = normalize(sn);

			float d = dot(s0, sn);


			for (int j = 0; j < 4; j++)
			{
				vec3 p0 = vertB[j];
				vec3 p1 = vertB[(j + 1) % 4]; // Loops back to the 0th for the last one

				float d0 = dot(p0, sn) - d;
				float d1 = dot(p1, sn) - d;

				// Check there on opposite sides of the plane
				if ((d0 * d1) < 0.0f)
				{
					vec3 pX = p0 + (dot(sn, (s0 - p0)) / dot(sn, (p1 - p0))) * (p1 - p0);

					if (VertInsideFace(vertA, pX, 0.1f))
					{
						temp[numVerts] = pX;
						numVerts++;
					}

				}


				if (VertInsideFace(vertA, p0))
				{
					temp[numVerts] = p0;
					numVerts++;
				}


				if (numVerts > 40)
				{
					// We have a fixed array we pass in, which has a max size of 50
					// so if we go past this we'll have memory corruption

					// temp above is size 50 as well
					break;
				}
			}
		}



	}

	// Remove verts which are very close to each other
	for (int i = 0; i < numVerts; i++)
	{
		for (int j = i; j < numVerts; j++)
		{
			if (i != j)
			{
				float dist = length(temp[i] - temp[j]) * length(temp[i] - temp[j]);

				if (dist < 6.5f)
				{

					for (int k = j; k < numVerts; k++)
					{
						temp[k] = temp[k + 1];
					}
					numVerts--;
				}
			}
		}
	}

	*numVertsX = numVerts;
	for (int i = 0; i < numVerts; i++)
	{
		vertsX[i] = temp[i];
	}
}

inline void ClosestPtPointOBB(const vec3& point,
	const Cube& box0,
	vec3* closestP)
{
	vec3 d = point - box0.GetPosition();

	vec3 q = box0.GetPosition();


	for (int i = 0; i < 3; i++)
	{
		float dist = dot(d, box0.GetLocalAxes()[i]);

		if (dist > box0.GetHalfWidths()[i]) dist = box0.GetHalfWidths()[i];
		if (dist < -box0.GetHalfWidths()[i]) dist = -box0.GetHalfWidths()[i];

		q += dist * box0.GetLocalAxes()[i];
	}

	*closestP = q;
}

void ClipLinePlane(const vec3* verts0,	// 2 verts
	const int* vertIndexs0,
	const Cube& box0,
	const vec3* verts1, // 4 verts
	int* vertIndexs1,
	const Cube& box1,
	vec3* vertsX,
	int* numVertX)
{
	ClosestPtPointOBB(verts0[0], box1, &vertsX[0]);
	ClosestPtPointOBB(verts0[1], box1, &vertsX[1]);
	*numVertX = 2;
}

void ClosestPointLineLine(const vec3* verts0,	// 2 verts
	const vec3* verts1, // 2 verts
	vec3* vertsX,
	int* numVertX)
{
	vec3 p1 = verts0[0];
	vec3 q1 = verts0[1];
	vec3 p2 = verts1[0];
	vec3 q2 = verts1[1];

	vec3 d1 = q1 - p1;
	vec3 d2 = q2 - p2;
	vec3 r = p1 - p2;
	float a = dot(d1, d1);
	float e = dot(d2, d2);
	float f = dot(d2, r);

	const float epsilon = 0.00001f;

	float s, t;
	vec3 c1, c2;

	if (a <= epsilon && e <= epsilon)
	{
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;

		vertsX[0] = c1;
		*numVertX = 1;
		return;
	}

	if (a <= epsilon)
	{
		s = 0.0f;
		t = f / e;
		t = Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = dot(d1, r);
		if (e <= epsilon)
		{
			t = 0.0f;
			s = Clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			float b = dot(d1, d2);
			float denom = a * e - b * b;

			if (denom != 0.0f)
			{
				s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}
			else
			{
				s = 0.0f;
			}

			t = (b * s + f) / e;

			if (t < 0.0f)
			{
				t = 0.0f;
				s = Clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = Clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;

	vertsX[0] = (c1 + c2) * 0.5f;
	*numVertX = 1;
}

void CalculateHitPoint(const Cube& box0,
	const Cube& box1,
	const float penetration,
	vec3& hitNormal,
	vec3* hitPoints,
	int* numHitPoints)
{
	vec3 verts0[8];
	int vertIndex0[8];
	vec3 norm0 = hitNormal;
	int numVerts0 = GetNumOfHitPoints(box0,
		norm0,
		penetration,
		verts0,
		vertIndex0);

	vec3 verts1[8];
	int vertIndex1[8];
	vec3 norm1 = -hitNormal;
	int numVerts1 = GetNumOfHitPoints(box1,
		norm1,
		penetration,
		verts1,
		vertIndex1);

	// This should never really happen!
	if (numVerts0 == 0 || numVerts1 == 0)
	{
		return;
	}


	int numVertsX = numVerts0;
	vec3* vertsX = verts0;

	if (numVerts0 >= 4 && numVerts1 >= 4)
	{
		static vec3 clipVerts[50];

		ClipFaceFaceVerts(verts0, vertIndex0,
			verts1, vertIndex1,
			clipVerts, &numVertsX);

		vertsX = clipVerts;
	}


	{
		// TO-DO - work out which one is the least number
		// of verts, and use that, if both have two, work out
		// the edge point exactly...if its just a single point, only
		// use that single vert

		// TO-DO** TO-DO
		//int numVertsX = numVerts0;
		//D3DXVECTOR3* vertsX = verts0;


		if (numVerts1 < numVerts0)
		{
			numVertsX = numVerts1;
			vertsX = verts1;
			hitNormal = -norm1;
		}

		if (numVerts1 == 2 && numVerts0 == 2)
		{
			static vec3 V[2];
			static int numV = 0;

			ClosestPointLineLine(verts0,
				verts1,
				V, &numV);

			vertsX = V;
			numVertsX = numV;
		}

		if (numVerts0 == 2 && numVerts1 == 4)
		{
			ClipLinePlane(verts0, vertIndex0, box0,
				verts1, vertIndex1, box1,
				vertsX, &numVertsX);
		}

		if (numVerts0 == 4 && numVerts1 == 2)
		{
			ClipLinePlane(verts1, vertIndex1, box1,
				verts0, vertIndex0, box0,
				vertsX, &numVertsX);
		}



		/*
		static debugsphere sphere0(g_pD3DDevice, 0xff00ff00);
		for (int i=0; i<numVerts0; i++)
		{
			sphere0.Draw(g_pD3DDevice, verts0[i].x, verts0[i].y, verts0[i].z, 0.5f);
		}

		static debugsphere sphere1(g_pD3DDevice, 0xff005500);
		for (int i=0; i<numVerts1; i++)
		{
			sphere1.Draw(g_pD3DDevice, verts1[i].x, verts1[i].y, verts1[i].z, 0.5f);
		}
		



		*hitPoint = D3DXVECTOR3(0,0,0);
		// Work out the average hit point
		for (int i=0; i<numVertsX; i++)
		{
			*hitPoint += vertsX[i];
		}
		*hitPoint = (*hitPoint) * (1.0f/numVertsX);

		* numHitPoints = numVertsX;
		for (int i = 0; i < numVertsX; i++)
		{
			hitPoints[i] = vertsX[i];
		}
	}


}

bool CubeCubeCollisionCheck(const Cube& box0,
	const Cube& box1,
	vec3* hitPoints,
	int* numHitPoints,
	float* penetration,
	vec3* hitNormal)
{

	// Simple bounding sphere check first
	float len = box0.GetRadius() + box1.GetRadius();
	if ((length(box1.GetPosition() - box0.GetPosition()) * length(box1.GetPosition() - box0.GetPosition())) > (len * len))
	{
		return false;
	}

	bool hit = true;
	float p = 10000.0f;
	vec3 h;

	//cout << box0.GetLocalAxes()[0].x << " " << box0.GetLocalAxes()[0].y << " " << box0.GetLocalAxes()[0].z << endl;
	//cout << box0.GetLocalAxes()[1].x << " " << box0.GetLocalAxes()[1].y << " " << box0.GetLocalAxes()[1].z << endl;
	//cout << box0.GetLocalAxes()[2].x << " " << box0.GetLocalAxes()[2].y << " " << box0.GetLocalAxes()[2].z << endl;


	hit &= SpanIntersect(box0, box1, box0.GetLocalAxes()[0], &p, &h);
	hit &= SpanIntersect(box0, box1, box0.GetLocalAxes()[1], &p, &h);
	hit &= SpanIntersect(box0, box1, box0.GetLocalAxes()[2], &p, &h);

	hit &= SpanIntersect(box0, box1, box1.GetLocalAxes()[0], &p, &h);
	hit &= SpanIntersect(box0, box1, box1.GetLocalAxes()[1], &p, &h);
	hit &= SpanIntersect(box0, box1, box1.GetLocalAxes()[2], &p, &h);

	hit &= SpanIntersect(box0, box1, cross(box0.GetLocalAxes()[0], box1.GetLocalAxes()[0]), &p, &h);
	hit &= SpanIntersect(box0, box1, cross(box0.GetLocalAxes()[0], box1.GetLocalAxes()[1]), &p, &h);
	hit &= SpanIntersect(box0, box1, cross(box0.GetLocalAxes()[0], box1.GetLocalAxes()[2]), &p, &h);

	hit &= SpanIntersect(box0, box1, cross(box0.GetLocalAxes()[1], box1.GetLocalAxes()[0]), &p, &h);
	hit &= SpanIntersect(box0, box1, cross(box0.GetLocalAxes()[1], box1.GetLocalAxes()[1]), &p, &h);
	hit &= SpanIntersect(box0, box1, cross(box0.GetLocalAxes()[1], box1.GetLocalAxes()[2]), &p, &h);

	hit &= SpanIntersect(box0, box1, cross(box0.GetLocalAxes()[2], box1.GetLocalAxes()[0]), &p, &h);
	hit &= SpanIntersect(box0, box1, cross(box0.GetLocalAxes()[2], box1.GetLocalAxes()[1]), &p, &h);
	hit &= SpanIntersect(box0, box1, cross(box0.GetLocalAxes()[2], box1.GetLocalAxes()[2]), &p, &h);

	*numHitPoints = 0;

	if (penetration && hitNormal && hit)
	{
		CalculateHitPoint(box0,
			box1,
			p,
			h,
			hitPoints,
			numHitPoints);
		*penetration = p;
		*hitNormal = -h;
	}
	return hit;
}

*/