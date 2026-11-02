import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
const retail=process.env.TH12_TEST_RETAIL;
const corePath=process.env.TH12_TEST_CORE;
function command(time,op,payload=Buffer.alloc(0)){const b=Buffer.alloc(4);b.writeUInt16LE(time);b[2]=op;b[3]=payload.length;return Buffer.concat([b,payload]);}
function load(c,bytes,method,index){const p=c._malloc(bytes.length);try{c.HEAPU8.set(bytes,p);assert.equal(index===undefined?c[method](p,bytes.length):c[method](index,p,bytes.length),1);}finally{c._free(p);}}
function read(c,name){const p=c['_th12_'+name](),n=c['_th12_'+name+'_size']();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,p+n)).replace(/\0+$/,''));}
for(const ctrl of [false,true])test(`MSG-owned portraits and text retire before the stage-clear fade (Ctrl ${ctrl})`,{skip:!retail||!corePath?'Set TH12_TEST_RETAIL and TH12_TEST_CORE for retail ANM/Wasm regression':false},async()=>{
 const {default:createCore}=await import(pathToFileURL(path.resolve(corePath)).href),c=await createCore();
 for(const [i,name]of [[0,'bullet'],[1,'enemy'],[2,'stgenm01'],[4,'pl00'],[7,'front'],[8,'text']])load(c,fs.readFileSync(path.join(retail,name+'.anm')),'_th12_load_anm',i);
 const row=JSON.parse(fs.readFileSync(new URL('fixtures/message-enemy-clear-original.json',import.meta.url))).integration[0];
 load(c,Buffer.from(row.syntheticEclHex,'hex'),'_th12_load_ecl');
 const head=Buffer.alloc(12);head.writeUInt32LE(1);head.writeUInt32LE(12,4);head.writeUInt32LE(256,8);
 const wait=Buffer.alloc(4);wait.writeInt32LE(20);
 // Fast teardown deliberately shares the exit commands' timestamp: the
 // public portrait handles have been zeroed while exit VMs remain alive.
 const msg=Buffer.concat([head,command(0,1),command(0,2),command(0,3),command(0,10,Buffer.from([1])),command(0,11,wait),command(100,4),command(100,5),command(100,6),command(100,21),command(100,0)]);
 load(c,msg,'_th12_load_msg',0);c._th12_start(0,0,1,1234);c._th12_set_full_game_mode(0);
 let active=false,finished=false;
 for(let frame=0;frame<200;frame++){
  c._th12_tick(ctrl?512:0,0);const message=read(c,'message');active||=!!message.active;
  if(active&&!message.active){
   const snapshot=read(c,'state');
   assert.equal(snapshot.animations.nodes.some(n=>n.bank===2||(n.bank===4&&n.script>=28)||n.bank===8),false);
   assert.ok(message.animationHandles.every(n=>n===0));finished=true;break;
  }
 }
 assert.ok(finished,'message terminated within the clear period');
});
