#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include <cstdlib>
using namespace th13;
namespace {
struct Variables final : EclVariables {
    std::vector<std::pair<char,int>> calls;
    std::vector<float> values;
    GameRandom* random=nullptr;
    std::vector<u32> random_indices;
    bool integer(i32 id,i32& out)override {
        if(random)random_indices.push_back(random->index());
        int index=-10000-id;calls.push_back({'i',index});out=i32(values[index]);return true;
    }
    bool floating(i32 id,float& out)override {
        if(random)random_indices.push_back(random->index());
        int index=-10000-id;calls.push_back({'f',index});out=values[index];return true;
    }
    i32* integer_destination(i32)override{return nullptr;}
    float* float_destination(i32)override{return nullptr;}
};
struct Case {u16 op;std::vector<float> values;u32 integers;std::vector<int> positive,zero;};
// Signed indices encode type: positive=index+1 float; negative=-(index+1) int.
// Expected sequences captured from original478ee0/478e60, not port order.
TEST(movement_operands_follow_original_read_order_and_zero_duration_gates) {
    const std::vector<Case> cases={
        {400,{1,2},0,{1,2},{}},{402,{1,2},0,{1,2},{}},
        {401,{30,0,1,2},3,{3,4,-1,-1,-2},{3,4,-1}},
        {403,{30,0,1,2},3,{3,4,-1,-1,-2},{3,4,-1}},
        {405,{30,0,1,2},3,{3,4,-1,-2,-1},{3,4,-1}},
        {407,{30,0,1,2},3,{3,4,-1,-2,-1},{3,4,-1}},
        {429,{30,0,1,2},3,{3,4,-1,-2,-1},{3,4,-1}},
        {431,{30,0,1,2},3,{3,4,-1,-2,-1},{3,4,-1}},
        {408,{1,2,3,4},0,{1,2,3,4},{}},{410,{1,2,3,4},0,{1,2,3,4},{}},
        {409,{30,0,1,2,3},3,{3,4,5,-1,-2},{3,4,5,-1,-2}},
        {411,{30,0,1,2,3},3,{3,4,5,-1,-2},{3,4,5,-1,-2}},
        {420,{1,2,3,4,5,6},0,{1,2,3,4,5,6},{}},{422,{1,2,3,4,5,6},0,{1,2,3,4,5,6},{}},
        {425,{30,1,2,3,4,5,6},1,{4,5,2,3,6,7,-1},{4,5,2,3,6,7,-1}},
        {412,{30,0,1},3,{-2,3,-1},{-2,3,-1}},
        {432,{0},1,{-1},{}},{433,{0},1,{-1},{}},
        {434,{30,0,0,1,2},7,{4,5,-1,-1,-2,-3},{4,5,-1}},
        {435,{30,0,0,1,2},7,{4,5,-1,-1,-2,-3},{4,5,-1}},
        {436,{30,0,1,2},3,{3,4,-1,-1,-2},{3,4,-1}},
        {437,{30,0,1,2},3,{3,4,-1,-1,-2},{3,4,-1}},
        {438,{30,0,0,1,2},7,{4,5,-1,-1,-2,-3},{4,5,-1}},
        {439,{30,0,0,1,2},7,{4,5,-1,-1,-2,-3},{4,5,-1}}
    };
    for(const auto& sample:cases)for(int zero=0;zero<(!sample.zero.empty()?2:1);++zero){
        StageSim sim;StageEnemy enemy;EclContext context;Variables vars;
        sim.commands.sim=&sim;sim.current=&enemy;context.variables=&vars;vars.values=sample.values;
        vars.random=&sim.random;
        if(zero)vars.values[0]=0;
        std::vector<u32> args;
        for(size_t i=0;i<vars.values.size();++i)
            args.push_back(sample.integers&(1u<<i)?u32(-10000-i32(i)):float_to_bits(float(-10000-i32(i))));
        std::vector<u8> code;
        th13test::emit(code,0,sample.op,u16((1u<<args.size())-1),255,0,args);
        context.instruction=reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim.commands.execute(context,sample.op),0);
        const auto& expected=zero?sample.zero:sample.positive;
        CHECK_EQ(vars.calls.size(),expected.size());
        if(sample.op==412)for(auto index:vars.random_indices)CHECK_EQ(index,3u);
        for(size_t i=0;i<vars.calls.size()&&i<expected.size();++i){
            CHECK_EQ(vars.calls[i].first,expected[i]>0?'f':'i');
            CHECK_EQ(vars.calls[i].second,std::abs(expected[i])-1);
        }
        if(std::getenv("TH13_OPERAND_ORDER_TRACE")){
            std::printf("ORDER %u %d",sample.op,zero);
            for(auto call:vars.calls)std::printf(" %c%d",call.first,call.second);
            std::puts("");
        }
    }
}
}
int main(){return th13test::run_all("movement-operand-order");}
