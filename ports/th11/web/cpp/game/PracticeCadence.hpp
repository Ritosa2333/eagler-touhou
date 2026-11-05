#pragma once
#include <algorithm>
#include <cmath>
namespace th11 {
// Title-owned catch-up policy: intentionally not shared.
struct PracticeCadence {
 double debt=0,period=1./60.;
 void reset(){debt=0;}
 unsigned advance(double seconds){debt=std::min(.1,debt+std::clamp(seconds,0.,.1));const auto ticks=std::min(std::abs(period-1./60.)<1e-12?4u:1024u,unsigned(std::floor((debt+1e-9)/period)));debt=std::max(0.,debt-ticks*period);return ticks;}
};
}
