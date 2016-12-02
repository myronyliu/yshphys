#pragma once
#include <glew.h>
// https://www.opengl.org/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)
class Shader
{
public:
	Shader();
	virtual ~Shader();

	GLuint GetProgram() const;

protected:
	// Each shader class will load from hardcoded filepaths in the constructor
	void LoadShader(const char *vertex_path, const char *fragment_path);

	GLuint m_program;
};

