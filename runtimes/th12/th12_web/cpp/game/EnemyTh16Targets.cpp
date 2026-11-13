#include "EnemyTh16Targets.hpp"
#include "EnemyState.hpp"
namespace th11 {
u32 enemy_th16_nearest(EnemyLink* list,Vec3 p,float radius)noexcept{
 // Retail425240: strict boundary and first encountered equal-distance target.
 u32 id=0;float best=float(radius*radius);
 for(auto* node=list;node;node=node->next){const auto& state=node->value->state;if(state.flags&0xc000021u)continue;
  const float dx=float(p.x-state.current.position.x),dy=float(p.y-state.current.position.y);
  const float distance=float(float(dy*dy)+float(dx*dx));if(best>distance){best=distance;id=u32(state.instance_id);}
 }return id;
}
bool enemy_th16_valid(EnemyLink* list,u32 id)noexcept{
 // Retail41a980 rejects zero independently of the list's contents.
 if(!id)return false;for(auto* node=list;node;node=node->next)if(u32(node->value->state.instance_id)==id)return true;return false;
}
Enemy* enemy_th16_lookup(EnemyLink* list,u32 id)noexcept{
 // Retail41b540 keeps the last visited enemy for an unmatched nonzero ID.
 if(!id)return nullptr;Enemy* last=nullptr;for(auto* node=list;node;node=node->next){last=node->value;if(u32(last->state.instance_id)==id)break;}return last;
}
}
