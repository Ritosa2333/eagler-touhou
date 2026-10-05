// Diagnostic setup only. Production multiplayer and ordinary builds never link
// this file; the following ticks still use native TH08 Player/Item/Menu owners.
#include "../../th08_web/cpp/platform/BrowserRuntime.hpp"
#include <algorithm>
#include <array>
#include <emscripten.h>
#include "enemy-journal-fixture.hpp"
#include "screen-journal-fixture.hpp"
#include "pools-journal-fixture.hpp"
#include "live-bullet-fixture.hpp"
#include "resources-journal-fixture.hpp"
#include "world-journal-fixture.hpp"
#include "correction-fixture.hpp"
#include "product-fixture.hpp"
#include "../../th08_web/cpp/multiplayer/TextureJournal.hpp"
#include "../../th08_web/cpp/multiplayer/RollbackDriver.hpp"

extern "C" __attribute__((export_name("mp_fixture_network_capture")))
th08::u32 mp_fixture_network_capture(th08::BrowserRuntime* r,th08::u32 frame){
    if(!r||!r->network_driver()||r->app.session.netplay.ReadOnly())return 0;
    // The diagnostic producer captures before Step. Announce those immutable
    // bytes to the same production channel that owns resend and repair.
    return const_cast<th08::multiplayer::NetworkConnection&>(r->network_driver()->Network()).Captured(frame);
}
#include "../../th08_web/cpp/game/Chain.hpp"
#include "../../th08_web/cpp/game/PlayerCollision.hpp"
#include "../../th08_web/cpp/game/BackgroundObjects.hpp"
#include "../../th08_web/cpp/sdl/GraphicsHost.hpp"
#ifndef TH_MULTIPLAYER_FIXTURES
#error Fixture exports must stay out of production builds
#endif
using namespace th08;
extern "C" {
__attribute__((export_name("mp_fixture_skip_resim_visual")))
u32 mp_fixture_skip_resim_visual(BrowserRuntime* r,u32 enabled){
    if(enabled>1)return 2;
    multiplayer::fixture_skip_resim_visual(enabled!=0);
    auto* d=r?r->network_driver():nullptr;
    return d&&!d->SetSkipResimVisual(enabled!=0)?2u:enabled;
}
__attribute__((export_name("mp_fixture_skip_resim_geometry")))
u32 mp_fixture_skip_resim_geometry(BrowserRuntime* r,u32 enabled){
    if(enabled>1)return 2;
    multiplayer::fixture_skip_resim_geometry(enabled!=0);
    auto* d=r?r->network_driver():nullptr;
    return d&&!d->SetSkipResimGeometry(enabled!=0)?2u:enabled;
}
__attribute__((export_name("mp_fixture_boundary_visual_redraws")))
u32 mp_fixture_boundary_visual_redraws(BrowserRuntime* r){
    const auto* d=r?r->network_driver():nullptr;
    return d?d->DiagnosticBoundaryVisualRedraws():0;
}
__attribute__((export_name("mp_fixture_rng_state")))
const u32* mp_fixture_rng_state(BrowserRuntime* r){
    static u32 out[4]{};std::fill(out,out+4,0);
    if(!r||!r->app.in_game())return out;
    const auto& rng=r->app.session.random;
    out[0]=1;out[1]=rng.seed;out[2]=rng.backup;out[3]=rng.calls;return out;
}
__attribute__((export_name("mp_fixture_texture_restore_stats")))
const u32* mp_fixture_texture_restore_stats(BrowserRuntime* r){
    static u32 out[3]{};std::fill(out,out+3,0);
    const auto* d=r?r->network_driver():nullptr;if(!d)return out;
    out[0]=1;out[1]=d->DiagnosticBackRestores();out[2]=d->DiagnosticBackSkipped();return out;
}
__attribute__((export_name("mp_fixture_world_instancing")))
u32 mp_fixture_world_instancing(u32 enabled){return fixture_world_instancing(enabled!=0)?1u:0u;}
__attribute__((export_name("mp_fixture_projection_reuse")))
u32 mp_fixture_projection_reuse(u32 enabled){return fixture_projection_reuse(enabled!=0)?1u:0u;}
__attribute__((export_name("mp_fixture_texture_coalesce")))
u32 mp_fixture_texture_coalesce(u32 enabled){return multiplayer::fixture_texture_coalesce(enabled!=0)?1u:0u;}
__attribute__((export_name("mp_fixture_back_metadata_only")))
u32 mp_fixture_back_metadata_only(BrowserRuntime* r,u32 enabled){
    if(enabled>1)return 2;
    multiplayer::fixture_back_metadata_only(enabled!=0);
    auto* d=r?r->network_driver():nullptr;
    return d&&!d->SetBackMetadataOnly(enabled!=0)?2u:enabled;
}
__attribute__((export_name("mp_fixture_back_metadata_state")))
u32 mp_fixture_back_metadata_state(BrowserRuntime* r){
    const auto* d=r?r->network_driver():nullptr;return d&&d->BackMetadataOnly();
}
__attribute__((export_name("mp_fixture_background_index")))
u32 mp_fixture_background_index(u32 enabled){return fixture_background_instance_index(enabled!=0)?1u:0u;}
__attribute__((export_name("mp_fixture_barrier_cache")))
u32 mp_fixture_barrier_cache(u32 enabled){return fixture_barrier_cache(enabled!=0);}
__attribute__((export_name("mp_fixture_update_optimizations")))
const u32* mp_fixture_update_optimizations(BrowserRuntime* r,u32 mode){
    return r?multiplayer::fixture::world_journal_probe(*r,mode,true):nullptr;
}
__attribute__((export_name("mp_fixture_chain_profile")))
const double* mp_fixture_chain_profile(){return fixture_chain_profile();}
__attribute__((export_name("mp_fixture_bullet_profile")))
const double* mp_fixture_bullet_profile(){return fixture_bullet_profile();}
__attribute__((export_name("mp_fixture_bullet_update_profile")))
const double* mp_fixture_bullet_update_profile(){return fixture_bullet_update_profile();}
__attribute__((export_name("mp_fixture_target_filter")))
u32 mp_fixture_target_filter(u32 enabled){return fixture_bullet_target_filter(enabled!=0)?1u:0u;}
__attribute__((export_name("mp_fixture_collision_broadphase")))
u32 mp_fixture_collision_broadphase(u32 enabled){return fixture_collision_broadphase(enabled!=0)?1u:0u;}
__attribute__((export_name("mp_fixture_collision_probe")))
u32 mp_fixture_collision_probe(){return fixture_collision_broadphase_probe()?1u:0u;}
__attribute__((export_name("mp_fixture_collision_counts")))
const double* mp_fixture_collision_counts(){return fixture_collision_broadphase_counts();}
__attribute__((export_name("mp_fixture_bullet_backend")))
u32 mp_fixture_bullet_backend(BrowserRuntime* r,u32 mode,u32 audit){
    auto* driver=r?r->network_driver():nullptr;if(!driver)return 0;
    if(mode<=1&&!driver->SetLiveBullets(mode!=0))return 0;
    driver->AuditBullets(audit!=0);
    return 1u+u32(driver->LiveBullets())+4u*driver->BulletAuditRestores();
}
__attribute__((export_name("mp_fixture_live_bullet_probe")))
const u32* mp_fixture_live_bullet_probe(BrowserRuntime* r){return r?multiplayer::fixture::live_bullet_probe(*r):nullptr;}
__attribute__((export_name("mp_fixture_early_input")))
u32 mp_fixture_early_input(BrowserRuntime* r,u32 value){
    auto* d=r?r->network_driver():nullptr;if(!d)return 2;
    if(value<2)d->diagnostic_early_input=value!=0;
    return d->diagnostic_early_input;
}
__attribute__((export_name("mp_fixture_checkpoint_span")))
u32 mp_fixture_checkpoint_span(BrowserRuntime* r,u32 span){
    auto* d=r?r->network_driver():nullptr;if(!d)return 0;
    if(span&& !d->SetCheckpointSpan(span))return 0;
    return d->CheckpointSpan();
}
__attribute__((export_name("mp_fixture_bomb_lazy")))
u32 mp_fixture_bomb_lazy(BrowserRuntime* r,u32 enabled){
    if(enabled>1)return 2;
    const bool result=multiplayer::fixture_bomb_lazy(enabled!=0);
    auto* d=r?r->network_driver():nullptr;
    if(d&&!d->SetBombLazy(enabled!=0))return 2;
    return result?1u:0u;
}
__attribute__((export_name("mp_fixture_shot_sparse")))
u32 mp_fixture_shot_sparse(BrowserRuntime* r,u32 enabled){
    if(enabled>1)return 2;
    const bool result=multiplayer::fixture_shot_sparse(enabled!=0);
    auto* d=r?r->network_driver():nullptr;
    if(d&&!d->SetShotSparse(enabled!=0))return 2;
    return result?1u:0u;
}
__attribute__((export_name("mp_fixture_record_sparse")))
u32 mp_fixture_record_sparse(BrowserRuntime* r,u32 enabled){
    if(enabled>1)return 2;
    const bool result=multiplayer::fixture_record_sparse(enabled!=0);
    auto* d=r?r->network_driver():nullptr;
    if(d&&!d->SetRecordSparse(enabled!=0))return 2;
    return result?1u:0u;
}
__attribute__((export_name("mp_fixture_title_spells_lazy")))
u32 mp_fixture_title_spells_lazy(BrowserRuntime* r,u32 enabled){
    if(enabled>1)return 2;
    const bool result=multiplayer::fixture_title_spells_lazy(enabled!=0);
    auto* d=r?r->network_driver():nullptr;
    if(d&&!d->SetTitleSpellsLazy(enabled!=0))return 2;
    return result?1u:0u;
}
__attribute__((export_name("mp_fixture_snapshot_modes")))
const u32* mp_fixture_snapshot_modes(BrowserRuntime* r){
    static u32 out[4]{};const auto* d=r?r->network_driver():nullptr;
    out[0]=d&&d->BombLazy();out[1]=d&&d->ShotSparse();out[2]=d&&d->RecordSparse();out[3]=d&&d->TitleSpellsLazy();return out;
}
__attribute__((export_name("mp_fixture_spell_record_probe")))
const u32* mp_fixture_spell_record_probe(BrowserRuntime* r){
    static u32 out[8]{};std::fill(out,out+8,0);
    if(!r||!r->app.in_game()||r->app.session.netplay.Configured())return out;
    multiplayer::WorldJournal journal;
    if(!journal.SetRecordSparse(true)||!journal.Bind(*r)){out[1]=1;return out;}
    const auto before=journal.AuditHash();const auto before_blocks=journal.BlockHashes();
    auto& game=r->app.game;EclVm enemy{};enemy.timeout=600;enemy.flags=1;enemy.pool_index=0;enemy.position={192,96,0};
    const auto original_record=r->app.session.records[0];const u32 shot=u32(game.globals.shot);
    const auto initial_enemy=enemy;
    u8 name[48]{},owner[48]{},comment1[64]{},comment2[64]{};
    const char* label="Snapshot spell";for(u32 i=0;i<48;++i){name[i]=u8((i<std::strlen(label)?label[i]:0)^0xaa);owner[i]=u8((i<5?"Owner"[i]:0)^0xbb);}
    for(u32 i=0;i<64;++i){comment1[i]=0xdd;comment2[i]=0xee;}
    const auto execute=[&](){
        if(!journal.BeginFrame(0)){out[1]=2;return false;}
        if(!game.spells.begin(enemy,0,-1,1000,name,owner,comment1,comment2)){out[1]=3;return false;}
        if(!journal.EndFrame()){out[1]=4;return false;}
        out[4]=u32(journal.DiagnosticBytesForFrame(0)[0]);
        if(!journal.BeginFrame(1)){out[1]=5;return false;}
        if(!game.spells.end()){out[1]=6;return false;}
        if(!journal.EndFrame()){out[1]=7;return false;}
        out[5]=u32(journal.DiagnosticBytesForFrame(1)[0]);
        return true;
    };
    if(!execute())return out;
    const auto& updated_record=r->app.session.records[0];
    if(shot>=13||updated_record.game.attempts[shot]<=original_record.game.attempts[shot]||
       updated_record.game.captures[shot]<=original_record.game.captures[shot]){out[1]=12;return out;}
    out[6]=updated_record.game.attempts[shot];out[7]=updated_record.game.captures[shot];
    const auto after=journal.AuditHash();const auto after_blocks=journal.BlockHashes();
    if(after==before){out[1]=8;return out;}
    if(!journal.UndoTo(0)||journal.AuditHash()!=before||journal.BlockHashes()!=before_blocks){out[1]=9;return out;}
    out[2]=1;enemy=initial_enemy;
    if(!execute()||journal.AuditHash()!=after||journal.BlockHashes()!=after_blocks){out[1]=10;return out;}
    out[3]=1;
    if(!journal.UndoTo(0)||journal.AuditHash()!=before||journal.BlockHashes()!=before_blocks){out[1]=11;return out;}
    out[0]=1;journal.Clear();return out;
}
__attribute__((export_name("mp_fixture_title_spells_probe")))
const u32* mp_fixture_title_spells_probe(BrowserRuntime* r){
    static u32 out[4]{};std::fill(out,out+4,0);
    if(!r||!r->app.in_game()||r->app.session.netplay.Configured())return out;
    multiplayer::WorldJournal journal;
    if(!journal.SetTitleSpellsLazy(true)||!journal.Bind(*r)){out[1]=1;return out;}
    const auto before=journal.AuditHash();const auto before_blocks=journal.BlockHashes();
    auto& spells=r->app.title.context.spells;
    std::array<u8,sizeof(spells)> original;std::memcpy(original.data(),spells,sizeof(spells));
    if(!journal.BeginFrame(0)||!journal.TouchTitleSpells()){out[1]=2;return out;}
    reinterpret_cast<u8*>(spells)[0]^=0x5a;
    if(!journal.EndFrame()||journal.AuditHash()==before){out[1]=3;return out;}
    out[2]=u32(journal.DiagnosticBytesForFrame(0)[0]);
    if(!journal.UndoTo(0)||std::memcmp(spells,original.data(),sizeof(spells))||
       journal.AuditHash()!=before||journal.BlockHashes()!=before_blocks){out[1]=4;return out;}
    out[0]=1;journal.Clear();return out;
}
__attribute__((export_name("mp_fixture_performance_mode")))
u32 mp_fixture_performance_mode(BrowserRuntime* r,u32 mode){
    auto* d=r?r->network_driver():nullptr;
    if(!d||mode>3||r->app.session.network_frame_open||r->app.session.netplay.RollbackFrame()!=Netplay::INVALID_FRAME)return 0;
    // 0 frontier; 1 always; 2 exact without snapshots; 3 exact with snapshots.
    d->diagnostic_exact_only=mode>=2;d->diagnostic_always_snapshot=mode==1||mode==3;
    return 1;
}
__attribute__((export_name("mp_fixture_performance_cost")))
const double* mp_fixture_performance_cost(BrowserRuntime* r){
    static double out[4]{};std::fill(out,out+4,0);
    const auto* d=r?r->network_driver():nullptr;if(!d)return out;
    out[0]=d->diagnostic_exact_only;out[1]=d->diagnostic_always_snapshot;
    out[2]=d->diagnostic_correction_ms;out[3]=d->diagnostic_correction_max_ms;return out;
}
__attribute__((export_name("mp_fixture_snapshot_owners")))
const double* mp_fixture_snapshot_owners(BrowserRuntime* r){
    static double out[12]{};std::fill(out,out+12,0);
    const auto* d=r?r->network_driver():nullptr;if(!d)return out;
    const auto& capture=d->DiagnosticWorldCaptureMs();
    for(u32 i=0;i<5;++i){out[i]=d->diagnostic_owner_bytes[i];out[i+6]=capture[i];}
    out[5]=d->diagnostic_owner_bytes[5];out[11]=d->diagnostic_texture_begin_ms;
    return out;
}
__attribute__((export_name("mp_fixture_snapshot_block_count")))
u32 mp_fixture_snapshot_block_count(BrowserRuntime* r){
    const auto* d=r?r->network_driver():nullptr;return d?u32(d->DiagnosticWorldBlockCount()):0;
}
__attribute__((export_name("mp_fixture_snapshot_block_bytes")))
u32 mp_fixture_snapshot_block_bytes(BrowserRuntime* r,u32 index){
    const auto* d=r?r->network_driver():nullptr;return d?u32(d->DiagnosticWorldBlockBytes(index)):0;
}
__attribute__((export_name("mp_fixture_snapshot_block_name")))
const char* mp_fixture_snapshot_block_name(BrowserRuntime* r,u32 index){
    const auto* d=r?r->network_driver():nullptr;return d?d->DiagnosticWorldBlockName(index):"";
}
__attribute__((export_name("mp_fixture_stage_clear_setup")))
u32 mp_fixture_stage_clear_setup(BrowserRuntime* r){
    if(!r||!r->app.in_game()||r->app.loading_game()||r->app.game.globals.stage!=0)return 0;
    auto& g=r->app.game;
    // Start at the native clear-clock animation, just before Shoot can shorten
    // it. Delayed P1 input then changes the exact stage-transition frame.
    g.display.clear_frames=2;g.display.clear_clock_old=660;
    g.display.clear_clock=690;g.display.clear_clock_display=682;g.display.clear_clock_delay=60;
    g.globals.game_flags&=~0x60u;
    return 1;
}
__attribute__((export_name("mp_fixture_dense_bullets")))
u32 mp_fixture_dense_bullets(BrowserRuntime* r,u32 count){
    if(!r||!r->app.in_game()||r->app.loading_game()||count>1200)return 0;
    auto& g=r->app.game;
    for(u32 i=0;i<count;++i){
        BulletEmission shot;shot.sprite=0;shot.color=i%8;
        shot.position={24.f+float(i%40)*8.f,32.f+float(i/40)*5.f,0};
        shot.speed=shot.ending_speed=0;shot.angle=0;shot.count=shot.layers=1;shot.pattern=0;
        g.bullets.emit(shot);if(g.bullets.invalid())return 0;
    }
    return 1;
}
__attribute__((export_name("mp_fixture_snapshot_profile")))
const double* mp_fixture_snapshot_profile(BrowserRuntime* r,u32 policy){
    static double out[10]{};std::fill(out,out+10,0);
    auto* d=r?r->network_driver():nullptr;if(!d)return out;
    if(policy<2)d->diagnostic_always_snapshot=policy==1;
    out[0]=1;out[1]=d->diagnostic_always_snapshot;out[2]=d->diagnostic_snapshots;out[3]=d->diagnostic_skipped;
    out[4]=d->diagnostic_capture_ms;out[5]=d->diagnostic_restore_ms;out[6]=d->diagnostic_update_ms;
    out[7]=d->diagnostic_draw_ms;out[8]=d->diagnostic_snapshot_bytes;out[9]=r->app.game.projectile_pool.active_count;
    return out;
}
__attribute__((export_name("mp_fixture_death_field")))
u32 mp_fixture_death_field(BrowserRuntime* r,u32 victim,u32 bombs,u32 scenario){
    if(!r||!r->app.in_game()||!r->app.game.ready()||r->app.session.player_count!=2||victim>1||bombs>3||scenario>1)return 0;
    auto& g=r->app.game;auto& s=r->app.session;
    // Author initial conditions only at a fully confirmed boundary. No reset,
    // no direct die(), no injected death button; movement must cause the hit.
    const float y=scenario==0?128.f:320.f;
    for(u32 seat=0;seat<2;++seat){
        auto& p=g.pilot(seat).status();
        if(g.cooperation.seats[seat].spirit||p.life.state==2)return 0;
        s.pilot_values[seat].set_lives(2);s.pilot_values[seat].set_power(seat==victim?80:48);
        s.pilot_values[seat].set_bombs(i32(bombs));g.roster.seats[seat].gauge->set(seat==victim?6000:-6000);
        p.life.state=0;p.life.timer.set(0);p.life.clear_frames=0;
        p.context.game_over=0;p.context.pause=0;
        p.motion.movement.position={seat==victim?112.f:256.f,seat==victim?y:y+16.f,0};
        for(auto& point:p.motion.movement.history)point=p.motion.movement.position;
        g.pilot_services(seat).sync_values();
    }
    s.pilot_values[0].add_time_orbs(2400-s.numbers.time_orbs);
    s.numbers.last_spell_requirement=1000;
    for(u32 seat=0;seat<2;++seat)g.pilot_services(seat).sync_values();
    for(u32 i=0;i<48;++i){
        // Keep this control below full Power: otherwise the pickup clear can
        // legitimately remove the lethal projectile before the player hits it.
        const i32 type=i%4==0?(i<8?2:0):i%4==1?7:i%4==2?10:0;
        const Vec3 p{88.f+float(i%8)*24.f,scenario==0?190.f+float(i/8)*24.f:260.f+float(i/8)*16.f,0};
        const auto* item=g.items.spawn(p,type,i%3==0?1:0);if(!item||!item->active)return 0;
    }
    // A stationary normal projectile lies ahead of the victim's Up path.
    // Other stationary projectiles survive until native respawn clears them.
    for(u32 i=0;i<25;++i){
        BulletEmission shot;shot.sprite=0;shot.color=0;shot.position=i==0?Vec3{112,y-32.f,0}:Vec3{32.f+float(i%8)*44.f,24.f+float(i/8)*16.f,0};
        shot.speed=shot.ending_speed=0;shot.angle=0;shot.count=shot.layers=1;shot.pattern=0;
        g.bullets.emit(shot);if(g.bullets.invalid())return 0;
    }
    return !g.items.invalid();
}
__attribute__((export_name("mp_fixture_route_history")))
u32 mp_fixture_route_history(BrowserRuntime* r,u32 route){
    if(!r||route>2||r->app.in_game())return 0;
    auto& s=r->app.session;const auto shot=s.multiplayer_session.characters[0];
    if(shot>=12)return 0;
    std::memset(s.clears,0,sizeof(s.clears));
    if(route==1)s.clears[shot].with_retries[0]|=64;
    else if(route==2)s.clears[shot].without_retries[0]|=128;
    return 1;
}
__attribute__((export_name("mp_fixture_local_route")))
u32 mp_fixture_local_route(BrowserRuntime* r,u32 route){
    if(!r||route>2||r->app.in_game()||r->app.session.multiplayer_session.session_id)return 0;
    r->app.session.multiplayer_route_state=u8(route);return 1;
}
__attribute__((export_name("mp_fixture_product_hud")))
const float* mp_fixture_product_hud(BrowserRuntime* r){return r?multiplayer::fixture::product_hud(*r):nullptr;}
__attribute__((export_name("mp_fixture_product_gauge")))
u32 mp_fixture_product_gauge(BrowserRuntime* r,u32 viewer){return r&&multiplayer::fixture::product_gauge(*r,viewer);}
__attribute__((export_name("mp_fixture_product_gauge_purity")))
u32 mp_fixture_product_gauge_purity(BrowserRuntime* r){return r&&multiplayer::fixture::product_gauge_draw_pure(*r);}
__attribute__((export_name("mp_fixture_product_guest_target")))
u32 mp_fixture_product_guest_target(BrowserRuntime* r){return r&&multiplayer::fixture::product_guest_target(*r);}
__attribute__((export_name("mp_fixture_product_presentation_clock")))
u32 mp_fixture_product_presentation_clock(BrowserRuntime* r){return r&&multiplayer::fixture::product_presentation_clock(*r);}
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
__attribute__((export_name("mp_fixture_gpu_history")))
u32 mp_fixture_gpu_history(BrowserRuntime* r){
    if(!r||r->app.session.netplay.Configured())return 0;
    auto* renderer=touhou::sdl::current();if(!renderer)return 0;renderer->state.target=r->backbuffer();
    r->app.renderer.flush();renderer->read(r->backbuffer());const auto original=r->app.textures.get(r->backbuffer())->image.pixels;
    multiplayer::TextureJournal journal;if(!journal.Bind(*r))return 0;
    for(u32 cycle=0;cycle<16;++cycle){
        if(!journal.BeginFrame(0))return 0;
        renderer->clear(1,0xff123456u+cycle*0x010101u,1,0);
        r->app.renderer.flush();renderer->read(r->backbuffer());const auto first=r->app.textures.get(r->backbuffer())->image.pixels;
        if(first==original||!journal.EndFrame()||!journal.BeginFrame(1))return 0;
        renderer->clear(1,0xffabcdefu-cycle*0x010101u,1,0);
        if(!journal.EndFrame()||!journal.UndoTo(1))return 0;
        r->app.renderer.flush();renderer->read(r->backbuffer());if(r->app.textures.get(r->backbuffer())->image.pixels!=first)return 0;
        if(!journal.UndoTo(0))return 0;
        r->app.renderer.flush();renderer->read(r->backbuffer());if(r->app.textures.get(r->backbuffer())->image.pixels!=original)return 0;
    }
    // Confirmation retires images for reuse without restoring old pixels.
    for(u32 frame=0;frame<24;++frame){
        if(!journal.BeginFrame(frame)||!journal.EndFrame())return 0;
        journal.DiscardBefore(frame+1);
    }
    journal.Clear();return 1;
}
__attribute__((export_name("mp_fixture_presentation_frame")))
u32 mp_fixture_presentation_frame(BrowserRuntime* r,float alpha){
    if(!r||alpha<0||alpha>1)return 0;
    const auto& g=r->app.game;
    const bool frozen=r->app.in_game()&&(g.paused||g.retrying||g.menus.context.pause_state||g.menus.context.show_retry);
    return r->app.draw(alpha,true,true,!frozen)?1:0;
}
__attribute__((export_name("mp_fixture_enemy_draw_purity")))
const u32* mp_fixture_enemy_draw_purity(BrowserRuntime* r,float alpha){
    static u32 out[128]{};std::fill(out,out+128,0);
    if(!r||!r->app.in_game())return out;
    std::vector<std::pair<u32,std::vector<u8>>> before;
    for(u32 index=0;index<481;++index)if(const auto* e=r->app.game.enemies.population.at(index)){
        const auto* p=reinterpret_cast<const u8*>(e);
        before.emplace_back(index,std::vector<u8>(p,p+sizeof(*e)));
    }
    if(!mp_fixture_presentation_frame(r,alpha))return out;
    out[0]=1;out[3]=sizeof(EclVm);out[4]=offsetof(EclVm,animation);out[5]=sizeof(AnmVm);
    out[6]=offsetof(EclVm,resolved_position);out[7]=offsetof(EclVm,position);out[8]=offsetof(EclVm,trail);
    for(const auto& [index,bytes]:before){
        const auto* e=r->app.game.enemies.population.at(index);
        if(!e){out[0]=3;out[1]=index;return out;}
        const auto* p=reinterpret_cast<const u8*>(e);
        for(u32 offset=0;offset<sizeof(*e);++offset)if(bytes[offset]!=p[offset]){
            if(out[0]==1){out[0]=2;out[1]=index;}
            if(out[1]!=index)continue;
            const u32 count=out[2]++;
            if(count<38){out[12+count*3]=offset;out[13+count*3]=bytes[offset];out[14+count*3]=p[offset];}
        }
    }
    return out;
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
__attribute__((export_name("mp_fixture_ordinary_death_setup")))
u32 mp_fixture_ordinary_death_setup(BrowserRuntime* runtime,u32 seat){
    if(!runtime||!runtime->app.in_game())return 0;
    auto& app=runtime->app;auto& game=app.game;
    if(!game.ready()||seat>=app.session.player_count||!game.roster.eligible(seat)||game.retrying)return 0;
    auto& simulation=game.pilot(seat);auto& services=game.pilot_services(seat);auto& state=simulation.status();
    // Prepare a first ordinary death without causing it here. The actual hit is
    // injected by a fixture-only committed input bit from inside the netplay
    // frame, after rollback/audio ownership has opened.
    app.session.pilot_values[seat].set_lives(2);
    app.session.pilot_values[seat].set_power(80);
    app.session.pilot_values[seat].set_bombs(0);
    services.sync_values();
    state.context.pause=0;state.context.gauge=0;state.context.game_over=0;
    state.life.state=0;state.life.timer.set(0);state.life.predead_count=0;
    state.life.auto_bomb=0;state.life.deathbomb=0;state.life.predead_effect=nullptr;
    state.motion.animation.flag17=0;
    game.items.cancel_homing(seat);
    return state.life.state==0&&!simulation.invalid()&&!services.invalid();
}
__attribute__((export_name("mp_fixture_grazed_bullet_hit")))
u32 mp_fixture_grazed_bullet_hit(BrowserRuntime* runtime,u32 seat){
    if(!runtime||!runtime->app.in_game())return 0;
    auto& app=runtime->app;auto& game=app.game;
    if(!game.ready()||seat>=app.session.player_count||!game.roster.eligible(seat)||game.retrying)return 0;
    auto& pilot=game.pilot(seat);auto& state=pilot.status();
    state.life.state=0;state.life.timer.set(0);state.context.game_over=0;
    const auto position=state.motion.movement.position;
    BulletEmission shot;shot.sprite=0;shot.color=0;shot.position=position;
    shot.angle=0;shot.speed=0;shot.ending_speed=0;shot.count=1;shot.layers=1;shot.pattern=0;
    game.bullets.emit(shot);
    BulletState* bullet=nullptr;
    for(auto& candidate:game.projectile_pool.bullets)if(candidate.state){bullet=&candidate;break;}
    if(!bullet)return 0;
    bullet->state=1;bullet->position=position;bullet->velocity={};bullet->active_time.set(20);
    bullet->grazed|=u8(1u<<seat);
    if(!game.bullets.update())return 0;
    return state.life.state==2?1:0;
}
__attribute__((export_name("mp_fixture_cancel_reward_owner")))
u32 mp_fixture_cancel_reward_owner(BrowserRuntime* runtime){
    if(!runtime||!runtime->app.in_game())return 0;
    auto& app=runtime->app;auto& game=app.game;
    if(!game.ready()||app.session.player_count!=2||game.retrying)return 0;
    game.items.reset();
    auto& p1=game.pilot(0).status();auto& p2=game.pilot(1).status();
    p1.cancel_item=6;p2.cancel_item=6;
    p1.motion.movement.position={72,352,0};p2.motion.movement.position={210,352,0};
    for(auto& point:p1.motion.movement.history)point=p1.motion.movement.position;
    for(auto& point:p2.motion.movement.history)point=p2.motion.movement.position;
    // Reproduce the first ordinary-death clear: this seat's barrier explicitly
    // requests no reward. The old multiplayer path nevertheless read P1's
    // live cancel_item (normally 6) after P2 returned the barrier hit.
    p2.shots.regions.rectangle(false,{p2.motion.movement.position.x,p2.motion.movement.position.y},64,64,-1,2);
    BulletEmission shot;shot.sprite=0;shot.color=0;shot.position=p2.motion.movement.position;
    shot.angle=0;shot.speed=0;shot.ending_speed=0;shot.count=1;shot.layers=1;shot.pattern=0;
    game.bullets.emit(shot);
    BulletState* bullet=nullptr;
    for(auto& candidate:game.projectile_pool.bullets)if(candidate.state){bullet=&candidate;break;}
    if(!bullet)return 0;
    bullet->state=1;bullet->position=p2.motion.movement.position;bullet->velocity={};bullet->active_time.set(20);
    if(!game.bullets.update())return 0;
    u32 items=0;for(u32 n=0;n<ItemPoolState::capacity;++n)items+=game.items.status().items[n].active?1u:0u;
    return p1.cancel_item==6&&p2.cancel_item==-1&&items==0?1u:0u;
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
__attribute__((export_name("mp_fixture_team_extend")))
u32 mp_fixture_team_extend(BrowserRuntime* runtime){
    if(!runtime||!runtime->app.in_game()||!runtime->app.game.ready())return 0;
    runtime->app.game.items.award_team_extend();
    return runtime->app.game.items.invalid()?0u:1u;
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

__attribute__((export_name("mp_fixture_native_bombs")))
i32 mp_fixture_native_bombs(BrowserRuntime* runtime,u32 seat){
    if(!runtime||!runtime->app.in_game()||seat>=runtime->app.session.player_count)return -1;
    return Scalar::truncate(runtime->app.game.pilot(seat).profile(false).initial_bombs);
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
            const auto* value=items.spawn_single({100,30,0},0,0);
            if(!value||!value->active)return 0;
        }
        return !items.invalid();
    case 5:items.reset();return 1;
    case 6:{const auto* value=items.spawn({180,340,0},2,1);return value&&value->active?1:0;}
    case 7:case 8:case 9:{
        items.reset();
        if(kind==9)items.spawn({180,100,0},3,2);
        else items.spawn_enemy_drop({180,100,0},kind==7?3:5,0);
        u32 count=0;const ItemState* first=nullptr;
        for(const auto& item:items.status().items)if(item.active){
            if(first&&item.position.x==first->position.x)return 0;
            if(kind!=9&&runtime->app.session.player_count==3&&item.velocity.y>=0)return 0;
            first=&item;++count;
        }
        return count;
    }
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
// Control initial conditions, then measure real native spawning and pickup.
// No reward model or replacement item-update loop is used by this fixture.
__attribute__((export_name("mp_fixture_power_drops")))
const i32* mp_fixture_power_drops(BrowserRuntime* runtime,u32 kind,u32 mode){
    static i32 out[24]{};std::fill(out,out+24,0);
    if(!runtime||!runtime->app.in_game()||!runtime->app.game.ready()||
       (kind!=0&&kind!=2&&kind!=4)||(mode!=0&&mode!=2))return out;
    auto& app=runtime->app;auto& game=app.game;auto& items=game.items;
    items.reset();
    for(u32 seat=0;seat<app.session.player_count;++seat){
        app.session.pilot_values[seat].set_power(0);game.pilot_services(seat).sync_values();
        auto& state=game.pilot(seat).status();state.life.state=0;
        state.motion.form.focused=0;state.motion.movement.position={340.f,410.f,0};
        auto& pilot=game.pilot(seat);
        move_player(state.motion.movement,pilot.profile(false),pilot.profile(true),false,
                    state.context.character,0,state.input.minimum,state.input.extent,pilot.timing,nullptr,false);
    }
    const auto rng_calls=app.session.random.calls;
    items.spawn({192,340,0},i32(kind),i32(mode));
    std::array<ItemState*,3> drops{};u32 count=0;
    auto& pool=const_cast<ItemPoolState&>(items.status());
    for(u32 slot=0;slot<ItemPoolState::capacity;++slot)if(pool.items[slot].active){
        if(count>=drops.size())return out;
        auto& item=pool.items[slot];drops[count]=&item;
        out[3+count]=Scalar::truncate(item.position.x*100);
        out[18+count]=Scalar::truncate(item.velocity.y*100);
        out[21+count]=item.state;
        if(item.type!=i32(kind))return out;
        ++count;
    }
    out[1]=i32(count);out[2]=i32(app.session.random.calls-rng_calls);
    for(u32 tick=0;tick<10;++tick)if(!items.update())return out;
    for(u32 index=0;index<count;++index){
        out[12+index]=Scalar::truncate(drops[index]->position.x*100);
        out[15+index]=Scalar::truncate(drops[index]->position.y*100);
    }
    for(u32 selected=0;selected<count;++selected){
        for(u32 index=selected;index<count;++index){
            auto& item=*drops[index];item.state=0;item.velocity={};
            item.position={40.f+18.f*float(index),64.f,0};
        }
        drops[selected]->position={192,340,0};
        game.pilot(0).status().motion.movement.position={192,340,0};
        auto& pilot=game.pilot(0);auto& state=pilot.status();
        move_player(state.motion.movement,pilot.profile(false),pilot.profile(true),false,
                    state.context.character,0,state.input.minimum,state.input.extent,pilot.timing,nullptr,false);
        const i32 before=Scalar::truncate(app.session.pilot_resources[0].power);
        if(!items.update()||drops[selected]->active)return out;
        out[6+selected]=Scalar::truncate(app.session.pilot_resources[0].power)-before;
    }
    out[9]=Scalar::truncate(app.session.pilot_resources[0].power);
    out[10]=Scalar::truncate(app.session.pilot_resources[1].power);
    out[11]=Scalar::truncate(app.session.pilot_resources[2].power);
    out[0]=1;return out;
}
__attribute__((export_name("mp_fixture_poc_setup")))
u32 mp_fixture_poc_setup(BrowserRuntime* runtime,u32 collector){
    if(!runtime||!runtime->app.in_game()||!runtime->app.game.ready()||
       runtime->app.session.player_count!=2||collector>2)return 0;
    auto& app=runtime->app;auto& game=app.game;auto& items=game.items;
    items.reset();
    for(u32 seat=0;seat<2;++seat){
        app.session.pilot_values[seat].set_power(0);game.pilot_services(seat).sync_values();
        auto& movement=game.pilot(seat).status().motion.movement;
        const bool collecting=collector==2||seat==collector;
        movement.position={seat?210.f:72.f,collecting?64.f:352.f,0};
        for(auto& point:movement.history)point=movement.position;
    }
    for(u32 i=0;i<8;++i){
        const Vec3 p{200.f+float(i%4)*6.f,300.f+float(i/4)*8.f,0};
        auto* item=items.spawn(p,0,0);if(!item||!item->active)return 0;
    }
    return 1;
}
__attribute__((export_name("mp_fixture_poc_crossing_setup")))
u32 mp_fixture_poc_crossing_setup(BrowserRuntime* runtime,u32 collector){
    if(!runtime||!runtime->app.in_game()||!runtime->app.game.ready()||
       runtime->app.session.player_count!=2||collector>1)return 0;
    auto& app=runtime->app;auto& game=app.game;auto& items=game.items;
    // This setup is intentionally safe after rollback history already exists:
    // do not reset the pool.  Both independent worlds receive the same eight
    // authored Power items immediately before frame 180, then only netplay
    // input moves the collector across the POC line.  A remote endpoint will
    // predict the old position for a few frames and must repair the claim via
    // rollback once the delayed Up input arrives.
    for(u32 seat=0;seat<2;++seat){
        app.session.pilot_values[seat].set_power(0);game.pilot_services(seat).sync_values();
        auto& movement=game.pilot(seat).status().motion.movement;
        movement.position={seat?210.f:72.f,seat==collector?140.f:352.f,0};
        for(auto& point:movement.history)point=movement.position;
    }
    for(u32 i=0;i<8;++i){
        const Vec3 p{180.f+float(i%4)*8.f,220.f+float(i/4)*8.f,0};
        auto* item=items.spawn(p,0,0);if(!item||!item->active)return 0;
    }
    return 1;
}
__attribute__((export_name("mp_fixture_power_type_guard")))
u32 mp_fixture_power_type_guard(BrowserRuntime* runtime){
    if(!runtime||!runtime->app.in_game()||!runtime->app.game.ready()||runtime->app.session.player_count<2)return 0;
    auto& app=runtime->app;auto& game=app.game;auto& items=game.items;
    items.reset();
    for(u32 seat=0;seat<app.session.player_count;++seat){
        app.session.pilot_values[seat].set_power(128);game.pilot_services(seat).sync_values();
    }
    auto* ordinary=items.spawn({180,280,0},0,0);if(!ordinary||!ordinary->active||ordinary->type!=0)return 0;
    auto* full=items.spawn({200,280,0},4,0);if(!full||!full->active||full->type!=4)return 0;
    items.convert_power_items(*full);
    return ordinary->active&&ordinary->type==0?1:0;
}
}
