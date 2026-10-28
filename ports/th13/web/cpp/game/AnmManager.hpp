// ANM 管理器（原版全局管理器 DAT_004dc688 对应物）：资源装载/模板、创建、
// 中断与每帧 update。th13 差异（docs/plans/anm-pipeline.md §五）：
//   - create 语义对齐 0x46f820：render_flags|=0x200（活动位）；layer∈[0,21)
//     时 |=0x40000；flags bit0 置位时位置偏移 (+320,+16,0)（游戏坐标→ANM 坐标，
//     th13 游戏区 384×448）；anmArg 原样落 vm+0x50（=变量 10025/旋转 z 槽）；
//   - spawn 子 VM 继承父 layer（0046f990.c:20-22 / 0046fab0.c:20-22）；
//   - update 单次调用内先主链后副链（AnmRegistry::update）。
#pragma once
#include "AnmRegistry.hpp"
#include <functional>
#include <unordered_map>
namespace th13 {
// 资源模板与活动 VM 共用同一指令引擎；图形存储属可移植渲染器，不归本管理器。
class AnmManager final:public AnmObjectHost {
    struct Bank {AnmResource data;std::vector<AnmVm> templates;};
    std::unordered_map<i32,std::unique_ptr<Bank>> banks;
    std::vector<std::unique_ptr<Bank>> retired_banks;
    GameRandom& random;AnmEnvironment& environment;
    AnmVm* instantiate(AnmResource&,i32);
    bool initial_tick(AnmVm&);
    bool adopt_bank(i32,std::unique_ptr<Bank>);
public:
    AnmRegistry registry;std::string error;float rate=1;
    // [f371 标题重挂] 入场重挂需公开实例化/提交面（oracle 批序尾插语义）。
    using AnmObjectHost::spawn_child;
    AnmVm* create_instance(AnmResource& resource,i32 script){return instantiate(resource,script);}
    bool prime_instance(AnmVm& vm){return initial_tick(vm);}
    // 0x472c90 游戏侧 RNG 源（原版全局态 0x4dc31c；表现层/装载模板共享）。
    GameRandom& random_source()const noexcept{return random;}
    std::unordered_map<i32,std::string> resource_names;
    bool name_resource(i32 id,const std::string& name){if(!resource(id)||name.empty()||name.size()>1024)return false;resource_names[id]=name;return true;}
    std::function<bool(AnmVm&,i32)> effect;
    std::function<i32(AnmVm&,float)> effect_update;
    std::function<bool(AnmVm&,i32,float)> effect_interrupt;
    AnmManager(GameRandom& random,AnmEnvironment& environment):random(random),environment(environment){}
    AnmEnvironment& scene_environment()noexcept{return environment;}
    void set_paused(bool paused)noexcept{environment.paused=paused;}
    bool load(i32 id,const u8*,u32 size);
    // 收编已打开的资源（多文件合并的全局脚本表，见 AnmResource::append）。
    bool adopt(i32 id,AnmResource&& data);
    void unload(i32 id);void collect_resources();
    u32 resource_count()const noexcept{return banks.size();}
    u32 retired_resource_count()const noexcept{return retired_banks.size();}
    std::function<void(const AnmResource&)> resource_release;
    AnmResource* resource(i32 id)const noexcept;
    i32 resource_id(const AnmResource* value)const noexcept;
    bool sprite_fallback(i32 id)noexcept{auto* bank=resource(id);if(!bank)return false;environment.fallback_sprite_resource=bank;return true;}
    AnmVm* script_template(i32 id,u32 script)const noexcept;
    bool bind_template(AnmVm&,i32 id,i32 script);
    // game_coordinates：flags bit0 —— 位置从游戏坐标系平移 (+320,+16,0)。
    u32 create(i32 id,i32 script,i32 layer=-1,u32 ordering=2,const Vec3& translation={},float anm_arg=0,bool game_coordinates=false);
    // 副链尾创建（th15 调用面保留；0xc000 清除已并入 Registry::submit）。
    u32 create_overlay(i32 resource,i32 script){return create(resource,script,-1,4);}
    AnmVm* spawn_child(AnmVm&,i32 script,u32 ordering)override;
    AnmVm* spawn_detached(AnmVm&,i32 script)override;
    bool spawn_effect(AnmVm&,i32 script)override;
    bool release_tree(AnmVm& vm)override{return registry.destroy_tree(vm);}
    i32 update_effect(AnmVm&,float rate)override;
    bool interrupt_effect(AnmVm&,i32 label,float rate)override;
    bool rebind(u32& handle,i32 script);
    bool retire(u32& handle);
    void retire_resource(i32 id)noexcept{if(auto* bank=resource(id))registry.retire_resource(bank);}
    bool pause(u32 handle,bool paused=true);
    bool interrupt(u32 handle,i32 label,bool immediate=false);
    bool update();                 // th13 单帧：主链+副链两遍（0x46f170+0x46f240）
    bool update(bool alternate);   // 单链一遍（th15 调用面兼容：false=主链 true=副链）
    int tick_instance(AnmVm&);
};
}
