#include "GameDraw.hpp"
namespace th17 {
GameDraw::GameDraw(StageGameplay& s,GameRun& state,GameRecords& data,SpellRecords& sp,AnmManager& a,AnmRenderer& r,ZunGraphics& g,ScreenViews& v,AsciiText& t,StageDrawServices& host):scene(s),run(state),records(data),spells(sp),animations(a),renderer(r),views(v),text(t),scheduler(s.frame_scheduler()),background(scheduler,s.background,s.background_draw,a,r,g,v,host){
    for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.index=i;c.frame.owner=&c;c.frame.enabled=true;c.frame.run=[](void* p)->i32{auto& c=*static_cast<Callback*>(p);return c.owner->pass(c.index)?1:5;};if(scheduler.add(c.frame,FramePass::Draw,priorities[i])<0)fail("Gameplay draw registration failed");}
}
GameDraw::~GameDraw(){for(auto& c:callbacks)scheduler.remove(c.frame);}
bool GameDraw::fail(const std::string& message){if(error.empty())error=message.empty()?"Game drawing failed":message;return false;}
bool GameDraw::popup_glyph(AnmVm& vm){return renderer.draw_glyph(vm)!=-2||fail(renderer.error);}
bool GameDraw::popup_text(const HudTextDraw& value){return text.hud_text(value)||fail(text.error);}
bool GameDraw::pass(u32 index){
    if(!error.empty())return false;auto& b=scene.battle;
    switch(index){
    case 0:return b.spell_card&&draw_spell_card(*b.spell_card,animations,spells,run.character,run.animal,run.scoring.mode_flags,text,error);
    case 1:return b.player&&b.player->draw(renderer)||fail(b.player?b.player->error:"Player draw owner unavailable");
    case 2:if(run.scene_flags&4)return true;return b.items&&b.items->items.draw(renderer)||fail(b.items?b.items->items.error:"Item draw owner unavailable");
    case 3:return b.laser_scene&&b.laser_scene->draw(renderer)||fail(b.laser_scene?b.laser_scene->error:"Laser draw owner unavailable");
    case 4:if(run.scene_flags&4)return true;return b.bullet_scene&&b.bullet_scene->draw(renderer)||fail(b.bullet_scene?b.bullet_scene->error:"Bullet draw owner unavailable");
    case 5:return b.bomb&&b.bomb->score_popup.draw(text,b.player->motion.position,background.rate,error);
    case 6:return b.player&&scene.popups.draw(*this,b.player->motion.position)||fail(scene.popups.error);
    case 7:{if(!b.hud||!b.hud->life_handles[0])return true;const auto* card=b.spell_card.get();HudDrawContext context{b.score,run.scoring,b.graze_total,bool(card),card?card->result_clock.displayed_frames:0,card?card->result_clock.encoded_time:0};return b.hud->draw(context,text)||fail(b.hud->error);}
    case 8:return !pause||(dates&&pause->draw(text,*dates,replay_header,captured))||fail(pause->error.empty()?"Pause calendar service unavailable":pause->error);
    default:return fail("Game draw pass outside schedule");
    }
}
bool GameDraw::draw(){
    if(!scene.ready())return fail("Game scene is not ready for drawing");background.rate=scene.battle.frame_state().rate;
    // Native shake writes the Playfield and Interface camera offsets. Keep the
    // final fullscreen/pause overlays still and let the ordinary layer schedule
    // apply these offsets, including the offscreen capture passes.
    auto& effects=scene.battle.screen_effects;
    effects.draw_offset(views.view(DrawCamera::Playfield).offset,views.view(DrawCamera::Interface).offset);
    const i32 result=scheduler.draw();effects.reset_offsets();
    if(result<0)return fail(background.error.empty()?error:background.error);renderer.flush();return background.error.empty()&&error.empty();
}
}
