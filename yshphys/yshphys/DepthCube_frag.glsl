#version 400

in vec3 ex_fragPos;

uniform vec3 pointLightPos;
uniform float gNear;
uniform float gFar;

void main(void)
{
	vec3 lightToFrag = ex_fragPos - pointLightPos;
	float d = length(lightToFrag);

	// For the depth cube map, we want to export the depth in world space (as opposed to the default Normalized Device Coordinates)
	// Beyond making the exported depth more intuitive, it means that our maximum depth of 1 corresponds to the surface of a sphere.
	// This is preferable, since the cube map samples all direction, and a spherical boundary, being isometric, aligns with this notion.
	// Put another way, if we use NDC, the maximum depth would correspond to an axis-aligned cube, which depends on our choice of frame.

	gl_FragDepth = (d - gNear) / (gFar - gNear);
}