#version 420

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4 uModel;
uniform mat4 uProj;
uniform mat4 uView;

// Input from Vertex shader
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn[]; // array size = num vertices in primitve

// Output from geometry shader
out vec3 position;
out vec2 texCoord;
out vec3 normal;
out vec4 colour;


// Geometry shader pass thru
void PassThru()
{
	for(int i =0; i< 3; i++)
	{
    	vec4 viewSpacePos = uView * uModel * gl_in[i].gl_Position;
    	position = viewSpacePos.xyz;
	    gl_Position = uProj * viewSpacePos;
		
                 //  gl_Position = uModel * gl_in[i].gl_Position;
		colour = vec4(1.0);
		normal = mat3(uView) * mat3(uModel) * vIn[i].normal;
	            //	position = (uView * gl_in[i].gl_Position).xyz;
		texCoord = vIn[i].texCoord.xy;
		EmitVertex();
	}
	EndPrimitive();
}

void Duplicate()
{
	for(int i =0; i< 3; i++)
	{
		vec4 offset = gl_in[i].gl_Position + vec4(0,0,4,0);

    	vec4 viewSpacePos = uView * uModel *gl_in[i].gl_Position;
    	position = (viewSpacePos * offset).xyz;
	    gl_Position = uProj *  (uView * uModel * offset);

        		//gl_Position = uModel * offset;
		colour = vec4(1.0);
		normal = mat3(uView) * mat3(uModel) * vIn[i].normal;
        		//position = (uView * offset).xyz;
		texCoord = vIn[i].texCoord.xy;
		EmitVertex();
	}
	EndPrimitive();
}


void main()
{
	PassThru();
	Duplicate();
}




