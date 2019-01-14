#version 420

uniform float u_bloomThreshold = 0.1;

// Note: Uniform bindings
// This lets you specify the texture unit directly in the shader!
layout(binding = 10) uniform sampler2D u_scene; // original scene image

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

void main()
{
    // IMPLEMENT BRIGHT PASS HERE
    // - See pseudo code in lab document
    vec4 colour = texture(u_scene, texCoord.xy);
    vec4 brightPixel = vec4((colour - u_bloomThreshold) / (1.0 - u_bloomThreshold));

    outColor = clamp(brightPixel, 0.0, 1.0);
}