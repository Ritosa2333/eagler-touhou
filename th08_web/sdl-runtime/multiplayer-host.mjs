// Shared Launcher options -> title-owned admission. This module does not drive
// gameplay, resample controls, implement signaling or own a room lifecycle.
export function validateMultiplayerOptions(value={}) {
 if(value.netplayMode!=='lan')return null;
 const url=new URL(value.netplayUrl),room=url.searchParams.get('room'),run=url.searchParams.get('run');
 if(!['ws:','wss:'].includes(url.protocol)||url.username||url.password||!room||!/^\d+$/.test(run||'')||Number(run)<1)
  throw Error('Invalid TH08 multiplayer room/run URL');
 const spectator=value.netplaySpectator===true,spectatorId=String(value.netplaySpectatorId||'');
 if(spectator&&!/^[A-Za-z0-9_-]{8,64}$/.test(spectatorId))throw Error('Invalid TH08 spectator admission');
 const count=value.netplayPlayerCount,seat=spectator?0:value.netplayPlayer;
 if(![2,3].includes(count)||!Number.isInteger(seat)||seat<0||seat>=count)
  throw Error('Invalid TH08 multiplayer seat');
 if(!Number.isInteger(value.netplaySeed)||value.netplaySeed<0||value.netplaySeed>65535||
    !Number.isInteger(value.netplayDifficulty)||value.netplayDifficulty<0||value.netplayDifficulty>4)
  throw Error('Invalid TH08 multiplayer seed/difficulty');
 const inputDelay=value.netplayInputDelay===undefined?0:value.netplayInputDelay;
 const adonisMode=value.netplayAdonisMode===undefined?0:value.netplayAdonisMode;
 const predictionLimit=value.netplayPredictionLimit===undefined?8:value.netplayPredictionLimit;
 if(!Number.isInteger(adonisMode)||adonisMode<0||adonisMode>2||
    !Number.isInteger(inputDelay)||inputDelay<0||inputDelay>(adonisMode?9:8)||
    !Number.isInteger(predictionLimit)||predictionLimit<1||predictionLimit>8)
  throw Error('Invalid TH08 multiplayer input timing');
 if(!Array.isArray(value.netplayLoadouts)||value.netplayLoadouts.length!==count)
  throw Error('Invalid TH08 multiplayer loadout count');
 const loadouts=value.netplayLoadouts.map(v=>{
  if(!v||!Number.isInteger(v.character)||v.character<0||v.character>11||v.shot!==0)
   throw Error('Invalid TH08 multiplayer loadout');
  return {character:v.character,shot:0};
 });
 return {url,room,run,count,seat,spectator,spectatorId,seed:value.netplaySeed,difficulty:value.netplayDifficulty,
  inputDelay,predictionLimit,adonisMode,loadouts};
}
function string(core,text,fn){
 const bytes=new TextEncoder().encode(text+'\0'),pointer=core.allocate(bytes.length);
 if(!pointer)throw Error('Multiplayer string allocation failed');
 try{new Uint8Array(core.memory.buffer,pointer,bytes.length).set(bytes);return fn(pointer);}
 finally{core.deallocate(pointer);}
}
export async function configureMultiplayer(core,app,options,{crypto=globalThis.crypto,runtimeBuildWords}={}) {
 const o=validateMultiplayerOptions(options);if(!o)return false;
 if(!app||typeof core.multiplayer_configure!=='function'||
    typeof core[o.spectator?'multiplayer_spectator_connect':'multiplayer_connect']!=='function')
  throw Error('TH08 multiplayer Runtime capability is missing');
 if(!Array.isArray(runtimeBuildWords)||runtimeBuildWords.length!==4||
    runtimeBuildWords.some(value=>!Number.isInteger(value)||value<0||value>0xffffffff)||
    !runtimeBuildWords.some(Boolean))throw Error('TH08 multiplayer Runtime build identity is missing');
 const identity=new TextEncoder().encode(`th08mp:${o.url.origin}${o.url.pathname}:${o.room}:${o.run}`);
 const digest=new DataView(await crypto.subtle.digest('SHA-256',identity));
 const words=[o.adonisMode?5:4,o.count,o.seat,o.difficulty,o.seed,digest.getUint32(0,true),digest.getUint32(4,true)||1,...runtimeBuildWords];
 for(let seat=0;seat<3;++seat)words.push(o.loadouts[seat]?.character||0,0);
 words.push(o.inputDelay,o.predictionLimit);
 if(o.adonisMode)words.push(o.adonisMode);
 const pointer=core.allocate(words.length*4);if(!pointer)throw Error('Multiplayer setup allocation failed');
 try{
  new Uint32Array(core.memory.buffer,pointer,words.length).set(words);
  if(!core.multiplayer_configure(app,pointer,words.length))throw Error('TH08 native admission rejected the room configuration');
 }finally{core.deallocate(pointer);}
 if(!string(core,o.url.href,p=>o.spectator?
    string(core,o.spectatorId,id=>core.multiplayer_spectator_connect(app,p,id)):
    core.multiplayer_connect(app,p)))throw Error('TH08 native transport rejected the room');
 globalThis.__eaglerNetplayInputDelayFrames=o.spectator?0:o.inputDelay;
 globalThis.__eaglerNetplayAdonisMode=o.adonisMode;
 return true;
}
export function networkError(core,app){
 if(!app||!core.multiplayer_network_error)return '';
 const pointer=core.multiplayer_network_error(app),bytes=new Uint8Array(core.memory.buffer);
 if(!Number.isInteger(pointer)||pointer<=0||pointer>=bytes.length)return '';
 let end=pointer;while(end<bytes.length&&end-pointer<1024&&bytes[end])++end;
 return new TextDecoder().decode(bytes.subarray(pointer,end));
}
export function updateNetworkDiagnostics(core,app,target=globalThis) {
 if(!app||!core.multiplayer_netplay_status)return;
 const n=core.multiplayer_netplay_status(app),state=Array.from(new Uint32Array(core.memory.buffer,n,12));
 if(state[0]>=2)target.__eaglerNetplayInputDelayFrames=new Uint32Array(core.memory.buffer,n,14)[12];
 if(state[0]>=3)target.__eaglerNetplayAdonisMode=new Uint32Array(core.memory.buffer,n,15)[14];
 const p=core.multiplayer_driver_status(app),driver=Array.from(new Uint32Array(core.memory.buffer,p,16));
 const invalid=0xffffffff,frame=state[4]===invalid?-1:state[4];
 target.__eaglerNetplayLanActive=!!state[2]&&!driver[2];
 target.__eaglerNetplayLanFrame=frame;
 target.__eaglerNetplayLanConfirmed=state[5]===invalid?undefined:state[5];
 target.__eaglerNetplayLanRollback=driver[3];target.__eaglerNetplayLanResimulated=driver[4];
 target.__eaglerNetplayGeneration=state[10];
 target.__eaglerNetplayTransport=driver[12]===1?'rtc':driver[12]===2?'relay':driver[12]===3?'spectator':'';
 target.__eaglerNetplayPacketsSent=driver[13];target.__eaglerNetplayPacketsReceived=driver[14];
 target.__eaglerNetplayChannelError=driver[15];
 if(core.multiplayer_spectator_status){
  const p=core.multiplayer_spectator_status(app),s=Array.from(new Uint32Array(core.memory.buffer,p,8));
  target.__eaglerNetplaySpectator=!!s[1];target.__eaglerNetplaySpectatorFinished=!!s[2];
  target.__eaglerNetplaySpectatorBacklog=s[3];
  if(s[2])target.__eaglerNetplayLanActive=false;
 }
 if(driver[2]){target.__eaglerNetplayFailed=true;target.__eaglerNetplayError=networkError(core,app);}
}
