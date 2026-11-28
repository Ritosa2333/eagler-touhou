import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const site=process.env.TH128_DEBUG_STAGE_SITE?path.resolve(process.env.TH128_DEBUG_STAGE_SITE):path.join(root,'site');
const source=fs.readFileSync(path.join(site,'play.mjs'),'utf8');
const html=fs.readFileSync(path.join(site,'play.html'),'utf8');

function functionSource(name){
  const start=source.indexOf(`function ${name}(`);
  assert.ok(start>=0,`missing production function ${name}`);
  const open=source.indexOf('{',start);
  assert.ok(open>start,`missing production function body ${name}`);
  let depth=0,quote=null,escape=false,lineComment=false,blockComment=false;
  for(let index=open;index<source.length;index++){
    const char=source[index],next=source[index+1];
    if(lineComment){if(char==='\n')lineComment=false;continue;}
    if(blockComment){if(char==='*'&&next==='/'){blockComment=false;++index;}continue;}
    if(quote){
      if(escape){escape=false;continue;}
      if(char==='\\'){escape=true;continue;}
      if(char===quote)quote=null;
      continue;
    }
    if(char==='/'&&next==='/'){lineComment=true;++index;continue;}
    if(char==='/'&&next==='*'){blockComment=true;++index;continue;}
    if(char==='\''||char==='"'||char==='`'){quote=char;continue;}
    if(char==='{')++depth;
    else if(char==='}'&&--depth===0)return source.slice(start,index+1);
  }
  throw new Error(`unterminated production function ${name}`);
}

const names=['debugRespawnSupported','syncDebugRespawnControl','syncDebugRespawnUI','refreshDebugRespawnUIFromWorld','setDebugInfiniteRespawn','applyDebugInfiniteRespawn'];
const factory=new Function('state','find','status','savePreference',
  `${names.map(functionSource).join('\n')}\nreturn {${names.join(',')}};`);

function setup({supported=true,result=1,requested=false}={}){
  const checkbox={checked:false,disabled:false,title:''};
  const note={textContent:''};
  const indicator={hidden:true,textContent:''};
  const calls=[],saved=[],statuses=[];
  const state={module:null,debugInfiniteRespawnRequested:requested,worldMode:false,worldSnapshot:null};
  const module={
    _th128_world_set_debug_infinite_respawn(value){calls.push(value);return result;},
    _th128_world_error(){return 'native failure';},
    _th128_world_state(){return JSON.stringify({debug:{infiniteRespawn:requested,runTainted:false}});},
    UTF8ToString(value){return value;},
  };
  state.module=supported?module:null;
  const find=selector=>({
    '#debug-infinite-respawn':checkbox,
    '#debug-respawn-note':note,
    '#debug-respawn-indicator':indicator,
  })[selector]??null;
  const api=factory(state,find,message=>statuses.push(message),(...args)=>saved.push(args));
  return {checkbox,note,indicator,calls,saved,statuses,state,module,api};
}

test('HTML defaults: checkbox unchecked/disabled and indicator hidden',()=>{
  assert.match(html,/id="debug-infinite-respawn"[^>]*type="checkbox"[^>]*disabled/);
  assert.doesNotMatch(html,/id="debug-infinite-respawn"[^>]*checked/);
  assert.match(html,/id="debug-respawn-indicator"[^>]*hidden/);
  assert.match(html,/开发调试：无限复活（非正常通关）/);
  assert.match(html,/等待新 core：无限复活 API 尚未可用/);
});

test('old core keeps the checkbox disabled and never calls a setter',()=>{
  const run=setup({supported:false});
  run.api.syncDebugRespawnControl();
  assert.equal(run.checkbox.disabled,true);
  assert.equal(run.checkbox.checked,false);
  assert.match(run.note.textContent,/等待新 core/);
  assert.equal(run.api.debugRespawnSupported(),false);
  assert.equal(run.api.setDebugInfiniteRespawn(true),false);
  assert.deepEqual(run.calls,[]);
  assert.equal(run.checkbox.checked,false);
});

test('supported native toggles call the real setter and persist only success',()=>{
  const run=setup();
  run.api.syncDebugRespawnControl();
  assert.equal(run.checkbox.disabled,false);
  assert.equal(run.api.setDebugInfiniteRespawn(true),true);
  assert.deepEqual(run.calls,[1]);
  assert.equal(run.state.debugInfiniteRespawnRequested,true);
  assert.equal(run.checkbox.checked,true);
  assert.deepEqual(run.saved,[['debugInfiniteRespawn','true']]);
  assert.equal(run.api.setDebugInfiniteRespawn(false),true);
  assert.deepEqual(run.calls,[1,0]);
  assert.equal(run.state.debugInfiniteRespawnRequested,false);
  assert.equal(run.checkbox.checked,false);
  assert.deepEqual(run.saved,[['debugInfiniteRespawn','true'],['debugInfiniteRespawn','false']]);

  const restored=setup({requested:true});
  restored.api.syncDebugRespawnControl();
  assert.equal(restored.checkbox.checked,true);
});

test('failed toggle reverts checkbox and reports the native error',()=>{
  const run=setup({result:0,requested:true});
  run.checkbox.checked=true;
  run.api.syncDebugRespawnControl();
  assert.equal(run.api.setDebugInfiniteRespawn(false),false);
  assert.deepEqual(run.calls,[0]);
  assert.equal(run.checkbox.checked,true);
  assert.equal(run.state.debugInfiniteRespawnRequested,true);
  assert.deepEqual(run.saved,[]);
  assert.match(run.statuses.at(-1),/失败.*native failure/);
});

test('apply is one call per new world and rejects failure without a fake success',()=>{
  const run=setup({requested:true});
  run.api.applyDebugInfiniteRespawn(run.module);
  run.api.applyDebugInfiniteRespawn(run.module);
  assert.deepEqual(run.calls,[1,1]);

  const failed=setup({requested:true,result:0});
  assert.throws(()=>failed.api.applyDebugInfiniteRespawn(failed.module),/配置失败.*native failure/);
  assert.equal(failed.state.debugInfiniteRespawnRequested,false);
  assert.equal(failed.checkbox.checked,false);
  assert.deepEqual(failed.calls,[1]);
  assert.deepEqual(failed.saved,[['debugInfiniteRespawn','false']]);

  const start=source.indexOf('_th128_world_start(');
  const apply=source.indexOf('applyDebugInfiniteRespawn(module,selection)',start);
  const tick=source.indexOf('_th128_world_tick(',start);
  assert.ok(start>=0&&apply>start&&tick>apply,'respawn config must apply after world_start before first tick');
  assert.equal((source.match(/applyDebugInfiniteRespawn\(module,selection\)/g)||[]).length,1);
});

test('indicator follows native debug flags, including taint after the switch is off',()=>{
  const run=setup();
  run.state.worldMode=true;
  run.api.syncDebugRespawnUI({debug:{infiniteRespawn:true,runTainted:true}});
  assert.equal(run.indicator.hidden,false);
  assert.equal(run.indicator.textContent,'调试：无限复活');
  run.api.syncDebugRespawnUI({debug:{infiniteRespawn:false,runTainted:true}});
  assert.equal(run.indicator.hidden,false);
  assert.equal(run.indicator.textContent,'本局使用过调试复活');
  run.api.syncDebugRespawnUI({debug:{infiniteRespawn:false,runTainted:false}});
  assert.equal(run.indicator.hidden,true);
  run.api.syncDebugRespawnUI({debug:{infiniteRespawn:true,runTainted:true}});
  run.state.worldMode=false;
  run.api.syncDebugRespawnUI({debug:{infiniteRespawn:true,runTainted:true}});
  assert.equal(run.indicator.hidden,true);
});
