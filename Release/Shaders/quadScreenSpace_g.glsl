// Geometry shader which turns points into billboarded quads

#version 420

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// Input from cpp shader
uniform float in_width;
uniform float in_height;
uniform float in_time;

// Output from geometry shader

    out vec3 position;
    out vec2 texCoord;
    
// Creates a quad of specified size around point p
void PointToQuadBillboarded(vec4 p, float w, float h)
{
	float halfWidth = w * 0.5;
	float halfHeight = h * 0.5;
	
	vec4 pEye = p;
	
	vec4 topLeft = vec4(pEye.xy +	  vec2(-halfWidth,  halfHeight), pEye.z, 1.0);
	vec4 bottomLeft = vec4(pEye.xy +  vec2(-halfWidth, -halfHeight), pEye.z, 1.0);
    vec4 topRight = vec4(pEye.xy +	  vec2( halfWidth,  halfHeight), pEye.z, 1.0);
    vec4 bottomRight = vec4(pEye.xy + vec2( halfWidth, -halfHeight), pEye.z, 1.0);

	gl_Position = bottomLeft;
	texCoord.xy = vec2(0,0);
	EmitVertex();

	gl_Position = topLeft;
	texCoord.xy = vec2(0,1);
	EmitVertex();

	gl_Position = bottomRight;
	texCoord.xy = vec2(1,0);
	EmitVertex();

	gl_Position = topRight;
	texCoord.xy = vec2(1,1);
	EmitVertex();

	EndPrimitive();
}

void main()
{
    PointToQuadBillboarded(gl_in[0].gl_Position, in_width, in_height);
}
