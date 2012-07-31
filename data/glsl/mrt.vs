// A simple vertex shader
#version 110

varying vec3 normal;
varying vec4 ecPosition;

void mrt()
{
	normal = gl_NormalMatrix * gl_Normal;

	ecPosition = gl_ModelViewMatrix * gl_Vertex;

	gl_FrontColor = gl_Color;
	gl_Position = ftransform();

	gl_TexCoord[0] = gl_MultiTexCoord0;
}

void main()
{
	mrt();
}
