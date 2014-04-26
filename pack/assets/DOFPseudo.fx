// Depth of Field
// Originally by Knu for MGE
// Ported to OBGE and modified by Tomerk
// Optimized by Ethatron
// Adapted and tweaked for DS2 by Durante
 
#ifndef DOF_BASE_BLUR_RADIUS
#define DOF_BASE_BLUR_RADIUS 1.0
#endif
 
// **
// ** ADJUSTABLE VARIABLES
 
extern float fr = 166.0; // retina focus point, dpt
// set slightly lower than fp to simulate myopia or as an alternative to MGE's distant blur
// set slightly higher than fp+fpa to simulate hyperopia
 
extern float fp = 166.0; // eye relaxed focus power, dpt
extern float fpa = 30.0; // accomodation, dpt
// set lower to simulate presbyopia

extern float dmax = 0.009; // pupil diameter range, m
 
extern float base_blur_radius = DOF_BASE_BLUR_RADIUS; // base blur radius;
// higher values mean more blur when out of DoF and shorter DoF.
extern float blur_falloff = 2.0; // More means more blur and less respect for edges
 
extern float MR = 1.1;		// blur radius before radius increse in pixels;
extern float R = 16.0;		// maximum blur radius in pixels;
 
#define	SMOOTHBLUR			// when on, smooths the blur, may have a performance hit

// ** END OF  ADJUSTABLE VARIABLES
// **
  
static const float k = 0.00001;
static const float eps = 0.000001;
 
static const float2 rcpres = PIXEL_SIZE;
 
static const float nearZ = 400000;
static const float farZ = 2200000;
static const float depthRange = (nearZ-farZ)*0.01;

texture thisframeTex;
texture lastpassTex;
texture depthTex;
 
sampler frameSampler = sampler_state {texture = <thisframeTex>;	AddressU = Mirror;	AddressV = Mirror;	MINFILTER = LINEAR;	MAGFILTER = LINEAR;};
sampler depthSampler = sampler_state {texture = <depthTex>;	AddressU = CLAMP;	AddressV = CLAMP;	MINFILTER = LINEAR;	MAGFILTER = LINEAR;};
sampler passSampler = sampler_state {texture= <lastpassTex>; AddressU = Mirror; AddressV = Mirror; MagFilter = Linear; MinFilter = Linear;};
 
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
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}
 
float readDepth(in float2 coord : TEXCOORD0)
{
//	float posZ = tex2D(depthSampler, coord).x;
//	return (2.0f * nearZ) / (nearZ + farZ - posZ * (farZ - nearZ));
	float4 col = tex2D(depthSampler, coord);
	float posZ = ((col.x) + (col.y)*256.0 + (col.z)*(256.0*256.0));
	//return (posZ-nearZ)/farZ;
	return (2.0f * nearZ) / (nearZ + farZ - posZ * (farZ - nearZ));
}
 
static float focus = (2.0f * nearZ) / (nearZ + farZ - 63800 * (farZ - nearZ));
//static float focus = readDepth(float2(0.5,0.5));

#define M 12
static const float2 taps[M] =
{
	float2(-0.326212, -0.40581 ),
	float2(-0.840144, -0.07358 ),
	float2(-0.695914,  0.457137),
	float2(-0.203345,  0.620716),
	float2( 0.96234 , -0.194983),
	float2( 0.473434, -0.480026),
	float2( 0.519456,  0.767022),
	float2( 0.185461, -0.893124),
	float2( 0.507431,  0.064425),
	float2( 0.89642 ,  0.412458),
	float2(-0.32194 , -0.932615),
	float2(-0.791559, -0.59771 )
};
 
 
float4 dof( float2 tex : TEXCOORD0 ) : COLOR0
{
	float s = focus*depthRange;
	float depth = readDepth(tex);
	float z = depth*depthRange;
	float weapon = 1;
 
	float fpf = clamp(1 / s + fr, fp, fp + fpa);
	float c = dmax * (fr - fpf + 1 / z) / fr / k;
	c *= 1.0 + smoothstep(MR, MR*1.2, c)*R;
	c = sign(z-s) * min(abs(c), R) / (2 * R) * step(eps, z);
  
	c += 0.5;
 
	return float4(tex2D(frameSampler, tex).rgb, c);
}
 
float4 smartblur( float2 tex : TEXCOORD0 ) : COLOR0
{
    float4 color = tex2D(passSampler, tex);
	float c = base_blur_radius * 2 * R * ( color.a - 0.5);
 
	float amount = 1;

	[unroll]
	for (int i = 0; i < M; i++) {
		float2 dir = taps[i];
 
		float2 s_tex = tex + rcpres * dir * c;
		float4 s_color = tex2D(passSampler, s_tex);
		float  s_c = (s_color.a - 0.5) * 2 * R;
		float  weight = min(exp2(-(c - s_c) / blur_falloff), 2);
 
		color  += s_color * weight;
		amount +=           weight;
	}

#ifdef SMOOTHBLUR
	return float4(color.rgb / amount, c);
#endif

	return float4(color.rgb / amount, 1);
}
 
float4 posthorblur( float2 tex : TEXCOORD0 ) : COLOR0
{
	float4 color = tex2D(passSampler, tex);
	float c = color.a; color *= 70;
	float scale = c / 8;
 
	color += tex2D(passSampler, float2(tex.x - (rcpres.x    ) * scale, tex.y)) * 56;
	color += tex2D(passSampler, float2(tex.x + (rcpres.x    ) * scale, tex.y)) * 56;
 
	color += tex2D(passSampler, float2(tex.x - (rcpres.x * 2) * scale, tex.y)) * 28;
	color += tex2D(passSampler, float2(tex.x + (rcpres.x * 2) * scale, tex.y)) * 28;
 
	color += tex2D(passSampler, float2(tex.x - (rcpres.x * 3) * scale, tex.y)) * 8;
	color += tex2D(passSampler, float2(tex.x + (rcpres.x * 3) * scale, tex.y)) * 8;
 
	color += tex2D(passSampler, float2(tex.x - (rcpres.x * 4) * scale, tex.y)) * 1;
	color += tex2D(passSampler, float2(tex.x + (rcpres.x * 4) * scale, tex.y)) * 1;
 
	return float4(color.rgb / 256, c );
}
 
 
float4 postvertblur( float2 tex : TEXCOORD0 ) : COLOR0
{
	float4 color = tex2D(passSampler, tex);
	float c = color.a; color *= 70;
	float scale = c / 8;
 
	color += tex2D(passSampler, float2(tex.x, tex.y - (rcpres.y    ) * scale)) * 56;
	color += tex2D(passSampler, float2(tex.x, tex.y + (rcpres.y    ) * scale)) * 56;
 
	color += tex2D(passSampler, float2(tex.x, tex.y - (rcpres.y * 2) * scale)) * 28;
	color += tex2D(passSampler, float2(tex.x, tex.y + (rcpres.y * 2) * scale)) * 28;
 
	color += tex2D(passSampler, float2(tex.x, tex.y - (rcpres.y * 3) * scale)) * 8;
	color += tex2D(passSampler, float2(tex.x, tex.y + (rcpres.y * 3) * scale)) * 8;
 
	color += tex2D(passSampler, float2(tex.x, tex.y - (rcpres.y * 4) * scale)) * 1;
	color += tex2D(passSampler, float2(tex.x, tex.y + (rcpres.y * 4) * scale)) * 1;
 
	return float4(color.rgb / 256, 1 );
}
 
 
technique T0
{ 
	pass p0
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 dof();
	}
 
	pass p1
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 smartblur();
	}
	#ifdef SMOOTHBLUR
	pass p2
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 posthorblur();
	}
	pass p3
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 postvertblur();
	}
	#endif
}