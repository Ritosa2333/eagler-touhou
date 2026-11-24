import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const source=fs.readFileSync(path.join(root,'site/play.mjs'),'utf8');
const html=fs.readFileSync(path.join(root,'site/play.html'),'utf8');
const css=fs.readFileSync(path.join(root,'site/play.css'),'utf8');
const stages=JSON.parse(fs.readFileSync(path.join(root,'local/research/stage-table.json'),'utf8')).stages;

function functionSource(name){
  const match=new RegExp('function '+name+'\\(').exec(source);
  assert.ok(match,'missing production function '+name);
  const open=source.indexOf('{',match.index);
  let depth=0,quote=null,escape=false;
  for(let index=open;index<source.length;index+=1){
    const character=source[index];
    if(quote){if(escape)escape=false;else if(character==='\\')escape=true;else if(character===quote)quote=null;continue;}
    if(character==="'"||character==='"'||character==='`'){quote=character;continue;}
    if(character==='{')depth+=1;
    if(character==='}'&&--depth===0)return source.slice(match.index,index+1);
  }
  throw new Error('unterminated function '+name);
}

function setup(){
  const nodes=Object.fromEntries(['#game-frame-indicator','#runtime-overlay-frame'].map(name=>[name,{textContent:'',dataset:{}}]));
  const state={worldMode:false,frame:999999,worldSnapshot:null,stageCatalog:{stages},stageSelection:null,paused:false,handoff:false,handoffFailed:false,module:null};
  const menu={fault:false};
  const names=['gameplayFrameDetails','syncGameplayFrameUI','captureGameplayFrameUI'];
  const create=new Function('state','menu','find',names.map(functionSource).join('\n')+'\nreturn {'+names.join(',')+'};');
  return {state,menu,nodes,...create(state,menu,name=>nodes[name]??null)};
}

function world(stageId,frame=0){return {frame,flow:{currentId:stageId},debug:{runTainted:true,infiniteRespawn:false}};}

test('native frame readout is visible in the game and fault overlay, without live announcements',()=>{
  assert.match(html,/id="game-frame-indicator"/);
  assert.match(html,/id="runtime-overlay-frame"/);
  assert.ok(html.indexOf('id="runtime-overlay-frame"')>html.indexOf('id="runtime-overlay"'));
  assert.match(css,/#game-frame-indicator\{[^}]*z-index:4[^}]*pointer-events:none/);
  assert.doesNotMatch(html.match(/<[^>]*id="game-frame-indicator"[^>]*>/)?.[0]??'',/aria-live/);
});

test('title never presents menu ticks as native gameplay frames',()=>{
  const run=setup();run.state.worldSnapshot=world(5,333);
  const details=run.syncGameplayFrameUI();
  assert.equal(details.frame,null);assert.equal(details.stageId,null);
  assert.match(run.nodes['#game-frame-indicator'].textContent,/未进入关卡/);
  assert.doesNotMatch(run.nodes['#game-frame-indicator'].textContent,/999999|333/);
});

test('all six routes display the actual native stage and frame, not the selected menu route',()=>{
  const run=setup();run.state.worldMode=true;run.state.stageSelection={stage:stages[0],debugJump:{route:'A1-3'}};
  for(const [route,stageId]of [['A1-3',3],['A2-3',5],['B1-3',8],['B2-3',10],['C1-3',13],['C2-3',15]]){
    run.state.worldSnapshot=world(stageId,9876);
    const details=run.syncGameplayFrameUI();assert.equal(details.route,route);assert.equal(details.stageId,stageId);assert.equal(details.frame,9876);
    assert.match(details.text,/原生帧 9876/);assert.ok(details.text.includes(route));assert.match(details.text,/调试/);
  }
});

test('WORLD0 and ordinary progression use snapshot frames, pause does not synthesize ticks',()=>{
  const run=setup();run.state.worldMode=true;run.state.worldSnapshot=world(5,0);
  assert.equal(run.syncGameplayFrameUI().frame,0);
  run.state.worldSnapshot=world(5,123);assert.equal(run.syncGameplayFrameUI().frame,123);
  run.state.paused=true;const before=run.syncGameplayFrameUI();
  assert.match(before.text,/已暂停/);assert.equal(run.syncGameplayFrameUI().text,before.text);
});

test('first fault captures the native failure frame and both displays retain it',()=>{
  const run=setup();run.state.worldMode=true;run.state.worldSnapshot=world(5,120);run.menu.fault=true;
  let reads=0;run.state.module={UTF8ToString:value=>value,_th128_world_state(){reads+=1;return JSON.stringify(world(5,121));}};
  const details=run.captureGameplayFrameUI();assert.equal(reads,1);assert.equal(details.frame,121);assert.match(details.text,/已停止/);
  assert.equal(run.nodes['#game-frame-indicator'].textContent,run.nodes['#runtime-overlay-frame'].textContent);
  run.state.module=null;assert.equal(run.syncGameplayFrameUI().frame,121);
});

test('unavailable native failure snapshot is unknown, not a stale successful frame',()=>{
  const run=setup();run.state.worldMode=true;run.state.worldSnapshot=world(5,120);run.menu.fault=true;
  run.state.module={UTF8ToString:value=>value,_th128_world_state(){throw new Error('unavailable');}};
  const details=run.captureGameplayFrameUI();assert.equal(details.frame,null);assert.match(details.text,/未知/);
  assert.equal(run.state.worldSnapshot,null);
});

test('invalid native frame values never fall back to UI frames, clocks or zero',()=>{
  const run=setup();run.state.worldMode=true;
  for(const frame of [undefined,null,-1,1.5,'99',Number.MAX_SAFE_INTEGER+1]){
    run.state.worldSnapshot={...world(5),frame};const details=run.syncGameplayFrameUI();assert.equal(details.frame,null);assert.match(details.text,/未知/);
  }
});

test('new native world resets the frame and return-to-title clears stale route data',()=>{
  const run=setup();run.state.worldMode=true;run.state.worldSnapshot=world(5,456);run.syncGameplayFrameUI();
  run.state.worldSnapshot=world(10,0);assert.equal(run.syncGameplayFrameUI().frame,0);assert.equal(run.gameplayFrameDetails().route,'B2-3');
  run.state.worldMode=false;assert.equal(run.syncGameplayFrameUI().route,null);assert.doesNotMatch(run.nodes['#game-frame-indicator'].textContent,/B2-3|456/);
});

test('existing actual render, native fault and title paths are connected to the frame UI',()=>{
  assert.match(functionSource('enterFaultMenu'),/captureGameplayFrameUI\(\)/);
  assert.match(functionSource('faultOverlay'),/syncGameplayFrameUI\(\)/);
  assert.match(functionSource('returnToTitle'),/syncGameplayFrameUI\(\)/);
  assert.match(source,/state\.worldSnapshot=world;\s*syncDebugRespawnUI\(world\);\s*syncGameplayFrameUI\(world\)/);
});
