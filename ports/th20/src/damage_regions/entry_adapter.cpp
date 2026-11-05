#include "damage.hpp"
#include "hit_callbacks.hpp"
#include "../bomb_system/bomb.hpp"
#include "../program_entry/program_entry.hpp"
#include <stdexcept>
namespace th20::source::damage {
namespace {
class GameEnvironment final:public Environment {
public:
    int bomb_damage(game_session::Context& context,const sprite::Vec3& p,const sprite::Vec2* size) override {
        return static_cast<bomb::Controller*>(context.objects_04[5])->event(reinterpret_cast<std::uint32_t>(&p),reinterpret_cast<std::uint32_t>(size));
    }
    int hit_callback(Region& region,const sprite::Vec3& p,const sprite::Vec2* size,float angle,float radius) override {
        switch(region.hit_callback){case 1:return shot_hit_callback(region,p,size,angle,radius,*program_entry::sprite_controller);case 2:return diminish_shot_hit(region,p);default:throw std::out_of_range("Original HitCtrlInf callback table index");}
    }
    void damage_reward(const sprite::Vec3& position,int damage) override{accumulate_damage_reward(game_session::overlay_owner(0),position,damage,13);}
};
}
Environment& environment(){static GameEnvironment value;return value;}
}
