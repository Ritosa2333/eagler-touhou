#include "EnemyCommands.hpp"
#include "BulletState.hpp"
#include "AnmVm.hpp"
namespace th11 {
#if PORT_TITLE == 14
// 42cc70 rewrites only five words in ETEX slot 1, in active-list order.
// Other parameters and the live transform cursor/active mask are preserved.
void enemy_th14_random_acceleration(BulletState* first,Rng& random)noexcept{
    for(auto* b=first;b;){auto* next=b->th14_active_next;auto& t=b->th14_transforms[1];
        t.a=random.signed_unit()*.009999999776482582f;t.b=-99999.f;t.p=60;t.opcode=4;t.concurrent=0;
        b=next;
    }
}
void enemy_th14_visual_interrupt(const EnemyState& e,AnmVm& vm)noexcept{vm.interrupt(wrapping_add(e.integers[0],7));}
void enemy_th14_closest_bullet(EnemyState& e,const BulletState* first)noexcept{
    const BulletState* closest=nullptr;float distance=e.floats[3]*e.floats[3];
    for(auto* b=first;b;b=b->th14_active_next)if(b->th14_script_state==1){
        const float dy=b->position.y-e.current.position.y,dx=b->position.x-e.current.position.x;
        const float squared=dx*dx+dy*dy;
        if(distance>squared){distance=squared;closest=b;}
    }
    e.integers[0]=closest?1:0;
    if(closest){e.floats[0]=closest->position.x;e.floats[1]=closest->position.y;}
}
void enemy_th14_freeze_bullets(const EnemyState& e,BulletState* first,Vec3 player,BulletEnvironment& environment)noexcept{
    // 42cb00 uses two squared radii and traverses the active list, saving next.
    const float inner=e.floats[0]*e.floats[0],outer=e.floats[1]*e.floats[1];
    for(auto* b=first;b;){auto* next=b->th14_active_next;
        if(b->th14_script_state==1||b->th14_script_state==2){
            const float dx=b->position.x-player.x,dy=b->position.y-player.y;
            const float distance=dx*dx+dy*dy;
            if(b->th14_script_state==1){
                if(inner>distance){b->th14_script_state=2;b->transform_index=5;b->active_transforms=0;
                    environment.sound(40,b->position.x,true);b->reserved_4c0=0;}
            }else{
                b->reserved_4c0=wrapping_add(b->reserved_4c0,1);
                if(distance>=outer){b->th14_script_state=1;b->transform_index=14;b->active_transforms=0;}
            }
        }b=next;
    }
}
#endif
}
