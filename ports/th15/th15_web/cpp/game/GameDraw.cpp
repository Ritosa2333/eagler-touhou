#include "GameDraw.hpp"
namespace th15 {
GameDraw::GameDraw(StageGameplay& s,SessionState& p,RecordStore& v,AnmManager& a,AnmRenderer& r,ZunGraphics& g,ScreenViews& views,AsciiText& t,StageDrawServices& h,ReplayCalendarServices& d):scene(s),progress(p),records(v),animations(a),renderer(r),text(t),dates(d),scheduler(s.frame_scheduler()),background(scheduler,s.background,a,r,g,views,h){
 for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.index=i;c.frame.owner=&c;c.frame.enabled=true;c.frame.run=[](void* p)->i32{auto& c=*static_cast<Callback*>(p);return c.owner->run(c.index)?1:5;};if(scheduler.add(c.frame,FramePass::Draw,priorities[i])<0)fail("Gameplay draw registration failed");}
}
GameDraw::~GameDraw(){for(auto& c:callbacks)scheduler.remove(c.frame);}
bool GameDraw::fail(const std::string& value){if(error.empty())error=value.empty()?"Gameplay drawing failed":value;return false;}
bool GameDraw::draw_glyph(AnmVm& vm){return renderer.draw_glyph(vm)!=-2||fail(renderer.error);}
bool GameDraw::draw_text(const Vec3& position,u32 color,const std::string& value){HudTextStyle style;style.font=2;style.coordinate_space=2;style.alignment=style.vertical_alignment=0;style.color=color;return text.hud_text({position,style,value})||fail(text.error);}
bool GameDraw::run(u32 index){
 if(!error.empty())return false;auto& b=scene.battle;
 switch(index){
 case 0:prepare_frame();return draw_spell_card(b.spell_card,animations,records,spell_context,text,error);
 case 1:return b.player&&b.player->draw(renderer)||fail(b.player?b.player->error:"Player drawing unavailable");
 case 2:if(progress.scene_flags&4)return true;return b.items&&b.items->draw(renderer)||fail(b.items?b.items->error:"Item drawing unavailable");
 case 3:return b.laser_scene&&b.laser_scene->draw(renderer)||fail(b.laser_scene?b.laser_scene->error:"Laser drawing unavailable");
 case 4:if(progress.scene_flags&4)return true;return b.bullet_scene&&b.bullet_scene->draw(renderer)||fail(b.bullet_scene?b.bullet_scene->error:"Bullet drawing unavailable");
 case 5:return b.player&&scene.popups.draw(*this,b.player->motion.position)||fail(scene.popups.error);
 case 6:{if(!scene.hud.root)return true;HudDrawContext context;context.frame={&b.enemy_world,b.player?&b.player->motion.position:nullptr,b.spell.flags,scene.messages.active()};context.spell_available=true;context.spell_frames=b.spell_card.clock.completed_frames;context.spell_best_time=b.spell_card.clock.encoded;return scene.hud.draw(context,text)||fail(scene.hud.error);}
 case 7:return !pause||pause->visuals.draw(text,dates,replay_header,captured)||fail(pause->visuals.error);
 default:return fail("Gameplay draw pass outside schedule");
 }
}
}
