// 专用显示夹具复用正式入口、显示计算和浏览器后台；写入接口只进入诊断构建。
#include "../../src/platform/web/Main.cpp"

extern "C" {
EMSCRIPTEN_KEEPALIVE int thkkd_test_word(unsigned address,unsigned value){return checked([&]{game->memory().word(address,value);game->memory().reconcile();return 0;});}
EMSCRIPTEN_KEEPALIVE int thkkd_test_real(unsigned address,double value){return checked([&]{std::uint64_t bits;std::memcpy(&bits,&value,8);game->memory().quad(address,bits);game->memory().reconcile();return 0;});}
EMSCRIPTEN_KEEPALIVE int thkkd_test_draw(unsigned entry){return checked([&]{
    platform->finish_gameplay();
    platform->invoke({MenuSceneApi::clear_draw_screen,{},{},{}});
    platform->invoke({static_cast<MenuSceneApi>(entry),{},{},{}});
    platform->invoke({MenuSceneApi::screen_flip,{},{},{}});
    return 0;
});}
}
