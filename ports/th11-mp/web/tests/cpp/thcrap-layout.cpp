#include "../../cpp/sdl/ThcrapLayout.hpp"
#include <cassert>
#include <iostream>
using namespace th11::sdl;
int main(){
    std::vector<int> tabs;
    auto width=[](const std::string& s,const std::string&){return int(s.size())*10;};
    auto line=thcrap_layout("<t$Suika >(Oh, hey, you can hear me.",tabs,1024,width);
    assert(tabs.size()==1&&tabs[0]==60);
    assert(line.runs.size()==2&&line.runs[0].text=="Suika "&&line.runs[1].x==60);
    line=thcrap_layout("<l$> We can talk using your yin-yang orbs.)",tabs,1024,width);
    assert(line.runs.size()==1&&line.runs[0].x==60);
    assert(line.runs[0].text==" We can talk using your yin-yang orbs.)");
    line=thcrap_layout("<t$\u8403\u9999 >\u4f60\u80fd\u542c\u5230\u6211\u5417",tabs,1024,width);
    assert(line.runs[0].text=="\u8403\u9999 "&&tabs[0]==70);
    line=thcrap_layout("<l$>\u4f7f\u7528\u9634\u9633\u7389\u5bf9\u8bdd",tabs,1024,width);
    assert(line.runs.size()==1&&line.runs[0].x==70);
    tabs.clear();
    line=thcrap_layout("<t$A$LONG>!",tabs,1024,width);
    assert(tabs[0]==40&&line.runs[1].x==40);
    line=thcrap_layout("<r$X$ABCD>",tabs,1024,width);
    assert(line.runs[0].x==30&&line.width==40);
    line=thcrap_layout("<c$X$ABCD>",tabs,1024,width);
    assert(line.runs[0].x==15&&line.width==40);
    line=thcrap_layout("<r$X$>",tabs,1024,width);
    assert(line.runs[0].x==1014&&line.width==1024);
    line=thcrap_layout("<s$hidden>text",tabs,1024,width);
    assert(line.runs.size()==1&&line.runs[0].text=="text"&&line.runs[0].x==0);
    line=thcrap_layout("<biu$text>",tabs,1024,width);
    assert(line.runs[0].commands=="biu"&&line.width==40);
    line=thcrap_layout("<text>",tabs,1024,width);
    assert(line.runs[0].text=="<text>");
    line=thcrap_layout("<l$a<b$c>>",tabs,1024,width);
    assert(line.runs[0].text=="a<b$c>");
    std::cout<<"THCRAP layout: cross-line English/Chinese tabs, width references, alignment, suppression, font modifiers, nesting passed\n";
}
