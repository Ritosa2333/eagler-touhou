#include "bullet_core.hpp"
#include <iostream>
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("Special bullet contract failed");}
int main(){try{
 soew::RandomRing rng;
 for(int rank=0;rank<5;++rank){soew::BulletCore pool(rank);pool.playperf=16;soew::EnemyBulletRequest r;r.kind=r.Bullet16;r.group=0x83;r.left=100;r.top=100;r.speed=8;r.angle=0;check(pool.add(r,0,300,300,rng));check(pool.bullets[0].velocity==std::array<int,2>{8,0});check(pool.easy_skip==0);pool.special.u1=3;pool.update(0,300,300);check(pool.bullets[0].position[0][1]==1600);check(pool.bullets[0].velocity[1]==3);pool.begin_frame(1);pool.update(1,300,300);check(pool.bullets[0].position[1][1]==1603&&pool.bullets[0].velocity[1]==6);}
 {soew::BulletCore p;auto &b=p.bullets[0];b.kind=soew::EnemyBulletRequest::Bullet16;b.flag=1;b.motion=0x80;b.position[0]={1600,1600};p.special.u1=2;p.update(0,200,200);check(b.velocity==std::array<int,2>{2,2}&&b.position[0][0]==1600);}
 {soew::BulletCore p;auto &b=p.bullets[0];b.kind=soew::EnemyBulletRequest::Bullet16;b.flag=1;b.motion=0x81;b.speed=32;b.position[0]={1600,1600};b.velocity={0,0};p.special.u2=2;p.update(0,200,200);check(b.counter==1&&b.motion==0x81&&b.velocity==soew::enemy_vector(32,32));p.begin_frame(1);p.update(1,200,200);check(b.counter==2&&b.motion==0);}
 {soew::BulletCore p;auto &b=p.bullets[0];b.kind=soew::EnemyBulletRequest::Bullet16;b.flag=1;b.motion=0x82;b.speed=32;b.position[0]={1600,1600};b.velocity={-1,1};p.update(0,300,300,1);check(b.velocity==std::array<int,2>{-1,1});p.begin_frame(1);p.update(1,300,300,8);check(b.counter==1&&b.motion==0&&b.velocity==soew::enemy_vector(soew::enemy_atan2(208,208),32));}
 {soew::BulletCore p;auto &b=p.bullets[0];b.kind=soew::EnemyBulletRequest::Bullet16;b.flag=1;b.motion=0x84;b.angle=250;b.speed=250;b.position[0]={1600,1600};p.special={10,10,0};p.update(0,300,300);check(b.angle==4&&b.speed==4&&b.counter==1&&b.motion==0);check(b.velocity==soew::enemy_vector(4,4));}
 {soew::BulletCore p;auto &b=p.bullets[0];b.kind=soew::EnemyBulletRequest::Bullet16;b.flag=1;b.motion=0x85;b.angle=0;b.speed=32;b.position[0]={1600,1600};p.special={64,3,10};p.update(0,300,300);check(b.angle==64&&b.speed==32&&b.velocity==std::array<int,2>{3,35});}
 {soew::BulletCore p;auto &b=p.bullets[0];b.kind=soew::EnemyBulletRequest::Bullet16;b.flag=1;b.motion=0x87;b.position[0]={39*16,23*16};b.velocity={0,0};p.special.u3=1;p.update(0,300,300);check(b.counter==2&&b.motion==0);}
 // Byte counter overflow never reaches a16-bit limit of256.
 {soew::BulletCore p;auto &b=p.bullets[0];b.kind=soew::EnemyBulletRequest::Bullet16;b.flag=1;b.motion=0x81;b.speed=0;b.position[0]={1600,1600};p.special.u2=256;for(unsigned i=0;i<512;++i){if(i)p.begin_frame(i&1);p.update(i&1,300,300,i);check(b.motion==0x81);}check(b.counter==0);}
 for(unsigned op:{0x80u,0x81u,0x82u,0x83u,0x84u,0x85u,0x87u,0x88u,0xffu}){soew::BulletCore p;soew::EnemyBulletRequest r;r.kind=r.Bullet16;r.group=static_cast<std::uint8_t>(op);r.left=100;r.top=100;r.speed=32;check(p.add(r,0,300,300,rng)&&p.spawned==1&&p.bullets[0].motion==op);}
 {soew::BulletCore p;soew::EnemyBulletRequest r;r.kind=r.Bullet16;r.group=0x86;r.left=100;r.top=100;r.speed=32;check(!p.add(r,0,300,300,rng)&&p.spawned==0);}
 std::cout<<"special_bullet_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
