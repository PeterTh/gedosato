/*
    -Volumetric SSAO-
	Implemented by Tomerk for OBGE
	Adapted and tweaked for Dark Souls by Durante
	Modified by Asmodean, for accurate occlusion distance, and positional depth offsets, for Dark Souls. Updated & tuned for Dark Souls II.
	Modified further for inclusion in DSfix 2.2 (depth-dependent thickness model) and adapted for Dark Souls 2 by Durante. Will it ever stop?
*/

//#[User Options]
#define LUMINANCE_CONSIDERATION 1 			//[0 or 1] Culls occlusion based on scene luminance. This can help with haloing/noise.
#define SamplingType 1						//[1|2|3] The type of sampling to use. 1: highest quality, 2: higher performance version of 1, & 3: lightweight sampling, similar to NV's.
extern float aoRadiusMultiplier = 0.2;		//[0.05 to 1.00] Linearly multiplies the radius of the AO Sampling. Affects the density of occlusion.
extern float ThicknessModel = 5.0; 			//[1.0 to 20.0] Units in space the AO assumes objects' thicknesses are. Can increase coverage, but also introduce haloing.
extern float FOV = 70.0; 					//[10.00 to 100.00] Field of View in Degrees. Visually affects density of occlusion within the FOV.
extern float aoClamp = 0.35;				//[0.00 to 1.00] Clamps occlusion blacks to a specified minimum. Keeps occluded areas looking like shadows, rather than pure black.
extern float luminosityThreshold = 0.50; 	//[0.00 to 1.00] This affects the degree of occlusion reduction based on luminance, when LUMINANCE_CONSIDERATION is enabled.

#ifndef SCALE
#define SCALE 1.0
#endif

#ifndef SSAO_STRENGTH_LOW
#ifndef SSAO_STRENGTH_MEDIUM
#ifndef SSAO_STRENGTH_HIGH
#define SSAO_STRENGTH_MEDIUM 1
#endif
#endif
#endif

#ifdef SSAO_STRENGTH_LOW
extern float aoStrengthMultiplier = 0.75;
#endif

#ifdef SSAO_STRENGTH_MEDIUM
extern float aoStrengthMultiplier = 0.90;
#endif

#ifdef SSAO_STRENGTH_HIGH
extern float aoStrengthMultiplier = 1.20;
#endif

//#End Of User Options

static float2 rcpres = float2(PIXEL_SIZE.x, PIXEL_SIZE.y);
static float aspect = rcpres.x / rcpres.y;
static const float nearZ = 1.0;
static const float farZ = 3600.0;
static const float2 g_InvFocalLen = { tan(0.5 * radians(FOV)) / rcpres.y * rcpres.x, tan(0.5 * radians(FOV)) };
static const float depthRange = nearZ-farZ;

Texture2D depthTex2D;
SamplerState depthSampler
{
	Texture = <depthTex2D>;
	Filter = Anisotropic;
	AddressU = Clamp; AddressV = Clamp;
	MaxAnisotropy = 16;
};

Texture2D frameTex2D;
SamplerState frameSampler
{
	Texture = <frameTex2D>;
	Filter = Anisotropic;
	AddressU = Clamp; AddressV = Clamp;
	MaxAnisotropy = 16;
};

Texture2D prevPassTex2D;
SamplerState passSampler
{
	Texture = <prevPassTex2D>;
	Filter = Anisotropic;
	AddressU = Clamp; AddressV = Clamp;
	MaxAnisotropy = 16;
};

cbuffer ConstBuffer : register(b0)
{
	matrix World : WORLD;
	matrix View : VIEW;
	matrix Projection : PROJECTION;
	static const float GammaConst = 2.2;
};

struct VSIN
{
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float Depth : DEPTH;
};

struct VSOUT
{
	float4 vertPos : SV_POSITION;
	float2 UVCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float Depth : SV_Depth;
};

VSOUT FrameVS(VSIN IN)
{	
	VSOUT OUT;
	
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	OUT.Depth = IN.Depth;
	OUT.Normal = mul(IN.Normal, (float3x3)World);
	OUT.Normal = normalize(OUT.Normal);
	
	return OUT;
}
#if (SamplingType == 1)
#define N_SAMPLES 16
static float2 sample_offset[N_SAMPLES] =
{
	float2(1.0,1.0), float2(-1.0,-1.0),
	float2(-1.0,1.0), float2(1.0,-1.0),
	float2(1.0,0.0), float2(-1.0,0.0),
	float2(0.0,1.0), float2(0.0,-1.0),
	float2(1.41,0.0), float2(-1.41,0.0),
	float2(0.0,1.41), float2(0.0,-1.41),
	float2(1.41,1.41), float2(-1.41,-1.41),
	float2(-1.41,1.41), float2(1.41,-1.41)
};

static float sample_radius[N_SAMPLES] =
{
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20
};
#elif (SamplingType == 2)
#define N_SAMPLES 16
static float2 sample_offset[N_SAMPLES] =
{
	 float2(1.00f,1.00f), float2(-1.00f,-1.00f),
	 float2(-1.00f,1.00f), float2(1.00f,-1.00f),
	 float2(1.00f,0.00f), float2(-1.00f,0.00f),
	 float2(0.00f,1.00f), float2(0.00f,-1.00f),
	 float2(1.00f,0.00f), float2(-1.00f,0.00f),
	 float2(0.00f,1.00f), float2(0.00f,-1.00f),
	 float2(1.00f,1.00f), float2(-1.00f,-1.00f),
	 float2(-1.00f,1.00f), float2(1.00f,-1.00f)
};

static float sample_radius[N_SAMPLES] =
{
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20
};
#elif (SamplingType >= 3)
#define N_SAMPLES 26
static float2 sample_offset[N_SAMPLES] =
{
	float2(0.2196607,0.9032637),
	float2(0.05916681,0.2201506),
	float2(-0.4152246,0.1320857),
	float2(-0.3790807,0.1454145),
	float2(0.3149606,-0.1294581),
	float2(-0.1108412,0.2162839),
	float2(0.658012,-0.4395972),
	float2(0.5377914,0.3112189),
	float2(-0.2752537,0.07625949),
	float2(-0.1915639,-0.4973421),
	float2(-0.2634767,0.5277923),
	float2(0.8242752,0.02434147),
	float2(0.06262707,-0.2128643),
	float2(-0.1795662,-0.3543862),
	float2(0.06039629,0.24629),
	float2(-0.7786345,-0.3814852),
	float2(0.2792919,0.2487278),
	float2(0.1841383,0.1696993),
	float2(-0.3479781,0.4725766),
	float2(-0.1365018,-0.2513416),
	float2(0.1280388,-0.563242),
	float2(-0.4800232,-0.1899473),
	float2(0.6389147,0.1191014),
	float2(0.1932822,-0.3692099),
	float2(-0.3465451,-0.1654651),
	float2(0.2448421,-0.1610962)
};

static float sample_radius[N_SAMPLES] =
{
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20, 0.20, 0.20,
	0.20, 0.20
};
#endif

float RGBLuminance(float3 color)
{
	const float3 lumCoeff = float3(0.2126729, 0.7151522, 0.0721750);
	return dot(color.rgb, lumCoeff);
}

float3 RGBGammaToLinear(float3 color, float gamma)
{
	color = abs(color);

	color.r = (color.r <= 0.0) ? saturate(color.r / 12.92) :
	saturate(pow((color.r + 0.055) / 1.055, gamma));
	color.g = (color.g <= 0.0) ? saturate(color.g / 12.92) :
	saturate(pow((color.g + 0.055) / 1.055, gamma));
	color.b = (color.b <= 0.0) ? saturate(color.b / 12.92) :
	saturate(pow((color.b + 0.055) / 1.055, gamma));

	return color;
}

float3 LinearToRGBGamma(float3 color, float gamma)
{
	color = abs(color);

	color.r = (color.r <= 0.0) ? saturate(color.r * 12.92) : 1.055 *
	saturate(pow(color.r, 1.0 / gamma)) - 0.055;
	color.g = (color.g <= 0.0) ? saturate(color.g * 12.92) : 1.055 *
	saturate(pow(color.g, 1.0 / gamma)) - 0.055;
	color.b = (color.b <= 0.0) ? saturate(color.b * 12.92) : 1.055 *
	saturate(pow(color.b, 1.0 / gamma)) - 0.055;

	return color;
}

float2 rand(in float2 uv : TEXCOORD0)
{
	float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
	float noiseY = sqrt(1.0 - noiseX * noiseX);

	return float2(noiseX, noiseY);
}

float readDepth(in float2 coord : TEXCOORD0)
{
	float4 col = tex2D(depthSampler, coord);
	float posZ = ((1.0-col.x) + (1.0-col.y) * 255.0 + (1.0-col.z) * (255.0 * 255.0));

	return sqrt(posZ / (5.0*(256.0*256.0)));
}

float3 getPosition(in float2 uv : TEXCOORD0, in float eye_z : SV_Depth)
{
   uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));
   float3 pos = float3(uv * g_InvFocalLen * eye_z, eye_z);

   return pos;
}

float4 ssao_Main(VSOUT IN) : COLOR0
{
	clip(1/SCALE - IN.UVCoord.x);
	clip(1/SCALE - IN.UVCoord.y);
	IN.UVCoord.xy *= SCALE;
	
	float2 d2 = readDepth(IN.UVCoord);
	if(d2.y < 0.03) return (float4)1.0;
	IN.Depth = d2.x;

	float3 pos = getPosition(IN.UVCoord, IN.Depth);
	float3 dx = ddx(pos);
	float3 dy = ddy(pos);

	float3 normal = normalize(cross(dx, dy));  normal.y *= -1.0;

	float2 rand_vec = rand(IN.UVCoord);
	float2 sample_vec_divisor = g_InvFocalLen * IN.Depth * depthRange / (aoRadiusMultiplier * 5000.0 * rcpres);
	float2 sample_center = IN.UVCoord + normal.xy / sample_vec_divisor * float2(1.0, aspect);
	float sample_center_depth = IN.Depth * depthRange + normal.z * aoRadiusMultiplier * 8.0;
	
	float ao = 0.0;
	float s = 0.0;

	for(int i = 0; i < N_SAMPLES; i++)
	{	
		float2 sample_vec = reflect(sample_offset[i], rand_vec); sample_vec /= sample_vec_divisor;
		float2 sample_coords = sample_center + sample_vec * float2(1.0, aspect);

		float curr_sample_radius = sample_radius[i] * aoRadiusMultiplier * 8.0;
		float curr_sample_depth = depthRange * readDepth(sample_coords);

		ao += clamp(0.0, curr_sample_radius + sample_center_depth - curr_sample_depth, 2.0 * curr_sample_radius);
		ao -= clamp(0.0, curr_sample_radius + sample_center_depth - curr_sample_depth - ThicknessModel, 2.0 * curr_sample_radius);

		s += 2.0 * curr_sample_radius;
	}

	ao /= s;
	
	//Adjust for close and far away
	if(IN.Depth < 0.065)
	{
		ao = lerp(ao, 0.0, (0.065 - IN.Depth) * 15.0);
	}

	ao = (1.0 - ao * aoStrengthMultiplier);
	ao = clamp(ao, aoClamp, 1.0);

	return float4(ao, ao, ao, 1.0);
}

float4 HBlur(VSOUT IN) : COLOR0
{
	float color = tex2D(passSampler, IN.UVCoord).r;
	float blurred = color * 0.2270270270;

	blurred += tex2D(passSampler, IN.UVCoord + float2(rcpres.x * 1.3846153846, 0.0)).r * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord - float2(rcpres.x * 1.3846153846, 0.0)).r * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord + float2(rcpres.x * 3.2307692308, 0.0)).r * 0.0702702703;
	blurred += tex2D(passSampler, IN.UVCoord - float2(rcpres.x * 3.2307692308, 0.0)).r * 0.0702702703;
	
	return blurred;
}

float4 VBlur(VSOUT IN) : COLOR0
{
	float color = tex2D(passSampler, IN.UVCoord).r;
	float blurred = color * 0.2270270270;

	blurred += tex2D(passSampler, IN.UVCoord + float2(0.0, rcpres.y * 1.3846153846)).r * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord - float2(0.0, rcpres.y * 1.3846153846)).r * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord + float2(0.0, rcpres.y * 3.2307692308)).r * 0.0702702703;
	blurred += tex2D(passSampler, IN.UVCoord - float2(0.0, rcpres.y * 3.2307692308)).r * 0.0702702703;
	
	return blurred;
}

float4 Combine(VSOUT IN) : COLOR0 
{
	float4 color = tex2D(frameSampler, IN.UVCoord);
	float ao = tex2D(passSampler, IN.UVCoord/SCALE).r;

	#if (LUMINANCE_CONSIDERATION == 1)
	float luminance = RGBLuminance(color.rgb);
	float white = 1.0;
	float black = 0.0;

	luminance = clamp(
	max(black, luminance - luminosityThreshold) +
	max(black, luminance - luminosityThreshold) +
	max(black, luminance - luminosityThreshold), 0.0, 1.0);
	ao = lerp(ao, white, luminance);
	#endif
	
	color.rgb = RGBGammaToLinear(color.rgb, GammaConst);
	color.rgb *= ao;
	color.rgb = LinearToRGBGamma(color.rgb, GammaConst);
	
	return saturate(color);
}

technique t0
{
	pass p0
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ssao_Main();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		StencilEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}
	pass p1
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 HBlur();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		StencilEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}
	pass p2
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 VBlur();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		StencilEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}
	pass p3
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Combine();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		StencilEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}
}