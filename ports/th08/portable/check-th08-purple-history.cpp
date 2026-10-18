#include "../th08_web/cpp/game/PracticeHistoryDigits.hpp"
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
int main(){
    const auto add=[](float a,float b){return a+b;};
    for(bool captures:{false,true})for(int n:{1000,12345,2147483647}){
        float x=0;std::string digits;std::vector<float> positions;
        assert(th08::practice_history_overflow(n,captures,x,[&](int d){digits+=char('0'+d);positions.push_back(x);},add));
        assert(digits==std::to_string(n));
        const float start=captures?28.f:7.f,step=25.f/digits.size();
        for(unsigned i=0;i<positions.size();++i)assert(std::fabs(positions[i]-(start+i*step))<.0001f);
        assert(std::fabs(x-(start+step*(digits.size()-(captures?1:0))))<.0001f);
    }
    for(int n:{0,1,99,999}){
        float x=12;bool drew=false;
        assert(!th08::practice_history_overflow(n,true,x,[&](int){drew=true;},add));
        assert(x==12&&!drew);
    }
}
