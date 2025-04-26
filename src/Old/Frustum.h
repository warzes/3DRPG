#pragma once

struct FrustumPlane final
{
	glm::vec3 normal;
	glm::vec3 position;
	/* the distance seems useless */
	float distance;
};

struct Frustum final
{
	FrustumPlane top, bottom, right, left, far, near;
};

struct Plane final
{
	glm::vec3 n;
	float     d;
};

enum FrustumPlanes
{
	FRUSTUM_PLANE_NEAR = 0,
	FRUSTUM_PLANE_FAR = 1,
	FRUSTUM_PLANE_LEFT = 2,
	FRUSTUM_PLANE_RIGHT = 3,
	FRUSTUM_PLANE_TOP = 4,
	FRUSTUM_PLANE_BOTTOM = 5
};

struct FrustumO2 final
{
	Plane planes[6];
};

inline void FrustumFromMatrix(FrustumO2& frustum, const glm::mat4& view_proj)
{
	frustum.planes[FRUSTUM_PLANE_RIGHT].n = glm::vec3(
		view_proj[0][3] - view_proj[0][0],
		view_proj[1][3] - view_proj[1][0],
		view_proj[2][3] - view_proj[2][0]);
	frustum.planes[FRUSTUM_PLANE_RIGHT].d = view_proj[3][3] - view_proj[3][0];

	frustum.planes[FRUSTUM_PLANE_LEFT].n = glm::vec3(
		view_proj[0][3] + view_proj[0][0],
		view_proj[1][3] + view_proj[1][0],
		view_proj[2][3] + view_proj[2][0]);
	frustum.planes[FRUSTUM_PLANE_LEFT].d = view_proj[3][3] + view_proj[3][0];

	frustum.planes[FRUSTUM_PLANE_BOTTOM].n = glm::vec3(
		view_proj[0][3] + view_proj[0][1],
		view_proj[1][3] + view_proj[1][1],
		view_proj[2][3] + view_proj[2][1]);
	frustum.planes[FRUSTUM_PLANE_BOTTOM].d = view_proj[3][3] + view_proj[3][1];

	frustum.planes[FRUSTUM_PLANE_TOP].n = glm::vec3(
		view_proj[0][3] - view_proj[0][1],
		view_proj[1][3] - view_proj[1][1],
		view_proj[2][3] - view_proj[2][1]);
	frustum.planes[FRUSTUM_PLANE_TOP].d = view_proj[3][3] - view_proj[3][1];

	frustum.planes[FRUSTUM_PLANE_FAR].n = glm::vec3(
		view_proj[0][3] - view_proj[0][2],
		view_proj[1][3] - view_proj[1][2],
		view_proj[2][3] - view_proj[2][2]);
	frustum.planes[FRUSTUM_PLANE_FAR].d = view_proj[3][3] - view_proj[3][2];

	frustum.planes[FRUSTUM_PLANE_NEAR].n = glm::vec3(
		view_proj[0][3] + view_proj[0][2],
		view_proj[1][3] + view_proj[1][2],
		view_proj[2][3] + view_proj[2][2]);
	frustum.planes[FRUSTUM_PLANE_NEAR].d = view_proj[3][3] + view_proj[3][2];

	// Normalize them
	for (int i = 0; i < 6; i++)
	{
		float invl = sqrt(frustum.planes[i].n.x * frustum.planes[i].n.x + frustum.planes[i].n.y * frustum.planes[i].n.y + frustum.planes[i].n.z * frustum.planes[i].n.z);
		frustum.planes[i].n /= invl;
		frustum.planes[i].d /= invl;
	}
}

inline void ExtractFrustumCorners(const glm::mat4& inv_view_proj, glm::vec3* corners)
{
	const glm::vec4 kFrustumCorners[] =
	{
		glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
		glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
		glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
		glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
		glm::vec4(1.0f, -1.0f, -1.0f, 1.0f)
	};

	for (int i = 0; i < 8; i++)
	{
		glm::vec4 v = inv_view_proj * kFrustumCorners[i];
		v = v / v.w;
		corners[i] = glm::vec3(v.x, v.y, v.z);
	}
}
