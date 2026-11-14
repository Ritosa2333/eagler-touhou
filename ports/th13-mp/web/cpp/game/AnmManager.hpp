// ANM 管理器（原版全局管理器 DAT_004dc688 对应物）：资源装载/模板、创建、
// 中断与每帧 update。th13 差异（docs/plans/anm-pipeline.md §五）：
//   - create 语义对齐 0x46f820：render_flags|=0x200（活动位）；layer∈[0,21)
//     时 |=0x40000；flags bit0 置位时位置偏移 (+320,+16,0)（游戏坐标→ANM 坐标，
//     th13 游戏区 384×448）；anmArg 原样落 vm+0x50（=变量 10025/旋转 z 槽）；
//   - render_flags 稳态：port 模板/主链 0x4000（AnmManager.cpp adopt_bank
//     的 BUG 8 机制替换——原版 0x46e0b0 预热链与 0x46f820 create 后均无
//     0xc000 转换，模板/主链稳态 0x8000、不受 0x462450:308 暂停门约束；
//     0x4000 默认值本身来自 00405010.c:65，但该转换在原版只发生于
//     0x461cb0 类内嵌 VM）。副链插入清 0（0046f820.c:65,71 原版实清，
//     port 统一在 AnmRegistry::submit 执行）。模板预热与出生首拍临时置
//     0x8000（00461bf0.c:22），保证暂停中创建也能执行首拍；
//   - spawn 子 VM 继承父 layer（0046f990.c:20-22 / 0046fab0.c:20-22）；
//   - update 单次调用内先主链后副链（AnmRegistry::update）。
#pragma once
#include "AnmRegistry.hpp"
#include <functional>
#include <unordered_map>
namespace th13 {
// 诊断用只读生成观察器（默认 nullptr，仅浏览器探针臂接）：spawn_child 成功
// 挂树后回调 (父 VM, 子脚本号, 子句柄)。不改变任何状态；用于按帧对拍
// option 根/子体的 spawn 相位。
using AnmSpawnObserver = void (*)(const AnmVm&, i32, u32);
extern AnmSpawnObserver anm_spawn_observer;
// 资源模板与活动 VM 共用同一指令引擎；图形存储属可移植渲染器，不归本管理器。
class AnmManager final:public AnmObjectHost {
    struct Bank {AnmResource data;std::vector<AnmVm> templates;};
    std::unordered_map<i32,std::unique_ptr<Bank>> banks;
    std::vector<std::unique_ptr<Bank>> retired_banks;
    GameRandom& random;AnmEnvironment& environment;
    AnmVm* instantiate(AnmResource&,i32);
    // 0x403800 模板拷贝保留 IP/插值通道：神霊拾取（0x438730→0x46f820）等
    // 创建点必须从模板预跑停点续跑，不能从指令 0 重放。
    AnmVm* instantiate_preserve_ip(AnmResource&,i32);
    bool initial_tick(AnmVm&);
    bool adopt_bank(i32,std::unique_ptr<Bank>);
public:
    // 0x4418b0 / 0x45bf10 屏幕截取队列（ANM 管理器 +0x20，4 槽 × 0x28）：
    // 暂停激活时 0x43e3f0 把战场 (32,16,384,448) 点采样写进 text.anm
    // entry1 Blank 纹理的精灵矩形（128×128）；0x45bf10 在 Present 前消费。
    // port 由宿主在 Draw 末尾（SceneDisplay::before_present）执行。
    // ⚠ 不变量：resource 裸指针只在**同帧**入队→消费窗口有效（RunPause
    // 的 finish/rollback 均 clear_captures；bank 常驻不卸载）。跨帧持有
    // 或延迟消费前必须改存 bank id。
    struct CaptureRequest {
        const AnmResource* resource = nullptr;
        u32 texture = 0;
        i32 source[4] = {32, 16, 384, 448};  // 战场：x,y,w,h（逻辑像素）
        i32 destination[4] = {};             // 目标纹理精灵矩形 x,y,w,h
    };
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
    // 主链 0x46f170 遍历完成后的收尾钩子（update(false)/update() 成功后恰一次；
    // update(true) 副链不触发）。表现层用它读取本帧 tick 之后的 VM 状态：
    // 0x4464d0 的四角剔除在下一帧位姿写之前消费上一帧 post-tick 状态，钩子
    // 必须晚于链遍历、早于下一帧。返回 false = 收尾失败：update 置 error 并
    // 返回 false（由 Application 回调转成 FrameAction::Error），不得静默吞掉；
    // 钩子已写入的具体 error 优先保留，不被通用文案覆盖。
    std::function<bool()> after_main_update;
    // 主链收尾钩子的当前属主。换关时旧 RunScene 在新 RunScene 之后析构
    // （Application::begin_run 的 previous_run 局部变量），若无条件清空
    // 会抹掉新 StagePresentation 刚安装的钩子，导致自机弹快照/出界判定
    // 整段失效（S1→S2 后伤害注册池耗尽）。析构只清自己的钩子。
    const void* after_main_update_owner = nullptr;
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
    // 0x403800 模板拷贝只重置 time，不重置 IP；道具内嵌 VM 用此包装。
    bool bind_template_preserve_ip(AnmVm&,i32 id,i32 script);
    // game_coordinates：flags bit0 —— 位置从游戏坐标系平移 (+320,+16,0)。
    u32 create(i32 id,i32 script,i32 layer=-1,u32 ordering=2,const Vec3& translation={},float anm_arg=0,bool game_coordinates=false,u8 shot_effect_owner=0);
    // 0x46f820 同路径，但按 0x403800 原语义保留模板 IP 与插值通道相位
    //（只重置 VM 计时器）。神霊拾取 0x438730 创建点需要此语义：模板预跑
    // 已把 op412 插值推进一拍，重放指令 0 会丢掉该相位。
    u32 create_preserve_ip(i32 id,i32 script,i32 layer=-1,u32 ordering=2,const Vec3& translation={},float anm_arg=0,bool game_coordinates=false);
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
    // 0x4418b0：把 VM 当前精灵的纹理矩形登记为截取目标（4 槽，满时丢最旧）。
    bool queue_capture(const AnmVm& vm);
    std::vector<CaptureRequest> take_captures();
    void clear_captures() noexcept { captures_.clear(); }
    bool captures_pending() const noexcept { return !captures_.empty(); }
private:
    std::vector<CaptureRequest> captures_;
};
}
