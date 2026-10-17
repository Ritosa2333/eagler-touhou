#include "../th08_web/cpp/multiplayer/NetworkFailure.hpp"
#include <cassert>
#include <cstring>
#include <string>
using namespace th08::multiplayer;
int main(){
    char text[768]{};
    std::string reason="RTC peer P2 input channel closed";
    FormatNetworkFailure(text,"network channel failed",{reason.c_str(),"rtc",2,104,96,97,103,4,900,891,137672,137000,672});
    assert(std::strstr(text,"network channel failed: RTC peer P2 input channel closed"));
    assert(std::strstr(text,"mode=rtc generation=2 next=104 confirmed=96 rollback=97 captured=103"));
    assert(std::strstr(text,"channel=4 sent=900 received=891 queuedBytes=137672"));
    assert(std::strstr(text,"inputBytes=137000 controlBytes=672"));
    reason.clear(); // diagnostic survives transport-owned storage changing
    assert(std::strstr(text,"RTC peer P2 input channel closed"));
    FormatNetworkFailure(text,"captured input send failed",{});
    assert(std::strstr(text,"captured input send failed: no channel detail available"));
    assert(std::strstr(text,"mode=pending"));
    assert(std::strstr(text,"confirmed=-1 rollback=-1 captured=-1"));
    FormatNetworkFailure(text,"network channel failed",{"Unsupported or out-of-window TH08 input","relay"});
    assert(std::strstr(text,"Unsupported or out-of-window TH08 input [mode=relay"));
    reason.assign(2000,'x');
    FormatNetworkFailure(text,"network channel failed",{reason.c_str(),"rtc"});
    assert(std::strlen(text)<sizeof(text));
    assert(std::strstr(text,"queuedBytes=0 inputBytes=0 controlBytes=0]")); // retain counters with long peer errors
    struct {char before='a';char text[768];char after='z';} bounded;
    FormatNetworkFailure(bounded.text,reason.c_str(),{reason.c_str(),reason.c_str(),0xffffffffu,0xffffffffu,
        0xfffffffeu,0xfffffffeu,0xfffffffeu,0xffffffffu,0xffffffffffffffffull,0xffffffffffffffffull,std::size_t(-1),std::size_t(-1),std::size_t(-1)});
    assert(bounded.before=='a'&&bounded.after=='z'&&std::strlen(bounded.text)<sizeof(bounded.text));
    assert(std::strstr(bounded.text,"controlBytes="));
    FormatNetworkFailure(bounded.text,nullptr,{nullptr,nullptr});
    assert(std::strstr(bounded.text,"network failure: no channel detail available"));
    std::puts("TH08 failure-time network diagnostics: PASS");
}
