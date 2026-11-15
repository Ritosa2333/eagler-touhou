import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const site=process.env.TH128_DEBUG_STAGE_SITE?path.resolve(process.env.TH128_DEBUG_STAGE_SITE):path.join(root,'site');
const source=fs.readFileSync(path.join(site,'play.mjs'),'utf8');
const html=fs.readFileSync(path.join(site,'play.html'),'utf8');
const originalStages=JSON.parse(fs.readFileSync(path.join(root,'local/research/stage-table.json'),'utf8')).stages;
const expected=[['A1-3',3,'st_a1_3.ecl'],['A2-3',5,'st_a2_3.ecl'],['B1-3',8,'st_b1_3.ecl'],
  ['B2-3',10,'st_b2_3.ecl'],['C1-3',13,'st_c1_3.ecl'],['C2-3',15,'st_c2_3.ecl']];

function functionSource(name){
  const match=new RegExp(`(?:async )?function ${name}\\(`).exec(source);
  assert.ok(match,`missing production function ${name}`);
  const start=match.index,open=source.indexOf('{',start);
  let depth=0,quote=null,escape=false;
  for(let index=open;index<source.length;++index){
    const character=source[index];
    if(quote){if(escape)escape=false;else if(character==='\\')escape=true;else if(character===quote)quote=null;continue;}
    if(character==='\''||character==='"'||character==='`'){quote=character;continue;}
    if(character==='{')++depth;
    if(character==='}'&&--depth===0)return source.slice(start,index+1);
  }
  throw new Error('unterminated production function '+name);
}

const functions=['debugRespawnSupported','syncDebugRespawnControl','syncDebugRespawnUI','applyDebugInfiniteRespawn',
  'debugThirdStageSelection','syncDebugStageJumpControls','startDebugThirdStage'];
const factory=new Function('state','menu','find','prepareStage','startLaunch','clock','lastCheckWorldError','status','savePreference',
  functions.map(functionSource).join('\n')+`\nreturn {${functions.join(',')}};`);

function setup({requested=false,setterResult=1,supported=true}={}){
  const controls={
    '#debug-stage-select':{disabled:true,value:'A1-3'},'#debug-stage-start':{disabled:true},
    '#debug-stage-status':{textContent:''},'#debug-infinite-respawn':{checked:requested,disabled:false},
    '#debug-respawn-note':{textContent:''},'#debug-respawn-indicator':{hidden:true,textContent:''},
    '#variant':{disabled:false},'#game':{focus(){}},
  };
  const calls=[],launches=[],saved=[],messages=[];
  const snapshot={frame:0,flow:{currentId:0,difficulty:1},debug:{runTainted:false,infiniteRespawn:requested}};
  const module={
    UTF8ToString:value=>value,
    _th128_world_set_debug_infinite_respawn(value){calls.push(value);if(!setterResult)return 0;
      snapshot.debug.infiniteRespawn=value===1;if(value===1)snapshot.debug.runTainted=true;return 1;},
    _th128_world_state:()=>JSON.stringify(snapshot),_th128_world_error:()=>setterResult?'':'native rejected',
  };
  const state={module:supported?module:null,stageCatalog:{stages:originalStages},debugInfiniteRespawnRequested:requested,
    handoff:false,pendingCarryToken:0,worldMode:false,active:false,paused:true,stageSelection:null};
  const menu={launching:false};
  const clock={resets:0,reset(){++this.resets;}};
  const find=selector=>controls[selector]??null;
  const prepareStage=(id,options)=>({stage:state.stageCatalog.stages.find(stage=>stage.id===id),...options});
  let api;
  const startLaunch=async selection=>{
    launches.push(selection);menu.launching=true;
    try{api.applyDebugInfiniteRespawn(module,selection);snapshot.flow.currentId=selection.stage.id;
      state.stageSelection=selection;state.worldMode=true;state.active=true;state.paused=false;}
    finally{menu.launching=false;}
  };
  api=factory(state,menu,find,prepareStage,startLaunch,clock,'',message=>messages.push(message),(...values)=>saved.push(values));
  return {api,state,menu,controls,calls,launches,saved,messages,snapshot,module,clock};
}

test('six visible entries sit above the game and start disabled',()=>{
  assert.ok(html.indexOf('id="debug-stage-jump"')<html.indexOf('id="game"'));
  assert.match(html,/id="debug-stage-select"[^>]*disabled/);
  assert.match(html,/id="debug-stage-start"[^>]*disabled/);
  for(const [route]of expected)assert.ok(html.includes(`value="${route}"`),route);
  assert.match(html,/不计正常通关/);
});

test('all six labels resolve exact original third-stage resources and fresh Normal context',()=>{
  const run=setup();
  for(const [route,id,ecl]of expected){
    const selection=run.api.debugThirdStageSelection(route);
    assert.equal(selection.stage.id,id);assert.equal(selection.stage.ecl,ecl);
    assert.equal(selection.difficulty,1);assert.equal(selection.progress,0);assert.equal(selection.seed,34772);
    assert.equal(selection.debugJump.route,route);assert.equal(selection.debugJump.normalModeClear,false);
    assert.equal(selection.debugJump.carryVerified,false);assert.equal(selection.carryToken,undefined);
  }
});

test('invalid label and altered catalog refuse before native calls',async()=>{
  const run=setup();
  for(const route of ['',null,'A1-2','a1-3','../A1-3',3])await assert.rejects(run.api.startDebugThirdStage(route));
  run.state.stageCatalog={stages:originalStages.map(stage=>stage.id===3?{...stage,ecl:'st_a1_1.ecl'}:stage)};
  await assert.rejects(run.api.startDebugThirdStage('A1-3'));
  assert.equal(run.launches.length,0);assert.deepEqual(run.calls,[]);
});

test('each jump uses real native taint while retaining the infinite-respawn preference',async()=>{
  for(const [route,id]of expected){
    const run=setup();const receipt=await run.api.startDebugThirdStage(route);
    assert.equal(run.launches.length,1);assert.equal(run.state.stageSelection.stage.id,id);
    assert.deepEqual(run.calls,[1,0]);assert.equal(run.snapshot.debug.runTainted,true);
    assert.equal(run.snapshot.debug.infiniteRespawn,false);assert.equal(run.state.debugInfiniteRespawnRequested,false);
    assert.equal(receipt.stageId,id);assert.equal(receipt.normalModeClear,false);assert.equal(receipt.nativeTainted,true);
    assert.deepEqual(run.saved,[]);assert.equal(run.snapshot.frame,0);
  }
});

test('enabled infinite respawn stays enabled without an extra native mark',async()=>{
  const run=setup({requested:true});await run.api.startDebugThirdStage('A2-3');
  assert.deepEqual(run.calls,[1]);assert.equal(run.snapshot.debug.infiniteRespawn,true);assert.deepEqual(run.saved,[]);
});

test('ordinary stage preparation retains its original one-call debug behavior',()=>{
  const run=setup();run.api.applyDebugInfiniteRespawn(run.module);
  assert.deepEqual(run.calls,[0]);assert.equal(run.snapshot.debug.runTainted,false);
});

test('busy handoff, owned carry and launch do not replace a current run',async()=>{
  for(const field of ['handoff','pendingCarryToken','launching']){
    const run=setup();if(field==='launching')run.menu.launching=true;else run.state[field]=field==='pendingCarryToken'?9:true;
    await assert.rejects(run.api.startDebugThirdStage('B1-3'));assert.equal(run.launches.length,0);assert.deepEqual(run.calls,[]);
  }
});

test('missing or rejected native taint cannot be reported as a successful debug start',async()=>{
  const missing=setup({supported:false});await assert.rejects(missing.api.startDebugThirdStage('B2-3'));
  assert.equal(missing.launches.length,0);
  const denied=setup({setterResult:0});await assert.rejects(denied.api.startDebugThirdStage('C1-3'),/native|调试/);
  assert.equal(denied.state.active,false);assert.equal(denied.state.paused,true);
});

test('controls remain unavailable during loading or any in-flight stage operation',()=>{
  const run=setup();run.api.syncDebugStageJumpControls();assert.equal(run.controls['#debug-stage-start'].disabled,false);
  run.menu.launching=true;run.api.syncDebugStageJumpControls();assert.equal(run.controls['#debug-stage-select'].disabled,true);
  run.menu.launching=false;run.controls['#variant'].disabled=true;run.api.syncDebugStageJumpControls();
  assert.equal(run.controls['#debug-stage-start'].disabled,true);
});

test('jump watermark remains explicit even when infinite respawn is disabled',async()=>{
  const run=setup();await run.api.startDebugThirdStage('C2-3');run.api.syncDebugRespawnUI(run.snapshot);
  assert.equal(run.controls['#debug-respawn-indicator'].hidden,false);
  assert.match(run.controls['#debug-respawn-indicator'].textContent,/调试跳关.*C2-3/);
  assert.match(run.controls['#debug-respawn-indicator'].textContent,/不计正常通关/);
});

test('an older cancelled launch cannot close the subsequently started debug world',async()=>{
  const controls=new Map();
  const find=selector=>{if(!controls.has(selector))controls.set(selector,{hidden:false,textContent:''});return controls.get(selector);};
  const state={autoFlow:false,pressed:new Set(),worldMode:false,active:false,stageSelection:null};
  const menu={launching:false,launchId:0};
  const completions=[];let titleCalls=0;
  const checkWorld=()=>new Promise(resolve=>completions.push(resolve));
  const makeLaunch=new Function('state','menu','find','checkWorld','syncDebugStageJumpControls',
    'suppressHeldConfirmKeys','showMenuPage','showLaunchError','returnToTitle','worldMovementInput','status','lastCheckWorldError','assertStageStartAllowed',
    functionSource('startLaunch')+'\nreturn startLaunch;');
  const noop=()=>{};
  const stageGuard=new Function(source.slice(0,source.indexOf('const {installCheckpointAdapter'))+'\nreturn assertStageStartAllowed;')();
  const launch=makeLaunch(state,menu,find,checkWorld,noop,noop,noop,noop,()=>{++titleCalls;},noop,noop,'',stageGuard);
  const older={stage:{id:3},difficulty:1},newer={stage:{id:5},difficulty:1};
  const oldPromise=launch(older);++menu.launchId;menu.launching=false;
  const newPromise=launch(newer);
  state.worldMode=true;state.active=true;state.stageSelection=newer;
  completions[1]();await newPromise;
  completions[0]();await oldPromise;
  assert.equal(titleCalls,0);assert.equal(state.stageSelection,newer);assert.equal(state.active,true);
});
