//#version 330
precision highp float;

//uniform sampler2D bgl_RenderedTexture;
//uniform sampler2D bgl_DepthTexture;
uniform highp sampler2D bgl_RenderedTexture;
uniform highp sampler2D bgl_DepthTexture;
uniform float bgl_RenderedTextureWidth;
uniform float bgl_RenderedTextureHeight;

//in vec4 TexCoord[1];
varying highp vec4 TexCoord;

//layout(location = 0) out vec4 FragColor;	// TODO: may need to find alternative

void main()
{
	//FragColor = texture(bgl_RenderedTexture, TexCoord[0].st);
	gl_FragColor = texture2D(bgl_RenderedTexture, TexCoord.st);
//	gl_FragColor = vec4(0.5,0.5,0.5,1.0);
}
