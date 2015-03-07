/*===============================================================================*\
|########################    [GSFx Shader Suite v1.70]    ########################|
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
||          GNU General Public License for more details. (c)2015                 ||
||                                                                               ||
|#################################################################################|
\*===============================================================================*/

/*------------------------------------------------------------------------------
                        [DEFINITIONS & ON/OFF OPTIONS]
------------------------------------------------------------------------------*/

//---------------------------#[CHOOSE EFFECTS]#--------------------------------\\

//-#[LIGHTING & COLOUR]         [1=ON|0=OFF]
#define BLENDED_BLOOM                1      //#High Quality Bloom, using blend techniques. Blooms naturally, per environment.
#define SCENE_TONEMAPPING            1      //#HDR Scene Tonemapping & RGB Colour Correction. Corrects colour, and tone maps the scene.
#define GAMMA_CORRECTION             1      //#RGB Gamma Correction. sRGB->Linear->sRGB correction curve. Enable for games with incorrect gamma.
#define TEXTURE_SHARPEN              0      //#Bicubic Texture Unsharpen Mask. Looks similar to a negative texture LOD bias. Enhances texture fidelity.
#define PIXEL_VIBRANCE               0      //#Pixel Vibrance. Intelligently adjusts pixel vibrance depending on original saturation.
#define S_CURVE_CONTRAST             0      //#S-Curve Scene Contrast Enhancement. Locally adjusts contrast using S-curves.
#define CEL_SHADING                  0      //#PX Cel Shading. Simulates the look of animation/toon. Typically best suited for animated style games.
#define SW_DITHERING                 0      //#SW Sub-pixel Dithering. Simulates more colors than your monitor can display. This can reduce color banding.

/*------------------------------------------------------------------------------
                          [EFFECT CONFIG OPTIONS]
------------------------------------------------------------------------------*/

//##[BLOOM OPTIONS]##
#define BloomType BlendGlow                 //[BlendGlow, BlendAddGlow, BlendAddLight, BlendScreen, BlendLuma, BlendOverlay] The type of blended bloom.
#define BloomStrength 0.220                 //[0.100 to 1.000] Overall strength of the bloom. You may want to readjust for each blend type.
#define BlendStrength 1.000                 //[0.100 to 1.000] Strength of the bloom blend. Lower for less blending, higher for more. (Default: 1.000).
#define BloomDefocus 2.000                  //[1.000 to 4.000] The initial bloom defocus value. Increases the softness of light, bright objects, etc.
#define BloomWidth 3.200                    //[1.000 to 8.000] Width of the bloom's soft glow. Scales with BloomStrength. Increase for a wider spread.
#define BloomReds 0.020                     //[0.000 to 1.000] Bloom-exclusive colour correction of the red channel. Adjust for desired manipulation of reds.
#define BloomGreens 0.010                   //[0.000 to 1.000] Bloom-exclusive colour correction of the green channel. Adjust for desired manipulation of greens.
#define BloomBlues 0.010                    //[0.000 to 1.000] Bloom-exclusive colour correction of the blue channel. Adjust for desired manipulation of blues.

//##[TONEMAP OPTIONS]##
#define TonemapType 2                       //[0|1|2] Type of base tone mapping operator. 0 is LDR, 1 is HDR(original), 2 is HDR Filmic ALU(cinematic).
#define ToneAmount 0.25                     //[0.05 to 1.00] Tonemap strength (tone correction). Higher for stronger tone mapping, lower for lighter.
#define BlackLevels 0.05                    //[0.00 to 1.00] Black level balance (shadow correction). Increase to deepen blacks, lower to lighten them.
#define Exposure 1.00                       //[0.10 to 2.00] White correction (brightness). Higher values for more scene exposure, lower for less.
#define Luminance 1.01                      //[0.10 to 2.00] Luminance average (luminance correction). Higher values will lower scene luminance average.
#define WhitePoint 1.02                     //[0.10 to 2.00] Whitepoint average (wp lum correction). Higher values will lower the maximum scene white point.

//##[CORRECTION OPTIONS]##
#define CorrectionPalette 3                 //[0|1|2|3] The colour correction palette type. 1: RGB, 2: YUV, 3: XYZ, 0: off. 1 is default. This requires tone mapping enabled.
#define RedCurve 1.25                       //[1.00 to 8.00] Red channel component of the RGB correction curve. Higher values equals red reduction. 1.00 is default.
#define GreenCurve 1.25                     //[1.00 to 8.00] Green channel component of the RGB correction curve. Higher values equals green reduction. 1.00 is default.
#define BlueCurve 1.25                      //[1.00 to 8.00] Blue channel component of the RGB correction curve. Higher values equals blue reduction. 1.00 is default.

//##[FILMIC OPTIONS]##
#define FilmicProcess 0                     //[0|1|2] Filmic cross processing. Alters the tone of the scene, for more of a filmic look. 0: off, 1|2: process type.
#define RedShift 0.50                       //[0.10 to 1.00] Red colour component shift of the filmic processing. Alters the red balance of the shift.
#define GreenShift 0.46                     //[0.10 to 1.00] Green colour component shift of the filmic processing. Alters the green balance of the shift.
#define BlueShift 0.45                      //[0.10 to 1.00] Blue colour component shift of the filmic processing. Alters the blue balance of the shift.
#define ShiftRatio 0.25                     //[0.10 to 1.00] The blending ratio for the base colour and the colour shift. Higher for a stronger effect. 

//##[SHARPEN OPTIONS]##
#define SharpenStrength 0.75                //[0.10 to 1.00] Strength of the texture sharpening effect. This is the maximum strength that will be used.
#define SharpenClamp 0.015                  //[0.005 to 0.500] Reduces the clamping/limiting on the maximum amount of sharpening each pixel recieves. Raise this to reduce the clamping.
#define SharpenBias 1.00                    //[1.00 to 4.00] Sharpening edge bias. Lower values for clean subtle sharpen, and higher values for a deeper textured sharpen.
#define DebugSharpen 0                      //[0 or 1] Visualize the sharpening effect. Useful for fine-tuning. Best to disable other effects, to see edge detection clearly.

//##[CSHADE OPTIONS]##
#define EdgeStrength 1.50                   //[0.00 to 4.00] Overall strength of the cel edge outline effect.  0.00: no outlines.
#define EdgeFilter 0.60                     //[0.10 to 2.00] Filters out fainter cel edges. Use it for balancing the cel edge density. EG: for faces, foliage, etc. Raise to filter out more edges.
#define EdgeThickness 1.00                  //[0.50 to 4.00] Thickness of the cel edges. Increase for thicker outlining.  Note: when downsampling, you may need to raise this further to keep the edges as noticeable.
#define PaletteType 2                       //[1|2|3] The colour palette to use. 1 is Game Original, 2 is Animated Shading, 3 is Water Painting (Default is 2: Animated Shading). #!Options below don't affect palette 1.
#define UseYuvLuma 0                        //[0 or 1] Uses YUV luma calculations, or base colour luma calculations. Yuv luma can produce a better shaded look, but if it looks odd, disable it for that game.
#define LumaConversion 1                    //[0 or 1] Uses BT.601, or BT.709, RGB<-YUV->RGB conversions. Some games prefer 601, but most prefer 709. BT.709 is typically recommended. 
#define ColorRounding 1                     //[0 or 1] Uses rounding methods on colors. This can emphasise shaded toon colors. Looks good in some games, and odd in others. Try it in-game and see.

//##[GAMMA OPTIONS]##
#define Gamma 2.20                          //[1.5 to 4.0] Gamma correction. Decrease for lower gamma(darker). Increase for higher gamma(brighter). (Default: 2.2)

//##[VIBRANCE OPTIONS]##
#define Vibrance 0.20                       //[-1.00 to 1.00] Locally adjust the vibrance of pixels depending on their original saturation. 0.00 is original vibrance.

//##[CONTRAST OPTIONS]##
#define Contrast 0.35                       //[0.00 to 2.00] The amount of contrast you want. Controls the overall contrast strength.

//##[DITHERING OPTIONS]##
#define DitherMethod 2                      //[1 or 2] 1: Ordered grid dithering(faster), 2: time-based random dithering(higher quality). Hardware dithering is also enabled by default.
#define ShowMethod 0                        //[0 or 1] Shows the dithering method, based of the type of dithering selected. Useful for debugging, and confirmation of working order.

//[END OF USER OPTIONS]##

#ifndef USE_SRGB
#define USE_SRGB true
#endif

/*------------------------------------------------------------------------------
                             [GLOBALS/FUNCTIONS]
------------------------------------------------------------------------------*/

static float delta = 0.001;
static float2 pixelSize = PIXEL_SIZE;
static float2 screenSize = SCREEN_SIZE;
static const float3 lumCoeff = float3(0.2126729, 0.7151522, 0.0721750);

texture thisframeTex;
sampler s0 = sampler_state
{
    Texture = <thisframeTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
    SRGBTexture = USE_SRGB;
};

struct VS_INPUT
{
    float4 vertPos : POSITION0;
    float2 UVCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 vertPos : POSITION0;
    float2 UVCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 color : COLOR0;
};

float AvgLuminance(float3 color)
{
    return sqrt((color.x * color.x * lumCoeff.x) +
                (color.y * color.y * lumCoeff.y) +
                (color.z * color.z * lumCoeff.z));
}

float smootherstep(float a, float b, float x)
{
    x = saturate((x - a) / (b - a));
    return x*x*x*(x*(x * 6 - 15) + 10);
}

/*
float RGBLuminance(float3 color)
{
    return dot(color.xyz, lumCoeff);
}

float4 DebugClipping(float4 color)
{
    if (color.x >= 0.99999 && color.y >= 0.99999 &&
    color.z >= 0.99999) color.xyz = float3(1.0f, 0.0f, 0.0f);
    if (color.x <= 0.00001 && color.y <= 0.00001 &&
    color.z <= 0.00001) color.xyz = float3(0.0f, 0.0f, 1.0f);

    return color;
}
*/

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

#if (GAMMA_CORRECTION == 1)
float3 EncodeGamma(float3 color, float gamma)
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

float3 DecodeGamma(float3 color, float gamma)
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

float4 GammaPass(float4 color, float2 texcoord)
{
    static const float GammaConst = 2.233333;
    color.rgb = EncodeGamma(color.rgb, GammaConst);
    color.rgb = DecodeGamma(color.rgb, float(Gamma));
    color.a = AvgLuminance(color.rgb);

    return color;
}
#endif

/*------------------------------------------------------------------------------
                        [BLENDED BLOOM CODE SECTION]
------------------------------------------------------------------------------*/

#if (BLENDED_BLOOM == 1)
float3 BlendAddLight(float3 bloom, float3 blend)
{
    return saturate(bloom + blend);
}

float3 BlendScreen(float3 bloom, float3 blend)
{
    return (bloom + blend) - (bloom * blend);
}

float3 BlendAddGlow(float3 bloom, float3 blend)
{
    float glow = smootherstep(0.0, 1.0, AvgLuminance(bloom));
    return lerp(saturate(bloom + blend),
    (blend + blend) - (blend * blend), glow);
}

float3 BlendGlow(float3 bloom, float3 blend)
{
    float glow = smootherstep(0.0, 1.0, AvgLuminance(bloom));
    return lerp((bloom + blend) - (bloom * blend),
    (blend + blend) - (blend * blend), glow);
}

float3 BlendLuma(float3 bloom, float3 blend)
{
    float lumavg = smootherstep(0.0, 1.0, AvgLuminance(bloom + blend));
    return lerp((bloom * blend), (1.0 -
    ((1.0 - bloom) * (1.0 - blend))), lumavg);
}

float3 BlendOverlay(float3 bloom, float3 blend)
{
    float3 overlay = step(0.5, bloom);
    return lerp((bloom * blend * 2.0), (1.0 - (2.0 *
    (1.0 - bloom) * (1.0 - blend))), overlay);
}

float4 PyramidFilter(sampler tex, float2 texcoord, float2 width)
{
    float4 X = tex2D(tex, texcoord + float2(0.5, 0.5) * width);
    float4 Y = tex2D(tex, texcoord + float2(-0.5,  0.5) * width);
    float4 Z = tex2D(tex, texcoord + float2(0.5, -0.5) * width);
    float4 W = tex2D(tex, texcoord + float2(-0.5, -0.5) * width);

    return (X + Y + Z + W) / 4.0;
}

float3 BloomCorrection(float3 color)
{
    float3 bloom = (color.rgb - 0.5) * 2.0;

    bloom.r = 2.0 / 3.0 * (1.0 - (bloom.r * bloom.r));
    bloom.g = 2.0 / 3.0 * (1.0 - (bloom.g * bloom.g));
    bloom.b = 2.0 / 3.0 * (1.0 - (bloom.b * bloom.b));

    bloom.r = saturate(color.r + BloomReds * bloom.r);
    bloom.g = saturate(color.g + BloomGreens * bloom.g);
    bloom.b = saturate(color.b + BloomBlues * bloom.b);

    color = bloom;

    return color;
}

float4 BloomPass(float4 color, float2 texcoord)
{
    float anflare = 4.00;
    float defocus = BloomDefocus;

    float4 bloom = PyramidFilter(s0, texcoord, pixelSize * defocus);

    float2 dx = float2(pixelSize.x * float(BloomWidth), 0.0);
    float2 dy = float2(0.0, pixelSize.y * float(BloomWidth));

    float2 mdx = mul(2.0, dx);
    float2 mdy = mul(2.0, dy);

    float4 blend = bloom * 0.22520613262190495;

    blend += 0.002589001911021066 * tex2D(s0, texcoord - mdx + mdy);
    blend += 0.010778807494659370 * tex2D(s0, texcoord - dx + mdy);
    blend += 0.024146616900339800 * tex2D(s0, texcoord + mdy);
    blend += 0.010778807494659370 * tex2D(s0, texcoord + dx + mdy);
    blend += 0.002589001911021066 * tex2D(s0, texcoord + mdx + mdy);

    blend += 0.010778807494659370 * tex2D(s0, texcoord - mdx + dy);
    blend += 0.044875475183061630 * tex2D(s0, texcoord - dx + dy);
    blend += 0.100529757860782610 * tex2D(s0, texcoord + dy);
    blend += 0.044875475183061630 * tex2D(s0, texcoord + dx + dy);
    blend += 0.010778807494659370 * tex2D(s0, texcoord + mdx + dy);

    blend += 0.024146616900339800 * tex2D(s0, texcoord - mdx);
    blend += 0.100529757860782610 * tex2D(s0, texcoord - dx);
    blend += 0.100529757860782610 * tex2D(s0, texcoord + dx);
    blend += 0.024146616900339800 * tex2D(s0, texcoord + mdx);

    blend += 0.010778807494659370 * tex2D(s0, texcoord - mdx - dy);
    blend += 0.044875475183061630 * tex2D(s0, texcoord - dx - dy);
    blend += 0.100529757860782610 * tex2D(s0, texcoord - dy);
    blend += 0.044875475183061630 * tex2D(s0, texcoord + dx - dy);
    blend += 0.010778807494659370 * tex2D(s0, texcoord + mdx - dy);

    blend += 0.002589001911021066 * tex2D(s0, texcoord - mdx - mdy);
    blend += 0.010778807494659370 * tex2D(s0, texcoord - dx - mdy);
    blend += 0.024146616900339800 * tex2D(s0, texcoord - mdy);
    blend += 0.010778807494659370 * tex2D(s0, texcoord + dx - mdy);
    blend += 0.002589001911021066 * tex2D(s0, texcoord + mdx - mdy);
    blend = lerp(color, blend, float(BlendStrength));

    bloom.xyz = BloomType(bloom.xyz, blend.xyz);
    bloom.xyz = BloomCorrection(bloom.xyz);

    color.a = AvgLuminance(color.xyz);
    bloom.a = AvgLuminance(bloom.xyz);
    bloom.a *= anflare;

    color = lerp(color, bloom, float(BloomStrength));

    return color;
}
#endif

/*------------------------------------------------------------------------------
                [COLOR CORRECTION/TONE MAPPING CODE SECTION]
------------------------------------------------------------------------------*/

#if (SCENE_TONEMAPPING == 1)
float3 FilmicALU(float3 color)
{
    float3 tone = color;

    static const float3 gamma = (float3)2.233333;
    tone = (tone * (6.2 * tone + 0.5)) / (tone * (6.2 * tone + 1.66) + 0.066);
    tone = pow(tone, gamma);

    color = lerp(color, tone, 0.25);

    return color;

}

float3 FilmicCurve(float3 color)
{
    float3 X = color;
    float TA = ToneAmount;

    float A = 0.10;
    float B = 0.30;
    float C = 0.10;
    float D = TA;
    float E = 0.02;
    float F = 0.30;
    float W = 1.00;

    float3 sum = ((X*(A*X + C*B) + D*E) / (X*(A*X + B) + D*F)) - E / F;
    float denom = ((W*(A*W + C*B) + D*E) / (W*(A*W + B) + D*F)) - E / F;

    color = sum / denom;

    return saturate(color);
}

float3 CrossShift(float3 color)
{
    float3 cross;

    float2 CrossMatrix[3] = {
    float2 (0.96, 0.04 * color.r),
    float2 (0.98, 0.02 * color.g),
    float2 (0.97, 0.03 * color.b), };

    cross.r = float(RedShift) * CrossMatrix[0].x + CrossMatrix[0].y;
    cross.g = float(GreenShift) * CrossMatrix[1].x + CrossMatrix[1].y;
    cross.b = float(BlueShift) * CrossMatrix[2].x + CrossMatrix[2].y;

    float lum = AvgLuminance(color);
    float3 black = float3(0.0, 0.0, 0.0);
    float3 white = float3(1.0, 1.0, 1.0);

    cross = lerp(black, cross, saturate(lum * 2.0));
    cross = lerp(cross, white, saturate(lum - 0.5) * 2.0);
    color = lerp(color, cross, saturate(lum * float(ShiftRatio)));

    return color;
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

float4 TonemapPass(float4 color, float2 texcoord)
{
    float avgluminance = AvgLuminance(Luminance);
    float wpoint = dot(abs(normalize(max(color.r, max(color.g, color.b)))), float(WhitePoint));

    float blevel = pow(saturate(max(color.r, max(color.g, color.b))), float(BlackLevels));
    color.rgb = color.rgb * blevel;

    if (TonemapType == 2) { color.rgb = FilmicALU(color.rgb); }
    if (FilmicProcess == 1) { color.rgb = CrossShift(color.rgb); }

    if (TonemapType == 1) { color.rgb = FilmicCurve(color.rgb); }
    if (CorrectionPalette == 1) { color.rgb = ColorCorrection(color.rgb); }

    // RGB -> XYZ conversion
    static const float3x3 RGB2XYZ = { 0.4124564, 0.3575761, 0.1804375,
                                      0.2126729, 0.7151522, 0.0721750,
                                      0.0193339, 0.1191920, 0.9503041 };

    float3 XYZ = mul(RGB2XYZ, color.rgb);

    // XYZ -> Yxy conversion
    float3 Yxy;

    Yxy.r = XYZ.g;                                  // copy luminance Y
    Yxy.g = XYZ.r / (XYZ.r + XYZ.g + XYZ.b);        // x = X / (X + Y + Z)
    Yxy.b = XYZ.g / (XYZ.r + XYZ.g + XYZ.b);        // y = Y / (X + Y + Z)

    if (TonemapType == 2) { Yxy.r = FilmicCurve(Yxy).r; }
    if (CorrectionPalette == 2) { Yxy = ColorCorrection(Yxy); }

    // (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
    float Lp = Yxy.r * float(Exposure) / (avgluminance + delta);

    // (Ld) Scale all luminance within a displayable range of 0 to 1
    Yxy.r = (Lp * (1.0 + Lp / wpoint)) / (1.0 + Lp);

    // Yxy -> XYZ conversion
    XYZ.r = Yxy.r * Yxy.g / Yxy.b;                  // X = Y * x / y
    XYZ.g = Yxy.r;                                  // copy luminance Y
    XYZ.b = Yxy.r * (1.0 - Yxy.g - Yxy.b) / Yxy.b;  // Z = Y * (1-x-y) / y

    if (FilmicProcess == 2) { XYZ = CrossShift(XYZ); }
    if (CorrectionPalette == 3) { XYZ = ColorCorrection(XYZ); }

    // XYZ -> RGB conversion
    static const float3x3 XYZ2RGB = { 3.2404542,-1.5371385,-0.4985314,
                                     -0.9692660, 1.8760108, 0.0415560,
                                      0.0556434,-0.2040259, 1.0572252 };

    color.rgb = mul(XYZ2RGB, XYZ);
    color.a = AvgLuminance(color.rgb);

    return color;
}
#endif

/*------------------------------------------------------------------------------
                       [TEXTURE SHARPEN CODE SECTION]
------------------------------------------------------------------------------*/

#if (TEXTURE_SHARPEN == 1)
float Cubic(float coeff)
{
    float4 n = float4(1.0, 2.0, 3.0, 4.0) - coeff;
    float4 s = n * n * n;

    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;

    return (x + y + z + w) / 4.0;
}

float4 SampleBicubic(sampler texSample, float2 TexCoord)
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

float4 TexSharpenPass(float4 color, float2 texcoord)
{
    float3 calcSharpen = lumCoeff * float(SharpenStrength);

    float4 blurredColor = SampleBicubic(s0, texcoord);
    float3 sharpenedColor = (color.rgb - blurredColor.rgb);

    float sharpenLuma = dot(sharpenedColor, calcSharpen);
    sharpenLuma = clamp(sharpenLuma, -float(SharpenClamp), float(SharpenClamp));

    color.rgb = color.rgb + sharpenLuma;
    color.a = AvgLuminance(color.rgb);

    #if (DebugSharpen == 1)
        color = saturate(0.5f + (sharpenLuma * 4)).rrrr;
    #endif

    return saturate(color);
}
#endif

/*------------------------------------------------------------------------------
                        [CEL SHADING CODE SECTION]
------------------------------------------------------------------------------*/

#if (CEL_SHADING == 1)
float3 GetYUV(float3 RGB)
{
    static const float3x3 RGB2YUV = {
    0.2126, 0.7152, 0.0722,
   -0.09991,-0.33609, 0.436,
    0.615, -0.55861, -0.05639 };

    return mul(RGB2YUV, RGB);
}

float3 GetRGB(float3 YUV)
{
    static const float3x3 YUV2RGB = {
    1.000, 0.000, 1.28033,
    1.000,-0.21482,-0.38059,
    1.000, 2.12798, 0.000 };

    return mul(YUV2RGB, YUV);
}

float4 CelPass(float4 color, float2 uv0)
{   
    float3 yuv;
    float3 sum = color.rgb;

    const int NUM = 9;
    const float2 RoundingOffset = float2(0.25, 0.25);
    const float3 thresholds = float3(9.0, 8.0, 6.0);

    float lum[NUM];
    float3 col[NUM];
    float2 set[NUM] = {
    float2(-0.0078125, -0.0078125),
    float2(0.00, -0.0078125),
    float2(0.0078125, -0.0078125),
    float2(-0.0078125, 0.00),
    float2(0.00, 0.00),
    float2(0.0078125, 0.00),
    float2(-0.0078125, 0.0078125),
    float2(0.00, 0.0078125),
    float2(0.0078125, 0.0078125) };

    for (int i = 0; i < NUM; i++)
    {
        col[i] = tex2D(s0, uv0 + set[i] * RoundingOffset).rgb;

        #if (ColorRounding == 1)
        col[i].r = round(col[i].r * thresholds.r) / thresholds.r;
        col[i].g = round(col[i].g * thresholds.g) / thresholds.g;
        col[i].b = round(col[i].b * thresholds.b) / thresholds.b;
        #endif

        lum[i] = AvgLuminance(col[i].xyz);
        yuv = GetYUV(col[i]);

        #if (UseYuvLuma == 0)
        yuv.r = round(yuv.r * thresholds.r) / thresholds.r;
        #else
        yuv.r = saturate(round(yuv.r * lum[i]) / thresholds.r + lum[i]);
        #endif
        
        yuv = GetRGB(yuv);
        sum += yuv;
    }

    float3 shadedColor = (sum / NUM);
    float2 pixel = float2(pixelSize.x * EdgeThickness, pixelSize.y * EdgeThickness);

    float edgeX = dot(tex2D(s0, uv0 + pixel).rgb, lumCoeff);
    edgeX = dot(float4(tex2D(s0, uv0 - pixel).rgb, edgeX), float4(lumCoeff, -1.0));

    float edgeY = dot(tex2D(s0, uv0 + float2(pixel.x, -pixel.y)).rgb, lumCoeff);
    edgeY = dot(float4(tex2D(s0, uv0 + float2(-pixel.x, pixel.y)).rgb, edgeY), float4(lumCoeff, -1.0));

    float edge = dot(float2(edgeX, edgeY), float2(edgeX, edgeY));

    #if (PaletteType == 1)
        color.rgb = lerp(color.rgb, color.rgb + pow(edge, EdgeFilter) * -EdgeStrength, EdgeStrength);
    #elif (PaletteType == 2)
        color.rgb = lerp(color.rgb + pow(edge, EdgeFilter) * -EdgeStrength, shadedColor, 0.25);
    #elif (PaletteType == 3)
        color.rgb = lerp(shadedColor + edge * -EdgeStrength, pow(edge, EdgeFilter) * -EdgeStrength + color.rgb, 0.5);
    #endif

    color.a = AvgLuminance(color.rgb);

    return saturate(color);
}
#endif

/*------------------------------------------------------------------------------
                      [SUBPIXEL DITHERING CODE SECTION]
------------------------------------------------------------------------------*/

#if (SW_DITHERING == 1)
const float timer;

float2 CoordRot(float2 tc, float t)
{
    float aspect = screenSize.x / screenSize.y;

    float rotX = ((tc.x * 2.0 - 1.0) * aspect * cos(t)) - ((tc.y * 2.0 - 1.0) * sin(t));
    float rotY = ((tc.y * 2.0 - 1.0) * cos(t)) + ((tc.x * 2.0 - 1.0) * aspect * sin(t));

    rotX = ((rotX / aspect) * 0.5 + 0.5);
    rotY = rotY * 0.5 + 0.5;

    return float2(rotX, rotY);
}

float4 Randomize(float2 texcoord)
{
    float2 tex = CoordRot(texcoord, timer);

    float noise = frac(sin(dot(tex, float2(12.9898, 78.233) * 2.0)) * 43758.5453);
    float noiseR = frac(noise) * 2.0 - 1.0;
    float noiseG = frac(noise * 1.2154) * 2.0 - 1.0;
    float noiseB = frac(noise * 1.3453) * 2.0 - 1.0;
    float noiseA = frac(noise * 1.3647) * 2.0 - 1.0;

    return float4(noiseR, noiseG, noiseB, noiseA);
}

float4 DitherPass(float4 color, float2 texcoord)
{
    float ditherBits = 8.0;

    #if DitherMethod == 2       //random dithering

    float noise = Randomize(texcoord).x;
    float ditherShift = (1.0 / (pow(2.0, ditherBits) - 1.0));
    float ditherHalfShift = (ditherShift * 0.5);
    ditherShift = ditherShift * noise - ditherHalfShift;

    color.rgb += float3(-ditherShift, ditherShift, -ditherShift);

    #if ShowMethod == 1
        color.rgb = noise;
    #endif

    #elif DitherMethod == 1     //ordered dithering

    float2 ditherSize = float2(1.0 / 16.0, 10.0 / 36.0);
    float gridPosition = frac(dot(texcoord, (screenSize * ditherSize)) + 0.25);
    float ditherShift = (0.25) * (1.0 / (pow(2.0, ditherBits) - 1.0));

    float3 RGBShift = float3(ditherShift, -ditherShift, ditherShift);
    RGBShift = lerp(2.0 * RGBShift, -2.0 * RGBShift, gridPosition);

    color.rgb += RGBShift;

    #if ShowMethod == 1
        color.rgb = gridPosition;
    #endif

    #endif

    color.a = AvgLuminance(color.rgb);

    return color;
}
#endif

/*------------------------------------------------------------------------------
                          [CONTRAST CODE SECTION]
------------------------------------------------------------------------------*/

#if (S_CURVE_CONTRAST == 1)
float4 ContrastPass(float4 color, float2 texcoord)
{
    float3 luma = (float3)AvgLuminance(color.rgb);
    float3 chroma = color.rgb - luma;
    float3 x = luma;

    //S-Curve - Cubic Bezier spline
    float3 a = float3(0.00, 0.00, 0.00);
    float3 b = float3(0.25, 0.25, 0.25);
    float3 c = float3(0.95, 0.95, 0.95);
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
    color.a = AvgLuminance(color.rgb);

    return saturate(color);
}
#endif

/*------------------------------------------------------------------------------
                       [PIXEL VIBRANCE CODE SECTION]
------------------------------------------------------------------------------*/

#if (PIXEL_VIBRANCE == 1)
float4 VibrancePass(float4 color, float2 texcoord)
{
    float cVibrance = Vibrance;
    float luma = AvgLuminance(color.rgb);

    float colorMax = max(color.r, max(color.g, color.b));
    float colorMin = min(color.r, min(color.g, color.b));

    float colorSaturation = colorMax - colorMin;

    color.rgb = lerp(luma, color.rgb, (1.0 + (cVibrance * (1.0 - (sign(cVibrance) * colorSaturation)))));
    color.a = AvgLuminance(color.rgb);

    return saturate(color); //Debug: return colorSaturation.xxxx;
}
#endif

/*------------------------------------------------------------------------------
                              [MAIN/COMBINE]
------------------------------------------------------------------------------*/

PS_OUTPUT postProcessing(VS_OUTPUT Input)
{
    PS_OUTPUT Output;

    float2 tex = Input.UVCoord;
    float4 c0 = tex2D(s0, tex);

    #if (TEXTURE_SHARPEN == 1)
        c0 = TexSharpenPass(c0, tex);
    #endif

    #if (GAMMA_CORRECTION == 1)
        c0 = GammaPass(c0, tex);
    #endif

    #if (PIXEL_VIBRANCE == 1)
        c0 = VibrancePass(c0, tex);
    #endif

    #if (CEL_SHADING == 1)
        c0 = CelPass(c0, tex);
    #endif

    #if (BLENDED_BLOOM == 1)
        c0 = BloomPass(c0, tex);
    #endif

    #if (SCENE_TONEMAPPING == 1)
        c0 = TonemapPass(c0, tex);
    #endif

    #if (S_CURVE_CONTRAST == 1)
        c0 = ContrastPass(c0, tex);
    #endif

    #if (SW_DITHERING == 1)
        c0 = DitherPass(c0, tex);
    #endif

    Output.color = c0;

    return Output;
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
        CullMode = NONE;
        ShadeMode = Phong;
        AlphaBlendEnable = false;
        AlphaTestEnable = false;
        DitherEnable = true;
        SRGBWriteEnable = USE_SRGB;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
    }
}
