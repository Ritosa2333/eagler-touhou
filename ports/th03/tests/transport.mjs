import assert from 'node:assert/strict';
import {createManagedTransport} from '../build-eagler/managed-transport.mjs';

// Exercise the shipped adapter without a browser. The fake server only routes
// authenticated sender envelopes; no game state or end decisions are invented.
for(const count of [2]){
  const data=new Map(),signal=new Map(),fallback=new Set(),errors=[],roles=['host','guest','guest2'];
  let routed=false;
  function choose(){
    if(routed||fallback.size!==count||data.size!==count)return;routed=true;
    for(const socket of [...data.values(),...signal.values()])socket.onmessage?.({data:JSON.stringify({type:'route',mode:'relay'})});
  }
  class Socket{
    constructor(value){
      this.url=new URL(value);this.player=Number(this.url.searchParams.get('player'));this.signaling=this.url.searchParams.has('signal');this.readyState=0;this.bufferedAmount=0;
      (this.signaling?signal:data).set(this.player,this);
      queueMicrotask(()=>{this.readyState=1;this.onopen?.();if(this.signaling)this.onmessage?.({data:JSON.stringify({type:'peers',peers:Array.from({length:count},(_,i)=>i).filter(i=>i!==this.player),iceServers:[]})});choose();});
    }
    send(value){
      if(this.signaling){if(JSON.parse(value).type==='rtc-failed')fallback.add(this.player);choose();return;}
      const packet=new Uint8Array(value);assert.equal(packet[0],0xe7);
      const target=data.get(packet[1]),authenticated=new Uint8Array(packet.length);authenticated.set([0xe6,this.player]);authenticated.set(packet.subarray(2),2);
      queueMicrotask(()=>target?.onmessage?.({data:authenticated.buffer}));
    }
    close(){this.readyState=3;this.onclose?.();}
  }
  const transports=[],opened=[];
  for(let player=0;player<count;player++){
    const transport=createManagedTransport({netplayPlayer:player,netplayPlayerCount:count,netplayUrl:'ws://local/?room=test&member=private&run=1'},roles,error=>errors.push(error.message),{WebSocket:Socket});
    transports.push(transport);
    for(let peer=0;peer<count;peer++)if(peer!==player)opened.push(new Promise(resolve=>{transport.channelFor(roles[peer]).onopen=resolve;}));
  }
  for(const transport of transports)transport.connect();await Promise.all(opened);
  let firstFinished=false;const first=transports[0].finish().then(()=>{firstFinished=true;});
  await new Promise(resolve=>setTimeout(resolve,250));assert.equal(firstFinished,false,'a local native exit cannot release the run before every engine ends');
  await Promise.all([first,...transports.slice(1).map(transport=>transport.finish())]);
  assert.equal(firstFinished,true);assert.deepEqual(errors,[]);
  for(const transport of transports)transport.close();
  console.log('PASS '+count+'P authenticated relay end barrier');
}
