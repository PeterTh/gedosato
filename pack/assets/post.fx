// Post processing effect file
// most shader code taken from SweetFX 1.5.1

// -------------------- Settings -----------------------------------------------

// Set to 1 for ON or 0 for OFF
#define USE_BLOOM             0 //[0 or 1] Bloom : Makes bright lights bleed their light into their surroundings (relatively high performance cost)
#define USE_HDR               0 //[0 or 1] HDR : Not actual HDR - It just tries to mimic an HDR look (relatively high performance cost)
#define USE_LUMASHARPEN       0 //[0 or 1] LumaSharpen
#define USE_VIBRANCE          0 //[0 or 1] Vibrance : Intelligently saturates (or desaturates if you use negative values) the pixels depending on their original saturation.
#define USE_TONEMAP           0 //[0 or 1] Tonemap : Adjust gamma, exposure, saturation, bleach and defog. (may cause clipping)
#define USE_CA                0 //[0 or 1] Chromatic aberration. You can only use Chromatic Aberration OR the Explosion Shader. Not both at the same time.
#define USE_CURVES            0 //[0 or 1] Curves : Contrast adjustments using S-curves.
#define USE_DITHER            0 //[0 or 1] Dither : Applies dithering to simulate more colors than your monitor can display. This lessens banding artifacts (mostly caused by Vignette)
#define USE_ADVANCED_CRT      0 //[0 or 1] Advanced CRT : Simulates an old CRT TV display. Set gaussian blur along with it to get a halation effect
#define USE_TECHNICOLOR       0 //[0 or 1] TECHNICOLOR : Attempts to mimic the look of an old movie using the Technicolor three-strip color process (Techicolor Process 4)
#define USE_DPX               0 //[0 or 1] Cineon DPX : Should make the image look like it's been converted to DXP Cineon - basically it's another movie-like look similar to technicolor.
#define USE_GRAIN           0 //[0 or 1] Film Grain: Adds an active graininess to the image.  Helps with banding and can add the illusion of detail
#define USE_BORDER            0 //[0 or 1] Border : Makes the screenedge black as a workaround for the bright edge that forcing some AA modes sometimes causes.
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
	#define Exposure 0.0                   //[-1.000 to 1.000] Adjust exposure
	#define Saturation 0.0                 //[-1.000 to 1.000] Adjust saturation
	#define Bleach 0.0                     //[0.000 to 1.000] Brightens the shadows and fades the colors
	#define Defog 1.0                      //[0.000 to 1.000] How much of the color tint to remove
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

// Dither settings
	#define dither_method      2  //[1 or 2] 1 = Ordered dithering (very good and very fast), 2 = Random dithering (different but slightly slower dithering)
		//Note that the patterns used by Dither, makes an image harder to compress.
		//This can make your screenshots and video recordings take up more space.

// Advanced CRT settings
	#define CRTAmount            1.00    //[0.00 to 1.00]  Amount of CRT effect you want

	#define CRTResolutionX       (SCREEN_SIZE.x)  / 4    //[1 to 2048]     Original input width of the game.  Default is 1/4 horizontal screen resolution
	#define CRTResolutionY       (SCREEN_SIZE.y) / 4    //[1 to 2048]     Original input height of the game.  Default is 1/4 vertical screen resolution
	#define CRTgamma             2.2     //[0.0 to 4.0]    Gamma of simulated CRT (default 2.2)
	#define CRTmonitorgamma      2.4     //[0.0 to 4.0]    Gamma of display monitor (typically 2.2 is correct)
	#define CRTBrightness        1.2     //[1.0 to 3.0]    Used to boost brightness a little. Default is 1.0
	#define CRTScanlineIntensity 2.0     //[2.0 to 4.0]    Scanlines intensity (use integer values preferably). Default is 2.0
	#define CRTScanlineGaussian  1       //[0 or 1]        Use the "new nongaussian scanlines bloom effect". Default is on

	#define CRTCurvature         1       //[[0 or 1]          "Barrel effect" enabled (1) or off (0)
	#define CRTCurvatureRadius   2.0     //[0.0 to 2.0]       Curvature Radius (only effective when Curvature is enabled). Default is 1.5
	#define CRTCornerSize        0.0100  //[0.0000 to 0.0020] Higher values, more rounded corner. Default is 0.001
	#define CRTDistance          2.00    //[0.00 to 4.00]     Simulated distance from viewer to monitor. Default is 2.00
	#define CRTAngleX            0.00    //[-0.20 to 0.20]    Tilt angle in radians (X coordinates)
	#define CRTAngleY           -0.15    //[-0.20 to 0.20]    Tilt angle in radians (Y coordinates). (Value of -0.15 gives the 'arcade tilt' look)
	#define CRTOverScan          1.00    //[1.00 to 1.10]     Overscan (e.g. 1.02 for 2% overscan). Default is 1.01
	#define CRTOversample        0       //[0 or 1]           Enable 3x oversampling of the beam profile (warning : performance hit)
	
// ----Advanced Advanced CRT settings
	//Amount of R,G,B tint for even scanlines, and amount of R,G,B tint for the odd scanlines. 1.0 is unchanged color
	#define R1 			1		//[0.0 to 1.0]   Amount of red color for even scanlines
	#define G1 			0.7		//[0.0 to 1.0]   Amount of green color for even scanlines
	#define B1 			1		//[0.0 to 1.0]   Amount of blue color for even scanlines
	
	#define R2 			0.7		//[0.0 to 1.0]   Amount of red color for odd scanlines
	#define G2 			1.0		//[0.0 to 1.0]   Amount of green color for odd scanlines
	#define B2 			0.7		//[0.0 to 1.0]   Amount of blue color for odd scanlines
	
// Technicolor settings
	#define TechniAmount 0.46        //[0.00 to 1.00]
	#define TechniPower  4.0         //[0.00 to 8.00]
	#define redNegativeAmount   0.74 //[0.00 to 1.00]
	#define greenNegativeAmount 0.83 //[0.00 to 1.00]
	#define blueNegativeAmount  0.9  //[0.00 to 1.00]
	
// Cineon DPX settings
	#define Red   9.0  //[1.0 to 15.0]
	#define Green 9.0  //[1.0 to 15.0]
	#define Blue  9.0  //[1.0 to 15.0]

	#define ColorGamma    1.0  //[0.1 to 2.5] Adjusts the colorfulness of the effect in a manner similar to Vibrance. 1.0 is neutral.
	#define DPXSaturation 1.0  //[0.0 to 8.0] Adjust saturation of the effect. 1.0 is neutral.

	#define RedC   0.33  //[0.60 to 0.20]
	#define GreenC 0.36  //[0.60 to 0.20]
	#define BlueC  0.35  //[0.60 to 0.20]

	#define Blend 0.23    //[0.00 to 1.00] How strong the effect should be
	
// Grain Settings
	#define GrainPower   0.05 //[0.00 to 1.00] Intensity of applied grain
	#define GrainColored   0 //[0 or 1] Whether grain should be colored or not
	#define GrainColorAmount   0.6//[0.00 to 1.00] Amount of color to add to grain
	#define GrainSize   1.6//[1.50 to 2.50] Size of individual pieces of grain
	#define GrainLuma   1.0//[0.00 to 1.00] Grain brightness
	
// Border settings
	#define border_width float2(0,0)		//[0 to 2048, 0 to 2048] (X,Y)-width of the border. Measured in pixels. If this is set to 0,0 then the border_ratio will be used instead
	#define border_ratio float(0.75 / 1.0)	//[0.1000 to 10.0000] Set the desired ratio for the visible area. You MUST use floating point - Integers do not work right.
	//Examples that work: (1680.0 / 1050.0), (16.0 / 10.0), (1.6) Examples that does NOT work right: (1680 / 1050), (16 / 10)
	#define border_color float3(0, 0, 0)	//[0 to 255, 0 to 255, 0 to 255] What color the border should be. In integer RGB colors, meaning 0,0,0 is black and 255,255,255 is full white.

// Splitscreen settings
	#define splitscreen_mode   1  //[1|2|3|4|5|6]  1 = Vertical 50/50 split, 2 = Vertical 25/50/25 split, 3 = Vertical 50/50 angled split, 
		// 4 = Horizontal 50/50 split, 5 = Horizontal 25/50/25 split, 6 = Curvy vertical 50/50 split

// -------------------- Interface -----------------------------------------------

texture2D thisframeTex;

static float2 rcpres = PIXEL_SIZE;

const float timer;

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
    //float A2 = Bleach * color.rgb; //why use a float for A2 here and then multiply by color.rgb (a float3)?
    float3 A2 = Bleach * color.rgb; //
    float3 mixRGB = (A2 * newColor.r, A2 * newColor.g, A2*newColor.b);
    
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

// -------------------------Dither--------------------------------------------

#ifndef dither_method
  #define dither_method 1
#endif

float4 DitherPass( float4 colorInput, float2 tex )
{
   float3 color = colorInput.rgb;

   float dither_bit  = 8.0;  //Number of bits per channel. Should be 8 for most monitors.
   
   //color = (tex.x*0.3+0.1); //draw a gradient for testing.
   //#define dither_method 2 //override method for testing purposes

  /*------------------------.
  | :: Ordered Dithering :: |
  '------------------------*/
   #if dither_method == 1 // Ordered dithering
     //Calculate grid position
     float grid_position = frac( dot(tex, (SCREEN_SIZE * float2(1.0/16.0,10.0/36.0)  )+(0.25) ) );

     //Calculate how big the shift should be
     float dither_shift = (0.25) * (1.0 / (pow(2,dither_bit) - 1.0));

     //Shift the individual colors differently, thus making it even harder to see the dithering pattern
     float3 dither_shift_RGB = float3(dither_shift, -dither_shift, dither_shift); //subpixel dithering

     //modify shift acording to grid position.
     dither_shift_RGB = lerp(2.0 * dither_shift_RGB, -2.0 * dither_shift_RGB, grid_position); //shift acording to grid position.

     //shift the color by dither_shift
     color.rgb += dither_shift_RGB;


  /*-----------------------.
  | :: Random Dithering :: |
  '-----------------------*/
   #elif dither_method == 2 //Random dithering

     //Pseudo Random Number Generator
     // -- PRNG 1 - Reference --
     float seed = dot(tex, float2(12.9898,78.233)); //I could add more salt here if I wanted to
     float sine = sin(seed); //cos also works well. Sincos too if you want 2D noise.
     float noise = frac(sine * 43758.5453 + tex.x); //tex.x is just some additional salt - it can be taken out.

     //Calculate how big the shift should be
     float dither_shift = (1.0 / (pow(2,dither_bit) - 1.0)); // Using noise to determine shift. Will be 1/255 if set to 8-bit.
     float dither_shift_half = (dither_shift * 0.5); // The noise should vary between +- 0.5
     dither_shift = dither_shift * noise - dither_shift_half; // MAD

     //shift the color by dither_shift
     color.rgb += float3(-dither_shift, dither_shift, -dither_shift); //subpixel dithering

  /*--------------------.
  | :: New Dithering :: |
  '--------------------*/
  //#define dither_method 3
   #elif dither_method == 3 // New Ordered dithering

     //Calculate grid position
     float grid_position = frac(dot(tex,(SCREEN_SIZE) * float2(0.75,0.5) /*+ (0.00025)*/)); //(0.6,0.8) is good too - TODO : experiment with values

     //Calculate how big the shift should be
     float dither_shift = (0.25) * (1.0 / (pow(2,dither_bit) - 1.0)); // 0.25 seems good both when using math and when eyeballing it. So does 0.75 btw.
     dither_shift = lerp(2.0 * dither_shift, -2.0 * dither_shift, grid_position); //shift acording to grid position.

     //shift the color by dither_shift
     color.rgb += float3(dither_shift, -dither_shift, dither_shift); //subpixel dithering

  /*-------------------------.
  | :: A Dither Dithering :: |
  '-------------------------*/
  //#define dither_method 4
  #elif dither_method == 4 // New Ordered dithering

  #define dither_pattern 11
  #define dither_levels 254

  float x=tex.x * SCREEN_SIZE.x;// * 1.31;
  float y=tex.y * SCREEN_SIZE.y;// * 1.31;

  //Calculate grid position
  float c = frac(dot(tex,(SCREEN_SIZE) * float2(1.0/4.0,3.0/4.0) + (0.00025) )); //the + (0.00025) part is to avoid errors with the floating point math

  float mask;

  #if dither_pattern == 1
    mask = ((x ^ y * 149) * 1234 & 511)/511.0; //requires bitwise XOR - doesn't work
  #elif dither_pattern == 2
    mask = (((x+c*17) ^ y * 149) * 1234 & 511)/511.0; //requires bitwise XOR - doesn't work
  #elif dither_pattern == 3
    mask = 256.0 * frac(((x + y * 237) * 119)/ 256.0 ) / 255.0 ;//1.00392 * frac(0.464844 * (x + 237.0 * y)); //256.0 * frac(((x + y * 237) * 119)/ 256.0 ) / 255.0
  #elif dither_pattern == 4
    mask = (256.0 * frac((((x+c*67.0) + y * 236.0) * 119.0) / 256.0)) / 255.0; //& 255 = 256 * frac(x / 256)
  #elif dither_pattern == 5
    mask = 0.5;
  #elif dither_pattern == 6
    mask = frac( dot(tex, float2(12.9898,78.233)) * 927.5453 );
  #elif dither_pattern == 7
    mask = frac( dot(tex, (SCREEN_SIZE * float2(1.0/7.0,9.0/17.0))+(0.00025) ) );
  #elif dither_pattern == 8
    mask = frac( dot(tex, (SCREEN_SIZE * float2(5.0/7.0,3.0/17.0))+(0.00025) ) );
  #elif dither_pattern == 9
    mask = frac( dot(tex, (SCREEN_SIZE * float2(1.0/4.0,3.0/5.0))+(0.000025) ) );
  #elif dither_pattern == 10
    mask = frac( dot(tex, (SCREEN_SIZE * float2(1.0/87.0,1.0/289.0))+(0.000025) ) ); //stylish pattern - but bad for dithering
  #elif dither_pattern == 11
    //mask = frac( dot(tex, (SCREEN_SIZE * float2(1.0/(floor(tex.y*10.0)/100.+16.0),87.0/289.0))+(0.000025) ) ); //
	//mask = frac( dot(float4(tex,tex), float4((SCREEN_SIZE * float2(0.666/16.0,6.66/36.)),(SCREEN_SIZE * float2(0.3344/16.0,3.34/36.)) ) ) ); //
      mask = frac( dot(tex, (SCREEN_SIZE * float2(1.0/16.0,10.0/36.0)  )+(0.25) ) ); //
//(floor(tex.y*10.0)/100.0 + 3.0)
  #else
    //return input;
  #endif

  color.rgb = floor(dither_levels * color.rgb + mask) / dither_levels;
  color.rgb = mask.xxx;

  /*---------------------------------------.
  | :: New Dithering - grid experiments :: |
  '---------------------------------------*/
  //#define dither_method 5
   #elif dither_method == 5 // New Ordered dithering

     //Calculate grid position
     float grid_position = frac(dot(tex,floor(SCREEN_SIZE * float2(-0.5,-0.9) ) /*- (0.00025)*/ )); //(0.6,0.8) is good too - TODO : experiment with values

     //Calculate grid position
     grid_position = frac(dot(tex,floor(SCREEN_SIZE * float2(0.4,0.70)) /*+ grid_position*/ /*+ (0.00025)*/ )); //

     //Calculate how big the shift should be
     float dither_shift = (0.25) * (1.0 / (pow(2,dither_bit) - 1.0)); // 0.25 seems good both when using math and when eyeballing it. So does 0.75 btw.
     dither_shift = lerp(2.0 * dither_shift, -2.0 * dither_shift, grid_position); //shift acording to grid position.

     //dither_shift = (2.0 * dither_shift) * grid_position + (2.0 * dither_shift) * grid_position;
     //dither_shift = 4.0 * dither_shift * grid_position;

     //shift the color by dither_shift
     //color.rgb += lerp(2.0 * dither_shift_RGB, -2.0 * dither_shift_RGB, grid_position); //shift acording to grid position.

     color.rgb += float3(dither_shift, -dither_shift, dither_shift); //subpixel dithering

  /*-------------------.
  | :: Checkerboard :: |
  '-------------------*/
   #elif dither_method == 6 // Checkerboard Ordered dithering
     //Calculate grid position
     float grid_position = frac(dot(tex, SCREEN_SIZE * 0.5) + 0.25); //returns 0.25 and 0.75

     //Calculate how big the shift should be
     float dither_shift = (0.25) * (1.0 / (pow(2,dither_bit) - 1.0)); // 0.25 seems good both when using math and when eyeballing it. So does 0.75 btw.

     //Shift the individual colors differently, thus making it even harder to see the dithering pattern
     float3 dither_shift_RGB = float3(dither_shift, -dither_shift, dither_shift); //subpixel dithering

     //modify shift acording to grid position.
     dither_shift_RGB = lerp(2.0 * dither_shift_RGB, -2.0 * dither_shift_RGB, grid_position); //shift acording to grid position.

     //shift the color by dither_shift
     //color.rgb += lerp(2.0 * dither_shift_RGB, -2.0 * dither_shift_RGB, grid_position); //shift acording to grid position.
     color.rgb += dither_shift_RGB;
	
   #endif

  /*---------------------------.
  | :: Returning the output :: |
  '---------------------------*/

   //color = (tex.x / 2.0); //draw a undithered gradient for testing.

   colorInput.rgb = color.rgb;

   return colorInput;
}

// CRT shader
// 
// Copyright (C) 2010-2012 cgwg, Themaister and DOLLS
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.

// Comment the next line to disable interpolation in linear gamma (and gain speed).
//#define LINEAR_PROCESSING

// aspect ratio
#define aspect float2(1.0, 0.75)

// Precalculate a bunch of useful values we'll need in the fragment
// shader.
#define sinangle sin(float2(CRTAngleX, CRTAngleY))
#define cosangle cos(float2(CRTAngleX, CRTAngleY))
#define stretch maxscale()

// Macros.
#define FIX(c) max(abs(c), 1e-5);
float PI = acos(-1); //#define PI 3.141592653589

// The size of one texel, in texture-coordinates.
#define coone 1.0 / rubyTextureSize

#define mod_factor tex.x * rubyTextureSize.x * rubyOutputSize.x / rubyInputSize.x

#ifdef LINEAR_PROCESSING
#       define TEX2D(c) pow(tex2D(s0, (c)), CRTgamma)
#else
#       define TEX2D(c) tex2D(s0, (c))
#endif

float intersect(float2 xy)
{
  float A = dot(xy,xy) + CRTDistance * CRTDistance;
  float B = 2.0 * (CRTCurvatureRadius * (dot(xy, sinangle) - CRTDistance * cosangle.x * cosangle.y) - CRTDistance * CRTDistance);
  float C = CRTDistance * CRTDistance + 2.0 * CRTCurvatureRadius * CRTDistance * cosangle.x * cosangle.y;
  return (-B - sqrt(B * B -4.0 * A * C)) / (2.0 * A);
}

float2 bkwtrans(float2 xy)
{
  float c = intersect(xy);
  float2 _point = float2(c, c) * xy;
  _point -= float2(-CRTCurvatureRadius, -CRTCurvatureRadius) * sinangle;
  _point /= float2(CRTCurvatureRadius, CRTCurvatureRadius);
  float2 tang = sinangle / cosangle;
  float2 poc = _point / cosangle;
  float A = dot(tang, tang) + 1.0;
  float B = -2.0 * dot(poc, tang);
  float C = dot(poc, poc) - 1.0;
  float a = (-B + sqrt(B * B -4.0 * A * C)) / (2.0 * A);
  float2 uv = (_point - a * sinangle) / cosangle;
  float r = FIX(CRTCurvatureRadius * acos(a));
  return uv * r / sin(r / CRTCurvatureRadius);
}

float2 fwtrans(float2 uv)
{
  float r = FIX(sqrt(dot(uv, uv)));
  uv *= sin(r / CRTCurvatureRadius) / r;
  float x = 1.0 - cos(r / CRTCurvatureRadius);
  float D = CRTDistance / CRTCurvatureRadius + x * cosangle.x * cosangle.y + dot(uv, sinangle);
  return CRTDistance * (uv * cosangle - x * sinangle) / D;
}

float3 maxscale()
{
  float2 c = bkwtrans(-CRTCurvatureRadius * sinangle / (1.0 + CRTCurvatureRadius / CRTDistance * cosangle.x * cosangle.y));
  float2 a = float2(0.5, 0.5) * aspect;
  float2 lo = float2(fwtrans(float2(-a.x, c.y)).x,
		             fwtrans(float2(c.x,-a.y)).y) / aspect;
  float2 hi = float2(fwtrans(float2(+a.x, c.y)).x,
		             fwtrans(float2(c.x, +a.y)).y) / aspect;
  return float3((hi + lo) * aspect * 0.5, max(hi.x - lo.x, hi.y - lo.y));
}

float2 transform(float2 coord, float2 textureSize, float2 inputSize)
{
  coord *= textureSize / inputSize;
  coord = (coord - 0.5) * aspect * stretch.z + stretch.xy;
  return (bkwtrans(coord) / float2(CRTOverScan, CRTOverScan) / aspect + 0.5) * inputSize / textureSize;
}

float corner(float2 coord, float2 textureSize, float2 inputSize)
{
  coord *= textureSize / inputSize;
  coord = (coord - 0.5) * float2(CRTOverScan, CRTOverScan) + 0.5;
  coord = min(coord, 1.0 - coord) * aspect;
  float2 cdist = float2(CRTCornerSize, CRTCornerSize);
  coord = (cdist - min(coord, cdist));
  float dist = sqrt(dot(coord, coord));
  return clamp((cdist.x-dist) * 1000.0, 0.0, 1.0);
}

// Calculate the influence of a scanline on the current pixel.
//
// 'distance' is the distance in texture coordinates from the current
// pixel to the scanline in question.
// 'color' is the colour of the scanline at the horizontal location of
// the current pixel.
float4 scanlineWeights(float distance, float4 color)
{
  // "wid" controls the width of the scanline beam, for each RGB channel
  // The "weights" lines basically specify the formula that gives
  // you the profile of the beam, i.e. the intensity as
  // a function of distance from the vertical center of the
  // scanline. In this case, it is gaussian if width=2, and
  // becomes nongaussian for larger widths. Ideally this should
  // be normalized so that the integral across the beam is
  // independent of its width. That is, for a narrower beam
  // "weights" should have a higher peak at the center of the
  // scanline than for a wider beam.
#if CRTScanlineGaussian == 0
	float4 wid = 0.3 + 0.1 * pow(color, 3.0);
	float4 weights = float4(distance / wid);
	return 0.4 * exp(-weights * weights) / wid;
#else
    float4 wid = 2.0 + 2.0 * pow(color, 4.0);
	float calcdistance = distance / 0.3; // Optimization  ?
    //float4 weights = float4(distance / 0.3, distance / 0.3, distance / 0.3, distance / 0.3);
    float4 weights = float4(calcdistance, calcdistance, calcdistance, calcdistance);
    return 1.4 * exp(-pow(weights * rsqrt(0.5 * wid), wid)) / (0.6 + 0.2 * wid);
#endif
}

float4 AdvancedCRTPass( float4 colorInput, float2 tex )
{
	// Here's a helpful diagram to keep in mind while trying to
	// understand the code:
	//
	//  |      |      |      |      |
	// -------------------------------
	//  |      |      |      |      |
	//  |  01  |  11  |  21  |  31  | <-- current scanline
	//  |      | @    |      |      |
	// -------------------------------
	//  |      |      |      |      |
	//  |  02  |  12  |  22  |  32  | <-- next scanline
	//  |      |      |      |      |
	// -------------------------------
	//  |      |      |      |      |
	//
	// Each character-cell represents a pixel on the output
	// surface, "@" represents the current pixel (always somewhere
	// in the bottom half of the current scan-line, or the top-half
	// of the next scanline). The grid of lines represents the
	// edges of the texels of the underlying texture.

	float2 Resolution = float2(CRTResolutionX, CRTResolutionY);
	float2 rubyTextureSize = Resolution;
	float2 rubyInputSize = Resolution;
	float2 rubyOutputSize = SCREEN_SIZE;
	
#if CRTCurvature == 1
    float2 xy = transform(tex, rubyTextureSize, rubyInputSize);
#else
    float2 xy = tex;
#endif
	float cval = corner(xy, rubyTextureSize, rubyInputSize);
  	
    // Of all the pixels that are mapped onto the texel we are
    // currently rendering, which pixel are we currently rendering?
    float2 ratio_scale = xy * rubyTextureSize - 0.5;
	
#if CRTOversample == 1
	float filter = fwidth(ratio_scale.y);
#endif	
	float2 uv_ratio = frac(ratio_scale);

    // Snap to the center of the underlying texel.
    xy = (floor(ratio_scale) + 0.5) / rubyTextureSize;

	// Calculate Lanczos scaling coefficients describing the effect
	// of various neighbour texels in a scanline on the current
	// pixel.
	float4 coeffs = PI * float4(1.0 + uv_ratio.x, uv_ratio.x, 1.0 - uv_ratio.x, 2.0 - uv_ratio.x);

	// Prevent division by zero.
	coeffs = FIX(coeffs);

	// Lanczos2 kernel.
	coeffs = 2.0 * sin(coeffs) * sin(coeffs / 2.0) / (coeffs * coeffs);

	// Normalize.
	coeffs /= dot(coeffs, 1.0);
	
	// Calculate the effective colour of the current and next
	// scanlines at the horizontal location of the current pixel,
	// using the Lanczos coefficients above.
	float4 col  = clamp(mul(coeffs, float4x4(
			TEX2D(xy + float2(-coone.x, 0.0)),
			TEX2D(xy),
			TEX2D(xy + float2(coone.x, 0.0)),
			TEX2D(xy + float2(2.0 * coone.x, 0.0)))),
			0.0, 1.0);
	float4 col2 = clamp(mul(coeffs, float4x4(
			TEX2D(xy + float2(-coone.x, coone.y)),
			TEX2D(xy + float2(0.0, coone.y)),
			TEX2D(xy + coone),
			TEX2D(xy + float2(2.0 * coone.x, coone.y)))),
			0.0, 1.0);

#ifndef LINEAR_PROCESSING
    col  = pow(col , CRTgamma);
    col2 = pow(col2, CRTgamma);
#endif

	// Calculate the influence of the current and next scanlines on
	// the current pixel.
	float4 weights  = scanlineWeights(uv_ratio.y, col);
	float4 weights2 = scanlineWeights(1.0 - uv_ratio.y, col2);
	
#if CRTOversample == 1
    uv_ratio.y = uv_ratio.y + 1.0 / 3.0 * filter;
    weights = (weights + scanlineWeights(uv_ratio.y, col)) / 3.0;
    weights2 = (weights2 + scanlineWeights(abs(1.0 - uv_ratio.y), col2)) / 3.0;
    uv_ratio.y = uv_ratio.y - 2.0 / 3.0 * filter;
    weights = weights + scanlineWeights(abs(uv_ratio.y), col) / 3.0;
    weights2 = weights2 + scanlineWeights(abs(1.0 - uv_ratio.y), col2) / 3.0;
#endif
	
	float3 mul_res  = (col * weights + col2 * weights2).rgb * float3(cval, cval, cval);

	// dot-mask emulation:
	// Output pixels are alternately tinted green and magenta.
	float3 dotMaskWeights = lerp(float3(R1, G1, B1), 
                                 float3(R2, G2, B2), 
                                 floor(mod_factor % CRTScanlineIntensity));

	mul_res *= dotMaskWeights * float3(0.83, 0.83, 0.83) * CRTBrightness;

	// Convert the image gamma for display on our output device.
	mul_res = pow(mul_res, 1.0 / CRTmonitorgamma);

	//return saturate(lerp(colorInput, float4(mul_res, 1.0), CRTAmount));
	colorInput.rgb = lerp(colorInput.rgb, mul_res, CRTAmount);
	return saturate(colorInput);
}

// ------------------------- Technicolor --------------------------------------------

#define cyanfilter float3(0.0, 1.30, 1.0)
#define magentafilter float3(1.0, 0.0, 1.05) 
#define yellowfilter float3(1.6, 1.6, 0.05)

#define redorangefilter float2(1.05, 0.620) //RG_
#define greenfilter float2(0.30, 1.0)       //RG_
#define magentafilter2 magentafilter.rb     //R_B

float4 TechnicolorPass( float4 colorInput )
{
	float3 tcol = colorInput.rgb;
	
  float2 rednegative_mul   = tcol.rg * (1.0 / (redNegativeAmount * TechniPower));
	float2 greennegative_mul = tcol.rg * (1.0 / (greenNegativeAmount * TechniPower));
	float2 bluenegative_mul  = tcol.rb * (1.0 / (blueNegativeAmount * TechniPower));
	
  float rednegative   = dot( redorangefilter, rednegative_mul );
	float greennegative = dot( greenfilter, greennegative_mul );
	float bluenegative  = dot( magentafilter2, bluenegative_mul );
	
	float3 redoutput   = rednegative.rrr + cyanfilter;
	float3 greenoutput = greennegative.rrr + magentafilter;
	float3 blueoutput  = bluenegative.rrr + yellowfilter;
	
	float3 result = redoutput * greenoutput * blueoutput;
	colorInput.rgb = lerp(tcol, result, TechniAmount);
	return colorInput;
}

// ------------------------- DPX/Cineon --------------------------------------------

static float3x3 RGB =
{
2.67147117265996,-1.26723605786241,-0.410995602172227,
-1.02510702934664,1.98409116241089,0.0439502493584124,
0.0610009456429445,-0.223670750812863,1.15902104167061
};

static float3x3 XYZ =
{
0.500303383543316,0.338097573222739,0.164589779545857,
0.257968894274758,0.676195259144706,0.0658358459823868,
0.0234517888692628,0.1126992737203,0.866839673124201
};

float4 DPXPass(float4 InputColor) : COLOR0 {

	float DPXContrast = 0.1;

	float DPXGamma = 1.0;

	float RedCurve = Red;
	float GreenCurve = Green;
	float BlueCurve = Blue;

	float3 B = InputColor.rgb;
	//float3 Bn = B; // I used InputColor.rgb instead.

	B = pow(B, 1.0/DPXGamma);

	B.r = pow(B.r, 1.00);
	B.g = pow(B.g, 1.00);
	B.b = pow(B.b, 1.00);

        B = (B * (1.0 - DPXContrast)) + DPXContrast / 2.0;
 	
 	B.r = (1.0 /(1.0 + exp(- RedCurve * (B.r - RedC))) - (1.0 / (1.0 + exp(RedCurve / 2.0))))/(1.0 - 2.0 * (1.0 / (1.0 + exp(RedCurve / 2.0))));				
	B.g = (1.0 /(1.0 + exp(- GreenCurve * (B.g - GreenC))) - (1.0 / (1.0 + exp(GreenCurve / 2.0))))/(1.0 - 2.0 * (1.0 / (1.0 + exp(GreenCurve / 2.0))));				
	B.b = (1.0 /(1.0 + exp(- BlueCurve * (B.b - BlueC))) - (1.0 / (1.0 + exp(BlueCurve / 2.0))))/(1.0 - 2.0 * (1.0 / (1.0 + exp(BlueCurve / 2.0))));					

        //TODO use faster code for conversion between RGB/HSV  -  see http://www.chilliant.com/rgb2hsv.html
	   float value = max(max(B.r, B.g), B.b);
	   float3 color = B / value;
	
	   color = pow(color, 1.0/ColorGamma);
	
	   float3 c0 = color * value;

	   c0 = mul(XYZ, c0);

	   float luma = dot(c0, float3(0.30, 0.59, 0.11)); //Use BT 709 instead?
	   float3 chroma = c0 - luma;

	   c0 = luma + chroma * DPXSaturation;
	   c0 = mul(RGB, c0);
	
	InputColor.rgb = lerp(InputColor.rgb, c0, Blend); //as long as Blend is always 0 we don't really need to lerp. The compiler *should* be smart enough to optimize this though (check to be sure)

	return InputColor;
}

/* ------------------------- Grain --------------------------------------------

Film Grain post-process shader v1.1	
Martins Upitis (martinsh) devlog-martinsh.blogspot.com
2013
--------------------------
This work is licensed under a Creative Commons Attribution 3.0 Unported License.
So you are free to share, modify and adapt it for your needs, and even use it for commercial use.
I would also love to hear about a project you are using it.

Have fun,
Martins
---------------------------
Ported to GeDoSaTo by Angelo Gonzalez
---------------------------

Perlin noise shader by toneburst:
http://machinesdontcare.wordpress.com/2009/06/25/3d-perlin-noise-sphere-vertex-shader-sourcecode/
*/

const float permTexUnit = 1.0/256.0;		// Perm texture texel-size
const float permTexUnitHalf = 0.5/256.0;	// Half perm texture texel-size

float width = SCREEN_SIZE.x;
float height = SCREEN_SIZE.y;

const float grainamount = GrainPower;
float colored = GrainColored;
float coloramount = GrainColorAmount;
float grainsize = GrainSize;
float lumamount = GrainLuma;

float fract (float inp)
{
	return (inp-floor(inp));
}

float4 mix (float x, float y, float a)
{
	return x * (1 - a) + y * a;
}
    
//a random texture generator, but you can also use a pre-computed perturbation texture
float4 rnm(in float2 tc) 
{
    float noise =  sin(dot(tc + float2(timer,timer),float2(12.9898,78.233))) * 43758.5453;

	float noiseR =  frac(noise)*2.0-1.0;
	float noiseG =  frac(noise*1.2154)*2.0-1.0; 
	float noiseB =  frac(noise*1.3453)*2.0-1.0;
	float noiseA =  frac(noise*1.3647)*2.0-1.0;
	
	return float4(noiseR,noiseG,noiseB,noiseA);
}

float fade(in float t) {
	return t*t*t*(t*(t*6.0-15.0)+10.0);
}

float pnoise3D(in float3 p)
{
	float3 pi = permTexUnit*floor(p)+permTexUnitHalf; // Integer part, scaled so +1 moves permTexUnit texel
	// and offset 1/2 texel to sample texel centers
	float3 pf = frac(p);     // Fractional part for interpolation

	// Noise contributions from (x=0, y=0), z=0 and z=1
	float perm00 = rnm(pi.xy).a ;
	float3  grad000 = rnm(float2(perm00, pi.z)).rgb * 4.0 - 1.0;
	float n000 = dot(grad000, pf);
	float3  grad001 = rnm(float2(perm00, pi.z + permTexUnit)).rgb * 4.0 - 1.0;
	float n001 = dot(grad001, pf - float3(0.0, 0.0, 1.0));

	// Noise contributions from (x=0, y=1), z=0 and z=1
	float perm01 = rnm(pi.xy + float2(0.0, permTexUnit)).a ;
	float3  grad010 = rnm(float2(perm01, pi.z)).rgb * 4.0 - 1.0;
	float n010 = dot(grad010, pf - float3(0.0, 1.0, 0.0));
	float3  grad011 = rnm(float2(perm01, pi.z + permTexUnit)).rgb * 4.0 - 1.0;
	float n011 = dot(grad011, pf - float3(0.0, 1.0, 1.0));

	// Noise contributions from (x=1, y=0), z=0 and z=1
	float perm10 = rnm(pi.xy + float2(permTexUnit, 0.0)).a ;
	float3  grad100 = rnm(float2(perm10, pi.z)).rgb * 4.0 - 1.0;
	float n100 = dot(grad100, pf - float3(1.0, 0.0, 0.0));
	float3  grad101 = rnm(float2(perm10, pi.z + permTexUnit)).rgb * 4.0 - 1.0;
	float n101 = dot(grad101, pf - float3(1.0, 0.0, 1.0));

	// Noise contributions from (x=1, y=1), z=0 and z=1
	float perm11 = rnm(pi.xy + float2(permTexUnit, permTexUnit)).a ;
	float3  grad110 = rnm(float2(perm11, pi.z)).rgb * 4.0 - 1.0;
	float n110 = dot(grad110, pf - float3(1.0, 1.0, 0.0));
	float3  grad111 = rnm(float2(perm11, pi.z + permTexUnit)).rgb * 4.0 - 1.0;
	float n111 = dot(grad111, pf - float3(1.0, 1.0, 1.0));

	// Blend contributions along x
	float4 n_x = lerp(float4(n000, n001, n010, n011), float4(n100, n101, n110, n111), fade(pf.x));

	// Blend contributions along y
	float2 n_xy = lerp(n_x.xy, n_x.zw, fade(pf.y));

	// Blend contributions along z
	float n_xyz = lerp(n_xy.x, n_xy.y, fade(pf.z));

	// We're done, return the final noise value.
	return n_xyz;
}

//2d coordinate orientation thing
float2 coordRot(in float2 tc, in float angle)
{
	float aspectr = width/height;
	float rotX = ((tc.x*2.0-1.0)*aspectr*cos(angle)) - ((tc.y*2.0-1.0)*sin(angle));
	float rotY = ((tc.y*2.0-1.0)*cos(angle)) + ((tc.x*2.0-1.0)*aspectr*sin(angle));
	rotX = ((rotX/aspectr)*0.5+0.5);
	rotY = rotY*0.5+0.5;
	return float2(rotX,rotY);
}

float4 GrainPass( float4 colorInput, float2 tex ) 
{
	float3 rotOffset = float3(1.425,3.892,5.835); //rotation offset values	
	float2 rotCoordsR = coordRot(tex, timer + rotOffset.x);
	float2 rot = rotCoordsR*float2(width/grainsize,height/grainsize);
    float pNoise = pnoise3D(float3(rot.x,rot.y,0.0));
	float3 noise = float3(pNoise, pNoise, pNoise);
  
	if (colored == 1)
	{
		float2 rotCoordsG = coordRot(tex, timer + rotOffset.y);
		float2 rotCoordsB = coordRot(tex, timer + rotOffset.z);
		noise.g = lerp(noise.r,pnoise3D(float3(rotCoordsG*float2(width/grainsize,height/grainsize),1.0)),coloramount);
		noise.b = lerp(noise.r,pnoise3D(float3(rotCoordsB*float2(width/grainsize,height/grainsize),2.0)),coloramount);
	}
	
	float3 col = colorInput.rgb;

	//noisiness response curve based on scene luminance
	float3 lumcoeff = float3(0.299,0.587,0.114);
	float luminance = lerp(0.0,dot(col, lumcoeff),lumamount);
	float lum = smoothstep(0.2,0.0,luminance);
	lum += luminance;
	
	float2 thepow = pow(lum, 4.0);
	
	noise = lerp(noise,float3(0.0, 0.0, 0.0),pow(lum,4.0));
	col += noise*grainamount;
   
	return float4(col,1.0);
}

// ------------------------- Border --------------------------------------------

#ifndef border_width
  #define border_width float2(1,0)
#endif
 
#ifndef border_color
  #define border_color float3(0, 0, 0)
#endif

#define screen_ratio (SCREEN_SIZE.x / SCREEN_SIZE.y)
 
float4 BorderPass( float4 colorInput, float2 tex )
{
  float3 border_color_float = border_color / 255.0;
 
  float2 border_width_variable = border_width;
 
  // -- calculate the right border_width for a given border_ratio --
  if (!any(border_width)) //if border_width is not used
    if (screen_ratio < border_ratio)
      border_width_variable = float2(0.0, (SCREEN_SIZE.y - (SCREEN_SIZE.x / border_ratio)) * 0.5);
    else
      border_width_variable = float2((SCREEN_SIZE.x - (SCREEN_SIZE.y * border_ratio)) * 0.5, 0.0);
 
  float2 border = (PIXEL_SIZE * border_width_variable); //Translate integer pixel width to floating point
 
  float2 within_border = saturate((-tex * tex + tex) - (-border * border + border)); //becomes positive when inside the border and 0 when outside
 
  colorInput.rgb = all(within_border) ?  colorInput.rgb : border_color_float ; //if the pixel is within the border use the original color, if not use the border_color
 
  return colorInput; //return the pixel
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

#if (USE_ADVANCED_CRT == 1)
    c0 = AdvancedCRTPass(c0, tex);
#endif

#if (USE_DITHER == 1)
    c0 = DitherPass(c0, tex);
#endif

#if (USE_TECHNICOLOR == 1)
    c0 = TechnicolorPass(c0);
#endif

#if (USE_DPX == 1)
    c0 = DPXPass(c0);
#endif

#if (USE_GRAIN == 1)
    c0 = GrainPass(c0, tex);
#endif

#if (USE_BORDER == 1)
    c0 = BorderPass(c0, tex);
#endif

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