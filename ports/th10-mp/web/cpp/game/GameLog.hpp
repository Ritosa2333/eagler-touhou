#pragma once
#include "Types.hpp"
namespace th10 {
struct GameLogEnvironment {
    u8* lock_depth;
    virtual void enter()=0;
    virtual void leave()=0;
    virtual void format(char* output,u32 capacity,const char* format,const u32* arguments)=0;
};
struct GameLog {
    char text[8192];char* cursor;u8 has_error;u8 reserved[3];
    const char* append(const char* format,const u32* arguments,bool error,GameLogEnvironment& environment);
};
static_assert(offsetof(GameLog,cursor)==0x2000&&sizeof(GameLog)==0x2008);
}
