// Debanding effect file

// -------------------- Settings -----------------------------------------------

// Set to 1 for ON or 0 for OFF
#define USE_DEBAND 1 //[0 or 1] // Applies debanding to minimize banding artifacts

// Deband settings
	#define DEBAND_RADIUS 32.0     //[0.0:1024.0]  Sampling radius, higher values will reduce further banding but might also reduce details
	#define DEBAND_THRESHOLD 0.017 //[0.000:0.100] Threshold, higher values will reduce further banding but might also reduce details and increase noise
	#define DEBAND_SAMPLE_COUNT 4  //[1:8] Sample count, higher values are better
	#define DEBAND_OFFSET_MODE 3   //[1:3] 1 = cross (axis aligned, fast), 2 = diagonal (45 degrees, slower), 3 = box (fully random, much slower)
	#define DEBAND_DITHERING 1     //[0:3] Additional dithering options to smoothen the output. 0 = No dithering 1 = Ordered dithering, 2 = Random dithering, 3 = Iestyn's RGB dither (Valve)

	//>Deband debug settings<\\
	#define DEBAND_SKIP_THRESHOLD_TEST 0 //[0:1] 1 = Skip threshold to see the unfiltered sampling pattern
	#define DEBAND_OUTPUT_BOOST 1.0      //[-2.0:2.0] Default = 1.0. Any value other than the default activates debug mode. When fine-tuning the values you might use both these settings to boost luminance, which should make it easier to see banding artifacts.
	#define DEBAND_OUTPUT_OFFSET 0.0     //[-1.0:3.0] Default = 0.0. Any value other than the default activates debug mode. When fine-tuning the values you might use both these settings to boost luminance, which should make it easier to see banding artifacts.
    
// -------------------- Interface -----------------------------------------------

texture2D thisframeTex;

static float2 rcpres = PIXEL_SIZE;

const float timer;

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

// ------------------------- Deband -------------------------------------------
float rand(float2 pos) {
	return frac(sin(dot(pos, float2(12.9898, 78.233))) * 43758.5453);
}

bool is_within_threshold(float3 original, float3 other) {
	return !any(max(abs(original - other) - DEBAND_THRESHOLD, float3(0.0, 0.0, 0.0))).x;
}

float4 DebandPass(float2 texcoord) {
	float2 step = PIXEL_SIZE * DEBAND_RADIUS;
	float2 halfstep = step * 0.5;

	// Compute additional sample positions
	float2 seed = texcoord + timer;
	#if  (DEBAND_OFFSET_MODE == 1)
	float2 offset = float2(rand(seed), 0.0);
	#elif(DEBAND_OFFSET_MODE == 2)
	float2 offset = float2(rand(seed).xx);
	#elif(DEBAND_OFFSET_MODE == 3)
	float2 offset = float2(rand(seed), rand(seed + float2(0.1, 0.2)));
	#endif

	float2 on[8] = {
		float2( offset.x,  offset.y) * step,
		float2( offset.y, -offset.x) * step,
		float2(-offset.x, -offset.y) * step,
		float2(-offset.y,  offset.x) * step,
		float2( offset.x,  offset.y) * halfstep,
		float2( offset.y, -offset.x) * halfstep,
		float2(-offset.x, -offset.y) * halfstep,
		float2(-offset.y,  offset.x) * halfstep,
	};

	float3 col0 = tex2D(s0, texcoord).rgb;
	float4 accu = float4(col0, 1.0);

	for(int i=0; i<DEBAND_SAMPLE_COUNT; i++) {
		float4 cn = float4(tex2D(s0, texcoord + on[i]).rgb, 1.0);
		#if (DEBAND_SKIP_THRESHOLD_TEST == 0)
		if(is_within_threshold(col0, cn.rgb))
		#endif
			accu += cn;
	}

	accu.rgb /= accu.a;

	// Boost to make it easier to inspect the effect's output
	if(DEBAND_OUTPUT_OFFSET != 0.0 || DEBAND_OUTPUT_BOOST != 1.0) {
		accu.rgb -= DEBAND_OUTPUT_OFFSET;
		accu.rgb *= DEBAND_OUTPUT_BOOST;
	}

	// Additional dithering
	#if   (DEBAND_DITHERING == 1)
		//Ordered dithering
		float dither_bit  = 8.0;
		float grid_position = frac( dot(texcoord,(SCREEN_SIZE * float2(1.0/16.0,10.0/36.0))) + 0.25 );
		float dither_shift = (0.25) * (1.0 / (pow(2,dither_bit) - 1.0));
		float3 dither_shift_RGB = float3(dither_shift, -dither_shift, dither_shift);
		dither_shift_RGB = lerp(2.0 * dither_shift_RGB, -2.0 * dither_shift_RGB, grid_position);
		accu.rgb += dither_shift_RGB;
	#elif (DEBAND_DITHERING == 2)
		//Random dithering
		float dither_bit  = 8.0;
		float sine = sin(dot(texcoord, float2(12.9898,78.233)));
		float noise = frac(sine * 43758.5453 + texcoord.x);
		float dither_shift = (1.0 / (pow(2,dither_bit) - 1.0));
		float dither_shift_half = (dither_shift * 0.5);
		dither_shift = dither_shift * noise - dither_shift_half;
		accu.rgb += float3(-dither_shift, dither_shift, -dither_shift);
	#elif (DEBAND_DITHERING == 3)
		float3 vDither = dot(float2(171.0, 231.0), texcoord * SCREEN_SIZE).xxx;
		vDither.rgb = frac( vDither.rgb / float3( 103.0, 71.0, 97.0 ) ) - float3(0.5, 0.5, 0.5);
		accu.rgb += (vDither.rgb / 255.0);
	#endif

	return accu;
}

// -------------------- Main -----------------------------------------------

float4 postProcessing(VSOUT IN) : COLOR0
{
    float2 tex = IN.UVCoord;
    float4 c0 = tex2D(s0, tex);
    
#if (USE_DEBAND == 1)
    c0 = DebandPass(tex);
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