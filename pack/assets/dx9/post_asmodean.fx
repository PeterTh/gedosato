/*===============================================================================*\
|########################    [GSFx Shader Suite v2.00]    ########################|
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

//--------------------------#[CHOOSE EFFECTS]#--------------------------------\\

//##[POST EFFECTS]             [1=ON|0=OFF]
#define BLENDED_BLOOM               1      //# High Quality SP Bloom. Soft lighting with blending techniques, for a natural looking bloom.
#define SCENE_TONEMAPPING           1      //# HDR Scene Tonemapping. Layered component conversion, and applies scene tone mapping.
#define COLOR_CORRECTION            0      //# Multiform Color Correction. Colorspace conversion, with correction curves, and multiple palette types.
#define CROSS_PROCESSING            0      //# Filmic Cross Processing. Alters the tone of the scene, crossing the game's color set, with another.
#define GAMMA_CORRECTION            0      //# RGB Gamma Correction. Fixed expansion to variable compression gamma correction curve.
#define PIXEL_VIBRANCE              0      //# Pixel Vibrance. Intelligently adjusts pixel vibrance depending on original color saturation.
#define TEXTURE_SHARPEN             0      //# Bicubic Texture Unsharpen Mask. Looks similar to a negative texture LOD bias. Enhances texture fidelity.
#define CURVE_CONTRAST              0      //# S-Curve Scene Contrast Enhancement. Locally adjusts contrast using a four-point cubic bezier spline.
#define CEL_SHADING                 0      //# PX Cel Shading. Simulates the look of animation/toon. Typically best suited for animated style games.
#define PAINT_SHADING               0      //# Paint Shading. Creates the effect of a painted scene. Adapted from ENB series, it's pretty performance heavy. 
#define SW_DITHERING                0      //# SW Sub-pixel Dithering. Simulates more colors than your monitor can display. This can reduce color banding.

/*------------------------------------------------------------------------------
                          [EFFECT CONFIG OPTIONS]
------------------------------------------------------------------------------*/

//##[BLENDED BLOOM]
#define BloomType BlendGlow                //[BlendGlow, BlendAddGlow, BlendAddLight, BlendScreen, BlendLuma, BlendOverlay] The type of blended bloom. Light<->Dark.
#define BloomStrength 0.200                //[0.000 to 1.000] Overall strength of the bloom. You may want to readjust for each blend type.
#define BlendStrength 1.000                //[0.000 to 1.000] Strength of the blending. This is a modifier based on bloom. 1.0 equates to 100% strength.
#define BloomDefocus 2.000                 //[1.000 to 4.000] The initial bloom defocus value. Increases the softness of light, bright objects, etc.
#define BloomWidth 3.500                   //[1.000 to 8.000] Width of the bloom. Adjusts the width of the spread and soft glow. Scales with BloomStrength.
#define BloomReds 0.040                    //[0.000 to 1.000] Red channel correction of the bloom. Raising will increase the bloom of reds.
#define BloomGreens 0.023                  //[0.000 to 1.000] Green channel correction of the bloom. Raising will increase the bloom of greens.
#define BloomBlues 0.025                   //[0.000 to 1.000] Blue channel correction of the bloom. Raising will increase the bloom of blues.

//##[SCENE TONEMAPPING]
#define TonemapType 2                      //[0|1|2|3] The base tone mapping operator. 0 is LDR, 1 is HDR(original), 2 & 3 are Filmic HDR(slight grading).
#define TonemapMask 1                      //[0 or 1] Enables an ALU tone masking curve. Produces a nice cinematic look, but also lowers contrast range.
#define MaskStrength 0.32                  //[0.000 to 1.000] Strength of the tone masking. Higher for a stronger effect. This is a dependency of TonemapMask.
#define ToneAmount 0.320                   //[0.050 to 1.000] Tonemap strength (tone correction). Higher for stronger tone mapping, lower for lighter.
#define BlackLevels 0.060                  //[0.000 to 1.000] Black level balance (shadow correction). Increase to deepen blacks, lower to lighten them.
#define Exposure 1.000                     //[0.100 to 2.000] White correction (brightness). Higher values for more scene exposure, lower for less.
#define Luminance 1.000                    //[0.100 to 2.000] Luminance average (luminance correction). Higher values will lower scene luminance average.
#define WhitePoint 1.022                   //[0.100 to 2.000] Whitepoint average (wp lum correction). Higher values will lower the maximum scene white point.

//##[COLOR CORRECTION]
#define CorrectionPalette 3                //[1|2|3|4|5] The colorspace palette type. 1: RGB, 2: YXY, 3: XYZ, 4: HSV, 5: YUV. Each one will produce a different combination of shades & hues.
#define ChannelR 2.00                      //[0.00 to 8.00] R(1), Y(2), X(3), H(4), Y(5) component channel varies with the colorspace used. Higher values increase correction strength.
#define ChannelG 1.20                      //[0.00 to 8.00] G(1), X(2), Y(3), S(4), U(5) component channel varies with the colorspace used. Higher values increase correction strength.
#define ChannelB 1.60                      //[0.00 to 8.00] B(1), Y(2), Z(3), V(4), V(5) component channel varies with the colorspace used. Higher values increase correction strength.
#define PaletteStrength 1.00               //[0.00 to 2.00] The interpolated strength ratio between the base color, and the corrected color. Raise to increase saturation.

//##[CROSS PROCESSING]
#define FilmicProcess 1                    //[1|2|3] The color conversion type for the cross process. 1: cool, 2: warm, 3: dusk. You can achieve different results with each.
#define RedShift 0.55                      //[0.10 to 1.00] Red color component shift of the filmic processing. Alters the red balance of the shift.
#define GreenShift 0.50                    //[0.10 to 1.00] Green color component shift of the filmic processing. Alters the green balance of the shift.
#define BlueShift 0.50                     //[0.10 to 1.00] Blue color component shift of the filmic processing. Alters the blue balance of the shift.
#define ShiftRatio 0.25                    //[0.10 to 2.00] The blending ratio for the base color and the color shift. Higher for a stronger effect. 

//##[TEXTURE SHARPEN]
#define SharpenStrength 0.75               //[0.10 to 2.00] Strength of the texture sharpening effect. This is the maximum strength that will be used.
#define SharpenClamp 0.015                 //[0.005 to 0.500] Reduces the clamping/limiting on the maximum amount of sharpening each pixel recieves. Raise this to reduce the clamping.
#define SharpenBias 1.00                   //[1.00 to 4.00] Sharpening edge bias. Lower values for clean subtle sharpen, and higher values for a deeper textured sharpen.
#define DebugSharpen 0                     //[0 or 1] Visualize the sharpening effect. Useful for fine-tuning. Best to disable other effects, to see edge detection clearly.

//##[PIXEL VIBRANCE]
#define Vibrance 0.12                      //[-1.00 to 1.00] Overall vibrance strength. Locally adjusts the vibrance of pixels depending on their original saturation.
#define RedVibrance 1.00                   //[-8.00 to 8.00] Red channel coefficient of the vibrance strength. Adjusting the vibrance of the red channel independently.
#define GreenVibrance 1.00                 //[-8.00 to 8.00] Green channel coefficient of the vibrance strength. Adjusting the vibrance of the green channel independently.
#define BlueVibrance 1.00                  //[-8.00 to 8.00] Blue channel coefficient of the vibrance strength. Adjusting the vibrance of the blue channel independently.

//##[CEL SHADING]
#define EdgeStrength 1.00                  //[0.00 to 4.00] Overall strength of the cel edge outline effect. Affects the overall density.  0.00: no outlines.
#define EdgeFilter 0.60                    //[0.10 to 2.00] Filters out fainter cel edges. Use it for balancing the cel edge density. EG: for faces, foliage, etc.
#define EdgeThickness 1.00                 //[0.50 to 4.00] Thickness of the cel edges. Increase for thicker outlining.  Note: when downsampling, raise this to keep the same thickness.
#define PaletteType 2                      //[1|2|3] The color palette to use. 1 is Game Original, 2 is Animated Shading, 3 is Water Painting (Default is 2: Animated Shading).
#define UseYuvLuma 0                       //[0 or 1] Uses YUV luma calculations, or base color luma calculations. Yuv luma can produce a better shaded look.
#define LumaConversion 1                   //[0 or 1] Uses BT.601, or BT.709, RGB<-YUV->RGB conversions. Some games prefer 601, but most prefer 709. BT.709 is typically recommended. 
#define ColorRounding 1                    //[0 or 1] Uses rounding methods on colors. This can emphasise shaded toon colors. Looks good in some games, and odd in others.

//##[PAINT SHADING]
#define PaintRadius 4                      //[2 to 8] Radius of the painted effect, increasing the radius also requires longer loops, so higher values require more performance.
#define PaintMethod 2                      //[1 or 2] The algorithm used for paint effect. 1: water painting, 2: oil painting. You may want to readjust the radius between the two.
#define PaintStrength 1.00                 //[0.00 to 1.00] The overall interpolated strength of the paint effect. Where 1.0 equates to 100% strength.

//##[GAMMA CORRECTION]
#define Gamma 2.20                         //[1.5 to 4.0] Gamma correction. Decrease for lower gamma(darker). Increase for higher gamma(brighter). (Default: 2.2)

//##[CURVE CONTRAST]
#define Contrast 0.35                      //[0.00 to 2.00] The amount of contrast you want. Controls the overall contrast strength.

//##[SUBPIXEL DITHERING]
#define DitherMethod 2                     //[1 or 2] 1: Ordered grid dithering(faster), 2: time-based random dithering(higher quality). Hardware dithering is also enabled by default.
#define ShowMethod 0                       //[0 or 1] Shows the dithering method, based of the type of dithering selected. Useful for debugging, and confirmation of working order.

//##[END OF USER OPTIONS]

#ifndef USE_SRGB
#define USE_SRGB true
#endif

/*------------------------------------------------------------------------------
                             [GLOBALS/FUNCTIONS]
------------------------------------------------------------------------------*/

static float Epsilon = 1e-10;
static float2 pixelSize = PIXEL_SIZE;
static float2 screenSize = SCREEN_SIZE;
static const float3 lumCoeff = float3(0.2126729, 0.7151522, 0.0721750);

Texture2D thisframeTex;
sampler s0 = sampler_state
{
    Texture = <thisframeTex>;
    MagFilter = Linear;
    MinFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
    MaxAnisotropy = 16;
    SRGBTexture = USE_SRGB;
};

//Conversion matrices
float3 RGBtoXYZ(float3 rgb)
{
    static const float3x3 m = {
    0.4124564, 0.3575761, 0.1804375,
    0.2126729, 0.7151522, 0.0721750,
    0.0193339, 0.1191920, 0.9503041 };

    return mul(m, rgb);
}

float3 XYZtoRGB(float3 xyz)
{
    static const float3x3 m = {
    3.2404542,-1.5371385,-0.4985314,
   -0.9692660, 1.8760108, 0.0415560,
    0.0556434,-0.2040259, 1.0572252 };

    return mul(m, xyz);
}

float3 RGBtoYUV(float3 RGB)
{
    static const float3x3 m = {
    0.2126, 0.7152, 0.0722,
   -0.09991,-0.33609, 0.436,
    0.615, -0.55861, -0.05639 };

    return mul(m, RGB);
}

float3 YUVtoRGB(float3 YUV)
{
    static const float3x3 m = {
    1.000, 0.000, 1.28033,
    1.000,-0.21482,-0.38059,
    1.000, 2.12798, 0.000 };

    return mul(m, YUV);
}

//Converting XYZ to Yxy
float3 XYZtoYxy(float3 xyz)
{
    float w = (xyz.r + xyz.g + xyz.b);
    float3 Yxy;

    Yxy.r = xyz.g;
    Yxy.g = xyz.r / w;
    Yxy.b = xyz.g / w;

    return Yxy;
}

//Converting Yxy to XYZ
float3 YxytoXYZ(float3 Yxy)
{
    float3 xyz;
    xyz.g = Yxy.r;
    xyz.r = Yxy.r * Yxy.g / Yxy.b;
    xyz.b = Yxy.r * (1.0 - Yxy.g - Yxy.b) / Yxy.b;

    return xyz;
}

//Average relative luminance
float AvgLuminance(float3 color)
{
    return sqrt(
    (color.x * color.x * lumCoeff.x) +
    (color.y * color.y * lumCoeff.y) +
    (color.z * color.z * lumCoeff.z));
}

float smootherstep(float a, float b, float x)
{
    x = saturate((x - a) / (b - a));
    return x*x*x*(x*(x * 6 - 15) + 10);
}

/*
float4 DebugClipping(float4 color)
{
    if (color.x >= 0.99999 && color.y >= 0.99999 &&
    color.z >= 0.99999) color.xyz = float3(1.0f, 0.0f, 0.0f);

    if (color.x <= 0.00001 && color.y <= 0.00001 &&
    color.z <= 0.00001) color.xyz = float3(0.0f, 0.0f, 1.0f);

    return color;
}
*/

struct VS_INPUT
{
    float4 vertPos : POSITION;
    float2 UVCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 vertPos : POSITION;
    float2 UVCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Color : COLOR;
};

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

#if GAMMA_CORRECTION == 1
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

#if BLENDED_BLOOM == 1
float3 BlendAddLight(float3 bloom, float3 blend)
{
    return saturate(bloom + blend);
}

float3 BlendScreen(float3 bloom, float3 blend)
{
    return (bloom + blend) - (bloom * blend);
}

float3 BlendGlow(float3 bloom, float3 blend)
{
    float glow = AvgLuminance(bloom);
    return lerp((bloom + blend) - (bloom * blend),
    (blend + blend) - (blend * blend), glow);
}

float3 BlendAddGlow(float3 bloom, float3 blend)
{
    float glow = smootherstep(0.0, 1.0, AvgLuminance(bloom));
    return lerp(saturate(bloom + blend),
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

float3 BloomCorrection(float3 color)
{
    float3 bloom = color;

    bloom.r = 2.0 / 3.0 * (1.0 - (bloom.r * bloom.r));
    bloom.g = 2.0 / 3.0 * (1.0 - (bloom.g * bloom.g));
    bloom.b = 2.0 / 3.0 * (1.0 - (bloom.b * bloom.b));

    bloom.r = saturate(color.r + float(BloomReds) * bloom.r);
    bloom.g = saturate(color.g + float(BloomGreens) * bloom.g);
    bloom.b = saturate(color.b + float(BloomBlues) * bloom.b);

    color = saturate(bloom);

    return color;
}

float4 DefocusFilter(sampler tex, float2 texcoord, float2 defocus)
{
    float2 buffer = screenSize;
    float2 texel = pixelSize * defocus;

    int tcx = int(texcoord.x * buffer.x);
    int tcy = int(texcoord.y * buffer.y);

    float2 nuv = float2((tcx + 0.5) / buffer.x, (tcy + 0.5) / buffer.y);

    float4 sampleA = tex2D(tex, nuv + float2(0.5, 0.5) * texel);
    float4 sampleB = tex2D(tex, nuv + float2(-0.5, 0.5) * texel);
    float4 sampleC = tex2D(tex, nuv + float2(0.5, -0.5) * texel);
    float4 sampleD = tex2D(tex, nuv + float2(-0.5, -0.5) * texel);

    float fx = frac(texcoord.x * buffer.x);
    float fy = frac(texcoord.y * buffer.y);

    float4 interpolateA = lerp(sampleA, sampleB, fx);
    float4 interpolateB = lerp(sampleC, sampleD, fx);

    return lerp(interpolateA, interpolateB, fy);
}

float4 BloomPass(float4 color, float2 texcoord)
{
    float anflare = 4.0;

    float2 defocus = float2(BloomDefocus, BloomDefocus);
    float4 bloom = DefocusFilter(s0, texcoord, defocus);

    float2 dx = float2(pixelSize.x * float(BloomWidth), 0.0);
    float2 dy = float2(0.0, pixelSize.y * float(BloomWidth));

    float2 mdx = mul(defocus.x, dx);
    float2 mdy = mul(defocus.y, dy);

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
                      [SCENE TONE MAPPING CODE SECTION]
------------------------------------------------------------------------------*/

#if SCENE_TONEMAPPING == 1
float3 TmMask(float3 color)
{
    float3 tone = color;

    float highTone = 6.2; float greyTone = 0.5;
    float midTone = 1.620; float lowTone = 0.066;

    tone.r = (tone.r * (highTone * tone.r + greyTone))/(
    tone.r * (highTone * tone.r + midTone) + lowTone);
    tone.g = (tone.g * (highTone * tone.g + greyTone))/(
    tone.g * (highTone * tone.g + midTone) + lowTone);
    tone.b = (tone.b * (highTone * tone.b + greyTone))/(
    tone.b * (highTone * tone.b + midTone) + lowTone);

    static const float gamma = 2.42;
    tone = EncodeGamma(tone, gamma);

    color = lerp(color, tone, float(MaskStrength));

    return color;
}

float3 TmCurve(float3 color)
{
    float3 T = color;
    float tnamn = ToneAmount;

    float A = 0.100; float B = 0.300;
    float C = 0.100; float D = tnamn;
    float E = 0.020; float F = 0.300;

    float W = 1.012;

    T.r = ((T.r*(A*T.r + C*B) + D*E) / (T.r*(A*T.r + B) + D*F)) - E / F;
    T.g = ((T.g*(A*T.g + C*B) + D*E) / (T.g*(A*T.g + B) + D*F)) - E / F;
    T.b = ((T.b*(A*T.b + C*B) + D*E) / (T.b*(A*T.b + B) + D*F)) - E / F;

    float denom = ((W*(A*W + C*B) + D*E) / (W*(A*W + B) + D*F)) - E / F;
    float3 white = float3(denom, denom, denom);

    T = T / white;
    color = saturate(T);

    return color;
}

float4 TonemapPass(float4 color, float2 texcoord)
{
    float3 tonemap = color.rgb;
    
    float blackLevel = length(tonemap);
    float luminanceAverage = AvgLuminance(Luminance);
    
    float shadowmask = pow(saturate(blackLevel), float(BlackLevels));
    tonemap = tonemap * shadowmask;

    if (TonemapMask == 1) { tonemap = TmMask(tonemap); }
    if (TonemapType == 1) { tonemap = TmCurve(tonemap); }

    // RGB -> XYZ conversion
    static const float3x3 RGB2XYZ = { 0.4124564, 0.3575761, 0.1804375,
                                      0.2126729, 0.7151522, 0.0721750,
                                      0.0193339, 0.1191920, 0.9503041 };

    float3 XYZ = mul(RGB2XYZ, tonemap);

    // XYZ -> Yxy conversion
    float3 Yxy;

    Yxy.r = XYZ.g;                                  // copy luminance Y
    Yxy.g = XYZ.r / (XYZ.r + XYZ.g + XYZ.b);        // x = X / (X + Y + Z)
    Yxy.b = XYZ.g / (XYZ.r + XYZ.g + XYZ.b);        // y = Y / (X + Y + Z)

    // (Wt) Tone mapped scaling of the initial wp before input modifiers
    float Wt = saturate(Yxy.r / AvgLuminance(XYZ));

    if (TonemapType == 2) { Yxy.r = TmCurve(Yxy).r; }

    // (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
    float Lp = Yxy.r * float(Exposure) / (luminanceAverage + Epsilon);

    // (Wp) White point calculated, based on the toned white, and input modifier
    float Wp = dot(abs(Wt), float(WhitePoint));

    // (Ld) Scale all luminance within a displayable range of 0 to 1
    Yxy.r = (Lp * (1.0 + Lp / (Wp * Wp))) / (1.0 + Lp);

    // Yxy -> XYZ conversion
    XYZ.r = Yxy.r * Yxy.g / Yxy.b;                  // X = Y * x / y
    XYZ.g = Yxy.r;                                  // copy luminance Y
    XYZ.b = Yxy.r * (1.0 - Yxy.g - Yxy.b) / Yxy.b;  // Z = Y * (1-x-y) / y

    if (TonemapType == 3) { XYZ = TmCurve(XYZ); }

    // XYZ -> RGB conversion
    static const float3x3 XYZ2RGB = { 3.2404542,-1.5371385,-0.4985314,
                                     -0.9692660, 1.8760108, 0.0415560,
                                      0.0556434,-0.2040259, 1.0572252 };

    tonemap = mul(XYZ2RGB, XYZ);

    color.rgb = tonemap;
    color.a = AvgLuminance(color.rgb);

    return color;
}
#endif

/*------------------------------------------------------------------------------
                      [CROSS PROCESSING CODE SECTION]
------------------------------------------------------------------------------*/

#if CROSS_PROCESSING == 1
float3 CrossShift(float3 color)
{
    float3 cross;

    float2 CrossMatrix[3] = {
    float2 (0.960, 0.040 * color.x),
    float2 (0.980, 0.020 * color.y),
    float2 (0.970, 0.030 * color.z), };

    cross.x = float(RedShift) * CrossMatrix[0].x + CrossMatrix[0].y;
    cross.y = float(GreenShift) * CrossMatrix[1].x + CrossMatrix[1].y;
    cross.z = float(BlueShift) * CrossMatrix[2].x + CrossMatrix[2].y;

    float lum = AvgLuminance(color);
    float3 black = float3(0.0, 0.0, 0.0);
    float3 white = float3(1.0, 1.0, 1.0);

    cross = lerp(black, cross, saturate(lum * 2.0));
    cross = lerp(cross, white, saturate(lum - 0.5) * 2.0);
    color = lerp(color, cross, saturate(lum * float(ShiftRatio)));

    return color;
}

float4 CrossPass(float4 color, float2 texcoord)
{
    #if FilmicProcess == 1
    color.rgb = CrossShift(color.rgb);

    #elif FilmicProcess == 2
    float3 XYZ = RGBtoXYZ(color.rgb);
    float3 Yxy = XYZtoYxy(XYZ);

    Yxy = CrossShift(Yxy);
    XYZ = YxytoXYZ(Yxy);

    color.rgb = XYZtoRGB(XYZ);

    #elif FilmicProcess == 3
    float3 XYZ = RGBtoXYZ(color.rgb);
    float3 Yxy = XYZtoYxy(XYZ);

    XYZ = YxytoXYZ(Yxy);
    XYZ = CrossShift(XYZ);

    color.rgb = XYZtoRGB(XYZ);
    #endif

    color.a = AvgLuminance(color.rgb);

    return saturate(color);
}
#endif

/*------------------------------------------------------------------------------
                      [COLOR CORRECTION CODE SECTION]
------------------------------------------------------------------------------*/

#if COLOR_CORRECTION == 1
// Converting pure hue to RGB
float3 HUEtoRGB(float H)
{
    float R = abs(H * 6.0 - 3.0) - 1.0;
    float G = 2.0 - abs(H * 6.0 - 2.0);
    float B = 2.0 - abs(H * 6.0 - 4.0);

    return saturate(float3(R, G, B));
}

// Converting RGB to hue/chroma/value
float3 RGBtoHCV(float3 RGB)
{
    float4 BG = float4(RGB.bg,-1.0, 2.0 / 3.0);
    float4 GB = float4(RGB.gb, 0.0,-1.0 / 3.0);

    float4 P = (RGB.g < RGB.b) ? BG : GB;
    float4 XY = float4(P.xyw, RGB.r);
    float4 YZ = float4(RGB.r, P.yzx);

    float4 Q = (RGB.r < P.x) ? XY : YZ;
    float C = Q.x - min(Q.w, Q.y);
    float H = abs((Q.w - Q.y) / (6.0 * C + Epsilon) + Q.z);

    return float3(H, C, Q.x);
}

// Converting RGB to HSV
float3 RGBtoHSV(float3 RGB)
{
    float3 HCV = RGBtoHCV(RGB);
    float S = HCV.y / (HCV.z + Epsilon);

    return float3(HCV.x, S, HCV.z);
}

// Converting HSV to RGB
float3 HSVtoRGB(float3 HSV)
{
    float3 RGB = HUEtoRGB(HSV.x);
    return ((RGB - 1.0) * HSV.y + 1.0) * HSV.z;
}

// Pre correction color mask
float3 PreCorrection(float3 color)
{
    float3 RGB = color;

    RGB.r = 2.0 / 3.0 * (1.0 - (RGB.r * RGB.r));
    RGB.g = 2.0 / 3.0 * (1.0 - (RGB.g * RGB.g));
    RGB.b = 2.0 / 3.0 * (1.0 - (RGB.b * RGB.b));

    RGB.r = saturate(color.r + (float(ChannelR) / 200.0) * RGB.r);
    RGB.g = saturate(color.g + (float(ChannelG) / 200.0) * RGB.g);
    RGB.b = saturate(color.b + (float(ChannelB) / 200.0) * RGB.b);

    color = saturate(RGB);

    return color;
}

float3 ColorCorrection(float3 color)
{
    float X = 1.0 / (1.0 + exp(float(ChannelR) / 2.0));
    float Y = 1.0 / (1.0 + exp(float(ChannelG) / 2.0));
    float Z = 1.0 / (1.0 + exp(float(ChannelB) / 2.0));

    color.r = (1.0 / (1.0 + exp(float(-ChannelR) * (color.r - 0.5))) - X) / (1.0 - 2.0 * X);
    color.g = (1.0 / (1.0 + exp(float(-ChannelG) * (color.g - 0.5))) - Y) / (1.0 - 2.0 * Y);
    color.b = (1.0 / (1.0 + exp(float(-ChannelB) * (color.b - 0.5))) - Z) / (1.0 - 2.0 * Z);

    return saturate(color);
}

float4 CorrectionPass(float4 color, float2 texcoord)
{
    float3 colorspace = PreCorrection(color.rgb);

    #if CorrectionPalette == 1
    colorspace = ColorCorrection(colorspace);

    #elif CorrectionPalette == 2
    float3 XYZ = RGBtoXYZ(colorspace);
    float3 Yxy = XYZtoYxy(XYZ);

    Yxy = ColorCorrection(Yxy);
    XYZ = YxytoXYZ(Yxy);
    colorspace = XYZtoRGB(XYZ);

    #elif CorrectionPalette == 3
    float3 XYZ = RGBtoXYZ(colorspace);
    float3 Yxy = XYZtoYxy(XYZ);

    XYZ = YxytoXYZ(Yxy);
    XYZ = ColorCorrection(XYZ);
    colorspace = XYZtoRGB(XYZ);

    #elif CorrectionPalette == 4
    float3 hsv = RGBtoHSV(colorspace);
    hsv = ColorCorrection(hsv);
    colorspace = HSVtoRGB(hsv);

    #elif CorrectionPalette == 5
    float3 yuv = RGBtoYUV(colorspace);
    yuv = ColorCorrection(yuv);
    colorspace = YUVtoRGB(yuv);
    #endif

    color.rgb = lerp(color.rgb, colorspace, float(PaletteStrength));
    color.a = AvgLuminance(color.rgb);

    return color;
}
#endif

/*------------------------------------------------------------------------------
                       [TEXTURE SHARPEN CODE SECTION]
------------------------------------------------------------------------------*/

#if TEXTURE_SHARPEN == 1
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

    float2 uvCoord = float2(float(nX) / screenSize.x,
                            float(nY) / screenSize.y);

    for (int m = -1; m <= 2; m++) {
    for (int n = -1; n <= 2; n++) {

    float4 Samples = tex2D(texSample, uvCoord +
    float2(texelSizeX * float(m), texelSizeY * float(n)));

    float vc1 = Cubic(float(m) - a);
    float4 vecCoeff1 = float4(vc1, vc1, vc1, vc1);

    float vc2 = Cubic(-(float(n) - b));
    float4 vecCoeff2 = float4(vc2, vc2, vc2, vc2);

    nSum = nSum + (Samples * vecCoeff2 * vecCoeff1);
    nDenom = nDenom + (vecCoeff2 * vecCoeff1); }}
    nSum = nSum / nDenom;

    return nSum;
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

    #if DebugSharpen == 1
        color = saturate(0.5f + (sharpenLuma * 4)).xxxx;
    #endif

    return saturate(color);
}
#endif

/*------------------------------------------------------------------------------
                         [CEL SHADING CODE SECTION]
------------------------------------------------------------------------------*/

#if CEL_SHADING == 1
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

        #if ColorRounding == 1
        col[i].r = round(col[i].r * thresholds.r) / thresholds.r;
        col[i].g = round(col[i].g * thresholds.g) / thresholds.g;
        col[i].b = round(col[i].b * thresholds.b) / thresholds.b;
        #endif

        lum[i] = AvgLuminance(col[i].xyz);
        yuv = RGBtoYUV(col[i]);

        #if UseYuvLuma == 0
        yuv.r = round(yuv.r * thresholds.r) / thresholds.r;
        #else
        yuv.r = saturate(round(yuv.r * lum[i]) / thresholds.r + lum[i]);
        #endif
        
        yuv = YUVtoRGB(yuv);
        sum += yuv;
    }

    float3 shadedColor = (sum / NUM);
    float2 pixel = float2(pixelSize.x * EdgeThickness, pixelSize.y * EdgeThickness);

    float edgeX = dot(tex2D(s0, uv0 + pixel).rgb, lumCoeff);
    edgeX = dot(float4(tex2D(s0, uv0 - pixel).rgb, edgeX), float4(lumCoeff, -1.0));

    float edgeY = dot(tex2D(s0, uv0 + float2(pixel.x, -pixel.y)).rgb, lumCoeff);
    edgeY = dot(float4(tex2D(s0, uv0 + float2(-pixel.x, pixel.y)).rgb, edgeY), float4(lumCoeff, -1.0));

    float edge = dot(float2(edgeX, edgeY), float2(edgeX, edgeY));

    #if PaletteType == 1
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
                        [PAINT SHADING CODE SECTION]
------------------------------------------------------------------------------*/

#if PAINT_SHADING == 1
float3 PaintShading(float3 color, float2 texcoord)
{
    #if PaintMethod == 1
    float2	tex;
    int		k, j, lum, cmax = 0;

    float	C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
    float3	A, B, C, D, E, F, G, H, I, J, shade;

    for (k = int(-PaintRadius); k < (int(PaintRadius) + 1); k++){
    for (j = int(-PaintRadius); j < (int(PaintRadius) + 1); j++){

    tex.x = texcoord.x + pixelSize.x * k;
    tex.y = texcoord.y + pixelSize.y * j;

    shade = tex2D(s0, tex).xyz;

    lum = AvgLuminance(shade) * 9.0;

    C0 = (lum == 0) ? C0 + 1 : C0;
    C1 = (lum == 1) ? C1 + 1 : C1;
    C2 = (lum == 2) ? C2 + 1 : C2;
    C3 = (lum == 3) ? C3 + 1 : C3;
    C4 = (lum == 4) ? C4 + 1 : C4;
    C5 = (lum == 5) ? C5 + 1 : C5;
    C6 = (lum == 6) ? C6 + 1 : C6;
    C7 = (lum == 7) ? C7 + 1 : C7;
    C8 = (lum == 8) ? C8 + 1 : C8;
    C9 = (lum == 9) ? C9 + 1 : C9;

    A = (lum == 0) ? A + shade : A;
    B = (lum == 1) ? B + shade : B;
    C = (lum == 2) ? C + shade : C;
    D = (lum == 3) ? D + shade : D;
    E = (lum == 4) ? E + shade : E;
    F = (lum == 5) ? F + shade : F;
    G = (lum == 6) ? G + shade : G;
    H = (lum == 7) ? H + shade : H;
    I = (lum == 8) ? I + shade : I;
    J = (lum == 9) ? J + shade : J;
    }}

    if (C0 > cmax){ cmax = C0; color.xyz = A / cmax; }
    if (C1 > cmax){ cmax = C1; color.xyz = B / cmax; }
    if (C2 > cmax){ cmax = C2; color.xyz = C / cmax; }
    if (C3 > cmax){ cmax = C3; color.xyz = D / cmax; }
    if (C4 > cmax){ cmax = C4; color.xyz = E / cmax; }
    if (C5 > cmax){ cmax = C5; color.xyz = F / cmax; }
    if (C6 > cmax){ cmax = C6; color.xyz = G / cmax; }
    if (C7 > cmax){ cmax = C7; color.xyz = H / cmax; }
    if (C8 > cmax){ cmax = C8; color.xyz = I / cmax; }
    if (C9 > cmax){ cmax = C9; color.xyz = J / cmax; }

    #else
    int j, i;

    float3 m0, m1, m2, m3, k0, k1, k2, k3, shade;
    float n = float((PaintRadius + 1.0) * (PaintRadius + 1.0));

    for (j = int(-PaintRadius); j <= 0; ++j)  {
    for (i = int(-PaintRadius); i <= 0; ++i)  {

    shade = tex2D(s0, texcoord + float2(i, j) / screenSize).rgb;
    m0 += shade; k0 += shade * shade; }}

    for (j = int(-PaintRadius); j <= 0; ++j) {
    for (i = 0; i <= int(PaintRadius); ++i)  {
    shade = tex2D(s0, texcoord + float2(i, j) / screenSize).rgb;
    m1 += shade; k1 += shade * shade; }}

    for (j = 0; j <= int(PaintRadius); ++j)  {
    for (i = 0; i <= int(PaintRadius); ++i)  {
    shade = tex2D(s0, texcoord + float2(i, j) / screenSize).rgb;
    m2 += shade; k2 += shade * shade; }}

    for (j = 0; j <= int(PaintRadius); ++j)  {
    for (i = int(-PaintRadius); i <= 0; ++i) {
    shade = tex2D(s0, texcoord + float2(i, j) / screenSize).rgb;
    m3 += shade; k3 += shade * shade; }}

    float min_sigma2 = 1e+2;
    m0 /= n; k0 = abs(k0 / n - m0 * m0);

    float sigma2 = k0.r + k0.g + k0.b;
    if (sigma2 < min_sigma2) {
    min_sigma2 = sigma2; color = m0; }

    m1 /= n; k1 = abs(k1 / n - m1 * m1);
    sigma2 = k1.r + k1.g + k1.b;

    if (sigma2 < min_sigma2) {
    min_sigma2 = sigma2;
    color = m1; }

    m2 /= n; k2 = abs(k2 / n - m2 * m2);
    sigma2 = k2.r + k2.g + k2.b;

    if (sigma2 < min_sigma2) {
    min_sigma2 = sigma2;
    color = m2; }

    m3 /= n;
    k3 = abs(k3 / n - m3 * m3);

    sigma2 = k3.r + k3.g + k3.b;
    if (sigma2 < min_sigma2) {
    min_sigma2 = sigma2;
    color = m3; }
    #endif

    return color;
}

float4 PaintPass(float4 color, float2 texcoord)
{
    float3 paint = PaintShading(color.rgb, texcoord);
    color.rgb = lerp(color.rgb, paint, float(PaintStrength));
    color.a = AvgLuminance(color.rgb);

    return color;
}
#endif

/*------------------------------------------------------------------------------
                      [SUBPIXEL DITHERING CODE SECTION]
------------------------------------------------------------------------------*/

#if SW_DITHERING == 1
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

float Randomize(float2 texcoord)
{
    float2 tex = CoordRot(texcoord, timer);

    float seed = dot(tex, float2(12.9898, 78.233));
    float sine = sin(seed);
    float noise = frac(sine * 43758.5453);

    return noise;
}

float4 DitherPass(float4 color, float2 texcoord)
{
    float ditherBits = 8.0;

    #if DitherMethod == 2       //random dithering

    float noise = Randomize(texcoord);
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

#if CURVE_CONTRAST == 1
float4 ContrastPass(float4 color, float2 texcoord)
{
    float3 luma = (float3)AvgLuminance(color.rgb);
    float3 chroma = color.rgb - luma;
    float3 x = luma;

    //S-Curve - Cubic Bezier spline
    float3 a = float3(0.00, 0.00, 0.00);
    float3 b = float3(0.25, 0.25, 0.25);
    float3 c = float3(0.85, 0.85, 0.85);
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

#if PIXEL_VIBRANCE == 1
float4 VibrancePass(float4 color, float2 texcoord)
{
    float vib = Vibrance;
    float luma = AvgLuminance(color.rgb);

    float colorMax = max(color.r, max(color.g, color.b));
    float colorMin = min(color.r, min(color.g, color.b));

    float colorSaturation = colorMax - colorMin;
    float3 colorCoeff = float3(RedVibrance * vib, GreenVibrance * vib, BlueVibrance * vib);

    color.rgb = lerp(luma, color.rgb, (1.0 + (colorCoeff * (1.0 - (sign(colorCoeff) * colorSaturation)))));
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

    #if TEXTURE_SHARPEN == 1
    c0 = TexSharpenPass(c0, tex);
    #endif

    #if PAINT_SHADING == 1
    c0 = PaintPass(c0, tex);
    #endif

    #if CEL_SHADING == 1
    c0 = CelPass(c0, tex);
    #endif

    #if GAMMA_CORRECTION == 1
    c0 = GammaPass(c0, tex);
    #endif

    #if PIXEL_VIBRANCE == 1
    c0 = VibrancePass(c0, tex);
    #endif

    #if COLOR_CORRECTION == 1
    c0 = CorrectionPass(c0, tex);
    #endif

    #if CROSS_PROCESSING == 1
    c0 = CrossPass(c0, tex);
    #endif

    #if SCENE_TONEMAPPING == 1
    c0 = TonemapPass(c0, tex);
    #endif

    #if BLENDED_BLOOM == 1
    c0 = BloomPass(c0, tex);
    #endif

    #if CURVE_CONTRAST == 1
    c0 = ContrastPass(c0, tex);
    #endif

    #if SW_DITHERING == 1
    c0 = DitherPass(c0, tex);
    #endif

    Output.Color = c0;

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
        CullMode = None;
        ShadeMode = Phong;
        AlphaBlendEnable = false;
        AlphaTestEnable = false;
        DitherEnable = true;
        SRGBWriteEnable = USE_SRGB;
        ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
    }
}
