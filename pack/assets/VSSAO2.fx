/*
	 -Volumetric SSAO-
	Implemented by Tomerk for OBGE
	Adapted and tweaked for Dark Souls by Durante
	Modified by Asmodean, for accurate occlusion distance, and dynamic positional depth offsets, for Dark Souls
	Modified further for inclusion in DSfix 2.2 (depth-dependent thickness model) and adapted for Dark Souls 2 by Durante. Will it ever stop?
*/

/***User-controlled variables***/
#define N_SAMPLES 32 //number of samples, currently do not change.

extern float aoRadiusMultiplier = 0.2; //Linearly multiplies the radius of the AO Sampling
extern float ThicknessModel = 6; //units in space the AO assumes objects' thicknesses are
extern float FOV = 75; //Field of View in Degrees
extern float luminosity_threshold = 0.5;

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
extern float aoClamp = 0.6;
extern float aoStrengthMultiplier = 0.9;
#endif

#ifdef SSAO_STRENGTH_MEDIUM
extern float aoClamp = 0.3;
extern float aoStrengthMultiplier = 1.5;
#endif

#ifdef SSAO_STRENGTH_HIGH
extern float aoClamp = 0.1;
extern float aoStrengthMultiplier = 2.0;
#endif


#define LUMINANCE_CONSIDERATION //comment this line to not take pixel brightness into account

/***End Of User-controlled Variables***/
static float2 rcpres = PIXEL_SIZE;
static float aspect = rcpres.y/rcpres.x;
static const float nearZ = 1;
static const float farZ = 3500;
static const float2 g_InvFocalLen = { tan(0.5f*radians(FOV)) / rcpres.y * rcpres.x, tan(0.5f*radians(FOV)) };
static const float depthRange = nearZ-farZ;


texture2D sampleTex2D;
sampler sampleSampler = sampler_state
{
	Texture   = <sampleTex2D>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Clamp;
	AddressV  = Clamp;
	SRGBTexture=FALSE;
};

texture2D depthTex2D;
sampler depthSampler = sampler_state
{
	texture = <depthTex2D>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU  = Mirror;
	AddressV  = Mirror;
	SRGBTexture=FALSE;
};

texture2D AOTex2D;
sampler AOSampler = sampler_state
{
	texture = <AOTex2D>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU  = Clamp;
	AddressV  = Clamp;
	SRGBTexture=FALSE;
};

texture2D frameTex2D;
sampler frameSampler = sampler_state
{
	texture = <frameTex2D>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Clamp;
	AddressV  = Clamp;
	SRGBTexture=TRUE;
};

texture2D prevPassTex2D;
sampler passSampler = sampler_state
{
	texture = <prevPassTex2D>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Clamp;
	AddressV  = Clamp;
	SRGBTexture=FALSE;
};

struct VSOUT
{
	float4 vertPos : POSITION0;
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
	float4 pos=float4(IN.vertPos.x, IN.vertPos.y, IN.vertPos.z, 1.0f);
	OUT.vertPos=pos;
	float2 coord=float2(IN.UVCoord.x, IN.UVCoord.y);
	OUT.UVCoord=coord;
	return OUT;
}

static float2 sample_offset[N_SAMPLES] =
{
	 float2(1.00f, 1.00f),
	 float2(-1.00f, -1.00f),
	 float2(-1.00f, 1.00f),
	 float2(1.00f, -1.00f),

	 float2(1.00f, 0.00f),
	 float2(-1.00f, 0.00f),
	 float2(0.00f, 1.00f),
	 float2(0.00f, -1.00f),

	 float2(1.00f, 0.00f),
	 float2(-1.00f, 0.00f),
	 float2(0.00f, 1.00f),
	 float2(0.00f, -1.00f),

	 float2(1.00f, 1.00f),
	 float2(-1.00f, -1.00f),
	 float2(-1.00f, 1.00f),
	 float2(1.00f, -1.00f),
	 
	 float2(1.00f, 0.00f),
	 float2(-1.00f, 0.00f),
	 float2(0.00f, 1.00f),
	 float2(0.00f, -1.00f),

	 float2(1.00f, 0.00f),
	 float2(-1.00f, 0.00f),
	 float2(0.00f, 1.00f),
	 float2(0.00f, -1.00f),

	 float2(1.00f, 1.00f),
	 float2(-1.00f, -1.00f),
	 float2(-1.00f, 1.00f),
	 float2(1.00f, -1.00f),
	 
	 float2(1.00f, 1.00f),
	 float2(-1.00f, -1.00f),
	 float2(-1.00f, 1.00f),
	 float2(1.00f, -1.00f)
};

static float sample_radius[N_SAMPLES] =
{
	0.20f, 0.20f,
	0.20f, 0.20f,
	0.20f, 0.20f,
	0.20f, 0.20f,
	
	0.20f, 0.20f,
	0.20f, 0.20f,
	0.20f, 0.20f,
	0.20f, 0.20f,
	
	0.20f, 0.20f,
	0.20f, 0.20f,
	0.20f, 0.20f,
	0.20f, 0.20f,
	
	0.20f, 0.20f,
	0.20f, 0.20f,
	0.20f, 0.20f,
	0.20f, 0.20f
};

float2 rand(in float2 uv : TEXCOORD0) {
	float noiseX = (frac(sin(dot(uv, float2(12.9898,78.233)*2.0)) * 43758.5453));
	float noiseY = sqrt(1-noiseX*noiseX);
	return float2(noiseX, noiseY);
}

float2 readDepth(in float2 coord : TEXCOORD0)
{
	float4 col = tex2D(depthSampler, coord);
	float posZ = ((1-col.x) + (1-col.y)*255.0 + (1-col.z)*(255.0*255.0));
	return float2(pow(posZ / (5*256.0*256.0) + 1.0, 8.0)-1.0, col.w);
}

float3 getPosition(in float2 uv : TEXCOORD0, in float eye_z : POSITION0) {
   uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));
   float3 pos = float3(uv * g_InvFocalLen * eye_z, eye_z );
   return pos;
}

float4 ssao_Main(VSOUT IN) : COLOR0
{
	clip(1/SCALE-IN.UVCoord.x);
	clip(1/SCALE-IN.UVCoord.y);	
	IN.UVCoord.xy *= SCALE;

	float2 d2 = readDepth(IN.UVCoord);
	if(d2.y < 0.1) return float4(1.0,1.0,1.0,1.0);
	float depth = d2.x;
	float3 pos = getPosition(IN.UVCoord, depth);
	float3 dx = ddx(pos);
	float3 dy = ddy(pos);
	float3 norm = normalize(cross(dx,dy));
	norm.y *= -1;

	float sample_depth;

	float ao=tex2D(AOSampler, IN.UVCoord);
	float s=tex2D(sampleSampler, IN.UVCoord);

	float2 rand_vec = rand(IN.UVCoord);
	float2 sample_vec_divisor = g_InvFocalLen*depth*depthRange/(aoRadiusMultiplier*5000*rcpres);
	float2 sample_center = IN.UVCoord + norm.xy/sample_vec_divisor*float2(1.0f,aspect);
	float sample_center_depth = depth*depthRange + norm.z*aoRadiusMultiplier*7;
	
	for(int i = 0; i < N_SAMPLES; i++)
	{
		float2 sample_vec = reflect(sample_offset[i], rand_vec);
		sample_vec /= sample_vec_divisor;
		float2 sample_coords = sample_center + sample_vec*float2(1.0f,aspect);
		
		float curr_sample_radius = sample_radius[i]*aoRadiusMultiplier*10;
		float curr_sample_depth = depthRange*readDepth(sample_coords);
		
		ao += clamp(0,curr_sample_radius+sample_center_depth-curr_sample_depth,2*curr_sample_radius);
		ao -= clamp(0,curr_sample_radius+sample_center_depth-curr_sample_depth-ThicknessModel,2*curr_sample_radius);
		s += 2.0*curr_sample_radius;
	}

	ao /= s;
	
	// adjust for close and far away
	if(depth<0.065f) ao = lerp(ao, 0.0f, (0.065f-depth)*13.3);

	ao = 1.0f-ao*aoStrengthMultiplier;
	
	return float4(ao,ao,ao,depth);
}

#ifdef BLUR_SHARP
float4 HBlur( VSOUT IN ) : COLOR0 {
	float4 sample = tex2D(passSampler, IN.UVCoord);
	float blurred = sample.r*2;
	float depth = sample.a;
	float divide = 2.0;

	float4 left = tex2D(passSampler, IN.UVCoord - float2(rcpres.x, 0));
	float lpower = saturate(ThicknessModel - abs(left.a - depth));
	blurred += lpower*left.r;
	divide += lpower;

	float4 right = tex2D(passSampler, IN.UVCoord + float2(rcpres.x, 0));
	float rpower = saturate(ThicknessModel - abs(right.a - depth));
	blurred += rpower*left.r;
	divide += rpower;

	return blurred/divide;
}
float4 VBlur( VSOUT IN ) : COLOR0 {
	float4 sample = tex2D(passSampler, IN.UVCoord);
	float blurred = sample.r*2;
	float depth = sample.a;
	float divide = 2.0;

	float4 top = tex2D(passSampler, IN.UVCoord - float2(0, rcpres.y));
	float tpower = saturate(ThicknessModel - abs(top.a - depth));
	blurred += tpower*top.r;
	divide += tpower;

	float4 bottom = tex2D(passSampler, IN.UVCoord + float2(0, rcpres.y));
	float bpower = saturate(ThicknessModel - abs(bottom.a - depth));
	blurred += bpower*bottom.r;
	divide += bpower;

	return blurred/divide;
}
#else // BLUR_GAUSSIAN
float4 HBlur(VSOUT IN) : COLOR0 {
	float color = tex2D(passSampler, IN.UVCoord).r;

	float blurred = color*0.2270270270;
	blurred += tex2D(passSampler, IN.UVCoord + float2(rcpres.x*1.3846153846, 0)).r * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord - float2(rcpres.x*1.3846153846, 0)).r * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord + float2(rcpres.x*3.2307692308, 0)).r * 0.0702702703;
	blurred += tex2D(passSampler, IN.UVCoord - float2(rcpres.x*3.2307692308, 0)).r * 0.0702702703;

	return blurred;
}

float4 VBlur(VSOUT IN) : COLOR0 {
	float color = tex2D(passSampler, IN.UVCoord).r;

	float blurred = color*0.2270270270;
	blurred += tex2D(passSampler, IN.UVCoord + float2(0, rcpres.y*1.3846153846)).r * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord - float2(0, rcpres.y*1.3846153846)).r * 0.3162162162;
	blurred += tex2D(passSampler, IN.UVCoord + float2(0, rcpres.y*3.2307692308)).r * 0.0702702703;
	blurred += tex2D(passSampler, IN.UVCoord - float2(0, rcpres.y*3.2307692308)).r * 0.0702702703;

	return blurred;
}
#endif // blur type

float4 Combine( VSOUT IN ) : COLOR0 {
	float4 color = tex2D(frameSampler, IN.UVCoord);
	float ao = tex2D(passSampler, IN.UVCoord/SCALE).r;
	ao = clamp(ao, aoClamp, 1.0);

	#ifdef LUMINANCE_CONSIDERATION
	float luminance = (color.r*0.2125f)+(color.g*0.7154f)+(color.b*0.0721f);
	float white = 1.0f;
	float black = 0.0f;

	luminance = clamp(max(black,luminance-luminosity_threshold)+max(black,luminance-luminosity_threshold)+max(black,luminance-luminosity_threshold),0.0,1.0);
	ao = lerp(ao,white,luminance);
	#endif

	color.rgb *= ao;
	
	return color;
	//return float4(ao,ao,ao,color.a);
}

technique t0
{
	pass p0
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ssao_Main();	
	}
	pass p1
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 HBlur();
	}
	pass p2
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 VBlur();
	}
	pass p3
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Combine();
	}
}