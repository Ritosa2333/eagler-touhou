// Shared Launcher options -> title-owned admission. This module does not drive
// gameplay, resample controls, implement signaling or own a room lifecycle.
export function validateMultiplayerOptions(value={}) {
 if(value.netplayMode!=='lan')return null;
 const url=new URL(value.netplayUrl),room=url.searchParams.get('room'),run=url.searchParams.get('run');
 if(!['ws:','wss:'].includes(url.protocol)||url.username||url.password||!room||!/^\d+$/.test(run||'')||Number(run)<1)
  throw Error('Invalid TH08 multiplayer room/run URL');
 const count=value.netplayPlayerCount,seat=value.netplayPlayer;
 if(![2,3].includes(count)||!Number.isInteger(seat)||seat<0||seat>=count)
  throw Error('Invalid TH08 multiplayer seat');
 if(!Number.isInteger(value.netplaySeed)||value.netplaySeed<0||value.netplaySeed>65535||
    !Number.isInteger(value.netplayDifficulty)||value.netplayDifficulty<0||value.netplayDifficulty>4)
  throw Error('Invalid TH08 multiplayer seed/difficulty');
 if(!Array.isArray(value.netplayLoadouts)||value.netplayLoadouts.length!==count)
  throw Error('Invalid TH08 multiplayer loadout count');
 const loadouts=value.netplayLoadouts.map(v=>{
  if(!v||!Number.isInteger(v.character)||v.character<0||v.character>11||v.shot!==0)
   throw Error('Invalid TH08 multiplayer loadout');
  return {character:v.character,shot:0};
 });
 if(value.netplaySpectator)throw Error('TH08 spectator admission is not connected in this build');
 return {url,room,run,count,seat,seed:value.netplaySeed,difficulty:value.netplayDifficulty,loadouts};
}
function string(core,text,fn){
 const bytes=new TextEncoder().encode(text+'\0'),pointer=core.allocate(bytes.length);
 if(!pointer)throw Error('Multiplayer string allocation failed');
 try{new Uint8Array(core.memory.buffer,pointer,bytes.length).set(bytes);return fn(pointer);}
 finally{core.deallocate(pointer);}
}
export async function configureMultiplayer(core,app,options,{crypto=globalThis.crypto}={}) {
 const o=validateMultiplayerOptions(options);if(!o)return false;
 if(!app||typeof core.multiplayer_configure!=='function'||typeof core.multiplayer_connect!=='function')
  throw Error('TH08 multiplayer Runtime capability is missing');
 const identity=new TextEncoder().encode(`th08mp:${o.url.origin}${o.url.pathname}:${o.room}:${o.run}`);
 const digest=new DataView(await crypto.subtle.digest('SHA-256',identity));
 const words=[2,o.count,o.seat,o.difficulty,o.seed,digest.getUint32(0,true),digest.getUint32(4,true)||1];
 for(let seat=0;seat<3;++seat)words.push(o.loadouts[seat]?.character||0,0);
 const pointer=core.allocate(words.length*4);if(!pointer)throw Error('Multiplayer setup allocation failed');
 try{
  new Uint32Array(core.memory.buffer,pointer,words.length).set(words);
  if(!core.multiplayer_configure(app,pointer,words.length))throw Error('TH08 native admission rejected the room configuration');
 }finally{core.deallocate(pointer);}
 if(!string(core,o.url.href,p=>core.multiplayer_connect(app,p)))throw Error('TH08 native transport rejected the room');
 return true;
}
export function networkError(core,app){
 if(!app||!core.multiplayer_network_error)return '';
 const pointer=core.multiplayer_network_error(app),bytes=new Uint8Array(core.memory.buffer);
 let end=pointer;while(end<bytes.length&&end-pointer<1024&&bytes[end])++end;
 return new TextDecoder().decode(bytes.subarray(pointer,end));
}
export function updateNetworkDiagnostics(core,app,target=globalThis) {
 if(!app||!core.multiplayer_netplay_status)return;
 const n=core.multiplayer_netplay_status(app),state=Array.from(new Uint32Array(core.memory.buffer,n,12));
 const p=core.multiplayer_driver_status(app),driver=Array.from(new Uint32Array(core.memory.buffer,p,16));
 const invalid=0xffffffff,frame=state[4]===invalid?-1:state[4];
 target.__eaglerNetplayLanActive=!!state[2];
 target.__eaglerNetplayLanFrame=frame;
 target.__eaglerNetplayLanConfirmed=state[5]===invalid?undefined:state[5];
 target.__eaglerNetplayLanRollback=driver[3];target.__eaglerNetplayLanResimulated=driver[4];
 if(target.__eaglerNetplayGeneration!==state[10]){
  target.__eaglerNetplayLanHashes=Object.create(null);target.__eaglerNetplayHashFrame=-1;
 }
 target.__eaglerNetplayGeneration=state[10];
 target.__eaglerNetplayTransport=driver[12]===1?'rtc':driver[12]===2?'relay':'';
 if(driver[2]){target.__eaglerNetplayFailed=true;target.__eaglerNetplayError=networkError(core,app);}
 // Read-only debugging follows a bounded sample; no hash participates in
 // frame admission or masks a native desynchronization.
 if(frame>=0&&frame%60===59&&target.__eaglerNetplayHashFrame!==frame&&state[6]===invalid&&state[5]>=state[4]&&core.multiplayer_canonical_state){
  const c=core.multiplayer_canonical_state(app),words=new Uint32Array(core.memory.buffer,c,13);
  const hashes=target.__eaglerNetplayLanHashes||(target.__eaglerNetplayLanHashes=Object.create(null));
  if(words[0]===1){target.__eaglerNetplayHashFrame=frame;hashes[String(frame)]=String(words[1]);
   for(const key of Object.keys(hashes))if(Number(key)<frame-512)delete hashes[key];}
 }
}
