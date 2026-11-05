import {PeerSocket} from './peer-socket.mjs';
import {RollbackSession} from './rollback-session.mjs';
import {nativeRollbackCore} from './native-adapter.mjs';
// TH09's shared lobby supplies seats, characters and the authenticated relay URL.
// TH19 advances its existing VS simulation only after both inputs are confirmed.
const magic=[0x54,0x31,0x39,0x4e,1];
const packet=(type,length=6)=>{const bytes=new Uint8Array(length);bytes.set(magic);bytes[5]=type;return bytes;};
const keyBits={KeyZ:1,Enter:1,NumpadEnter:1,KeyX:2,KeyC:4,ShiftLeft:8,ShiftRight:8,
 ArrowUp:16,ArrowDown:32,ArrowLeft:64,ArrowRight:128,Escape:1024};
const neutral=()=>({keys:0,direct:0,x:0,y:0});

export class Netplay {
 constructor(core,{onStatus,onClose,onResult}){
  this.core=core;this.onStatus=onStatus;this.onClose=onClose;this.onResult=onResult;
  this.connected=false;this.active=false;this.finished=false;this.prepared=false;
  this.keys=new Set();this.motionX=0;this.motionY=0;this.direct=false;
  this.local=new Map();this.remote=new Map();this.hashes=new Map();this.remoteHashes=new Map();
  this.baselineRttMs=undefined;this.stepCostMs=0;this.frame=0;this.timer=null;this.socket=null;this.peerReady=false;this.peerFinished=false;
 }
 key(code,down){if(down)this.keys.add(code);else this.keys.delete(code);}
 clear(){this.keys.clear();this.motionX=this.motionY=0;}
 motion(x,y,direct){this.direct=!!direct;this.motionX+=x;this.motionY+=y;}
 sample(){
  let keys=0;for(const code of this.keys)keys|=keyBits[code]||0;
  const pad=globalThis.__eaglerGamepadInputBridge?.sample(0)??-1;
  if(pad>=0){const actions=[1,4,8,1024,16,32,64,128,0,1,2,0];for(let n=0;n<actions.length;n++)if(pad&(1<<n))keys|=actions[n];}
  if(this.core._th19_touch_sample){const p=this.core._th19_touch_sample(this.side),words=new Uint32Array(this.core.HEAPU8.buffer,p,2),motion=new Float32Array(this.core.HEAPU8.buffer,p+8,2);return {keys:keys|words[0],direct:words[1],x:motion[0],y:motion[1]};}
  const limit=value=>Math.fround(Math.max(-512,Math.min(512,value)));
  // A saved touch preference must not suppress the keyboard. Only a frame
  // containing an actual drag uses direct movement instead of direction keys.
  const direct=this.direct&&(this.motionX!==0||this.motionY!==0);
  const input={keys,direct:+direct,x:limit(this.motionX),y:limit(this.motionY)};
  this.motionX=this.motionY=0;return input;
 }
 async connect(options){
  if(this.connected)throw Error('已经连接房间');
  const url=new URL(options.netplayUrl,location.href),side=Number(options.netplayPlayer);
  const characters=options.netplayLoadouts?.slice(0,2).map(loadout=>loadout.character);
  if(!['ws:','wss:'].includes(url.protocol)||![0,1].includes(side)||options.netplaySpectator||
   Number(options.netplayPlayerCount)!==2||!/^th19mp-\d{4}$/.test(url.searchParams.get('room')||'')||
   Number(url.searchParams.get('player'))!==side||!/^\d+$/.test(url.searchParams.get('run')||'')||
   !characters||characters.length!==2||characters.some(ch=>!Number.isInteger(ch)||ch<0||ch>=19))
   throw Error('兽王园双人房间配置无效');
  const difficulty=Number(options.netplayDifficulty),delay=Number(options.netplayInputDelay||0);
  if(!Number.isInteger(difficulty)||difficulty<0||difficulty>3||!Number.isInteger(delay)||delay<0||delay>8)
   throw Error('兽王园房间难度或输入延迟无效');
  const response=await fetch(new URL('manifest.json',import.meta.url));
  if(!response.ok)throw Error('联机版本信息读取失败');
  const manifest=await response.json(),build=manifest.buildId;
  if(manifest.game!=='th19'||manifest.protocol!=='eagler-touhou/1'||!/^[a-f0-9]{64}$/.test(build))throw Error('联机版本信息无效');
  const mode=Number(options.netplayAdonisMode||0);if(![0,1,2].includes(mode))throw Error('兽王园回滚选项无效');
  this.rollbackEnabled=mode!==1;
  // The shared run serial gives each rematch a fresh, identical seed on both seats.
  const seed=((options.netplaySeed>>>0)^Math.imul(Number(url.searchParams.get('run')),0x9e3779b9))>>>0;
  const rules=options.netplayTh19Rules??[0,1,1,4,4,1,1,0,0];const v=rules;if(!(Array.isArray(v)&&v.length===9&&v.every((n,i)=>Number.isInteger(n)&&n>=0&&n<([7,3,4,5,5,4,4,39,39][i]))&&v[0]===0&&v[3]===v[4]&&v[5]===1&&v[6]===1&&(v[2]!==2||(v[7]<2&&v[8]<2))))throw Error("对战规则无效");
  this.config={rules,protocol:1,build,seed:seed||1,difficulty,characters,delay,rollback:this.rollbackEnabled};
  this.core.eaglerOptions=options;
  this.side=side;this.core._th19_touch_side?.(side);this.delay=delay;this.direct=options.touchEnabled===true&&options.touchMovementMode==='touch-unlimited';
  this.connected=true;this.lastReceived=performance.now();this.lastHello=0;this.lastPing=0;
  this.socket=new PeerSocket(url,options);this.socket.binaryType='arraybuffer';
  globalThis.__th19PeerTransport={relay:this.socket,peers:new Map(),failed:false};
  globalThis.__eaglerNetplayTransport='relay';
  this.socket.onopen=()=>this.onStatus('已连接中继，等待对手…');
  this.socket.onmessage=event=>{
   try{
    if(typeof event.data==='string'){
     const control=JSON.parse(event.data);
     if(control.type!=='route'||!['rtc','relay'].includes(control.mode))throw Error('联机中继路线无效');
     this.routeReady=true;globalThis.__eaglerNetplayTransport=control.mode;globalThis.__eaglerNetplayPath=control.mode;this.sendHello();return;
    }
    if(!(event.data instanceof ArrayBuffer))throw Error('联机数据格式错误');this.receive(new Uint8Array(event.data));
   }
   catch(error){this.close(error.message);}
  };
  this.socket.onerror=event=>this.close(event.error?.message||'无法连接联机服务器');
  this.socket.onclose=()=>{if(this.connected)this.close('对手已断开，已返回房间');};
  this.onStatus('正在连接对手并核对兽王园版本…');
  this.timer=setInterval(()=>{try{this.pump();}catch(error){this.close(error.message);}},8);
 }
 send(bytes){
  if(!this.connected||this.socket.readyState!==WebSocket.OPEN)return;
  if(this.socket.bufferedAmount>256*1024)throw Error('联机数据积压，请重新进入房间');
  this.socket.send(bytes);
 }
 sendHello(){
  const text=new TextEncoder().encode(JSON.stringify(this.config)),bytes=packet(1,text.length+6);
  bytes.set(text,6);this.send(bytes);this.lastHello=performance.now();
 }
 receive(bytes){
  if(bytes.length<6||bytes.length>2048||magic.some((v,i)=>bytes[i]!==v))throw Error('联机协议不一致');
  this.lastReceived=performance.now();const data=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  switch(bytes[5]){
   case 1:{
    const config=JSON.parse(new TextDecoder().decode(bytes.subarray(6)));
    if(JSON.stringify(config)!==JSON.stringify(this.config))throw Error('双方游戏版本或房间设置不同，请刷新后重试');
    if(!this.prepared){
     const c=this.config;
     if(!this.core._th19_network_set_rules(...c.rules)||!this.core._th19_network_begin(c.seed,c.difficulty,...c.characters))throw Error('无法启动人间 vs 人间对战');
     this.prepared=true;
     if(this.rollbackEnabled)this.prepareRollback();
     for(let i=0;i<this.delay;++i){this.local.set(i,neutral());this.remote.set(i,neutral());}
    }
    this.send(packet(2));this.activate();break;
   }
   case 2:if(bytes.length!==6)throw Error('联机握手格式错误');this.peerReady=true;this.activate();break;
   case 3:{
    if(!this.prepared||bytes.length!==21)throw Error('联机输入格式错误');
    const frame=data.getUint32(6,true),keys=data.getUint16(10,true),direct=bytes[12];
    const x=data.getFloat32(13,true),y=data.getFloat32(17,true);
    if(frame>this.frame+300||keys>2047||direct>1||!Number.isFinite(x)||!Number.isFinite(y)||Math.abs(x)>512||Math.abs(y)>512)
     throw Error('对手输入无效');
    if(this.rollback){this.rollback.receive(frame,{keys,direct,x,y});break;}
    if(frame<this.frame)return;
    const input={keys,direct,x,y},old=this.remote.get(frame);
    if(old&&JSON.stringify(old)!==JSON.stringify(input))throw Error('对手重复发送了不同的输入');
    this.remote.set(frame,input);break;
   }
   case 4:{
    if(bytes.length!==14)throw Error('联机校验格式错误');
    const frame=data.getUint32(6,true);if(frame>this.frame+300)throw Error('联机校验帧无效');
    this.remoteHashes.set(frame,data.getUint32(10,true));this.compare(frame);break;
   }
   case 5:if(bytes.length!==6)throw Error('结算信息无效');this.peerFinished=true;this.complete();break;
   case 6:if(bytes.length!==14)throw Error('联机延迟信息无效');bytes[5]=7;this.send(bytes);break;
   case 7:{
    if(bytes.length!==14)throw Error('联机延迟信息无效');
    const rtt=performance.now()-data.getFloat64(6,true);
    if(Number.isFinite(rtt)&&rtt>=0&&rtt<30000){this.baselineRttMs=this.baselineRttMs===undefined?rtt:Math.min(this.baselineRttMs,rtt);const peer=globalThis.__th19PeerTransport?.peers.get(1-this.side);if(peer)peer.rttMs=rtt;}
    break;
   }
   default:throw Error('不支持的联机消息');
  }
 }
 activate(){
  if(this.active||!this.prepared||!this.peerReady)return;
  this.active=true;this.lastTick=performance.now();this.budget=0;
  this.inputBudget=0;this.nextInputFrame=this.delay;this.lastPing=-Infinity;
  globalThis.__th19PeerTransport.peers.set(1-this.side,{inputOpen:true,controlOpen:true,rttMs:null});
  globalThis.__eaglerNetplayLanActive=true;
  globalThis.__eaglerNetplayLanFrame=0;
  globalThis.__eaglerNetplayLanConfirmed=0;
  globalThis.__eaglerNetplayLanRollback=0;
  globalThis.__eaglerNetplayLanResimulated=0;
  globalThis.__eaglerNetplayInputDelayFrames=this.delay;
  globalThis.__eaglerNetplayPath=this.socket.route||'relay';
  this.onStatus(`对局开始 · 人间 vs 人间 · 输入延迟 ${this.delay} 帧 · 回滚${this.rollbackEnabled?'开启':'关闭'}`);
 }
 pump(){
  if(!this.connected)return;
  const now=performance.now();
  if(now-this.lastReceived>(this.active?30000:180000))throw Error('等待对手超时，已返回房间');
  if(this.socket.readyState!==WebSocket.OPEN||!this.routeReady)return;
  if(!this.active&&now-this.lastHello>500)this.sendHello();
  if(now-this.lastPing>2000){const bytes=packet(6,14);new DataView(bytes.buffer).setFloat64(6,now,true);this.send(bytes);this.lastPing=now;}
  if(!this.active||this.finished)return;
  if(this.rollback){this.pumpRollback(now);return;}
  const elapsed=(now-this.lastTick)*60/1000;this.lastTick=now;
  this.budget=Math.min(8,this.budget+elapsed);
  this.inputBudget=Math.min(8,this.inputBudget+elapsed);
  // Keep sending on the input clock while a delayed remote frame blocks the
  // simulation. Otherwise each stall also starves the peer's input pipeline.
  // Bound queued input so a suspended peer cannot accumulate seconds of input.
  const latestRtt=globalThis.__th19PeerTransport?.peers.get(1-this.side)?.rttMs||0;
  // A ping includes time stuck behind rendering on either peer. One long
  // frame must not inflate every subsequent input by another second. Keep
  // the baseline for this fixed WebSocket route, including sustained GPU load.
  // Allow ordinary jitter to fill the pipeline, but bound the contribution
  // from main-thread stalls to 100ms above the best measured route latency.
  const peerRtt=Math.min(latestRtt,(this.baselineRttMs??latestRtt)+100);
  const capacityHz=Math.min(60,1000/Math.max(1,this.stepCostMs));
  const maxInputLead=Math.min(60,Math.max(2,Math.ceil(this.delay*capacityHz/60)+2,Math.ceil(peerRtt*capacityHz/2000)+2));
  while(this.inputBudget>=1&&this.nextInputFrame<this.frame+maxInputLead){
    const sending=this.nextInputFrame++;
    const input=this.sample();this.local.set(sending,input);
    const bytes=packet(3,21),data=new DataView(bytes.buffer);
    data.setUint32(6,sending,true);data.setUint16(10,input.keys,true);bytes[12]=input.direct;
    data.setFloat32(13,input.x,true);data.setFloat32(17,input.y,true);this.send(bytes);
    --this.inputBudget;
  }
  // Discard sample-clock debt at the lead limit: old wall-clock slots must
  // not turn into a burst of stale input after a slow peer resumes.
  if(this.nextInputFrame>=this.frame+maxInputLead)this.inputBudget=0;
  let steps=0;
  while(steps<Math.floor(this.budget)&&this.local.has(this.frame+steps)&&this.remote.has(this.frame+steps))++steps;
  const workStart=performance.now();
  for(let count=0;count<steps;++count){
   const own=this.local.get(this.frame),other=this.remote.get(this.frame);
   const [left,right]=this.side===0?[own,other]:[other,own];
   const present=count===steps-1;
   const result=this.core._th19_network_step(this.frame,left.keys,right.keys,left.x,left.y,right.x,right.y,left.direct,right.direct,+present);
   if(!result)throw Error('对局输入帧不连续');
   this.local.delete(this.frame);this.remote.delete(this.frame);++this.frame;--this.budget;
   globalThis.__eaglerNetplayLanFrame=globalThis.__eaglerNetplayLanConfirmed=this.frame;
   if(this.frame%120===0){
    const hash=this.core._th19_network_hash()>>>0;this.hashes.set(this.frame,hash);
    const bytes=packet(4,14),data=new DataView(bytes.buffer);
    data.setUint32(6,this.frame,true);data.setUint32(10,hash,true);this.send(bytes);this.compare(this.frame);
    for(const frame of this.hashes.keys())if(frame<this.frame-1200)this.hashes.delete(frame);
   }
   if(result===2){this.finished=true;this.send(packet(5));this.complete();break;}
  }
  if(steps){const cost=(performance.now()-workStart)/steps;this.stepCostMs=this.stepCostMs?this.stepCostMs*.8+cost*.2:cost;}
  globalThis.__th19NetplayPerformance={queuedFrames:this.nextInputFrame-this.frame,stepCostMs:this.stepCostMs,rttMs:latestRtt,baselineRttMs:this.baselineRttMs??latestRtt,maxInputLead};
 }
 prepareRollback(){
  this.rollbackCore=nativeRollbackCore(this.core,{present:()=>this.rollbackPresent!==false});
  this.rollback=new RollbackSession(this.rollbackCore,{side:this.side,window:8,
   commit:(frame,effects)=>{
    this.rollbackCore.commit(effects);const confirmed=frame+1;
    if(effects.hash!==null){
     this.hashes.set(confirmed,effects.hash);const bytes=packet(4,14),data=new DataView(bytes.buffer);
     data.setUint32(6,confirmed,true);data.setUint32(10,effects.hash,true);this.send(bytes);this.compare(confirmed);
     for(const f of this.hashes.keys())if(f<confirmed-1200)this.hashes.delete(f);
    }
    if(effects.result===2&&!this.finished){this.finished=true;this.send(packet(5));this.complete();}
   }});
  for(let frame=0;frame<this.delay;frame++){
   this.rollback.local.set(frame,neutral());this.rollback.receive(frame,neutral());
  }
 }
 pumpRollback(now){
  const r=this.rollback,elapsed=(now-this.lastTick)*60/1000;this.lastTick=now;
  this.budget=Math.min(3,this.budget+elapsed);
  // Receive input immediately, but combine corrections until the next 60 Hz
  // simulation tick. Replaying on every 8 ms transport poll repeatedly rebuilt
  // states that could not yet be presented, blocking the next input/render turn.
  if(this.budget<1)return;
  const start=performance.now();r.reconcile();
  let advanced=0,presented=0;
  while(this.budget>=1&&!this.finished){
   const sending=r.frame+this.delay;
   if(!r.local.has(sending)){
    const input=this.sample();r.local.set(sending,input);
    const bytes=packet(3,21),data=new DataView(bytes.buffer);
    data.setUint32(6,sending,true);data.setUint16(10,input.keys,true);bytes[12]=input.direct;
    data.setFloat32(13,input.x,true);data.setFloat32(17,input.y,true);this.send(bytes);
   }
   // Catch-up ticks retain original simulation/ANM updates but only the
   // newest available tick needs to build and submit a visible frame.
   const next=r.frame+1;
   const safe=this.core._th19_rollback_can_predict((r.local.get(r.frame)?.keys||0)|(r.remote.get(r.frame)?.keys||0));
   const canFollow=next<r.remoteEnd||(safe&&next-r.confirmed<r.window);
   this.rollbackPresent=this.budget<2||!canFollow;
   if(!r.advance(r.local.get(r.frame))){this.budget=Math.min(this.budget,1);break;}
   --this.budget;++advanced;if(this.rollbackPresent)++presented;
   if(this.rollbackPresent&&performance.now()-start>=8)break;
  }

  this.frame=r.frame;
  globalThis.__eaglerNetplayLanFrame=r.frame;globalThis.__eaglerNetplayLanConfirmed=r.confirmed;
  globalThis.__eaglerNetplayLanRollback=r.rollbacks;globalThis.__eaglerNetplayLanResimulated=r.resimulated;
  globalThis.__th19NetplayPerformance={rollback:true,predictedFrames:r.frame-r.confirmed,
   retainedSnapshots:r.snapshots.size,advancedFrames:advanced,presentedFrames:presented,pumpCostMs:performance.now()-start};
 }
 compare(frame){
  if(!this.hashes.has(frame)||!this.remoteHashes.has(frame))return;
  if(this.hashes.get(frame)!==this.remoteHashes.get(frame))throw Error(`对局校验不同步（第 ${frame} 帧），请重新开局`);
  this.remoteHashes.delete(frame);
 }
 complete(){if(this.finished&&this.peerFinished&&!this.completed){this.completed=true;this.onResult();}}
 close(reason=''){
  if(!this.connected)return;
  this.connected=false;this.active=false;clearInterval(this.timer);this.timer=null;
  this.socket?.close();this.clear();
  this.rollback?.close();this.rollback=null;
  if(this.prepared)this.core._th19_network_end();
  globalThis.__eaglerNetplayLanActive=false;
  if(reason)this.onClose(reason);
 }
}


