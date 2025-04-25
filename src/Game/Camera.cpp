#include "stdafx.h"
#include "Camera.h"
//=============================================================================
#define PI 3.14159265358979323846
//=============================================================================
glm::mat4 makeInfReversedZProjRH(float fovY_radians, float aspectWbyH, float zNear)
{
	float f = 1.0f / tan(fovY_radians / 2.0f);
	return glm::mat4(
		f / aspectWbyH, 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, zNear, 0.0f);
}
//=============================================================================
Camera::Camera(CameraType type) : m_type(type)
{
}
//=============================================================================
void Camera::Init(float FOV, float width, float height, float nearPlane, float farPlane)
{
	m_FOV = FOV;
	m_width = width;
	m_height = height;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_position = glm::vec3(0.0f, 0.0f, 4.0f);
	m_lookPoint = glm::vec3(0.0);
}
//=============================================================================
void Camera::SetPosition(const glm::vec3& position)
{
	m_position = position;
}
//=============================================================================
void Camera::LookAt(const glm::vec3& position)
{
	m_lookPoint = position;
}
//=============================================================================
void Camera::UpdateViewMatrix()
{
	if (m_forceLookAtpoint)
		m_direction = -glm::normalize(m_position - m_lookPoint);
	else
		m_lookPoint = m_position + m_direction;

	m_viewMatrix = glm::lookAt(m_position, m_lookPoint, glm::vec3(0.0f, 1.0f, 0.0f));
}
//=============================================================================
void Camera::UpdateProjectionMatrix()
{
	switch (m_type)
	{
	case CameraType::Perspective:
#ifdef INVERTED_Z
		m_projectionMatrix = makeInfReversedZProjRH(m_FOV, m_width / m_height, m_nearPlane);
#else
		m_projectionMatrix = glm::perspective(m_FOV, m_width / m_height, m_nearPlane, m_farPlane);
#endif
		break;

	case CameraType::Orthographic:
#ifdef INVERTED_Z
		m_projectionMatrix = glm::ortho<float>(-m_width / 2, m_width / 2, -m_height / 2, m_height / 2, m_farPlane, m_nearPlane);
#else
		m_projectionMatrix = glm::ortho<float>(-m_width / 2, m_width / 2, -m_height / 2, m_height / 2, m_nearPlane, m_farPlane);
#endif
		break;

	default:
		break;
	}
}
//=============================================================================
Frustum Camera::GetFrustum() { return m_frustum; }
//=============================================================================
void Camera::UpdateFrustum()
{
	if (m_type == CameraType::Perspective)
	{
		const glm::vec3 front = m_forceLookAtpoint ? glm::normalize(m_lookPoint - m_position) : m_direction;

		const glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), front));
		const glm::vec3 up = glm::cross(right, front);
		const glm::vec3 p = m_position;
		const float d = glm::length(p);

		const float n = m_nearPlane;
		const float f = m_farPlane;

		const float halfVSide = f * tanf(m_FOV * .5f);
		const float halfHSide = halfVSide * m_width / m_height;
		const glm::vec3 fvec = f * front;

		m_frustum.near.position = p + (n * front);
		m_frustum.near.distance = glm::length(m_frustum.near.position);
		m_frustum.near.normal = front;

		m_frustum.far.position = p + fvec;
		m_frustum.far.distance = glm::length(m_frustum.far.position);
		m_frustum.far.normal = -front;

		m_frustum.right.distance = d;
		m_frustum.right.position = p;
		m_frustum.right.normal = glm::normalize(glm::cross(fvec - (right * halfHSide), up));

		m_frustum.left.distance = d;
		m_frustum.left.position = p;
		m_frustum.left.normal = glm::normalize(glm::cross(up, fvec + (right * halfHSide)));

		m_frustum.top.distance = d;
		m_frustum.top.position = p;
		m_frustum.top.normal = glm::normalize(glm::cross(right, fvec - up * halfVSide));

		m_frustum.bottom.distance = d;
		m_frustum.bottom.position = p;
		m_frustum.bottom.normal = glm::normalize(glm::cross(fvec + up * halfVSide, right));
	}
	else
	{
		const glm::vec3 front = m_forceLookAtpoint ? glm::normalize(m_lookPoint - m_position) : m_direction;
		const glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), front));
		const glm::vec3 up = cross(right, front);
		const glm::vec3 p = m_position;

		const float n = m_nearPlane;
		const float f = m_farPlane;

		const float w = m_width / 2;
		const float h = m_height / 2;

		m_frustum.near.position = p + (n * front);
		m_frustum.near.normal = front;

		m_frustum.far.position = p + (f * front);
		m_frustum.far.normal = -front;

		m_frustum.right.position = p + (w * right);
		m_frustum.right.normal = -right;

		m_frustum.left.position = p - (w * right);
		m_frustum.left.normal = right;

		m_frustum.top.position = p + (h * up);
		m_frustum.top.normal = -up;

		m_frustum.bottom.position = p - (h * up);
		m_frustum.bottom.normal = up;
	}
}
//=============================================================================
const glm::mat4 Camera::UpdateProjectionViewMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();
	m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix * glm::mat4(1.0);

	UpdateFrustum();

	return m_projectionViewMatrix;
}
//=============================================================================
void Camera::Move(glm::vec3 velocity, double deltatime)
{
	velocity *= deltatime;
	m_position += m_direction * velocity.x;
	m_position += glm::cross(glm::vec3(0.f, 1.f, 0.f), m_direction) * velocity.z;
	m_position.y += velocity.y;
}
//=============================================================================
void Camera::SetFrameBufferSize(float width, float height)
{
	m_width = width;
	m_height = height;
}
//=============================================================================
Camerao2::Camerao2(float Fov, float Near, float Far, float AspectRatio, glm::vec3 Position, glm::vec3 Forward)
	: fov(Fov)
	, near(Near)
	, far(Far)
	, aspectRatio(AspectRatio)
	, position(Position)
{
	forward = glm::normalize(Forward);
	worldUp = glm::vec3(0, 1, 0);
	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::normalize(glm::cross(right, forward));

	roll = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;

	orientation = glm::quat();
	projection = glm::perspective(glm::radians(fov), aspectRatio, near, far);
	prevViewProjection = glm::mat4(1.0f);

	Update();
}
//=============================================================================
void Camerao2::UpdateProjection(float Fov, float Near, float Far, float AspectRatio)
{
	projection = glm::perspective(glm::radians(Fov), AspectRatio, Near, Far);
}
//=============================================================================
void Camerao2::SetTranslationDelta(glm::vec3 Direction, float Amount)
{
	position += Direction * Amount;
}
//=============================================================================
void Camerao2::SetRotationDelta(glm::vec3 Angles)
{
	yaw = glm::radians(Angles.y);
	pitch = glm::radians(Angles.x);
	roll = glm::radians(Angles.z);
}
//=============================================================================
void Camerao2::SetPosition(glm::vec3 Position) { position = Position; }
//=============================================================================
void Camerao2::Update()
{
	glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qRoll = glm::angleAxis(roll, glm::vec3(0.0f, 0.0f, 1.0f));

	orientation = qPitch * orientation;
	orientation = orientation * qYaw;
	orientation = qRoll * orientation;
	orientation = glm::normalize(orientation);

	rotate = glm::mat4_cast(orientation);
	forward = glm::conjugate(orientation) * glm::vec3(0.0f, 0.0f, -1.0f);

	right = glm::normalize(glm::cross(forward, worldUp));

	up = glm::normalize(glm::cross(right, forward));

	translate = glm::translate(glm::mat4(1.0f), -position);
	view = rotate * translate;
	prevViewProjection = viewProjection;
	viewProjection = projection * view;

	FrustumFromMatrix(frustum, viewProjection);
}
//=============================================================================
void Camerao2::UpdateFromFrame(glm::vec3 Position, glm::vec3 Forward, glm::vec3 Right)
{
	position = Position;
	forward = Forward;
	right = Right;

	up = glm::normalize(glm::cross(right, forward));

	view = glm::lookAt(position, position + forward * 10.0f, up);
	prevViewProjection = viewProjection;
	viewProjection = projection * view;

	FrustumFromMatrix(frustum, viewProjection);
}
//=============================================================================
Camerao::Camerao(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
	: m_position(position)
	, m_front(glm::vec3(0.0f, 0.0f, -1.0f))
	, m_up(up)
	, m_right(glm::vec3(1.0f, 0.0f, 0.0f))
	, m_worldUp(up)
	, m_yaw(yaw)
	, m_pitch(pitch)
	, m_movementSpeed(SPEED)
	, m_mouseSensitivity(SENSITIVITY)
	, m_zoom(ZOOM)
{
	updateCameraVectors();
}
//=============================================================================
glm::mat4 Camerao::GetViewMatrix() const
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}
//=============================================================================
glm::mat4 Camerao::GetProjectionMatrix(float aspect) const
{
	return glm::perspective(glm::radians(m_zoom), aspect, 0.1f, 1000.0f);
}
//=============================================================================
void Camerao::SetSpeed(float val)
{
	m_movementSpeed = val;
}
//=============================================================================
void Camerao::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= m_mouseSensitivity;
	yoffset *= m_mouseSensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	if (m_pitch > 89.0f) m_pitch = 89.0f;
	if (m_pitch < -89.0f) m_pitch = -89.0f;

	updateCameraVectors();
}
//=============================================================================
void Camerao::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
	float velocity = m_movementSpeed * deltaTime;
	if (direction == CameraMovement::Forward)  m_position += m_front * velocity;
	if (direction == CameraMovement::Backward) m_position -= m_front * velocity;
	if (direction == CameraMovement::Left)     m_position -= m_right * velocity;
	if (direction == CameraMovement::Right)    m_position += m_right * velocity;

	if (direction == CameraMovement::Up)       m_position += m_up * velocity;
	if (direction == CameraMovement::Down)     m_position -= m_up * velocity;
}
//=============================================================================
void Camerao::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}
//=============================================================================