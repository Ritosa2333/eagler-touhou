#include "game/EndingScene.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace th13;
namespace {
struct Save : EndingSaveBytes {
    std::array<u8, 13> bytes{};
    u8 read(i32 i) const override { return bytes.at(i); }
    void write(i32 i, u8 v) override { bytes.at(i) = v; }
};
std::vector<u8> message(bool reload) {
    std::vector<u8> b{1,0,0,0,12,0,0,0,0,1,0,0};
    if (reload) {
        const char name[] = "staff.msg";
        b.insert(b.end(), {0,0,12,sizeof(name)});
        b.insert(b.end(), name, name + sizeof(name));
    }
    b.insert(b.end(), {0xff,0x7f,0,0});
    return b;
}
struct Host : EndingPlatform {
    Save save;
    bool reload = true;
    bool read_asset(const std::string& n, std::vector<u8>& b, bool) override {
        b = message(n != "staff.msg" && reload); return true;
    }
    EndingSaveBytes* save_bytes() override { return &save; }
    bool request_destination(i32) override { return true; }
    bool queue_music(i32,i32,const std::string&) override { return true; }
    bool fade_out_music(i32) override { return true; }
    bool advance_sound() override { return true; }
    bool ending_page_line(i32,u32,const std::string&) override { return true; }
};
}
int main() {
    SessionState progress;
    GameInput ctrl; ctrl.held = 0x200;
    Host first;
    EndingScene unseen(progress, first);
    assert(unseen.initialize());
    assert(unseen.update(ctrl));
    assert(!unseen.should_restart(ctrl)); // 00416637: first good ending, even in staff.
    Host multiplayer;
    EndingScene mp_staff(progress, multiplayer, true);
    assert(mp_staff.initialize() && mp_staff.update(ctrl));
    assert(mp_staff.should_restart(ctrl));
    Host known;
    known.save.bytes[12] = 1;
    EndingScene staff(progress, known);
    assert(staff.initialize());
    for (int i = 1; i <= 24; ++i) {
        assert(staff.update(ctrl));
        assert(staff.frames() == i);
        assert(staff.should_restart(ctrl) == (i % 12 != 0));
    }
    assert(staff.update(ctrl));
    GameInput z; z.held = 1; z.repeat_age[0] = 19;
    assert(!staff.should_restart(z));
    z.repeat_age[0] = 20;
    assert(staff.should_restart(z));
    assert(!staff.should_restart(GameInput{}));
    Host text; text.reload = false; text.save.bytes[12] = 1;
    EndingScene before_staff(progress, text);
    assert(before_staff.initialize() && before_staff.update(ctrl));
    assert(!before_staff.should_restart(ctrl)); // 0041663d: staff/reload bit required.
    std::puts("ending_fast_forward_test: PASS (first-view gate, staff gate, Ctrl/Z, 12-frame phase)");
}
