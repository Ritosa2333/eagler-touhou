#include "../game/TextFormat.hpp"
extern "C" __attribute__((export_name("text_format"))) th10::i32 text_format(char* output,th10::u32 capacity,const char* format,const th10::u32* words,th10::u32 count){return th10::format_text(output,capacity,format,words,count);}
