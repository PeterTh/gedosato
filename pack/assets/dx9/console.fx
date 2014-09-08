texture2D textTex2D;

float4 rectColor;

sampler textSampler = sampler_state
{
	texture = <textTex2D>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
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

float4 flat(VSOUT IN) : COLOR0
{
	return rectColor;
}

float4 font(VSOUT IN) : COLOR0
{
	return float4(1.0, 1.0, 1.0, tex2D(textSampler, IN.UVCoord).a);
}

technique t0
{
	pass P0
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 flat();
		ZEnable = false;        
		AlphaBlendEnable = true;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
		AlphaTestEnable = false;
	}
	pass P1
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 font();
		ZEnable = false;        
		AlphaBlendEnable = true;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
		AlphaTestEnable = false;
	}
}