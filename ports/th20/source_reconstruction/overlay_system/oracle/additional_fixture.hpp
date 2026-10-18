#include "../weapon_cancellation.hpp"
#include "../../player_entity/firing_at_position.hpp"
namespace bs=th20::source::bullet;
std::vector<unsigned> extra_trace;
void record_shot(unsigned packed,int frame,const th20::source::sprite::Vec3& p,pe::Option* attachment){extra_trace.insert(extra_trace.end(),{packed,unsigned(frame),fbits(p.x),fbits(p.y),fbits(p.z),reinterpret_cast<unsigned>(attachment)});}
int __fastcall create_shot_boundary(void*,void*,unsigned packed,int frame,const th20::source::sprite::Vec3* p,pe::Option* attachment){record_shot(packed,frame,*p,attachment);return -1;}
pe::Option* __fastcall option_boundary(pe::Player* p,void*,int index){return &p->options[index];}
void cancel_one(bs::Bullet& b,int mode){extra_trace.insert(extra_trace.end(),{b.index,unsigned(mode),b.flags});b.state=4;}
int __fastcall cancel_one_boundary(bs::Bullet* b,void*,int mode){cancel_one(*b,mode);return 0;}
namespace th20::source::bullet {int cancel(Bullet& b,int mode){cancel_one(b,mode);return 0;}}
namespace th20::source::player_entity {
Option& shot_option(Player& p,int index) noexcept{return p.options[index];}
int create_shot(ShotController&,unsigned packed,int frame,const sprite::Vec3& p,Option* attachment,FiringServices&){record_shot(packed,frame,p,attachment);return -1;}
}
struct FiringBoundaryHost final:pe::FiringServices {
 gs::Session& session()override{throw std::logic_error("Unexpected FiringServices call");}float clock_rate()override{throw std::logic_error("Unexpected FiringServices call");}float signed_random()override{throw std::logic_error("Unexpected FiringServices call");}pe::Shot* create_heap_shot()override{throw std::logic_error("Unexpected FiringServices call");}pe::ShotCallbacks callbacks(const pe::ShotRecord&)override{throw std::logic_error("Unexpected FiringServices call");}unsigned spawn_animation(th20::source::sprite::AnimationFile&,int)override{throw std::logic_error("Unexpected FiringServices call");}th20::source::sprite::Animation& animation(unsigned&)override{throw std::logic_error("Unexpected FiringServices call");}unsigned create_damage(gs::Context&,const pe::Shot&)override{throw std::logic_error("Unexpected FiringServices call");}th20::source::damage::Region* damage(unsigned&)override{throw std::logic_error("Unexpected FiringServices call");}void retire(pe::Shot&)override{throw std::logic_error("Unexpected FiringServices call");}void sound_at(int,float)override{throw std::logic_error("Unexpected FiringServices call");}
};
