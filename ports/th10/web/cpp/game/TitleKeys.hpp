#pragma once
#include "TitleMain.hpp"
namespace th10 {
struct TitleKeysEnvironment : TitleMainEnvironment {
    u16 *active_bindings,*saved_bindings;
    virtual const u8* buttons()=0;
    virtual void bind_digit(AnmVm& vm,i32 sprite)=0;
};
struct TitleKeys {
    TitleMenu& title;
    TitleKeysEnvironment& environment;
    void draw_bindings();
    void assign(i32 action,i32 button);
    i32 update();
private:
    void restore();
    void leave();
};
}
