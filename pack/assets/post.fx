// Post processing effect file
// most shader code taken from SweetFX 1.5

// -------------------- Settings -----------------------------------------------

// Set to 1 for ON or 0 for OFF
#define USE_BLOOM         1  // [0 or 1] Bloom : Makes bright lights bleed their light into their surroundings (relatively high performance cost)
#define USE_HDR           1  // [0 or 1] HDR : Not actual HDR - It just tries to mimic an HDR look (relatively high performance cost)
#define USE_LUMASHARPEN   1  // [0 or 1] LumaSharpen
#define USE_VIBRANCE      1  // [0 or 1] Vibrance : Intelligently saturates (or desaturates if you use negative values) the pixels depending on their original saturation.

// Bloom settings
#define BloomThreshold 20.25 // [0.00 to 50.00] Threshold for what is a bright light (that causes bloom) and what isn't.
#define BloomPower 1.600     // [0.00 to  8.00] Strength of the bloom
#define BloomWidth 0.035     // [0.00 to  1.00] Width of the bloom

// HDR settings
#define HDRPower 1.15        // [0.00 to 8.00] Strangely lowering this makes the image brighter
#define radius2  0.80        // [0.00 to 8.00] Raising this seems to make the effect stronger and also brighter

// LumaSharpen settings
#define sharp_strength 0.35   // [0.10 to 3.00] Strength of the sharpening
#define sharp_clamp    0.035  // [0.00 to 1.00] Limits maximum amount of sharpening a pixel recieves - Default is 0.035
#define pattern 2             // [1|2|3|4] Choose a sample pattern. 1 = Fast, 2 = Normal, 3 = Wider, 4 = Pyramid shaped.

// Vibrance settings
#define Vibrance     0.08     // [-1.00 to 1.00] Intelligently saturates (or desaturates if you use negative values) the pixels depending on their original saturation.
#define Vibrance_RGB_balance float3(1.00, 1.00, 1.00) // [-10.00 to 10.00,-10.00 to 10.00,-10.00 to 10.00] A per channel multiplier to the Vibrance strength so you can give more boost to certain colors over others

// -------------------- Interface -----------------------------------------------

texture2D thisframeTex;

static float2 rcpres = PIXEL_SIZE;

#ifndef USE_SRGB
#define USE_SRGB true
#endif

sampler s0 = sampler_state
{
	texture = <thisframeTex>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	SRGBTexture = USE_SRGB;
};

struct VSOUT
{
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN
{
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

// -------------------- Effects -----------------------------------------------

#include "LumaSharpen.h"

// -------------------- Bloom -----------------------------------------------

float4 BloomPass(float4 ColorInput2, float2 Tex)
{
	float3 BlurColor2 = 0;
	float3 Blurtemp = 0;
	float MaxDistance = 8*BloomWidth;
	float CurDistance = 0;
	
	float Samplecount = 25.0;
	
	float2 blurtempvalue = Tex * rcpres * BloomWidth;
	
	float2 BloomSample = float2(2.5,-2.5);
	float2 BloomSampleValue;
	
	for(BloomSample.x = (2.5); BloomSample.x > -2.0; BloomSample.x = BloomSample.x - 1.0) // runs 5 times
	{
        BloomSampleValue.x = BloomSample.x * blurtempvalue.x;
        float2 distancetemp = BloomSample.x * BloomSample.x * BloomWidth;
        
		for(BloomSample.y = (- 2.5); BloomSample.y < 2.0; BloomSample.y = BloomSample.y + 1.0) // runs 5 ( * 5) times
		{
            distancetemp.y = BloomSample.y * BloomSample.y;
			CurDistance = (distancetemp.y * BloomWidth) + distancetemp.x;
			
			BloomSampleValue.y = BloomSample.y * blurtempvalue.y;
			Blurtemp.rgb = tex2D(s0, float2(Tex + BloomSampleValue)).rgb;

			BlurColor2.rgb += lerp(Blurtemp.rgb,ColorInput2.rgb, sqrt(CurDistance / MaxDistance));
		}
	}
	BlurColor2.rgb = (BlurColor2.rgb / (Samplecount - (BloomPower - BloomThreshold*5)));
	float Bloomamount = (dot(ColorInput2.rgb,float3(0.299f, 0.587f, 0.114f)));
	float3 BlurColor = BlurColor2.rgb * (BloomPower + 4.0);

	ColorInput2.rgb = lerp(ColorInput2.rgb,BlurColor.rgb, Bloomamount);	
	return saturate(ColorInput2);
}

// -------------------- HDR -----------------------------------------------

float4 HDRPass(float4 colorInput, float2 Tex)
{
	float3 c_center = tex2D(s0, Tex).rgb;
	
	float radius1 = 0.793;
	float3 bloom_sum1 = tex2D(s0, Tex + float2(1.5, -1.5) * radius1).rgb;
	bloom_sum1 += tex2D(s0, Tex + float2(-1.5, -1.5) * radius1).rgb;
	bloom_sum1 += tex2D(s0, Tex + float2(1.5, 1.5) * radius1).rgb;
	bloom_sum1 += tex2D(s0, Tex + float2(-1.5, 1.5) * radius1).rgb;
	
	bloom_sum1 += tex2D(s0, Tex + float2(0, -2.5) * radius1).rgb;
	bloom_sum1 += tex2D(s0, Tex + float2(0, 2.5) * radius1).rgb;
	bloom_sum1 += tex2D(s0, Tex + float2(-2.5, 0) * radius1).rgb;
	bloom_sum1 += tex2D(s0, Tex + float2(2.5, 0) * radius1).rgb;
	
	bloom_sum1 *= 0.005;
	
	float3 bloom_sum2 = tex2D(s0, Tex + float2(1.5, -1.5) * radius2).rgb;
	bloom_sum2 += tex2D(s0, Tex + float2(-1.5, -1.5) * radius2).rgb;
	bloom_sum2 += tex2D(s0, Tex + float2(1.5, 1.5) * radius2).rgb;
	bloom_sum2 += tex2D(s0, Tex + float2(-1.5, 1.5) * radius2).rgb;


	bloom_sum2 += tex2D(s0, Tex + float2(0, -2.5) * radius2).rgb;	
	bloom_sum2 += tex2D(s0, Tex + float2(0, 2.5) * radius2).rgb;
	bloom_sum2 += tex2D(s0, Tex + float2(-2.5, 0) * radius2).rgb;
	bloom_sum2 += tex2D(s0, Tex + float2(2.5, 0) * radius2).rgb;

	bloom_sum2 *= 0.010;
	
	float dist = radius2 - radius1;
	
	float3 HDR = (c_center + (bloom_sum2 - bloom_sum1)) * dist;
	float3 blend = HDR + colorInput.rgb;
	colorInput.rgb = HDR + pow(blend, HDRPower); // pow - don't use fractions for HDRpower
	
	return saturate(colorInput);
}

// -------------------- Vibrance -----------------------------------------------

float4 VibrancePass(float4 colorInput)
{  
	#define Vibrance_coeff float3(Vibrance_RGB_balance * Vibrance)

	float4 color = colorInput; //original input color
	float3 lumCoeff = float3(0.212656, 0.715158, 0.072186);  //Values to calculate luma with

	float luma = dot(lumCoeff, color.rgb); //calculate luma (grey)

	float max_color = max(colorInput.r, max(colorInput.g,colorInput.b)); //Find the strongest color
	float min_color = min(colorInput.r, min(colorInput.g,colorInput.b)); //Find the weakest color

	float color_saturation = max_color - min_color; //The difference between the two is the saturation

	color.rgb = lerp(luma, color.rgb, (1.0 + (Vibrance_coeff * (1.0 - (sign(Vibrance_coeff) * color_saturation)))));
	return color;
}

// -------------------- Main -----------------------------------------------

float4 postProcessing(VSOUT IN) : COLOR0
{
	float2 tex = IN.UVCoord;
	float4 c0 = tex2D(s0, tex);

#if (USE_BLOOM == 1)
	c0 = BloomPass(c0, tex);
#endif
	
#if (USE_HDR == 1)
	c0 = HDRPass(c0, tex);
#endif

#if (USE_LUMASHARPEN == 1)
	c0 = LumaSharpenPass(c0, tex);
#endif

#if (USE_VIBRANCE == 1)
	c0 = VibrancePass(c0);
#endif

	c0.w = 1.0;
	return saturate(c0);
}

technique t0
{
	pass P0
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 postProcessing();
		ZEnable = false;        
		SRGBWriteEnable = USE_SRGB;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}
}