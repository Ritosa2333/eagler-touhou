#ifdef TH11_LAYOUT_PROOF
#include "ThcrapLayout.hpp"
#else
#include "../th10_web/cpp/sdl/ThcrapLayout.hpp"
#endif
#include <cassert>
#include <iostream>
#ifdef TH11_LAYOUT_PROOF
using namespace th11::sdl;
#else
using namespace th10::sdl;
#endif
int main(){
    // Deterministic font metric substitute; production uses TTF_GetStringSize.
    auto measure=[](const std::string& text,const std::string&){
        int width=0;for(size_t i=0;i<text.size();){const auto c=(unsigned char)text[i];
            size_t n=c<128?1:(c&0xe0)==0xc0?2:(c&0xf0)==0xe0?3:4;
            width+=c<128?8:16;i+=n;
        }return width;
    };
    std::vector<int> tabs;
    auto line=thcrap_layout("<ts$灵梦  >　　早晨的博丽神社。",tabs,1024,measure);
    assert(tabs.size()==1&&tabs[0]==48);
    assert(line.runs.size()==1&&line.runs[0].x==0); // ts defines a tab without advancing narration.
    line=thcrap_layout("<r$灵梦  >「啊—事到如今懊悔也无济于事。",tabs,1024,measure);
    assert(line.runs.size()==2&&line.runs[0].x==0&&line.runs[1].x==48);
    line=thcrap_layout("<l$> 这样下去的话，我的神社会被别人抢去的。",tabs,1024,measure);
    assert(line.runs.size()==1&&line.runs[0].x==48);
    // Longer/different speaker aligns within the established name column.
    line=thcrap_layout("<r$魔理沙  >「这是什么？」",tabs,1024,measure);
    assert(line.runs[0].x==-16&&line.runs[1].x==48);
    tabs.clear();
    line=thcrap_layout("<ts$Reimu $Marisa >The Hakurei Shrine.",tabs,1024,measure);
    assert(tabs[0]==56&&line.runs[0].x==0);
    line=thcrap_layout("<r$Reimu >I can't let things go on like this.",tabs,1024,measure);
    assert(line.runs[0].x==8&&line.runs[1].x==56);
    line=thcrap_layout("<l$> The continuation.",tabs,1024,measure);
    assert(line.runs[0].x==56);
    line=thcrap_layout("<c$X$ABCD>",tabs,1024,measure);
    assert(line.runs[0].x==12&&line.width==32);
    line=thcrap_layout("<r$X$>",tabs,1024,measure);
    assert(line.runs[0].x==1016);
    line=thcrap_layout("<biu$text>",tabs,1024,measure);
    assert(line.runs[0].commands=="biu");
    line=thcrap_layout("<text>",tabs,1024,measure);
    assert(line.runs[0].text=="<text>");
    tabs.clear();
    line=thcrap_layout("原版无标记文本",tabs,1024,measure);
    assert(line.runs.size()==1&&line.runs[0].x==0&&line.runs[0].text=="原版无标记文本");
    std::cout<<"TH10 ending layout: Chinese/English cross-line tabs, speaker-only alignment, hidden definitions, reference widths and literal fallback PASS\n";
}
