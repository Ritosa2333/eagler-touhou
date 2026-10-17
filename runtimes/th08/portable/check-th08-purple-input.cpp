#include <eagler/thprac/PracticeInput.hpp>
#include "../th08_web/cpp/game/Types.hpp"
#include <cassert>
using namespace th08;
int main(){
    eagler::thprac::PracticeInput input;u8 keys[256]{};
    keys[90]=keys[88]=keys[160]=keys[161]=128;
    input.apply(keys);assert(keys[90]&&keys[88]&&keys[160]&&keys[161]);
    input.disable_xkey=input.disable_zkey=input.disable_shiftkey=true;
    keys[67]=128;input.apply(keys);
    assert(!keys[90]&&!keys[88]&&!keys[67]&&!keys[160]&&!keys[161]);
    input.force_shiftkey=true;input.apply(keys);assert(keys[160]&&keys[161]);
    input.begin_retry(1);assert(input.fast_retry_count_down==0);
    input.enable_fast_retry=true;input.begin_retry(0);assert(input.fast_retry_count_down==0);
    input.begin_retry(1);assert(input.fast_retry_count_down==15);
    for(int n=15;n>0;--n){keys[27]=keys[82]=0;input.apply(keys);assert(keys[27]);assert(bool(keys[82])==(n==1));input.gui_tick();}
    keys[27]=keys[82]=0;input.apply(keys);assert(!keys[27]&&!keys[82]);
    input.begin_retry(1);input.reset();assert(!input.fast_retry_count_down&&input.enable_fast_retry);
}
