// 恢复657960/6579b0/657a70/657a90/657ab0/657ae0/657b80，保留原两角判断及严格半径边界。
#pragma once
#include "core/math/GameMath.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/math/FiniteSqrt.hpp"
#include <array>
namespace kaikeidou {
namespace polygon_detail {
using Point = std::array<std::uint64_t, 2>;
struct Arithmetic {
    unsigned precision;
    using Number = binary_float::Number;
    Number q(std::uint64_t v) const { return binary_float::binary64(v); }
    Number add(Number a, Number b) const { return binary_float::add(a,b,precision); }
    Number sub(Number a, Number b) const { b.negative=!b.negative;return add(a,b); }
    Number mul(Number a, Number b) const { return binary_float::multiply(a,b,precision); }
    Number div(Number a, Number b) const { return binary_float::divide(a,b,precision); }
    std::uint64_t store(Number v) const { return binary_float::to_binary64(v); }
};
inline bool point_inside_circle(const Point& point,const Point& center,std::uint64_t radius,unsigned precision) {
    const Arithmetic m{precision};
    const auto dx=m.q(m.store(m.sub(m.q(point[0]),m.q(center[0]))));
    const auto dy=m.q(m.store(m.sub(m.q(point[1]),m.q(center[1]))));
    const auto distance=m.add(m.mul(dx,dx),m.mul(dy,dy));const auto r=m.mul(m.q(radius),m.q(radius));
    if (!r.mantissa) return false;
    return !distance.mantissa || distance.exponent<r.exponent || (distance.exponent==r.exponent && distance.mantissa<r.mantissa);
}
inline std::uint64_t point_segment_distance(const Point& point,const Point& a,const Point& b,unsigned precision) {
    const Arithmetic m{precision};
    const auto dx=m.q(m.store(m.sub(m.q(b[0]),m.q(a[0]))));
    const auto dy=m.q(m.store(m.sub(m.q(b[1]),m.q(a[1]))));
    const auto denominator=m.q(m.store(m.add(m.mul(dx,dx),m.mul(dy,dy))));
    auto dot=m.q(m.store(m.add(m.mul(m.sub(m.q(a[0]),m.q(point[0])),dx),m.mul(m.sub(m.q(a[1]),m.q(point[1])),dy))));
    dot.negative=!dot.negative;
    if (!denominator.mantissa && !dot.mantissa) return 0xfff8000000000000ull;
    auto t=denominator.mantissa ? m.store(m.div(dot,denominator)) : (dot.negative ? 0ull : 0x3ff0000000000000ull);
    if (crt_sse2::real(t)<0.) t=0;
    else if (crt_sse2::real(t)>1.) t=0x3ff0000000000000ull;
    const auto x=m.store(m.add(m.mul(dx,m.q(t)),m.q(a[0]))),y=m.store(m.add(m.mul(dy,m.q(t)),m.q(a[1])));
    const auto ex=m.sub(m.q(point[0]),m.q(x)),ey=m.sub(m.q(point[1]),m.q(y));
    const auto square=m.store(m.add(m.mul(ex,ex),m.mul(ey,ey)));
    return m.store(finite_square_root(square,precision));
}
inline std::uint64_t three_point_angle(const Point& a,const Point& b,const Point& point,unsigned precision,GameMath& math) {
    const Arithmetic m{precision};Point u{},v{};
    for (unsigned i=0;i<2;++i) u[i]=m.store(m.sub(m.q(b[i]),m.q(a[i])));
    for (unsigned i=0;i<2;++i) v[i]=m.store(m.sub(m.q(point[i]),m.q(a[i])));
    const auto cross=m.store(m.sub(m.mul(m.q(u[0]),m.q(v[1])),m.mul(m.q(u[1]),m.q(v[0]))));
    const auto dot=m.store(m.add(m.mul(m.q(u[0]),m.q(v[0])),m.mul(m.q(u[1]),m.q(v[1]))));
    return m.store(math.atan2(cross,dot));
}
}
inline bool polygon_circle_overlap(const std::array<std::array<std::uint64_t,2>,4>& vertices,
    std::uint64_t x,std::uint64_t y,std::uint64_t radius,unsigned precision,GameMath& math) {
    const polygon_detail::Point point{{x,y}};
    for (const auto& vertex:vertices) if (polygon_detail::point_inside_circle(vertex,point,radius,precision)) return true;
    const auto a=crt_sse2::real(polygon_detail::three_point_angle(vertices[0],vertices[1],point,precision,math));
    const auto b=crt_sse2::real(polygon_detail::three_point_angle(vertices[2],vertices[3],point,precision,math));
    const auto half_pi=crt_sse2::real(0x3ff921fb54442d28ull);
    if (a>=0. && a<=half_pi && b>=0. && b<=half_pi) return true;
    for (unsigned i=0;i<4;++i)
        if (crt_sse2::real(polygon_detail::point_segment_distance(point,vertices[i],vertices[(i+1)%4],precision))<crt_sse2::real(radius)) return true;
    return false;
}
}
