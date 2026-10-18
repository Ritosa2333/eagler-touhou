#include "text.hpp"
#include <algorithm>
#include <cstring>
namespace th20::source::text {
void construct_line(Line& line) noexcept {std::memset(&line,0,sizeof(line));line.align_x=1;line.align_y=1;}
Line::Line(){construct_line(*this);}
void Renderer::compact_lines() {
    const auto count=std::min(line_count,320);int destination=0;
    for(int i=0;i<count;++i){auto& line=lines[i];const auto bits=static_cast<std::uint32_t>(line.frames)-1;std::memcpy(&line.frames,&bits,4);
        if(line.frames>=0){if(destination!=i)lines[destination]=line;++destination;}}
    line_count=destination;
}
}
