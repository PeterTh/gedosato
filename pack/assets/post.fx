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

/*
   _____________________

     LumaSharpen 1.4.1
   _____________________

  by Christian Cann Schuldt Jensen ~ CeeJay.dk

  It blurs the original pixel with the surrounding pixels and then subtracts this blur to sharpen the image.
  It does this in luma to avoid color artifacts and allows limiting the maximum sharpening to avoid or lessen halo artifacts.

  This is similar to using Unsharp Mask in Photoshop.

  Compiles with 3.0
*/

#ifndef offset_bias
#define offset_bias 1.0
#endif

#define px (rcpres.x)
#define py (rcpres.y)

   /*-----------------------------------------------------------.
  /                      Developer settings                     /
  '-----------------------------------------------------------*/
#define CoefLuma float3(0.2126, 0.7152, 0.0722)      // BT.709 & sRBG luma coefficient (Monitors and HD Television)
//#define CoefLuma float3(0.299, 0.587, 0.114)       // BT.601 luma coefficient (SD Television)
//#define CoefLuma float3(1.0/3.0, 1.0/3.0, 1.0/3.0) // Equal weight coefficient

   /*-----------------------------------------------------------.
  /                          Main code                          /
  '-----------------------------------------------------------*/

float4 LumaSharpenPass(float4 inputcolor, float2 tex )
{
  // -- Get the original pixel --
  float3 ori = tex2D(s0, tex).rgb;       // ori = original pixel

  // -- Combining the strength and luma multipliers --
  float3 sharp_strength_luma = (CoefLuma * sharp_strength); //I'll be combining even more multipliers with it later on

   /*-----------------------------------------------------------.
  /                       Sampling patterns                     /
  '-----------------------------------------------------------*/
  //   [ NW,   , NE ] Each texture lookup (except ori)
  //   [   ,ori,    ] samples 4 pixels
  //   [ SW,   , SE ]

  // -- Pattern 1 -- A (fast) 7 tap gaussian using only 2+1 texture fetches.
  #if pattern == 1

	// -- Gaussian filter --
	//   [ 1/9, 2/9,    ]     [ 1 , 2 ,   ]
	//   [ 2/9, 8/9, 2/9]  =  [ 2 , 8 , 2 ]
 	//   [    , 2/9, 1/9]     [   , 2 , 1 ]

    float3 blur_ori = tex2D(s0, tex + (float2(px,py) / 3.0) * offset_bias).rgb;  // North West
    blur_ori += tex2D(s0, tex + (float2(-px,-py) / 3.0) * offset_bias).rgb; // South East

    //blur_ori += tex2D(s0, tex + float2(px,py) / 3.0 * offset_bias); // North East
    //blur_ori += tex2D(s0, tex + float2(-px,-py) / 3.0 * offset_bias); // South West

    blur_ori /= 2;  //Divide by the number of texture fetches

    sharp_strength_luma *= 1.5; // Adjust strength to aproximate the strength of pattern 2

  #endif

  // -- Pattern 2 -- A 9 tap gaussian using 4+1 texture fetches.
  #if pattern == 2

	// -- Gaussian filter --
	//   [ .25, .50, .25]     [ 1 , 2 , 1 ]
	//   [ .50,   1, .50]  =  [ 2 , 4 , 2 ]
 	//   [ .25, .50, .25]     [ 1 , 2 , 1 ]


    float3 blur_ori = tex2D(s0, tex + float2(px,-py) * 0.5 * offset_bias).rgb; // South East
    blur_ori += tex2D(s0, tex + float2(-px,-py) * 0.5 * offset_bias).rgb;  // South West
    blur_ori += tex2D(s0, tex + float2(px,py) * 0.5 * offset_bias).rgb; // North East
    blur_ori += tex2D(s0, tex + float2(-px,py) * 0.5 * offset_bias).rgb; // North West

    blur_ori *= 0.25;  // ( /= 4) Divide by the number of texture fetches

  #endif

  // -- Pattern 3 -- An experimental 17 tap gaussian using 4+1 texture fetches.
  #if pattern == 3

	// -- Gaussian filter --
	//   [   , 4 , 6 ,   ,   ]
	//   [   ,16 ,24 ,16 , 4 ]
	//   [ 6 ,24 ,   ,24 , 6 ]
	//   [ 4 ,16 ,24 ,16 ,   ]
	//   [   ,   , 6 , 4 ,   ]

    float3 blur_ori = tex2D(s0, tex + float2(0.4*px,-1.2*py)* offset_bias).rgb;  // South South East
    blur_ori += tex2D(s0, tex + float2(-1.2*px,-0.4*py) * offset_bias).rgb; // West South West
    blur_ori += tex2D(s0, tex + float2(1.2*px,0.4*py) * offset_bias).rgb; // East North East
    blur_ori += tex2D(s0, tex + float2(-0.4*px,1.2*py) * offset_bias).rgb; // North North West

    blur_ori *= 0.25;  // ( /= 4) Divide by the number of texture fetches

    sharp_strength_luma *= 0.51;
  #endif

  // -- Pattern 4 -- A 9 tap high pass (pyramid filter) using 4+1 texture fetches.
  #if pattern == 4

	// -- Gaussian filter --
	//   [ .50, .50, .50]     [ 1 , 1 , 1 ]
	//   [ .50,    , .50]  =  [ 1 ,   , 1 ]
 	//   [ .50, .50, .50]     [ 1 , 1 , 1 ]

    float3 blur_ori = tex2D(s0, tex + float2(0.5 * px,-py * offset_bias)).rgb;  // South South East
    blur_ori += tex2D(s0, tex + float2(offset_bias * -px,0.5 * -py)).rgb; // West South West
    blur_ori += tex2D(s0, tex + float2(offset_bias * px,0.5 * py)).rgb; // East North East
    blur_ori += tex2D(s0, tex + float2(0.5 * -px,py * offset_bias)).rgb; // North North West

    //blur_ori += (2 * ori); // Probably not needed. Only serves to lessen the effect.

    blur_ori /= 4.0;  //Divide by the number of texture fetches

    sharp_strength_luma *= 0.666; // Adjust strength to aproximate the strength of pattern 2
  #endif

  // -- Pattern 8 -- A (slower) 9 tap gaussian using 9 texture fetches.
  #if pattern == 8

	// -- Gaussian filter --
	//   [ 1 , 2 , 1 ]
	//   [ 2 , 4 , 2 ]
 	//   [ 1 , 2 , 1 ]

    half3 blur_ori = tex2D(s0, tex + float2(-px,py) * offset_bias).rgb; // North West
    blur_ori += tex2D(s0, tex + float2(px,-py) * offset_bias).rgb;     // South East
    blur_ori += tex2D(s0, tex + float2(-px,-py)  * offset_bias).rgb;  // South West
    blur_ori += tex2D(s0, tex + float2(px,py) * offset_bias).rgb;    // North East

    half3 blur_ori2 = tex2D(s0, tex + float2(0,py) * offset_bias).rgb; // North
    blur_ori2 += tex2D(s0, tex + float2(0,-py) * offset_bias).rgb;    // South
    blur_ori2 += tex2D(s0, tex + float2(-px,0) * offset_bias).rgb;   // West
    blur_ori2 += tex2D(s0, tex + float2(px,0) * offset_bias).rgb;   // East
    blur_ori2 *= 2.0;

    blur_ori += blur_ori2;
    blur_ori += (ori * 4); // Probably not needed. Only serves to lessen the effect.

    // dot()s with gaussian strengths here?

    blur_ori /= 16.0;  //Divide by the number of texture fetches

    //sharp_strength_luma *= 0.75; // Adjust strength to aproximate the strength of pattern 2
  #endif

  // -- Pattern 9 -- A (slower) 9 tap high pass using 9 texture fetches.
  #if pattern == 9

	// -- Gaussian filter --
	//   [ 1 , 1 , 1 ]
	//   [ 1 , 1 , 1 ]
 	//   [ 1 , 1 , 1 ]

    float3 blur_ori = tex2D(s0, tex + float2(-px,py) * offset_bias).rgb; // North West
    blur_ori += tex2D(s0, tex + float2(px,-py) * offset_bias).rgb;     // South East
    blur_ori += tex2D(s0, tex + float2(-px,-py)  * offset_bias).rgb;  // South West
    blur_ori += tex2D(s0, tex + float2(px,py) * offset_bias).rgb;    // North East

    blur_ori += ori.rgb; // Probably not needed. Only serves to lessen the effect.

    blur_ori += tex2D(s0, tex + float2(0,py) * offset_bias).rgb;    // North
    blur_ori += tex2D(s0, tex + float2(0,-py) * offset_bias).rgb;  // South
    blur_ori += tex2D(s0, tex + float2(-px,0) * offset_bias).rgb; // West
    blur_ori += tex2D(s0, tex + float2(px,0) * offset_bias).rgb; // East

    blur_ori /= 9;  //Divide by the number of texture fetches

    //sharp_strength_luma *= (8.0/9.0); // Adjust strength to aproximate the strength of pattern 2
  #endif


   /*-----------------------------------------------------------.
  /                            Sharpen                          /
  '-----------------------------------------------------------*/

  // -- Calculate the sharpening --
  float3 sharp = ori - blur_ori;  //Subtracting the blurred image from the original image

  #if 0 //New experimental limiter .. not yet finished
    float sharp_luma = dot(sharp, sharp_strength_luma); //Calculate the luma
    sharp_luma = (abs(sharp_luma)*8.0) * exp(1.0-(abs(sharp_luma)*8.0)) * sign(sharp_luma) / 16.0; //I should probably move the strength modifier here

  #elif 0 //SweetFX 1.4 code
    // -- Adjust strength of the sharpening --
    float sharp_luma = dot(sharp, sharp_strength_luma); //Calculate the luma and adjust the strength

    // -- Clamping the maximum amount of sharpening to prevent halo artifacts --
    sharp_luma = clamp(sharp_luma, -sharp_clamp, sharp_clamp);  //TODO Try a curve function instead of a clamp
  
  #else //SweetFX 1.5.1 code
    // -- Adjust strength of the sharpening and clamp it--
    float4 sharp_strength_luma_clamp = float4(sharp_strength_luma * (0.5 / sharp_clamp),0.5); //Roll part of the clamp into the dot

    //sharp_luma = saturate((0.5 / sharp_clamp) * sharp_luma + 0.5); //scale up and clamp
    float sharp_luma = saturate(dot(float4(sharp,1.0), sharp_strength_luma_clamp)); //Calculate the luma, adjust the strength, scale up and clamp
    sharp_luma = (sharp_clamp * 2.0) * sharp_luma - sharp_clamp; //scale down
  #endif

  // -- Combining the values to get the final sharpened pixel	--
  //float4 done = ori + sharp_luma;    // Add the sharpening to the original.
  inputcolor.rgb = inputcolor.rgb + sharp_luma;    // Add the sharpening to the input color.

   /*-----------------------------------------------------------.
  /                     Returning the output                    /
  '-----------------------------------------------------------*/
  #ifndef show_sharpen
  #define show_sharpen 0
  #endif
  #if show_sharpen == 1
    //inputcolor.rgb = abs(sharp * 4.0);
    inputcolor.rgb = saturate(0.5 + (sharp_luma * 4)).rrr;
  #endif

  return saturate(inputcolor);

}

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