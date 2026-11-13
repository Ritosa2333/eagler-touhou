// Actual deferred startup owner and parser values; no graphics/audio devices.
#define main legacy_fixture_main
#include "th14-initial-stage-decode-fixture.cpp"
#undef main
#include "../../cpp/game/GameSessionResources.hpp"
#include <emscripten.h>
namespace {
struct Effects:StageResourceEffects {
    bool core_ready()const override{return false;}
    bool defer_initial_stage_decode()const override{return true;}
} effects;
GameResources source;
GameSessionResources actual;
AnmResource signature,expected_signature,expected_ascii,expected_text;
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int fixture_reset(){
    actual=GameSessionResources{};actual.effects=&effects;signature={};return actual.load_core(source);
}
EMSCRIPTEN_KEEPALIVE int fixture_open(const u8* bytes,u32 size){
    return source.open_archive(bytes,size)&&source.open_anm("sig.anm",expected_signature)
        &&source.open_anm("ascii.anm",expected_ascii)&&source.open_anm("text.anm",expected_text)&&fixture_reset();
}
EMSCRIPTEN_KEEPALIVE int fixture_signature(){return source.open_anm("sig.anm",signature);}
EMSCRIPTEN_KEEPALIVE int fixture_load(){return actual.load_core_bootstrap();}
EMSCRIPTEN_KEEPALIVE int fixture_empty(){
    return signature.scripts.empty()&&actual.core.ascii.scripts.empty()&&actual.core.text.scripts.empty();
}
EMSCRIPTEN_KEEPALIVE int fixture_compare(){
    return anm(signature,expected_signature)&&anm(actual.core.ascii,expected_ascii)&&anm(actual.core.text,expected_text);
}
EMSCRIPTEN_KEEPALIVE const char* fixture_error(){return actual.error.empty()?source.error().c_str():actual.error.c_str();}
}
