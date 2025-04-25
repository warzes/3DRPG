#pragma once

#include "Frustum.h"

struct AABB final
{
	glm::vec3 min;
	glm::vec3 max;
};

inline float Classify(const Plane& plane, const AABB& aabb)
{
	glm::vec3 center = (aabb.max + aabb.min) / 2.0f;
	glm::vec3 extents = aabb.max - aabb.min;

	float r = fabsf(extents.x * plane.n.x) + fabsf(extents.y * plane.n.y) + fabsf(extents.z * plane.n.z);

	float d = glm::dot(plane.n, center) + plane.d;

	if (fabsf(d) < r)
		return 0.0f;
	else if (d < 0.0f)
		return d + r;
	else
		return d - r;
}

inline bool Intersects(const FrustumO2& frustum, const AABB& aabb)
{
	for (int i = 0; i < 6; i++)
	{
		if (Classify(frustum.planes[i], aabb) < 0.0f)
			return false;
	}

	return true;
}