#include "score.hpp"
#include "../item_system/rewards.hpp"
#include "../gameplay/loading_dependencies.hpp"
namespace th20::source::item::unrecovered {
void floating_score_00510710(void* owner,const sprite::Vec3& position,int value,std::uint32_t color){small_score::spawn(*static_cast<small_score::SmallScoreInf*>(owner),position,value,color);}
}
namespace th20::source::gameplay::unrecovered {
runtime::CallbackOwner* create_005108d0(int index){return small_score::create_controller(index);}
}
