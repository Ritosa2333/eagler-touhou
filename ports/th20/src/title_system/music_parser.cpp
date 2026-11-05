#include "music.hpp"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <stdexcept>
namespace th20::source::title {
namespace {
void copy_string(std::uint8_t* target,std::size_t capacity,const std::string& value){if(value.size()>=capacity)throw std::out_of_range("Music comment exceeds original fixed string buffer");std::memcpy(target,value.c_str(),value.size()+1);}
}
void parse_music_comments(TitleInf& o,std::string_view bytes){
    auto nul=bytes.find('\0');if(nul!=std::string_view::npos)bytes=bytes.substr(0,nul);
    std::string text(bytes);text.erase(std::remove(text.begin(),text.end(),'\r'),text.end());text.erase(std::remove(text.begin(),text.end(),'\f'),text.end());
    std::istringstream stream(text);std::string line;int count=0;
    while(std::getline(stream,line)){
        if(line.empty()||line[0]=='#')continue;if(line[0]=='\\')break;if(line[0]!='@')continue;
        if(count>=32)throw std::out_of_range("Music comments exceed32 records");
        copy_string(o.data490+count*0x40,0x40,line.substr(1));
        std::getline(stream,line);copy_string(o.datac90+count*0x42,0x42,line);
        for(int row=0;row<8;++row){std::getline(stream,line);copy_string(o.data14d0+count*0x210+row*0x42,0x42,line);}++count;
    }
    o.cursor.count=count;o.cursor.select(0);o.word56d0=0;o.words47c[0]=count;o.flag478.store(true,std::memory_order_seq_cst);
}
}
