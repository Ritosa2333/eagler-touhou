#include "SharedEffects.hpp"
namespace th075 {
void SharedEffect::bind(){image=nullptr;cursor.duration=cursor.frameCount=0;if(!pattern||cursor.action<0||u32(cursor.action)>=pattern->sequences.size()){alive=false;return;}const auto& s=pattern->sequences[cursor.action];cursor.frameCount=i16(s.frames.size());if(s.present&&cursor.index>=0&&u32(cursor.index)<s.frames.size()){image=&s.frames[cursor.index];cursor.duration=i16(image->duration);}alive=image!=nullptr;}
void SharedEffect::rebound(void* owner,AnimationCursor&){static_cast<SharedEffect*>(owner)->bind();}
void SharedEffect::select(i16 action,i16 index,i16 elapsed){cursor.set(action,index,elapsed,rebound,this);}
i16 SharedEffect::advance(){return cursor.advance(rebound,this);}
void SharedEffects::spawn(i16 action,float x,float y,i8 facing,u8 layer,const std::array<float,4>& parameters){SharedEffectRequest request;request.action=action;request.x=x;request.y=y;request.facing=facing;request.layer=layer;request.parameters=parameters;spawn(request);}
SharedEffect* SharedEffects::spawn(const SharedEffectRequest& request){
    if(request.action<=0||u32(request.action)>=pattern.sequences.size()||!pattern.sequences[request.action].present||request.layer>=layers.size())return nullptr;
    auto effect=std::make_unique<SharedEffect>();effect->pattern=&pattern;effect->select(request.action);
    effect->motion.x=request.x;effect->motion.y=request.y;effect->motion.facing=request.facing;effect->parameters=request.parameters;
    effect->alternateDraw=request.alternateDraw;effect->shadow=request.shadow;
    auto* live=effect.get();layers[request.layer].push_back(std::move(effect));
    // Unlike character-owned objects, shared particles receive their first
    // update immediately. Children use this same path, including RNG order.
    advance_shared_effect(*live,random,*this);return live;
}
void SharedEffects::advance(){for(auto& list:layers){const u32 n=u32(list.size());for(u32 i=0;i<n;i++)advance_shared_effect(*list[i],random,*this);for(auto it=list.begin();it!=list.end();)if(!(*it)->alive)it=list.erase(it);else ++it;}}
}
