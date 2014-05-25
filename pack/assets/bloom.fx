/*
	HDR Bloom Effect
	by Durante
*/

// These will be set by GeDoSaTo ////////////////////////////////////////////////////////

// e.g. (1/1920, 1/1080, 1920, 1080) at input size 1080p
float4 inputPixelMetrics;
// 1/[number of steps in the pyramid]
float invSteps;

// Tweakable variables //////////////////////////////////////////////////////////////////

// values above this cutoff are considered to be highlights
#ifndef CUTOFF
#define CUTOFF 0.08
#endif

// linearly increases the strength of the bloom effect
#ifndef STRENGTH
#define STRENGTH 8.0
#endif

// strength of the "camera lens dirt" effect
#ifndef DIRT_STRENGTH
#define DIRT_STRENGTH 0.5
#endif

// to what degree non-HDR can be darkened by HDR eye adjustment
#ifndef MIX_FACTOR
#define MIX_FACTOR 0.7
#endif

// how much to progressively desaturate distant bloom
#ifndef SATURATION
#define SATURATION 0.95
#endif

// maximum brightness for HDR input - to prevent e.g. signs from blooming too much
#ifndef MAX_BRIGHTNESS
#define MAX_BRIGHTNESS float4(0.7,0.7,0.7,1.0)
#endif

// speed of eye adaption - larger number = quicker adaption
#ifndef ADAPT_SPEED
#define ADAPT_SPEED 0.015f
#endif

/////////////////////////////////////////////////////////////////////////////////////////

texture2D sampleTex; // original texture
texture2D passTex; // previous pass
texture2D avgTex; // 1x1 averaged bloom strength
texture2D dirtTex; // (static) dirt mask

sampler sampleSampler = sampler_state {
	Texture   = <sampleTex>;
	MinFilter = LINEAR; MagFilter = LINEAR; MipFilter   = LINEAR;
	AddressU  = Clamp;  AddressV  = Clamp;  SRGBTexture = TRUE;
};

sampler passSampler = sampler_state {
	texture   = <passTex>;
	MinFilter = LINEAR; MagFilter = LINEAR; MipFilter   = LINEAR;
	AddressU  = Clamp;  AddressV  = Clamp;  SRGBTexture = FALSE;
};

sampler dirtSampler = sampler_state {
	texture   = <dirtTex>;
	MinFilter = LINEAR; MagFilter = LINEAR; MipFilter   = LINEAR;
	AddressU  = Clamp;  AddressV  = Clamp;  SRGBTexture = FALSE;
};

sampler avgSampler = sampler_state {
	texture   = <avgTex>;
	MinFilter = LINEAR; MagFilter = LINEAR; MipFilter   = LINEAR;
	AddressU  = Clamp;  AddressV  = Clamp;  SRGBTexture = FALSE;
};

struct VSOUT {
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN {
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT;
	float4 pos = float4(IN.vertPos.x, IN.vertPos.y, IN.vertPos.z, 1.0f);
	OUT.vertPos = pos;
	float2 coord = float2(IN.UVCoord.x, IN.UVCoord.y);
	OUT.UVCoord = coord;
	return OUT;
}

float4 initialCutoffAndDownsampleShader(VSOUT IN) : COLOR0 {
	float4 cA = tex2D(sampleSampler, IN.UVCoord + float2(-inputPixelMetrics.x, -inputPixelMetrics.y)*0.5);
	float4 cB = tex2D(sampleSampler, IN.UVCoord + float2( inputPixelMetrics.x, -inputPixelMetrics.y)*0.5);
	float4 cC = tex2D(sampleSampler, IN.UVCoord + float2( inputPixelMetrics.x,  inputPixelMetrics.y)*0.5);
	float4 cD = tex2D(sampleSampler, IN.UVCoord + float2(-inputPixelMetrics.x,  inputPixelMetrics.y)*0.5);
	float4 m = max(cA, max(cB, max(cC, cD)));
	float4 cut = clamp(m-CUTOFF, float4(0,0,0,0), MAX_BRIGHTNESS);
	float3 luminanceWeights = float3(0.299,0.587,0.114);
	float luminance = dot(cut.xyz, luminanceWeights);
	return m*luminance;
}

float4 HBlurShader(VSOUT IN) : COLOR0 {
	float4 color = tex2D(passSampler, IN.UVCoord);

	float4 blurred = color*0.2270270270;
	blurred += tex2D(passSampler, IN.UVCoord + float2(inputPixelMetrics.x*(1.3846153846), 0) + inputPixelMetrics.xy*0.55) * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord - float2(inputPixelMetrics.x*(1.3846153846), 0) + inputPixelMetrics.xy*0.55) * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord + float2(inputPixelMetrics.x*(3.2307692308), 0) + inputPixelMetrics.xy*0.55) * 0.0702702703;
	blurred += tex2D(passSampler, IN.UVCoord - float2(inputPixelMetrics.x*(3.2307692308), 0) + inputPixelMetrics.xy*0.55) * 0.0702702703;
	
	return blurred;
}

float4 VBlurShader(VSOUT IN) : COLOR0 {
	float4 color = tex2D(passSampler, IN.UVCoord);

	float4 blurred = color*0.2270270270;
	blurred += tex2D(passSampler, IN.UVCoord + float2(0, inputPixelMetrics.y*(1.3846153846)) + inputPixelMetrics.xy*0.55) * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord - float2(0, inputPixelMetrics.y*(1.3846153846)) + inputPixelMetrics.xy*0.55) * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord + float2(0, inputPixelMetrics.y*(3.2307692308)) + inputPixelMetrics.xy*0.55) * 0.0702702703;
	blurred += tex2D(passSampler, IN.UVCoord - float2(0, inputPixelMetrics.y*(3.2307692308)) + inputPixelMetrics.xy*0.55) * 0.0702702703;
	
	float3 luminanceWeights = float3(0.299,0.587,0.114);
	float luminance = dot(blurred.rgb, luminanceWeights);
	blurred = lerp(luminance, blurred, SATURATION);

	return blurred;
}

float4 integrateUpwardsShader(VSOUT IN) : COLOR0 {
	return float4(tex2D(passSampler, IN.UVCoord).rgb, 1.0);
}

float4 eyeAdaptionShader(VSOUT IN) : COLOR0 {
	float3 current = tex2D(avgSampler, float2(0.5,0.5)).rgb;
	return float4(current, ADAPT_SPEED);
}

float4 finalComposeShader(VSOUT IN) : COLOR0 {
	float4 cBloom = tex2D(passSampler, IN.UVCoord);
	float4 color = tex2D(sampleSampler, IN.UVCoord);
	float4 avg = tex2D(avgSampler, float2(0.5,0.5));
	float totalPower = avg.r+avg.g+avg.b;
	
	cBloom = cBloom * invSteps;
	
	float4 dirt = tex2D(dirtSampler, IN.UVCoord);
	cBloom = cBloom + cBloom*dirt*DIRT_STRENGTH;
	
	return MIX_FACTOR*color + ((1-MIX_FACTOR)*color+cBloom*STRENGTH)/(1.0+(totalPower*100));
}

technique initialCutoffAndDownsample {
	pass initialCutoffAndDownsample	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 initialCutoffAndDownsampleShader();
		ZEnable = false; AlphaBlendEnable = false; AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE;
	}
}

technique gaussian {
	pass horizontal {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 HBlurShader();
		ZEnable = false; AlphaBlendEnable = false; AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE;
	}
	pass vertical {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 VBlurShader();
		ZEnable = false; AlphaBlendEnable = false; AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE;
	}
}

technique integrateUpwards {
	pass integrate {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 integrateUpwardsShader();
		ZEnable = false; AlphaTestEnable = false;
		AlphaBlendEnable = true; 
		BlendOp = ADD;
		SrcBlend = ONE;
		DestBlend = ONE;
		ColorWriteEnable = RED|GREEN|BLUE;
	}
}

technique eyeAdaption {
	pass eyeAdaption {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 eyeAdaptionShader();
		ZEnable = false; AlphaTestEnable = false;
		AlphaBlendEnable = true; 
		BlendOp = ADD;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
		ColorWriteEnable = RED|GREEN|BLUE;
	}
}

technique finalCompose {
	pass finalCompose {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 finalComposeShader();
		ZEnable = false; AlphaBlendEnable = false; AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE;
        SRGBWriteEnable = true;
	}
}
