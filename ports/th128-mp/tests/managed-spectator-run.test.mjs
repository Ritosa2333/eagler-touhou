import assert from 'node:assert/strict';
import {test} from 'node:test';
import {setTimeout as sleep} from 'node:timers/promises';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createConfirmedHistory} from '../runtime/confirmed-history.mjs';
import {createTh128SpectatorEncoder} from '../runtime/spectator-wire.mjs';
import {launchManagedRun} from '../runtime/managed-run.mjs';

const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)};
const rules=epoch=>createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'th128mp-34772-1',epoch,
  mode:'parallel-stage',route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:2,...build});
const row=frame=>({frame,inputs:[0,1].map(player=>({player,held:0,pressed:0,released:0,predicted:false}))});
const payload=message=>message.slice(1).buffer;
async function encoder(epoch){
  const history=await createConfirmedHistory({rules:rules(epoch),allowedMask:763});
  return createTh128SpectatorEncoder(history.header,build);
}
function bootstrap(head,frame){
  const bytes=head.slice(1,33),fields=new DataView(bytes.buffer);
  bytes[5]=4;fields.setUint32(20,frame,true);return bytes.buffer;
}

test('managed viewer decodes real spectator packets and owns presentation controls without touching a player socket',async t=>{
  let socket,serial=0;
  const archives=[],frames=new Map(),reports=[],worlds=[],errors=[],first=[],renders=[];
  const win=new EventTarget(),doc=Object.assign(new EventTarget(),{hidden:false});
  class Socket extends EventTarget{
    readyState=0;
    constructor(){super();socket=this;}
    open(){this.readyState=1;this.dispatchEvent(new Event('open'));}
    input(data){this.dispatchEvent(new MessageEvent('message',{data}));}
    send(){assert.fail('receive-only viewer sent a packet');}
    close(){this.readyState=3;this.dispatchEvent(new Event('close'));}
  }
  const pairs={window:win,document:doc,WebSocket:Socket,requestAnimationFrame:callback=>{frames.set(++serial,callback);return serial;},
    cancelAnimationFrame:id=>frames.delete(id)};
  const originals=Object.fromEntries(Object.keys(pairs).map(name=>[name,Object.getOwnPropertyDescriptor(globalThis,name)]));
  for(const [name,value] of Object.entries(pairs)){
    Object.defineProperty(globalThis,name,{value,configurable:true,writable:true});
  }
  const scope=new AbortController();t.after(()=>{scope.abort();for(const [name,original] of Object.entries(originals)){
    if(original)Object.defineProperty(globalThis,name,original);else delete globalThis[name];
  }});
  function emit(value){if(value.spectatorPlayback)reports.push(value.spectatorPlayback);}
  const room={role:'spectator',run:1,rules:rules(1),relayUrl:'ws://127.0.0.1:53128/?room=th128mp-34772&run=1&spectator=viewer-entry&member=member-viewer&players=2'};
  const launching=launchManagedRun({room,onReplay:bytes=>archives.push(JSON.parse(new TextDecoder().decode(bytes))),canvas:{},build,signal:scope.signal,onStatus:emit,onError:error=>errors.push(error),
    openPeer(){assert.fail('spectator unexpectedly opened a player transport');},present(actual,path){first.push({actual,path});return true;},
    view:{openWorld(selection,_owner,options){const seen=[],x={seen,options,closes:0,step(r){seen.push(r.frame);},display(seat,frame){renders.push({epoch:selection.epoch,seat,frame});return true;},
      close(){x.closes++;}};worlds.push(x);return x;}}});
  socket.open();const owned=await launching;
  let time=0;
  async function raf(){const copy=[...frames.values()];frames.clear();assert.equal(copy.length,1);copy[0](time+=1000/60);await sleep(1);}
  async function until(condition){for(let i=0;i<40;i++){if(condition())return;await raf();}assert.fail('managed viewer failed to reach the expected checkpoint');}
  const a=await encoder(1),head=a.open();socket.input(JSON.stringify(a.announce()));socket.input(payload(head));
  for(let i=0;i<6;i++)socket.input(payload(a.frame(row(i))));
  for(let i=0;i<3;i++)await raf();assert.equal(worlds.length,0);assert.equal(first.length,0);
  socket.input(bootstrap(head,6));
  await until(()=>first.length===1);
  assert.equal(worlds[0].options?.seatCues,false,'a spectator has no personal seat cues');
  assert.deepEqual(first,[{actual:true,path:'spectator-relay'}]);assert.deepEqual(worlds[0].seen,[0,1,2,3,4,5]);
  const original=reports.at(-1).run;owned.spectatorPlayback({run:original,action:'pause'});
  for(let i=6;i<11;i++)socket.input(payload(a.frame(row(i))));
  socket.input(payload(a.finish('restart')));
  const b=await encoder(2),head2=b.open();socket.input(JSON.stringify(b.announce()));socket.input(payload(head2));
  socket.input(payload(b.frame(row(0))));
  await until(()=>reports.at(-1).received===11);
  assert.equal(reports.at(-1).position,6,'a stopped stream still publishes its final sub-batch while paused');
  assert.equal(archives.length,0,'a viewer paused before the last frame cannot claim the ending yet');
  owned.spectatorPlayback({run:original,action:'live'});
  await until(()=>reports.at(-1).run!==original&&!reports.at(-1).seeking);
  assert.ok(renders.some(frame=>frame.epoch===1&&frame.frame===11));
  assert.deepEqual(renders.at(-1),{epoch:2,seat:0,frame:1});
  assert.deepEqual(archives.map(f=>[f.tail.reason,f.tail.frameCount]),[['restart',11]]);
  socket.input(payload(b.finish('completed')));await until(()=>archives.length===2);assert.deepEqual(archives.map(f=>[f.tail.reason,f.tail.frameCount]),[['restart',11],['completed',1]]);
  assert.deepEqual(errors,[]);assert.equal(first.length,1);
  owned.close();assert.equal(socket.readyState,3);assert.equal(frames.size,0);
  assert.deepEqual(worlds.map(world=>world.closes),[1,1]);
});
