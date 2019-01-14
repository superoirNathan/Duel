#version 420

// (1.0 / windowWidth, 1.0 / windowHeight)
uniform vec4 u_texelSize; // Remember to set this!

layout(binding = 10) uniform sampler2D u_bright; // bright pass image

struct Material
{
    //Maps
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
    
    //Texture
    sampler2D texture;
    
    //WARP_1
    sampler2D warp;

    //Variables
	vec3 hue;
	float specularExponent;
};

uniform Material material;


in vec3 position;
in vec2 texCoord;

out vec4 outColor;

out vec4 FragColor;

vec3 gaussianBlur(sampler2D tex)
{
	vec2 uv = (texCoord).xy;

	vec2 offsetCoordinates[9];

	// Left column
	offsetCoordinates[0] = vec2(-u_texelSize.x, u_texelSize.y) + uv;	// top left
	offsetCoordinates[1] = vec2(-u_texelSize.x, 0.0) + uv;				// middle left
	offsetCoordinates[2] = vec2(-u_texelSize.x, -u_texelSize.y) + uv;	// bottom left

	// Mid column
	offsetCoordinates[3] = vec2(-u_texelSize.x, u_texelSize.y) + uv;	// top middle
	offsetCoordinates[4] = uv;											// center
	offsetCoordinates[5] = vec2(0.0, -u_texelSize.y) + uv;				// bottom middle

	// right column
	offsetCoordinates[6] = vec2(u_texelSize.x, u_texelSize.y) + uv;		// top right
	offsetCoordinates[7] = vec2(u_texelSize.x, 0.0f) + uv;				// middle right
	offsetCoordinates[8] = vec2(u_texelSize.x, -u_texelSize.y) + uv;	// bottom right

	vec3 blurred = vec3(0.0);

	blurred += texture(tex, offsetCoordinates[0]).rgb* 0.077847;        //Value multiplied are 3x3 kernel weights 
	blurred += texture(tex, offsetCoordinates[1]).rgb* 0.123317;
	blurred += texture(tex, offsetCoordinates[2]).rgb* 0.077847;
													 
	blurred += texture(tex, offsetCoordinates[3]).rgb* 0.123317;
	blurred += texture(tex, offsetCoordinates[4]).rgb* 0.195346;
	blurred += texture(tex, offsetCoordinates[5]).rgb* 0.123317;
													 
	blurred += texture(tex, offsetCoordinates[6]).rgb* 0.077847;
	blurred += texture(tex, offsetCoordinates[7]).rgb* 0.123317;
	blurred += texture(tex, offsetCoordinates[8]).rgb* 0.077847;

	blurred /= 9.0;

	return blurred;
}

void main()
{
	// IMPLEMENT 2D GAUSSIAN BLUR HERE
	// - Exact same convolution process as a box blur the only difference 
	//	is that you just need to scale each sample by a weight.

	vec3 blurred = gaussianBlur(u_bright);
	outColor = vec4(blurred, 1.0);
}