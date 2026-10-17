#pragma once
#include "EnemyState.hpp"
#include "AnmVm.hpp"
#include "EclContext.hpp"
namespace th15 {
struct EnemyVisualHost:EnemyAnimationHost {
    virtual AnmVm* find(u32 handle)=0;
    virtual bool create(u32& handle,i32 resource,i32 script,i32 layer)=0;
    virtual bool retire(u32& handle)=0;
    virtual bool pause(u32 handle)=0;
    virtual bool interrupt(u32 handle,i32 label)=0;
    virtual bool rebind(u32& handle,i32 script)=0;
    virtual const std::string* failure()const noexcept{return nullptr;}
};
class EnemyVisualCommands {
    EnemyState& enemy;
public:
    EnemyVisualHost* host=nullptr;
    explicit EnemyVisualCommands(EnemyState& enemy):enemy(enemy){}
    int execute(EclContext&,u16 opcode);
};
}
