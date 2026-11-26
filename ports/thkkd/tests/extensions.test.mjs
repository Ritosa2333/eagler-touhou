import assert from 'node:assert/strict';
import {test} from 'node:test';
import {Input} from '../web/input.mjs';
import {nativeReplay,decodeReplay,encodeExtended} from '../web/replay.mjs';
import {PresentationClock} from '../web/presentation-clock.mjs';
import {Platform} from '../web/platform.mjs';
import {Renderer} from '../web/renderer.mjs';
import {PresentationStats} from '../web/presentation-stats.mjs';
Object.defineProperty(globalThis,'navigator',{configurable:true,value:{getGamepads:()=>[]}});
const makeInput=()=>new Input({protocol:'eagler-touhou/1',game:'thkkd',epoch:1,origin:'http://localhost'},()=>true);
test('呈现 FPS 由真实提交间隔计算，挂起恢复重新计量',()=>{
 for(const hz of [30,60,120,144]){const stats=new PresentationStats();for(let i=0;i<=hz*2;i++)stats.presented(i*1000/hz);assert.ok(Math.abs(stats.fps-hz)<1e-8);assert.equal(stats.total,hz*2+1);stats.presented(5000);assert.equal(stats.fps,0);stats.presented(5000+1000/60);assert.ok(Math.abs(stats.fps-60)<1e-8);}
});
test('触控关闭的自动射击消息不持有 Z，菜单确认与游戏射击保留物理边沿',()=>{
 const input=makeInput();let gameplay=false;input.isGameplay=()=>gameplay;input.configure({touchEnabled:false});
 const controls={command:'touch-controls',fireEnabled:true,focusEnabled:true,bombSerial:0,escapeSerial:0,joystickX:1,joystickY:1,touchSensitivity:150};input.command(controls);
 let keys=new Uint8Array(256);input.sample(keys);for(const code of [44,42,205,208])assert.equal(keys[code],0);
 input.keyboard.event({code:'KeyZ',key:'z',keyCode:90},true);keys.fill(0);input.sample(keys);assert.equal(keys[44],1);
 input.keyboard.event({code:'KeyZ',key:'z',keyCode:90},false);keys.fill(0);input.sample(keys);assert.equal(keys[44],0);
 input.configure({touchEnabled:true});input.command(controls);keys.fill(0);input.sample(keys);assert.equal(keys[44],0);
 gameplay=true;keys.fill(0);input.sample(keys);assert.equal(keys[44],1);gameplay=false;keys.fill(0);input.sample(keys);assert.equal(keys[44],0);
});
test('额外呈现读取身份快照，出生、切面、瞬移和池复用保持当前状态',()=>{
 let submitted;const r=Object.create(Renderer.prototype),texture={texture:1};
 r.gl=new Proxy({bufferData(_target,data){submitted=Array.from(data);}},{get:(object,key)=>key in object?object[key]:/^[A-Z_]+$/.test(String(key))?1:()=>{}});r.commands=[];r.current=[];r.vertices=new Float32Array(60*256);
 const command=(id,x)=>({texture,identity:id,points:[x,0,x+1,0,x+1,1,x,1],uv:[0,0,1,0,1,1,0,1],tint:[1,1,1,1],depth:[1,.5,1,.5,1,.5,1,.5],mode:1,area:[0,0,640,480]});
 const draw=(id,x,tick,scene='100:0')=>{r.current=[command(id,x)];r.tick=tick;r.scene=scene;r.commit();const before=JSON.stringify(r.commands);r.present(.5);assert.equal(JSON.stringify(r.commands),before);return submitted[0];};
 assert.equal(draw('slot:birth1',0,0),0);assert.equal(draw('slot:birth1',8,1),12);assert.equal(draw('slot:birth2',16,2),16);assert.equal(draw('slot:birth2',200,3),200);assert.equal(draw('slot:birth2',208,4,'100:1'),208);
});
test('手柄语义输入按原生绑定生成位图，确认、设备释放和键盘归属独立',()=>{
 const input=makeInput(),bridge=globalThis.__eaglerGamepadInputBridge;input.isGameplay=()=>true;let bits=(1<<0)|(1<<7);
 globalThis.__eaglerGamepadInputBridge={sample:()=>bits};
 try{
  const keys=new Uint8Array(256);input.sample(keys);assert.equal(input.padState(),(1<<5)|(1<<2));assert.equal(keys[44],0);
  input.key('KeyZ',true,'keyboard');bits=1<<9;input.sample(keys);assert.equal(input.padState(),0);assert.equal(keys[44],1);
  bits=1;input.sample(keys);input.bindings=()=>[1,2,3,0,9,4,11,13,6];assert.equal(input.padState(),1<<9);
  bits=0;input.sample(keys);assert.equal(input.padState(),0);assert.equal(keys[44],1);
  input.bindings=undefined;input.isGameplay=()=>false;bits=1;input.pulse('KeyZ');keys.fill(0);input.sample(keys);assert.equal(input.padState(),0);assert.equal(keys[44],0);keys.fill(0);input.sample(keys);assert.equal(input.padState(),1<<5);assert.equal(keys[44],1);
  input.replaying=true;bits=1;keys.fill(0);input.sample(keys);assert.equal(input.padState(),0);assert.equal(keys[44],0);
 }finally{globalThis.__eaglerGamepadInputBridge=bridge;}
});
test('对话将新确认边沿和长按跳过接入原录像推进位，进入对话时持续射击不误确认',()=>{
 const input=makeInput(),keys=new Uint8Array(256);let dialogue=false;input.isGameplay=()=>!dialogue;input.isDialogue=()=>dialogue;
 input.key('KeyZ',true,'keyboard');input.sample(keys);dialogue=true;keys.fill(0);input.sample(keys);assert.equal(input.padState()&(1<<6),0);
 input.key('KeyZ',false,'keyboard');keys.fill(0);input.sample(keys);input.key('KeyZ',true,'keyboard');keys.fill(0);input.sample(keys);assert.equal(input.padState()&(1<<6),1<<6);keys.fill(0);input.sample(keys);assert.equal(input.padState()&(1<<6),0);
 input.key('KeyZ',false,'keyboard');input.key('ControlLeft',true,'keyboard');const skip=[];for(let i=0;i<4;i++){keys.fill(0);input.sample(keys);assert.equal(keys[29],0);skip.push(!!(input.padState()&(1<<6)));}assert.deepEqual(skip,[true,false,true,false]);
 dialogue=false;keys.fill(0);input.sample(keys);assert.equal(keys[29],1);input.clear();assert.equal(input.padState(),0);
});
test('公共键盘 Enter 和独立手柄确认可以完成菜单确认',()=>{
 const input=makeInput(),bridge=globalThis.__eaglerGamepadInputBridge,keys=new Uint8Array(256);input.isGameplay=()=>false;let bits=1;
 globalThis.__eaglerGamepadInputBridge={sample:()=>bits};
 try{input.sample(keys);bits|=1<<9;keys.fill(0);input.sample(keys);assert.equal(input.padState()&(1<<5),0);keys.fill(0);input.sample(keys);assert.equal(input.padState()&(1<<5),1<<5);input.clear();bits=0;input.key('Enter',true,'keyboard');keys.fill(0);input.sample(keys);assert.equal(keys[44],1);}finally{globalThis.__eaglerGamepadInputBridge=bridge;}
});
test('视频和文字纹理按 GPU 身份回收，保留共享素材纹理',()=>{
 const shared={texture:1},videoOwner={texture:2},text={texture:3},deleted=[];
 const renderer=Object.assign(Object.create(Renderer.prototype),{textures:new Set([shared,videoOwner,text]),gl:{deleteTexture:texture=>deleted.push(texture)}});
 renderer.releaseTexture({...videoOwner});renderer.releaseTexture(text);assert.deepEqual([...renderer.textures],[shared]);assert.deepEqual(deleted,[2,3]);
});
test('菜单双指轻触返回，保留单指确认与双指低速',()=>{
 const input=makeInput();input.isGameplay=()=>false;input.configure({touchEnabled:true,touchFocusMode:'two-finger'});
 for(const id of [1,2])input.command({command:'direct-touch',type:'down',id,x:.2,y:.3});
 let keys=new Uint8Array(256);input.sample(keys);assert.equal(keys[42],1);
 for(const id of [1,2])input.command({command:'direct-touch',type:'up',id,x:.2,y:.3});keys.fill(0);input.sample(keys);assert.equal(keys[45],1);assert.equal(keys[44],0);
});
test('方向及自由摇杆、按钮序列、双击 Bomb 和灵敏度共同进入逻辑输入',()=>{
 const input=makeInput();input.isGameplay=()=>true;input.configure({touchEnabled:true,touchMovementMode:'joystick'});
 const controls={command:'touch-controls',fireEnabled:true,focusEnabled:true,bombSerial:0,escapeSerial:0,joystickX:1,joystickY:-1,touchSensitivity:100};input.command(controls);
 let keys=new Uint8Array(256);input.sample(keys);for(const scan of [44,42,205,200])assert.equal(keys[scan],1);
 input.command({...controls,bombSerial:1,escapeSerial:1});keys.fill(0);input.sample(keys);assert.equal(keys[45],1);assert.equal(keys[1],1);
 input.clear();input.configure({touchEnabled:true,touchMovementMode:'joystick-free'});input.command({...controls,joystickX:.3,joystickY:.4});const state={x:100,y:300,oldX:100,oldY:300,speed:4,cursor:1};let position=input.motion(state);assert.ok(Math.abs(position.x-101.2)<1e-12&&Math.abs(position.y-301.6)<1e-12);
 input.clear();input.configure({touchEnabled:true,touchMovementMode:'touch-unlimited',doubleTapBombEnabled:true});input.player={x:100,y:300};
 for(const type of ['down','up','down'])input.command({command:'direct-touch',type,id:1,x:.2,y:.3});keys.fill(0);input.sample(keys);assert.equal(keys[45],1);
 for(const sensitivity of [100,200,300]){input.clear();input.player={x:100,y:300};input.configure({touchEnabled:true,touchMovementMode:'touch-unlimited',touchSensitivity:sensitivity});input.command({command:'direct-touch',type:'down',id:1,x:.2,y:.3});input.command({command:'direct-touch',type:'move',id:1,x:.21,y:.3});position=input.motion(state);assert.ok(Math.abs(position.x-(100+6.4*sensitivity/100))<1e-10);}
});
test('音乐等待期间音效节点立即输出并独立释放',()=>{
 let starts=0,stops=0;const platform=Object.create(Platform.prototype);
 platform.music=true;platform.emit=()=>{};platform.context={currentTime:1,createBufferSource:()=>({connect(){},disconnect(){},start(){starts++;},stop(){stops++;}})};
 const bgm={bgm:true,audio:{play:()=>new Promise(()=>{}),currentTime:0}},sfx={buffer:{duration:1},gain:{}};
 platform.playSound(bgm,2,1);platform.playSound(sfx,0,1);assert.equal(starts,1);assert.equal(sfx.playing,true);platform.stopSound(sfx);assert.equal(stops,1);assert.equal(bgm.playing,true);
});
test('主动暂停使过期媒体播放请求失效，实际播放失败仍报告给启动器',async()=>{
 const events=[],pending=[],platform=Object.create(Platform.prototype),audio={currentTime:0,paused:false,play:()=>new Promise((resolve,reject)=>pending.push({resolve,reject})),pause(){this.paused=true;}};
 platform.emit=(event,value)=>events.push({event,...value});platform.music=true;platform.context={state:'running',suspend:async()=>{},resume:async()=>{}};platform.sounds=new Map();platform.graphs=new Map();const sound={audio,bgm:true,playing:true};platform.sounds.set(1,sound);
 platform.playMedia(sound);await platform.suspend();pending[0].reject(Object.assign(Error('play interrupted by pause'),{name:'AbortError'}));await Promise.resolve();assert.equal(events.length,0);
 await platform.resume();pending[1].reject(Object.assign(Error('play blocked'),{name:'NotAllowedError'}));await Promise.resolve();assert.equal(events[0].event,'audio-health');assert.equal(events[0].state,'blocked');
});
test('显示截止时刻在 60、120、144、240 Hz 下保持 60 Hz 呈现',()=>{for(const hz of [60,120,144,240]){const clock=new PresentationClock();let count=0;for(let i=0;i<hz*10;i++)count+=+clock.due(i*1000/hz,true);assert.equal(count,600);}});
test('多来源按键分别持有与释放',()=>{
 const input=makeInput(),keys=new Uint8Array(256);input.key('KeyZ',true,'keyboard');input.key('KeyZ',true,'touch');input.key('KeyZ',false,'touch');input.sample(keys);assert.equal(keys[44],1);input.key('KeyZ',false,'keyboard');keys.fill(0);input.sample(keys);assert.equal(keys[44],0);
});
test('触控限速、暂时禁移与同触点恢复',()=>{
 const input=makeInput();input.isGameplay=()=>true;input.player={x:100,y:300};input.configure({touchEnabled:true,touchMovementMode:'touch',touchSensitivity:200});
 input.command({command:'direct-touch',type:'down',id:1,x:.2,y:.7});input.command({command:'direct-touch',type:'move',id:1,x:.5,y:.5});
 const state={x:100,y:300,oldX:100,oldY:300,speed:4,frame:1,cursor:1,blocked:false,replay:false};
 const movement=input.motion(state);assert.ok(Math.hypot(movement.x-100,movement.y-300)<=4+1e-12);assert.equal(input.unlimitedUsed,false);
 assert.equal(input.motion({...state,blocked:true}),null);assert.equal(input.pointers.size,1);input.command({command:'direct-touch',type:'move',id:1,x:.6,y:.4});assert.ok(input.motion(state));
});
test('无限速标记由实际非零移动触发',()=>{
 const input=makeInput();input.isGameplay=()=>true;input.configure({touchEnabled:true,touchMovementMode:'touch-unlimited'});input.player={x:100,y:300};
 const state={x:100,y:300,oldX:100,oldY:300,speed:4,frame:1,cursor:1,blocked:false,replay:false};assert.equal(input.motion(state),null);assert.equal(input.unlimitedUsed,false);
 input.command({command:'direct-touch',type:'down',id:1,x:.2,y:.7});input.command({command:'direct-touch',type:'move',id:1,x:.5,y:.5});assert.ok(input.motion(state).x>104);assert.equal(input.unlimitedUsed,true);
});
test('菜单确认释放后产生新按下边沿，回放隔离实时输入',()=>{
 const input=makeInput();input.isGameplay=()=>false;input.key('KeyZ',true,'keyboard');input.pulse('KeyZ');const keys=new Uint8Array(256);input.sample(keys);assert.equal(keys[44],0);keys.fill(0);input.sample(keys);assert.equal(keys[44],1);input.replaying=true;keys.fill(0);input.sample(keys);assert.equal(keys[44],0);
});
function fixture(){const bytes=new Uint8Array(3+62),view=new DataView(bytes.buffer,3,62);bytes.set([32,32,0]);view.setUint32(20,3,true);return bytes;}
test('录像先校验原生元数据边界，拒绝无法建立游戏场景的输入',()=>{
 for(const [index,value]of [[0,6],[1,3],[2,3],[3,8],[6,3]]){const bytes=fixture();new DataView(bytes.buffer,3,62).setUint32(index*4,value,true);assert.throws(()=>nativeReplay(bytes),/元数据无效/);}
});
test('原格式保留，扩展格式带版本、移动与内容身份',async()=>{
 const bytes=fixture();assert.equal(nativeReplay(bytes).count,3);assert.deepEqual((await decodeReplay(bytes)).native,bytes);
 const encoded=await encodeExtended(bytes,new Map([[1,{x:128.25,y:320.5}]]),true),decoded=await decodeReplay(encoded);assert.equal(decoded.description.slowdown,100);assert.equal(decoded.motion.get(1).x,128.25);assert.deepEqual(decoded.native,bytes);
 const bad=encoded.slice();bad[bad.length-1]^=1;await assert.rejects(()=>decodeReplay(bad));assert.throws(()=>nativeReplay(bytes.subarray(0,64)));
});
