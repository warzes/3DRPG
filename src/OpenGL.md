разделы docs.gl 4.5

	Textures
	Rendering
	FrameBuffers
		+Shaders
	Buffers
	State
	TransformFeedback
	Utility
	Queries
	Syncing
		+VertexArray
	Samplers
	ProgramPipelines
	Debug


// настройка сглаживания в мультисэмплинге
glEnable(GL_MULTISAMPLE); // Включаем мультисэмплинг
glEnable(GL_SAMPLE_SHADING); // Включаем sample shading
glMinSampleShading(0.5f); // Устанавливаем минимальную частоту затенения
if (isLowEndDevice) {
    glMinSampleShading(0.25f); // Низкая частота затенения для слабых устройств
} else {
    glMinSampleShading(1.0f); // Максимальное качество для мощных устройств
}



вместо glCreateShader с 4.1:
GLuint shaderProgram0 = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vertexShaderSource);
GLuint shaderProgram1 = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragmentShaderSource);
glUseProgramStages(shaderProgram0, GL_VERTEX_SHADER_BIT, shaderProgram1);

вместо glUniform использовать glProgramUniform