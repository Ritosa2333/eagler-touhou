#include "ecl_test_fixtures.hpp"
#include "game/EclContext.hpp"
using namespace th13;
TEST(float_to_integer_stack_reads_and_store_truncate_toward_zero) {
    for(float value:{14.99f,14.5f,14.01f,-14.99f,-14.5f,-.75f,.75f}) {
        const i32 expected=value>=1?14:value<=-1?-14:0;
        EclContext context;
        std::vector<u8> code;
        th13test::emit(code,0,44,0,255,0,{float_to_bits(value)});
        th13test::emit(code,0,43,1,255,0,{0});
        th13test::emit(code,100,0,0,255,0,{});
        CHECK(context.bind_code(code.data(),u32(code.size()),0));context.stack_pointer=32;
        CHECK_EQ(context.step(1),0);
        i32 stored=0;std::memcpy(&stored,context.stack.data(),4);CHECK_EQ(stored,expected);
        CHECK(context.push(value));i32 popped=99;CHECK(context.pop(popped));CHECK_EQ(popped,expected);
        CHECK(context.push(value));
        std::vector<u8> ref;th13test::emit(ref,0,23,1,255,0,{u32(-1)});
        context.instruction=reinterpret_cast<const EclInstruction*>(ref.data());
        CHECK(context.integer(0,-1,popped));CHECK_EQ(popped,expected);
    }
}
int main(){return th13test::run_all("ecl-float-integer");}
