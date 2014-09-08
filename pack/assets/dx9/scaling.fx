texture2D frameTex2D;

float2 inputSize;

#ifndef USE_SRGB
#define USE_SRGB true
#endif

sampler frameSampler = sampler_state
{
	texture = <frameTex2D>;
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

float4 scaleLinear(VSOUT IN) : COLOR0
{
	return tex2D(frameSampler, IN.UVCoord);
}

// Improved bicubic interpolation that only need 4 texture fetch instead of 16
// From the paper: Accuracy of GPU-based B-Spline evaluation
// http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter20.html
float4 bicubicInterpolation(in float2 uv, in sampler2D tex, in float2 texSize) {
	float2 rec_nrCP = 1.0/texSize;
	float2 coord_hg = uv * texSize-0.5;
	float2 index = floor(coord_hg);

	float2 f = coord_hg - index;
	float4x4 M = {
	-1, 3, -3, 1,
	3, -6, 3, 0,
	-3, 0, 3, 0,
	1, 4, 1, 0
	};
	M /= 6;

	float4 wx = mul(float4(f.x*f.x*f.x, f.x*f.x, f.x, 1), M);
	float4 wy = mul(float4(f.y*f.y*f.y, f.y*f.y, f.y, 1), M);
	float2 w0 = float2(wx.x, wy.x);
	float2 w1 = float2(wx.y, wy.y);
	float2 w2 = float2(wx.z, wy.z);
	float2 w3 = float2(wx.w, wy.w);

	float2 g0 = w0 + w1;
	float2 g1 = w2 + w3;
	float2 h0 = w1 / g0 - 1;
	float2 h1 = w3 / g1 + 1;

	float2 coord00 = index + h0;
	float2 coord10 = index + float2(h1.x,h0.y);
	float2 coord01 = index + float2(h0.x,h1.y);
	float2 coord11 = index + h1;

	coord00 = (coord00 + 0.5) * rec_nrCP;
	coord10 = (coord10 + 0.5) * rec_nrCP;
	coord01 = (coord01 + 0.5) * rec_nrCP;
	coord11 = (coord11 + 0.5) * rec_nrCP;

	float4 tex00 = tex2Dlod(tex, float4(coord00, 0, 0));
	float4 tex10 = tex2Dlod(tex, float4(coord10, 0, 0));
	float4 tex01 = tex2Dlod(tex, float4(coord01, 0, 0));
	float4 tex11 = tex2Dlod(tex, float4(coord11, 0, 0));

	tex00 = lerp(tex01, tex00, float4(g0.y,g0.y,g0.y,g0.y));
	tex10 = lerp(tex11, tex10, float4(g0.y,g0.y,g0.y,g0.y));
	float4 res = lerp(tex10, tex00, float4(g0.x,g0.x,g0.x,g0.x));
	return res;
}

float4 scaleCubic(VSOUT IN) : COLOR0
{
	return bicubicInterpolation(IN.UVCoord, frameSampler, inputSize);
}

// Lanczos - adapted from libretro
#define FIX(c) max(abs(c), 1e-5);
const float PI = 3.1415926535897932384626433832795;

float4 weight4(float x)
{
	float4 sample = FIX(PI * float4(1.0 + x, x, 1.0 - x, 2.0 - x));

	// Lanczos2. Note: we normalize below, so no point in multiplying by radius (2.0)
	float4 ret = /*2.0 **/ sin(sample) * sin(sample / 2.0) / (sample * sample);

	// Normalize
	return ret / dot(ret, float4(1.0, 1.0, 1.0, 1.0));
}

float3 pixel(float xpos, float ypos)
{
	return tex2D(frameSampler, float2(xpos, ypos)).rgb;
}

float3 line_run(float ypos, float4 xpos, float4 linetaps)
{
	return mul(linetaps, float4x3(
		pixel(xpos.x, ypos),
		pixel(xpos.y, ypos),
		pixel(xpos.z, ypos),
		pixel(xpos.w, ypos)));
}

float4 scaleLanczos(VSOUT IN) : COLOR0
{
	float2 stepxy = 1.0 / inputSize;
	float2 pos = IN.UVCoord + stepxy * 0.5;
	float2 f = frac(pos / stepxy);

	float2 xystart = (-1.5 - f) * stepxy + pos;
	float4 xpos = float4(
		xystart.x,
		xystart.x + stepxy.x,
		xystart.x + stepxy.x * 2.0,
		xystart.x + stepxy.x * 3.0);

	float4 linetaps = weight4(f.x);
	float4 columntaps = weight4(f.y);
	
	// final sum and weight normalization
	return float4(mul(columntaps, float4x3(
		line_run(xystart.y, xpos, linetaps),
		line_run(xystart.y + stepxy.y, xpos, linetaps),
		line_run(xystart.y + stepxy.y * 2.0, xpos, linetaps),
		line_run(xystart.y + stepxy.y * 3.0, xpos, linetaps))), 1.0);
}

technique t0
{
	pass P0
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 scaleLinear();
		ZEnable = false;        
		SRGBWriteEnable = USE_SRGB;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}
	pass P1
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 scaleCubic();
		ZEnable = false;        
		SRGBWriteEnable = USE_SRGB;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}
	pass P2
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 scaleLanczos();
		ZEnable = false;
		SRGBWriteEnable = USE_SRGB;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		ColorWriteEnable = RED | GREEN | BLUE | ALPHA;
	}
}