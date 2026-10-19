#include "LaserManager.hpp"
namespace th15 {
u32 LaserManager::insert(std::unique_ptr<LaserObject> object){if(!object){error="Laser instance unavailable";return 0;}if(objects.size()>=capacity)return 0;generation++;if(signed_bits(generation)<65536)generation=65536;object->id=generation;objects.push_back(std::move(object));return generation;}
LaserObject* LaserManager::find(u32 id)const noexcept{if(id)for(const auto& p:objects)if(p->id==id)return p.get();return nullptr;}
bool LaserManager::clear(){while(!objects.empty()){auto& object=*objects.front();if(!object.retire()){error=object.error;return false;}objects.pop_front();}return true;}
bool LaserManager::update(float rate,u32 game_flags){
    if(game_flags&0x405)return true;if(game_flags&2)rate=0;
    for(auto it=objects.begin();it!=objects.end();){auto next=std::next(it);auto& laser=**it;bool finished=false;
        if(laser.flags&6){const u32 advanced=(laser.flags&~1u)+2;laser.flags^=(advanced^laser.flags)&6;finished=(laser.flags&6)>=4;}
        if(!finished){if(laser.state==1)finished=true;else if(laser.flags&8){if(!laser.update_visual(rate,true)){error=laser.error;return false;}it=next;continue;}
            else if(!laser.update(rate,finished)){error=laser.error;return false;}}
        if(finished){if(!laser.retire()){error=laser.error;return false;}objects.erase(it);}else{laser.age.tick(&rate);laser.flags|=1;}it=next;
    }return true;
}
i32 LaserManager::cancel_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward){query_position=p;query_size={size.x,size.y,0};i32 total=0;for(auto it=objects.begin();it!=objects.end();){auto next=std::next(it);auto& object=**it;if(object.state!=1&&(object.flags&1)){const i32 result=object.cancel_rectangle(p,size,angle,reward,true);if(result<0&&!object.error.empty()){error=object.error;return -1;}total=wrapping_add(total,result);}it=next;}return total;}
i32 LaserManager::query_rectangle(const Vec3& p,const Vec2& size,float angle,i32 a,i32 b,i32 c){query_count=0;query_position=p;query_size={size.x,size.y,0};i32 total=0;for(auto it=objects.begin();it!=objects.end();){auto next=std::next(it);auto& object=**it;if(object.state!=1&&(object.flags&1)){const i32 result=object.query_rectangle(p,size,angle,a,b,c);if(result<0&&!object.error.empty()){error=object.error;return -1;}total=wrapping_add(total,result);}it=next;}return total;}
i32 LaserManager::cancel_circle(const Vec3& p,float radius,i32 reward,bool flag){query_position=p;i32 total=0;for(auto it=objects.begin();it!=objects.end();){auto next=std::next(it);auto& object=**it;if(object.state!=1){const i32 result=object.cancel_circle(p,radius,reward,flag);if(result<0&&!object.error.empty()){error=object.error;return -1;}total=wrapping_add(total,result);}it=next;}return total;}
bool LaserManager::cancel_all(i32 reward,bool flag){for(auto it=objects.begin();it!=objects.end();){auto next=std::next(it);auto& object=**it;if(object.state!=1&&!object.cancel_all(reward,flag)){error=object.error;return false;}it=next;}return true;}
bool LaserManager::clear_with_rewards(){for(auto it=objects.begin();it!=objects.end();){auto next=std::next(it);auto& object=**it;object.outside_delay.set(0);object.outside_count=0;if(!object.cancel_all(1,false)){error=object.error;return false;}it=next;}return true;}
i32 LaserManager::query_circle(const Vec3& p,float radius){i32 total=0;for(auto it=objects.begin();it!=objects.end();){auto next=std::next(it);auto& object=**it;if(object.state!=1){const i32 result=object.query_circle(p,radius);if(result<0&&!object.error.empty()){error=object.error;return -1;}total=wrapping_add(total,result);}it=next;}return total;}
}
