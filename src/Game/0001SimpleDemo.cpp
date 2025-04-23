#include "stdafx.h"
#include "0001SimpleDemo.h"
#include "RHIShaders.h"
#include "RHIBuffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Core.h"
#include "Context.h"
#include "Profiler.h"
#include "Material.h"
//=============================================================================
namespace
{
	// Embedded vertex shader source.
	const char* g_sample_vs_src = R"(
#version 430 core

layout (location = 0) in vec4 VS_IN_Position;
layout (location = 1) in vec4 VS_IN_TexCoord;
layout (location = 2) in vec4 VS_IN_Normal;
layout (location = 3) in vec4 VS_IN_Tangent;
layout (location = 4) in vec4 VS_IN_Bitangent;
layout (std140) uniform Transforms //#binding 0
{ 
	mat4 model;
	mat4 view;
	mat4 projection;
};

out vec3 PS_IN_FragPos;
out vec3 PS_IN_Normal;
out vec2 PS_IN_TexCoord;
void main()
{
	vec4 position = model * vec4(VS_IN_Position.xyz, 1.0);
	PS_IN_FragPos = position.xyz;
	PS_IN_Normal = mat3(model) * VS_IN_Normal.xyz;
	PS_IN_TexCoord = VS_IN_TexCoord.xy;
	gl_Position = projection * view * position;
}
)";

	// Embedded fragment shader source.
	const char* g_sample_fs_src = R"(
#version 430 core

precision mediump float;
out vec4 PS_OUT_Color;
in vec3 PS_IN_FragPos;
in vec3 PS_IN_Normal;
in vec2 PS_IN_TexCoord;
uniform sampler2D s_Diffuse; //#slot 0
void main()
{
	vec3 light_pos = vec3(-200.0, 200.0, 0.0);
	vec3 n = normalize(PS_IN_Normal);
	vec3 l = normalize(light_pos - PS_IN_FragPos);
	float lambert = max(0.0f, dot(n, l));
	vec3 diffuse = texture(s_Diffuse, PS_IN_TexCoord).xyz;// + vec3(1.0);
	vec3 ambient = diffuse * 0.03;
	vec3 color = diffuse * lambert + ambient;

	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0 / 2.2));

	PS_OUT_Color = vec4(color, 1.0);
}
)";

	// Uniform buffer data structure.
	struct Transforms final
	{
		glm::aligned_mat4 model;
		glm::aligned_mat4 view;
		glm::aligned_mat4 projection;
	};

	// GPU resources.
	Shader::Ptr  m_vs;
	Shader::Ptr  m_fs;
	Program::Ptr m_program;
	Buffer::Ptr  m_ubo;

	// Camera.
	std::unique_ptr<Camera> m_main_camera;
	//Camerao camera{ glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0, -1.0f) };
	bool firstMouse = true;
	float lastX = 1600.0f / 2.0;
	float lastY = 900.0f / 2.0;

	// Assets.
	Mesh::Ptr m_mesh;

	// Uniforms.
	Transforms m_transforms;
}
//=============================================================================
_0001SimpleDemo::_0001SimpleDemo(Context& context)
	: m_context(context)
{

}
//=============================================================================
bool _0001SimpleDemo::Init()
{
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	// Create shaders
	m_vs = Shader::Create(GL_VERTEX_SHADER, g_sample_vs_src);
	m_fs = Shader::Create(GL_FRAGMENT_SHADER, g_sample_fs_src);

	if (!m_vs || !m_fs)
	{
		Error("Failed to create Shaders");
		return false;
	}

	// Create shader program
	m_program = Program::Create({ m_vs, m_fs });

	if (!m_program)
	{
		Error("Failed to create Shader Program");
		return false;
	}

	m_program->UniformBlockBinding("Transforms", 0);

	// Create uniform buffer for matrix data
	m_ubo = Buffer::Create(GL_UNIFORM_BUFFER, GL_MAP_WRITE_BIT, sizeof(Transforms), nullptr);

	m_mesh = Mesh::Load("data/mesh/teapot.obj");

	m_main_camera = std::make_unique<Camera>(60.0f, 0.1f, 1000.0f, GetFrameAspect(), glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0, -1.0f));

	return true;
}
//=============================================================================
void _0001SimpleDemo::Close()
{
	m_mesh.reset();
}
//=============================================================================
void _0001SimpleDemo::Resize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	m_main_camera->UpdateProjection(60.0f, 0.1f, 1000.0f, GetFrameAspect());
}
//=============================================================================
void _0001SimpleDemo::Update(double deltaTime)
{
	SE_SCOPED_SAMPLE("update");

	// Update camera.
	m_main_camera->Update();
	//ProcessInput(camera, deltaTime, firstMouse, lastX, lastY);

	m_transforms.model = glm::mat4(1.0f);
	m_transforms.model = glm::translate(m_transforms.model, glm::vec3(0.0f, -20.0f, 0.0f));
	m_transforms.model = glm::rotate(m_transforms.model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
	m_transforms.model = glm::scale(m_transforms.model, glm::vec3(0.6f));
	m_transforms.view = m_main_camera->view;
	m_transforms.projection = m_main_camera->projection;
	//m_transforms.view = camera.GetViewMatrix();
	//m_transforms.projection = camera.GetProjectionMatrix(GetFrameAspect());

	void* ptr = m_ubo->Map(GL_WRITE_ONLY);
	memcpy(ptr, &m_transforms, sizeof(Transforms));
	m_ubo->Unmap();
}
//=============================================================================
void _0001SimpleDemo::Draw(double deltaTime)
{
	SE_SCOPED_SAMPLE("Draw");

	// Bind framebuffer and set viewport.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, GetFrameWidth(), GetFrameHeight());

	// Clear default framebuffer.
	glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind shader program.
	m_program->Use();

	// Bind uniform buffer.
	m_ubo->BindBase(0);

	// Bind vertex array.
	m_mesh->MeshVertexArray()->Bind();

	// Set active texture unit uniform
	m_program->SetUniform("s_Diffuse", 0);

	const auto& submeshes = m_mesh->SubMeshes();

	for (uint32_t i = 0; i < submeshes.size(); i++)
	{
		auto& submesh = submeshes[i];
		auto& mat = m_mesh->GetMaterial(submesh.matIdx);

		// Bind texture.
		if (mat->AlbedoTexture())
			mat->AlbedoTexture()->Bind(0);

		// Issue draw call.
		glDrawElementsBaseVertex(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * submesh.baseIndex), submesh.baseVertex);
	}
}
//=============================================================================
void _0001SimpleDemo::DrawImGui(double deltaTime)
{
	SE_SCOPED_SAMPLE("DrawImGui");

	// Render profiler.
	profiler::Ui();
}
//=============================================================================
//void _0001SimpleDemo::ProcessInput(Camerao& camera, float deltaTime, bool& firstMouse, float& lastX, float& lastY)
//{
//	if (glfwGetMouseButton(GetWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
//	{
//		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//		double xpos, ypos;
//		glfwGetCursorPos(GetWindow(), &xpos, &ypos);
//
//		if (firstMouse)
//		{
//			lastX = xpos;
//			lastY = ypos;
//			firstMouse = false;
//		}
//
//		float xoffset = xpos - lastX;
//		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//		lastX = xpos;
//		lastY = ypos;
//
//		camera.ProcessMouseMovement(xoffset, yoffset);
//	}
//	else
//	{
//		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//		firstMouse = true;
//	}
//
//	if (glfwGetKey(GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(Direction::Forward, deltaTime);
//	if (glfwGetKey(GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(Direction::Backward, deltaTime);
//	if (glfwGetKey(GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(Direction::Left, deltaTime);
//	if (glfwGetKey(GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(Direction::Right, deltaTime);
//}
//=============================================================================