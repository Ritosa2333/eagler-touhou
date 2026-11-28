#pragma once
#include "GraphicsState.hpp"

namespace touhou::sdl {
enum class UnormBlendProfile { NativeGles, SeparateRgbTerms };

inline bool alpha_factor_reads_source_rgb(graphics::BlendFactor factor) {
    using namespace graphics;
    return factor == BlendFactor::SourceColor || factor == BlendFactor::InverseSourceColor;
}

inline bool separate_rgb_terms(UnormBlendProfile profile, const graphics::PipelineState& pipeline,
                               graphics::PixelFormat target_format) {
    using namespace graphics;
    const bool alpha_factors_safe =
        !pipeline.separateAlphaBlend || (!alpha_factor_reads_source_rgb(pipeline.sourceAlphaBlend) &&
                                         !alpha_factor_reads_source_rgb(pipeline.destinationAlphaBlend));
    return profile == UnormBlendProfile::SeparateRgbTerms && target_format == PixelFormat::Bgra8 && pipeline.blend &&
           alpha_factors_safe && pipeline.blendEquation == BlendEquation::Add &&
           pipeline.sourceBlend == BlendFactor::SourceAlpha &&
           (pipeline.destinationBlend == BlendFactor::InverseSourceAlpha ||
            pipeline.destinationBlend == BlendFactor::One || pipeline.destinationBlend == BlendFactor::Zero);
}

inline constexpr const char* separateRgbTermsShader = R"GLSL(
 // Quantize the source term before the fixed-function destination term is
 // added. RGB source blend becomes ONE; alpha retains the original factors.
 // Preweighting makes the source term an integer q, so GL's single final
 // UNORM rounding equals the per-term sum:
 // round(q + dst*(255-a)/255) == q + round(dst*(255-a)/255).
 // Source RGBA is quantized before blending, including modulated alpha.
 // Destination alpha still uses GL's alpha equation and is not certified;
 // Renderer limits this profile to an opaque display, never alpha-bearing RTs.
 if(separateRgbTerms){vec3 source8=floor(c.rgb*255.0+0.5);
   float alpha8=floor(c.a*255.0+0.5);vec3 product=source8*alpha8;
   c.rgb=floor((product+128.0+floor(product/256.0))/256.0)/255.0;
   c.a=alpha8/255.0;}
)GLSL";
}
