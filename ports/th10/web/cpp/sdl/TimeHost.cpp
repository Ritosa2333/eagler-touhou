#include "../game/Types.hpp"
#include <ctime>
extern "C" double sdl_loop_time();
extern "C" double monotonic(){return sdl_loop_time();}
extern "C" th10::i32 current_timestamp(){return static_cast<th10::i32>(std::time(nullptr));}
extern "C" void local_date(th10::i32 stamp,void* out){const std::time_t value=stamp;const auto* t=std::localtime(&value);th10::i32 fields[9]{};if(t){const th10::i32 actual[]{t->tm_sec,t->tm_min,t->tm_hour,t->tm_mday,t->tm_mon,t->tm_year,t->tm_wday,t->tm_yday,0};std::memcpy(fields,actual,sizeof(fields));}std::memcpy(out,fields,sizeof(fields));}
