#pragma once
#include <cmath>
#include <cstdint>

namespace th10::host {
// A display-only exception to the existing 10 ms final-presentation gate.
// It does not admit a tick, change network pacing, or suppress authored Draw.
// The caller supplies the scale actually used by this callback's schedule.
// A normal input gap needs the same complete-forward interpolation opportunity
// as a deep gap. Admission depends on remaining work and authored boundaries,
// never on creating additional prediction or slowing an otherwise ready peer.
struct NetworkTailPresentation final {
    enum BoundaryFlag : std::uint32_t {
        NoAuthoredDraw=1u<<0,
        IncompleteAuthoredFrame=1u<<1,
        NotLiveForward=1u<<2,
        UnavailableApplication=1u<<3,
        LoadingOrTransition=1u<<4,
        FrozenOrPaused=1u<<5,
        CurrentStageBoundary=1u<<6,
        PreviousStageBoundary=1u<<7,
        CaptureRequested=1u<<8,
        ScreenshotRequested=1u<<9,
        SceneIdentityChanged=1u<<10,
        DeviceReset=1u<<11,
        RepeatedAuthoredDraw=1u<<12
    };
    static constexpr std::uint32_t Unobserved=NoAuthoredDraw|IncompleteAuthoredFrame;
    // These values are also the schema-2 CallbackTiming diagnostic codes.
    enum class Decision : unsigned {
        NotEvaluated=0,
        Accept=1,
        Disabled=2,
        NotRealFastSample=3,
        NotHighRefresh=4,
        NotSingleForward=5,
        IncompleteFrontier=6,
        InputGapNotDeep=7, // Reserved historical r10h rejection; never reassigned.
        AlphaNotInterior=8,
        GateOutsideWindow=9,
        AuthoredBoundary=10,
        InvalidSample=11
    };
    struct Sample {
        bool enabled=false,real_fast_sample=false,high_refresh=false;
        unsigned completed_forwards=0;
        bool complete_frontier=false;
        double input_gap_scale=1.,schedule_alpha=1.,gate_ms=0.;
        std::uint32_t boundary_flags=Unobserved;
    };
    template<class Stage> static bool StageHasAuthoredBoundary(const Stage* stage) noexcept {
        // Read the real Stage before AND after Draw. Timer::previous can keep
        // the last fade value indefinitely, so it is not a boundary predicate.
        return !stage||!stage->file||!stage->animation_file||
            (stage->draw_flags&0xfu)!=1u||stage->fade_timer.current>0||
            stage->fade_timer.fractional>0.f||(stage->fade_color>>24)!=0||stage->frame_effect!=0;
    }
    static Decision Decide(const Sample& sample) noexcept {
        if(!sample.enabled)return Decision::Disabled;
        if(!sample.real_fast_sample)return Decision::NotRealFastSample;
        if(!sample.high_refresh)return Decision::NotHighRefresh;
        if(sample.completed_forwards!=1)return Decision::NotSingleForward;
        if(!sample.complete_frontier)return Decision::IncompleteFrontier;
        if(!std::isfinite(sample.input_gap_scale)||sample.input_gap_scale<1.||!std::isfinite(sample.schedule_alpha)||
           !std::isfinite(sample.gate_ms))return Decision::InvalidSample;
        if(!(sample.schedule_alpha>0.&&sample.schedule_alpha<1.))return Decision::AlphaNotInterior;
        // The renderer consumes float alpha. Rounding an interior double to
        // an endpoint must not present that endpoint while keeping it primed.
        const auto rendered_alpha=static_cast<float>(sample.schedule_alpha);
        if(!(rendered_alpha>0.f&&rendered_alpha<1.f))return Decision::AlphaNotInterior;
        if(!(sample.gate_ms>=10.&&sample.gate_ms<14.))return Decision::GateOutsideWindow;
        if(sample.boundary_flags)return Decision::AuthoredBoundary;
        return Decision::Accept;
    }
};
}
