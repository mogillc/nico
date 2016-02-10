#version 330

uniform sampler2D bgl_RenderedTexture;
uniform sampler2D bgl_DepthTexture;
uniform float bgl_RenderedTextureWidth;
uniform float bgl_RenderedTextureHeight;

in vec4 TexCoord[1];

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture(bgl_RenderedTexture, TexCoord[0].st);
}
