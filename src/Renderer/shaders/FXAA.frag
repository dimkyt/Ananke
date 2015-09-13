#version 330 core

#define FXAA_EDGE_THRESHOLD			1.0/16.0
#define FXAA_EDGE_THRESHOLD_MIN		1.0/16.0

#define FXAA_SUBPIX_TRIM			1.0/8.0
#define FXAA_SUBPIX_TRIM_SCALE		(1.0/(1.0 - FXAA_SUBPIX_TRIM))
#define FXAA_SUBPIX_CAP				7.0/8.0

#define FXAA_SEARCH_ACCELERATION	1
#define FXAA_SEARCH_STEPS			12*FXAA_SEARCH_ACCELERATION
#define FXAA_SEARCH_THRESHOLD		1.0/4.0

in vec2 UV;

out vec4 color;

uniform vec2 pixel_size;
uniform sampler2D tex_color;

float FxaaLuma(vec3 rgb)
{
	return rgb.y * (0.587/0.299) + rgb.x;
}

void main()
{
	vec3 rgbN = texture(tex_color, UV + pixel_size*vec2( 0,1)).xyz;
	vec3 rgbW = texture(tex_color, UV + pixel_size*vec2(-1, 0)).xyz;
	vec3 rgbM = texture(tex_color, UV).xyz;
	vec3 rgbE = texture(tex_color, UV + pixel_size*vec2( 1, 0)).xyz;
	vec3 rgbS = texture(tex_color, UV + pixel_size*vec2( 0, -1)).xyz;
	float lumaN = FxaaLuma(rgbN);
	float lumaW = FxaaLuma(rgbW);
	float lumaM = FxaaLuma(rgbM);
	float lumaE = FxaaLuma(rgbE);
	float lumaS = FxaaLuma(rgbS);
	float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
	float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
	float range = rangeMax - rangeMin;

	if(range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD))
	{
		color = vec4(rgbM, 1);
		return;
	}

	//Sub-pixel aliasing test
	float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;
	float rangeL = abs(lumaL - lumaM);
	float blendL = max(0.0,
	(rangeL / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE;
	blendL = min(FXAA_SUBPIX_CAP, blendL);

	vec3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;

	vec3 rgbNW = texture(tex_color, UV + pixel_size*vec2(-1,1)).xyz;
	vec3 rgbNE = texture(tex_color, UV + pixel_size*vec2( 1,1)).xyz;
	vec3 rgbSW = texture(tex_color, UV + pixel_size*vec2(-1, -1)).xyz;
	vec3 rgbSE = texture(tex_color, UV + pixel_size*vec2( 1, -1)).xyz;
	rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);
	rgbL = rgbL*(1.0/9.0);

	float lumaNW = FxaaLuma(rgbNW);
	float lumaNE = FxaaLuma(rgbNE);
	float lumaSW = FxaaLuma(rgbSW);
	float lumaSE = FxaaLuma(rgbSE);

	
	//Edge Test
	float edgeVert =
		abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) +
		abs((0.50 * lumaW ) + (-1.0 * lumaM) + (0.50 * lumaE )) +
		abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));
	float edgeHorz =
		abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) +
		abs((0.50 * lumaN ) + (-1.0 * lumaM) + (0.50 * lumaS )) +
		abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));
	vec2 dir = edgeHorz >= edgeVert ? vec2(1., .0) : vec2(.0, 1.);
	
	
	//End-of-Edge Test
	bool doneN = false;
	bool doneP = false;
	vec2 offNP = FXAA_SEARCH_ACCELERATION*pixel_size*dir;
	vec2 posN = offNP;
	vec2 posP = offNP;
	float lumaEndN = 0.0;
	float lumaEndP = 0.0;
	lumaN = dir==vec2(.0, 1.) ? lumaE : lumaN;
	lumaS = dir==vec2(.0, 1.) ? lumaW : lumaS;
	float gradientN = abs(lumaN - lumaM)*FXAA_SEARCH_THRESHOLD;
	float gradientS = abs(lumaS - lumaM)*FXAA_SEARCH_THRESHOLD;

	for(int i = 0; i < FXAA_SEARCH_STEPS; i++)
	{
		if(FXAA_SEARCH_ACCELERATION == 1)
		{
			if(!doneN) lumaEndN = FxaaLuma(texture(tex_color, UV-posN).xyz);
			if(!doneP) lumaEndP = FxaaLuma(texture(tex_color, UV+posP).xyz);
		}
		/*
		else
		{
			if(!doneN) lumaEndN = FxaaLuma(
				FxaaTextureGrad(tex, posN.xy, offNP).xyz);
			if(!doneP) lumaEndP = FxaaLuma(
				FxaaTextureGrad(tex, posP.xy, offNP).xyz);
		}
		*/
		doneN = (abs(lumaEndN - lumaS) >= gradientS) ? false : doneN;
		doneP = (abs(lumaEndP - lumaN) >= gradientN) ? false : doneP;
		if(doneN && doneP) break;
		if(!doneN) posN += offNP;
		if(!doneP) posP += offNP;
	}
	vec2 subPixelOff = posP - posN;
	vec2 offset = vec2(subPixelOff.y, subPixelOff.x);
	vec3 rgb = texture(tex_color, UV + offset).xyz;

	color = vec4(mix(rgb, rgbL, blendL), 1);
}