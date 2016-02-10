#version 330

uniform sampler2D bgl_RenderedTexture;
uniform sampler2D bgl_DepthTexture;
uniform float bgl_RenderedTextureWidth;
uniform float bgl_RenderedTextureHeight;

#define PI  (3.14159265)

float width = bgl_RenderedTextureWidth; //texture width
float height = bgl_RenderedTextureHeight; //texture height

uniform float focalDepth;  //focal distance value in meters, but you may use autofocus option below
uniform float focalLength; //focal length in mm
uniform float fstop; //f-stop value
uniform bool showFocus; //show debug focus point and focal range (red = focal point, green = focal range)

uniform bool autofocus;


uniform float znear; //camera clipping start
uniform float zfar; //camera clipping end

int samples = 3;		//samples on the first ring
int rings = 3;			//ring count

float CoC = 0.03;		//circle of confusion

bool vignetting = true; //use optical lens vignetting
float vignout = 1.3; //vignetting outer border
float vignin = 0.0; //vignetting inner border
float vignfade = 4.0; //f-stops till vignete fades

vec2 focus = vec2(0.5,0.5); // autofocus point on screen (0.0,0.0 - left lower corner, 1.0,1.0 - upper right)
float maxblur = 2.0; //clamp value of max blur (0.0 = no blur,1.0 default)

float threshold = 0.5; //highlight threshold;
float gain = 0.8; //highlight gain;

float bias = 0.5; //bokeh edge bias
float fringe = 0.0; //bokeh chromatic aberration/fringing

bool noise = true; //use noise instead of pattern for sample dithering
float namount = 0.0001; //dither amount


in vec4 TexCoord[1];
//layout(location = 0) out FragColor;
layout(location = 0) out vec4 FragColor;

vec3 color(vec2 coords,float blur) //processing the sample
{
	vec3 col = vec3(0.0);

	col.r = texture( bgl_RenderedTexture, coords + vec2( 0.0, 1.0) * fringe * blur).r;
	col.g = texture( bgl_RenderedTexture, coords + vec2( -0.866, -0.5) * fringe * blur).g;
	col.b = texture( bgl_RenderedTexture, coords + vec2( 0.866, -0.5) * fringe * blur).b;

	vec3 lumcoeff = vec3( 0.299, 0.587, 0.114);
	float lum = dot( col.rgb, lumcoeff);
	float thresh = max( (lum - threshold)*gain, 0.0);
	return col + mix( vec3(0.0), col, thresh*blur);
}

vec2 rand(vec2 coord) //generating noise/pattern texture for dithering
{
	float noiseX = ((fract( 1.0 - coord.s*(width/2.0))*0.25) + (fract(coord.t*(height/2.0))*0.75))*2.0 - 1.0;
	float noiseY = ((fract( 1.0 - coord.s*(width/2.0))*0.75) + (fract(coord.t*(height/2.0))*0.25))*2.0 - 1.0;

	if (noise)
	{
		noiseX = clamp(fract(sin(dot(coord , vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0 - 1.0;
		noiseY = clamp(fract(sin(dot(coord , vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0 - 1.0;
	}
	return vec2( noiseX, noiseY);
}

vec3 debugFocus(vec3 col, float blur, float depth)
{
	float edge = 0.002*depth; //distance based edge smoothing
	float m = clamp(smoothstep(0.0, edge, blur), 0.0, 1.0);
	float e = clamp(smoothstep(1.0 - edge, 1.0, blur), 0.0, 1.0);

	col = mix( col, vec3( 1.0, 0.5, 0.0), (1.0 - m)*0.6);
	col = mix( col, vec3( 0.0, 0.5, 1.0), ((1.0 - e) - (1.0 - m))*0.2);

	return col;
}

float linearize(float depth)
{
	return -zfar * znear / (depth * (zfar - znear) - zfar);
}

float vignette()
{
	float dist = 0.0;//distance(TexCoord[3].st, vec2(0.5,0.5));
	dist = smoothstep(vignout + (fstop/vignfade), vignin + (fstop/vignfade), dist);
	return clamp( dist, 0.0, 1.0);
}

void main()
{
	//scene depth calculation

	float depth = linearize(texture(bgl_DepthTexture, TexCoord[0].st ).x);


	float fDepth = focalDepth;


	float blur = 0.0;


	float f = focalLength;		//focal length in mm
	float d = fDepth*1000.0;	//focal plane in mm
	float o = depth*1000.0;		//depth in mm

	float a = (o * f)/(o - f);
	float b = (d * f)/(d - f);
	float c = (d - f)/(d * fstop * CoC);

	blur = abs(a - b)*c;


	blur = clamp(blur, 0.0, 1.0);

	// calculation of pattern for ditering

	vec2 noise = rand(TexCoord[0].st)*namount*blur;

	// getting blur x and y step factor

	float w = (1.0/width)*blur*maxblur+noise.x;
	float h = (1.0/height)*blur*maxblur+noise.y;

	// calculation of final color

	vec3 col = vec3(0.0);

	col = texture(bgl_RenderedTexture, TexCoord[0].st).rgb;
	float s = 1.0;
	int ringsamples;

	for (int i = 1; i <= rings; i += 1)
	{
		ringsamples = i * samples;

		for (int j = 0 ; j < ringsamples ; j += 1)
		{
			float step = PI*2.0 / float(ringsamples);
			float pw = (cos(float(j)*step)*float(i));
			float ph = (sin(float(j)*step)*float(i));
			float p = 1.0;

			col += color(TexCoord[0].st + vec2(pw*w,ph*h), blur) * mix(1.0, (float(i))/(float(rings)), bias)*p;
			s += 1.0*mix(1.0, (float(i))/(float(rings)), bias)*p;
		}
	}
	col /= s; //divide by sample count


	if (showFocus)
	{
		col = debugFocus( col, blur, depth);
	}

	if (vignetting)
	{
		col *= vignette();
	}

	FragColor.rgb = col;
	FragColor.a = 1.0;

	//vec2 pos=gl_FragCoord.xy *texel;
	//vec2 pos = gl_TexCoord[0].st ;
	//vec4 bgl_FragColor = texture( bgl_RenderedTexture, pos);
	//gl_FragColor = vec4( bgl_FragColor[0], bgl_FragColor[1], bgl_FragColor[2], bgl_FragColor[3]);
}
