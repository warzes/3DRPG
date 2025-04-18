#include "stdafx.h"
#include "DebugDraw.h"
#include "Core.h"
//=============================================================================
const char* gVSSrc = R"(
layout (location = 0) in vec3 VS_IN_Position;
layout (location = 1) in vec2 VS_IN_TexCoord;
layout (location = 2) in vec3 VS_IN_Color;
	
layout (std140) uniform CameraUniforms //#binding 0
{ 
	mat4 viewProj;
};
	
out vec3 FS_IN_Color;
out vec3 FS_IN_FragPos;
	
void main()
{
	FS_IN_Color = VS_IN_Color;
	FS_IN_FragPos = VS_IN_Position;
	gl_Position = viewProj * vec4(VS_IN_Position, 1.0);
}
)";
//=============================================================================
const char* gFSSrc = R"(
precision mediump float;

out vec4 FS_OUT_Color;

in vec3 FS_IN_Color;
in vec3 FS_IN_FragPos;

uniform vec4 camera_pos;
uniform vec4 fade_params;

void main()
{
	float fade_start = fade_params.x;
	float fade_end = fade_params.y;

	if (fade_start < 0.0)
	FS_OUT_Color = vec4(FS_IN_Color, 1.0);
	else
	{
		float distance = length(FS_IN_FragPos - camera_pos.xyz);
		float after_fade_start = max(distance - fade_start, 0.0);
		float opacity = clamp(1.0 - after_fade_start/(fade_end - fade_start), 0.0, 1.0);

		FS_OUT_Color = vec4(FS_IN_Color, opacity);
	}
}
)";
//=============================================================================
DebugDraw::DebugDraw()
{
	m_worldVertices.resize(MAX_VERTICES);
	m_worldVertices.clear();

	m_drawCommands.resize(MAX_VERTICES);
	m_drawCommands.clear();
}
//=============================================================================
bool DebugDraw::Init()
{
	// Create shaders
	m_lineVs = Shader::Create(GL_VERTEX_SHADER, gVSSrc);
	m_lineFs = Shader::Create(GL_FRAGMENT_SHADER, gFSSrc);

	if (!m_lineVs || !m_lineFs)
	{
		Fatal("Failed to create Shaders");
		return false;
	}

	// Create shader program
	m_lineProgram = Program::Create({ m_lineVs, m_lineFs });

	// Bind uniform block index
	m_lineProgram->UniformBlockBinding("CameraUniforms", 0);

	// Create vertex buffer
	m_lineVbo = Buffer::Create(GL_ARRAY_BUFFER, GL_MAP_WRITE_BIT, sizeof(VertexWorld) * MAX_VERTICES);

	// Declare vertex attributes
	VertexAttrib attribs[] = { 
		{ 3, GL_FLOAT, false, 0 },
		{ 2, GL_FLOAT, false, sizeof(float) * 3 },
		{ 3, GL_FLOAT, false, sizeof(float) * 5 }
	};

	// Create vertex array
	m_lineVao = VertexArray::Create(m_lineVbo, nullptr, sizeof(float) * 8, 3, attribs);

	if (!m_lineVao || !m_lineVbo)
	{
		Fatal("Failed to create Vertex Buffers/Arrays");
		return false;
	}

	// Create uniform buffer for matrix data
	m_ubo = Buffer::Create(GL_UNIFORM_BUFFER, GL_MAP_WRITE_BIT, sizeof(CameraUniforms));

	return true;
}
//=============================================================================
void DebugDraw::Close()
{
	m_lineVbo.reset();
	m_ubo.reset();
}
//=============================================================================
void DebugDraw::BeginBatch()
{
	m_batchedMode = true;
	m_batchStart = m_worldVertices.size();
	m_batchEnd = m_worldVertices.size();
}
//=============================================================================
void DebugDraw::EndBatch()
{
	m_batchedMode = false;
}
//=============================================================================
void DebugDraw::Capsule(const float& height, const float& radius, const glm::vec3& pos, const glm::vec3& c)
{
	// Draw four lines
	Line(glm::vec3(pos.x, pos.y + radius, pos.z - radius),
		glm::vec3(pos.x, height - radius, pos.z - radius),
		c);
	Line(glm::vec3(pos.x, pos.y + radius, pos.z + radius),
		glm::vec3(pos.x, height - radius, pos.z + radius),
		c);
	Line(glm::vec3(pos.x - radius, pos.y + radius, pos.z),
		glm::vec3(pos.x - radius, height - radius, pos.z),
		c);
	Line(glm::vec3(pos.x + radius, pos.y + radius, pos.z),
		glm::vec3(pos.x + radius, height - radius, pos.z),
		c);

	glm::vec3 verts[10];

	int idx = 0;

	for (int i = 0; i <= 180; i += 20)
	{
		float degInRad = glm::radians((float)i);
		verts[idx++] = glm::vec3(pos.x + cos(degInRad) * radius,
			height - radius + sin(degInRad) * radius,
			pos.z);
	}

	LineStrip(&verts[0], 10, c);

	idx = 0;

	for (int i = 0; i <= 180; i += 20)
	{
		float degInRad = glm::radians((float)i);
		verts[idx++] = glm::vec3(pos.x, height - radius + sin(degInRad) * radius, pos.z + cos(degInRad) * radius);
	}

	LineStrip(&verts[0], 10, c);

	idx = 0;

	for (int i = 180; i <= 360; i += 20)
	{
		float degInRad = glm::radians((float)i);
		verts[idx++] = glm::vec3(
			pos.x + cos(degInRad) * radius,
			radius + sin(degInRad) * radius,
			pos.z);
	}

	LineStrip(&verts[0], 10, c);

	idx = 0;

	for (int i = 180; i <= 360; i += 20)
	{
		float degInRad = glm::radians((float)i);
		verts[idx++] = glm::vec3(pos.x, radius + sin(degInRad) * radius, pos.z + cos(degInRad) * radius);
	}

	LineStrip(&verts[0], 10, c);

	CircleXZ(radius, glm::vec3(pos.x, height - radius, pos.z), c);
	CircleXZ(radius, glm::vec3(pos.x, radius, pos.z), c);
}
//=============================================================================
void DebugDraw::Aabb(const glm::vec3& _min, const glm::vec3& _max, const glm::vec3& _c)
{
	glm::vec3 _pos = (_max + _min) * 0.5f;

	glm::vec3 min = _pos + _min;
	glm::vec3 max = _pos + _max;

	BeginBatch();

	Line(min, glm::vec3(max.x, min.y, min.z), _c);
	Line(glm::vec3(max.x, min.y, min.z), glm::vec3(max.x, min.y, max.z), _c);
	Line(glm::vec3(max.x, min.y, max.z), glm::vec3(min.x, min.y, max.z), _c);
	Line(glm::vec3(min.x, min.y, max.z), min, _c);

	Line(glm::vec3(min.x, max.y, min.z), glm::vec3(max.x, max.y, min.z), _c);
	Line(glm::vec3(max.x, max.y, min.z), max, _c);
	Line(max, glm::vec3(min.x, max.y, max.z), _c);
	Line(glm::vec3(min.x, max.y, max.z), glm::vec3(min.x, max.y, min.z), _c);

	Line(min, glm::vec3(min.x, max.y, min.z), _c);
	Line(glm::vec3(max.x, min.y, min.z), glm::vec3(max.x, max.y, min.z), _c);
	Line(glm::vec3(max.x, min.y, max.z), max, _c);
	Line(glm::vec3(min.x, min.y, max.z), glm::vec3(min.x, max.y, max.z), _c);

	EndBatch();
}
//=============================================================================
void DebugDraw::Obb(const glm::vec3& _min, const glm::vec3& _max, const glm::mat4& _model, const glm::vec3& _c)
{
	glm::vec3 verts[8];
	glm::vec3 size = _max - _min;
	int       idx = 0;

	for (float x = _min.x; x <= _max.x; x += size.x)
	{
		for (float y = _min.y; y <= _max.y; y += size.y)
		{
			for (float z = _min.z; z <= _max.z; z += size.z)
			{
				glm::vec4 v = _model * glm::vec4(x, y, z, 1.0f);
				verts[idx++] = glm::vec3(v.x, v.y, v.z);
			}
		}
	}

	Line(verts[0], verts[1], _c);
	Line(verts[1], verts[5], _c);
	Line(verts[5], verts[4], _c);
	Line(verts[4], verts[0], _c);

	Line(verts[2], verts[3], _c);
	Line(verts[3], verts[7], _c);
	Line(verts[7], verts[6], _c);
	Line(verts[6], verts[2], _c);

	Line(verts[2], verts[0], _c);
	Line(verts[6], verts[4], _c);
	Line(verts[3], verts[1], _c);
	Line(verts[7], verts[5], _c);
}
//=============================================================================
void DebugDraw::Grid(const float& _x, const float& _z, const float& _y_level, const float& spacing, const glm::vec3& _c)
{
	int offset_x = floor((_x * spacing) / 2.0f);
	int offset_z = floor((_z * spacing) / 2.0f);

	for (int x = -offset_x; x <= offset_x; x += spacing)
	{
		Line(glm::vec3(x, _y_level, -offset_z), glm::vec3(x, _y_level, offset_z), _c);
	}

	for (int z = -offset_z; z <= offset_z; z += spacing)
	{
		Line(glm::vec3(-offset_x, _y_level, z), glm::vec3(offset_x, _y_level, z), _c);
	}
}
//=============================================================================
float closest_divisable(float a, float b)
{
	int c1 = int(a) - (int(a) % int(b));
	int c2 = (a + b) - (int(a) % int(b));

	if (int(a) - c1 > c2 - int(a))
		return float(c2);
	else
		return float(c1);
}
//=============================================================================
void DebugDraw::Grid(const glm::mat4& viewProj, const float& unitSize, const float& highlightUnitSize)
{
	// Get world space frustum corners
	glm::mat4 inverse = glm::inverse(viewProj);
	glm::vec3 corners[8];

	for (int i = 0; i < 8; i++)
	{
		glm::vec4 v = inverse * kFrustumCorners[i];
		v = v / v.w;
		corners[i] = glm::vec3(v.x, v.y, v.z);
	}

	// Find min and max
	glm::vec3 min = corners[0];
	glm::vec3 max = corners[0];

	for (int i = 0; i < 8; i++)
	{
		if (corners[i].x < min.x)
			min.x = corners[i].x;
		if (corners[i].y < min.y)
			min.y = corners[i].y;
		if (corners[i].z < min.z)
			min.z = corners[i].z;

		if (corners[i].x > max.x)
			max.x = corners[i].x;
		if (corners[i].y > max.y)
			max.y = corners[i].y;
		if (corners[i].z > max.z)
			max.z = corners[i].z;
	}

	glm::vec3 min_mod = glm::vec3(closest_divisable(min.x, unitSize), closest_divisable(min.y, unitSize), closest_divisable(min.z, unitSize));
	glm::vec3 max_mod = glm::vec3(closest_divisable(max.x, unitSize), closest_divisable(max.y, unitSize), closest_divisable(max.z, unitSize));

	float x = min_mod.x;

	BeginBatch();

	while (x < max_mod.x)
	{
		glm::vec3 color = glm::vec3(1.0f);

		float coord = int(x) % int(highlightUnitSize);

		if (coord != 0.0f)
			color = glm::vec3(0.2f);

		if (x != 0.0f)
			Line(glm::vec3(x, 0.0f, min.z), glm::vec3(x, 0.0f, max.z), color);

		x += unitSize;
	}

	float z = min_mod.z;

	while (z < max_mod.z)
	{
		glm::vec3 color = glm::vec3(1.0f);

		float coord = int(z) % int(highlightUnitSize);

		if (coord != 0.0f)
			color = glm::vec3(0.2f);

		if (z != 0.0f)
			Line(glm::vec3(min.x, 0.0f, z), glm::vec3(max.x, 0.0f, z), color);

		z += unitSize;
	}

	// X-axis = Red
	Line(glm::vec3(0.0f, 0.0f, min.z), glm::vec3(0.0f, 0.0f, max.z), glm::vec3(1.0f, 0.0f, 0.0f));

	// Y-axis = Green
	Line(glm::vec3(0.0f, min.y, 0.0f), glm::vec3(0.0f, max.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Z-axis = Blue
	Line(glm::vec3(min.x, 0.0f, 0.0f), glm::vec3(max.x, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	EndBatch();
}
//=============================================================================
void DebugDraw::Line(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& c)
{
	if (m_worldVertices.size() < MAX_VERTICES)
	{
		VertexWorld vw0, vw1;
		vw0.position = v0;
		vw0.color = c;

		vw1.position = v1;
		vw1.color = c;

		m_worldVertices.push_back(vw0);
		m_worldVertices.push_back(vw1);

		if (!m_batchedMode || m_batchStart == m_batchEnd)
		{
			DrawCommand cmd;

			cmd.depthTest = m_depthTest;
			cmd.distanceFade = m_distanceFade;
			cmd.fadeStart = m_fadeStart;
			cmd.fadeEnd = m_fadeEnd;
			cmd.type = GL_LINES;
			cmd.vertices = 2;

			m_drawCommands.push_back(cmd);
		}
		else
		{
			DrawCommand& cmd = m_drawCommands.back();
			cmd.vertices += 2;
		}

		if (m_batchedMode)
			m_batchEnd = m_worldVertices.size();
	}
}
//=============================================================================
void DebugDraw::LineStrip(glm::vec3* v, const int& count, const glm::vec3& c)
{
	for (int i = 0; i < count; i++)
	{
		VertexWorld vert;
		vert.position = v[i];
		vert.color    = c;

		m_worldVertices.push_back(vert);
	}

	DrawCommand cmd;
	cmd.depthTest    = m_depthTest;
	cmd.distanceFade = m_distanceFade;
	cmd.fadeStart    = m_fadeStart;
	cmd.fadeEnd      = m_fadeEnd;
	cmd.type         = GL_LINE_STRIP;
	cmd.vertices     = count;

	m_drawCommands.push_back(cmd);
}
//=============================================================================
void DebugDraw::CircleXY(float radius, const glm::vec3& pos, const glm::vec3& c)
{
	glm::vec3 verts[19];

	int idx = 0;

	for (int i = 0; i <= 360; i += 20)
	{
		float degInRad = glm::radians((float)i);
		verts[idx++] = pos + glm::vec3(cos(degInRad) * radius, sin(degInRad) * radius, 0.0f);
	}

	LineStrip(&verts[0], 19, c);
}
//=============================================================================
void DebugDraw::CircleXZ(float radius, const glm::vec3& pos, const glm::vec3& c)
{
	glm::vec3 verts[19];

	int idx = 0;

	for (int i = 0; i <= 360; i += 20)
	{
		float degInRad = glm::radians((float)i);
		verts[idx++] = pos + glm::vec3(cos(degInRad) * radius, 0.0f, sin(degInRad) * radius);
	}

	LineStrip(&verts[0], 19, c);
}
//=============================================================================
void DebugDraw::CircleYZ(float radius, const glm::vec3& pos, const glm::vec3& c)
{
	glm::vec3 verts[19];

	int idx = 0;

	for (int i = 0; i <= 360; i += 20)
	{
		float degInRad = glm::radians((float)i);
		verts[idx++] = pos + glm::vec3(0.0f, cos(degInRad) * radius, sin(degInRad) * radius);
	}

	LineStrip(&verts[0], 19, c);
}
//=============================================================================
void DebugDraw::Sphere(const float& radius, const glm::vec3& pos, const glm::vec3& c)
{
	CircleXY(radius, pos, c);
	CircleXZ(radius, pos, c);
	CircleYZ(radius, pos, c);
}
//=============================================================================
void DebugDraw::Frustum(const glm::mat4& view_proj, const glm::vec3& c)
{
	glm::mat4 inverse = glm::inverse(view_proj);
	glm::vec3 corners[8];

	for (int i = 0; i < 8; i++)
	{
		glm::vec4 v = inverse * kFrustumCorners[i];
		v = v / v.w;
		corners[i] = glm::vec3(v.x, v.y, v.z);
	}

	glm::vec3 _far[5] = { corners[0], corners[1], corners[2], corners[3], corners[0] };

	LineStrip(&_far[0], 5, c);

	glm::vec3 _near[5] = { corners[4], corners[5], corners[6], corners[7], corners[4] };

	LineStrip(&_near[0], 5, c);

	Line(corners[0], corners[4], c);
	Line(corners[1], corners[5], c);
	Line(corners[2], corners[6], c);
	Line(corners[3], corners[7], c);
}
//=============================================================================
void DebugDraw::Frustum(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& c)
{
	glm::mat4 inverse = glm::inverse(proj * view);
	glm::vec3 corners[8];

	for (int i = 0; i < 8; i++)
	{
		glm::vec4 v = inverse * kFrustumCorners[i];
		v = v / v.w;
		corners[i] = glm::vec3(v.x, v.y, v.z);
	}

	glm::vec3 _far[5] = { corners[0], corners[1], corners[2], corners[3], corners[0] };

	LineStrip(&_far[0], 5, c);

	glm::vec3 _near[5] = { corners[4], corners[5], corners[6], corners[7], corners[4] };

	LineStrip(&_near[0], 5, c);

	Line(corners[0], corners[4], c);
	Line(corners[1], corners[5], c);
	Line(corners[2], corners[6], c);
	Line(corners[3], corners[7], c);
}
//=============================================================================
void DebugDraw::Transform(const glm::mat4& trans, const float& axis_length)
{
	glm::vec3 p = glm::vec3(trans[3][0], trans[3][1], trans[3][2]);
	glm::vec3 x = glm::vec3(trans[0][0], trans[0][1], trans[0][2]);
	glm::vec3 y = glm::vec3(trans[1][0], trans[1][1], trans[1][2]);
	glm::vec3 z = glm::vec3(trans[2][0], trans[2][1], trans[2][2]);

	// Draw X axis
	Line(p, p + x * axis_length, glm::vec3(1.0f, 0.0f, 0.0f));

	// Draw Y axis
	Line(p, p + y * axis_length, glm::vec3(0.0f, 1.0f, 0.0f));

	// Draw Z axis
	Line(p, p + z * axis_length, glm::vec3(0.0f, 0.0f, 1.0f));
}
//=============================================================================
void DebugDraw::Render(Framebuffer::Ptr fbo, int width, int height, const glm::mat4& view_proj, const glm::vec3& view_pos)
{
	if (m_worldVertices.size() > 0)
	{
		m_uniforms.viewProj = view_proj;

#    if defined(__EMSCRIPTEN__)
		void* ptr = m_line_vbo->Map(0);
#    else
		void* ptr = m_lineVbo->Map(GL_WRITE_ONLY);
#    endif

		if (m_worldVertices.size() > MAX_VERTICES)
			Error("Vertex count above allowed limit!");
		else
			memcpy(ptr, &m_worldVertices[0], sizeof(VertexWorld) * m_worldVertices.size());

		m_lineVbo->Unmap();

#    if defined(__EMSCRIPTEN__)
		ptr = m_ubo->Map(0);
#    else
		ptr = m_ubo->Map(GL_WRITE_ONLY);
#    endif

		memcpy(ptr, &m_uniforms, sizeof(CameraUniforms));
		m_ubo->Unmap();

		// Get previous state
		GLenum last_blend_src_rgb;
		glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
		GLenum last_blend_dst_rgb;
		glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
		GLenum last_blend_src_alpha;
		glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
		GLenum last_blend_dst_alpha;
		glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
		GLenum last_blend_equation_rgb;
		glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
		GLenum last_blend_equation_alpha;
		glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
		GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
		GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
		GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
		GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

		// Set initial state
		glDisable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (fbo)
			fbo->Bind();
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, width, height);
		m_lineProgram->Use();
		m_ubo->BindBase(0);
		m_lineVao->Bind();

		int v = 0;

		for (int i = 0; i < m_drawCommands.size(); i++)
		{
			DrawCommand& cmd = m_drawCommands[i];

			if (cmd.distanceFade)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);

			if (cmd.depthTest)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			glm::vec4 params[2];

			params[0] = glm::vec4(view_pos, 0.0f);
			params[1] = glm::vec4(cmd.distanceFade ? cmd.fadeStart : -1.0f, cmd.fadeEnd, 0.0f, 0.0f);

			m_lineProgram->SetUniform("camera_pos", params[0]);
			m_lineProgram->SetUniform("fade_params", params[1]);

			glDrawArrays(cmd.type, v, cmd.vertices);
			v += cmd.vertices;
		}

		m_drawCommands.clear();
		m_worldVertices.clear();

		// Restore state
		//glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
		glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
		if (last_enable_blend)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
		if (last_enable_cull_face)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		if (last_enable_depth_test)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		if (last_enable_scissor_test)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}
}
//=============================================================================