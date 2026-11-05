// Executable contract for the shared display cadence headers.
#include "FrameCadence.hpp"
#include "PresentationCadence.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace touhou::sdl;
int main(){
    // 120 Hz display: one fixed tick every two callbacks, no catch-up.
    {
        FrameCadence cadence;unsigned ticks=0;unsigned single=0;double min_alpha=1,max_alpha=0;
        for(int frame=0;frame<240;++frame){
            const unsigned n=cadence.advance(1./120.);
            assert(n<=1);
            ticks+=n;single+=n;
            const double alpha=cadence.interpolation_alpha();
            assert(alpha>=0&&alpha<=1);
            min_alpha=std::fmin(min_alpha,alpha);max_alpha=std::fmax(max_alpha,alpha);
        }
        assert(ticks>=119&&ticks<=121);
        assert(min_alpha<.01&&max_alpha>.49);
    }
    // 30 Hz display: overdue deadlines are dropped, never replayed.
    {
        FrameCadence cadence;unsigned ticks=0;
        for(int frame=0;frame<30;++frame)ticks+=cadence.advance(1./30.);
        // One tick per callback: overdue slots are discarded, so a 30 Hz host
        // advances 30 logical ticks per second rather than replaying 60.
        assert(ticks==30);
    }
    // A one second stall advances exactly one tick.
    {
        FrameCadence cadence;
        assert(cadence.advance(1.0)==1);
        assert(cadence.advance(1.0)==1);
    }
    // Presentation detection needs six consecutive fast samples and twelve
    // slow ones to leave; a single slow callback does not flap.
    {
        PresentationCadence presentation;
        for(int i=0;i<5;++i)assert(!presentation.advance(1./120.));
        assert(presentation.advance(1./120.));
        assert(presentation.high_refresh);
        for(int i=0;i<11;++i)assert(presentation.advance(1./60.));
        assert(!presentation.advance(1./60.));
        assert(!presentation.high_refresh);
        assert(PresentationCadence::fast_sample(1./120.));
        assert(!PresentationCadence::fast_sample(1./60.));
        assert(!PresentationCadence::fast_sample(0));
    }
    // The gate only presents interpolated frames after a real tick.
    {
        PresentationGate gate;
        assert(!gate.advance(true,false));
        assert(gate.advance(true,true));
        assert(gate.advance(true,false));
        assert(!gate.advance(false,false));
    }
    std::puts("TH11 display cadence CPU contract PASS");
    return 0;
}
