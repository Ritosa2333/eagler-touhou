#pragma once
#include "TitleMenus.hpp"
namespace th09 {
// Native th09.cfg layout is retained only as a file format. Platform handles
// and obsolete renderer capabilities are never interpreted by the game.
class GameConfiguration {
    std::array<u8,204> bytes{};
public:
    GameConfiguration(){reset();}
    void reset();
    bool load(const u8*,u32);
    void apply(TitleSettings&)const;
    bool capture(const TitleSettings&);
    const std::array<u8,204>& data()const{return bytes;}
    u8 extra_lives()const{return bytes[0xac];}
    static InputBindings default_bindings();
};
}
