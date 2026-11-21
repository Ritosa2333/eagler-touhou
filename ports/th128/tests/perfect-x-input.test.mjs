import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const source=fs.readFileSync(path.join(root,'site/play.mjs'),'utf8');

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

const names=['supportsPerfectFreezeX','gameplayXHeld','gameplayXPress','gameplayXRelease','gameplayInputMask','worldMovementInput'];
const factory=new Function('state','flowCapabilities','menu','movementKeys','status','perfectFreezeXBit',
  `${names.map(functionSource).join('\n')}\nreturn {${names.join(',')}};`);

function runtime(capability=true){
  const calls=[];
  const state={
    worldMode:true,shotMode:true,active:true,paused:false,handoff:false,pressed:new Set(),
    module:{
      _th128_world_input(holding,pressed,released){calls.push([holding,pressed,released]);return 1;},
      UTF8ToString(value){return value;},
    },
  };
  const menu={fault:false,pauseOpen:false};
  const flowCapabilities=()=>capability?{perfectFreezeNormalEmptyLasers:true}:{supportedBulletTypes:[]};
  return {state,menu,api:factory(state,flowCapabilities,menu,new Map(),()=>{},2),calls};
}

test('capability false keeps X out of old-core input and diagnostics stay 761',()=>{
  const run=runtime(false);
  assert.equal(run.api.supportsPerfectFreezeX(),false);
  assert.equal(run.api.gameplayInputMask(),761);
  run.state.pressed.add('KeyX');
  assert.equal(run.api.gameplayXHeld(),false);
  assert.equal(run.api.gameplayXPress(),false);
  assert.deepEqual(run.calls,[]);
  run.api.worldMovementInput(2,0);
  assert.deepEqual(run.calls,[[0,0,0]]);
  assert.ok(run.calls.every(call=>call.every(value=>(value&2)===0)),'unsupported X bit reached native input');
});

test('capability true sends one press edge, holds bit2, and releases on keyup',()=>{
  const run=runtime(true);
  assert.equal(run.api.gameplayInputMask(),763);
  run.state.pressed.add('KeyX');
  assert.equal(run.api.gameplayXPress(),true);
  assert.deepEqual(run.calls,[[2,2,0]]);
  run.api.worldMovementInput(0,0);
  assert.deepEqual(run.calls,[[2,2,0],[2,0,0]],'held X must not emit a second press edge');
  run.state.pressed.delete('KeyX');
  assert.equal(run.api.gameplayXRelease(),true);
  assert.deepEqual(run.calls,[[2,2,0],[2,0,0],[0,0,2]]);
});

test('menu X context makes no native input call even with the capability',()=>{
  const run=runtime(true);
  run.state.worldMode=false;
  run.state.active=false;
  run.state.pressed.add('KeyX');
  assert.equal(run.api.gameplayXPress(),false);
  assert.equal(run.api.gameplayXRelease(),false);
  run.api.worldMovementInput(2,0);
  assert.deepEqual(run.calls,[]);
});
