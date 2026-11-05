#include "PlayerItems.hpp"
namespace th09 {
void PlayerItems::spawn(i32 type,const Vec3& p,bool locked){
    if(locked)return;for(auto& item:items){if(item.active)continue;item.type=type;item.position=p;item.velocity={0,-1.5f,0};item.active=1;actions.start_animation(item.animation,type+70);return;}
}
void PlayerItems::update(const ItemContext& c){
    attraction.x=-1000;u32 slot=0;i32 counter=0;
    while(counter<4&&slot<4){auto& item=items[slot++];
        if(item.active){item.position={item.velocity.x+item.position.x,item.velocity.y+item.position.y,item.velocity.z+item.position.z};
            if(item.position.y>=464)item.active=0;
            else {
                if(item.velocity.y>=3)item.velocity.y=3;else item.velocity.y+=.03f;
                if(!collects_item(c.state,c.pickup_bounds,{item.position.x,item.position.y},{c.pickup_size,c.pickup_size})){
                    attraction.x=item.position.x;attraction.y=400.f-(400.f-item.position.y)*.5f;
                }else{
                    if(item.type==0)actions.charge(400);
                    else if(item.type==1){
                        // The original reuses the outer-loop counter here, so
                        // collecting this item can defer later slots a frame.
                        counter=0;const i32 total=c.rank+10+c.difficulty*2;
                        while(counter<total){const auto origin=c.geometry[c.side].to_screen(c.player);Vec3 from,target;
                            from.x=float(random.signed_unit())*64.f+origin.x;from.y=float(random.signed_unit())*64.f+origin.y;
                            target.x=float(random.signed_unit())*(c.geometry[1-c.side].width*.5f-8.f);target.y=float(random.range(128));
                            if(auto result=actions.create_transfer(c.side+1,from,target,float(counter)+float(counter))){result->source_kind=3;result->target_kind=4;
                                switch(c.difficulty){case 0:result->speed=float(c.rank)*.04f+.9f;break;case 1:result->speed=float(c.rank)*.05f+1.1f;break;case 2:result->speed=float(c.rank)*.08f+1.3f;break;case 3:result->speed=float(c.rank)*.1f+1.7f;break;case 4:result->speed=float(c.rank)*.08f+1.5f;break;}
                                result->source_side=i16(c.side);
                            }++counter;
                        }
                    }else if(item.type==2)actions.combo(c.player,1,0,400,0);
                    else if(item.type==3)actions.combo(c.player,0,0,0,70000);
                    item.active=0;actions.play_sound(31,c.side?500:-500);
                }
            }
        }++counter;
    }
}
void PlayerItems::draw(const PlayfieldGeometry& g){for(auto& item:items)if(item.active){item.animation.pos=g.to_screen(item.position);item.animation.pos.z=.15f;actions.draw_animation(item.animation);}}
}
