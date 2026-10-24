import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {createScoreSave,isScoreSaveCancelled} from '../../web/sdl-runtime/score-save.mjs';

const deferred=()=>{let resolve,reject;const promise=new Promise((yes,no)=>{resolve=yes;reject=no;});return {promise,resolve,reject};};
const settle=async()=>{for(let i=0;i<8;++i)await Promise.resolve();};
function fixture(overrides={}){
  const timers=new Map(),statuses=new Map(),calls=[],disk=deferred();let clock=0,serial=0,timerSerial=0;
  const options={request:()=>{const token=++serial;calls.push(['request',token]);statuses.set(token,0);return token;},
    poll:token=>{calls.push(['poll',token]);return statuses.get(token)??-2;},
    cancel:token=>{calls.push(['cancel',token]);if(statuses.get(token)===0)statuses.set(token,-1);},
    error:()=>'',flushReplay:()=>calls.push(['replay']),sync:()=>{calls.push(['sync']);return disk.promise;},
    now:()=>clock,schedule:(fn,ms)=>{const id=++timerSerial;timers.set(id,{fn,at:clock+ms});return id;},
    unschedule:id=>timers.delete(id),timeoutMs:200,pollMs:50,...overrides};
  const controller=createScoreSave(options);
  const advance=async(ms=50)=>{clock+=ms;const due=[...timers].filter(([,t])=>t.at<=clock);
    for(const [id,timer] of due){if(!timers.delete(id))continue;timer.fn();}await settle();};
  return {controller,options,timers,statuses,calls,disk,advance,count:name=>calls.filter(c=>c[0]===name).length};
}

test('concurrent callers share one native request and await real filesystem completion',async()=>{
  const f=fixture(),first=f.controller.save();let done=false;first.then(()=>done=true);
  assert.equal(f.controller.save(),first);assert.equal(f.count('request'),1);assert.equal(f.count('sync'),0);
  await f.advance();assert.equal(f.count('sync'),0);assert.equal(done,false);
  f.statuses.set(1,1);await f.advance();assert.equal(f.count('sync'),1);assert.equal(done,false);
  assert.equal(f.controller.save(),first,'completed native token stays joined while IDBFS is writing');
  f.disk.resolve();await first;assert.equal(done,true);assert.equal(f.timers.size,0);
  const next=f.controller.save();assert.notEqual(next,first);assert.equal(f.count('request'),2);
  f.statuses.set(2,1);await f.advance();await next;
});

test('native failure rejects without syncing predicted files',async()=>{
  const f=fixture({error:()=> 'Peer disconnected'}),saved=f.controller.save();
  const rejected=assert.rejects(saved,/Peer disconnected/);f.statuses.set(1,-1);await f.advance();await rejected;
  assert.equal(f.count('sync'),0);assert.equal(f.timers.size,0);
});

test('bounded timeout cancels the native fence and never reports success',async()=>{
  const f=fixture(),saved=f.controller.save(),rejected=assert.rejects(saved,/confirmation timed out/);
  await f.advance(200);await rejected;
  assert.equal(f.count('cancel'),1);assert.equal(f.statuses.get(1),-1);assert.equal(f.count('sync'),0);assert.equal(f.timers.size,0);
});

test('world reset makes the original request a typed cancellation',async()=>{
  const f=fixture(),saved=f.controller.save();
  const rejected=assert.rejects(saved,error=>isScoreSaveCancelled(error)&&/previous game/.test(error.message));
  f.statuses.clear();await f.advance();await rejected;assert.equal(f.count('sync'),0);
  const fresh=f.controller.save();f.statuses.set(2,1);await f.advance();f.disk.resolve();await fresh;
});

test('lifecycle cancellation releases a pending fence and does not reuse its token',async()=>{
  const f=fixture(),old=f.controller.save(),rejected=assert.rejects(old,isScoreSaveCancelled);
  f.controller.cancel('Room changed');await rejected;assert.equal(f.timers.size,0);assert.equal(f.statuses.get(1),-1);
  const fresh=f.controller.save();assert.equal(f.count('request'),2);assert.notEqual(fresh,old);
  f.statuses.set(2,1);await f.advance();f.disk.resolve();await fresh;
});

test('an old filesystem callback cannot resolve a new world request',async()=>{
  const f=fixture(),old=f.controller.save();f.statuses.set(1,1);await f.advance();
  const rejected=assert.rejects(old,isScoreSaveCancelled);f.controller.cancel('World replaced during persistence');await rejected;
  const fresh=f.controller.save();let freshDone=false;fresh.then(()=>freshDone=true);
  f.disk.resolve();await settle();assert.equal(freshDone,false);assert.equal(f.statuses.get(2),0);
  f.statuses.set(2,1);await f.advance();await fresh;
});

test('best-effort pagehide flush does not claim a pending native score completed',async()=>{
  const f=fixture(),saved=f.controller.save();let done=false;saved.then(()=>done=true,()=>{});
  const flush=f.controller.flushConfirmed();assert.equal(f.count('request'),1);assert.equal(f.statuses.get(1),0);
  f.disk.resolve();await flush;await settle();assert.equal(done,false);
  const rejected=assert.rejects(saved,isScoreSaveCancelled);f.controller.cancel();await rejected;
});

test('real syncfs errors propagate after native completion',async()=>{
  const f=fixture(),saved=f.controller.save(),rejected=assert.rejects(saved,/IDB write failed/);
  f.statuses.set(1,1);await f.advance();f.disk.reject(Error('IDB write failed'));await rejected;
  assert.equal(f.count('request'),1);assert.equal(f.count('sync'),1);assert.equal(f.timers.size,0);
});

test('a host polling exception releases native pending state',async()=>{
  let fail=false;const f=fixture({poll:()=>{if(fail)throw Error('Poll failed');return 0;}}),saved=f.controller.save();
  const rejected=assert.rejects(saved,/Poll failed/);fail=true;await f.advance();await rejected;
  assert.equal(f.statuses.get(1),-1);assert.equal(f.count('cancel'),1);assert.equal(f.count('sync'),0);
});

test('inactive or immediate saves still wait for the real filesystem callback',async()=>{
  const f=fixture({request:()=>null}),saved=f.controller.save();let done=false;saved.then(()=>done=true);
  await settle();assert.equal(f.count('poll'),0);assert.equal(f.count('sync'),1);assert.equal(done,false);
  f.disk.resolve();await saved;assert.equal(done,true);
});

// Execute the production functions themselves so this checks dispatch order,
// input ABI calls and lifecycle placement, rather than a second queue model.
const shell=readFileSync(new URL('../../web/sdl-runtime/shell.mjs',import.meta.url),'utf8');
const commandSource=shell.slice(shell.indexOf('async function command(m)'),shell.indexOf("document.addEventListener('visibilitychange'"));
const stopSource=shell.slice(shell.indexOf('async function stop()'),shell.indexOf('\nfunction path('));
const backgroundSaveSource=shell.slice(shell.indexOf('const backgroundSaveError='),shell.indexOf('\nfunction closeAudio('));
const phaseExitSource=shell.match(/if\(core\._th15_phase\(\)===4\)void stop\(\)\.catch\([^)]+\);/)[0];
const transportExitSource=shell.match(/onExit:(\(\)=>\{void stop\(\)\.catch\([^)]+\);\})/)[1];
function hostFixture(){
  const disk=deferred(),calls=[],replies=[],listeners=new Map(),fatals=[];
  const context={queue:Promise.resolve(),initialized:Promise.resolve(true),launched:true,stopping:false,
    protocol:'eagler-touhou/1',game:'th15',epoch:1,validEpoch:true,roomOwned:false,roomPreview:false,first:true,
    runtimeBuild:{multiplayer:true},options:{touchEnabled:true},saveRoot:'/savesth15mp',location:{origin:'https://example.test'},
    save:()=>{calls.push('save');return disk.promise;},scoreSave:{cancel:()=>calls.push('cancel')},
    document:{hidden:false},parent:{postMessage:message=>replies.push(message)},window:{addEventListener:(name,fn)=>listeners.set(name,fn)},
    core:{_th15_touch_controls:(...args)=>calls.push(['controls',...args]),_th15_touch_stick:(...args)=>calls.push(['stick',...args]),
      _th15_loop_stop:()=>calls.push('stop-loop'),_th15_loop_start:()=>calls.push('start-loop'),
      _th15_room_return:()=>{calls.push('return-room');return 1;},_th15_room_preview:()=>1,
      FS:{readFile:()=>{calls.push('read');return new Uint8Array([1,2]);}}},
    clearKeyboard:()=>calls.push('clear-input'),spectatorPlayback:{stop:()=>calls.push('spectator-stop')},
    localReplay:{close:()=>calls.push('replay-close')},multiplayerBridge:{close:()=>calls.push('close-peer')},
    closeAudio:()=>calls.push('close-audio'),emit:(...args)=>calls.push(['emit',...args]),fatal:e=>fatals.push(e),path:value=>value,
    requestAnimationFrame:fn=>Promise.resolve().then(fn),err:()=>'',isScoreSaveCancelled,
  };
  runInNewContext(backgroundSaveSource+'\n'+commandSource+'\n'+stopSource,context);
  const dispatch=(command,fields={})=>context.dispatchHostCommand({command,request:command,...fields});
  return {disk,calls,replies,listeners,fatals,context,dispatch};
}

test('live touch input and ACK pass pending sync while file and room commands stay ordered',async()=>{
  const h=hostFixture();h.dispatch('sync');h.dispatch('read',{path:'th15.cfg'});h.dispatch('return-room');
  h.dispatch('touch-controls',{controls:{fireEnabled:true,joystickX:22000,joystickY:0}});await settle();
  assert.ok(h.calls.includes('save'));assert.ok(h.calls.some(c=>Array.isArray(c)&&c[0]==='controls'&&c[2]===1));
  assert.deepEqual(h.calls.find(c=>Array.isArray(c)&&c[0]==='stick'),['stick',22000,0]);
  assert.deepEqual(h.replies.map(r=>r.request),['touch-controls']);assert.equal(h.calls.includes('read'),false);assert.equal(h.calls.includes('return-room'),false);
  h.disk.resolve();await h.context.queue;
  assert.deepEqual(h.replies.map(r=>r.request),['touch-controls','sync','read','return-room']);
  assert.ok(h.calls.indexOf('read')<h.calls.indexOf('return-room'));assert.equal(h.fatals.length,0);
});

test('failed sync replies with failure and releases queued file operations',async()=>{
  const h=hostFixture();h.dispatch('sync');h.dispatch('read',{path:'th15.cfg'});await settle();h.disk.reject(Error('Save failed'));await h.context.queue;
  assert.equal(h.replies[0].ok,false);assert.match(h.replies[0].error,/Save failed/);assert.equal(h.replies[1].request,'read');
});

test('live input fast path retains source, origin, protocol and world-epoch validation',async()=>{
  const h=hostFixture(),receive=h.listeners.get('message'),data={protocol:h.context.protocol,game:'th15',epoch:1,command:'touch-controls',request:'input',controls:{fireEnabled:true}};
  const valid={source:h.context.parent,origin:h.context.location.origin,data};
  for(const event of [{...valid,source:{}},{...valid,origin:'https://foreign.test'},{...valid,data:{...data,protocol:'wrong'}},
    {...valid,data:{...data,game:'th11'}},{...valid,data:{...data,epoch:2}}])receive(event);
  await settle();assert.equal(h.calls.length,0);assert.equal(h.replies.length,0);
  receive(valid);await settle();assert.equal(h.replies.length,1);assert.equal(h.replies[0].ok,true);
});

test('unlaunched input cannot bypass a queued control operation',async()=>{
  const h=hostFixture();h.context.launched=false;h.dispatch('sync');h.dispatch('touch-controls');await settle();
  assert.equal(h.calls.some(c=>Array.isArray(c)&&c[0]==='controls'),false);
  h.disk.resolve();await h.context.queue;assert.deepEqual(h.replies.map(r=>r.request),['sync','touch-controls']);
});

test('stop keeps the native loop alive until score confirmation and filesystem completion',async()=>{
  const h=hostFixture(),stopped=h.context.stop();await settle();assert.ok(h.calls.includes('save'));assert.equal(h.calls.includes('stop-loop'),false);
  h.disk.resolve();await stopped;assert.ok(h.calls.includes('stop-loop'));assert.equal(h.context.launched,false);assert.equal(h.context.stopping,false);
  assert.ok(h.calls.some(c=>Array.isArray(c)&&c[0]==='emit'&&c[1]==='exit'&&c[2].status==='success'));
});

test('failed stop save leaves the loop alive and never emits successful exit',async()=>{
  const h=hostFixture(),stopped=h.context.stop(),rejected=assert.rejects(stopped,/Disk unavailable/);
  h.disk.reject(Error('Disk unavailable'));await rejected;assert.equal(h.calls.includes('stop-loop'),false);assert.equal(h.context.launched,true);assert.equal(h.context.stopping,false);
  assert.equal(h.calls.some(c=>Array.isArray(c)&&c[0]==='emit'&&c[1]==='exit'),false);
});

test('phase exit consumes lifecycle cancellation without failing the new world',async()=>{
  const h=hostFixture();h.context.core._th15_phase=()=>4;runInNewContext(phaseExitSource,h.context);
  h.disk.reject(Object.assign(Error('World changed'),{code:'EAGLER_SCORE_SAVE_CANCELLED'}));await settle();
  assert.equal(h.fatals.length,0);assert.equal(h.calls.includes('stop-loop'),false);
  assert.equal(h.calls.some(c=>Array.isArray(c)&&c[0]==='emit'&&c[1]==='exit'),false);
});

test('phase exit still surfaces real filesystem failure',async()=>{
  const h=hostFixture();h.context.core._th15_phase=()=>4;runInNewContext(phaseExitSource,h.context);
  h.disk.reject(Error('Disk unavailable'));await settle();assert.equal(h.fatals.length,1);assert.match(h.fatals[0].message,/Disk unavailable/);
});

test('unsolicited sync cancellation is consumed while requested sync explicitly rejects',async()=>{
  const background=hostFixture();background.context.dispatchHostCommand({command:'sync'});await settle();
  background.disk.reject(Object.assign(Error('World changed'),{code:'EAGLER_SCORE_SAVE_CANCELLED'}));await background.context.queue;
  assert.equal(background.fatals.length,0);assert.equal(background.replies.length,0);
  const requested=hostFixture();requested.dispatch('sync');await settle();
  requested.disk.reject(Object.assign(Error('World changed'),{code:'EAGLER_SCORE_SAVE_CANCELLED'}));await requested.context.queue;
  assert.equal(requested.fatals.length,0);assert.equal(requested.replies.length,1);assert.equal(requested.replies[0].ok,false);assert.match(requested.replies[0].error,/World changed/);
});

test('transport exit consumes its asynchronous stop result and surfaces real failure',async()=>{
  const h=hostFixture();runInNewContext('('+transportExitSource+')()',h.context);
  h.disk.reject(Error('Disconnected before score confirmation'));await settle();
  assert.equal(h.fatals.length,1);assert.match(h.fatals[0].message,/Disconnected before score confirmation/);
});
