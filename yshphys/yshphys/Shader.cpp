#include "stdafx.h"
#include "Shader.h"
#include "Utils.h"


Shader::Shader() : m_program(0)
{
}
Shader::~Shader()
{
	glDeleteProgram(m_program);
}
GLuint Shader::GetProgram() const
{
	return m_program;
}

bool Shader::Bind() const
{
	if (m_program == 0)
	{
		return false;
	}
	else
	{
		glUseProgram(m_program);
		return true;
	}
}

// http://www.nexcius.net/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c/
void Shader::LoadShader(const char* vertex_path, const char* fragment_path)
{
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read shaders
	std::string vertShaderStr = Utils::ReadFile(vertex_path);
	std::string fragShaderStr = Utils::ReadFile(fragment_path);
	const char *vertShaderSrc = vertShaderStr.c_str();
	const char *fragShaderSrc = fragShaderStr.c_str();

	GLint result = GL_FALSE;
	int logLength;

	// Compile vertex shader
	std::cout << "Compiling vertex shader." << std::endl;
	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);

	// Check vertex shader
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
	std::cout << &vertShaderError[0] << std::endl;

	// Compile fragment shader
	std::cout << "Compiling fragment shader." << std::endl;
	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);

	// Check fragment shader
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
	std::cout << &fragShaderError[0] << std::endl;

	std::cout << "Linking program" << std::endl;
	m_program = glCreateProgram();
	glAttachShader(m_program, vertShader);
	glAttachShader(m_program, fragShader);
	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &result);
	glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(m_program, logLength, NULL, &programError[0]);
	std::cout << &programError[0] << std::endl;

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

void Shader::LoadShader(const char* vertex_path, const char* geometry_path, const char* fragment_path)
{
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint geomShader = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read shaders
	std::string vertShaderStr = Utils::ReadFile(vertex_path);
	std::string geomShaderStr = Utils::ReadFile(geometry_path);
	std::string fragShaderStr = Utils::ReadFile(fragment_path);
	const char *vertShaderSrc = vertShaderStr.c_str();
	const char *geomShaderSrc = geomShaderStr.c_str();
	const char *fragShaderSrc = fragShaderStr.c_str();

	GLint result = GL_FALSE;
	int logLength;

	// Compile vertex shader
	std::cout << "Compiling vertex shader." << std::endl;
	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);
	// Check vertex shader
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
	std::cout << &vertShaderError[0] << std::endl;
	
	// Compile geometry shader
	std::cout << "Compiling geometry shader." << std::endl;
	glShaderSource(geomShader, 1, &geomShaderSrc, NULL);
	glCompileShader(geomShader);
	// Check geometry shader
	glGetShaderiv(geomShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(geomShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> geomShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(geomShader, logLength, NULL, &geomShaderError[0]);
	std::cout << &geomShaderError[0] << std::endl;

	// Compile fragment shader
	std::cout << "Compiling fragment shader." << std::endl;
	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);
	// Check fragment shader
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
	std::cout << &fragShaderError[0] << std::endl;

	std::cout << "Linking program" << std::endl;
	m_program = glCreateProgram();
	glAttachShader(m_program, vertShader);
	glAttachShader(m_program, geomShader);
	glAttachShader(m_program, fragShader);
	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &result);
	glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(m_program, logLength, NULL, &programError[0]);
	std::cout << &programError[0] << std::endl;

	glDeleteShader(vertShader);
	glDeleteShader(geomShader);
	glDeleteShader(fragShader);
}
