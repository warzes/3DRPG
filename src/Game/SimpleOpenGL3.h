#pragma once

namespace utils
{
	class Transform final
	{
	public:
		void SetPosition(const glm::vec3& position)
		{
			m_position = position;
		}

		void SetScale(const glm::vec3& scale)
		{
			m_scale = scale;
		}
		void SetScale(float scale)
		{
			m_scale = glm::vec3(scale);
		}

		void SetRotation(const glm::vec3& eulerAngles)
		{
			m_rotation = glm::quat(eulerAngles);
		}

		void SetRotation(const glm::quat& rotation)
		{
			m_rotation = rotation;
		}

		void Translate(const glm::vec3 movePosition)
		{
			m_position += movePosition;
		}

		void Rotate(const glm::vec3& anglesRadians)
		{
			glm::quat rotationX = glm::angleAxis(anglesRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::quat rotationY = glm::angleAxis(anglesRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat rotationZ = glm::angleAxis(anglesRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));

			glm::quat combinedRotation = rotationZ * rotationY * rotationX;
			m_rotation *= combinedRotation;
		}

		const glm::vec3& GetPosition() const { return m_position; }
		const glm::vec3& GetScale() const { return m_scale; }
		const glm::quat& GetRotation() const { return m_rotation; }

		glm::mat4 GetWorldMatrix() const
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);
			glm::mat4 rotation = glm::toMat4(m_rotation);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);

			return translation * rotation * scale;
		}

	private:
		glm::vec3 m_position = glm::vec3(0.0f);
		glm::vec3 m_scale = glm::vec3(1.0f);
		glm::quat m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	};

	enum class CameraMovement : uint8_t
	{
		Forward,
		Backward,
		Left,
		Right
	};

	class Camera final
	{
	public:

		Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = 90.0f, float pitch = 0.0f) 
			: m_position(position)
			, m_yaw(yaw)
			, m_pitch(pitch)
			, m_worldUp(up)
		{
			updateVector();
		}

		void Move(CameraMovement direction, float deltaTime)
		{
			float velocity = m_speed * deltaTime;
			if (direction == CameraMovement::Forward)      m_position += m_front * velocity;
			else if(direction == CameraMovement::Backward) m_position -= m_front * velocity;
			else if (direction == CameraMovement::Left)    m_position += m_right * velocity;
			else if (direction == CameraMovement::Right)   m_position -= m_right * velocity;
		}

		void Rotate(float xOffset, float yOffset)
		{
			xOffset *= m_sensitivity;
			yOffset *= m_sensitivity;

			m_yaw += xOffset;
			m_pitch = glm::clamp(m_pitch + yOffset, -89.0f, 89.0f);

			updateVector();
		}

		glm::mat4 GetViewMatrix() const
		{
			return glm::lookAt(m_position, m_position + m_front, m_up);
		}
	private:
		void updateVector()
		{
			float yaw = glm::radians(m_yaw);
			float pitch = glm::radians(m_pitch);

			glm::vec3 newFront;
			newFront.x = cosf(yaw) * cosf(pitch);
			newFront.y =             sinf(pitch);
			newFront.z = sinf(yaw) * cosf(pitch);
			m_front = glm::normalize(newFront);

			m_right = glm::normalize(glm::cross(m_front, m_worldUp));
			m_up = glm::normalize(glm::cross(m_right, m_front));
		}

		// camera Attributes
		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::vec3 m_worldUp;
		// euler angles
		float m_yaw{ 90.0f };
		float m_pitch{ 0.0 };
		// camera options
		float m_speed{ 10.0f };
		float m_sensitivity{ 0.1f };
	};
}

namespace gl3
{
	inline GLuint CreateShader(GLenum type, const char* shaderSource)
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

	inline GLuint CreateShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
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

	inline GLuint CreateVertexBuffer(GLenum usage, size_t size, void* data)
	{
		GLint lastBuffer;
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastBuffer);

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);

		glBindBuffer(GL_ARRAY_BUFFER, lastBuffer);

		return buffer;
	}

	inline GLuint CreateIndexBuffer(GLenum usage, size_t size, void* data)
	{
		GLint lastBuffer;
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &lastBuffer);

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lastBuffer);

		return buffer;
	}
}