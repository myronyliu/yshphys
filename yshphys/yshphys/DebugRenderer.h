#pragma once
#include "Viewport.h"
#include "Shader_FlatUniformColor.h"
#include "BVTree.h"
#include <glew.h>

#define SINGLE_DRAW_VERTEX_BUFFER_SIZE 64
#define SINGLE_DRAW_INDEX_BUFFER_SIZE 256

class Picker;

class DebugRenderer
{
	friend class RenderScene;
public:
	// The following functions are for one time draws. The caller has no control over the shader. Everything is drawn with flat colors.
	// Intended for debugging and should be used sparingly as these shuttle memory to the GPU each time they are called.
	void DrawTriangle(const fVec3& A, const fVec3& B, const fVec3& C, const fVec3& color, bool wireFrame);
	void DrawPolygon(const fVec3* verts, int nVerts, const fVec3& color, bool wireFrame);
	void DrawBox(float halfDimX, float halfDimY, float halfDimZ, const fVec3& pos, const fQuat& rot, const fVec3& color, bool wireFrame);
	void DrawCone(const fVec3& tip, const fVec3& base, const float& radius, const fVec3& color);
	void DrawLine(const fVec3& start, const fVec3& end, const fVec3& color);
	void DrawBVTree(const BVTree& tree, const fVec3& color);
	void DrawPicker(const Picker& picker, const fVec3& color);

	Picker* m_picker;

protected:

	DebugRenderer();
	virtual ~DebugRenderer();

	void DrawObjects(const Viewport& viewport) const;
	void EvictObjects();

	struct DebugDrawData
	{
		float vertices[SINGLE_DRAW_VERTEX_BUFFER_SIZE][3];
		float normals[SINGLE_DRAW_VERTEX_BUFFER_SIZE][3];
		unsigned int indices[SINGLE_DRAW_INDEX_BUFFER_SIZE];
		unsigned int nVertices = 0;
		unsigned int nIndices= 0;

		GLenum polygonType = GL_TRIANGLES;

		fVec3 pos = fVec3(0.0f, 0.0f, 0.0f);
		fQuat rot = fQuat(0.0f, 0.0f, 0.0f, 1.0f);
		float color[3] = { 1.0f, 1.0f, 1.0f };
	};

	std::vector<DebugDrawData> m_objects;
	GLuint m_VAO;
	GLuint m_xVBO;
	GLuint m_nVBO;
	GLuint m_IBO;
	Shader_FlatUniformColor m_shader;


};

