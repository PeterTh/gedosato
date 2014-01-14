texture2D frameTex2D;

float2 inputSize;

sampler frameSampler = sampler_state
{
	texture = <frameTex2D>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	SRGBTexture = 1;
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

technique t0
{
	pass P0
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 scaleLinear();
		ZEnable = false;        
		SRGBWriteEnable = true;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}
	pass P1
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 scaleCubic();
		ZEnable = false;        
		SRGBWriteEnable = true;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}
}