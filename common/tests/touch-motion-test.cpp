#include <eagler/input/TouchMotionTrace.hpp>
#include <eagler/input/LimitedTouchVector.hpp>
#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>

using eagler::input::TouchMotionTraceBank;

int main() {
    TouchMotionTraceBank bank;
    assert(!bank.context().frame_known);
    bank.set_context(31,TouchMotionTraceBank::resimulation,2.f);
    assert(bank.context().frame_known&&bank.context().frame==31);
    assert(bank.context().execution_kind==2&&bank.context().sensitivity==2.f);
    const auto* empty=bank.snapshot(2,1);
    assert(empty[0]==2&&empty[1]==8&&empty[2]==16&&empty[3]==512);
    assert(empty[4]==0&&empty[5]==0&&empty[6]==2&&empty[7]==1);
    assert(bank.snapshot(4,0)==nullptr&&bank.snapshot(0,4)==nullptr);
    TouchMotionTraceBank::Sample first{};
    first.frame=41;first.mode=1;first.flags=1;first.seat=2;first.execution_kind=1;
    first.before_x=10;first.before_y=20;first.target_x=16;first.target_y=28;
    first.after_x=12.4f;first.after_y=23.2f;first.speed=4;first.sensitivity=3;
    assert(bank.append(first));
    auto second=first;second.seat=1;second.execution_kind=0;second.frame=75;
    assert(bank.append(second));
    const auto* read=bank.snapshot(2,1);
    const std::uint32_t expected[]{1,41,1,1,0x41200000u,0x41a00000u,
        0x41800000u,0x41e00000u,0x41466666u,0x41b9999au,
        0x40800000u,0x3f800000u,0x3f800000u,0x40400000u,2,1};
    assert(read[4]==1&&read[5]==1);
    assert(std::memcmp(read+8,expected,sizeof(expected))==0);
    std::uint32_t copy[24];std::memcpy(copy,read,sizeof(copy));
    bank.snapshot(1,0);
    assert(std::memcmp(copy,bank.snapshot(2,1),sizeof(copy))==0);
    for(unsigned frame=42;frame<=554;++frame){first.frame=frame;assert(bank.append(first));}
    read=bank.snapshot(2,1);
    assert(read[4]==514&&read[5]==512);
    assert(read[8]==3&&read[9]==43);
    assert(read[8+511*16]==514&&read[9+511*16]==554);
    const auto* other=bank.snapshot(1,0);
    assert(other[4]==1&&other[8]==1&&other[9]==75);
    assert(bank.snapshot(2,0)[4]==0);
    first.seat=4;assert(!bank.append(first));
    assert(bank.snapshot(2,1)[4]==514);

    eagler::input::TouchMotionTrace legacy;
    eagler::input::TouchMotionTrace::Sample old{};old.frame=9;old.flags=1;
    legacy.append(old);const auto* v1=legacy.snapshot();
    assert(v1[0]==1&&v1[4]==1&&v1[6]==0&&v1[7]==0);
    assert(v1[8+14]==0&&v1[8+15]==0);

    using eagler::input::nearest_integer_touch_vector_within_speed;
    std::int32_t x=0,y=0;
    float dx=293,dy=273;
    const auto factor=400.f/std::hypot(dx,dy);dx*=factor;dy*=factor;
    assert(nearest_integer_touch_vector_within_speed(dx,dy,400,x,y));
    assert(x==292&&y==273&&std::hypot(x,y)<=400);
    assert(nearest_integer_touch_vector_within_speed(30,40,400,x,y)&&x==30&&y==40);
    assert(nearest_integer_touch_vector_within_speed(-30,-40,400,x,y)&&x==-30&&y==-40);
    assert(nearest_integer_touch_vector_within_speed(0,0,0,x,y)&&x==0&&y==0);
    assert(!nearest_integer_touch_vector_within_speed(std::numeric_limits<float>::infinity(),0,400,x,y));
    assert(!nearest_integer_touch_vector_within_speed(std::numeric_limits<float>::max(),0,400,x,y));
    assert(!nearest_integer_touch_vector_within_speed(0,0,-1,x,y));
}
