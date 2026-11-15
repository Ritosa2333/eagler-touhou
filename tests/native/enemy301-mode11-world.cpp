#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace ecl_fixture;
static void near(float a,float b){assert(std::abs(a-b)<1e-5);}
static void run(std::int32_t duration,float x,float y,float expectedX,float expectedY,float firstX){
    th128::StageWorld w;std::string reason;
    const auto data=program({{"main",join({instruction(301,{std::uint32_t(duration),11,floatBits(x),floatBits(y)}),instruction(0,{},10000)})}});
    assert(w.load("mode11.ecl",data.data(),data.size(),reason));assert(w.start("mode11.ecl",1,0));assert(w.enableLocalEnemyPasses(reason));
    assert(w.tick());assert(w.entities()[0].x==0&&w.entities()[0].y==0);
    assert(w.tick());near(w.entities()[0].x,firstX);near(w.entities()[0].y,0);
    for(int tick=3;tick<=duration+1;++tick){
        assert(w.tick());
        if(duration==240&&tick==60){near(w.entities()[0].x,-4.39f);near(w.entities()[0].y,.93f);}
        if(duration==240&&tick==120){near(w.entities()[0].x,-72.54f);near(w.entities()[0].y,15.47f);}
        if(duration==240&&tick==239){near(w.entities()[0].x,-150.f);near(w.entities()[0].y,31.99f);}
    }
    near(w.entities()[0].x,expectedX);near(w.entities()[0].y,expectedY);
}
int main(){
    run(240,-150,32,-150,32,-0.01f);
    run(60,-130,160,-130,160,-0.01f);
    run(60,150,32,150,32,0.f);
    std::printf("PASS mode11 StageWorld 240/60/mirrored\n");
}
