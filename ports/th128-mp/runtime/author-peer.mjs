const codec=new TextEncoder(),decode=new TextDecoder('utf-8',{fatal:true});
const packetBytes=4096,incomingBytes=packetBytes+2,spectatorBytes=4136;
const bufferedLimit=64*1024,packetBudget=32;
const symbolNames=['open','close','state','mode','error','buffered','send','poll','spectator_begin','spectator_send'];
let owner=null;

// The separate WASM module links common/src/netplay/BrowserPeerTransport.cpp.
// Its signaling, ICE, channels, close behavior and route selection are the
// author's. This adapter serializes TH128 messages; it never chooses a route.
export function openAuthorPeerLink({module,room,onMessage,signal}){
  if(owner)throw new Error('The shared author browser transport is already owned in this page');
  const cooperative=room?.rules?.mode==='same-stage-coop',players=cooperative?room.rules.players:2;
  if(room?.role!=='player'||![2,3].includes(players)||!Number.isInteger(room.seat)||room.seat<0||room.seat>=players||typeof onMessage!=='function')throw new TypeError('An owned TH128 author player seat is required');
  if(!(module?.HEAPU8 instanceof Uint8Array)||['malloc','free'].some(name=>typeof module[`_${name}`]!=='function')||
     typeof module.UTF8ToString!=='function'||symbolNames.some(name=>typeof module[`_th128_mp_${name}`]!=='function'))
    throw new TypeError('The author browser transport Wasm bridge is incomplete');
  if(cooperative&&typeof module._th128_mp_open_coop!=='function')throw new Error('Cooperative transport needs its sender-aware native bridge');
  signal?.throwIfAborted();
  const url=new URL(room.relayUrl),query=url.searchParams,seat=room.seat;
  if(!['ws:','wss:'].includes(url.protocol)||url.username||url.password||url.hash||
     !/^th128mp-\d{4,8}$/.test(query.get('room')??'')||!/^\d{1,10}$/.test(query.get('run')??'')||
     !/^[A-Za-z0-9_-]{8,64}$/.test(query.get('member')??'')||query.get('player')!==String(seat)||query.get('players')!==String(players)||
     ['spectator','lobby','signal','directory','diagnostic'].some(name=>query.has(name))||
     ['room','run','member','player','players'].some(key=>query.getAll(key).length!==1))throw new Error('Invalid author TH128 gameplay seat URL');
  const urlBytes=codec.encode(url.href+'\0');if(urlBytes.length>2049)throw new Error('Author relay URL exceeds its budget');
  const call=(name,...args)=>module[`_th128_mp_${name}`](...args);
  const describe=()=>module.UTF8ToString(call('error'));
  const identity={};owner=identity;
  let pointer=0,closed=false,pumping=false,attached=false,route='connecting',received=0;
  let generation=-1,spectatorSession=null;
  function close(){
    if(closed)return;closed=true;
    signal?.removeEventListener('abort',close);
    try{if(attached)call('close');}
    finally{try{if(pointer)module._free(pointer);}finally{owner=null;}}
  }
  function ensure(){
    if(closed||owner!==identity)throw new Error('Author player transport is closed');
    signal?.throwIfAborted();
  }
  try{
    pointer=module._malloc(spectatorBytes);if(!pointer)throw new Error('Author transport packet allocation failed');
    module.HEAPU8.set(urlBytes,pointer);
    attached=true;
    if((cooperative?call('open_coop',pointer,seat,players):call('open',pointer,seat))!==1)throw new Error(describe()||'Author transport could not open this seat');
    signal?.addEventListener('abort',close,{once:true});
  }catch(error){close();throw error;}
  const status=()=>Object.freeze({route,received,closed});
  function writable(){
    ensure();if(route==='connecting'||call('state')!==1)throw new Error('Author gameplay route is not ready');
    if(call('buffered')>bufferedLimit)throw new Error('Author transport send budget was exhausted');
  }
  function send(value){
    writable();const bytes=codec.encode(JSON.stringify(value));
    if(bytes.length>packetBytes)throw new Error('Author TH128 gameplay packet exceeds its limit');
    module.HEAPU8.set(bytes,pointer);
    if(call('send',pointer,bytes.length)!==1)throw new Error('Author transport did not accept a gameplay packet');
  }
  function sendSpectator(value){
    writable();if(seat!==0)throw new Error('Only the bound host seat may publish spectator input');
    if(value instanceof Uint8Array){
      if(generation<0||value.length<33||value.length>spectatorBytes+1||value[0]!==0xe8||value[1]!==0x45||value[2]!==0x48)
        throw new Error('Invalid TH128 spectator packet for the author transport');
      const bytes=value.subarray(1);module.HEAPU8.set(bytes,pointer);
      if(call('spectator_send',pointer,bytes.length)!==1)throw new Error('Author spectator lane unavailable');
      return;
    }
    if(value?.type!=='spectator-session'||typeof value.session!=='string'||!(/^[a-f0-9]{16}$/.test(value.session))||
       /^0+$/.test(value.session)||value.session===spectatorSession||!Number.isInteger(value.seed)||value.seed<0||value.seed>65535||
       value.generation!==generation+1)throw new Error('Invalid author spectator session announcement');
    const id=BigInt(`0x${value.session}`);
    if(call('spectator_begin',Number(id&0xffffffffn),Number(id>>32n),value.seed)!==1)throw new Error('Author spectator lane rejected session');
    generation=value.generation;spectatorSession=value.session;
  }
  async function pump(){
    ensure();if(pumping)throw new Error('Author transport poll already running');pumping=true;
    try{
      const state=call('state');
      if(state<0)throw new Error(describe()||'Author player route failed');
      if(state!==1)return status();
      const next=module.UTF8ToString(call('mode'));
      if(!['rtc','relay'].includes(next)||route!=='connecting'&&next!==route)throw new Error('Unexpected author gameplay route');
      route=next;
      for(let step=0;step<packetBudget&&!closed;step++){
        const length=call('poll',pointer,incomingBytes);
        if(!length)break;
        if(length<2||length>incomingBytes)throw new Error('Invalid author gameplay packet size');
        let bytes=module.HEAPU8.slice(pointer,pointer+length);
        let remote=1-seat; // Only the historical unframed two-seat RTC build may infer its single peer.
        if(cooperative||route==='relay'){
          remote=bytes[1];const marker=route==='rtc'?0xec:0xe6;
          if(bytes.length<3||bytes[0]!==marker||remote>=players||remote===seat)throw new Error('Invalid author gameplay sender envelope');
          bytes=bytes.subarray(2);
        }
        if(bytes.length>packetBytes)throw new Error('Author TH128 gameplay packet exceeds its limit');
        const value=JSON.parse(decode.decode(bytes));
        if(!value||Array.isArray(value)||typeof value!=='object')throw new Error('Author TH128 packet is not data');
        received++;await onMessage(value,remote);
      }
      return status();
    }catch(error){close();throw error;}
    finally{pumping=false;}
  }
  return Object.freeze({send,sendSpectator,pump,status,close});
}
