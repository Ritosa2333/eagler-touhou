import {createMultiplayerRules} from '../../../th128/site/runtime/multiplayer-rules.mjs';
import {orderDrawLayers} from '../../../th128/site/runtime/draw.mjs';
import {WebGlRenderer,StageBackground,parseStd} from '../../../th128/site/runtime/renderer.mjs';
import {openPinnedPreviewResources} from '../../runtime/preview-resources.mjs';
import {prepareA13Preview} from '../../runtime/preview-world.mjs';
import {createPlayerMatch} from '../../runtime/player-match.mjs';
import {openRelayLink} from '../../runtime/relay-link.mjs';
import {createTh128SpectatorEncoder,createTh128SpectatorDecoder} from '../../runtime/spectator-wire.mjs';
import {openTh128SpectatorRelay} from '../../runtime/spectator-link.mjs';
import {openPreviewReplayStore} from '../../runtime/preview-replay-store.mjs';
import {createMatchClock} from '../../runtime/match-clock.mjs';
import {createSeatInput,TH128_HELD} from '../../runtime/seat-input.mjs';
import {makeTh128ReplayFile,readTh128ReplayFile} from '../../runtime/replay-file.mjs';
import {openTh128ReplayPlayback} from '../../runtime/replay-playback.mjs';
import {openAuthorPeerLink} from '../../runtime/author-peer.mjs';
import {readTh128RoomOptions} from '../../runtime/managed-room.mjs';

const index=/^\/seat-([01])$/.exec(location.pathname)?.[1],seat=index===undefined?null:Number(index);
const role=seat!==null?'player':location.pathname==='/watch'?'spectator':location.pathname==='/replay'?'replay':null;
const dom=id=>document.getElementById(id),screen=dom('game'),controller=new AbortController(),{signal}=controller;
const member=crypto.randomUUID(),client=role==='player'?`th128-player-${seat}`:'th128-watcher';
const counts={factories:0,backgroundFactories:0,worldStarts:0,worldTicks:0,backgroundTicks:0,rendered:0};
const worlds=new Map(),tracks=[];
let config,resources,preview,renderer,lobby,signaling,link,match,decoder,receiver,store;
let phase='loading',errorText=null,room=null,serial=null,rules=null,held=0,native=null,renderedTag='',entered=false;
let readySent=false,resourceReported=false,routed=false,refreshChain=Promise.resolve(),restartPump=null,transportPump=null,routeName=null;
let encoder=null,uploadSource=null,uploadCursor=0,uploadCount=0,uploadError=null;
let watchPaused=false,watchWorld=null,watchFrame=0,watchBoot=false,watchChain=Promise.resolve(),saved=null,loaded=null,watchTail=null;
let replayPlayback=null,replayRaf=null;
const controls=seat===null?null:createSeatInput({allowedMask:763,controllerIndex:seat});
const cadence={callbacks:0,advanced:0,samples:0,everHeld:0,lastHeld:0,waits:0,stalls:0,started:null,finished:null};
// Keep this diagnostic local to the owned validation fixture. Record only
// the two test arrows and the disposition of their next simulation sample.
const inputProbe={rows:[],clears:Object.create(null),gapMs:0,maxGapMs:0,paintMs:0,tickMs:0,watchMs:0};
let lastPresentationTimestamp=null;
const milliseconds=n=>Math.round(n*100)/100;
function noteArrow(kind,fields={}){
  if(!config?.inputDiagnostic)return;
  const row={kind,ms:milliseconds(performance.now()),sample:cadence.samples,...fields};
  if(inputProbe.rows.length===24)inputProbe.rows.shift();inputProbe.rows.push(row);
}
function clearSeat(reason){
  if(config?.inputDiagnostic){
    inputProbe.clears[reason]=(inputProbe.clears[reason]??0)+1;
    const pending=inputProbe.rows.findLast(row=>row.kind==='keydown');
    if(pending?.accepted&&pending.sample===cadence.samples&&!pending.clearBeforeSample&&reason!=='stall')pending.clearBeforeSample=reason;
    if(pending?.accepted&&pending.sample===cadence.samples&&reason==='stall')pending.stallBeforeSample=true;
  }
  if(reason==='stall')controls?.presentationStall();
  else if(reason==='blur'&&!document.hidden)controls?.focusLost();
  else controls?.clear();
}
let runRequested=false,frameRequest=null,queuedPaint=false,clockEpoch=null;
const clock=controls?createMatchClock({stepFrame:options=>{
  const step=match.tick(sampleControls,options);
  if(step.advanced)cadence.advanced++;
  if(step.reason==='waiting-input')cadence.waits++;
  return step;
}}):null;
const bytes=async url=>{
  const response=await fetch(url,{signal,cache:'no-store',redirect:'error'});
  if(!response.ok)throw new Error(`${url}: HTTP ${response.status}`);return new Uint8Array(await response.arrayBuffer());
};

function button(id,show,enable){const el=dom(id);el.hidden=!show;el.disabled=!enable;}
function continuous(){return role==='player'&&config?.continuous===true;}
function canRun(){
  if(!continuous()||!runRequested||signal.aborted||document.hidden||phase!=='playing')return false;
  return ['running','voting'].includes(match.status().phase);
}
function publish(){
  const active=match?.status()??null,current=room?.seats?.[seat];
  const track=tracks[0],watch=decoder?.status()??null;
  const playback=replayPlayback?.status()??null;
  dom('status').textContent=`${role==='player'?`P${seat+1}`:role==='spectator'?'旁观':'多人录像'} · ${phase}${errorText?` · ${errorText}`:''}`;
  dom('evidence').textContent=JSON.stringify({role,seat,phase,error:errorText,counts:{...counts},serial,routed,routeName,
    peer:config?.authorTransport?link?.status()??null:null,held,native,match:active,
    clock:continuous()?{...cadence,requested:runRequested,visibility:document.visibilityState,focused:document.hasFocus()}:null,
    inputDiagnostic:config?.inputDiagnostic?inputProbe:null,
    room:room?{phase:room.phase,count:room.playerCount,seats:room.seats.slice(0,2).map(s=>s?{id:s.clientId,ready:s.ready}:null),
      spectators:room.spectatorCount,timeline:room.spectatorTimeline===true}:null,
    watch:watch?{...watch,playingEpoch:track?.header.rules.epoch??null,playingFrame:watchFrame,paused:watchPaused,
      playable:watchBoot,retained:tracks.length,tail:watchTail}:null,uploadCount,uploadError,saved,loaded,
    ...(config?.sequenceReplay===true&&role==='replay'?{playback}: {})});
  button('join',role!=='replay',phase==='prepared');
  button('ready',role==='player',phase==='room'&&current?.clientId===client&&resourceReported&&!readySent);
  button('start',seat===0,phase==='room'&&room?.phase==='lobby'&&room.seats.slice(0,2).every(s=>s?.ready));
  const playing=phase==='playing'&&active?.startup.canStart&&['running','voting'].includes(active.phase);
  for(const id of ['left','neutral','right','step'])button(id,role==='player'&&!continuous(),playing&&!continuous());
  button('run',continuous(),playing&&!runRequested);
  dom('touch-pad').hidden=!continuous()||!runRequested;
  button('restart',role==='player',playing&&!active.votes[seat]);
  button('save-replay',seat===0,playing&&active.epoch>1&&!saved);
  button('export-replay',seat===0,playing&&active.epoch>1);
  button('pause',role==='spectator',phase==='watching'&&!watchPaused);button('live',role==='spectator',phase==='watching'&&watchPaused);
  button('load-replay',role==='replay',phase==='prepared'&&!loaded);
  dom('import-replay-label').hidden=role!=='replay';
  dom('import-replay').disabled=(phase!=='prepared'&&phase!=='replayed')||['playing','opening'].includes(playback?.phase);
  const replayControls=role==='replay'&&phase==='replayed'&&!!playback;
  const canReplay=replayControls&&['paused','playing','ended'].includes(playback?.phase);
  button('replay-play',replayControls,canReplay&&playback.phase!=='ended');
  button('replay-again',replayControls,canReplay&&playback.total>0);
  button('replay-seat',replayControls,canReplay);
  dom('replay-play').textContent=playback?.phase==='playing'?'暂停':'播放';
  dom('replay-seat').textContent=`切换到 P${playback?.seat===1?1:2}`;
  if(!signal.aborted&&active&&['preparing','ready'].includes(active.phase)&&restartPump===null){
    restartPump=setTimeout(()=>{
      restartPump=null;if(signal.aborted)return;
      try{match.flush();refresh().catch(fail);}catch(error){fail(error);}
    },50);
  }
}
function budget(name){if(counts[name]>=config.limits[name])throw new Error(`Room preview ${name} limit`);counts[name]++;}
function stop(){
  controller.abort();clearTimeout(restartPump);clearTimeout(transportPump);if(frameRequest!==null)cancelAnimationFrame(frameRequest);
  clearReplayFrame();
  controls?.clear();clock?.reset();receiver?.close();link?.close();signaling?.close();lobby?.close();store?.close();
  try{match?.close();watchWorld?.engine.close();}finally{renderer?.dispose();renderer=null;}
}
function fail(error){
  if(phase==='closed'||phase==='failed')return;
  errorText=String(error?.message??error);phase='failed';try{stop();}finally{publish();}
}
function address(query){const url=new URL(config.relay);url.search=new URLSearchParams({room:config.room,member,...query});return url;}
function jsonLink(url,receive){
  const socket=new WebSocket(url),{promise:ready,resolve,reject}=Promise.withResolvers();void ready.catch(()=>{});
  let chain=Promise.resolve(),pending=0,closed=false;
  function close(){if(!closed){closed=true;reject(new Error('Room socket closed'));if(socket.readyState<2)socket.close(1000,'leave room');}}
  function error(problem){if(!closed){reject(problem);close();fail(problem);}}
  socket.addEventListener('open',()=>resolve(),{once:true});
  socket.addEventListener('close',event=>error(new Error(`Room socket closed (${event.code})`)),{once:true});
  socket.addEventListener('error',()=>error(new Error('Room socket error')));
  socket.addEventListener('message',({data})=>{
    if(closed)return;
    if(typeof data!=='string'||data.length>32768||pending>=64){error(new Error('Room text packet limit'));return;}
    pending++;chain=chain.then(()=>{if(!closed)return receive(JSON.parse(data));}).catch(error).finally(()=>{pending--;});
  });
  return {ready,close,send(message){if(closed||socket.readyState!==1)throw new Error('Room socket not connected');socket.send(JSON.stringify(message));}};
}
async function openWorld(selection,owner=signal){
  const engine=await preview.openPair(selection,owner);
  try{
    const {module,std}=await preview.openBackground();owner.throwIfAborted();
    return {engine,background:new StageBackground(parseStd(std),module),epoch:selection.epoch};
  }catch(error){engine.close();throw error;}
}
async function openEpoch(selection,owner){
  const world=await openWorld(selection,owner);worlds.set(selection.epoch,world);
  return Object.freeze({...world.engine,close(){if(worlds.get(selection.epoch)===world)worlds.delete(selection.epoch);world.engine.close();}});
}
function paint(world,seat,confirmed,revision=0){
  const start=config?.inputDiagnostic?performance.now():null;
  while(world.background.frame<confirmed){budget('backgroundTicks');world.background.tick();}
  const states=[0,1].map(index=>world.engine.state(index));
  if(states.some(s=>s.normalRecord.bound||s.normalRecord.scoreEligible))throw new Error('Multiplayer owns no single player recorder');
  const tag=[world.epoch,states[seat].frame,confirmed,revision].join(':');if(tag===renderedTag)return;
  const draws=world.engine.draws(seat);
  renderer.render(orderDrawLayers(draws,{world:true}),{world:true,background:world.background,shake:states[seat].screenShake});
  renderedTag=tag;counts.rendered++;screen.dataset.epoch=world.epoch;screen.dataset.frame=states[seat].frame;
  native={epoch:world.epoch,draws:draws.length,players:states.map(s=>({frame:s.frame,x:s.playerX,y:s.playerY,record:s.normalRecord.bound}))};
  if(start!==null)inputProbe.paintMs=Math.max(inputProbe.paintMs,milliseconds(performance.now()-start));
}
function readRules(wire){
  const selected=createMultiplayerRules(wire);
  if(selected.matchId!==config.matchId||selected.route!=='A1-3'||selected.seed!==34772||selected.rollbackWindow!==2||selected.inputDelay!==0)
    throw new Error('Unexpected TH128 local room rules');return selected;
}
function drainWatcher(){
  const {nextFrame}=uploadSource.status();
  if(nextFrame-uploadCursor>32)throw new Error('Spectator upload queue full');
  while(uploadCursor<nextFrame){link.sendSpectator(encoder.frame(uploadSource.read(uploadCursor++)));uploadCount++;}
}
async function refreshPlayer(){
  if(signal.aborted||!routed)return;
  const active=match.status();
  if(!active.startup.canStart){publish();return;}
  if(seat===0&&!uploadError){
    try{
      if(uploadSource?.header.rules.epoch!==active.epoch){
        if(uploadSource){
          if(uploadSource.status().end!=='restart')throw new Error('Retired spectator prefix has no agreed end');
          drainWatcher();link.sendSpectator(encoder.finish('restart'));
        }
        uploadSource=match.spectatorSource();uploadCursor=0;
        encoder=await createTh128SpectatorEncoder(uploadSource.header,resources.build);signal.throwIfAborted();
        link.sendSpectator(encoder.announce());link.sendSpectator(encoder.open());
      }
      drainWatcher();
    }catch(error){uploadError=String(error.message??error);}
  }
  if(signal.aborted)return;
  const state=match.status(),world=worlds.get(state.epoch);
  if(continuous()&&state.history.nextFrame===config.limits.roundFrames&&cadence.finished===null)cadence.finished=performance.now();
  paint(world,seat,state.history.nextFrame,state.round.replayedFrames);phase='playing';publish();
}
function refresh(){refreshChain=refreshChain.then(refreshPlayer);return refreshChain;}
function sampleControls(){
  let pads=[];try{pads=navigator.getGamepads?.()??[];}catch{}
  const bits=controls.read(pads);cadence.samples++;cadence.everHeld|=bits;cadence.lastHeld=bits;
  if(config.inputDiagnostic){
    const pending=inputProbe.rows.findLast(row=>row.kind==='keydown');
    if(pending?.accepted&&!pending.nextSample)pending.nextSample={number:cadence.samples,bits};
  }
  return bits;
}
function queuePaint(){
  if(!queuedPaint){queuedPaint=true;refresh().catch(fail).finally(()=>{queuedPaint=false;});}
}
function syncClockEpoch(){
  const epoch=match.status().epoch;
  if(epoch!==clockEpoch){clockEpoch=epoch;clock.reset();clearSeat('epoch');cadence.finished=null;cadence.started=null;}
}
function presentationFrame(now){
  frameRequest=null;if(signal.aborted)return;
  try{
    const active=canRun();syncClockEpoch();
    cadence.callbacks++;
    if(active&&cadence.started===null)cadence.started=now;
    const before=config.inputDiagnostic?performance.now():null;
    if(before!==null){
      if(lastPresentationTimestamp!==null)inputProbe.gapMs=milliseconds(now-lastPresentationTimestamp);
      lastPresentationTimestamp=now;inputProbe.maxGapMs=Math.max(inputProbe.maxGapMs,inputProbe.gapMs);
    }
    const outcome=clock.advance(now,active);
    if(before!==null)inputProbe.tickMs=Math.max(inputProbe.tickMs,milliseconds(performance.now()-before));
    if(outcome.stalled){cadence.stalls++;clearSeat('stall');}
    queuePaint();
    if(!signal.aborted)frameRequest=requestAnimationFrame(presentationFrame);
  }catch(error){fail(error);}
}
function mountControls(){
  const options={signal};
  window.addEventListener('keydown',event=>{
    const canvasFocused=document.activeElement===screen,active=canRun(),modified=event.ctrlKey||event.altKey||event.metaKey;
    const accepted=!!(event.isTrusted&&canvasFocused&&!modified&&active&&controls.key(event.code,true));
    if(['ArrowLeft','ArrowRight'].includes(event.code))noteArrow('keydown',{code:event.code,trusted:event.isTrusted,canvasFocused,active,modified,accepted});
    if(accepted)event.preventDefault();
  },options);
  window.addEventListener('keyup',event=>{
    if(event.isTrusted)controls.key(event.code,false);
    if(['ArrowLeft','ArrowRight'].includes(event.code))noteArrow('keyup',{code:event.code,trusted:event.isTrusted});
  },options);
  window.addEventListener('blur',()=>clearSeat('blur'),options);
  document.addEventListener('visibilitychange',()=>{
    if(document.hidden){clearSeat('visibility');clock.reset();}publish();
  },options);
  for(const target of dom('touch-pad').querySelectorAll('button[data-key]')){
    target.addEventListener('pointerdown',event=>{
      if(!event.isTrusted||!canRun()||event.button!==0||!controls.touch(event.pointerId,TH128_HELD[target.dataset.key]))return;
      try{target.setPointerCapture(event.pointerId);}catch{controls.touch(event.pointerId,0);}
      event.preventDefault();
    },options);
    for(const type of ['pointerup','pointercancel','lostpointercapture'])target.addEventListener(type,event=>controls.touch(event.pointerId,0),options);
  }
}
async function launchPlayer(row){
  if(serial!==null)return;
  if(!Number.isSafeInteger(row.serial)||row.serial<1||room?.seats?.[seat]?.clientId!==client||room.difficulty!==rules.difficulty||
    room.playerCount!==2||room.inputDelay!==rules.inputDelay||room.predictionLimit!==rules.rollbackWindow||room.gameplayRelayFallback!==true)
    throw new Error('Author Relay run differs from the prepared TH128 room');
  serial=row.serial;phase='routing';publish();
  const query={run:String(serial),players:'2',player:String(seat)};
  let offered=false;
  function offer(){if(!offered){offered=true;match.announceStart();}}
  async function receive(message,from){
    if(config.authorTransport&&!routed){routeName=link.status().route;routed=true;}
    offer();
    await match.receive(message,from);
    if(canRun()&&Number.isInteger(message.frame)){syncClockEpoch();clock.input();}
    else await refresh();
  }
  if(config.authorTransport){
    const selection=readTh128RoomOptions({netplayMode:'lan',netplayUrl:address(query).href,
      netplayPlayer:seat,netplayPlayerCount:room.playerCount,netplaySeed:rules.seed,netplayDifficulty:room.difficulty,
      netplayInputDelay:room.inputDelay,netplayPredictionLimit:room.predictionLimit,netplayAdonisMode:0,
      netplaySpectator:false,netplaySpectatorId:'',netplaySpectatorCount:room.spectatorCount,
      netplayIceServers:[],netplayRouteChoice:'direct',netplayLoadouts:[{character:0,shot:0},{character:0,shot:0}]},resources.build);
    if(JSON.stringify(selection.rules)!==JSON.stringify(rules))throw new Error('The author transport role and preloaded rules differ');
    const {default:factory}=await import(resources.url('peerCore'));
    const module=await factory({wasmBinary:await resources.read('peerWasm'),eaglerOptions:{...selection.transport,netplayAdaptiveTurn:false}});
    signal.throwIfAborted();link=openAuthorPeerLink({module,room:selection,onMessage:receive,signal});
    const deadline=performance.now()+20000;
    async function pump(){
      if(signal.aborted)return;
      try{
        const state=await link.pump();if(signal.aborted)return;
        if(!routed&&state.route!=='connecting'){routed=true;routeName=state.route;offer();await refresh();}
        if(!routed&&performance.now()>=deadline)throw new Error('Shared author room route timed out');
        transportPump=setTimeout(pump,12);
      }catch(error){fail(error);}
    }
    void pump();return;
  }
  link=openRelayLink({url:address(query),localPlayer:seat,requireRoute:true,spectatorUpload:seat===0,onError:fail,onMessage:receive});
  let requested=false;
  signaling=jsonLink(address({...query,signal:'1'}),message=>{
    if(!requested&&message.type==='peers'&&message.peers?.includes(1-seat)){
      // This owned loopback validation explicitly selects the author's fallback.
      requested=true;signaling.send({type:'rtc-failed'});
    }
  });
  await Promise.all([link.ready,signaling.ready,link.routeReady]);signal.throwIfAborted();routed=true;routeName='relay';
  offer();await refresh();
}
async function driveWatch(){
  if(!watchBoot||watchPaused||signal.aborted)return;
  const before=config.inputDiagnostic?performance.now():null;
  while(tracks.length){
    const track=tracks[0],latest=track.status();
    if(!watchWorld){watchWorld=await openWorld(readRules(track.header.rules));watchFrame=0;}
    if(latest.nextFrame>config.limits.roundFrames)throw new Error('Spectator preview exceeds frame budget');
    while(watchFrame<latest.nextFrame){watchWorld.engine.step(track.read(watchFrame));watchFrame++;}
    paint(watchWorld,0,watchFrame);
    if(tracks.length<2||!latest.end)break;
    watchTail={epoch:track.header.rules.epoch,frames:watchFrame,end:latest.end,players:native.players};
    tracks.shift();watchWorld.engine.close();watchWorld=null;watchFrame=0;
  }
  if(before!==null)inputProbe.watchMs=Math.max(inputProbe.watchMs,milliseconds(performance.now()-before));
  phase='watching';publish();
}
function catchUp(){watchChain=watchChain.then(driveWatch);return watchChain;}
async function acceptViewer(row){
  if(receiver)return;
  if(!Number.isSafeInteger(row.serial)||row.serial<1||room?.spectatorCount<1||room?.seats.slice(0,2).some(s=>s?.clientId===client))
    throw new Error('Spectator room did not admit this independent seat');
  serial=row.serial;decoder=createTh128SpectatorDecoder({build:resources.build,matchId:config.matchId});
  receiver=openTh128SpectatorRelay({url:address({run:String(serial),players:'2',spectator:client}),decoder,onError:fail,async onPacket(kind){
    if(kind==='checkpoint'){
      if(tracks.length>=3)throw new Error('Spectator pending epoch limit');tracks.push(decoder.retain());
    }
    if(kind==='bootstrap')watchBoot=true;
    publish();await catchUp();
  }});
  await receiver.ready;
}
async function receiveLobby(row){
  if(row.type==='error')throw new Error(`Author Relay lobby: ${row.error}`);
  if(row.room?.seats){
    if(row.room.playerCount!==2)throw new Error('Local TH128 room must have two player slots');room=row.room;
    if(role==='player'&&room.seats[seat]?.clientId===client&&!resourceReported){
      resourceReported=true;lobby.send({type:'resource-progress',status:'ready',stage:'runtime',percent:100});
    }
  }
  publish();
  if(role==='player'&&row.type==='start')await launchPlayer(row);
  if(role==='spectator'&&row.type==='spectator-start')await acceptViewer(row);
}
async function join(){
  if(phase!=='prepared'||entered)return;phase='initializing';entered=true;publish();
  if(role==='player'){
    rules=readRules({schema:'th128-multiplayer-rules/1',matchId:config.matchId,epoch:1,mode:'parallel-stage',route:'A1-3',seed:34772,
      ...resources.build,inputDelay:0,rollbackWindow:2});
    match=await createPlayerMatch({rules,localPlayer:seat,allowedMask:763,openEngine:openEpoch,signal,maxFrames:config.limits.roundFrames,
      send:message=>link.send(message)});
  }
  lobby=jsonLink(address({lobby:client,intent:seat===0?'create':'join',players:'2',visibility:'private'}),receiveLobby);
  await lobby.ready;signal.throwIfAborted();phase='room';
  lobby.send(role==='player'?{type:'take-seat',seat,loadout:0}:{type:'spectate'});publish();
}
async function replayStore(){
  if(!store)store=await openPreviewReplayStore({indexedDB,build:resources.build,matchId:config.matchId,isCurrent:()=>!signal.aborted});
  return store;
}
async function saveReplay(){
  const data=match.previousReplay();if(!data)throw new Error('No finished multiplayer Replay');
  saved=await (await replayStore()).save(data);publish();
}
const utf8=new TextEncoder(),hashText=async text=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',utf8.encode(text))),byte=>byte.toString(16).padStart(2,'0')).join('');
function clearReplayFrame(){if(replayRaf!==null)cancelAnimationFrame(replayRaf);replayRaf=null;}
function requestReplayFrame(){
  if(replayRaf!==null||signal.aborted||replayPlayback?.status().phase!=='playing')return;
  replayRaf=requestAnimationFrame(time=>{
    replayRaf=null;
    try{if(replayPlayback.advance(time,!document.hidden).advanced)publish();requestReplayFrame();}
    catch(error){fail(error);}
  });
}
async function playArchivedReplay({tape,sha256,name=null}){
  const selection=createMultiplayerRules(tape.header.rules),{nextFrame,end}=tape.status();
  if(nextFrame>config.limits.roundFrames)throw new Error('Replay preview exceeds frame budget');
  if(config.sequenceReplay===true){
    clearReplayFrame();replayPlayback?.close();replayPlayback=null;loaded=null;
    replayPlayback=await openTh128ReplayPlayback({tape,signal,
      openWorld:(_rules,owner)=>openWorld(selection,owner),
      render(world,track,frame){renderedTag='';paint(world,track,frame);}});
    loaded={epoch:selection.epoch,frames:nextFrame,end,sha256,name};
    return;
  }
  const world=await openWorld(selection);
  try{
    for(let frame=0;frame<nextFrame;frame++)world.engine.step(tape.read(frame));
    renderedTag='';paint(world,0,nextFrame);
    const [stateSHA256,drawSHA256]=await Promise.all([
      Promise.all([0,1].map(seat=>hashText(world.engine.stateText(seat)))),
      Promise.all([0,1].map(seat=>hashText(JSON.stringify(world.engine.draws(seat))))),
    ]);
    loaded={epoch:selection.epoch,frames:nextFrame,end,sha256,name,stateSHA256,drawSHA256};
  }finally{world.engine.close();}
}
async function loadReplay(){
  phase='opening-replay';publish();
  const data=await (await replayStore()).read();if(!data)throw new Error('No stored multiplayer Replay for this test match');
  await playArchivedReplay(data);phase='replayed';publish();
}
async function openFile(){
  const input=dom('import-replay'),file=input.files?.[0];input.value='';
  if(!file||role!=='replay')return;
  phase='opening-replay';errorText=null;publish();
  try{await playArchivedReplay(await readTh128ReplayFile(file,resources.build));phase='replayed';}
  catch(error){if(!signal.aborted){errorText=String(error.message??error);phase='prepared';}}
  if(!signal.aborted)publish();
}
async function exportReplay(){
  const data=match.previousReplay();if(!data)throw new Error('No finished multiplayer Replay');
  const file=await makeTh128ReplayFile(data,resources.build);signal.throwIfAborted();
  const url=URL.createObjectURL(file),a=document.createElement('a');a.href=url;a.download=file.name;a.click();
  setTimeout(()=>URL.revokeObjectURL(url),1000);
}
function handle(id,action){dom(id).addEventListener('click',()=>Promise.resolve().then(action).catch(fail));}
handle('join',join);handle('ready',()=>{readySent=true;lobby.send({type:'set-ready',ready:true});publish();});
handle('start',()=>lobby.send({type:'start',adonisMode:0,inputDelay:0,predictionLimit:2}));
for(const [id,value] of [['left',64],['neutral',0],['right',128]])handle(id,()=>{held=value;publish();});
handle('step',async()=>{
  const state=match.status();if(state.round.committedThrough+state.round.pendingFrames+1>=config.limits.roundFrames)throw new Error('Room preview frame budget reached');
  match.tick(held);await refresh();
});
handle('run',()=>{
  if(!continuous()||phase!=='playing'||runRequested)return;
  clockEpoch=match.status().epoch;runRequested=true;screen.focus({preventScroll:true});
  frameRequest=requestAnimationFrame(presentationFrame);publish();
});
handle('restart',async()=>{match.requestRestart();await refresh();});
handle('pause',()=>{watchPaused=true;publish();});handle('live',async()=>{watchPaused=false;await catchUp();});
handle('save-replay',saveReplay);handle('load-replay',loadReplay);
handle('export-replay',exportReplay);dom('import-replay').addEventListener('change',()=>openFile().catch(fail),{signal});
handle('replay-play',()=>{
  if(!replayPlayback)return;
  if(replayPlayback.status().phase==='playing'){replayPlayback.pause();clearReplayFrame();}
  else if(replayPlayback.play())requestReplayFrame();
  publish();
});
handle('replay-again',async()=>{
  if(!replayPlayback)return;
  clearReplayFrame();const restarting=replayPlayback.restart();publish();
  if(await restarting&&replayPlayback.play())requestReplayFrame();
  if(!signal.aborted)publish();
});
handle('replay-seat',()=>{if(replayPlayback?.selectSeat(1-replayPlayback.status().seat))publish();});
document.addEventListener('visibilitychange',()=>{
  if(document.hidden)replayPlayback?.advance(performance.now(),false);
},{signal});
handle('close',()=>{phase='closed';stop();publish();});window.addEventListener('pagehide',()=>{if(!signal.aborted)stop();},{once:true});

try{
  if(!role||location.hostname!=='127.0.0.1')throw new Error('Only the owned loopback TH128 validation room is supported');
  config=JSON.parse(new TextDecoder().decode(await bytes('/preview-config.json')));
  if(continuous())mountControls();
  resources=await openPinnedPreviewResources(config.files,bytes);
  const {default:factory}=await import(resources.url('core'));
  preview=await prepareA13Preview({resources,factory,signal,
    onFactory(kind){budget('factories');if(kind==='background')counts.backgroundFactories++;},
    onWorldStart(){budget('worldStarts');},onTick(){budget('worldTicks');}});
  renderer=new WebGlRenderer(screen);
  for(const [bank,name] of preview.banks){
    const meta=JSON.parse(new TextDecoder().decode(await resources.read(`anim_${name}`)));
    await renderer.loadBank(bank,meta,new URL('/art/',location.href).href,{signal});
  }
  phase='prepared';publish();
}catch(error){fail(error);}
