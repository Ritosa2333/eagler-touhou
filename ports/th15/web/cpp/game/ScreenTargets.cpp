#include "ScreenTargets.hpp"
namespace th15 {
ScreenTargets::ScreenTargets(AnmManager& a,AnmEnvironment& e,ScreenViews& v,ScreenCompositor& c,i32 bank):animations(a),environment(e),views(v),compositor(c),text_bank(bank){for(u32 i=0;i<captures.size();i++)compositor.captures[i]=&captures[i];}
ScreenTargets::~ScreenTargets(){deactivate();for(u32 i=0;i<captures.size();i++){if(compositor.captures[i]==&captures[i])compositor.captures[i]=nullptr;animations.registry.destroy_tree(captures[i]);}}
bool ScreenTargets::fail(const char* value){if(error.empty())error=value;return false;}
void ScreenTargets::deactivate()noexcept{compositor.active=false;compositor.main={};compositor.alternate={};compositor.replacement={};}
bool ScreenTargets::prepare(){
 if(!error.empty())return false;if(compositor.active){views.view(DrawCamera::Playfield)=views.view(DrawCamera::Hud);return true;}
 const auto* file=animations.resource(text_bank);if(!file||file->textures.size()<4)return fail("Screen target textures unavailable");compositor.main={file,2};compositor.alternate={file,3};compositor.active=true;
 if(!(captures[0].visual.flags&1)){i32 variation=-1;switch(environment.screen_width){case 640:variation=0;break;case 960:variation=1;break;case 1280:variation=2;break;}
  if(variation>=0){constexpr i32 first[]={59,65,62,68};for(u32 i=0;i<captures.size();i++){auto& vm=captures[i];if(!animations.bind_template(vm,text_bank,first[i]+variation))return fail(animations.error.c_str());vm.creation_parent=vm.rotation_parent=nullptr;if(animations.tick_instance(vm)<0)return fail(animations.error.c_str());}}
 }
 if(environment.resolution_scale==1.5f)captures[2].visual.render_flags&=~0x800u;return true;
}
}
