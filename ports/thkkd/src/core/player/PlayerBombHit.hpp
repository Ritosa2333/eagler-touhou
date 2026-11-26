// 恢复完整65e340炸弹碰撞；逐Bomb槽先扫敌人再扫普通弹，保留闲置Bomb槽仍清弹的原行为。
#pragma once
#include "core/player/PlayerBomb.hpp"
#include "core/player/PlayerBombHitServices.hpp"
#include "core/shared/PolygonCircleCollision.hpp"
namespace kaikeidou {
inline bool player_bomb_projectile_visible(const Projectile& projectile){
    const auto x=crt_sse2::real(projectile.quad(0x40)),y=crt_sse2::real(projectile.quad(0x48));return x>0.&&x<384.&&y>0.&&y<448.;
}
inline void hit_player_bombs(const PlayerBombPool& pool,const ObjectRecord<240>& player,const PlayerShotFireContext& fire,
    EnemyPool& enemies,ProjectilePools& projectiles,PlayerBombHitServices& services,GameMath& math,unsigned precision){
    if(player.word(0x2c)==0)return;
    using player_shot_hit_detail::distance;const player_shot_hit_detail::Arithmetic m{precision};
    const auto selector=fire.options.character*10u+fire.options.weapon;
    if(selector==0){
        std::array<std::array<std::uint64_t,2>,4> vertices{};
        for(std::uint32_t i=0;i<4;++i){
            const auto angle=m.store(m.add(m.add(m.div(m.mul(m.i(i),m.q(0x400921fb54442d28ull)),m.n(2.)),m.q(pool.bombs[0].quad(0x48))),m.q(0x3fe921fb54442d28ull)));
            const auto scale=m.store(m.mul(m.q(pool.bombs[0].quad(0x20)),m.n(50.)));
            const auto radius=m.store(m.mul(finite_square_root(crt_sse2::bits(2.),precision),m.q(scale)));
            vertices[i][0]=m.store(m.add(m.mul(math.cosine(angle),m.q(radius)),m.q(pool.bombs[0].quad(0x10))));
            vertices[i][1]=m.store(m.add(m.mul(math.sine(angle),m.q(radius)),m.q(pool.bombs[0].quad(0x18))));
        }
        for(std::uint32_t i=0;i<enemies.enemies.size();++i){const auto& e=enemies.enemies[i];if(e.word(0)!=1||!player_bomb_enemy_visible(e))continue;
            const auto x=options_detail::stored_bits(e.quad(0x38)),y=options_detail::stored_bits(e.quad(0x40));
            const auto radius=m.store(m.mul(m.i(enemy_contact_radius(e.word(4))),m.n(.5)));
            if(polygon_circle_overlap(vertices,x,y,radius,precision,math))services.damage(i,24);
        }
        for(std::size_t i=0;i<projectiles.bullets.size();++i){const auto& bullet=projectiles.bullets[i];if(bullet.word(0)!=1||!player_bomb_projectile_visible(bullet))continue;
            if(polygon_circle_overlap(vertices,options_detail::stored_bits(bullet.quad(0x40)),options_detail::stored_bits(bullet.quad(0x48)),0,precision,math))remove_projectile(projectiles,i,0);
        }
        return;
    }
    if(selector==12){
        std::array<std::uint32_t,7> columns{},rows{};
        const auto absolute=[](std::uint32_t v){return v>>31?0u-v:v;};
        for(std::uint32_t i=0;i<7;++i){columns[i]=7u-absolute(pool.bombs[0].word(4)-i);rows[i]=7u-absolute(pool.bombs[0].word(8)-i);}
        for(std::uint32_t i=0;i<enemies.enemies.size();++i){const auto& e=enemies.enemies[i];if(e.word(0)!=1||!player_bomb_enemy_visible(e))continue;
            const auto column=floor_to_int32(m.store(m.div(m.add(m.q(e.quad(0x38)),m.n(32.)),m.n(64.))));
            const auto row=floor_to_int32(m.store(m.div(m.q(e.quad(0x40)),m.n(64.))));
            services.damage(i,columns.at(column)+(row==7?services.overflow_row_word():rows.at(row)));
        }
        clear_projectiles(projectiles,0);return;
    }
    std::size_t count=0,field=0;double factor=1.;std::uint32_t amount=0;bool enemy_radius=false,active_enemy_bomb=false;
    switch(selector){
    case 1:count=8;field=0x50;factor=32.;amount=20;enemy_radius=true;break;
    case 2:count=1;field=0x28;factor=128.;amount=24;enemy_radius=true;break;
    case 10:count=2;field=0x20;factor=64.;amount=48;enemy_radius=true;break;
    case 11:count=30;field=0x20;factor=128.;amount=2;active_enemy_bomb=true;break;
    case 20:count=30;field=0x20;factor=16.;amount=8;active_enemy_bomb=true;break;
    case 21:count=30;field=0x28;amount=12;active_enemy_bomb=true;break;
    case 22:count=1;field=0x20;factor=100.;enemy_radius=true;break;
    default:return;
    }
    for(std::size_t index=0;index<count;++index){const auto& b=pool.bombs[index];
        for(std::uint32_t i=0;i<enemies.enemies.size();++i){const auto& e=enemies.enemies[i];if(e.word(0)!=1||!player_bomb_enemy_visible(e))continue;
            if(active_enemy_bomb&&b.word(0)!=1)continue;
            const auto d=distance(m,b.quad(0x10),b.quad(0x18),e.quad(0x38),e.quad(0x40));
            auto radius=selector==21?m.q(b.quad(field)):m.mul(m.q(b.quad(field)),m.n(factor));
            if(enemy_radius){radius=m.q(m.store(radius));radius=m.add(m.mul(m.i(enemy_contact_radius(e.word(4))),m.n(.5)),radius);}
            if(m.le(m.q(d),radius))services.damage(i,selector==22?b.word(4):amount);
        }
        for(std::size_t i=0;i<projectiles.bullets.size();++i){const auto& bullet=projectiles.bullets[i];if(bullet.word(0)!=1||!player_bomb_projectile_visible(bullet))continue;
            const auto d=distance(m,b.quad(0x10),b.quad(0x18),bullet.quad(0x40),bullet.quad(0x48));
            const auto radius=selector==21?m.q(b.quad(field)):m.mul(m.q(b.quad(field)),m.n(factor));
            if(m.le(m.q(d),radius))remove_projectile(projectiles,i,0);
        }
    }
}
}
