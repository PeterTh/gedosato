// Shaped Bokeh Depth of Field
// CoC Calculations based off Knu's DoF shader for MGE
// Blurring Algorithm Created by Tomerk
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
 
extern float base_blur_radius = DOF_BASE_BLUR_RADIUS; // base blur radius;
// higher values mean more blur when out of DoF and shorter DoF. 
 
extern float MR = 1.1;		// blur radius before radius increse in pixels;
extern float R = 16.0;		// maximum blur radius in pixels;
 
#define	EDGEWEIGHTING 		// gives edges more weight in highlights
 
#define	SMOOTHBLUR			// when on, smooths the blur, may have a performance hit

// ** END OF  ADJUSTABLE VARIABLES
// **
 
static float k = 0.00001;
 
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
 
#define M 60
static float2 taps[M] =
{
float2( 0.0000, 0.2500 ),
float2( -0.2165, 0.1250 ),
float2( -0.2165, -0.1250 ),
float2( -0.0000, -0.2500 ),
float2( 0.2165, -0.1250 ),
float2( 0.2165, 0.1250 ),
float2( 0.0000, 0.5000 ),
float2( -0.2500, 0.4330 ),
float2( -0.4330, 0.2500 ),
float2( -0.5000, 0.0000 ),
float2( -0.4330, -0.2500 ),
float2( -0.2500, -0.4330 ),
float2( -0.0000, -0.5000 ),
float2( 0.2500, -0.4330 ),
float2( 0.4330, -0.2500 ),
float2( 0.5000, -0.0000 ),
float2( 0.4330, 0.2500 ),
float2( 0.2500, 0.4330 ),
float2( 0.0000, 0.7500 ),
float2( -0.2565, 0.7048 ),
float2( -0.4821, 0.5745 ),
float2( -0.6495, 0.3750 ),
float2( -0.7386, 0.1302 ),
float2( -0.7386, -0.1302 ),
float2( -0.6495, -0.3750 ),
float2( -0.4821, -0.5745 ),
float2( -0.2565, -0.7048 ),
float2( -0.0000, -0.7500 ),
float2( 0.2565, -0.7048 ),
float2( 0.4821, -0.5745 ),
float2( 0.6495, -0.3750 ),
float2( 0.7386, -0.1302 ),
float2( 0.7386, 0.1302 ),
float2( 0.6495, 0.3750 ),
float2( 0.4821, 0.5745 ),
float2( 0.2565, 0.7048 ),
float2( 0.0000, 1.0000 ),
float2( -0.2588, 0.9659 ),
float2( -0.5000, 0.8660 ),
float2( -0.7071, 0.7071 ),
float2( -0.8660, 0.5000 ),
float2( -0.9659, 0.2588 ),
float2( -1.0000, 0.0000 ),
float2( -0.9659, -0.2588 ),
float2( -0.8660, -0.5000 ),
float2( -0.7071, -0.7071 ),
float2( -0.5000, -0.8660 ),
float2( -0.2588, -0.9659 ),
float2( -0.0000, -1.0000 ),
float2( 0.2588, -0.9659 ),
float2( 0.5000, -0.8660 ),
float2( 0.7071, -0.7071 ),
float2( 0.8660, -0.5000 ),
float2( 0.9659, -0.2588 ),
float2( 1.0000, -0.0000 ),
float2( 0.9659, 0.2588 ),
float2( 0.8660, 0.5000 ),
float2( 0.7071, 0.7071 ),
float2( 0.5000, 0.8660 ),
float2( 0.2588, 0.9659 )
};
 
 
float4 dof( float2 tex : TEXCOORD0 ) : COLOR0
{
	float s = focus*depthRange;
	float depth = readDepth(tex);
	float z = depth*depthRange;
	float weapon = 1;
 
	float fpf = clamp(1 / s + fr, fp, fp + fpa);
	float c = base_blur_radius * 0.009 * (fr - fpf + 1 / z) / fr / k;
	c *= 1.0 + smoothstep(MR, MR*1.2, c)*R;
	c = sign(z-s) * min(abs(c), R) / (2 * R);
  
	c += 0.5;
 
	return float4(tex2D(frameSampler, tex).rgb, c);
}
 
float4 smartblur( float2 tex : TEXCOORD0 ) : COLOR0
{
    float4 color = tex2D(passSampler, tex);
    float c = 2 * R * (color.a - 0.5);
 
    float weight = (1/(c*c+1))*dot(color.rgb + 0.01, float3(0.2126,0.7152,0.0722) );
	weight = max(0, weight);
#ifdef	EDGEWEIGHTING
	weight *= 0.25;
#endif
 
    color *= weight;
    float amount = weight;
 
    for (int i = 0; i < M; i++)
    {
        float2 dir = taps[i];
 
        float2 s_tex = tex + rcpres * dir * c;
        float4 s_color = tex2D(passSampler, s_tex);
        float s_c = abs((s_color.a - 0.5) * 2 * R);
 
		if (c < 0)
			s_c = max(abs(c), s_c);

		weight = (1 / (s_c * s_c + 1)) * dot(s_color.rgb + 0.01, float3(0.2126, 0.7152, 0.0722));
		weight *= saturate(1 - smoothstep(s_c, s_c * 1.1, length(dir) * abs(c)));

#ifdef	EDGEWEIGHTING
		weight *= saturate(0.25 + 0.75 * pow(length(dir) * c / (s_c), 2));
#endif
 
        color += s_color * weight;
        amount += weight;
    }
 
#ifdef SMOOTHBLUR
	return float4(color.rgb / amount, c / (2 * R) + 0.5);
#endif
 
	return float4(color.rgb / amount, 1);
}
 
float4 posthorblur( float2 tex : TEXCOORD0 ) : COLOR0
{
	float4 color = tex2D(passSampler, tex);
	const float c = 2 * R * (color.a - 0.5);
	float scale = c / 8; color *= 6;
 
	color += tex2D(passSampler, float2(tex.x - (rcpres.x    ) * scale, tex.y)) * 4;
	color += tex2D(passSampler, float2(tex.x + (rcpres.x    ) * scale, tex.y)) * 4;
 
	color += tex2D(passSampler, float2(tex.x - (rcpres.x * 2) * scale, tex.y)) * 1;
	color += tex2D(passSampler, float2(tex.x + (rcpres.x * 2) * scale, tex.y)) * 1;
 
	return float4(color.rgb / 16, c / (2 * R) + 0.5);
}
 
 
float4 postvertblur( float2 tex : TEXCOORD0 ) : COLOR0
{
	float4 color = tex2D(passSampler, tex);
	const float c = 2 * R * (color.a - 0.5);
	float scale = c / 8; color *= 6;
 
	color += tex2D(passSampler, float2(tex.x, tex.y - (rcpres.y    ) * scale)) * 4;
	color += tex2D(passSampler, float2(tex.x, tex.y + (rcpres.y    ) * scale)) * 4;
 
	color += tex2D(passSampler, float2(tex.x, tex.y - (rcpres.y * 2) * scale)) * 1;
	color += tex2D(passSampler, float2(tex.x, tex.y + (rcpres.y * 2) * scale)) * 1;
 
	return float4(color.rgb / 16, 1);
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