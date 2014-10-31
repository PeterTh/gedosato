//----------------------------------------------------------------------------------
// File:    HorizonBasedAOEngine.fx
// Authors: Louis Bavoil & Miguel Sainz
// Email:   sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//----------------------------------------------------------------------------------

/** 
  \file SAO_blur.pix
  \author Morgan McGuire and Michael Mara, NVIDIA Research

  \brief 7-tap 1D cross-bilateral blur using a packed depth key

  DX11 HLSL port by Leonardo Zide, Treyarch
  
  Open Source under the "BSD" license: http://www.opensource.org/licenses/bsd-license.php

  Copyright (c) 2011-2012, NVIDIA
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//////////////////////////////////////////////////////// TWEAKABLE VALUES /////////////////////////////////////////////////////////////

//#define SHOW_SSAO           /** Uncomment this to display the bare SSAO output */
//#define SHOW_DEPTH          /** Shows depth texture. Useful to debug if you get no AO (white output) when #define SHOW_SSAO is on */
//#define SHOW_UNBLURRED      /** Useful to fine-tune your unblurred AO. Also random noise generation is cool */

/** Manual nearZ/farZ values to compensate the fact we do not have access to the real projection matrix from the game */
/** This is key for the effect to look right. Basically you need to define the boundaries of the space you're going to shade */
/** Usually nearZ is (0..10) and farZ is (10..300). Experiment with these until you approximate the right values for the game */
static float nearZ = 1.0;
static float farZ = 1000.0;

/** Used for preventing AO computation on the sky (at infinite depth) and defining the CS Z to bilateral depth key scaling. */
/** This need not match the real far plane */
/** This goes together with nearZ/farZ values. If you can't see any (or partial) AO output at all make sure this value isn't too low :] */
#define FAR_PLANE_Z (460.0)

/** intensity : darkending factor, e.g., 1.0 */
#ifdef SSAO_STRENGTH_LOW
	static float  g_Strength = 0.6f;                 // 0.0..3.0
	static float  g_IntensityMul = 0.8f;             // 1.0..3.0
#endif

#ifdef SSAO_STRENGTH_MEDIUM
	static float  g_Strength = 0.8f;                 // 0.0..3.0
	static float  g_IntensityMul = 1.0f;             // 1.0..3.0
#endif

#ifdef SSAO_STRENGTH_HIGH
	static float  g_Strength = 1.0f;                 // 0.0..3.0
	static float  g_IntensityMul = 1.1f;             // 1.0..3.0
#endif

static float  g_NumSteps = 3;                        // 0..32
static float  g_NumDir = 5;                          // 0..25
static float  m_RadiusMultiplier = 8.0;              // 0.0..2.0
static float  m_AngleBias = 0.0;                     // 0.0..60.0
#define SAMPLE_FIRST_STEP 1

/** Comment this line to not take pixel brightness into account (the higher the more AO will blend into bright surfaces) */
#define LUMINANCE_CONSIDERATION
extern float luminosity_threshold = 0.7;

/** Increase to make edges crisper. Decrease to reduce temporal flicker. */
#define EDGE_SHARPNESS     (1.0)

/** Step in 2-pixel intervals since we already blurred against neighbors in the
first AO pass.  This constant can be increased while R decreases to improve
performance at the expense of some dithering artifacts.

Morgan found that a scale of 3 left a 1-pixel checkerboard grid that was
unobjectionable after shading was applied but eliminated most temporal incoherence
from using small numbers of sample taps.
*/
#define SCALE               (2)

/** Filter radius in pixels. This will be multiplied by SCALE. */
#define R                   (6)

/////////////////////////////////////////////////// END OF TWEAKABLE VALUES ///////////////////////////////////////////////////////////

#define M_PI 3.14159265f
static float  g_R = m_RadiusMultiplier * 1.0f;   // R = m_RadiusMultiplier (1.0f) * m_AORadius (radius*SceneScale) https://code.google.com/p/core-fusion/source/browse/trunk/SubKrieg/Fusion.Rendering/RenderSystem.HBAO.cs?r=1202
static float  g_Radius2 = g_R * g_R;
static float  g_NegInvRadius2 = -1.0f / g_Radius2;     // g_NegInvRadius2 (???) --> -1.0f / g_R;
static float  g_AngleBias = m_AngleBias * M_PI / 180; // m_AngleBias (30) * D3DX_PI / 180;
static float  g_TanAngleBias = tan(g_AngleBias);
// static float  g_Contrast = m_Contrast / (1.0f - sin(m_AngleBias * M_PI / 180));  // m_Contrast (1.25) / (1.0f - sin(m_AngleBias * D3DX_PI / 180));
static float  g_MaxRadiusPixels = 0.1f * min(SCREEN_SIZE.x, SCREEN_SIZE.y);

extern float2 g_FocalLen;
// extern float2 g_InvFocalLen;
// extern float2 g_InvResolution;
// extern float2 g_Resolution;
extern bool isBlurHorizontal;

#ifndef USE_SRGB
#define USE_SRGB true
#endif

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

texture2D frameTex2D;
sampler frameSampler = sampler_state
{
	texture = <frameTex2D>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Clamp;
	AddressV  = Clamp;
	SRGBTexture = USE_SRGB;
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

texture2D noiseTexture < string filename = "RandomNoiseB.dds"; >;
sampler2D noiseSampler = sampler_state {
	texture = <noiseTexture>;
	AddressU = WRAP;
	AddressV = WRAP;
	MINFILTER = POINT;
	MAGFILTER = POINT;
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


VSOUT FrameVS(VSIN IN) {
	VSOUT OUT = (VSOUT)0.0f;
 
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
 
	return OUT;
}

//----------------------------------------------------------------------------------
float3 fetchEyePos(in float2 v)
{
	// float z = tex2Dlod(depthSampler, float4(uv, 0, 0)).r;
    // return uv_to_eye(uv, z);
    float z = tex2Dlod(depthSampler, float4(v, 0, 0)).r;
	v = v * 2 - 1;   // (v * float2(2.0, -2.0) - float2(1.0, -1.0));
	float3 result = float3(v * 1.0, 1) * z;
	return result;
}

//----------------------------------------------------------------------------------
float length2(float3 v)
{
    return dot(v, v);
}

//----------------------------------------------------------------------------------
float invLength(float2 v)
{
    return rsqrt(dot(v,v));
}

//----------------------------------------------------------------------------------
float3 minDiff(float3 P, float3 Pr, float3 Pl)
{
    float3 V1 = Pr - P;
    float3 V2 = P - Pl;
    return (length2(V1) < length2(V2)) ? V1 : V2;
}

//----------------------------------------------------------------------------------
float2 rotateDirections(float2 Dir, float2 CosSin)
{
    return float2(Dir.x*CosSin.x - Dir.y*CosSin.y, Dir.x*CosSin.y + Dir.y*CosSin.x);
}

//----------------------------------------------------------------------------------
float falloff(float d2)
{
    return d2 * g_NegInvRadius2 + 1.0f;
}

//----------------------------------------------------------------------------------
float2 snapUVOffset(float2 uv)
{
    return round(uv * SCREEN_SIZE) * PIXEL_SIZE;
}

//----------------------------------------------------------------------------------
float tangent(float3 T)
{
    return -T.z * invLength(T.xy);
}

//----------------------------------------------------------------------------------
float tangent(float3 P, float3 S)
{
    return (P.z - S.z) * invLength(S.xy - P.xy);
}

//----------------------------------------------------------------------------------
float biasedtangent(float3 T)
{
    // float phi = atan(tangent(T)) + g_AngleBias;
    // return tan(min(phi, M_PI*0.5));
    return tangent(T) + g_TanAngleBias;
}

//----------------------------------------------------------------------------------
float3 tangentVector(float2 deltaUV, float3 dPdu, float3 dPdv)
{
    return deltaUV.x * dPdu + deltaUV.y * dPdv;
}

//----------------------------------------------------------------------------------
float tanToSin(float x)
{
    return x * rsqrt(x*x + 1.0f);
}

//----------------------------------------------------------------------------------
void computeSteps(inout float2 step_size_uv, inout float numSteps, float ray_radius_pix, float rand)
{
    // Avoid oversampling if g_NumSteps is greater than the kernel radius in pixels
    numSteps = min(g_NumSteps, ray_radius_pix);

    // Divide by Ns+1 so that the farthest samples are not fully attenuated
    float step_size_pix = ray_radius_pix / (numSteps + 1);

    // Clamp numSteps if it is greater than the max kernel footprint
    float maxNumSteps = g_MaxRadiusPixels / step_size_pix;
	[branch]
    if (maxNumSteps < numSteps)
    {
        // Use dithering to avoid AO discontinuities
        numSteps = floor(maxNumSteps + rand);
        numSteps = max(numSteps, 1);
        step_size_pix = g_MaxRadiusPixels / numSteps;
    }

    // Step size in uv space
    step_size_uv = step_size_pix * PIXEL_SIZE;
}

//----------------------------------------------------------------------------------
float integerateOcclusion(float2 uv0, float2 snapped_duv, float3 P, float3 dPdu, float3 dPdv, inout float tanH)
{
    float ao = 0;

    // Compute a tangent vector for snapped_duv
    float3 T1 = tangentVector(snapped_duv, dPdu, dPdv);
    float tanT = biasedtangent(T1);
    float sinT = tanToSin(tanT);

    float3 S = fetchEyePos(uv0 + snapped_duv);
    float tanS = tangent(P, S);

    float sinS = tanToSin(tanS);
    float d2 = length2(S - P);

	[branch]	
    if ((d2 < g_Radius2) && (tanS > tanT))
    {
        // Compute AO between the tangent plane and the sample
        ao = falloff(d2) * (sinS - sinT);

        // Update the horizon angle
        tanH = max(tanH, tanS);
    }

    return ao;
}

//----------------------------------------------------------------------------------
float horizon_occlusion(float2 deltaUV, float2 texelDeltaUV, float2 uv0, float3 P, float numSteps, float randstep, float3 dPdu, float3 dPdv )
{
    float ao = 0;

    // Randomize starting point within the first sample distance
    float2 uv = uv0 + snapUVOffset( randstep * deltaUV );

    // Snap increments to pixels to avoid disparities between xy
    // and z sample locations and sample along a line
    deltaUV = snapUVOffset( deltaUV );

    // Compute tangent vector using the tangent plane
    float3 T = deltaUV.x * dPdu + deltaUV.y * dPdv;

    float tanH = biasedtangent(T);

#if SAMPLE_FIRST_STEP
    // Take a first sample between uv0 and uv0 + deltaUV
    float2 snapped_duv = snapUVOffset( randstep * deltaUV + texelDeltaUV );
    ao = integerateOcclusion(uv0, snapped_duv, P, dPdu, dPdv, tanH);
    --numSteps;
#endif

    float sinH = tanH / sqrt(1.0f + tanH*tanH);

	[loop]
    for (float j = 1; j <= numSteps; ++j)
    {
        uv += deltaUV;
        float3 S = fetchEyePos(uv);
        float tanS = tangent(P, S);
        float d2 = length2(S - P);

        // Use a merged dynamic branch
		[branch]
        if ((d2 < g_Radius2) && (tanS > tanH))
        {
            // Accumulate AO between the horizon and the sample
            float sinS = tanS / sqrt(1.0f + tanS*tanS);
            ao += falloff(d2) * (sinS - sinH);

            // Update the current horizon angle
            tanH = tanS;
            sinH = sinS;
        }
    }

    return ao;
}

/** Used for packing Z into the GB channels */
float CSZToKey(float z)
{
    return clamp(z * (1.0 / FAR_PLANE_Z), 0.0, 1.0);
}

/** Used for packing Z into the GB channels */
void packKey(float key, out float2 p)
{
    // Round to the nearest 1/256.0
    float temp = floor(key * 256.0);
    // Integer part
    p.x = temp * (1.0 / 256.0);
    // Fractional part
    p.y = key * 256.0 - temp;
}

float unpackKey(float2 p)
{
    return p.x * (256.0 / 257.0) + p.y * (1.0 / 257.0);
}

/** Negative, "linear" values in world-space units */
float LinearizeDepth(float depth)
{
    //return (2.0f * nearZ) / (nearZ + farZ - depth * (farZ - nearZ));
	return rcp(depth * ((farZ - nearZ) / (-farZ * nearZ)) + farZ / (farZ * nearZ));
}

float4 reconstructCSZPass(VSOUT IN) : COLOR0
{
	return float4(LinearizeDepth(tex2D(depthSampler, IN.UVCoord).r), 0, 0, 0);
}

//----------------------------------------------------------------------------------
float4 mainSSAOPass( VSOUT IN, float2 vPos : VPOS ) : COLOR0 
{
    float4 output = float4(1,1,1,1);
    float3 P = fetchEyePos(IN.UVCoord);
   
    // r = cos(alpha), g = sin(alpha), b = jitter
	float3 rand = tex2Dlod(noiseSampler, float4(vPos / 64.0, 0, 0)).rgb * 2 - 1; // 64.0 <-- noise texture width
	//float3 rand = tex2Dlod(noiseSampler, float4((vPos.x - floor(vPos.x)) * 64, (vPos.y - floor(vPos.y)) * 64, 0, 0)).rgb;
	//float3 rand = tex2Dlod(noiseSampler, float4(IN.UVCoord*24.0, 0, 0)).rgb;
	//float3 rand = tex2Dlod(noiseSampler, float4(IN.UVCoord * (SCREEN_SIZE / 4), 0, 0)).rgb;

    // Compute projection of disk of radius radius into uv space
    // Multiply by 0.5 to scale from [-1,1]^2 to [0,1]^2
    float2 ray_radius_uv = 0.5 * g_R * g_FocalLen / P.z;
    float ray_radius_pix = ray_radius_uv.x * SCREEN_SIZE.x;
    if (ray_radius_pix < 10)
    {
    	return 1.0;
    }

    float numSteps;
    float2 step_size;
    computeSteps(step_size, numSteps, ray_radius_pix, rand.z);

	// Store Z for blur
    // packKey(CSZToKey(P.z), output.gb);
	//float z = tex2D(depthSampler, IN.UVCoord).r;
	//packKey(CSZToKey(z), output.gb);
	packKey(CSZToKey(P.z), output.gb);

    // Nearest neighbor pixels on the tangent plane
    float3 Pr, Pl, Pt, Pb;
    Pr = fetchEyePos(IN.UVCoord + float2(PIXEL_SIZE.x, 0));
    Pl = fetchEyePos(IN.UVCoord + float2(-PIXEL_SIZE.x, 0));
    Pt = fetchEyePos(IN.UVCoord + float2(0, PIXEL_SIZE.y));
    Pb = fetchEyePos(IN.UVCoord + float2(0, -PIXEL_SIZE.y));

    // Screen-aligned basis for the tangent plane
    float3 dPdu = minDiff(P, Pr, Pl) * (SCREEN_SIZE.x * PIXEL_SIZE.y);
    float3 dPdv = minDiff(P, Pt, Pb) * (SCREEN_SIZE.y * PIXEL_SIZE.x);

    float ao = 0;
    float d;
    float alpha = 2.0f * M_PI / g_NumDir;

	[loop]	// <-- without this the shader takes 1 mn to compile
    for (d = 0; d < g_NumDir; ++d)
    {
         float angle = alpha * d;
         float2 dir = rotateDirections(float2(cos(angle), sin(angle)), rand.xy);
         float2 deltaUV = dir * step_size.xy;
         float2 texelDeltaUV = dir * PIXEL_SIZE;
         ao += horizon_occlusion(deltaUV, texelDeltaUV, IN.UVCoord, P, numSteps, rand.z, dPdu, dPdv);
    }

	output.r = 1.0 - ao / g_NumDir * g_Strength * g_IntensityMul;
	#ifdef SHOW_UNBLURRED
		return float4(output.r, output.r, output.r, 1.0);
	#endif
	
    return output;
}

//----------------------------------------------------------------------------------

/** Swizzle to use to extract the channels of source. This macro allows
the same blur shader to be used on different kinds of input data. */
#define VALUE_COMPONENTS   r

#define VALUE_IS_KEY       0

/** Channel encoding the bilateral key value (which must not be the same as VALUE_COMPONENTS) */
#define KEY_COMPONENTS     gb

// Gaussian coefficients
static const float gaussian[] =
//	{ 0.356642, 0.239400, 0.072410, 0.009869 };
//	{ 0.398943, 0.241971, 0.053991, 0.004432, 0.000134 };  // stddev = 1.0
//  { 0.153170, 0.144893, 0.122649, 0.092902, 0.062970 };  // stddev = 2.0
  { 0.111220, 0.107798, 0.098151, 0.083953, 0.067458, 0.050920, 0.036108 }; // stddev = 3.0

#define  result         output.VALUE_COMPONENTS
#define  keyPassThrough output.KEY_COMPONENTS

//----------------------------------------------------------------------------------
float4 BlurBL(VSOUT IN) : COLOR0
{
	float4 output = float4(1,1,1,1);

	float2 ssC = IN.UVCoord;

	float4 temp = tex2D(passSampler, ssC);

    keyPassThrough = temp.KEY_COMPONENTS;
    float key = unpackKey(keyPassThrough);

	float sum = temp.VALUE_COMPONENTS;

	[branch]
    if (key >= 0.999) {
        // Sky pixel (if you aren't using depth keying, disable this test)
        result = sum;
        return output;
    }

	// Base weight for depth falloff.  Increase this for more blurriness,
	// decrease it for better edge discrimination
	float BASE = gaussian[0];
	float totalWeight = BASE;
	sum *= totalWeight;

	[unroll]
	for (int r = -R; r <= R; ++r) {
		// We already handled the zero case above.  This loop should be unrolled and the branch discarded
		if (r != 0) {
            float2 axis = (isBlurHorizontal) ? float2(1, 0) : float2(0, 1);
			temp = tex2D(passSampler, ssC + axis * PIXEL_SIZE * (r * SCALE));
			float tapKey = unpackKey(temp.KEY_COMPONENTS);
			float value  = temp.VALUE_COMPONENTS;

            // spatial domain: offset gaussian tap
            float weight = 0.3 + gaussian[abs(r)];
		
			// range domain (the "bilateral" weight). As depth difference increases, decrease weight.
			weight *= max(0.0, 1.0 - (400.0 * EDGE_SHARPNESS) * abs(tapKey - key));

            sum += value * weight;
            totalWeight += weight;
		} 
	}

	const float epsilon = 0.0001;
	result = sum / (totalWeight + epsilon);

	return output;
}

//----------------------------------------------------------------------------------
// Usually to go from 16:9 to 16:10 but could be used for wilder AR
// See ssao.cp to see what happens with aspectQuad()
float4 aspectRatioPass( VSOUT IN ) : COLOR0 {
	return tex2D(passSampler, IN.UVCoord);
}

//----------------------------------------------------------------------------------
float4 combine( VSOUT IN ) : COLOR0 {
	float4 color = tex2D(frameSampler, IN.UVCoord);
	float ao = tex2D(passSampler, IN.UVCoord).r;

	#ifdef SHOW_DEPTH
		nearZ = 0.1;
		farZ = 1.0;
		float depth = LinearizeDepth(tex2D(depthSampler, IN.UVCoord).r);
		return float4(depth, depth, depth, 1.0);		
	#endif
	#ifdef SHOW_SSAO
		return float4(ao,ao,ao,1);
	#endif
	
	#ifdef LUMINANCE_CONSIDERATION
	float luminance = (color.r*0.2125f)+(color.g*0.7154f)+(color.b*0.0721f);
	float white = 1.0f;
	float black = 0.0f;

	luminance = clamp(max(black,luminance-luminosity_threshold)+max(black,luminance-luminosity_threshold)+max(black,luminance-luminosity_threshold),0.0,1.0);
	ao = lerp(ao,white,luminance);
	#endif	

	color.rgb *= ao;
	return color;
}

technique t0
{
	pass p0
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 reconstructCSZPass();
	}
	pass p1
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 mainSSAOPass();
	}
	pass p2
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 BlurBL();
	}
	pass p3
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 aspectRatioPass();	
	}		
	pass p4
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 combine();
	}	
}