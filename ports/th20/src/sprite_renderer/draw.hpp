#pragma once
#include "quad.hpp"
namespace th20::source::sprite {
void draw_animation(Controller&,Animation&); //443880 full original render-mode dispatch
namespace draw_environment {void enable_fog();void disable_fog();void enable_depth_write();void disable_depth_write();}
// These are actual, separately identified rendering routines. Declarations keep
// unported geometry visible to the linker; no default/fake primitive is used.
namespace primitive {
void p440310(Controller&,Animation&);
void p4413a0(Controller&,Animation&);
void p4408b0(Controller&,Animation&);
void p441c00(Controller&,Animation&);
void p441f00(Controller&,Animation&);
std::int32_t p445350(Controller&,Animation&,float*,std::uint32_t);
std::int32_t p445130(Controller&,Animation&,float*,std::uint32_t);
void p443020(Controller&,Animation&,void*,std::uint32_t);
void p439a00(Controller&,float,float,float,float,float,std::uint32_t,std::uint32_t,std::int32_t,std::int32_t);
std::int32_t p43a1c0(Controller&,float,float,float,float,float,std::uint32_t,std::uint32_t,std::int32_t,std::int32_t);
std::int32_t p43b960(Controller&,float,float,float,float,std::uint32_t,std::uint32_t,std::uint32_t);
std::int32_t p43bc60(Controller&,float,float,float,float,std::uint32_t,std::uint32_t);
std::int32_t p43be80(Controller&,float,float,float,float,float,std::uint32_t,std::uint32_t,std::uint32_t);
void p43cd40(Controller&,float,float,float,float,std::uint32_t,std::uint32_t,std::int32_t,std::int32_t);
void p43b0e0(Controller&,float,float,float,float,float,std::uint32_t,std::uint32_t,std::int32_t,std::int32_t);
void p43e2c0(Controller&,float,float,float,float,float,std::uint32_t,std::uint32_t,std::int32_t,std::int32_t);
void p43c180(Controller&,float,float,float,float,float,std::uint32_t,std::uint32_t,std::uint32_t);
std::int32_t p43d830(Controller&,float,float,std::uint32_t);
void p43c4e0(Controller&,float,float,float,float,float,std::uint32_t,std::uint32_t,std::uint32_t);
void p43c780(Controller&,float,float,float,float,float,float,std::uint32_t,std::uint32_t,std::uint32_t);
std::int32_t p43a2b0(Controller&,float,float,float,float,float,float,std::uint32_t,std::uint32_t,std::uint32_t);
std::int32_t p43a750(Controller&,float,float,float,float,float,float,std::uint32_t,std::uint32_t);
std::int32_t p43ab20(Controller&,float,float,float,float,float,float,float,std::uint32_t,std::uint32_t,std::uint32_t);
std::int32_t p43d9b0(Controller&,float,float,float,float,float,std::uint32_t,std::uint32_t,std::uint32_t);
std::int32_t p43dce0(Controller&,float,float,float,float,float,std::uint32_t,std::uint32_t);
std::int32_t p43df30(Controller&,float,float,float,float,float,float,std::uint32_t,std::uint32_t,std::uint32_t);
}
}
