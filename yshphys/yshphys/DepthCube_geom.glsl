#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 projectionViewMatrices[6];

out vec3 ex_fragPos; // FragPos from GS (output per emitvertex)

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.

        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            ex_fragPos = gl_in[i].gl_Position.xyz;
            gl_Position = projectionViewMatrices[face] * gl_in[i].gl_Position;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  