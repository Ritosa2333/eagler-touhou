#include "OwnedObjects.hpp"
namespace th075 {
FighterObject* OwnedObjects::spawn(const ObjectRequest& request){
    if(request.layer>=layers.size()||!owner.animation.pattern)return nullptr;
    auto p=factory.create_object();if(!p)return nullptr;p->animation.pattern=owner.animation.pattern;p->action(request.action);
    p->motion.x=request.x;p->motion.y=request.y;p->motion.facing=request.facing;p->layer=request.layer;
    p->order=request.alternateDraw;p->group=request.shadow;p->pose.active=false;p->parameters=request.parameters;
    p->contactDelay=request.contactDelay;p->bodyPower=p->sourcePower=1;p->sourceModifier=owner.damage.sourceModifier;
    if(request.visual){const auto& v=*request.visual;p->begin_pose();p->pose.flags=v.mode;p->pose.color=(u32(v.alpha)<<24)|(u32(v.red)<<16)|(u32(v.green)<<8)|v.blue;p->pose.projection=v.projection;p->pose.scale=v.scale;p->pose.rotation=v.rotation;}
    FighterObject* result=p.get();layers[request.layer].push_back(std::move(p));return result;
}
void OwnedObjects::advance(FighterObjectUpdates& callback){
    for(auto& list:layers){
        // Only the initial count of this layer advances. Appending to a later
        // layer can still advance that object in the same simulation frame.
        const u32 initialCount=u32(list.size());for(u32 i=0;i<initialCount;i++)callback.update(*list[i]);
        for(auto it=list.begin();it!=list.end();)if(!(*it)->alive)it=list.erase(it);else ++it;
    }
}
void OwnedObjects::render(u8 layer,FighterObjectCallbacks& callback){if(layer>=layers.size())return;for(auto& p:layers[layer])callback.draw(*p,p->order!=0);}
void OwnedObjects::render_shadows(FighterObjectCallbacks& callback){for(auto& list:layers)for(auto& p:list)if(p->group!=0)callback.shadow(*p);}
}
