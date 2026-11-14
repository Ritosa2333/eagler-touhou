#pragma once
#include "GameEconomy.hpp"
namespace th11 {
#if PORT_TITLE == 14
struct ContinueActionsTh14 {
    virtual ~ContinueActionsTh14()=default;
    virtual bool bombs(i32,i32)noexcept{return true;}
    virtual bool power_notice()noexcept{return true;}
    virtual bool options()noexcept{return true;}
    virtual bool lives(i32,i32)noexcept{return true;}
};
bool th14_continue_apply(GameEconomy&,i32& continues,ContinueActionsTh14&)noexcept;
bool th14_continue_economy(GameEconomy&,i32& continues)noexcept;
#endif
}
