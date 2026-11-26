// EX模式13..15；公共演出和输入复用Stage6Dialogue，尾声实体和掉落保留原顺序。
#pragma once
#include "core/stages/Stage6Dialogue.hpp"
#include "core/stages/ExDialogueConstants.hpp"
#include "core/player/ItemPool.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/Random.hpp"
namespace kaikeidou::ex_dialogue_detail {
inline std::uint64_t quiet(std::uint64_t bits){if((bits&0x7fffffffffffffffull)>0x7ff0000000000000ull)bits|=0x8000000000000ull;return bits;}
inline Enemy& enemy_at(EnemyPool& enemies,Enemy& prefix,std::uint32_t slot){return slot==0xffffffffu?prefix:enemies.enemies.at(slot);}
inline void write(DialogueState& state,const Write& value){
    if(value.address>=0xa73058&&value.address<0xa73118){if(value.width==4)state.fields.word(value.address-0xa73058,static_cast<std::uint32_t>(value.value));else state.fields.quad(value.address-0xa73058,value.value);}
    else if(value.address==0xa7e7fc)state.word_a7e7fc=static_cast<std::uint32_t>(value.value);
    else if(value.address==0xa78e30)state.word_a78e30=static_cast<std::uint32_t>(value.value);
    else throw std::out_of_range("EX dialogue write address");
}
inline std::uint64_t projectile_position(const ProjectilePools& pool,std::uint32_t slot,unsigned offset){
    if(slot==0xffffffffu)return pool.secondary.back().quad(offset-0x30u);
    return pool.bullets.at(slot).quad(offset);
}
inline void initialize_boss_pose(Enemy& enemy,std::uint64_t x,std::uint64_t y,std::uint64_t target_x,std::uint64_t target_y){
    enemy.word(8,14);enemy.quad(0x38,x);enemy.quad(0x40,y);enemy.quad(0xa8,target_x);enemy.quad(0xb0,target_y);
}
inline void item_at_projectile(ItemPool& items,ProjectilePools& projectiles,PatternParameters& parameters,std::int32_t slot,
    unsigned source,std::uint32_t kind,std::uint64_t angle,std::uint64_t speed,GameMath& math,unsigned precision){
    auto& item=items.items[static_cast<std::size_t>(slot)];
    item.word(0,1);item.word(0x10,0);item.word(8,kind);item.word(4,0);item.word(0xc,0);
    item.quad(0x18,quiet(projectile_position(projectiles,parameters.integers[source],0x40)));
    const auto x=dialogue_detail::real(item.quad(0x18));
    if(x<0)item.quad(0x18,0);else if(x>dialogue_detail::real(constant(0x6d1cb0)))item.quad(0x18,constant(0x6d1cb0));
    item.quad(0x20,quiet(projectile_position(projectiles,parameters.integers[source],0x48)));
    item.quad(0x28,binary_float::to_binary64(binary_float::multiply(math.cosine(angle),binary_float::binary64(speed),precision)));
    item.quad(0x30,binary_float::to_binary64(binary_float::multiply(math.sine(angle),binary_float::binary64(speed),precision)));
    item.quad(0x38,constant(0x6d23b0));
}
inline void drop_items(ItemPool& items,ProjectilePools& projectiles,PatternParameters& parameters,Random& random,GameMath& math,unsigned precision){
    using namespace binary_float;
    for(unsigned source=0;source<2;++source){
        for(std::uint32_t ring=0;ring<2;++ring){
            const auto base=to_binary64(multiply(random.real_number(constant(0x6d1c60),static_cast<RandomPrecision>(precision)),binary64(constant(0x6d1950)),precision));
            for(std::uint32_t element=0;element<20;++element){
                // 原空槽检查在角度/速度计算之前，满池仍消费每圈一次随机数。
                const auto slot=find_free_item(items);if(slot==-1)continue;
                const auto half=multiply(integer(element),binary64(constant(0x6d1860)),precision);
                const auto angle=to_binary64(add(divide(add(half,half,precision),binary64(constant(0x6d1a40)),precision),binary64(base),precision));
                const auto speed=to_binary64(divide(integer(ring*12u+48u),binary64(constant(0x6d1a40)),precision));
                item_at_projectile(items,projectiles,parameters,slot,source,source?4u:0u,angle,speed,math,precision);
            }
        }
        const auto slot=find_free_item(items);if(slot!=-1)item_at_projectile(items,projectiles,parameters,slot,source,source?7u:2u,0,0,math,precision);
    }
}
inline dialogue_detail::Lines script(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,
    BossAllocationContext& allocation,ProjectilePools& projectiles,const ObjectRecord<240>& player,DialogueServices& services,unsigned precision,
    PatternParameters& parameters,ItemPool& items,Random& random,GameMath& math){
    const auto mode=state.fields.word(0),phase=state.fields.word(8);
    if(mode<13||mode>15)return stage6_dialogue_detail::script(state,context,enemies,prefix,allocation,projectiles,player,services,precision);
    const Row* row=nullptr;const auto choose=[&](const auto& rows){if(phase>=1&&phase<=rows.size())row=&rows[phase-1];};
    if(mode==13){if(context.character==0)choose(mode13_character0);else if(context.character==1)choose(mode13_character1);else if(context.character==2)choose(mode13_character2);}
    else if(mode==14){if(context.character==0)choose(mode14_character0);else if(context.character==1)choose(mode14_character1);else if(context.character==2)choose(mode14_character2);}
    else{if(context.character==0)choose(mode15_character0);else if(context.character==1)choose(mode15_character1);else if(context.character==2)choose(mode15_character2);}
    dialogue_detail::Lines text;if(!row||(row->scene_guard&&context.scene!=0))return text;
    // 脚本行和角色在原分派时选定；服务回调后继续该行，保留其初始角色分支。
    const auto character=context.character;
    for(std::uint32_t i=0;i<row->count;++i)write(state,writes.at(row->first+i));
    if(row->action==1){
        const auto slot=static_cast<std::uint32_t>(services.allocate_boss(7));
        initialize_boss_pose(enemy_at(enemies,prefix,slot),constant(0x6d1a60),constant(0x6d1b58),constant(0x6d1a60),constant(0x6d1a90));
    }else if(row->action==2){
        const auto left=static_cast<std::uint32_t>(services.allocate_boss(7));
        auto& a=enemy_at(enemies,prefix,left);initialize_boss_pose(a,constant(0x6d23c8),constant(0x6d1b58),constant(0x6d23c0),constant(0x6d1a88));a.word(0x80,(character+1)%3);parameters.integers[3]=left;
        const auto right=static_cast<std::uint32_t>(services.allocate_boss(7));
        auto& b=enemy_at(enemies,prefix,right);initialize_boss_pose(b,constant(0x6d23b8),constant(0x6d1b58),constant(0x6d1de8),constant(0x6d1a88));b.word(0x80,(character+2)%3);parameters.integers[4]=right;
        const auto center=static_cast<std::uint32_t>(services.allocate_boss(7));
        auto& c=enemy_at(enemies,prefix,center);initialize_boss_pose(c,constant(0x6d1a60),constant(0x6d1b58),constant(0x6d1a60),constant(0x6b25f8));c.word(0x80,character);parameters.integers[2]=center;
        c.word(0x18,9999999);enemy_at(enemies,prefix,parameters.integers[2]).word(0x14,9999999);
        for(unsigned index=3;index<=4;++index){enemy_at(enemies,prefix,parameters.integers[index]).word(0x18,9999999);enemy_at(enemies,prefix,parameters.integers[index]).word(0x14,9999999);}
    }else if(row->action==3){
        for(unsigned side=0;side<2;++side){
            const auto slot=allocate_projectile(projectiles,{32,1,{{0,0,0}},0,255,0,41});if(slot==-1)continue;
            auto& bullet=projectiles.bullets.at(static_cast<std::size_t>(slot));bullet.word(0x78,(character+side+1)%3);bullet.word(0x7c,0);
            bullet.quad(0x40,constant(side?0x6d23b8:0x6d23c8));bullet.quad(0x48,constant(0x6d1d68));
            bullet.quad(0xa0,constant(side?0x6d1de8:0x6d23c0));bullet.quad(0xa8,constant(0x6d1af0));
            bullet.word(8,189);bullet.word(0x34,1);bullet.word(0x3c,1);parameters.integers[side]=static_cast<std::uint32_t>(slot);
        }
    }else if(row->action==4)drop_items(items,projectiles,parameters,random,math,precision);
    text.first=line(row->text1);text.second=line(row->text2);
    if(row->music&&state.fields.word(4)==0)services.music(14,1);
    return text;
}
}
namespace kaikeidou {
inline void update_ex_dialogue(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,
    BossAllocationContext& allocation,ProjectilePools& projectiles,const ObjectRecord<240>& player,DialogueServices& services,unsigned precision,
    PatternParameters& parameters,ItemPool& items,Random& random,GameMath& math){
    const auto script=[&](DialogueState& s,DialogueContext& c,EnemyPool& e,Enemy& p,DialogueServices& v){return ex_dialogue_detail::script(s,c,e,p,allocation,projectiles,player,v,precision,parameters,items,random,math);};
    update_stage1_dialogue(state,context,enemies,prefix,services,precision,script);
}
}
