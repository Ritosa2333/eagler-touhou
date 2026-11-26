// 在GameStartEntry基础地址表之后注册菜单别名；入口和服务边界统一reconcile。
#pragma once
#include "core/runtime/GameStartEntryBindings.hpp"
#include "core/frontend/MenuSceneRuntimeContext.hpp"
#include "core/frontend/MenuReplayBindings.hpp"
#include <stdexcept>
namespace kaikeidou {
template<class Memory>
void bind_menu_scene_state(Memory& memory,GameStartEntryRefs& game,MenuSceneRuntimeContext& c,bool replay_variant){
    if(&game.weapon!=&c.selection.weapon||&game.practice_selected!=&c.selection.practice_selected)
        throw std::logic_error("Menu scene selection must own the shared game-start references");
    const auto shared=[&](MenuOptionsSharedContext& state){
        memory.bind(0xa8ce58,state.outer_scene);memory.bind(0xa87f48,state.phase);
        memory.bind(0xa87f4c,state.timer);memory.bind(0xa73230,state.sound_requests);
    };
    shared(c.options);shared(c.keys);
    memory.bind(0xa87f40,c.options.selected);memory.bind(0xa7a238,c.options.pending);
    memory.bind(0xa87f3c,c.keys.selected);memory.bind(0xa773b4,c.keys.pending);
    auto& room=c.music_room;
    memory.bind(0xa87f60,room.entry_x_bits);memory.bind(0xa87f34,room.selected);memory.bind(0xa87f38,room.scroll);
    memory.bind(0xa87f48,room.phase);memory.bind(0xa87f4c,room.timer);memory.bind(0xa8ce58,room.outer_scene);
    memory.bind(0xa733e4,room.bindings);memory.bind(0xa73234,room.cancel_sound);memory.bind(0xa73238,room.nav_sound);
    memory.bind(0xa74bac,c.selection.name_font);memory.bind(0xa74bb0,c.selection.detail_font);memory.bind(0xa74bb4,c.selection.list_font);
    memory.bind(0xa73018,c.selection.other_character_graphs[0]);memory.bind(0xa73038,c.selection.other_character_graphs[1]);
    memory.bind(0xa773e0,c.title.strips);memory.bind(0xa785f8,c.title.particles);memory.bind(0xa73118,c.title.objects);
    memory.bind(0xa78e34,c.assets.object_graphs);memory.bind(0xa74ba4,c.assets.help_fonts);
    memory.bind(0xa71d08,c.timing.data.data);memory.bind(0xb3e890,c.timing.previous_tick);
    memory.bind(0xa78e48,c.sound_handles);memory.bind(0xa71e08,c.keyboard_counts);memory.bind(0xa72208,c.logical_input);
    memory.bind(0x8b63f4,c.default_font);
    if(replay_variant){
        bind_menu_replay(memory,c.replay,c.title,c.default_font);
    }
}
}
