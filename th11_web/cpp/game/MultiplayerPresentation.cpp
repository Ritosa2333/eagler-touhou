#ifdef TH11_MULTIPLAYER
#include "GameBattle.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
namespace th11 {
namespace {float local_graze_y(unsigned seats){return seats==2?248.f:320.f;}}
bool GameBattle::mp_initialize_presentation(){
    mp_presentation.script_rng={u16(mp_options.seed),0,0};mp_presentation.visual_rng={u16(mp_options.seed>>16),0,0};
    auto* focus=mp_presentation.create(resources.core.bullet,74,6,15);if(!focus)return false;mp_focus_marker=focus->id;
    for(unsigned seat=0;seat<mp_options.seat_count;++seat){
        for(unsigned i=0;i<9;++i){auto& vm=mp_life_icons[seat][i];if(!vm.bind_script(resources.core.front,10+i,5,&mp_presentation.rate)||vm.update(mp_presentation)<0)return false;}
        for(unsigned i=0;i<4;++i){auto& vm=mp_communication_icons[seat][i];if(!vm.bind_script(resources.core.front,32+i,5,&mp_presentation.rate)||vm.update(mp_presentation)<0)return false;}
    }
    // HiScore/Score retain the original single owner and original position.
    // Life/Power repeat by seat; the local Graze row follows all seat groups.
    if(auto* frame=animations.find(hud.frame_animation))for(auto* node=frame->child.next;node;node=node->next)if(node->value->script_index==7||node->value->script_index==8||node->value->script_index==9)node->value->flags&=~2u;
    return mp_update_presentation();
}
bool GameBattle::mp_update_presentation(){
    mp_presentation.rate=animations.rate;
    if(!mp_presentation.update(true)||!mp_presentation.update(false))return false;
    for(unsigned seat=0;seat<mp_options.seat_count;++seat){const auto& e=pilots[seat]->economy;auto& life=mp_life_icons[seat];auto& comm=mp_communication_icons[seat];
        const i32 count=std::clamp(e.lives,0,9);
        if(mp_display_lives[seat]!=count||mp_display_fragments[seat]!=i32(mp_fragments)){
            unsigned i=0;for(;i<unsigned(count);++i){life[i].flags|=2;life[i].pending_interrupt=2;}
            if(i<9){life[i].flags|=2;life[i++].pending_interrupt=i16(mp_fragments+7);}
            for(;i<9;++i)life[i].flags&=~2u;
            mp_display_lives[seat]=count;mp_display_fragments[seat]=i32(mp_fragments);
        }
        for(auto& vm:life)if(vm.update(mp_presentation)<0)return false;
        // Preserve the native lower-left display's response when its own
        // player covers it. Every seat has the same fixed presentation clock;
        // selecting the local view below never changes a shared owner.
        const auto position=pilots[seat]->player->motion.state.position;
        if(!mp_communication_hidden[seat]&&position.y>416&&position.x<-64){for(auto& vm:comm)vm.pending_interrupt=3;mp_communication_hidden[seat]=true;}
        else if(mp_communication_hidden[seat]&&(position.y<400||position.x>-64)){for(auto& vm:comm)vm.pending_interrupt=2;mp_communication_hidden[seat]=false;}
        for(auto& vm:comm)if(vm.update(mp_presentation)<0)return false;
        const i32 percentage=signed_bits(u32(e.communication)*100)/10000;
        if(mp_tick>=20){const unsigned mode=percentage<100?0:percentage<110?1:percentage<120?2:3;if(mode!=mp_communication_modes[seat])comm[1].pending_interrupt=comm[2].pending_interrupt=i16(7+mode);mp_communication_modes[seat]=mode;}
        const i32 blocks=(e.communication>=10000?100:percentage)/10;auto& bar=comm[1];
        bar.uv[1].x=bar.uv[3].x=float(double(bar.uv[0].x)+.017578125+double(blocks)*4*.001953125);bar.flags|=8;bar.sprite_size.x=float(double(blocks)*4+9);comm[2].flags|=8;comm[2].sprite_size.x=0;
    }
    return true;
}
void GameBattle::mp_prepare_presentation(AnmRenderer& renderer){
    if(stage)stage->multiplayer_frozen_draw=mp_paused||mp_finished;
    if(outgoing_stage)outgoing_stage->multiplayer_frozen_draw=mp_paused||mp_finished;
    renderer.multiplayer_animations=&animations;renderer.multiplayer_opacity.fill(255);
    renderer.multiplayer_hud_background=nullptr;
    if(auto* frame=animations.find(hud.frame_animation))for(auto* node=frame->child.next;node;node=node->next)if(node->value->script_index==2)renderer.multiplayer_hud_background=node->value;
    const auto& local=*pilots[mp_options.local_seat];
    for(unsigned i=0;i<mp_options.seat_count;++i){const auto& p=*pilots[i];if(i==mp_options.local_seat){renderer.multiplayer_opacity[i+1]=p.ghost?112:255;continue;}const auto a=local.player->motion.state.position,b=p.player->motion.state.position;const double x=double(a.x)-b.x,y=double(a.y)-b.y;renderer.multiplayer_opacity[i+1]=p.ghost?88:x*x+y*y<2304?72:160;}
    // Rescue feedback follows the same deterministic recipient selection as
    // the transfer rule. It changes only the submitted ghost alpha.
    for(unsigned giver=0;giver<mp_options.seat_count;++giver){const auto& p=*pilots[giver];if(!p.life_hold)continue;const int receiver=mp_life_receiver(giver);if(receiver<0||!pilots[unsigned(receiver)]->ghost)continue;
        const unsigned owner=unsigned(receiver)+1,base=unsigned(receiver)==mp_options.local_seat?112:88;
        renderer.multiplayer_opacity[owner]=u8(std::max<unsigned>(renderer.multiplayer_opacity[owner],base+(255-base)*std::min(p.life_hold,90u)/90));
    }
}
bool GameBattle::mp_draw_players(AnmRenderer& renderer){
    for(unsigned i=0;i<mp_options.seat_count;++i){auto& p=*pilots[i];if(p.ghost){auto& body=p.player->motion.body;const auto pos=p.player->motion.state.position;body.position={float(double(pos.x)+224),float(double(pos.y)+16),0};if(renderer.draw(body)==-2)return false;}else if(!p.player->draw(renderer))return false;}
    auto& local=*pilots[mp_options.local_seat];const auto p=local.player->motion.state.position;const float x=p.x+224,y=p.y+16;
    if(!local.ghost){renderer.pipeline().sourceBlend=touhou::graphics::BlendFactor::SourceAlpha;const u32 c=0xc0ffffff;
        renderer.solid_rectangle(x-7,y-7,x-3,y-6,c);renderer.solid_rectangle(x+3,y-7,x+7,y-6,c);renderer.solid_rectangle(x-7,y+6,x-3,y+7,c);renderer.solid_rectangle(x+3,y+6,x+7,y+7,c);
        if(mp_always_hitbox&&!local.player->motion.focus_animation){
            // The same native two-layer Focus marker, with only its visibility
            // changed. These copies never touch motion.focused or native VMs.
            if(auto* marker=mp_presentation.find(mp_focus_marker))for(auto* node=&marker->child;node;node=node->next){auto vm=*node->value;vm.position={x,y,0};if(renderer.draw(vm)==-2)return false;}
        }
    }
    for(unsigned i=0;i<mp_options.seat_count;++i){const auto& pilot=*pilots[i];if(!pilot.alive())continue;const auto p=pilot.player->motion.state.position;AsciiStyle style;style.font=2;style.pass=1;char text[8];
        if(pilot.life_hold&&mp_life_receiver(i)>=0){std::snprintf(text,sizeof(text),"%u%%",std::min(pilot.life_hold*100/90,100u));const float left=p.x+224-float(std::strlen(text))*3.5f;if(!ascii.add(text,{left,p.y-8,0},style))return false;continue;}
        if(pilot.power_taps<3)continue;AnmVm icon;icon.initialize();icon.resource=&resources.core.bullet;icon.flags=0x140003;icon.script_position={p.x+208,p.y-8,0};if(!icon.bind_sprite(0x159)||renderer.draw(icon)==-2)return false;std::snprintf(text,sizeof(text),"%u",pilot.power_taps);if(!ascii.add(text,{p.x+225,p.y-5,0},style))return false;
    }
    return true;
}
bool GameBattle::mp_draw_local_hud(AnmRenderer& renderer){
    const unsigned seat=mp_options.local_seat;const auto& e=pilots[seat]->economy;
    const auto& comm=mp_communication_icons[seat];
    // These stay at TH11's original playfield positions and draw priority.
    // Only the selected local seat is read; all-seat resource groups follow
    // later in the outer HUD pass.
    for(const auto& source:comm){auto vm=source;if(renderer.draw(vm)==-2)return false;}
    char buffer[96];AsciiStyle style;style.font=3;
    style.color=0xffffff|(hud.lives[0].color&0xff000000);
    std::snprintf(buffer,sizeof(buffer),"%d",e.graze);if(!ascii.add(buffer,{520,local_graze_y(mp_options.seat_count),0},style))return false;
    style.font=2;style.pass=1;style.color=0xffffff|(comm[0].color&0xff000000);
    const i32 percentage=e.communication<10000?signed_bits(u32(e.communication)*100)/10000:100;
    const i32 factor=wrapping_add(percentage,std::min(e.graze/100,899)),points=e.point_value/100;
    std::snprintf(buffer,sizeof(buffer),"%.6d*%1.2f",points-points%10,double(factor)/100);
    return ascii.add(buffer,{48,455,0},style);
}
bool GameBattle::mp_draw_hud(AnmRenderer& renderer){
    renderer.set_camera(compositor.full_camera,true);
    // Original-size life/Power groups, followed by an independent local row.
    constexpr float scale=1,tops[]={104,176,248};char buffer[96];
    for(unsigned seat=0;seat<mp_options.seat_count;++seat){const auto& pilot=*pilots[seat];const auto& e=pilot.economy;const float top=tops[seat];const u32 opacity=pilot.ghost?112:255;
        auto draw=[&](const AnmVm& source){auto vm=source;const auto p=Vec3{vm.position.x+vm.script_position.x+vm.child_position.x,vm.position.y+vm.script_position.y+vm.child_position.y,0};vm.position=vm.child_position={};vm.script_position={p.x,top+p.y-104,0};vm.color=(vm.color&0xffffff)|((vm.color>>24)*opacity/255<<24);return renderer.draw(vm)!=-2;};
        if(auto* frame=animations.find(hud.frame_animation))for(auto* node=frame->child.next;node;node=node->next)if(node->value->script_index==7||node->value->script_index==8){auto vm=*node->value;vm.flags|=2;if(!draw(vm))return false;}
        if(!mp_options.challenge)for(const auto& vm:mp_life_icons[seat])if(!draw(vm))return false;
        AsciiStyle style;style.font=3;style.scale={scale,scale};style.color=0xffffff|(((mp_life_icons[seat][0].color>>24)*opacity/255)<<24);
        auto put=[&](float x,float y){return ascii.add(buffer,{428+(x-428)*scale,top+(y-104)*scale,0},style);};
        if(mp_options.challenge){std::snprintf(buffer,sizeof(buffer),"%u",pilot.challenge_misses);if(!put(520,104))return false;}
        if(!e.power_step)return false;
        std::snprintf(buffer,sizeof(buffer),"%d.",e.power/e.power_step);if(!put(520,128))return false;style.scale={scale*.6f,scale*.6f};std::snprintf(buffer,sizeof(buffer),"%.2d",signed_bits(u32(e.power%e.power_step)*100)/e.power_step);if(!put(540,135))return false;
        style.scale={scale,scale};std::snprintf(buffer,sizeof(buffer),"/%d.",e.max_power/e.power_step);if(!put(554,128))return false;style.scale={scale*.6f,scale*.6f};std::snprintf(buffer,sizeof(buffer),"00");if(!put(586,135))return false;
        style.font=1;style.scale={1,1};style.color=opacity<<24|(seat==mp_options.local_seat?0xffff00:0xffffff);std::snprintf(buffer,sizeof(buffer),"%uP",seat+1);if(!ascii.add(buffer,{436,top-16,0},style))return false;
    }
    const float graze_y=local_graze_y(mp_options.seat_count);
    if(auto* frame=animations.find(hud.frame_animation)){
        float label_x=0;
        for(auto* node=frame->child.next;node;node=node->next)if(node->value->script_index==8){const auto& vm=*node->value;label_x=vm.position.x+vm.script_position.x+vm.child_position.x;}
        for(auto* node=frame->child.next;node;node=node->next)if(node->value->script_index==9){
            auto vm=*node->value;const float y=vm.position.y+vm.script_position.y+vm.child_position.y;
            vm.flags|=2;vm.position=vm.child_position={};vm.script_position={label_x,graze_y+y-152,0};
            if(renderer.draw(vm)==-2)return false;
        }
    }
    auto* boss=enemy_commands.bosses[0];if(boss&&!(boss->flags&0x21)&&renderer.draw(hud.indicator)==-2)return false;
    return true;
}
}
#endif
