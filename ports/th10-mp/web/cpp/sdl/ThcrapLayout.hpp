#pragma once
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace th10::sdl {
// Port of thcrap_tsa/src/layout.cpp: layout_tokenize/layout_parse_tabs/
// layout_process. Same port as TH11. Tabs survive TextOut calls, not x/tab.
struct LayoutRun { std::string text, commands; int x=0; };
struct LayoutLine { std::vector<LayoutRun> runs; int width=0; };
template<class Measure>
LayoutLine thcrap_layout(const std::string& text,std::vector<int>& tabs,
                        int bitmap_width,Measure measure){
    std::vector<std::vector<std::string>> tokens;
    for(size_t i=0;i<text.size();){
        size_t length=text.size()-i;
        std::vector<std::string> args;
        if(text[i]=='<'){
            int depth=0;size_t start=i+1,end=start;
            for(size_t p=start;p<text.size()&&depth>=0;++p){
                depth+=text[p]=='<';depth-=text[p]=='>';
                if((depth==0&&text[p]=='$')||(depth==-1&&text[p]=='>')){
                    args.push_back(text.substr(start,p-start));start=p+1;end=start;
                }
            }
            length=end-i;
        }
        if(args.size()>1)tokens.push_back(std::move(args));
        else {
            const auto next=text.find('<',i+1);
            if(next!=std::string::npos&&next<i+length)length=next-i;
            tokens.push_back({text.substr(i,length)});
        }
        i+=length;
    }
    LayoutLine result;size_t tab=0;int x=0;
    for(size_t n=0;n<tokens.size();++n){
        const auto& args=tokens[n];const bool markup=args.size()>1;
        const std::string commands=markup?args[0]:"",draw=markup?args[1]:args[0];
        int advance=measure(draw,commands);bool print=true;
        if(markup){
            int end=args.size()>2?(args[2].empty()?bitmap_width:x+measure(args[2],commands)):
                tab<tabs.size()?tabs[tab]:!tabs.empty()&&n+1==tokens.size()?bitmap_width:x+advance;
            bool tab_command=false;
            for(char command:commands){
                switch(command){
                case 's':end=x;print=false;tab_command=true;break;
                case 't':
                    end=advance;
                    for(size_t j=2;j<args.size();++j)end=std::max(end,measure(args[j],commands));
                    end+=x;if(tabs.size()<=tab)tabs.resize(tab+1);tabs[tab]=end;
                    tab_command=true;break;
                case 'l':tab_command=true;break;
                case 'c':x+=(end-x)/2-advance/2;tab_command=true;break;
                case 'r':x=end-advance;tab_command=true;break;
                default:break;
                }
            }
            if(tab_command){++tab;advance=end-x;}
        }
        if(print&&!draw.empty())result.runs.push_back({draw,commands,x});
        x+=advance;
    }
    result.width=x;return result;
}
}
