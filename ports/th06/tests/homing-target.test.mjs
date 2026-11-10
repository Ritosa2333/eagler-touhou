import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';

// Execute the actual per-pilot target-selection block, with collision damage
// supplied at the seam. A target must be available before a homing shot hits.
test('cooperative homing acquires unhit enemies independently for every pilot',()=>{
  const root=resolve(import.meta.dirname,'..'),out=resolve(root,'build/homing-target-check');
  const source=readFileSync(resolve(root,'src/EnemyManager.cpp'),'utf8');
  const begin=source.indexOf('                    playerDamage[playerId] = 0;');
  const end=source.indexOf('                    if (mgr->spellcardInfo.isActive',begin);
  assert(begin>=0&&end>begin);
  const body=source.slice(begin,end);
  const fixture=`#include <cassert>
#include <cstdio>
using i32=int; using u8=unsigned char;
constexpr int TH06_MULTI_MAX_PLAYERS=3;
struct Vec{float x=-999,y=-999,z=0;};
struct Enemy{Vec position,hitboxDimensions;};
struct Pilot{Vec positionOfLastEnemyHit; int damage=0; bool active=true;
 int CalcDamageToEnemy(const Vec*,const Vec*,bool*){return damage;}};
Pilot g_Players[TH06_MULTI_MAX_PLAYERS];
bool IsPlayerGameplayActive(u8 id){return g_Players[id].active;}
int GetItemAutoCollectStateForPlayer(u8 id){return id+1;}
void visit(Enemy* curEnemy){
 int playerDamage[TH06_MULTI_MAX_PLAYERS]{},itemDropState=0,scoreDamage=0;
 for(u8 playerId=0;playerId<TH06_MULTI_MAX_PLAYERS;++playerId){${body}}
}
int main(){
 for(int count:{2,3}) for(int damaging=-1;damaging<count;++damaging){
  for(int id=0;id<3;++id){g_Players[id]=Pilot{};g_Players[id].active=id<count;g_Players[id].damage=id==damaging?10:0;}
  Enemy first{{80,100,0},{}},lower{{300,220,0},{}},tie{{120,220,0},{}},higher{{200,40,0},{}};
  visit(&first);
  for(int id=0;id<count;++id)assert(g_Players[id].positionOfLastEnemyHit.x==80);
  visit(&lower);visit(&tie);visit(&higher);
  for(int id=0;id<count;++id){assert(g_Players[id].positionOfLastEnemyHit.x==300);assert(g_Players[id].positionOfLastEnemyHit.y==220);}
  if(count==2)assert(g_Players[2].positionOfLastEnemyHit.x==-999);
 }
 std::puts("homing target: PASS (zero damage, mixed damage, 2P/3P, target priority, ties, inactive seat)");
}
`;
  mkdirSync(out,{recursive:true});
  writeFileSync(resolve(out,'target.cpp'),'#include <initializer_list>\n'+fixture);
  writeFileSync(resolve(out,'CMakeLists.txt'),`cmake_minimum_required(VERSION 3.20)
project(homing_target LANGUAGES CXX)
add_executable(target target.cpp)
target_compile_features(target PRIVATE cxx_std_17)
if(MSVC)
target_compile_options(target PRIVATE /UNDEBUG)
else()
target_compile_options(target PRIVATE -UNDEBUG)
endif()
`);
  const run=(cmd,args)=>execFileSync(cmd,args,{windowsHide:true,encoding:'utf8',maxBuffer:1024*1024});
  run('cmake',['-S',out,'-B',resolve(out,'build')]);
  run('cmake',['--build',resolve(out,'build'),'--config','Release']);
  console.log(run(resolve(out,'build',process.platform==='win32'?'Release/target.exe':'target'),[]).trim());
});
