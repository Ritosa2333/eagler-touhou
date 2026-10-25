import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {AnimationSampler,MenuAnimation} from '../src/animation.mjs';
import {GameProfile} from '../src/game-profile.mjs';
import {readCoreHud,Renderer} from '../src/renderer.mjs';
import {StageLoader} from '../src/stage-loader.mjs';

test('HI SCORE uses the saved machine/difficulty and a separate per-stage Practice record',()=>{
 const profile=new GameProfile({});
 profile.data.scores=[{character:0,shot:0,difficulty:1,stage:6,score:345678900,ranked:true},
  {character:1,shot:0,difficulty:1,stage:6,score:900000000,ranked:true},
  {character:0,shot:0,difficulty:2,stage:6,score:800000000,ranked:true},
  {character:0,shot:0,difficulty:1,stage:3,score:45678000,mode:'practice',practice:true,ranked:true}];
 assert.equal(profile.highScore(0,0,1),345678900);assert.equal(profile.highScore(0,0,1,3),45678000);
 assert.equal(profile.highScore(0,0,1,2),0);
 profile.data.nativePractice={'0:1':[{scoreStored:7654321}]};assert.equal(profile.highScore(0,0,1,1),76543210);
 const core={HEAPU8:new Uint8Array(512),_th12_hud:()=>64,_th12_hud_size:()=>41,_th12_score_stored:()=>12345678};
 let h=readCoreHud(core,{highScore:profile.highScore(0,0,1),scoreLowDigit:3});
 assert.equal(h[3],123456783);assert.equal(h[40],345678900);
 h=readCoreHud(core,{highScore:100,scoreLowDigit:3});assert.equal(h[40],123456783);
 h=readCoreHud(core,{highScore:100,scoreLowDigit:0});assert.equal(h[3],123456780);
});

test('runtime HI SCORE preserves zero and continue digits without Float32 rounding',()=>{
 const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8').replaceAll('\r\n','\n');
 const from=source.indexOf('function gameTick('),to=source.indexOf('\n}\n',from)+2;
 const session={stage:1,cursor:0,clock:0,hudAge:0,previous:0,highScore:0,replay:{input:()=>({held:0,pressed:0})}};
 let continues=0;
 const context={session,core:{_th12_score_stored:()=>12345678,_th12_motion(){},_th12_tick(){},_th12_music_serial:()=>0},
  readJSON:()=>({continueCount:continues}),readCoreSoundEvents:()=>null,effect(){},replayInputEnded:()=>false,
  runtimeOptions:{replayViewer:true},viewerMusic:{ticks:0},audio:{musicSerial:0}};
 const tick=runInNewContext(source.slice(from,to)+';gameTick',context);
 tick({held:0});assert.equal(session.highScore,123456780);assert.equal(session.highScore%10,0);
 continues=3;tick({held:0});assert.equal(session.highScore,123456783);assert.equal(session.highScore%10,3);
});

for(const op of [93,94])for(const duration of [0,4])test(`ANM ${op} scroll interpolation starts at its current velocity (${duration} ticks)`,()=>{
 const axis=op===93?'u':'v',commands=[{op:op===93?70:71,time:0,offset:0,ints:[0],floats:[.125]},
  {op,time:0,offset:4,ints:[duration,0,0],floats:[0,0,.375]},
  {op:63,time:20,offset:8,ints:[],floats:[]}];
 const animation=new MenuAnimation(new AnimationSampler({test:{scripts:[{commands}]}}),'test',0),track=animation.track;
 assert.equal(track.interpolations.size,duration?1:0);
 if(duration)assert.equal(track.interpolations.values().next().value.from[0],.125);
 else assert.equal(track.velocity[axis],.375);
 for(let i=0;i<8;i++){animation.tick();assert(Number.isFinite(track.state[axis]));assert(Number.isFinite(track.velocity[axis]));}
 assert.equal(track.velocity[axis],.375);
});

test('next-stage prefetch leaves the live core and background untouched until the boundary',async()=>{
 const calls=[],core={HEAPU8:new Uint8Array(512),_malloc:()=>32,_free(){},_th12_select_stage:n=>calls.push(n),_th12_unload_anm(){}};
 for(const name of ['ecl','std','sht','msg','anm'])core['_th12_load_'+name]=()=>1;
 let reads=0;const loader=new StageLoader(core,{},async()=>{reads++;return new Uint8Array([1]);});
 await loader.prepare(2);const prefetched=reads;assert.deepEqual(calls,[]);
 await loader.load(2);assert.deepEqual(calls,[2]);assert.equal(reads,prefetched);
 const old={},next={},renderer={requestedStage:1,background:old,stageJobs:new Map([[2,Promise.resolve(next)]])};
 await Renderer.prototype.prepareStage.call(renderer,2,{activate:false});assert.equal(renderer.background,old);assert.equal(renderer.requestedStage,1);
 await Renderer.prototype.prepareStage.call(renderer,2);assert.equal(renderer.background,next);
});

test('replay stage changes enable clear ticks and reset only the incoming fade',()=>{
 const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
 const from=source.indexOf('function initializeReplayStage('),to=source.indexOf('\nfunction touchState',from);
 const calls=[],context={core:{_th12_replay_stage_flow:(...a)=>{calls.push(a);return 1;}},practiceBridge:{setReplay(){},prepareReplay(){return true;}},
  startReplayStage(){},checkDamageEntry(){},runtimeOptions:{},session:{mode:'replay'}};
 const initialize=runInNewContext(source.slice(from,to)+';initializeReplayStage',context);
 initialize({practice:false,difficulty:1},{});initialize({practice:false,difficulty:1},{},{fadeIn:true});initialize({practice:true,difficulty:1},{});
 assert.deepEqual(calls,[[0,0],[0,1],[1,0]]);
});

for(const replay of [null,{practice:false,stages:[{number:1},{number:2}]}])test(`stage-clear prefetch and ${replay?'Replay':'live'} transition retain the battle picture during resource installation`,async()=>{
 const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8'),calls=[];
 let release;const gate=new Promise(resolve=>release=resolve);
 const session={mode:replay?'replay':'story',stage:1,character:0,shot:0,difficulty:1,replay,
  recorder:{finishStage:()=>calls.push('finish')},cursor:123};
 const held={frame:123};const context={currentPresentation:held,session,busy:false,fatalError:false,last:0,accumulator:20,performance:{now:()=>123},
  readJSON:()=>({status:'stage-clear',nextStage:2}),loader:{prepare:()=>{calls.push('prefetch-core');return gate;}},
  renderer:{renderPresentation:(previous,current)=>{assert.equal(current,held);calls.push('held HUD');},prepareStage:(n,o)=>{calls.push(['prefetch-background',n,o.activate]);return gate;}},
  audio:{context:null},runtimeMusic:'none',fail:error=>{throw error;},
  localization:{setContext:()=>calls.push('localize')},prepareStage:async n=>{calls.push(['install',n]);},
  resetGamePresentation:()=>calls.push('reset'),touch:{cancel(){}},recordStage:()=>calls.push('record'),
  profile:{recordStageClear(){},recordStageEntry(){},unlockMusic(){}},saveProfile:()=>new Promise(()=>{}),
  core:{_th12_score_stored:()=>123,_th12_start_next_stage:()=>1,_th12_practice_damage_transition:()=>1,_th12_music_serial:()=>0,_th12_music_track:()=>0},
  musicIndex:[{id:'stage2'}],stageMusic:[null,'stage1','stage2'],playMusic:async()=>{},captureGamePresentation:()=>calls.push('capture'),
  initializeReplayStage:(r,p,o)=>calls.push(['replay',p.number,o.fadeIn]),loadingScreen:{startResources:({renderBase})=>renderBase(),completeResources(){}},meter:{reset(){}}};
 const warmFrom=source.indexOf('function warmNextStage('),warmTo=source.indexOf('\nasync function prepareStage(',warmFrom);
 const nextFrom=source.indexOf('function holdStagePresentation('),nextTo=source.indexOf('\nfunction resultData(',nextFrom);
 const functions=runInNewContext(source.slice(warmFrom,warmTo)+source.slice(nextFrom,nextTo)+';({warmNextStage,nextStage})',context);
 functions.warmNextStage();functions.warmNextStage();assert.equal(calls.filter(n=>n==='prefetch-core').length,1);
 assert.equal(session.stage,1);assert.equal(session.cursor,123);assert(!calls.includes('reset'));
 const crossing=functions.nextStage();assert(context.busy);assert.equal(session.stage,1);assert(calls.includes('held HUD'));
 assert(!calls.some(n=>Array.isArray(n)&&n[0]==='install'));
 release();await crossing;assert.equal(session.stage,2);assert.equal(session.cursor,0);assert.equal(context.busy,false);
 assert(!calls.includes('loading'));assert(calls.includes('capture'));
 assert.deepEqual(calls.filter(Array.isArray).map(a=>[...a]),[['prefetch-background',2,false],['install',2],...(replay?[['replay',2,true]]:[])]);
});
