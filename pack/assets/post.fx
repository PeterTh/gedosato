// Post processing effect file
// most shader code taken from SweetFX 1.5.1

// -------------------- Settings -----------------------------------------------

// Set to 1 for ON or 0 for OFF
#define USE_BLOOM         0  // [0 or 1] Bloom : Makes bright lights bleed their light into their surroundings (relatively high performance cost)
#define USE_HDR           0  // [0 or 1] HDR : Not actual HDR - It just tries to mimic an HDR look (relatively high performance cost)
#define USE_LUMASHARPEN   0  // [0 or 1] LumaSharpen
#define USE_VIBRANCE      0  // [0 or 1] Vibrance : Intelligently saturates (or desaturates if you use negative values) the pixels depending on their original saturation.
#define USE_TONEMAP           0 //[0 or 1] Tonemap : Adjust gamma, exposure, saturation, bleach and defog. (may cause clipping)
#define USE_CA                0 //[0 or 1] Chromatic aberration. You can only use Chromatic Aberration OR the Explosion Shader. Not both at the same time.
#define USE_CURVES            0 //[0 or 1] Curves : Contrast adjustments using S-curves.
#define USE_SPLITSCREEN       0 //[0 or 1] Splitscreen : Enables the before-and-after splitscreen comparison mode.

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

// Tonemap settings
#define Gamma 1.0                      //[0.000 to 2.000] Adjust midtones
#define Exposure 0.0                     //[-1.000 to 1.000] Adjust exposure
#define Saturation 0.0                   //[-1.000 to 1.000] Adjust saturation
#define Bleach 0.0                       //[0.000 to 1.000] Brightens the shadows and fades the colors
#define Defog 1.0                       //[0.000 to 1.000] How much of the color tint to remove
#define FogColor float3(0.0, 0.0, 0.0) //[0.00 to 2.55, 0.00 to 2.55, 0.00 to 2.55] What color to remove - default is blue

// Chromatic Abberation settings
#define outfocus  0.016                     //[0.00 to 1.000] How strong the effect should be.

// Curves settings
#define Curves_mode     0   //[0|1|2] Choose what to apply contrast to. 0 = Luma, 1 = Chroma, 2 = both Luma and Chroma. Default is 0 (Luma)
#define Curves_contrast 1.0 //[-1.00 to 1.00] The amount of contrast you want

// -- Advanced curve settings --
#define Curves_formula     10 //[1|2|3|4|5|6|7|8|9|10] The contrast s-curve you want to use.
//1 = Sine, 2 = Abs split, 3 = Smoothstep, 4 = Exp formula, 5 = Simplified Catmull-Rom (0,0,1,1), 6 = Perlins Smootherstep
//Note that Technicolor Cinestyle is practically identical to Sine, but runs slower. In fact I think the difference might only be due to rounding errors.
//I prefer 2 myself, but 3 is a nice alternative with a little more effect (but harsher on the highlight and shadows) and it's the fastest formula.

// Splitscreen settings
#define splitscreen_mode   1  //[1|2|3|4|5|6]  1 = Vertical 50/50 split, 2 = Vertical 25/50/25 split, 3 = Vertical 50/50 angled split, 
// 4 = Horizontal 50/50 split, 5 = Horizontal 25/50/25 split, 6 = Curvy vertical 50/50 split

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

// -------------------------Tonemap--------------------------------------------

float4 TonemapPass(float4 colorInput)
{
    float3 color = colorInput.rgb;

    color = saturate(color - Defog * FogColor); // Defog
    
    color *= pow(2.0f, Exposure); // Exposure
    
    color = pow(color, Gamma);    // Gamma -- roll into the first gamma correction in main.h ?

    //#define BlueShift 0.00    //Blueshift
    //float4 d = color * float4(1.05f, 0.97f, 1.27f, color.a);
    //color = lerp(color, d, BlueShift);
    
    float3 lumCoeff = float3(0.2126, 0.7152, 0.0722);
    float lum = dot(lumCoeff, color.rgb);
    
    float3 blend = lum.rrr; //dont use float3
    
    float L = saturate( 10.0 * (lum - 0.45) );
    
    float3 result1 = 2.0f * color.rgb * blend;
    float3 result2 = 1.0f - 2.0f * (1.0f - blend) * (1.0f - color.rgb);
    
    float3 newColor = lerp(result1, result2, L);
    float A2 = Bleach * color.rgb; //why use a float for A2 here and then multiply by color.rgb (a float3)?
    //float3 A2 = Bleach * color.rgb; //
    float3 mixRGB = A2 * newColor;
    
    color.rgb += ((1.0f - A2) * mixRGB);
    
    //float3 middlegray = float(color.r + color.g + color.b) / 3;
    float3 middlegray = dot(color,(1.0/3.0)); //1fps slower than the original on nvidia, 2 fps faster on AMD
    
    float3 diffcolor = color - middlegray; //float 3 here
    colorInput.rgb = (color + diffcolor * Saturation)/(1+(diffcolor*Saturation)); //saturation
    
    return colorInput;
}


// ------------------------- Chromatic Abberation --------------------------------------------

#define CHROMA_POW		35.0
 
float3 fvChroma = float3(0.995, 1.000, 1.005);
 
float4 ChromaticAberrationFocus(float2 tex, float outOfFocus)
{
	float3 chroma = pow(fvChroma, CHROMA_POW * outOfFocus);

	float2 tr = ((2.0 * tex - 1.0) * chroma.r) * 0.5 + 0.5;
	float2 tg = ((2.0 * tex - 1.0) * chroma.g) * 0.5 + 0.5;
	float2 tb = ((2.0 * tex - 1.0) * chroma.b) * 0.5 + 0.5;
	
	float3 color = float3(tex2D(s0, tr).r, tex2D(s0, tg).g, tex2D(s0, tb).b) * (1.0 - outOfFocus);
	
	return float4(color, 1.0);
}

float4 CAPass(float4 colorInput, float2 tex)
{
	return ChromaticAberrationFocus(tex, outfocus);
}

// ------------------------- Curves --------------------------------------------

float4 CurvesPass( float4 colorInput )
{
  float3 lumCoeff = float3(0.2126, 0.7152, 0.0722);  //Values to calculate luma with
  float Curves_contrast_blend = Curves_contrast;
  float PI = acos(-1); //3.1415926589

// --------------------------------- Separation of Luma and Chroma

  // -- Calculate Luma and Chroma if needed --
  #if Curves_mode != 2

    //calculate luma (grey)
    float luma = dot(lumCoeff, colorInput.rgb);

    //calculate chroma
	  float3 chroma = colorInput.rgb - luma;
  #endif

  // -- Which value to put through the contrast formula? --
  // I name it x because makes it easier to copy-paste to Graphtoy or Wolfram Alpha or another graphing program
  #if Curves_mode == 2
	  float3 x = colorInput.rgb; //if the curve should be applied to both Luma and Chroma
	#elif Curves_mode == 1
	  float3 x = chroma; //if the curve should be applied to Chroma
	  x = x * 0.5 + 0.5; //adjust range of Chroma from -1 -> 1 to 0 -> 1
  #else // Curves_mode == 0
    float x = luma; //if the curve should be applied to Luma
  #endif

// --------------------------------- Contrast formulas

  // -- Curve 1 --
  #if Curves_formula == 1
    x = sin(PI * 0.5 * x); // Sin - 721 amd fps, +vign 536 nv
    x *= x;
    
    //x = 0.5 - 0.5*cos(PI*x);
    //x = 0.5 * -sin(PI * -x + (PI*0.5)) + 0.5;
  #endif

  // -- Curve 2 --
  #if Curves_formula == 2
    x = x - 0.5;  
    x = ( x / (0.5 + abs(x)) ) + 0.5;
    
    //x = ( (x - 0.5) / (0.5 + abs(x-0.5)) ) + 0.5;
  #endif

  // -- Curve 3 --
  #if Curves_formula == 3
    //x = smoothstep(0.0,1.0,x); //smoothstep
    x = x*x*(3.0-2.0*x); //faster smoothstep alternative - 776 amd fps, +vign 536 nv
    //x = x - 2.0 * (x - 1.0) * x* (x- 0.5);  //2.0 is contrast. Range is 0.0 to 2.0
  #endif

  // -- Curve 4 --
  #if Curves_formula == 4
    x = (1.0524 * exp(6.0 * x) - 1.05248) / (20.0855 + exp(6.0 * x)); //exp formula
  #endif

  // -- Curve 5 --
  #if Curves_formula == 5
    //x = 0.5 * (x + 3.0 * x * x - 2.0 * x * x * x); //a simplified catmull-rom (0,0,1,1) - btw smoothstep can also be expressed as a simplified catmull-rom using (1,0,1,0)
    //x = (0.5 * x) + (1.5 -x) * x*x; //estrin form - faster version
    x = x * (x * (1.5-x) + 0.5); //horner form - fastest version

    Curves_contrast_blend = Curves_contrast * 2.0; //I multiply by two to give it a strength closer to the other curves.
  #endif

 	// -- Curve 6 --
  #if Curves_formula == 6
    x = x*x*x*(x*(x*6.0 - 15.0) + 10.0); //Perlins smootherstep
	#endif

	// -- Curve 7 --
  #if Curves_formula == 7
    //x = ((x-0.5) / ((0.5/(4.0/3.0)) + abs((x-0.5)*1.25))) + 0.5;
	x = x - 0.5;
	x = x / ((abs(x)*1.25) + 0.375 ) + 0.5;
	//x = ( (x-0.5) / ((abs(x-0.5)*1.25) + (0.5/(4.0/3.0))) ) + 0.5;
  #endif

  // -- Curve 8 --
  #if Curves_formula == 8
    x = (x * (x * (x * (x * (x * (x * (1.6 * x - 7.2) + 10.8) - 4.2) - 3.6) + 2.7) - 1.8) + 2.7) * x * x; //Techicolor Cinestyle - almost identical to curve 1
  #endif

  // -- Curve 9 --
  #if Curves_formula == 9
    x =  -0.5 * (x*2.0-1.0) * (abs(x*2.0-1.0)-2.0) + 0.5; //parabola
  #endif

  // -- Curve 10 --
  #if Curves_formula == 10 //Half-circles

    #if Curves_mode == 0
      float xstep = step(x,0.5);
	    float xstep_shift = (xstep - 0.5);
	    float shifted_x = x + xstep_shift;
    #else
      float3 xstep = step(x,0.5);
	    float3 xstep_shift = (xstep - 0.5);
	    float3 shifted_x = x + xstep_shift;
    #endif

	x = abs(xstep - sqrt(-shifted_x * shifted_x + shifted_x) ) - xstep_shift;

  //x = abs(step(x,0.5)-sqrt(-(x+step(x,0.5)-0.5)*(x+step(x,0.5)-0.5)+(x+step(x,0.5)-0.5)))-(step(x,0.5)-0.5); //single line version of the above
    
  //x = 0.5 + (sign(x-0.5)) * sqrt(0.25-(x-trunc(x*2))*(x-trunc(x*2))); //worse
  
  /* // if/else - even worse
  if (x-0.5)
  x = 0.5-sqrt(0.25-x*x);
  else
  x = 0.5+sqrt(0.25-(x-1)*(x-1));
	*/

  //x = (abs(step(0.5,x)-clamp( 1-sqrt(1-abs(step(0.5,x)- frac(x*2%1)) * abs(step(0.5,x)- frac(x*2%1))),0 ,1))+ step(0.5,x) )*0.5; //worst so far
	
	//TODO: Check if I could use an abs split instead of step. It might be more efficient
	
	Curves_contrast_blend = Curves_contrast * 0.5; //I divide by two to give it a strength closer to the other curves.
  #endif

  // -- Curve 11 --
  #if Curves_formula == 11 //Cubic catmull
    float a = 1.00; //control point 1
    float b = 0.00; //start point
    float c = 1.00; //endpoint
    float d = 0.20; //control point 2
    x = 0.5 * ((-a + 3*b -3*c + d)*x*x*x + (2*a -5*b + 4*c - d)*x*x + (-a+c)*x + 2*b); //A customizable cubic catmull-rom spline
  #endif

  // -- Curve 12 --
  #if Curves_formula == 12 //Cubic Bezier spline
    float a = 0.00; //start point
    float b = 0.00; //control point 1
    float c = 1.00; //control point 2
    float d = 1.00; //endpoint

    float r  = (1-x);
	float r2 = r*r;
	float r3 = r2 * r;
	float x2 = x*x;
	float x3 = x2*x;
	//x = dot(float4(a,b,c,d),float4(r3,3*r2*x,3*r*x2,x3));

	//x = a * r*r*r + r * (3 * b * r * x + 3 * c * x*x) + d * x*x*x;
	//x = a*(1-x)*(1-x)*(1-x) +(1-x) * (3*b * (1-x) * x + 3 * c * x*x) + d * x*x*x;
	x = a*(1-x)*(1-x)*(1-x) + 3*b*(1-x)*(1-x)*x + 3*c*(1-x)*x*x + d*x*x*x;
  #endif

  // -- Curve 13 --
  #if Curves_formula == 13 //Cubic Bezier spline - alternative implementation.
    float3 a = float3(0.00,0.00,0.00); //start point
    float3 b = float3(0.25,0.15,0.85); //control point 1
    float3 c = float3(0.75,0.85,0.15); //control point 2
    float3 d = float3(1.00,1.00,1.00); //endpoint

    float3 ab = lerp(a,b,x);           // point between a and b
    float3 bc = lerp(b,c,x);           // point between b and c
    float3 cd = lerp(c,d,x);           // point between c and d
    float3 abbc = lerp(ab,bc,x);       // point between ab and bc
    float3 bccd = lerp(bc,cd,x);       // point between bc and cd
    float3 dest = lerp(abbc,bccd,x);   // point on the bezier-curve
    x = dest;
  #endif

  // -- Curve 14 --
  #if Curves_formula == 14
    x = 1.0 / (1.0 + exp(-(x * 10.0 - 5.0))); //alternative exp formula
  #endif

// ---------------------------------Joining of Luma and Chroma

  #if Curves_mode == 2 //Both Luma and Chroma
	float3 color = x;  //if the curve should be applied to both Luma and Chroma
	colorInput.rgb = lerp(colorInput.rgb, color, Curves_contrast_blend); //Blend by Curves_contrast

  #elif Curves_mode == 1 //Only Chroma
	x = x * 2.0 - 1.0; //adjust the Chroma range back to -1 -> 1
	float3 color = luma + x; //Luma + Chroma
	colorInput.rgb = lerp(colorInput.rgb, color, Curves_contrast_blend); //Blend by Curves_contrast

  #else // Curves_mode == 0 //Only Luma
    x = lerp(luma, x, Curves_contrast_blend); //Blend by Curves_contrast
    colorInput.rgb = x + chroma; //Luma + Chroma

  #endif

  //Return the result
  return colorInput;
}

// ------------------------- Splitscreen --------------------------------------------

float4 SplitscreenPass( float4 colorInput, float2 tex )
{
  // -- Vertical 50/50 split --
  #if splitscreen_mode == 1
	  return (tex.x < 0.5) ? tex2D(s0, tex) : colorInput;
  #endif

  // -- Vertical 25/50/25 split --
	#if splitscreen_mode == 2
    //Calculate the distance from center
    float distance = abs(tex.x - 0.5);
    
    //Further than 1/4 away from center?
    distance = saturate(distance - 0.25);
    
    return distance ? tex2D(s0, tex) : colorInput;
	#endif

  // -- Vertical 50/50 angled split --
	#if splitscreen_mode == 3
	  //Calculate the distance from center
    float distance = ((tex.x - 3.0/8.0) + (tex.y * 0.25));
    
    //Further than 1/4 away from center?
    distance = saturate(distance - 0.25);
    
    return distance ? colorInput : tex2D(s0, tex);
	#endif
  
  // -- Horizontal 50/50 split --
  #if splitscreen_mode == 4
	  return (tex.y < 0.5) ? tex2D(s0, tex) : colorInput;
  #endif
	
  // -- Horizontal 25/50/25 split --
  #if splitscreen_mode == 5
    //Calculate the distance from center
    float distance = abs(tex.y - 0.5);
    
    //Further than 1/4 away from center?
    distance = saturate(distance - 0.25);
    
    return distance ? tex2D(s0, tex) : colorInput;
  #endif

  // -- Vertical 50/50 curvy split --
    #if splitscreen_mode == 6
    //Calculate the distance from center
    float distance = (tex.x - 0.25) + (sin(tex.y * 10)*0.10);
    
    //Further than 1/4 away from center?
    distance = saturate(distance - 0.25);
    
    return distance ? colorInput : tex2D(s0, tex);
    #endif

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

#if (USE_TONEMAP == 1)
    c0 = TonemapPass(c0);
#endif

#if (USE_CA == 1)
    c0 = CAPass(c0, tex);
#endif

#if (USE_CURVES == 1)
    c0 = CurvesPass(c0);
#endif

//#if (USE_DITHER == 1)
//    c0 = DPass(c0, tex);
//#endif

#if (USE_SPLITSCREEN == 1)
    c0 = SplitscreenPass(c0, tex);
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