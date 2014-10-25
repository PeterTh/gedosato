/*===============================================================================*\
|########################    [GSFx Shader Suite v1.50]    ########################|
|##########################        By Asmodean          ##########################|
||                                                                               ||
||          This program is free software; you can redistribute it and/or        ||
||          modify it under the terms of the GNU General Public License          ||
||          as published by the Free Software Foundation; either version 2       ||
||          of the License, or (at your option) any later version.               ||
||                                                                               ||
||          This program is distributed in the hope that it will be useful,      ||
||          but WITHOUT ANY WARRANTY; without even the implied warranty of       ||
||          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        ||
||          GNU General Public License for more details. (c)2014                 ||
||                                                                               ||
|#################################################################################|
\*===============================================================================*/

/*------------------------------------------------------------------------------
                        [DEFINITIONS & ON/OFF OPTIONS]
------------------------------------------------------------------------------*/

//---------------------------#[CHOOSE EFFECTS]#--------------------------------\\

//-#[LIGHTING & COLOUR]         [1=ON|0=OFF]
#define BLENDED_BLOOM                1      //#High Quality Bloom, using blend techniques. Blooms naturally, per environment.
#define SCENE_TONEMAPPING            1      //#Scene Tonemapping & RGB Colour Correction. Corrects colour, and tone maps the scene.
#define GAMMA_CORRECTION             1      //#RGB Post Gamma Correction Curve. sRGB->Linear->sRGB correction curve. 
#define TEXTURE_SHARPEN              0      //#Bicubic Texture Unsharpen Mask. Looks similar to a negative LOD bias. Enhances texture fidelity.
#define PIXEL_VIBRANCE               0      //#Pixel Vibrance. Intelligently adjusts pixel vibrance depending on original saturation.
#define S_CURVE_CONTRAST             0      //#S-Curve Scene Contrast Enhancement. Locally adjusts contrast using S-curves.
#define CEL_SHADING                  0      //#PX Cel Shading. Simulates the look of animation/toon. Typically best suited for animated-style games.

/*------------------------------------------------------------------------------
                          [EFFECT CONFIG OPTIONS]
------------------------------------------------------------------------------*/

//##[BLOOM OPTIONS]##
#define BloomType BlendBloom                //[BlendScreen, BlendOverlay, BlendBloom, BlendAddLight] The type of blending for the bloom (Default: BlendScreen).
#define BloomStrength 0.200                 //[0.100 to 1.000] Overall strength of the bloom. You may want to readjust for each blend type.
#define BlendStrength 1.000                 //[0.100 to 1.000] Strength of the bloom blend. Lower for less blending, higher for more. (Default: 1.000).
#define BlendSpread 4.000                   //[1.000 to 8.000] Width of the bloom 'glow' spread. 0.000 = off. Scales with BloomStrength. (Default: 4.000).
#define BloomReds 1.00                      //[0.00 to 8.00] Red channel component of the RGB correction curve. Higher values equals red reduction. 1.00 is default.
#define BloomGreens 1.00                    //[0.00 to 8.00] Green channel component of the RGB correction curve. Higher values equals green reduction. 1.00 is default.
#define BloomBlues 1.00                     //[0.00 to 8.00] Blue channel component of the RGB correction curve. Higher values equals blue reduction. 1.00 is default.

//##[TONEMAP OPTIONS]##
#define TonemapType 1                       //[1 or 2] Type of tone mapping operator. 1 is natural, 2 is cinematic. You may want to readjust BlackLevels to compensate for diff types.
#define ToneAmount 0.25                     //[0.00 to 1.00] Tonemap strength (scene correction) higher for stronger tone mapping, lower for lighter. (Default: ~ 0.25)
#define BlackLevels 0.30                    //[0.00 to 1.00] Black level balance (shadow correction). Increase to lighten blacks, lower to deepen them. (Default: ~ 0.30).
#define Exposure 1.00                       //[0.10 to 2.00] White correction (brightness) Higher values for more Exposure, lower for less.
#define Luminance 1.02                      //[0.10 to 2.00] Luminance average (luminance correction) Higher values to decrease luminance average, lower values to increase luminance.
#define WhitePoint 1.02                     //[0.10 to 2.00] Whitepoint avg (lum correction) Use to alter the scene whitepoint average. Raising can give a cinema look.

//##[CORRECTION OPTIONS]##
#define CorrectionPalette 1                 //[0|1|2|3] The colour correction palette type. 1: RGB, 2: YUV, 3: XYZ, 0: off. 1 is default. This requires tone mapping enabled.
#define RedCurve 1.00                       //[1.00 to 8.00] Red channel component of the RGB correction curve. Higher values equals red reduction. 1.00 is default.
#define GreenCurve 1.00                     //[1.00 to 8.00] Green channel component of the RGB correction curve. Higher values equals green reduction. 1.00 is default.
#define BlueCurve 1.00                      //[1.00 to 8.00] Blue channel component of the RGB correction curve. Higher values equals blue reduction. 1.00 is default.
#define FilmicShift 1                       //[0 or 1] Filmic cross processing. Shifts colour balance to alter the mood of the scene. For a more filmic look.
#define RedShift 0.60                       //[0.10 to 1.00] Red colour component shift of the filmic tone shift. Alters the red balance of the shift. Requires FilmicShift.
#define GreenShift 0.40                     //[0.10 to 1.00] Green colour component shift of the filmic tone shift. Alters the green balance of the shift. Requires FilmicShift.
#define BlueShift 0.50                      //[0.10 to 1.00] Blue colour component shift of the filmic tone shift. Alters the blue balance of the shift. Requires FilmicShift.
#define ShiftRatio 0.15                     //[0.10 to 1.00] The blending ratio for the base colour and the colour shift. Higher for a stronger effect. Requires FilmicShift.

//##[SHARPEN OPTIONS]##
#define SharpeningType 2                    //[1 or 2] The type of sharpening to use. Type 1 is a High Pass Gaussian. Type 2 is a higher quality(slightly slower) Bicubic Sampling type.
#define SharpenStrength 0.60                //[0.10 to 1.00] Strength of the texture luma sharpening effect. This is the maximum strength that will be used.
#define SharpenClamp 0.012                  //[0.005 to 0.500] Reduces the clamping/limiting on the maximum amount of sharpening each pixel recieves. Raise this to reduce the clamping.
#define SharpenBias 1.00                    //[1.00 to 4.00] Sharpening edge bias. Lower values for clean subtle sharpen, and higher values for a deeper textured sharpen.
#define DebugSharpen 0                      //[0 or 1] Visualize the sharpening effect. Useful for fine-tuning. Best to disable other effects, to see edge detection clearly.

//##[CSHADE OPTIONS]##
#define EdgeStrength 1.50                   //[0.00 to 4.00] Overall strength of the cel edge outline effect.  0.00 = no outlines.
#define EdgeFilter 0.60                     //[0.10 to 2.00] Filters out fainter cel edges. Use it for balancing the cel edge density. EG: for faces, foliage, etc. Raise to filter out more edges.
#define EdgeThickness 1.25                  //[0.50 to 4.00] Thickness of the cel edges. Increase for thicker outlining.  Note: when downsampling, you may need to raise this further to keep the edges as noticeable.
#define PaletteType 2                       //[1|2|3] The colour palette to use. 1 is Game Original, 2 is Animated Shading, 3 is Water Painting (Default is 2: Animated Shading). #!Options below don't affect palette 1.
#define UseYuvLuma 0                        //[0 or 1] Uses YUV luma calculations, or base colour luma calculations. Yuv luma can produce a better shaded look, but if it looks odd, disable it for that game.
#define LumaConversion 1                    //[0 or 1] Uses BT.601, or BT.709, RGB<-YUV->RGB conversions. Some games prefer 601, but most prefer 709. BT.709 is typically recommended. 
#define ColorRounding 1                     //[0 or 1] Uses rounding methods on colors. This can emphasise shaded toon colors. Looks good in some games, and odd in others. Try it in-game and see.

//##[GAMMA OPTIONS]##
#define Gamma 2.20                          //[1.5 to 4.0] Gamma correction. Lower Values = more gamma toning(darker), higher Values = brighter (2.2 correction is generally recommended)

//##[VIBRANCE OPTIONS]##
#define Vibrance 0.10                       //[-1.00 to 1.00] Adjust the vibrance of pixels depending on their original saturation. 0.00 is original vibrance.

//##[CONTRAST OPTIONS]##
#define Contrast 0.35                       //[0.00 to 2.00] The amount of contrast you want. Controls the overall contrast strength.

//[END OF USER OPTIONS]##

#ifndef USE_SRGB
#define USE_SRGB true
#endif

/*------------------------------------------------------------------------------
                             [GLOBALS/FUNCTIONS]
------------------------------------------------------------------------------*/

static float2 pixelSize = PIXEL_SIZE;
static float2 screenSize = SCREEN_SIZE;
static float2 invDefocus = float2(1.0 / 3840.0, 1.0 / 2160.0);
static const float3 lumCoeff = float3(0.2126729, 0.7151522, 0.0721750);

Texture2D thisframeTex;
SamplerState s0
{
    Texture = <thisframeTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
    SRGBTexture = USE_SRGB;
};

Texture2D paletteTex
<
    string path = "palette.bmp";
    string format = "D3DFMT_X8R8G8B8";
>;
SamplerState s1
{
    Texture = <paletteTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
    SRGBTexture = false;
};

struct VS_INPUT
{
    float4 vertPos : POSITION;
    float2 UVCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 vertPos : SV_POSITION;
    float2 UVCoord : TEXCOORD0;
};

float RGBLuminance(float3 color)
{
    return dot(color.rgb, lumCoeff);
}

/*------------------------------------------------------------------------------
                            [VERTEX CODE SECTION]
------------------------------------------------------------------------------*/

VS_OUTPUT FrameVS(VS_INPUT Input)
{
    VS_OUTPUT Output;

    Output.vertPos = Input.vertPos;
    Output.UVCoord = Input.UVCoord;

    return Output;
}

/*------------------------------------------------------------------------------
                       [GAMMA CORRECTION CODE SECTION]
------------------------------------------------------------------------------*/

float3 RGBGammaToLinear(float3 color, float gamma)
{
    color = saturate(color);
    color.r = (color.r <= 0.0404482362771082) ?
    color.r / 12.92 : pow((color.r + 0.055) / 1.055, gamma);
    color.g = (color.g <= 0.0404482362771082) ?
    color.g / 12.92 : pow((color.g + 0.055) / 1.055, gamma);
    color.b = (color.b <= 0.0404482362771082) ?
    color.b / 12.92 : pow((color.b + 0.055) / 1.055, gamma);

    return color;
}

float3 LinearToRGBGamma(float3 color, float gamma)
{
    color = saturate(color);
    color.r = (color.r <= 0.00313066844250063) ?
    color.r * 12.92 : 1.055 * pow(color.r, 1.0 / gamma) - 0.055;
    color.g = (color.g <= 0.00313066844250063) ?
    color.g * 12.92 : 1.055 * pow(color.g, 1.0 / gamma) - 0.055;
    color.b = (color.b <= 0.00313066844250063) ?
    color.b * 12.92 : 1.055 * pow(color.b, 1.0 / gamma) - 0.055;

    return color;
}

float4 GammaPass(float4 color, float2 texcoord) : COLOR0
{
    const float GammaConst = 2.233;
    color.rgb = RGBGammaToLinear(color.rgb, GammaConst);
    color.rgb = LinearToRGBGamma(color.rgb, float(Gamma));
    color.a = RGBLuminance(color.rgb);

    return color;
}

/*------------------------------------------------------------------------------
                        [BLENDED BLOOM CODE SECTION]
------------------------------------------------------------------------------*/

float3 BlendAddLight(float3 color, float3 bloom)
{
    return color + bloom;
}

float3 BlendScreen(float3 color, float3 bloom)
{
    return (color + bloom) - (color * bloom);
}

float3 BlendBloom(float3 color, float3 bloom)
{
    float3 coeff = step(0.5, color);
    return lerp((color + bloom) - (color * bloom), (bloom + bloom) - (bloom * bloom), coeff);
}

float3 BlendOverlay(float3 color, float3 bloom)
{
    float3 overlay = step(0.5, color);
    overlay = lerp((color * bloom * 2.0), (1.0 - (2.0 * (1.0 - color) * (1.0 - bloom))), overlay);

    return overlay;
}

float4 PyramidFilter(sampler2D tex, float2 texcoord, float2 width)
{
    float4 color = tex2D(tex, texcoord + float2(0.5, 0.5) * width);
    color += tex2D(tex, texcoord + float2(-0.5,  0.5) * width);
    color += tex2D(tex, texcoord + float2(0.5, -0.5) * width);
    color += tex2D(tex, texcoord + float2(-0.5, -0.5) * width);
    color *= 0.25;

    return color;
}

float3 BloomCorrection(float3 color)
{
    float X = 1.0 / (1.0 + exp(float(BloomReds) / 2.0));
    float Y = 1.0 / (1.0 + exp(float(BloomGreens) / 2.0));
    float Z = 1.0 / (1.0 + exp(float(BloomBlues) / 2.0));

    color.r = (1.0 / (1.0 + exp(float(-BloomReds) * (color.r - 0.5))) - X) / (1.0 - 2.0 * X);
    color.g = (1.0 / (1.0 + exp(float(-BloomGreens) * (color.g - 0.5))) - Y) / (1.0 - 2.0 * Y);
    color.b = (1.0 / (1.0 + exp(float(-BloomBlues) * (color.b - 0.5))) - Z) / (1.0 - 2.0 * Z);

    return saturate(color);
}

float4 BloomPass(float4 color, float2 texcoord) : COLOR0
{
    float defocus = 1.2;
    float4 bloom = PyramidFilter(s0, texcoord, pixelSize * defocus);

    float2 dx = float2(invDefocus.x * float(BlendSpread), 0.0);
    float2 dy = float2(0.0, invDefocus.y * float(BlendSpread));

    float2 dx2 = 2.0 * dx;
    float2 dy2 = 2.0 * dy;

    float4 bloomBlend = bloom * 0.22520613262190495;

    bloomBlend += 0.002589001911021066 * tex2D(s0, texcoord - dx2 + dy2);
    bloomBlend += 0.010778807494659370 * tex2D(s0, texcoord - dx + dy2);
    bloomBlend += 0.024146616900339800 * tex2D(s0, texcoord + dy2);
    bloomBlend += 0.010778807494659370 * tex2D(s0, texcoord + dx + dy2);
    bloomBlend += 0.002589001911021066 * tex2D(s0, texcoord + dx2 + dy2);

    bloomBlend += 0.010778807494659370 * tex2D(s0, texcoord - dx2 + dy);
    bloomBlend += 0.044875475183061630 * tex2D(s0, texcoord - dx + dy);
    bloomBlend += 0.100529757860782610 * tex2D(s0, texcoord + dy);
    bloomBlend += 0.044875475183061630 * tex2D(s0, texcoord + dx + dy);
    bloomBlend += 0.010778807494659370 * tex2D(s0, texcoord + dx2 + dy);

    bloomBlend += 0.024146616900339800 * tex2D(s0, texcoord - dx2);
    bloomBlend += 0.100529757860782610 * tex2D(s0, texcoord - dx);
    bloomBlend += 0.100529757860782610 * tex2D(s0, texcoord + dx);
    bloomBlend += 0.024146616900339800 * tex2D(s0, texcoord + dx2);

    bloomBlend += 0.010778807494659370 * tex2D(s0, texcoord - dx2 - dy);
    bloomBlend += 0.044875475183061630 * tex2D(s0, texcoord - dx - dy);
    bloomBlend += 0.100529757860782610 * tex2D(s0, texcoord - dy);
    bloomBlend += 0.044875475183061630 * tex2D(s0, texcoord + dx - dy);
    bloomBlend += 0.010778807494659370 * tex2D(s0, texcoord + dx2 - dy);

    bloomBlend += 0.002589001911021066 * tex2D(s0, texcoord - dx2 - dy2);
    bloomBlend += 0.010778807494659370 * tex2D(s0, texcoord - dx - dy2);
    bloomBlend += 0.024146616900339800 * tex2D(s0, texcoord - dy2);
    bloomBlend += 0.010778807494659370 * tex2D(s0, texcoord + dx - dy2);
    bloomBlend += 0.002589001911021066 * tex2D(s0, texcoord + dx2 - dy2);
    bloomBlend = lerp(color, bloomBlend, float(BlendStrength));

    bloom.rgb = BloomType(bloom.rgb, bloomBlend.rgb);
    bloom.rgb = BloomCorrection(bloom.rgb);

    color.a = RGBLuminance(color.rgb);
    bloom.a = RGBLuminance(bloom.rgb);

    color = lerp(color, bloom, float(BloomStrength));

    return color;
}

/*------------------------------------------------------------------------------
                [COLOR CORRECTION/TONE MAPPING CODE SECTION]
------------------------------------------------------------------------------*/

float3 FilmicTonemap(float3 color)
{
    float3 Q = color.xyz;

    float A = 0.10;
    float B = float(BlackLevels);
    float C = 0.10;
    float D = float(ToneAmount);
    float E = 0.02;
    float F = 0.30;
    float W = float(WhitePoint);

    float3 numerator = ((Q*(A*Q + C*B) + D*E) / (Q*(A*Q + B) + D*F)) - E / F;
    float3 denominator = ((W*(A*W + C*B) + D*E) / (W*(A*W + B) + D*F)) - E / F;

    color.xyz = numerator / denominator;

    return color;
}

float3 ColorShift(float3 color)
{
    float3 colMood;

    colMood.r = float(RedShift);
    colMood.g = float(GreenShift);
    colMood.b = float(BlueShift);

    float fLum = RGBLuminance(color.rgb);
    colMood = lerp(0.0, colMood, saturate(fLum * 2.0));
    colMood = lerp(colMood, 1.0, saturate(fLum - 0.5) * 2.0);
    float3 colOutput = lerp(color, colMood, saturate(fLum * float(ShiftRatio)));

    return colOutput;
}

float3 ColorCorrection(float3 color)
{
    float X = 1.0 / (1.0 + exp(float(RedCurve) / 2.0));
    float Y = 1.0 / (1.0 + exp(float(GreenCurve) / 2.0));
    float Z = 1.0 / (1.0 + exp(float(BlueCurve) / 2.0));

    color.r = (1.0 / (1.0 + exp(float(-RedCurve) * (color.r - 0.5))) - X) / (1.0 - 2.0 * X);
    color.g = (1.0 / (1.0 + exp(float(-GreenCurve) * (color.g - 0.5))) - Y) / (1.0 - 2.0 * Y);
    color.b = (1.0 / (1.0 + exp(float(-BlueCurve) * (color.b - 0.5))) - Z) / (1.0 - 2.0 * Z);

    return saturate(color);
}

float4 TonemapPass(float4 color, float2 texcoord) : COLOR0
{
    const float delta = 0.001f;
    
    if (CorrectionPalette == 1) { color.rgb = ColorCorrection(color.rgb); }
    if (FilmicShift == 1) { color.rgb = ColorShift(color.rgb); }
    
    color.rgb = FilmicTonemap(color.rgb);

    // RGB -> XYZ conversion
    const float3x3 RGB2XYZ = { 0.4124564, 0.3575761, 0.1804375,
                               0.2126729, 0.7151522, 0.0721750,
                               0.0193339, 0.1191920, 0.9503041 };

    float3 XYZ = mul(RGB2XYZ, color.rgb);

    // XYZ -> Yxy conversion
    float3 Yxy = lumCoeff;
    float Wp = 1.000;

    Yxy.r = XYZ.g;                              // copy luminance Y
    Yxy.g = XYZ.r / (XYZ.r + XYZ.g + XYZ.b);    // x = X / (X + Y + Z)
    Yxy.b = XYZ.g / (XYZ.r + XYZ.g + XYZ.b);    // y = Y / (X + Y + Z)

    if (CorrectionPalette == 2) { Yxy.rgb = ColorCorrection(Yxy.rgb); }

    // (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
    #if (TonemapType == 1)
        float Lp = Yxy.r * float(Exposure) / (float(Luminance) + delta);
    #elif (TonemapType == 2)
        float Lp = Yxy.r * FilmicTonemap(Yxy.rrr) / RGBLuminance(Yxy.rrr) *
        float(Exposure) / (float(Luminance) + delta);
    #endif

    // (Ld) Scale all luminance within a displayable range of 0 to 1
    Yxy.r = (Lp * (1.0 + Lp / (Wp * Wp))) / (1.0 + Lp);

    // Yxy -> XYZ conversion
    XYZ.r = Yxy.r * Yxy.g / Yxy.b;                  // X = Y * x / y
    XYZ.g = Yxy.r;                                  // copy luminance Y
    XYZ.b = Yxy.r * (1.0 - Yxy.g - Yxy.b) / Yxy.b;  // Z = Y * (1-x-y) / y

    if (CorrectionPalette == 3) { XYZ.rgb = ColorCorrection(XYZ.rgb); }

    // XYZ -> RGB conversion
    const float3x3 XYZ2RGB = { 3.2404542,-1.5371385,-0.4985314,
                              -0.9692660, 1.8760108, 0.0415560,
                               0.0556434,-0.2040259, 1.0572252 };

    color.rgb = mul(XYZ2RGB, XYZ);
    color.a = RGBLuminance(color.rgb);

    return saturate(color);
}

/*------------------------------------------------------------------------------
                       [TEXTURE SHARPEN CODE SECTION]
------------------------------------------------------------------------------*/

float Cubic(float x)
{
    float x2 = x * x;
    float x3 = x2 * x;

    float cx = -x3 + 3.0 * x2 - 3.0 * x + 1.0;
    float cy = 3.0 * x3 - 6.0 * x2 + 4.0;
    float cz = -3.0 * x3 + 3.0 * x2 + 3.0 * x + 1.0;
    float cw = x3;

    return (lerp(cx, cy, 0.5) + lerp(cz, cw, 0.5)) / 6.0;
}

float4 SampleBiCubic(SamplerState texSample, float2 TexCoord)
{
    float texelSizeX = pixelSize.x * float(SharpenBias);
    float texelSizeY = pixelSize.y * float(SharpenBias);

    float4 nSum = (float4)0.0;
    float4 nDenom = (float4)0.0;

    float a = frac(TexCoord.x * screenSize.x);
    float b = frac(TexCoord.y * screenSize.y);

    int nX = int(TexCoord.x * screenSize.x);
    int nY = int(TexCoord.y * screenSize.y);

    float2 uvCoord = float2(float(nX) / screenSize.x, float(nY) / screenSize.y);

    for (int m = -1; m <= 2; m++)
    {
        for (int n = -1; n <= 2; n++)
        {
            float4 Samples = tex2D(texSample, uvCoord +
            float2(texelSizeX * float(m), texelSizeY * float(n)));

            float vc1 = Cubic(float(m) - a);
            float4 vecCoeff1 = float4(vc1, vc1, vc1, vc1);

            float vc2 = Cubic(-(float(n) - b));
            float4 vecCoeff2 = float4(vc2, vc2, vc2, vc2);

            nSum = nSum + (Samples * vecCoeff2 * vecCoeff1);
            nDenom = nDenom + (vecCoeff2 * vecCoeff1);
        }
    }
    return nSum / nDenom;
}

float4 TexSharpenPass(float4 color, float2 texcoord) : COLOR0
{
    float3 calcSharpen = lumCoeff * float(SharpenStrength);

    float4 blurredColor = SampleBiCubic(s0, texcoord);
    float3 sharpenedColor = (color.rgb - blurredColor.rgb);

    float sharpenLuma = dot(sharpenedColor, calcSharpen);
    sharpenLuma = clamp(sharpenLuma, -float(SharpenClamp), float(SharpenClamp));

    color.rgb = color.rgb + sharpenLuma;
    color.a = RGBLuminance(color.rgb);

    #if (DebugSharpen == 1)
        color = saturate(0.5f + (sharpenLuma * 4)).rrrr;
    #endif

    return saturate(color);
}

/*------------------------------------------------------------------------------
                        [CEL SHADING CODE SECTION]
------------------------------------------------------------------------------*/

static const int NUM = 9;
static const float3 thresholds = float3(5.0, 8.0, 6.0);

#if (LumaConversion == 1)
#define celLumaCoef float3(0.2126729, 0.7151522, 0.0721750)
#else
#define celLumaCoef float3(0.299, 0.587, 0.114)
#endif

float3 GetYUV(float3 rgb)
{
#if (LumaConversion == 1)
    float3x3 RGB2YUV = { 
             0.2126, 0.7152, 0.0722,
            -0.09991, -0.33609, 0.436,
             0.615, -0.55861, -0.05639 };

#else
    float3x3 RGB2YUV = {
             0.299, 0.587, 0.114,
            -0.14713, -0.28886f, 0.436,
             0.615, -0.51499, -0.10001 };
#endif

    return mul(RGB2YUV, rgb);
}

float3 GetRGB(float3 yuv)
{
#if (LumaConversion == 1)
    float3x3 YUV2RGB = {
             1.000, 0.000, 1.28033,
             1.000, -0.21482, -0.38059,
             1.000, 2.12798, 0.000 };

#else
    float3x3 YUV2RGB = {
             1.000, 0.000, 1.13983,
             1.000, -0.39465, -0.58060,
             1.000, 2.03211, 0.000 };
#endif

    return mul(YUV2RGB, yuv);
}

float GetCelLuminance(float3 rgb)
{
    return dot(rgb, celLumaCoef);
}

float4 CelPass(float4 color, float2 uv0)
{
    float3 yuv;
    float3 sum = color.rgb;
    float2 pixel = pixelSize * EdgeThickness;
    const float2 RoundingOffset = float2(0.20, 0.40);

    float2 c[NUM] = {
    float2(-0.0078125, -0.0078125),
    float2(0.00, -0.0078125),
    float2(0.0078125, -0.0078125),
    float2(-0.0078125, 0.00),
    float2(0.00, 0.00),
    float2(0.0078125, 0.00),
    float2(-0.0078125, 0.0078125),
    float2(0.00, 0.0078125),
    float2(0.0078125, 0.0078125) };

    float3 col[NUM];
    float lum[NUM];

    for (int i = 0; i < NUM; i++)
    {
        col[i] = tex2D(s0, uv0 + c[i] * RoundingOffset).rgb;

        #if (ColorRounding == 1)
        col[i].r = saturate(round(col[i].r * thresholds.r) / thresholds.r);
        col[i].g = saturate(round(col[i].g * thresholds.g) / thresholds.g);
        col[i].b = saturate(round(col[i].b * thresholds.b) / thresholds.b);
        #endif

        lum[i] = GetCelLuminance(col[i].xyz);

        yuv = GetYUV(col[i]);

        if (UseYuvLuma == 0)
        {
            yuv.r = saturate(round(yuv.r * lum[i]) / thresholds.r + lum[i]);
        }
        else
        {
            yuv.r = saturate(round(yuv.r * thresholds.r) / thresholds.r + lum[i] / (255.0 / 5.0));
        }

        yuv = GetRGB(yuv);

        sum += yuv;
    }

    float3 shadedColor = (sum / NUM);

    float edgeX = dot(tex2D(s0, uv0 + pixel).rgb, celLumaCoef);
    edgeX = dot(float4(tex2D(s0, uv0 - pixel).rgb, edgeX), float4(celLumaCoef, -1.0));

    float edgeY = dot(tex2D(s0, uv0 + float2(pixel.x, -pixel.y)).rgb, celLumaCoef);
    edgeY = dot(float4(tex2D(s0, uv0 + float2(-pixel.x, pixel.y)).rgb, edgeY), float4(celLumaCoef, -1.0));

    float edge = dot(float2(edgeX, edgeY), float2(edgeX, edgeY));

    #if (PaletteType == 1)
        color.rgb = lerp(color.rgb, color.rgb + pow(edge, EdgeFilter) * -EdgeStrength, EdgeStrength);
    #elif (PaletteType == 2)
        color.rgb = lerp(color.rgb + pow(edge, EdgeFilter) * -EdgeStrength, shadedColor, 0.3);
    #elif (PaletteType == 3)
        color.rgb = lerp(shadedColor + edge * -EdgeStrength, pow(edge, EdgeFilter) * -EdgeStrength + color.rgb, 0.5);
    #endif

    color.a = RGBLuminance(color.rgb);

    return saturate(color);
}

/*------------------------------------------------------------------------------
                          [CONTRAST CODE SECTION]
------------------------------------------------------------------------------*/

float4 ContrastPass(float4 color, float2 texcoord) : COLOR0
{
    float3 luma = (float3)RGBLuminance(color.rgb);
    float3 chroma = color.rgb - luma;
    float3 x = luma;

    //S-Curve - Cubic Bezier spline
    float3 a = float3(0.00, 0.00, 0.00);
    float3 b = float3(0.25, 0.25, 0.25);
    float3 c = float3(1.00, 1.00, 1.00);
    float3 d = float3(1.00, 1.00, 1.00);

    float3 ab = lerp(a, b, x);          //point between a and b (green)
    float3 bc = lerp(b, c, x);          //point between b and c (green)
    float3 cd = lerp(c, d, x);          //point between c and d (green)
    float3 abbc = lerp(ab, bc, x);      //point between ab and bc (blue)
    float3 bccd = lerp(bc, cd, x);      //point between bc and cd (blue)
    float3 dest = lerp(abbc, bccd, x);  //point on the bezier-curve (black)

    x = dest;
    x = lerp(luma, x, float(Contrast));

    color.rgb = x + chroma;
    color.a = RGBLuminance(color.rgb);

    return saturate(color);
}

/*------------------------------------------------------------------------------
                       [PIXEL VIBRANCE CODE SECTION]
------------------------------------------------------------------------------*/

float4 VibrancePass(float4 color, float2 texcoord) : COLOR0
{
    float cVibrance = Vibrance;
    float luma = RGBLuminance(color.rgb);

    float colorMax = max(color.r, max(color.g, color.b));
    float colorMin = min(color.r, min(color.g, color.b));

    float colorSaturation = colorMax - colorMin;

    color.rgb = lerp(luma, color.rgb, (1.0 + (cVibrance * (1.0 - (sign(cVibrance) * colorSaturation)))));
    color.a = RGBLuminance(color.rgb);

    return saturate(color); //Debug: return colorSaturation.xxxx;
}

/*------------------------------------------------------------------------------
                              [MAIN/COMBINE]
------------------------------------------------------------------------------*/

float4 ColorCorrectionPS(VS_OUTPUT Input) : COLOR0
{
    float2 tex = Input.UVCoord;
    float4 c0 = tex2D(s0, tex);

    c0.r = tex1D(s1, c0.r).r;
    c0.g = tex1D(s1, c0.g).g;
    c0.b = tex1D(s1, c0.b).b;
    c0.a = RGBLuminance(c0.rgb);

    return c0;
}

float4 postProcessing(VS_OUTPUT Input) : COLOR0
{
    float2 tex = Input.UVCoord;
    float4 c0 = tex2D(s0, tex);

    #if (GAMMA_CORRECTION == 1)
        c0 = GammaPass(c0, tex);
    #endif

     #if (CEL_SHADING == 1)
        c0 = CelPass(c0, tex);
    #endif
    
    #if (TEXTURE_SHARPEN == 1)
        c0 = TexSharpenPass(c0, tex);
    #endif

    #if (BLENDED_BLOOM == 1)
        c0 = BloomPass(c0, tex);
    #endif

    #if (SCENE_TONEMAPPING == 1)
        c0 = TonemapPass(c0, tex);
    #endif

    #if (PIXEL_VIBRANCE == 1)
        c0 = VibrancePass(c0, tex);
    #endif

    #if (S_CURVE_CONTRAST == 1)
        c0 = ContrastPass(c0, tex);
    #endif

    return c0;
}

/*------------------------------------------------------------------------------
                            [TECHNIQUES/PASSES]
------------------------------------------------------------------------------*/

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

