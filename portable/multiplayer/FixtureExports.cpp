// Diagnostic setup only. Production multiplayer and ordinary builds never link
// this file; the following ticks still use native TH08 Player/Item/Menu owners.
#include "../../th08_web/cpp/platform/BrowserRuntime.hpp"
#include <algorithm>
#include "enemy-journal-fixture.hpp"
#include "screen-journal-fixture.hpp"
#include "pools-journal-fixture.hpp"
#include "resources-journal-fixture.hpp"
#include "world-journal-fixture.hpp"
#include "correction-fixture.hpp"
#include "../../th08_web/cpp/multiplayer/TextureJournal.hpp"
#ifndef TH_MULTIPLAYER_FIXTURES
#error Fixture exports must stay out of production builds
#endif
using namespace th08;
extern "C" {
__attribute__((export_name("mp_fixture_texture_restore")))
u32 mp_fixture_texture_restore(BrowserRuntime* r){
    if(!r||r->app.session.netplay.Configured())return 0;
    auto& store=r->app.textures;TexturePixels pixels;if(!pixels.create(4,4,22))return 0;
    const auto handle=store.insert(std::move(pixels));auto* original=store.get(handle);if(!original)return 0;
    const auto before=original->image.pixels;const auto revision=original->revision,live=store.live_count();
    multiplayer::TextureJournal journal;if(!journal.Bind(*r))return 0;
    if(!journal.BeginFrame(0)||!store.before_write(handle))return 0;
    original->image.pixels[0]=37;store.changed(handle);r->capture_screen(250);
    if(!journal.EndFrame()||!r->has_surface(250))return 0;
    if(!journal.BeginFrame(1))return 0;store.release(handle);
    TexturePixels replacement;if(!replacement.create(4,4,22))return 0;
    const auto created=store.insert(std::move(replacement));
    if(!journal.EndFrame()||store.get(handle)||!journal.UndoTo(0))return 0;
    const bool restored=store.get(handle)==original&&original->references==1&&
        original->revision==revision&&original->image.pixels==before&&
        !store.get(created)&&!r->has_surface(250)&&store.live_count()==live;
    journal.Clear();store.release(handle);return restored?1:0;
}
__attribute__((export_name("mp_fixture_presentation_frame")))
u32 mp_fixture_presentation_frame(BrowserRuntime* r,float alpha){
    return r&&alpha>=0&&alpha<=1&&r->app.draw(alpha,true,true,false)?1:0;
}
__attribute__((export_name("mp_fixture_audio_routing")))
u32 mp_fixture_audio_routing(BrowserRuntime* runtime){return runtime&&runtime->diagnostic_audio_routing()?1u:0u;}
__attribute__((export_name("mp_fixture_audio_clock")))
u32 mp_fixture_audio_clock(BrowserRuntime* runtime){return runtime&&runtime->diagnostic_audio_clock_independent()?1u:0u;}
__attribute__((export_name("mp_fixture_native_correction")))
const u32* mp_fixture_native_correction(BrowserRuntime* runtime){
    return runtime?multiplayer::fixture::correction_probe(*runtime):nullptr;
}
__attribute__((export_name("mp_fixture_world_journal")))
const u32* mp_fixture_world_journal(BrowserRuntime* runtime,u32 mode){
    return runtime?multiplayer::fixture::world_journal_probe(*runtime,mode):nullptr;
}
__attribute__((export_name("mp_fixture_resources_journal")))
const u32* mp_fixture_resources_journal(BrowserRuntime* runtime){
    return runtime?multiplayer::fixture::resources_journal_probe(runtime->app.textures):nullptr;
}
__attribute__((export_name("mp_fixture_enemy_journal")))
const u32* mp_fixture_enemy_journal(){return multiplayer::fixture::enemy_journal_probe();}
__attribute__((export_name("mp_fixture_screen_journal")))
const u32* mp_fixture_screen_journal(BrowserRuntime* runtime){
    return runtime?multiplayer::fixture::screen_journal_probe(runtime->app.renderer):nullptr;
}
__attribute__((export_name("mp_fixture_pools_journal")))
const u32* mp_fixture_pools_journal(BrowserRuntime* runtime){
    return runtime&&runtime->app.in_game()?multiplayer::fixture::pools_journal_probe(runtime->app.game,runtime->app.session):nullptr;
}
__attribute__((export_name("mp_fixture_die")))
u32 mp_fixture_die(BrowserRuntime* runtime,u32 seat){
    if(!runtime||!runtime->app.in_game())return 0;
    auto& app=runtime->app;auto& game=app.game;
    if(!game.ready()||seat>=app.session.player_count||!game.roster.eligible(seat)||game.retrying)return 0;
    auto& simulation=game.pilot(seat);auto& services=game.pilot_services(seat);
    app.session.pilot_values[seat].set_lives(0);
    simulation.status().life.state=0;simulation.status().life.timer.set(0);
    simulation.status().context.game_over=0;
    if(!services.prepare())return 0;
    simulation.die();services.finish();
    return !simulation.invalid()&&!services.invalid();
}
__attribute__((export_name("mp_fixture_place")))
u32 mp_fixture_place(BrowserRuntime* runtime,u32 seat,i32 x,i32 y,i32 drift_x,i32 drift_y){
    if(!runtime||!runtime->app.in_game())return 0;
    auto& game=runtime->app.game;
    if(!game.ready()||seat>=runtime->app.session.player_count||x<8||x>368||y<316||y>416)return 0;
    auto& movement=game.pilot(seat).status().motion.movement;
    movement.position.x=float(x);movement.position.y=float(y);
    for(auto& point:movement.history)point=movement.position;
    if(game.cooperation.seats[seat].spirit){
        if((drift_x!=1&&drift_x!=-1)||(drift_y!=1&&drift_y!=-1))return 0;
        game.cooperation.seats[seat].drift_x=i8(drift_x);
        game.cooperation.seats[seat].drift_y=i8(drift_y);
    }
    return 1;
}
__attribute__((export_name("mp_fixture_power")))
u32 mp_fixture_power(BrowserRuntime* runtime,u32 seat,i32 power){
    if(!runtime||!runtime->app.in_game())return 0;
    auto& app=runtime->app;auto& game=app.game;
    if(!game.ready()||seat>=app.session.player_count||power<0||power>128)return 0;
    app.session.pilot_values[seat].set_power(power);
    game.pilot_services(seat).sync_values();
    return 1;
}
__attribute__((export_name("mp_fixture_status")))
const i32* mp_fixture_status(BrowserRuntime* runtime){
    static i32 out[20]{};std::fill(out,out+20,0);
    if(!runtime||!runtime->app.in_game())return out;
    auto& game=runtime->app.game;
    out[0]=1;out[1]=game.menus.context.show_retry;out[2]=game.cooperation.wipe_progress;
    out[3]=game.cooperation.retry_pending;out[4]=game.menus.context.supervisor_state;
    for(u32 seat=0;seat<runtime->app.session.player_count;++seat){
        auto* lane=out+5+seat*5;
        lane[0]=game.cooperation.seats[seat].spirit;
        lane[1]=game.cooperation.seats[seat].progress;
        lane[2]=game.cooperation.seats[seat].target;
        lane[3]=Scalar::truncate(runtime->app.session.pilot_resources[seat].lives);
        lane[4]=game.pilot(seat).status().life.state;
    }
    return out;
}

// Controlled item initial conditions and real native homing operations. These
// are intentionally absent from ordinary and production multiplayer binaries.
__attribute__((export_name("mp_fixture_items")))
u32 mp_fixture_items(BrowserRuntime* runtime,u32 kind,u32 seat){
    if(!runtime||!runtime->app.in_game()||!runtime->app.game.ready()||
       runtime->app.game.retrying||seat>=runtime->app.session.player_count)return 0;
    auto& items=runtime->app.game.items;
    switch(kind){
    case 1:return items.spawn_power_gift({180,340,0},seat)?1:0;
    case 2:items.cancel_homing();return 1;
    case 3:items.collect_all();return 1;
    case 4:
        items.reset();
        for(u32 n=0;n<ItemPoolState::capacity-5;++n){
            const auto* value=items.spawn({100,30,0},0,0);
            if(!value||!value->active)return 0;
        }
        return !items.invalid();
    case 5:items.reset();return 1;
    case 6:{const auto* value=items.spawn({180,340,0},2,1);return value&&value->active?1:0;}
    default:return 0;
    }
}
__attribute__((export_name("mp_fixture_item_status")))
const u32* mp_fixture_item_status(BrowserRuntime* runtime){
    static u32 out[9]{};std::fill(out,out+9,0);
    if(!runtime||!runtime->app.in_game()||!runtime->app.game.ready())return out;
    out[0]=1;const auto& pool=runtime->app.game.items.status();
    for(u32 n=0;n<ItemPoolState::capacity;++n)out[1]+=pool.items[n].active?1u:0u;
    out[2]=u32(pool.next_index);out[3]=runtime->app.session.random.seed;out[4]=runtime->app.session.random.calls;
    for(u32 n=0;n<6;++n)out[5]+=pool.items[n].active?1u:0u;
    for(u32 seat=0;seat<3;++seat)out[6+seat]=runtime->app.game.items.assigned_gifts(seat);
    return out;
}
}
