#include "SampleFrameworkOGL3.h"
//=============================================================================
#define POSITION_LOCATION  0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION    2
//=============================================================================
GLuint gl3::CreateShader(GLenum type, const char* shaderSource)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);

	GLint  success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLchar log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);

		const std::string logError
			= "OPENGL: Shader compilation failed : "
			+ std::string(log) + ", Source: \n" + shaderSource;
		throw std::exception(logError.c_str());
	}

	return shader;
}
//=============================================================================
GLuint gl3::CreateShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char  log[512];
		glGetProgramInfoLog(program, 512, nullptr, log);
		std::string logError = "OPENGL: Shader program linking failed: " + std::string(log);
		throw std::exception(logError.c_str());
	}
	return program;
}
//=============================================================================
GLuint gl3::CreateVertexBuffer(GLenum usage, size_t size, void* data)
{
	GLint lastBuffer = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastBuffer);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);

	glBindBuffer(GL_ARRAY_BUFFER, lastBuffer);

	return buffer;
}
//=============================================================================
GLuint gl3::CreateIndexBuffer(GLenum usage, size_t size, void* data)
{
	GLint lastBuffer = 0;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &lastBuffer);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lastBuffer);

	return buffer;
}
//=============================================================================
void gl3::SetVertexFormat(size_t vertexSize, const std::vector<VertexAttribute>& attributes)
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		const auto& attr = attributes[i];
		glEnableVertexAttribArray(attr.index);
		glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized ? GL_TRUE : GL_FALSE, vertexSize, reinterpret_cast<void*>(attr.offset));
	}
}
//=============================================================================
GLuint gl3::CreateVertexArray(GLuint vbo, GLuint ibo, size_t vertexSize, const std::vector<VertexAttribute>& attributes)
{
	GLint lastVAO = 0;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVAO);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if (vbo > 0) glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (ibo > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	SetVertexFormat(vertexSize, attributes);

	glBindVertexArray(lastVAO);

	return vao;
}
//=============================================================================
GLuint gl3::CreateTexture2D(GLenum internalFormat, GLenum format, int width, int height, void* data)
{
	GLint lastTextureID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, lastTextureID);

	return texture;
}
//=============================================================================
GLuint gl3::LoadTexture2D(const char* filename, bool isFlip)
{
	stbi_set_flip_vertically_on_load(isFlip);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

	GLenum internalFormat = GL_RGBA8;
	GLenum format = GL_RGBA;
	if (nrChannels == 1)
	{
		internalFormat = GL_R8;
		format = GL_RED;
	}
	else if (nrChannels == 2)
	{
		internalFormat = GL_RG8;
		format = GL_RG;
	}
	else if (nrChannels == 3)
	{
		internalFormat = GL_RGB8;
		format = GL_RGB;
	}

	GLuint texture = gl3::CreateTexture2D(internalFormat, format, width, height, data);
	stbi_image_free(data);

	return texture;
}
//=============================================================================
void utils::Transform::SetPosition(const glm::vec3& position)
{
	m_position = position;
}
//=============================================================================
void utils::Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
}
//=============================================================================
void utils::Transform::SetScale(float scale)
{
	m_scale = glm::vec3(scale);
}
//=============================================================================
void utils::Transform::SetRotation(const glm::vec3& eulerAngles)
{
	m_rotation = glm::quat(eulerAngles);
}
//=============================================================================
void utils::Transform::SetRotation(const glm::quat& rotation)
{
	m_rotation = rotation;
}
//=============================================================================
void utils::Transform::Translate(const glm::vec3 movePosition)
{
	m_position += movePosition;
}
//=============================================================================
void utils::Transform::Rotate(const glm::vec3& anglesRadians)
{
	glm::quat rotationX = glm::angleAxis(anglesRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat rotationY = glm::angleAxis(anglesRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat rotationZ = glm::angleAxis(anglesRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::quat combinedRotation = rotationZ * rotationY * rotationX;
	m_rotation *= combinedRotation;
}
//=============================================================================
glm::mat4 utils::Transform::GetWorldMatrix() const
{
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);
	glm::mat4 rotation = glm::toMat4(m_rotation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);

	return translation * rotation * scale;
}
//=============================================================================
utils::Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
	: m_position(position)
	, m_yaw(yaw)
	, m_pitch(pitch)
	, m_worldUp(up)
{
	updateVector();
}
//=============================================================================
void utils::Camera::Move(CameraMovement direction, float deltaTime)
{
	float velocity = m_speed * deltaTime;
	if (direction == CameraMovement::Forward)      m_position += m_front * velocity;
	else if (direction == CameraMovement::Backward) m_position -= m_front * velocity;
	else if (direction == CameraMovement::Left)    m_position += m_right * velocity;
	else if (direction == CameraMovement::Right)   m_position -= m_right * velocity;
}
//=============================================================================
void utils::Camera::Rotate(float xOffset, float yOffset)
{
	xOffset *= m_sensitivity;
	yOffset *= m_sensitivity;

	m_yaw += xOffset;
	m_pitch = glm::clamp(m_pitch + yOffset, -89.0f, 89.0f);

	updateVector();
}
//=============================================================================
glm::mat4 utils::Camera::GetViewMatrix() const
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}
//=============================================================================
void utils::Camera::updateVector()
{
	float yaw = glm::radians(m_yaw);
	float pitch = glm::radians(m_pitch);

	glm::vec3 newFront;
	newFront.x = cosf(yaw) * cosf(pitch);
	newFront.y = sinf(pitch);
	newFront.z = sinf(yaw) * cosf(pitch);
	m_front = glm::normalize(newFront);

	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}
//=============================================================================
static int MARGIN = 10;
static float EDGE_STEP = 1.0f;
//=============================================================================
utils::CameraTemp::CameraTemp(int WindowWidth, int WindowHeight)
{
	m_windowWidth = WindowWidth;
	m_windowHeight = WindowHeight;
	m_pos = Vector3f(0.0f, 0.0f, 0.0f);
	m_target = Vector3f(0.0f, 0.0f, 1.0f);
	m_up = Vector3f(0.0f, 1.0f, 0.0f);

	init();
}
//=============================================================================
utils::CameraTemp::CameraTemp(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
	m_windowWidth = WindowWidth;
	m_windowHeight = WindowHeight;
	m_pos = Pos;

	m_target = Target;
	m_target.Normalize();

	m_up = Up;
	m_up.Normalize();

	init();
}
//=============================================================================
void utils::CameraTemp::init()
{
	Vector3f HTarget(m_target.x, 0.0, m_target.z);
	HTarget.Normalize();

	float Angle = ToDegree(asin(abs(HTarget.z)));

	if (HTarget.z >= 0.0f)
	{
		if (HTarget.x >= 0.0f)
		{
			m_AngleH = 360.0f - Angle;
		}
		else
		{
			m_AngleH = 180.0f + Angle;
		}
	}
	else
	{
		if (HTarget.x >= 0.0f)
		{
			m_AngleH = Angle;
		}
		else
		{
			m_AngleH = 180.0f - Angle;
		}
	}

	m_AngleV = -ToDegree(asin(m_target.y));

	m_OnUpperEdge = false;
	m_OnLowerEdge = false;
	m_OnLeftEdge = false;
	m_OnRightEdge = false;
	m_mousePos.x = m_windowWidth / 2;
	m_mousePos.y = m_windowHeight / 2;
}
//=============================================================================
void utils::CameraTemp::SetPosition(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}
//=============================================================================
void utils::CameraTemp::OnKeyboard(unsigned char Key)
{
	switch (Key) {

	case GLFW_KEY_UP:
		m_pos += (m_target * m_speed);
		break;

	case GLFW_KEY_DOWN:
		m_pos -= (m_target * m_speed);
		break;

	case GLFW_KEY_LEFT:
	{
		Vector3f Left = m_target.Cross(m_up);
		Left.Normalize();
		Left *= m_speed;
		m_pos += Left;
	}
	break;

	case GLFW_KEY_RIGHT:
	{
		Vector3f Right = m_up.Cross(m_target);
		Right.Normalize();
		Right *= m_speed;
		m_pos += Right;
	}
	break;

	case GLFW_KEY_Q:
		m_pos.y += m_speed;
		break;

	case GLFW_KEY_E:
		m_pos.y -= m_speed;
		break;

	case '+':
		m_speed += 0.1f;
		printf("Speed changed to %f\n", m_speed);
		break;

	case '-':
		m_speed -= 0.1f;
		if (m_speed < 0.1f) {
			m_speed = 0.1f;
		}
		printf("Speed changed to %f\n", m_speed);
		break;
	}
}
//=============================================================================
void utils::CameraTemp::OnMouse(int x, int y)
{
	int DeltaX = x - m_mousePos.x;
	int DeltaY = y - m_mousePos.y;

	m_mousePos.x = x;
	m_mousePos.y = y;

	m_AngleH += (float)DeltaX / 20.0f;
	m_AngleV += (float)DeltaY / 50.0f;

	if (DeltaX == 0) {
		if (x <= MARGIN) {
			m_OnLeftEdge = true;
		}
		else if (x >= (m_windowWidth - MARGIN)) {
			m_OnRightEdge = true;
		}
	}
	else {
		m_OnLeftEdge = false;
		m_OnRightEdge = false;
	}

	if (DeltaY == 0) {
		if (y <= MARGIN) {
			m_OnUpperEdge = true;
		}
		else if (y >= (m_windowHeight - MARGIN)) {
			m_OnLowerEdge = true;
		}
	}
	else {
		m_OnUpperEdge = false;
		m_OnLowerEdge = false;
	}

	update();
}
//=============================================================================
void utils::CameraTemp::OnRender()
{
	bool ShouldUpdate = false;

	if (m_OnLeftEdge) {
		m_AngleH -= EDGE_STEP;
		ShouldUpdate = true;
	}
	else if (m_OnRightEdge) {
		m_AngleH += EDGE_STEP;
		ShouldUpdate = true;
	}

	if (m_OnUpperEdge) {
		if (m_AngleV > -90.0f) {
			m_AngleV -= EDGE_STEP;
			ShouldUpdate = true;
		}
	}
	else if (m_OnLowerEdge) {
		if (m_AngleV < 90.0f) {
			m_AngleV += EDGE_STEP;
			ShouldUpdate = true;
		}
	}

	if (ShouldUpdate) {
		update();
	}
}
//=============================================================================
void utils::CameraTemp::update()
{
	Vector3f Yaxis(0.0f, 1.0f, 0.0f);

	// Rotate the view vector by the horizontal angle around the vertical axis
	Vector3f View(1.0f, 0.0f, 0.0f);
	View.Rotate(m_AngleH, Yaxis);
	View.Normalize();

	// Rotate the view vector by the vertical angle around the horizontal axis
	Vector3f U = Yaxis.Cross(View);
	U.Normalize();
	View.Rotate(m_AngleV, U);

	m_target = View;
	m_target.Normalize();

	m_up = m_target.Cross(U);
	m_up.Normalize();
}
//=============================================================================
Matrix4f utils::CameraTemp::GetMatrix()
{
	Matrix4f CameraTransformation;
	CameraTransformation.InitCameraTransform(m_pos, m_target, m_up);

	return CameraTransformation;
}
//=============================================================================
void utils::WorldTrans::SetScale(float scale)
{
	m_scale = scale;
}
//=============================================================================
void utils::WorldTrans::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}
//=============================================================================
void utils::WorldTrans::SetPosition(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}
//=============================================================================
void utils::WorldTrans::SetPosition(const Vector3f& WorldPos)
{
	m_pos = WorldPos;
}
//=============================================================================
void utils::WorldTrans::Rotate(float x, float y, float z)
{
	m_rotation.x += x;
	m_rotation.y += y;
	m_rotation.z += z;
}
//=============================================================================
Matrix4f utils::WorldTrans::GetMatrix() const
{
	Matrix4f Scale;
	Scale.InitScaleTransform(m_scale, m_scale, m_scale);

	Matrix4f Rotation;
	Rotation.InitRotateTransform(m_rotation.x, m_rotation.y, m_rotation.z);

	Matrix4f Translation;
	Translation.InitTranslationTransform(m_pos.x, m_pos.y, m_pos.z);

	Matrix4f WorldTransformation = Translation * Rotation * Scale;

	return WorldTransformation;
}
//=============================================================================
Matrix4f utils::WorldTrans::GetReversedTranslationMatrix() const
{
	Matrix4f ReversedTranslation;
	ReversedTranslation.InitTranslationTransform(m_pos.Negate());
	return ReversedTranslation;
}
//=============================================================================
Matrix4f utils::WorldTrans::GetReversedRotationMatrix() const
{
	Matrix4f ReversedRotation;
	ReversedRotation.InitRotateTransformZYX(-m_rotation.x, -m_rotation.y, -m_rotation.z);
	return ReversedRotation;
}
//=============================================================================
Vector3f utils::WorldTrans::WorldPosToLocalPos(const Vector3f& WorldPos) const
{
	Matrix4f WorldToLocalTranslation = GetReversedTranslationMatrix();
	Matrix4f WorldToLocalRotation = GetReversedRotationMatrix();
	Matrix4f WorldToLocalTransformation = WorldToLocalRotation * WorldToLocalTranslation;
	Vector4f WorldPos4f = Vector4f(WorldPos, 1.0f);
	Vector4f LocalPos4f = WorldToLocalTransformation * WorldPos4f;
	Vector3f LocalPos3f(LocalPos4f);
	return LocalPos3f;
}
//=============================================================================
Vector3f utils::WorldTrans::WorldDirToLocalDir(const Vector3f& WorldDirection) const
{
	Matrix3f World3f(GetMatrix());  // Initialize using the top left corner

	// Inverse local-to-world transformation using transpose
	// (assuming uniform scaling)
	Matrix3f WorldToLocal = World3f.Transpose();

	Vector3f LocalDirection = WorldToLocal * WorldDirection;

	LocalDirection = LocalDirection.Normalize();

	return LocalDirection;
}
//=============================================================================
utils::Mesh::Mesh(const std::string& filename, int assimpFlags)
{
	// TODO: использовать функции из gl

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);

	m_scene = m_importer.ReadFile(filename.c_str(), assimpFlags);

	if (m_scene)
	{
		m_globalInverseTransform = m_scene->mRootNode->mTransformation;
		m_globalInverseTransform = m_globalInverseTransform.Inverse();
		if (!initFromScene(m_scene, filename))
			throw std::exception("initFromScene failed");
	}
	else 
	{
		throw std::exception(("Error parsing '" + filename + "': '" + std::string(m_importer.GetErrorString()) + "'").c_str());
	}

	glBindVertexArray(0);

}
//=============================================================================
utils::Mesh::~Mesh()
{
	if (m_buffers[0] != 0)
	{
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);
	}

	if (m_vao != 0)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
}
//=============================================================================
void utils::Mesh::Render(IRenderCallbacks* pRenderCallbacks)
{
	if (m_isPBR)
	{
		setupRenderMaterialsPBR();
	}

	glBindVertexArray(m_vao);

	for (unsigned int MeshIndex = 0; MeshIndex < m_meshes.size(); MeshIndex++)
	{
		unsigned int MaterialIndex = m_meshes[MeshIndex].MaterialIndex;
		assert(MaterialIndex < m_materials.size());

		if (!m_isPBR)
		{
			setupRenderMaterialsPhong(MeshIndex, MaterialIndex, pRenderCallbacks);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES,
			m_meshes[MeshIndex].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * m_meshes[MeshIndex].BaseIndex),
			m_meshes[MeshIndex].BaseVertex);
	}

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);
}
//=============================================================================
void utils::Mesh::Render(uint32_t DrawIndex, uint32_t PrimID)
{
	glBindVertexArray(m_vao);

	unsigned int MaterialIndex = m_meshes[DrawIndex].MaterialIndex;
	assert(MaterialIndex < m_materials.size());

	if (m_materials[MaterialIndex].pDiffuse)
	{
		m_materials[MaterialIndex].pDiffuse->Bind(COLOR_TEXTURE_UNIT);
	}

	if (m_materials[MaterialIndex].pSpecularExponent)
	{
		m_materials[MaterialIndex].pSpecularExponent->Bind(SPECULAR_EXPONENT_UNIT);
	}

	glDrawElementsBaseVertex(GL_TRIANGLES,
		3,
		GL_UNSIGNED_INT,
		(void*)(sizeof(unsigned int) * (m_meshes[DrawIndex].BaseIndex + PrimID * 3)),
		m_meshes[DrawIndex].BaseVertex);

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);
}
//=============================================================================
void utils::Mesh::Render(uint32_t NumInstances, const Matrix4f* WVPMats, const Matrix4f* WorldMats)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[WVP_MAT_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4f) * NumInstances, WVPMats, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[WORLD_MAT_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4f) * NumInstances, WorldMats, GL_DYNAMIC_DRAW);

	glBindVertexArray(m_vao);

	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		const unsigned int MaterialIndex = m_meshes[i].MaterialIndex;

		assert(MaterialIndex < m_materials.size());

		if (m_materials[MaterialIndex].pDiffuse)
		{
			m_materials[MaterialIndex].pDiffuse->Bind(COLOR_TEXTURE_UNIT);
		}

		if (m_materials[MaterialIndex].pSpecularExponent)
		{
			m_materials[MaterialIndex].pSpecularExponent->Bind(SPECULAR_EXPONENT_UNIT);
		}

		glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
			m_meshes[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * m_meshes[i].BaseIndex),
			NumInstances,
			m_meshes[i].BaseVertex);
	}

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);
}
//=============================================================================
const utils::Material& utils::Mesh::GetMaterial()
{
	for (unsigned int i = 0; i < m_materials.size(); i++)
	{
		if (m_materials[i].ambientColor != glm::vec4(0.0f))
		{
			return m_materials[i];
		}
	}

	if (m_materials.size() == 0)
	{
		printf("No materials\n");
		exit(0);
	}

	return m_materials[0];
}
//=============================================================================
void utils::Mesh::GetLeadingVertex(uint32_t DrawIndex, uint32_t PrimID, Vector3f& Vertex)
{
	uint32_t MeshIndex = DrawIndex; // Each mesh is rendered in its own draw call

	assert(MeshIndex < m_scene->mNumMeshes);
	const aiMesh* paiMesh = m_scene->mMeshes[MeshIndex];

	assert(PrimID < paiMesh->mNumFaces);
	const aiFace& Face = paiMesh->mFaces[PrimID];

	uint32_t LeadingIndex = Face.mIndices[0];

	assert(LeadingIndex < paiMesh->mNumVertices);
	const aiVector3D& Pos = paiMesh->mVertices[LeadingIndex];
	Vertex.x = Pos.x;
	Vertex.y = Pos.y;
	Vertex.z = Pos.z;
}
//=============================================================================
bool utils::Mesh::initFromScene(const aiScene* pScene, const std::string& Filename)
{
	m_meshes.resize(pScene->mNumMeshes);
	m_materials.resize(pScene->mNumMaterials);

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;

	countVerticesAndIndices(pScene, NumVertices, NumIndices);

	reserveSpace(NumVertices, NumIndices);

	initAllMeshes(pScene);

	if (!initMaterials(pScene, Filename))
	{
	return false;
	}

	populateBuffers();

	return GLCheckError();
}
//=============================================================================
void utils::Mesh::countVerticesAndIndices(const aiScene* pScene, uint32_t& NumVertices, uint32_t& NumIndices)
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_meshes[i].BaseVertex = NumVertices;
		m_meshes[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_meshes[i].NumIndices;
	}
}
//=============================================================================
void utils::Mesh::reserveSpace(uint32_t NumVertices, uint32_t NumIndices)
{
	m_vertices.reserve(NumVertices);
	m_indices.reserve(NumIndices);
}
//=============================================================================
void utils::Mesh::initAllMeshes(const aiScene* pScene)
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
#ifdef USE_MESH_OPTIMIZER
		initSingleMeshOpt(i, paiMesh);
#else
		initSingleMesh(i, paiMesh);
#endif
	}
}
//=============================================================================
void utils::Mesh::initSingleMesh(uint32_t MeshIndex, const aiMesh* paiMesh)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// printf("Mesh %d\n", MeshIndex);
	// Populate the vertex attribute vectors
	Vertex v;

	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
	{
		const aiVector3D& pPos = paiMesh->mVertices[i];
		// printf("%d: ", i); Vector3f t(pPos.x, pPos.y, pPos.z); t.Print();
		v.Position = glm::vec3(pPos.x, pPos.y, pPos.z);

		if (paiMesh->mNormals)
		{
			const aiVector3D& pNormal = paiMesh->mNormals[i];
			v.Normal = glm::vec3(pNormal.x, pNormal.y, pNormal.z);
		}
		else
		{
			aiVector3D Normal(0.0f, 1.0f, 0.0f);
			v.Normal = glm::vec3(Normal.x, Normal.y, Normal.z);
		}

		const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;
		v.TexCoords = glm::vec2(pTexCoord.x, pTexCoord.y);

		m_vertices.push_back(v);
	}

	// Populate the index buffer
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
	{
		const aiFace& Face = paiMesh->mFaces[i];
		m_indices.push_back(Face.mIndices[0]);
		m_indices.push_back(Face.mIndices[1]);
		m_indices.push_back(Face.mIndices[2]);
	}
}
//=============================================================================
void utils::Mesh::initSingleMeshOpt(uint32_t MeshIndex, const aiMesh* paiMesh)
{
	// TODO: оптимизация меша с meshoptimizer по урокам ogldev
}
//=============================================================================
bool utils::Mesh::initMaterials(const aiScene* pScene, const std::string& Filename)
{
	std::string Dir = GetDirFromFilename(Filename);

	bool Ret = true;

	printf("Num materials: %d\n", pScene->mNumMaterials);

	// Initialize the materials
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		loadTextures(Dir, pMaterial, i);
		loadColors(pMaterial, i);
	}

	return Ret;
}
//=============================================================================
void utils::Mesh::loadTextures(const std::string& Dir, const aiMaterial* pMaterial, int index)
{
	loadDiffuseTexture(Dir, pMaterial, index);
	loadSpecularTexture(Dir, pMaterial, index);

	// PBR
	loadAlbedoTexture(Dir, pMaterial, index);
	loadMetalnessTexture(Dir, pMaterial, index);
	loadRoughnessTexture(Dir, pMaterial, index);
}
//=============================================================================
void utils::Mesh::loadDiffuseTexture(const std::string& Dir, const aiMaterial* pMaterial, int MaterialIndex)
{
	m_materials[MaterialIndex].pDiffuse = NULL;

	if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			const aiTexture* paiTexture = m_scene->GetEmbeddedTexture(Path.C_Str());

			if (paiTexture)
			{
				loadDiffuseTextureEmbedded(paiTexture, MaterialIndex);
			}
			else
			{
				loadDiffuseTextureFromFile(Dir, Path, MaterialIndex);
			}
		}
	}
}
//=============================================================================
void utils::Mesh::loadDiffuseTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex)
{
	printf("Embeddeded diffuse texture type '%s'\n", paiTexture->achFormatHint);
	m_materials[MaterialIndex].pDiffuse = new Texture(GL_TEXTURE_2D);
	int buffer_size = paiTexture->mWidth;
	m_materials[MaterialIndex].pDiffuse->Load(buffer_size, paiTexture->pcData);
}
//=============================================================================
void utils::Mesh::loadDiffuseTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex)
{
	std::string FullPath = GetFullPath(dir, Path);

	m_materials[MaterialIndex].pDiffuse = new Texture(GL_TEXTURE_2D, FullPath.c_str());

	if (!m_materials[MaterialIndex].pDiffuse->Load())
	{
		printf("Error loading diffuse texture '%s'\n", FullPath.c_str());
		exit(0);
	}
	else
	{
		printf("Loaded diffuse texture '%s' at index %d\n", FullPath.c_str(), MaterialIndex);
	}
}
//=============================================================================
void utils::Mesh::loadSpecularTexture(const std::string& Dir, const aiMaterial* pMaterial, int MaterialIndex)
{
	m_materials[MaterialIndex].pSpecularExponent = NULL;

	if (pMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
	{
		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_SHININESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			const aiTexture* paiTexture = m_scene->GetEmbeddedTexture(Path.C_Str());

			if (paiTexture)
			{
				loadSpecularTextureEmbedded(paiTexture, MaterialIndex);
			}
			else
			{
				loadSpecularTextureFromFile(Dir, Path, MaterialIndex);
			}
		}
	}
}
//=============================================================================
void utils::Mesh::loadSpecularTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex)
{
	printf("Embeddeded specular texture type '%s'\n", paiTexture->achFormatHint);
	m_materials[MaterialIndex].pSpecularExponent = new Texture(GL_TEXTURE_2D);
	int buffer_size = paiTexture->mWidth;
	m_materials[MaterialIndex].pSpecularExponent->Load(buffer_size, paiTexture->pcData);
}
//=============================================================================
void utils::Mesh::loadSpecularTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex)
{
	std::string FullPath = GetFullPath(dir, Path);

	m_materials[MaterialIndex].pSpecularExponent = new Texture(GL_TEXTURE_2D, FullPath.c_str());

	if (!m_materials[MaterialIndex].pSpecularExponent->Load())
	{
		printf("Error loading specular texture '%s'\n", FullPath.c_str());
		exit(0);
	}
	else
	{
		printf("Loaded specular texture '%s'\n", FullPath.c_str());
	}
}
//=============================================================================
void utils::Mesh::loadAlbedoTexture(const std::string& Dir, const aiMaterial* pMaterial, int MaterialIndex)
{
	m_materials[MaterialIndex].pbrMaterial.pAlbedo = NULL;

	if (pMaterial->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
	{
		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			const aiTexture* paiTexture = m_scene->GetEmbeddedTexture(Path.C_Str());

			if (paiTexture)
			{
				loadAlbedoTextureEmbedded(paiTexture, MaterialIndex);
			}
			else
			{
				loadAlbedoTextureFromFile(Dir, Path, MaterialIndex);
			}
		}
	}
}
//=============================================================================
void utils::Mesh::loadAlbedoTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex)
{
	printf("Embeddeded albedo texture type '%s'\n", paiTexture->achFormatHint);
	m_materials[MaterialIndex].pbrMaterial.pAlbedo = new Texture(GL_TEXTURE_2D);
	int buffer_size = paiTexture->mWidth;
	m_materials[MaterialIndex].pbrMaterial.pAlbedo->Load(buffer_size, paiTexture->pcData);
}
//=============================================================================
void utils::Mesh::loadAlbedoTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex)
{
	std::string FullPath = GetFullPath(dir, Path);

	m_materials[MaterialIndex].pbrMaterial.pAlbedo = new Texture(GL_TEXTURE_2D, FullPath.c_str());

	if (!m_materials[MaterialIndex].pbrMaterial.pAlbedo->Load())
	{
		printf("Error loading albedo texture '%s'\n", FullPath.c_str());
		exit(0);
	}
	else
	{
		printf("Loaded albedo texture '%s'\n", FullPath.c_str());
	}
}
//=============================================================================
void utils::Mesh::loadMetalnessTexture(const std::string& Dir, const aiMaterial* pMaterial, int MaterialIndex)
{
	m_materials[MaterialIndex].pbrMaterial.pMetallic = NULL;

	int NumTextures = pMaterial->GetTextureCount(aiTextureType_METALNESS);

	if (NumTextures > 0)
	{
		printf("Num metalness textures %d\n", NumTextures);

		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_METALNESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			const aiTexture* paiTexture = m_scene->GetEmbeddedTexture(Path.C_Str());

			if (paiTexture)
			{
				loadMetalnessTextureEmbedded(paiTexture, MaterialIndex);
			}
			else
			{
				loadMetalnessTextureFromFile(Dir, Path, MaterialIndex);
			}
		}
	}
}
//=============================================================================
void utils::Mesh::loadMetalnessTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex)
{
	printf("Embeddeded metalness texture type '%s'\n", paiTexture->achFormatHint);
	m_materials[MaterialIndex].pbrMaterial.pMetallic = new Texture(GL_TEXTURE_2D);
	int buffer_size = paiTexture->mWidth;
	m_materials[MaterialIndex].pbrMaterial.pMetallic->Load(buffer_size, paiTexture->pcData);
}
//=============================================================================
void utils::Mesh::loadMetalnessTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex)
{
	std::string FullPath = GetFullPath(dir, Path);

	m_materials[MaterialIndex].pbrMaterial.pMetallic = new Texture(GL_TEXTURE_2D, FullPath.c_str());

	if (!m_materials[MaterialIndex].pbrMaterial.pMetallic->Load())
	{
		printf("Error loading metalness texture '%s'\n", FullPath.c_str());
		exit(0);
	}
	else
	{
		printf("Loaded metalness texture '%s'\n", FullPath.c_str());
	}
}
//=============================================================================
void utils::Mesh::loadRoughnessTexture(const std::string& Dir, const aiMaterial* pMaterial, int MaterialIndex)
{
	m_materials[MaterialIndex].pbrMaterial.pRoughness = NULL;

	int NumTextures = pMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);

	if (NumTextures > 0)
	{
		printf("Num roughness textures %d\n", NumTextures);

		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			const aiTexture* paiTexture = m_scene->GetEmbeddedTexture(Path.C_Str());

			if (paiTexture)
			{
				loadRoughnessTextureEmbedded(paiTexture, MaterialIndex);
			}
			else
			{
				loadRoughnessTextureFromFile(Dir, Path, MaterialIndex);
			}
		}
	}
}
//=============================================================================
void utils::Mesh::loadRoughnessTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex)
{
	printf("Embeddeded roughness texture type '%s'\n", paiTexture->achFormatHint);
	m_materials[MaterialIndex].pbrMaterial.pRoughness = new Texture(GL_TEXTURE_2D);
	int buffer_size = paiTexture->mWidth;
	m_materials[MaterialIndex].pbrMaterial.pRoughness->Load(buffer_size, paiTexture->pcData);
}
//=============================================================================
void utils::Mesh::loadRoughnessTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex)
{
	std::string FullPath = GetFullPath(dir, Path);

	m_materials[MaterialIndex].pbrMaterial.pRoughness = new Texture(GL_TEXTURE_2D, FullPath.c_str());

	if (!m_materials[MaterialIndex].pbrMaterial.pRoughness->Load())
	{
		printf("Error loading roughness texture '%s'\n", FullPath.c_str());
		exit(0);
	}
	else
	{
		printf("Loaded roughness texture '%s'\n", FullPath.c_str());
	}
}
//=============================================================================
void utils::Mesh::loadColors(const aiMaterial* pMaterial, int index)
{
	aiColor4D AmbientColor(0.0f, 0.0f, 0.0f, 0.0f);
	glm::vec4 AllOnes(1.0f);

	int ShadingModel = 0;
	if (pMaterial->Get(AI_MATKEY_SHADING_MODEL, ShadingModel) == AI_SUCCESS)
	{
		printf("Shading model %d\n", ShadingModel);
	}

	if (pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor) == AI_SUCCESS)
	{
		printf("Loaded ambient color [%f %f %f]\n", AmbientColor.r, AmbientColor.g, AmbientColor.b);
		m_materials[index].ambientColor.r = AmbientColor.r;
		m_materials[index].ambientColor.g = AmbientColor.g;
		m_materials[index].ambientColor.b = AmbientColor.b;
	}
	else
	{
		m_materials[index].ambientColor = AllOnes;
	}

	aiColor3D DiffuseColor(0.0f, 0.0f, 0.0f);

	if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == AI_SUCCESS)
	{
		printf("Loaded diffuse color [%f %f %f]\n", DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
		m_materials[index].diffuseColor.r = DiffuseColor.r;
		m_materials[index].diffuseColor.g = DiffuseColor.g;
		m_materials[index].diffuseColor.b = DiffuseColor.b;
	}

	aiColor3D SpecularColor(0.0f, 0.0f, 0.0f);

	if (pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor) == AI_SUCCESS) {
		printf("Loaded specular color [%f %f %f]\n", SpecularColor.r, SpecularColor.g, SpecularColor.b);
		m_materials[index].specularColor.r = SpecularColor.r;
		m_materials[index].specularColor.g = SpecularColor.g;
		m_materials[index].specularColor.b = SpecularColor.b;
	}
}
//=============================================================================
void utils::Mesh::populateBuffers()
{
	//if (IsGLVersionHigher(4, 5)) {
	//	populateBuffersDSA();
	//}
	//else {
		populateBuffersNonDSA();
	//}
}
//=============================================================================
void utils::Mesh::populateBuffersNonDSA()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VERTEX_BUFFER]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

	size_t NumFloats = 0;

	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
	NumFloats += 3;

	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
	NumFloats += 2;

	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
}
//=============================================================================
void utils::Mesh::populateBuffersDSA()
{
	glNamedBufferStorage(m_buffers[VERTEX_BUFFER], sizeof(m_vertices[0]) * m_vertices.size(), m_vertices.data(), 0);
	glNamedBufferStorage(m_buffers[INDEX_BUFFER], sizeof(m_indices[0]) * m_indices.size(), m_indices.data(), 0);

	glVertexArrayVertexBuffer(m_vao, 0, m_buffers[VERTEX_BUFFER], 0, sizeof(Vertex));
	glVertexArrayElementBuffer(m_vao, m_buffers[INDEX_BUFFER]);

	size_t NumFloats = 0;

	glEnableVertexArrayAttrib(m_vao, POSITION_LOCATION);
	glVertexArrayAttribFormat(m_vao, POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, (GLuint)(NumFloats * sizeof(float)));
	glVertexArrayAttribBinding(m_vao, POSITION_LOCATION, 0);
	NumFloats += 3;

	glEnableVertexArrayAttrib(m_vao, TEX_COORD_LOCATION);
	glVertexArrayAttribFormat(m_vao, TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, (GLuint)(NumFloats * sizeof(float)));
	glVertexArrayAttribBinding(m_vao, TEX_COORD_LOCATION, 0);
	NumFloats += 2;

	glEnableVertexArrayAttrib(m_vao, NORMAL_LOCATION);
	glVertexArrayAttribFormat(m_vao, NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, (GLuint)(NumFloats * sizeof(float)));
	glVertexArrayAttribBinding(m_vao, NORMAL_LOCATION, 0);
}
//=============================================================================
void utils::Mesh::setupRenderMaterialsPhong(unsigned int MeshIndex, unsigned int MaterialIndex, IRenderCallbacks* pRenderCallbacks)
{
	if (m_materials[MaterialIndex].pDiffuse)
	{
		m_materials[MaterialIndex].pDiffuse->Bind(COLOR_TEXTURE_UNIT);
	}

	if (m_materials[MaterialIndex].pSpecularExponent)
	{
		m_materials[MaterialIndex].pSpecularExponent->Bind(SPECULAR_EXPONENT_UNIT);

		if (pRenderCallbacks)
		{
			pRenderCallbacks->ControlSpecularExponent(true);
		}
	}
	else
	{
		if (pRenderCallbacks)
		{
			pRenderCallbacks->ControlSpecularExponent(false);
		}
	}

	if (pRenderCallbacks)
	{
		if (m_materials[MaterialIndex].pDiffuse)
		{
			pRenderCallbacks->DrawStartCB(MeshIndex);
			pRenderCallbacks->SetMaterial(m_materials[MaterialIndex]);
		}
		else
		{
			pRenderCallbacks->DisableDiffuseTexture();
		}
	}
}
//=============================================================================
void utils::Mesh::setupRenderMaterialsPBR()
{
	int PBRMaterialIndex = 0;

	if (m_materials[PBRMaterialIndex].pbrMaterial.pAlbedo)
	{
		m_materials[PBRMaterialIndex].pbrMaterial.pAlbedo->Bind(ALBEDO_TEXTURE_UNIT);
	}

	if (m_materials[PBRMaterialIndex].pbrMaterial.pRoughness)
	{
		m_materials[PBRMaterialIndex].pbrMaterial.pRoughness->Bind(ROUGHNESS_TEXTURE_UNIT);
	}

	if (m_materials[PBRMaterialIndex].pbrMaterial.pMetallic)
	{
		m_materials[PBRMaterialIndex].pbrMaterial.pMetallic->Bind(METALLIC_TEXTURE_UNIT);
	}

	if (m_materials[PBRMaterialIndex].pbrMaterial.pNormalMap)
	{
		m_materials[PBRMaterialIndex].pbrMaterial.pNormalMap->Bind(NORMAL_TEXTURE_UNIT);
	}
}
//=============================================================================
bool ReadFile(const char* pFileName, std::string& outFile)
{
	std::ifstream f(pFileName);

	bool ret = false;

	if (f.is_open())
	{
		std::string line;
		while (getline(f, line))
		{
			outFile.append(line);
			outFile.append("\n");
		}

		f.close();

		ret = true;
	}
	else 
	{
		throw std::exception(pFileName);
	}

	return ret;
}
//=============================================================================
utils::Technique::~Technique()
{
	// Delete the intermediate shader objects that have been added to the program
	// The list will only contain something if shaders were compiled but the object itself was destroyed prior to linking.
	for (ShaderObjList::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++)
	{
		glDeleteShader(*it);
	}

	if (m_shaderProg != 0)
	{
		glDeleteProgram(m_shaderProg);
		m_shaderProg = 0;
	}
}
//=============================================================================
bool utils::Technique::Init()
{
	m_shaderProg = glCreateProgram();

	if (m_shaderProg == 0) {
		fprintf(stderr, "Error creating shader program\n");
		return false;
	}

	return true;
}
//=============================================================================
// Use this method to add shaders to the program. When finished - call finalize()
bool utils::Technique::addShader(GLenum ShaderType, const char* pFilename)
{
	std::string s;

	if (!ReadFile(pFilename, s)) {
		return false;
	}

	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		return false;
	}

	// Save the shader object - will be deleted in the destructor
	m_shaderObjList.push_back(ShaderObj);

	const GLchar* p[1];
	p[0] = s.c_str();
	GLint Lengths[1] = { (GLint)s.size() };

	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj);

	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling '%s': '%s'\n", pFilename, InfoLog);
		return false;
	}

	glAttachShader(m_shaderProg, ShaderObj);

	return true;
}
//=============================================================================
// After all the shaders have been added to the program call this function to link and validate the program.
bool utils::Technique::finalize()
{
	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(m_shaderProg);

	glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		return false;
	}

	glValidateProgram(m_shaderProg);

	glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		return false;
	}

	// Delete the intermediate shader objects that have been added to the program
	for (ShaderObjList::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++) {
		glDeleteShader(*it);
	}

	m_shaderObjList.clear();

	//    PrintUniformList();

	return GLCheckError();
}
//=============================================================================
void utils::Technique::printUniformList()
{
	int Count = 0;
	glGetProgramiv(m_shaderProg, GL_ACTIVE_UNIFORMS, &Count);
	printf("Active Uniforms: %d\n", Count);

	GLint Size;
	GLenum Type;
	const GLsizei BufSize = 16;
	GLchar Name[BufSize];
	GLsizei Length;

	for (int i = 0; i < Count; i++) {
		glGetActiveUniform(m_shaderProg, (GLuint)i, BufSize, &Length, &Size, &Type, Name);

		printf("Uniform #%d Type: %u Name: %s\n", i, Type, Name);
	}
}
//=============================================================================
void utils::Technique::Enable()
{
	glUseProgram(m_shaderProg);
}
//=============================================================================
GLint utils::Technique::getUniformLocation(const char* pUniformName)
{
	GLuint Location = glGetUniformLocation(m_shaderProg, pUniformName);

	if (Location == INVALID_UNIFORM_LOCATION) {
		fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
	}

	return Location;
}
//=============================================================================