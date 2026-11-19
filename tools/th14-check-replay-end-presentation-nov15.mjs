import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync,mkdtempSync} from 'node:fs';
import {resolve,join} from 'node:path';
import {spawnSync} from 'node:child_process';
import {WASI} from 'node:wasi';
import {pathToFileURL} from 'node:url';
const root=resolve(process.env.TH14_SOURCE_ROOT??'ports/th14/runtimes/th12/th12_web/cpp');
const source=readFileSync(process.env.TH14_APPLICATION_SOURCE??join(root,'sdl/Application.cpp'),'utf8');
function method(s){const a=s.indexOf('const u32* hosted_replay_status()const');assert(a>=0);let p=s.indexOf('{',a)+1,d=1;while(d){assert(p<s.length);d+=(s[p]==='{')-(s[p]==='}');p++;}return s.slice(a,p);}
const baseline=readFileSync(new URL('./fixtures/th14-replay-end-presentation-baseline.cpp',import.meta.url),'utf8');
const sessionSource=readFileSync(join(root,'game/GameSession.cpp'),'utf8');
assert(/if\(battle&&!battle->completion\.state\.ending_active\)battle->hud\.score\.update\(economy\.score_units\);/.test(sessionSource),'Native paused menu must keep its authored HUD callback');
assert(/if\(app\.hosted_replay&&app\.session\.state\.replay\)return app\.tick_once\(0\);/.test(source),'Hosted terminal updates must not consume physical game/menu input');

const cache=resolve('.cache/th14-replay-end-presentation');mkdirSync(cache,{recursive:true});const out=mkdtempSync(join(cache,'check-'));
const cxx=process.env.TH14_WASI_CXX??resolve('ports/th14/runtimes/th12/tools/wasi-sdk-34.0-x86_64-windows/bin/clang++.exe');
const skeleton=body=>`#include "HudScalarTh14.hpp"
using namespace th11;
enum class GameSessionPhase:u32 {stage=1,paused=2};
struct HudScore {i32 displayed=42309130,speed=0; i32 displayed_value()const{return displayed;}};
struct Battle {struct {struct {bool ending_active=false;} state;} completion;struct {HudScore score;} hud;bool game_over_requested=false;};
struct Playback {struct Entry {u32 frames=46936;} entry;u32 cursor=46936;const Entry* stage(u32 n)const{return n==7?&entry:nullptr;}u32 frame()const{return cursor;}};
struct Session {struct {bool replay=true;u32 stage=7;GameSessionPhase phase=GameSessionPhase::paused;}state;Playback playback;Battle owned;Battle* battle=&owned;struct Menu {bool forced_exit=true;}menu;Menu* pause_menu=&menu;struct {i32 score_units=73241120;}economy;};
struct Application {Session session;bool hosted_replay=true;${body}};
Application app;
extern "C" void setup(u32 which){app=Application{};app.session.battle=&app.session.owned;app.session.pause_menu=&app.session.menu;
 if(which==1){app.session.battle->game_over_requested=true;app.session.playback.cursor=17;app.session.economy.score_units=1000;app.session.battle->hud.score.displayed=500;}
 if(which==2){app.session.battle->completion.state.ending_active=true;}
 if(which==3){app.session.menu.forced_exit=false;}
 if(which==4){app.hosted_replay=false;}
 if(which==5){app.session.battle=nullptr;}
 if(which==6){app.session.battle->hud.score.displayed=app.session.economy.score_units;}
 if(which==7){app.session.economy.score_units=0;app.session.battle->hud.score.displayed=0;}
 if(which==8){app.session.battle->hud.score.displayed=0;app.session.economy.score_units=999999999;}
}
extern "C" const u32* status(){return app.hosted_replay_status();}
extern "C" i32 total(){return app.session.economy.score_units;}
extern "C" i32 shown(){return app.session.battle?app.session.battle->hud.score.displayed:0;}
extern "C" void tick_menu(){auto& s=app.session;if(!s.battle||s.battle->completion.state.ending_active)return;auto& h=s.battle->hud.score;i32 high=0,flags=0;
 th14_hud_score_update([&](HudScoreAccessTh14,HudScoreWordTh14 word)->i32 {switch(word){case HudScoreWordTh14::Current:return s.economy.score_units;case HudScoreWordTh14::Displayed:return h.displayed;case HudScoreWordTh14::Speed:return h.speed;case HudScoreWordTh14::High:return high;case HudScoreWordTh14::Flags:return flags;default:return 0;}},
 [&](HudScoreAccessTh14,HudScoreWordTh14 word,i32 value){switch(word){case HudScoreWordTh14::Displayed:h.displayed=value;break;case HudScoreWordTh14::Speed:h.speed=value;break;case HudScoreWordTh14::High:high=value;break;case HudScoreWordTh14::Flags:flags=value;break;default:break;}});
}
`;
const modules={};
for(const [mode,body]of [['baseline',baseline],['candidate',method(source)]]){
 const cpp=join(out,mode+'.cpp'),wasm=join(out,mode+'.wasm');writeFileSync(cpp,skeleton(body));
 const result=spawnSync(cxx,['-std=c++17','-O1','-fno-exceptions','-fno-rtti','-DPORT_TITLE=14','-mexec-model=reactor','-Wl,--export-all','-I'+join(root,'game'),cpp,'-o',wasm],{encoding:'utf8',timeout:120000,env:{...process.env,TEMP:out,TMP:out,TMPDIR:out}});
 writeFileSync(join(out,mode+'-compile.log'),(result.stdout??'')+(result.stderr??''));assert.equal(result.status,0,result.stderr??String(result.error));
 const wasi=new WASI({version:'preview1',args:[],env:{}});const instance=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(instance.instance);modules[mode]=instance.instance.exports;
}
const words=c=>Array.from(new Uint32Array(c.memory.buffer,c.status(),6));
let baselineRejected=0;const cases=[];
for(let scenario=0;scenario<9;scenario++){
 const c=modules.candidate,b=modules.baseline;c.setup(scenario);b.setup(scenario);const initial=words(c),old=words(b),score=c.total();
 const pending=[0,1,8].includes(scenario);assert.equal(initial[3],pending?0:[3,4].includes(scenario)?0:1);
 assert.equal(initial[1],scenario===1?17:46936,'real early cursor retained');assert.equal(initial[2],46936);
 if(pending){assert.equal(old[3],1);baselineRejected++;let ticks=0;while(!words(c)[3]&&ticks<2000){c.tick_menu();ticks++;assert.equal(c.total(),score,'presentation cannot change actual score');}assert.equal(words(c)[3],1);assert.equal(c.shown(),score);cases.push({scenario,settleTicks:ticks,totalUnchanged:true});}
 else {assert.deepEqual(initial,old,'unaffected end/status control');cases.push({scenario,unchanged:true});}
}
// Exercise the authored host controller against the actual extracted C++
// status and HUD provider. Session/menu types above are component stand-ins.
const host=resolve(process.env.TH14_HOST_SOURCE??join(root,'../sdl-runtime/eagler-host.mjs'));
const {createReplayViewerHostTh14}=await import(pathToFileURL(host));
async function hostCase(c){const jobs=new Map();let next=0,paused=true;c.setup(0);const events=[];
 const controller=createReplayViewerHostTh14({read:path=>({path,bytes:new Uint8Array([1])}),describe:()=>({character:0,shot:1,difficulty:4,stages:[{id:7,frames:46936}]}),open:()=>c.setup(0),status:()=>{const w=words(c);return {stage:w[0],frame:w[1],frames:w[2],ended:!!w[3],phase:w[4],active:!!w[5]};},step:()=>0,rate:()=>{},pause:on=>{paused=on;},close:()=>{},emit:s=>events.push(s),schedule:f=>{jobs.set(++next,f);return next;},cancel:id=>jobs.delete(id)});
 await controller.command({action:'open',path:'replay/test.rpy',stage:7});let ticks=0;while(jobs.size&&ticks<2000){if(!paused)c.tick_menu();const [id,f]=jobs.entries().next().value;jobs.delete(id);await f();ticks++;if(paused)break;}
 const result={paused,shown:c.shown(),score:c.total(),polls:ticks,last:events.at(-1)};await controller.close();return result;}
const oldHost=await hostCase(modules.baseline),newHost=await hostCase(modules.candidate);
assert(oldHost.paused&&oldHost.shown<oldHost.score);assert(newHost.paused&&newHost.shown===newHost.score);assert.equal(newHost.last.frame,46936);assert(newHost.last.ended);
const report={passed:true,cases,baselineRejected,hostRegression:{baselineFrozenScore:oldHost.shown*10,candidateSettledScore:newHost.shown*10,actualScoreUnchanged:oldHost.score===newHost.score},scope:'actual extracted Application status plus actual HUD scalar and authored host; session/menu stand-ins; no full browser acceptance',completeGame:false};
writeFileSync(join(out,'qualified.json'),JSON.stringify(report,null,2)+'\n');if(process.env.TH14_CHECK_OUTPUT)writeFileSync(process.env.TH14_CHECK_OUTPUT,JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({out,...report}));
