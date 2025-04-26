#pragma once

#include "RHIFramebuffer.h"
#include "RHIVertexArray.h"
#include "RHIShaders.h"

// Hard-limit of vertices. Used to reserve space in vertex and draw command vectors.
#define MAX_VERTICES 100000

struct CameraUniforms final
{
	glm::aligned_mat4 viewProj = glm::aligned_mat4(1.0f);
};

struct VertexWorld final
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 color;
};

struct DrawCommand final
{
	int   type;
	int   vertices;
	bool  depthTest;
	bool  distanceFade;
	float fadeStart;
	float fadeEnd;
};

const glm::vec4 kFrustumCorners[] = {
	glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),  // Far-Bottom-Left
	glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),   // Far-Top-Left
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),    // Far-Top-Right
	glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),   // Far-Bottom-Right
	glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // Near-Bottom-Left
	glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),  // Near-Top-Left
	glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),   // Near-Top-Right
	glm::vec4(1.0f, -1.0f, -1.0f, 1.0f)   // Near-Bottom-Right
};

class DebugDraw final
{
public:
	DebugDraw();

	// Initialization and shutdown.
	bool Init();
	void Close();

	void SetDepthTest(bool depthTest) { m_depthTest = depthTest; }
	bool DepthTest() const { return m_depthTest; }

	void SetDistanceFade(bool fade) { m_distanceFade = fade; }
	bool DistanceFade() const { return m_distanceFade; }

	void SetFadeStart(float fade) { m_fadeStart = fade; }
	bool FadeStart() const { return m_fadeStart; }

	void SetFadeEnd(float fade) { m_fadeEnd = fade; }
	bool FadeEnd() const { return m_fadeEnd; }

	// Debug shape drawing.
	void BeginBatch();
	void EndBatch();
	void Capsule(const float& height, const float& radius, const glm::vec3& pos, const glm::vec3& c);
	void Aabb(const glm::vec3& min, const glm::vec3& max, const glm::vec3& c);
	void Obb(const glm::vec3& min, const glm::vec3& max, const glm::mat4& model, const glm::vec3& c);
	void Grid(const float& x, const float& z, const float& yLevel, const float& spacing, const glm::vec3& c);
	void Grid(const glm::mat4& viewProj, const float& unitSize, const float& highlightUnitSize);
	void Line(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& c);
	void LineStrip(glm::vec3* v, const int& count, const glm::vec3& c);
	void CircleXY(float radius, const glm::vec3& pos, const glm::vec3& c);
	void CircleXZ(float radius, const glm::vec3& pos, const glm::vec3& c);
	void CircleYZ(float radius, const glm::vec3& pos, const glm::vec3& c);
	void Sphere(const float& radius, const glm::vec3& pos, const glm::vec3& c);
	void Frustum(const glm::mat4& view_proj, const glm::vec3& c);
	void Frustum(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& c);
	void Transform(const glm::mat4& trans, const float& axisLength = 5.0f);

	// Render method. Pass in target Framebuffer, viewport size and view-projection matrix.
	void Render(Framebuffer::Ptr fbo, int width, int height, const glm::mat4& viewProj, const glm::vec3& viewPos);

private:
	// Vertex list to be uploaded to GPU.
	std::vector<VertexWorld> m_worldVertices;

	// Draw command list.
	std::vector<DrawCommand> m_drawCommands;

	// Camera matrix.
	CameraUniforms m_uniforms{};

	// Depth state
	bool m_depthTest = false;

	// Batching
	uint32_t m_batchStart = 0;
	uint32_t m_batchEnd = 0;
	bool     m_batchedMode = false;

	// Fading
	float m_fadeEnd = 0.0f;
	float m_fadeStart = 0.0f;
	bool  m_distanceFade = false;

	// GPU resources.
	VertexArray::Ptr m_lineVao;
	Buffer::Ptr      m_lineVbo;
	Shader::Ptr      m_lineVs;
	Shader::Ptr      m_lineFs;
	Program::Ptr     m_lineProgram;
	Buffer::Ptr      m_ubo;
};