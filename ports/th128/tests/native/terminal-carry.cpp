#include "game/StageSessionIdentity.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace {
th128::stage::StageResourceRecord record(int id,const std::string& ecl) {
    return {id,"stage.std",ecl,"stage.msg","stage.anm",{"a.wav","b.wav"}};
}
}

int main() {
    using namespace th128::stage;
    const auto source2=record(2,"st_a1_2.ecl");
    const auto target3=record(3,"st_a1_3.ecl");
    const auto source4=record(4,"st_a2_2.ecl");
    const auto target5=record(5,"st_a2_3.ecl");
    const auto source1=record(1,"st_a1_1.ecl");
    const auto extra16=record(16,"st_ex.ecl");
    const SessionIdentity s2{2,"st_a1_2.ecl",1,0};
    const SessionIdentity t3{3,"st_a1_3.ecl",1,0};
    const SessionIdentity s4{4,"st_a2_2.ecl",1,1};
    const SessionIdentity t5{5,"st_a2_3.ecl",1,1};
    const SessionIdentity s3{3,"st_a1_3.ecl",1,0};
    const SessionIdentity t1{1,"st_a1_1.ecl",1,0};
    const SessionIdentity t16{16,"st_ex.ecl",1,0};
    std::string error;
    assert(canAcceptCarry(Role::Terminal));
    assert(!canRequestOrdinaryHandoff(Role::Terminal));
    assert(validateCarryTransition(s2,source2,t3,target3,0,0,error));
    assert(validateCarryTransition(s4,source4,t5,target5,0,0,error));
    const SessionIdentity wrongEcl{3,"st_a1_1.ecl",1,0};
    assert(!validateCarryTransition(s2,source2,wrongEcl,target3,0,0,error));
    const SessionIdentity wrongProgress{3,"st_a1_3.ecl",1,1};
    assert(!validateCarryTransition(s2,source2,wrongProgress,target3,0,0,error));
    const SessionIdentity wrongDifficulty{3,"st_a1_3.ecl",2,0};
    assert(!validateCarryTransition(s2,source2,wrongDifficulty,target3,0,0,error));
    assert(!validateCarryTransition(s3,target3,t1,source1,0,0,error));
    assert(!validateCarryTransition(s2,source2,t16,extra16,0,0,error));
    std::cout << "terminal carry identity checks passed\n";
    return 0;
}
