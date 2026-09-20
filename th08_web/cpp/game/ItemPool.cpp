#include "ItemPool.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
namespace th08 {
ItemState* ItemPool::spawn(const Vec3& position,i32 type,i32 mode,i32 power,i8 player_state){
    auto* item=&state.items[state.next_index];auto* overflow=&state.items[ItemPoolState::capacity];
    if(position.x<-64||position.x>448)return overflow;
    if(power>=128&&(type==0||type==2))type=8;if(type==7)mode=3;else if(type==10){mode=5;type=7;}
    for(u32 i=0;i<ItemPoolState::capacity;++i){
        ++state.next_index;
        if(item->active){if(state.next_index>=i32(ItemPoolState::capacity)){state.next_index=0;item=state.items;}else ++item;if(type==7)return overflow;continue;}
        if(state.next_index>=i32(ItemPoolState::capacity))state.next_index=0;
        item->active=1;item->position=position;item->velocity={0,-2.2f,0};item->type=i8(type);item->state=i8(mode);item->timer.set(0);
        if(mode==2){item->target={(rng.range(288)+number(48)).to_float(),(rng.range(192)-number(64)).to_float(),0};item->velocity=position;}
        else if(mode==3||mode==5){item->velocity.y=(number(-2)-rng.range(.2f)).to_float();item->velocity.x=rng.signed_range(.6f).to_float();
            if(player_state==2){item->state=0;item->velocity={0,-.9f,0};}}
        actions.animation(item->animation,wrapping_add(type,61));item->animation.color1.d3dColor=-1;item->animation.zWriteDisabled=true;item->max_value=0;item->onscreen=1;
        state.tail->next=item;item->previous=state.tail;item->next=nullptr;state.tail=item;return item;
    }return overflow;
}
void ItemPool::remove(ItemState& item){item.active=0;item.previous->next=item.next;if(item.next)item.next->previous=item.previous;if(state.tail==&item)state.tail=item.previous;}
void ItemPool::collect_all(){for(auto* p=state.head.next;p;p=p->next){p->state=1;p->velocity={0,-.5f,0};}}
void ItemPool::cancel_homing(){for(auto* p=state.head.next;p;p=p->next)if(p->state==1){p->state=0;p->velocity={0,-.9f,0};}}
void ItemPool::convert_power(ItemState* except){
    for(auto* p=state.head.next;p;p=p->next)if(p!=except&&(p->type==0||p->type==2)){
        if(p->velocity.y>-.5f)p->velocity={0,-.5f,0};actions.effect(0,p->position,1,0xffffffff);p->type=8;actions.animation(p->animation,69);
    }
}
i32 ItemPool::time_orb_count()const{ i32 count=0;for(auto* p=state.head.next;p;p=p->next)if(p->type==7)count=wrapping_add(count,1);return count;}
void point_item_extend_threshold(GameGlobals& values,i32 difficulty)noexcept{
    static constexpr i32 normal[]{100,250,500,800,1100,9999},extra[]{200,666,9999};const u32 n=values.point_extends;
    values.next_point_extend=difficulty<4?(n<6?normal[n]:signed_bits((n-5)*500u+9999u)):(n<3?extra[n]:99999);
}
void ItemPool::snapshot(){if(!presentation_marker.capture())return;for(size_t i=0;i<previous.size();++i){const auto& item=state.items[i];auto& p=previous[i];p.active=item.active!=0;if(p.active){p.position=item.position;p.age=item.timer.current;p.type=item.type;p.visual.capture(item.animation);}}}
void ItemPool::draw(const Vec2& offset){
    for(auto* p=state.head.next;p;p=p->next){auto& source=p->animation;
        TH08_AUDIT_SCOPE(Item,p,source.currentTimeInScript.current,u32(p->type));
        if(!presentation::render_only){if(p->position.y<-8){if(p->onscreen){actions.sprite(source,wrapping_add(p->type,182));p->onscreen=0;source.zWriteDisabled=true;}}else if(!p->onscreen){actions.sprite(source,wrapping_add(p->type,172));p->onscreen=1;source.color1.d3dColor=-1;source.zWriteDisabled=true;}}
        if(!presentation::render_only){source.pos={Scalar::add(offset.x,p->position.x),Scalar::add(offset.y,p->position.y),.15f};if(p->position.y<-8){source.pos.y=Scalar::add(8,offset.y);i32 alpha=wrapping_sub(255,((number(8)-number(p->position.y))*number(255)/number(128)).truncate_int());if(alpha<64)alpha=64;source.color1.d3dColor=signed_bits((u32(source.color1.d3dColor)&0xffffffu)|(u32(alpha)<<24));}}
        Vec3 draw_position=p->position;if(presentation::active){const size_t index=size_t(p-state.items);const auto& before=previous[index];const float dx=p->position.x-before.position.x,dy=p->position.y-before.position.y;if(before.active&&before.type==p->type&&p->timer.current>=before.age&&dx*dx+dy*dy<16384.0f)draw_position={presentation::lerp_world(before.position.x,p->position.x),presentation::lerp_world(before.position.y,p->position.y),presentation::lerp_world(before.position.z,p->position.z)};}
        AnmVm copy;AnmVm* vm=&source;if(presentation::render_only){copy=source;const auto& before=previous[size_t(p-state.items)];if(before.active&&before.type==p->type&&p->timer.current>=before.age)before.visual.apply(source,copy,presentation::world_alpha);vm=&copy;}vm->pos={Scalar::add(offset.x,draw_position.x),Scalar::add(offset.y,draw_position.y),.15f};
        if(draw_position.y<-8){
            vm->pos.y=Scalar::add(8,offset.y);i32 alpha=wrapping_sub(255,((number(8)-number(draw_position.y))*number(255)/number(128)).truncate_int());if(alpha<64)alpha=64;vm->color1.d3dColor=signed_bits((u32(vm->color1.d3dColor)&0xffffffu)|(u32(alpha)<<24));
        }
        actions.draw(*vm);
    }
}
}
