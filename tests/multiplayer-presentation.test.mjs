import test from 'node:test';
import assert from 'node:assert/strict';
import {applyMultiplayerOpacity,multiplayerOpacity,readMultiplayerVisualOwners} from '../src/multiplayer-presentation.mjs';
import {createMultiplayerRuntime} from '../src/multiplayer.mjs';
import {prepareLocalization} from '../src/localization/loader.mjs';
import {messageTextPoses,Renderer} from '../src/renderer.mjs';
import {copyMeshPose} from '../src/mesh-vertices.mjs';
import {th12MessagePatch} from '../../../launcher/server/th12-thcrap-compiler.mjs';
import {createStaticThcrapPack} from '../../../launcher/server/thcrap-static-pack.mjs';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {isLocalizationResourcePath} from '../src/localization/loader.mjs';

test('remote body, shots, options and custom bomb meshes fade without changing authority',()=>{
 const players=[{seat:0,x:0,y:200},{seat:1,x:20,y:200},{seat:2,x:100,y:200}],world={players};
 const owners=new Map([[0x10000001,1],[0xa0000001,2]]),vertex={x:1,y:2,color:0x80ffffff},before=structuredClone(world);
 const poses=[{owner:0xffff0000,alpha:1},{owner:0xffff0101,alpha:.5},{owner:0x10000001,alpha:1},
  {owner:0xa0000001,mesh:true,vertices:[vertex]},{owner:0x123,alpha:1},{owner:0xa0000001,beamTrail:true,start:0xffffffff,end:0x80ffffff}];
 applyMultiplayerOpacity(poses,world,0,owners);
 assert.equal(poses[0].alpha,1);assert.equal(poses[1].alpha,.5*72/255);assert.equal(poses[2].alpha,72/255);
 assert.equal(poses[3].vertices[0].color>>>24,Math.round(128*160/255));assert.equal(vertex.color,0x80ffffff);
 assert.equal(poses[4].alpha,1);assert.equal(poses[5].start>>>24,160);assert.equal(poses[5].end>>>24,80);
 assert.deepEqual(world,before);assert.equal(multiplayerOpacity(players[2],players[0]),160/255);
 assert.equal(multiplayerOpacity({...players[1],ghost:true},players[0]),88/255);
 const view=[{owner:0xffff0000,alpha:1}];applyMultiplayerOpacity(view,world,-1);assert.equal(view[0].alpha,160/255);
 const core={HEAPU8:new Uint8Array(64),_th12_mp_visual_owners_count:()=>2,_th12_mp_visual_owners:()=>8};
 new Uint32Array(core.HEAPU8.buffer,8,4).set([0x10000001,1,0xa0000001,2]);
 assert.deepEqual(readMultiplayerVisualOwners(core),owners);
});

test('remote mesh opacity stays on the contiguous-buffer draw path without expanding vertices',()=>{
 const floats=new Float32Array(14),colors=new Uint32Array(floats.buffer);colors[4]=0x80ffffff;colors[11]=0xffffffff;
 floats[0]=10;floats[1]=20;floats[3]=1;floats[7]=30;floats[8]=40;floats[10]=1;
 const pose=copyMeshPose({mesh:true,owner:0xa0000001,vertexData:{floats,colors,offset:0,count:2},primitive:4,untextured:true,a:{blend:0}});
 Object.defineProperty(pose,'vertices',{get(){throw Error('flat meshes must not allocate vertex objects');}});
 const before=colors.slice();applyMultiplayerOpacity([pose],{players:[{seat:0,x:0,y:200},{seat:1,x:100,y:200}]},0,new Map([[pose.owner,1]]));
 const vertices=[],target={solid:{},textures:new Map(),gl:{TRIANGLES:4,LINE_STRIP:3},material(){},texture(){},primitive(){},batch:{push(...v){vertices.push(v);}},flush(){}};
 Renderer.prototype.mesh.call(target,pose);
 assert.equal(vertices[0][7],Math.round(128*160/255)/255);assert.equal(vertices[1][7],160/255);
 assert.deepEqual(colors,before);assert.equal(vertices[0][0],10);assert.equal(vertices[1][1],40);
});

const command=(op,payload=Buffer.alloc(0))=>Buffer.concat([Buffer.from([0,0,op,payload.length]),payload]);
const original=()=>{
 const text=Buffer.from('old\0');let key=0x77,step=7;
 for(let i=0;i<text.length;i++){text[i]^=key;key=(key+step)&255;step=(step+16)&255;}
 const header=Buffer.alloc(12);header.writeUInt32LE(1);header.writeUInt32LE(12,4);
 return Buffer.concat([header,command(17,text),command(0)]);
};
test('guest uses the host dialogue route and renders patched Chinese; diagnostics do not serialize the world hash',async()=>{
 const result=th12MessagePatch(original(),{'0':{'0_0':{lines:['老鼠的中文对话']}}});
 const overlay={schema:'th12-localization-overlay/2',game:'th12',language:'lang_zh-hans',sourceKind:'thcrap',sourceExeSha256:'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417',
  dialogue:{'st01_00a.msg':result.lookup},dialoguePatches:{'st01_00a.msg':result.patch},endings:{},endingPatches:{},spells:{},textures:{},musicComments:{}};
 const bytes=Buffer.from(JSON.stringify(overlay)),pack=createStaticThcrapPack({pack:{game:'th12',language:'lang_zh-hans',assets:[]},resources:[{targetPath:'/thcrap/th12/overlay.json',bytes}]});
 const localization=await prepareLocalization({language:'lang_zh-hans',manifest:pack.manifest,readFile:()=>bytes});
 // Launcher configures once for room preload, then again before launch. Use
 // the actual adapter handler so replacing an active provider is exercised.
 const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
 const start=source.indexOf('configure:async settings=>{')+'configure:'.length;
 const end=source.indexOf('\n    },',start)+6;
 const adapter={localization,assets:{fonts:new Map([['/unifont.otf',new Uint8Array()]])},prepareLocalization,isLocalizationResourcePath,
  installRuntimeFonts:async()=>{},touch:{configure(){}},runtimeOptions:{},runtimeMusic:null};
 const configure=runInNewContext('('+source.slice(start,end)+')',adapter);
 const settings={language:'lang_zh-hans',runtimePack:{manifest:pack.manifest,files:[{path:'/thcrap/th12/overlay.json',bytes}]},options:{},music:'none'};
 localization.activate();await configure(settings);await configure(settings);
 adapter.localization.setContext({stage:1,character:0,shot:0});
 const first=Object.values(result.lookup)[0];
 assert.equal(messageTextPoses({entry:0,active:1,textRecords:[{textHex:first.sourceHex,offset:0,spacing:0}]},[],{text:{sprites:[{w:354,h:17}]}})[0].text,'老鼠的中文对话');
 const loadouts=[{character:0,shot:0},{character:2,shot:1}],loaded=[];
 const options={netplayMode:'lan',netplayUrl:'ws://localhost/?room=test&run=1',netplayPlayerCount:2,netplayPlayer:1,netplaySeed:1,netplayDifficulty:1,netplayLoadouts:loadouts,netplayAdonisMode:1,netplayPredictionLimit:0};
 const heap=new Uint8Array(8192),status=new Uint32Array(heap.buffer,2048,24);status[2]=1;status[9]=1;
 const state=new TextEncoder().encode(JSON.stringify({players:loadouts.map((p,seat)=>({...p,seat}))})+'\0');heap.set(state,4096);
 let hashes=0;
 const core={HEAPU8:heap,_malloc:()=>256,_free(){},_th12_mp_configure:()=>1,_th12_mp_connect:()=>1,_th12_mp_start:()=>1,_th12_mp_stop(){},_th12_mp_calibration_status:()=>1024,
  _th12_mp_status:()=>2048,_th12_mp_state:()=>4096,_th12_mp_hash:()=>{hashes++;return 1;},_th12_mp_step:()=>1,_th12_mp_stage_needed:()=>0,_th12_mp_sounds_count:()=>0,_th12_mp_sounds:()=>0,_th12_mp_sounds_clear(){}};
 const runtime=createMultiplayerRuntime({core,manifest:{game:'th12',product:'th12mp',variant:'multiplayer',profile:'multiplayer',features:{multiplayer:true},execution:{sha256:'1'.repeat(64)}},getOptions:()=>options,emit(){},
  loadStage:async(stage,local,story)=>{loaded.push({local,story});localization.setContext({stage,...story});},onError:e=>{throw e;}});
 localization.activate();
 try{
  await runtime.launch();assert.deepEqual(loaded,[{local:loadouts[1],story:loadouts[0]}]);
  const r=result.patch.entries[0].anchors.flatMap(a=>a.texts)[0];
  const poses=messageTextPoses({active:1,entry:0,textRecords:[{textHex:r.sourceHex,offset:0,spacing:0,style:0,color:0xffffff}]},[],{text:{sprites:[{w:568,h:32}]}},{logicalText:false});
  assert.equal(poses[0].text,'老鼠的中文对话');
  runtime.tick();assert.equal(hashes,0,'routine runtime-info never hashes the complete world');
 }finally{runtime.stop();localization.deactivate();}
});
