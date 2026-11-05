#include "stone.hpp"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
namespace th20::source::stone_menu {
namespace {
constexpr std::string_view labels[]{
#include "names.inc"
};
static_assert(std::size(labels)==88);
void copy_line(char (&destination)[256],const std::string& value){
    // strcpy_s clears destination on ERANGE; valid stock resource lines fit.
    // Keep the exact original byte encoding and bytes beyond the terminator.
    strcpy_s(destination,sizeof(destination),value.c_str());
}
}
void parse_text(StoneMenuInf& owner,std::string_view input){
    const auto nul=input.find('\0');if(nul!=std::string_view::npos)input=input.substr(0,nul);
    std::string content(input);content.erase(std::remove(content.begin(),content.end(),'\r'),content.end());content.erase(std::remove(content.begin(),content.end(),'\f'),content.end());
    std::istringstream stream(content);std::string line;
    while(std::getline(stream,line)){
        if(!line.empty()&&line.front()=='\\')return;
        if(line.empty()||line.front()!='@')continue;
        for(;;){
            const auto label=std::string_view(line).substr(1);const auto found=std::find(std::begin(labels),std::end(labels),label);
            // An unknown label indexed past the original buffers. Do not turn
            // corrupt resource input into a fabricated menu entry.
            if(found==std::end(labels))throw std::out_of_range("Unknown original StoneMenu text label");
            const auto index=static_cast<unsigned>(found-std::begin(labels));
            if(index<72&&index%4==0){std::getline(stream,line);for(unsigned k=0;k<4;++k)copy_line(owner.names[index+k],line);}
            if(index>=72){std::getline(stream,line);copy_line(owner.names[index],line);}
            for(auto& description:owner.descriptions[index])description[0]=0;
            unsigned description=0;bool next_header=false;
            while(std::getline(stream,line)){
                if(line.empty()||line.front()=='#')continue;
                if(line.front()=='\\')return;
                if(line.front()=='@'){next_header=true;break;}
                copy_line(owner.descriptions[index][description++],line);
                if(description>4)break;
            }
            if(!next_header)break;
        }
    }
}
}
