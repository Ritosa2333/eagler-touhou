#pragma once
#include "FighterObject.hpp"
#include <deque>
#include <memory>
namespace th075 {
struct ObjectRequest {
    i16 action=0;float x=0,y=0;i8 facing=1;u8 layer=1,alternateDraw=0,shadow=0,contactDelay=0;
    std::array<float,4> parameters{};const FrameVisual* visual=nullptr;
};
struct FighterObjectFactory {virtual ~FighterObjectFactory()=default;virtual std::unique_ptr<FighterObject> create_object()=0;};
struct FighterObjectUpdates {virtual ~FighterObjectUpdates()=default;virtual void update(FighterObject&)=0;};
struct FighterObjectCallbacks:FighterObjectUpdates {
    virtual ~FighterObjectCallbacks()=default;
    virtual void draw(FighterObject&,bool alternate)=0;virtual void shadow(FighterObject&)=0;
};
class OwnedObjects {
    FighterActor& owner;FighterObjectFactory& factory;
    std::array<std::deque<std::unique_ptr<FighterObject>>,4> layers;
public:
    OwnedObjects(FighterActor& owner,FighterObjectFactory& factory):owner(owner),factory(factory){}
    FighterObject* spawn(const ObjectRequest&);
    void clear(){for(auto& list:layers)list.clear();}
    void advance(FighterObjectUpdates&);
    void render(u8 layer,FighterObjectCallbacks&);
    void render_shadows(FighterObjectCallbacks&);
    u32 size(u8 layer)const{return layer<layers.size()?u32(layers[layer].size()):0;}
    FighterObject& get(u8 layer,u32 index){return *layers.at(layer).at(index);}
    template<class Callback>void each(Callback&& callback){for(auto& list:layers)for(auto& p:list)callback(*p);}
};
}
