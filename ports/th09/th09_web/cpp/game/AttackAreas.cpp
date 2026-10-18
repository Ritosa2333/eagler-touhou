#include "AttackAreas.hpp"
#include "GameMath.hpp"
namespace th09 {
namespace {
bool contains(const AttackArea& area,const Vec3& p){
    const float dx=p.x-area.position.x,dy=p.y-area.position.y;
    if(area.radius!=0)return dx*dx+dy*dy<area.radius*area.radius;
    if(area.angle==0)return p.x>=area.position.x-area.half_extent.x&&p.x<=area.half_extent.x+area.position.x&&p.y>=area.position.y-area.half_extent.y&&p.y<=area.half_extent.y+area.position.y;
    Vec2 point;rotate(point,{dx,dy},-area.angle);return point.x>=-area.half_extent.x&&point.x<=area.half_extent.x&&point.y>=-area.half_extent.y&&point.y<=area.half_extent.y;
}
bool tick(AttackArea& a){
    if(a.delay>0){a.delay=wrapping_sub(a.delay,1);return false;}
    a.lifetime=wrapping_sub(a.lifetime,1);a.radius=a.radius_growth+a.radius;a.half_extent.x=a.extent_growth.x+a.half_extent.x;a.half_extent.y=a.extent_growth.y+a.half_extent.y;
    if(a.lifetime>0)return false;a.active=0;return true;
}
}
AttackAreas::AttackAreas(){for(u32 i=0;i<capacity;++i)free[i]=&pool[i];}
void AttackAreas::reset(){for(u32 i=0;i<capacity;++i){pool[i]=AttackArea{};free[i]=&pool[i];active[i]=nullptr;}count=0;free_count=capacity;}
AttackArea& AttackAreas::create(const Vec3& position,i32 lifetime,i32 delay){
    AttackArea* result=&pool[capacity];
    if(free_count>=2){result=free[--free_count];free[free_count]=nullptr;active[count++]=result;}
    *result=AttackArea{};result->active=1;result->position={position.x,position.y};result->lifetime=lifetime;result->delay=delay;return *result;
}
AttackArea& AttackAreas::circle(const Vec3& p,float radius,float growth,i32 damage,i32 lifetime,i32 delay,AttackAreaKind kind){auto& a=create(p,lifetime,delay);a.radius=radius;a.radius_growth=growth;a.damage=damage;a.kind=kind;return a;}
AttackArea& AttackAreas::box(const Vec3& p,float width,float height,i32 damage,i32 lifetime,i32 delay,AttackAreaKind kind){auto& a=create(p,lifetime,delay);a.half_extent={width*.5f,height*.5f};a.damage=damage;a.kind=kind;return a;}
void AttackAreas::update(){
    for(i32 i=0;i<count;){auto a=active[i];if(!tick(*a)){++i;continue;}
        free[free_count++]=a;u32 j=u32(i);do{active[j]=active[j+1];++j;}while(active[j-1]);--count;
    }
    auto& overflow=pool[capacity];if(!overflow.active){active[count]=nullptr;return;}
    if(overflow.delay>0){overflow.delay=wrapping_sub(overflow.delay,1);return;}
    if(!tick(overflow))active[count]=&overflow;
}
i32 AttackAreas::cancel(const Vec3& position,Bullet* bullet,AreaCollisionContext& context){
    for(auto a:active){if(!a)break;if((a->kind!=AttackAreaKind::cancel&&a->kind!=AttackAreaKind::reflect_damage&&a->kind!=AttackAreaKind::reflect)||a->delay>0||!contains(*a,position))continue;
        if(a->kind==AttackAreaKind::cancel){a->applied_damage=wrapping_add(a->applied_damage,1);return 2;}
        if(!bullet||bullet->sprite!=0)continue;
        const auto from=context.geometry[context.side].to_screen(position);Vec3 target;
        target.x=float(context.random.signed_unit())*(context.geometry[1-context.side].width*.5f-8.f);target.y=float(context.random.range(128));
        if(auto transfer=context.actions.create_transfer(context.side+1,from,target)){
            transfer->source_kind=bullet->sprite;transfer->target_kind=bullet->color;transfer->speed=bullet->speed;transfer->source_side=i16(context.transfer_kind);transfer->owner_flags=bullet->owner_flags;
        }
        context.actions.charge(.2f);context.actions.play_sound(46,context.side?500:-500);
        context.actions.combo(position,context.difficulty>2?3:2,2,0,context.combo_count<101?context.combo_count*10:1000);context.actions.add_score(50);
        a->applied_damage=wrapping_add(a->applied_damage,1);return 2;
    }return 0;
}
i32 AttackAreas::probe(const Vec3& p,const Vec3& e,const Bounds& graze,Bullet* b,AreaCollisionContext& context){
    if(cancel(p,b,context))return 2;
    return graze.minimum.x<=e.x*.5f+p.x+60.f&&(p.x-e.x*.5f)-60.f<=graze.maximum.x&&graze.minimum.y<=e.y*.5f+p.y+60.f&&(p.y-e.y*.5f)-60.f<=graze.maximum.y?1:0;
}
i32 AttackAreas::damage(const Vec3& p,const Vec3& e,i32& special){
    i32 total=0;const Vec2 lo{p.x-e.x*.5f,p.y-e.y*.5f},hi{e.x*.5f+p.x,e.y*.5f+p.y};
    for(auto a:active){if(!a)break;if((a->kind!=AttackAreaKind::damage&&a->kind!=AttackAreaKind::reflect_damage&&a->kind!=AttackAreaKind::special_damage)||a->delay>0||!a->interval||a->lifetime%a->interval)continue;
        bool hit;
        if(a->radius!=0){const float dx=a->position.x-p.x,dy=a->position.y-p.y;hit=dy*dy+dx*dx<=a->radius*a->radius;}
        else if(a->angle==0)hit=a->position.x-a->half_extent.x<=hi.x&&lo.x<=a->half_extent.x+a->position.x&&a->position.y-a->half_extent.y<=hi.y&&lo.y<=a->half_extent.y+a->position.y;
        else {Vec2 point;rotate(point,{p.x-a->position.x,p.y-a->position.y},-a->angle);hit=-a->half_extent.x<=e.x*.5f+point.x&&point.x-e.x*.5f<=a->half_extent.x&&-a->half_extent.y<=e.y*.5f+point.y&&point.y-e.y*.5f<=a->half_extent.y;}
        if(!hit)continue;total=wrapping_add(total,a->damage);a->applied_damage=wrapping_add(a->applied_damage,a->damage);
        if(a->damage_limit>0&&a->damage_limit<=a->applied_damage){a->damage=0;total=wrapping_add(total,wrapping_sub(a->damage_limit,a->applied_damage));}
        if(a->kind==AttackAreaKind::special_damage)special=wrapping_add(special,a->damage);
    }return total;
}
}
