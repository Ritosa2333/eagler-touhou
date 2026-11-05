#include "text.hpp"
#include "../program_entry/program_entry.hpp"
#include <charconv>
#include <cstring>
#include <cstdarg>
#include <cstdio>
namespace th20::source::text {
namespace n=th20::recovered;
char* Renderer::next_text_buffer() noexcept{return line_count<320?lines[line_count].text:text_buffer;}
void Renderer::commit_shadow_line(const sprite::Vec3& position,std::uint32_t font) {
    if(line_count+1>=320)return;strcpy_s(lines[line_count+1].text,256,lines[line_count].text);
    const auto original_color=color,original_font=fields_1a1d4[3];color=shadow_color;fields_1a1d4[3]=font;
    commit_line(position);color=original_color;fields_1a1d4[3]=original_font;
}
void Renderer::commit_line(const sprite::Vec3& position) {
    if(line_count>=320)return;
    switch(fields_1a1d4[3]){case 6:commit_shadow_line(position,8);break;case 7:commit_shadow_line(position,9);break;case 10:commit_shadow_line(position,11);break;case 12:commit_shadow_line(position,13);break;}
    auto& line=lines[line_count++];const float scale=program_entry::window_state.scale;
    line.position={n::mul32(position.x,scale),n::mul32(position.y,scale),n::mul32(position.z,scale)};
    line.layer=static_cast<std::int32_t>(fields_1a1d4[6]);line.color=color;line.scale_x=scale_x;line.scale_y=scale_y;line.fields_120[1]=fields_1a1d4[0];
    line.font=static_cast<std::int32_t>(fields_1a1d4[3]);line.shadow=fields_1a1d4[2];line.frames=static_cast<std::int32_t>(fields_1a1d4[7]);line.blend=fields_1a1d4[10];
    line.align_x=fields_1a1d4[8];line.align_y=fields_1a1d4[9];line.rotation=rotation;
}
void Renderer::write_float(const sprite::Vec3& position,float value,std::string_view suffix,std::int32_t precision) {
    auto* buffer=next_text_buffer();const auto result=std::to_chars(buffer,buffer+255,value,std::chars_format::fixed,precision);
    std::memcpy(result.ptr,suffix.data(),suffix.size());result.ptr[suffix.size()]=0;commit_line(position);
}
void Renderer::write_ascii_format(const sprite::Vec3& position,const char* format,...) {
    std::lock_guard lock(runtime::shared_locks().slot(18));auto* buffer=next_text_buffer();va_list args;va_start(args,format);
    //The original statically linked CRT rounds exact decimal halves away from
    //zero (0.25 with %.1f is 0.3). Modern UCRT enables ties-to-even by default.
    __stdio_common_vsprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS&~_CRT_INTERNAL_PRINTF_STANDARD_ROUNDING,buffer,256,format,nullptr,args);
    va_end(args);commit_line(position);
}
void Renderer::write_character(const sprite::Vec3& position,char character) {auto* buffer=next_text_buffer();buffer[0]=character;buffer[1]=0;commit_line(position);}
void Renderer::write_integer(const sprite::Vec3& position,std::int32_t value) {std::lock_guard lock(runtime::shared_locks().slot(18));auto* buffer=next_text_buffer();*std::to_chars(buffer,buffer+255,value).ptr=0;commit_line(position);}
void Renderer::write_padded_integer(const sprite::Vec3& position,std::int32_t value,std::int32_t width,char padding) {
    auto* buffer=next_text_buffer();const auto end=std::to_chars(buffer,buffer+255,value).ptr;const auto digits=static_cast<int>(end-buffer),fill=width-digits;
    if(fill<1)*end=0;else{for(int i=digits-1;i>=0;--i)buffer[i+fill]=buffer[i];std::memset(buffer,static_cast<unsigned char>(padding),fill);buffer[width]=0;}commit_line(position);
}
void Renderer::write_integer_suffix(const sprite::Vec3& position,std::int32_t value,std::string_view suffix) {
    auto* buffer=next_text_buffer();auto* end=std::to_chars(buffer,buffer+255,value).ptr;std::memcpy(end,suffix.data(),suffix.size());end[suffix.size()]=0;commit_line(position);
}
void Renderer::write_ascii(const sprite::Vec3& position,const char* value) {if(line_count<320){strcpy_s(lines[line_count].text,256,value);commit_line(position);}}
void Renderer::write_affixed_integer(const sprite::Vec3& position,std::string_view prefix,std::int32_t value,std::string_view suffix) {
    auto* buffer=next_text_buffer();std::memcpy(buffer,prefix.data(),prefix.size());auto* end=std::to_chars(buffer+prefix.size(),buffer+255,value).ptr;std::memcpy(end,suffix.data(),suffix.size());end[suffix.size()]=0;commit_line(position);
}
void Renderer::write_prefixed_integer(const sprite::Vec3& position,std::string_view prefix,std::int32_t value) {
    auto* buffer=next_text_buffer();std::memcpy(buffer,prefix.data(),prefix.size());*std::to_chars(buffer+prefix.size(),buffer+255,value).ptr=0;commit_line(position);
}
void format_grouped_integer(char* buffer,std::int32_t capacity,std::int64_t value) {
    // Original453500 inserts decimal spacer digits by integer arithmetic,
    // then replaces them with commas. In particular its final branch uses
    // only five groups; it is not an unlimited locale-formatting operation.
    auto signed_bits=[](std::uint64_t bits){std::int64_t result;std::memcpy(&result,&bits,8);return result;};
    if(value<0){value=signed_bits(0-static_cast<std::uint64_t>(value));*buffer++='-';--capacity;}
    int commas=0;std::uint64_t packed=static_cast<std::uint64_t>(value);
    if(value>=1000){
        commas=value<1000000?1:value<1000000000?2:value<1000000000000LL?3:4;
        packed=static_cast<std::uint64_t>(value%1000);std::uint64_t factor=10000;std::int64_t divisor=1000;
        for(int group=1;group<=commas;++group){const auto part=(group==commas&&commas<4)?value/divisor:(value/divisor)%1000;packed+=static_cast<std::uint64_t>(part)*factor;factor*=10000;divisor*=1000;}
    }
    char* end;if(commas==1)end=std::to_chars(buffer,buffer+capacity,static_cast<std::uint32_t>(packed)).ptr;else end=std::to_chars(buffer,buffer+capacity,signed_bits(packed)).ptr;
    *end=0;for(int group=1;group<=commas;++group)end[-group*4]=',';
}
void Renderer::write_grouped_score(const sprite::Vec3& position,std::uint64_t score,std::int32_t final_digit){
    const std::uint64_t bits=score*10u+static_cast<std::uint64_t>(static_cast<std::int64_t>(final_digit));std::int64_t value;std::memcpy(&value,&bits,8);auto* buffer=next_text_buffer();format_grouped_integer(buffer,256,value);write_ascii(position,buffer);
}
}
