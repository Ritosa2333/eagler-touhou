import {createCoopRecorder} from './coop-replay.mjs';
import {createSpectatorPublisher} from './spectator.mjs';
import {applyDisplay} from './display.mjs';
import {createManagedTransport} from './managed-transport.mjs';

const roles=['host','guest','guest2'];
const integer=(value,min,max)=>Number.isSafeInteger(value)&&value>=min&&value<=max;
export function multiplayerOptions(value){
 const count=value.netplayPlayerCount,local=value.netplayPlayer;
 if(value.netplayMode!=='lan'||![2,3].includes(count)||(value.netplaySpectator===true?![null,0].includes(local)||!/^[-A-Za-z0-9_]{8,64}$/.test(value.netplaySpectatorId??''):!integer(local,0,count-1)))throw Error('Invalid TH05 multiplayer seats');
 const url=new URL(value.netplayUrl);
 const localDevelopment=url.protocol==='ws:'&&['127.0.0.1','localhost','[::1]'].includes(url.hostname);
 if(!['ws:','wss:'].includes(url.protocol)||!['room','run','member'].every(key=>url.searchParams.get(key))||!localDevelopment&&!/^[a-f0-9]{64}$/.test(url.searchParams.get('runtimeGeneration')??''))throw Error('Missing TH05 room run identity');
 if(!integer(value.netplaySeed,0,65535)||!integer(value.netplayDifficulty,0,4))throw Error('Invalid TH05 multiplayer settings');
 if(value.netplayInitialSpares!==undefined&&!integer(value.netplayInitialSpares,0,5)||value.netplayInitialBombs!==undefined&&!integer(value.netplayInitialBombs,0,3))throw Error('Invalid TH05 initial stocks');
 if(!Array.isArray(value.netplayLoadouts)||value.netplayLoadouts.length!==count||value.netplayLoadouts.some(p=>!integer(p?.character,0,3)||p.shot!==0))throw Error('Invalid TH05 multiplayer characters');
 const mode=value.netplayAdonisMode??1;
 // The room advertises the title's capacity even when prediction is disabled.
 // Delay-only runs may also carry zero from older exact-input clients.
 if(!integer(mode,0,2)||value.netplayPredictionLimit!==undefined&&!(mode===1?[0,8]:[8]).includes(value.netplayPredictionLimit))throw Error('Invalid TH05 rollback timing');
 if(value.netplayPredictionReserve!==undefined&&!integer(value.netplayPredictionReserve,1,2))throw Error('Invalid TH05 prediction reserve');
 if(value.netplayChallengeMode!==undefined&&typeof value.netplayChallengeMode!=='boolean')throw Error('Invalid TH05 challenge mode');
 if(value.netplayInputDelay!==undefined&&!integer(value.netplayInputDelay,0,9))throw Error('Invalid TH05 input delay');
 if(value.netplayInputDelayAuto!==undefined&&typeof value.netplayInputDelayAuto!=='boolean')throw Error('Invalid TH05 measured timing');
 return value;
}

// The authenticated transport carries immutable physical samples. The native
// shared RollbackCore owns prediction, correction and confirmed output commits.
export function createMultiplayer(options,{module,sample,saveReplay,emit=()=>{},fail,now=()=>performance.now(),transportFactory=createManagedTransport,setTimer=setTimeout,clearTimer=clearTimeout}){
 multiplayerOptions(options);
 const count=options.netplayPlayerCount,local=options.netplayPlayer,url=new URL(options.netplayUrl);
 const mode=options.netplayAdonisMode??1,limit=mode===1?0:8;
 const reserve=options.netplayPredictionReserve??2;
 const identity=JSON.stringify({protocol:'th05-native-coop/2',mode,limit,run:url.searchParams.get('run'),generation:url.searchParams.get('runtimeGeneration'),count,challenge:options.netplayChallengeMode===true,spares:options.netplayInitialSpares??2,bombs:options.netplayInitialBombs??3,seed:options.netplaySeed,difficulty:options.netplayDifficulty,characters:options.netplayLoadouts.map(p=>p.character)});
 const rows=Array.from({length:count},()=>new Map()),peers=new Map(),proposals=new Map(),acknowledged=new Set(),checks=new Map();
 let publisher,recorder;
 function record(action){
  if(!recorder)return;
  try{action(recorder);}catch(error){recorder=null;emit('runtime-info',{replaySaveError:error.message});}
 }
 let transport,started=false,closed=false,ending=false,frame=0,delay=0,lastProgress=now(),lastWarning=0,proposalTimer,deadline,probeTimer,resolveStart,rejectStart,endTask,nextCheck=120;
 const startTask=new Promise((resolve,reject)=>{resolveStart=resolve;rejectStart=reject;});
 // A rejection can precede the caller awaiting start (e.g. early transport
 // failure); retain it without generating an unhandled rejection.
 startTask.catch(()=>{});
 const fatal=reason=>{if(closed)return;record(r=>r.stop());closed=true;clearTimer(deadline);clearTimer(proposalTimer);clearTimer(probeTimer);transport?.close();module._th05_net_close();rejectStart(reason);fail(reason);};
 const send=(peer,message)=>peer.channel.send(JSON.stringify(message));
 const broadcast=message=>{for(const peer of peers.values())send(peer,message);};
 const guarded=fn=>(...args)=>{if(closed)return;try{return fn(...args);}catch(error){fatal(error);}};
 function negotiate(){
  if(local!==0||proposals.size!==count)return;
  delay=Math.max(...proposals.values());broadcast({type:'timing',delay});acknowledged.add(local);maybeStart();
 }
 function proposal(){
  if(proposals.has(local)||[...peers.values()].some(p=>!p.hello))return;
  const rtt=Math.max(0,...[...peers.values()].map(p=>Math.max(0,...p.samples)));
  const measured=Math.min(9,Math.max(1,Math.ceil(rtt*56/1000)+1));
  const requested=mode===0?0:options.netplayInputDelayAuto===false?(options.netplayInputDelay??0):mode===2?Math.max(0,measured-reserve):measured;
  proposals.set(local,requested);broadcast({type:'proposal',delay:requested});negotiate();
 }
 function maybeStart(){
  if(local!==0||acknowledged.size!==count)return;
  broadcast({type:'start',delay});begin();
 }
 function begin(){
  if(started)throw Error('Duplicate TH05 start');
  const chars=options.netplayLoadouts.map(p=>p.character);while(chars.length<3)chars.push(0);
  if(!module._th05_coop_start_stock(count,local,options.netplayDifficulty,options.netplaySeed,...chars,+(options.netplayChallengeMode===true),+(options.multiplayerLocalPlayerVisibility===true),options.netplayInitialSpares??2,options.netplayInitialBombs??3))throw Error('TH05 cooperative world could not start');
  applyDisplay(module,options.th05MultiplayerDisplay);
  if(!module._th05_net_reset(delay,limit))throw Error('TH05 rollback session could not start');
  for(let i=0;i<delay;i++)for(const row of rows)row.set(i,[0,0,0,0]);
  for(let seat=0;seat<count;seat++)if(seat!==local)for(const [inputFrame,input] of rows[seat])if(inputFrame>=delay&&!module._th05_net_input(seat,inputFrame,...input))throw Error('TH05 bootstrap input rejected');
  if(local===0&&typeof module._th05_spectator_serial==='function'){publisher=createSpectatorPublisher(module,transport,count,options.netplaySeed,rows,emit);publisher.begin();}
  if(saveReplay&&options.netplayReplayAutoSave!==false){
   recorder=createCoopRecorder({module,rows,count,save:saveReplay});record(r=>r.begin());
  }
  started=true;clearTimer(deadline);clearTimer(probeTimer);lastProgress=now();
  globalThis.__eaglerNetplaySpectator=false;globalThis.__eaglerNetplayLanActive=true;globalThis.__eaglerNetplayInputDelay=globalThis.__eaglerNetplayLanInputDelay=delay;
  globalThis.__eaglerNetplayRollback=globalThis.__eaglerNetplayLanRollback=0;globalThis.__eaglerNetplayResimulated=globalThis.__eaglerNetplayLanResimulated=0;
  emit('netplay-status',{state:'running',players:count,inputDelay:delay});resolveStart();
 }
 function receive(peer,message){
  if(!message||typeof message!=='object')throw Error('Invalid TH05 peer message');
  if(message.type==='hello'){
   if(peer.hello||message.identity!==identity)throw Error('TH05 run settings differ');peer.hello=true;
   if([...peers.values()].every(p=>p.hello)&&!proposalTimer)proposalTimer=setTimer(guarded(proposal),450);
   return;
  }
  if(!peer.hello)throw Error('TH05 peer identity is not confirmed');
  switch(message.type){
   case 'ping':if(!integer(message.serial,0,20))throw Error('Invalid ping');send(peer,{type:'pong',serial:message.serial});break;
   case 'pong':{const sent=peer.probes.get(message.serial);if(sent!==undefined){peer.probes.delete(message.serial);peer.samples.push(now()-sent);}break;}
   case 'proposal':if(started||proposals.has(peer.id)||!integer(message.delay,0,9))throw Error('Invalid timing proposal');proposals.set(peer.id,message.delay);negotiate();break;
   case 'timing':
    if(peer.id!==0||local===0||started||!integer(message.delay,proposals.get(local)??0,9))throw Error('Invalid TH05 timing decision');
    delay=message.delay;send(peer,{type:'timing-ack',delay});break;
   case 'timing-ack':if(local!==0||message.delay!==delay||acknowledged.has(peer.id))throw Error('Invalid timing acknowledgement');acknowledged.add(peer.id);maybeStart();break;
   case 'start':if(peer.id!==0||local===0||message.delay!==delay)throw Error('Invalid TH05 start');begin();break;
   case 'input':{
    if(!integer(message.frame,Math.max(0,frame-64),frame+32)||!Array.isArray(message.input)||message.input.length!==4)throw Error('TH05 input is outside the frame window');
    const [buttons,x,y,unlimited]=message.input;
    if(!integer(buttons,0,65535)||![x,y].every(v=>Number.isFinite(v)&&Math.abs(v)<=8192&&Number.isInteger(v*256))||!integer(unlimited,0,3))throw Error('Invalid TH05 frame input');
    const row=rows[peer.id],old=row.get(message.frame);if(old&&JSON.stringify(old)!==JSON.stringify(message.input))throw Error('Conflicting TH05 frame input');
    row.set(message.frame,message.input);
    if(started&&!module._th05_net_input(peer.id,message.frame,...message.input))throw Error('TH05 remote input rejected');break;
   }
   case 'check':{
    if(!integer(message.frame,Math.max(0,frame-480),frame+32)||message.frame%120||!integer(message.hash,0,0xffffffff))throw Error('Invalid TH05 state check');
    const record=checks.get(message.frame)??new Map();record.set(peer.id,message.hash);checks.set(message.frame,record);checkState(record);break;
   }
   case 'end':void end();break;
   default:throw Error('Unknown TH05 peer message');
  }
 }
 function checkState(record){
  if(record.has(local)&&[...record.values()].some(hash=>hash!==record.get(local)))throw Error('TH05 multiplayer state diverged');
 }
 function probe(serial=0){
  if(started||closed)return;
  for(const peer of peers.values())if(peer.hello){peer.probes.set(serial,now());send(peer,{type:'ping',serial});}
  if(serial<6)probeTimer=setTimer(guarded(()=>probe(serial+1)),55);
 }
 function end(){
  if(endTask)return endTask;if(closed)return Promise.resolve();ending=true;record(r=>r.stop());publisher?.end();broadcast({type:'end'});
  endTask=(async()=>{try{await transport.finish();close();emit('netplay-ended');}catch(error){fatal(error);throw error;}})();endTask.catch(()=>{});return endTask;
 }

 function close(){
  if(closed)return;record(r=>r.stop());closed=true;clearTimer(deadline);clearTimer(proposalTimer);clearTimer(probeTimer);transport.close();
  module._th05_net_close();globalThis.__eaglerNetplayLanActive=false;for(const row of rows)row.clear();
 }
 transport=transportFactory(options,roles,fatal);
 for(let id=0;id<count;id++)if(id!==local){
  const peer={id,channel:transport.channelFor(roles[id]),hello:false,probes:new Map(),samples:[]};peers.set(id,peer);
  peer.channel.onopen=guarded(()=>{send(peer,{type:'hello',identity});if([...peers.values()].every(p=>p.channel.readyState==='open'))probeTimer=setTimer(guarded(()=>probe()),20);});
  peer.channel.onmessage=guarded(event=>receive(peer,JSON.parse(event.data)));
 }
 return {
  start(){deadline=setTimer(()=>fatal(Error('TH05 multiplayer start timed out')),45000);transport.connect();return startTask;},
  advance(){
   if(!started||closed||ending)return false;
   const future=frame+delay;
   if(!rows[local].has(future)){
    const raw=sample(),input=[raw[0]&65535,...raw.slice(1,3).map(v=>Math.round(Math.max(-8192,Math.min(8192,v))*256)/256),integer(raw[3],0,3)?raw[3]:0];
    rows[local].set(future,input);if(!module._th05_net_input(local,future,...input))throw Error('TH05 local capture rejected');broadcast({type:'input',frame:future,input});
   }
   const advanced=module._th05_net_advance();if(advanced<0)throw Error('TH05 rollback simulation failed');
   const confirmed=module._th05_net_confirmed();
   record(r=>r.flush(confirmed));
   publisher?.flush(confirmed);
   globalThis.__eaglerNetplayConfirmed=globalThis.__eaglerNetplayLanConfirmed=confirmed;
   globalThis.__eaglerNetplayRollback=globalThis.__eaglerNetplayLanRollback=module._th05_net_stat(0);
   globalThis.__eaglerNetplayResimulated=globalThis.__eaglerNetplayLanResimulated=module._th05_net_stat(1);
   while(confirmed>=nextCheck-1&&frame+Number(!!advanced)>=nextCheck){
    const hash=module._th05_net_hash(nextCheck)>>>0,record=checks.get(nextCheck)??new Map();record.set(local,hash);checks.set(nextCheck,record);checkState(record);broadcast({type:'check',frame:nextCheck,hash});nextCheck+=120;
   }
   for(const old of checks.keys())if(old<frame-480)checks.delete(old);
   // The frontier advances one frame at a time. Retire its single expired row
   // without walking every seat's 64-frame ledger on each browser callback.
   if(frame>64)for(const row of rows)row.delete(frame-65);
   if(!advanced){
    const wait=now()-lastProgress;if(wait>3000&&now()-lastWarning>1000){emit('netplay-status',{state:'waiting',frame,waitMs:wait});lastWarning=now();}
    if(wait>15000)fatal(Error('TH05 multiplayer input timed out'));return false;
   }
   ++frame;lastProgress=now();globalThis.__eaglerNetplayFrame=globalThis.__eaglerNetplayLanFrame=frame;
   if(module._th05_coop_finished())void end();return true;
  },
  end,close,get frame(){return frame;},get delay(){return delay;},get confirmed(){return module._th05_net_confirmed();},get revision(){return module._th05_net_stat(8);},get recovering(){return module._th05_net_stat(5)<frame;},get active(){return started&&!closed&&!ending;},
 };
}
