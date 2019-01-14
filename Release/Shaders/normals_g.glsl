// Geometry shader which draws vertex and face normals and wireframe.

#version 400

layout(triangles) in;
layout(line_strip, max_vertices = 64) out;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

// Input from Vertex shader
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec3 posEye;
} vIn[]; // array size = num vertices in primitve

// Output from geometry shader
//out VertexData
//{
//	vec3 normal;
//	vec3 texCoord;
//	vec4 colour;
//	vec3 posEye;
//} vOut; // array size depends on max_vertices

out vec3 position;
out vec2 texCoord;
out vec3 normal;
out vec4 colour;
// Outputs vertex normals
void VertexNormals()
{
	float lineLength = 0.2f;

	for (int i = 0; i<3; i++)
{
	gl_Position = uProj * uView * uModel * gl_in[i].gl_Position;
	colour = vec4(0,1,0,1);
	EmitVertex();

	vec3 lineEnd = gl_in[i].gl_Position.xyz + vIn[i].normal * lineLength;
	gl_Position = uProj * uView * uModel * vec4(lineEnd, 1.0);
	colour = vec4(0,1,0,1);
	EmitVertex();

	EndPrimitive();
}
}

void FaceNormals();
void WireFrame();

void main()
{
	VertexNormals();
	FaceNormals();
	WireFrame();
}

// Outputs face normals normals
void FaceNormals()
{
}

void WireFrame()
{
	//index is 0 - 1
	gl_Position = uProj * uView * uModel * vec4((gl_in[0].gl_Position.xyz + vIn[0].normal.xyz* 0.003), 1.0);
	colour = vec4(0,0,0,0);
	EmitVertex();

	gl_Position = uProj * uView * uModel * vec4((gl_in[1].gl_Position.xyz + vIn[1].normal.xyz* 0.003), 1.0);
	colour = vec4(0,0,0,0);
	EmitVertex();
	
EndPrimitive();

	//Second Line
	// index is 0 - 2
	gl_Position = uProj * uView * uModel * vec4((gl_in[0].gl_Position.xyz + vIn[0].normal.xyz* 0.003), 1.0);
	colour = vec4(0,0,0,0);
	EmitVertex();

	gl_Position = uProj * uView * uModel * vec4((gl_in[2].gl_Position.xyz + vIn[2].normal.xyz* 0.003), 1.0);
	colour = vec4(0,0,0,0);
	EmitVertex();

EndPrimitive();

	//Third Frame
	// index is 1 - 2	
	gl_Position = uProj * uView * uModel * vec4((gl_in[1].gl_Position.xyz + vIn[1].normal.xyz* 0.003), 1.0);
	colour = vec4(0,0,0,0);
	EmitVertex();

	gl_Position = uProj * uView * uModel * vec4((gl_in[2].gl_Position.xyz + vIn[2].normal.xyz* 0.003), 1.0);
	colour = vec4(0,0,0,0);
	EmitVertex();

EndPrimitive();


}

