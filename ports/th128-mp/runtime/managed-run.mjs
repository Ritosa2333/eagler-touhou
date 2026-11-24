import {mainEndingStages,openCoopEnding} from './coop-ending.mjs';
import {createCoopPauseMenu} from './coop-pause-menu.mjs';
import {openCoopSound} from './coop-sound.mjs';
import {historyRules,historyWire} from './confirmed-history.mjs';
import {createCoopMatch} from './coop-match.mjs';
import {TH128_COOP_FRAME_LIMIT} from './coop-rules.mjs';
import {analogGamepadDirections,mappedGamepadCodes} from '../../th128/site/runtime/gamepad-input.mjs';
import {createPlayerMatch} from './player-match.mjs';
import {createMatchClock} from './match-clock.mjs';
import {createSeatInput,TH128_HELD} from './seat-input.mjs';
import {createTh128SpectatorDecoder,createTh128SpectatorEncoder} from './spectator-wire.mjs';
import {openTh128SpectatorRelay} from './spectator-link.mjs';
import {createManagedSpectatorPlayback} from './managed-spectator-playback.mjs';

const heldMask=763,controlContact=Number.MIN_SAFE_INTEGER;

export async function launchManagedRun({room,view,build,signal,openPeer,present,onError,onStatus=()=>{},onReplay=()=>{},canvas,soundResources,audioEventTargets,musicEnabled=false,onLocalResult=()=>{},onNativeResult=()=>{},openPauseArt,openReplaySave,onReplaySaveActive=()=>{},openEnding=openCoopEnding}){
  const scope=new AbortController(),live=AbortSignal.any([signal,scope.signal]),scenes=new Map();
  let match=null,link=null,viewer=null,playback=null,sounds=null,renderFrame=null,networkTask=null,closed=false,errorAfterClose=false;
  let readyClock=null,shown=false,path=null;
  let inputEpoch=null,audioEpoch=null;
  const player=room.role==='player',input=player?createSeatInput({allowedMask:heldMask,controllerIndex:room.seat}):null;
  const contacts=new Map();let controlSerial=null,restartHeld=false,lastPaused=false,pauseArt=null;
  const cooperative=room.rules.mode==='same-stage-coop';
  let terminal=null,viewerFrame=null,saveOnFinish=false,ending=null,replaySave=null;
  const replaySaveKeys=new Set(['KeyZ','KeyC','ControlLeft','Enter','Escape','KeyX','ArrowUp','ArrowDown','ArrowLeft','ArrowRight']);
  function closeReplaySave(){const old=replaySave;if(!old)return;replaySave=null;old.scope.abort();pauseMenu?.clearInput();onReplaySaveActive(false);}
  function startReplaySave(value,gameOver,postrun=false){
    if(replaySave||!value.replayBytes||typeof openReplaySave!=='function')return false;
    const owner={epoch:value.epoch,gameOver,postrun,value,scope:new AbortController(),menu:null};replaySave=owner;onReplaySaveActive(true);
    const signal=AbortSignal.any([owner.scope.signal,live]);clearInput();
    void openReplaySave(value.replayBytes,{gameOver,postrun,signal,sound:id=>sounds?.menuCue(id)}).then(menu=>{
      if(replaySave===owner&&!signal.aborted)owner.menu=menu;
    }).catch(error=>{if(replaySave===owner&&!signal.aborted){
      closeReplaySave();onStatus({replaySaveError:String(error.message??error)});
      if(postrun)onNativeResult('end',value);
    }});
    return true;
  }
  function drawClearSave(epoch,now){
    const owner=replaySave;if(!owner?.postrun)return false;
    if(owner.epoch!==epoch){closeReplaySave();return false;}
    if(owner.returning||document.hidden)return true;
    const save=owner.menu?.frame(now)??{phase:0,slot:0,revision:0,records:[]};
    if(save.phase===5){owner.returning=true;onNativeResult('end',owner.value);return true;}
    view.displayReplaySave(save,pauseArt?.postrunFrame(save,now)??save.records);return true;
  }
  function replaySaveSelection(selection,epoch,now){
    if(replaySave&&replaySave.epoch!==epoch)closeReplaySave();
    if(!replaySave)return selection;
    const save=replaySave.menu?.frame(now)??{phase:0,revision:0,slot:0,records:[]};
    if(save.phase===5){closeReplaySave();return selection;}
    return {kind:replaySave.gameOver?'game-over':'pause',selection:'save',replaySave:save};
  }
  const pauseMenu=cooperative?createCoopPauseMenu(action=>{
    if(action==='pause'||action==='resume')match.togglePause();
    else if(action==='restart')match.requestRestart();
    else if(action==='end'||action==='save'){
      saveOnFinish=action==='save';if(!match.requestFinish())saveOnFinish=false;
    }else if(terminal&&action==='terminal-restart'&&player)match.requestRestart();
    else if(terminal&&action==='terminal-save')startReplaySave(terminal,true);
    else if(terminal&&action.startsWith('terminal-'))onNativeResult(action.slice(9),terminal);
  },id=>sounds?.menuCue(id)):null;
  function syncPlayerAudio(state){
    if(!cooperative||!sounds)return;
    const phase=state.history.end||audioEpoch!==null&&state.epoch!==audioEpoch?'stopped':
      state.phase==='paused'||state.phase==='voting'?'paused':'playing';
    sounds.setBattlePhase(phase);
  }
  function clearInput(){input?.clear();ending?.owner?.reset();contacts.clear();restartHeld=false;controlSerial=null;}
  function adoptPlayerEpoch(state=match?.status()){
    if(!readyClock||!state?.startup.canStart||inputEpoch===state.epoch)return;
    // Ready and restart acknowledgements can commit between animation frames.
    // Retire old holds first, not the fresh inputs arriving after that commit.
    inputEpoch=state.epoch;readyClock.reset();clearInput();
  }
  function close(){
    if(closed)return;closed=true;signal.removeEventListener('abort',close);
    if(renderFrame!==null)cancelAnimationFrame(renderFrame);if(networkTask!==null)clearTimeout(networkTask);
    scope.abort();closeReplaySave();stopEnding();sounds?.close();pauseArt?.close();clearInput();
    try{link?.close();viewer?.close();}finally{try{match?.close();}finally{playback?.close();}}
  }
  function fail(error){
    if(closed||signal.aborted||errorAfterClose)return;
    errorAfterClose=true;
    try{close();}finally{onError(error);}
  }
  signal.throwIfAborted();signal.addEventListener('abort',close,{once:true});
  function draw(world,seat,confirmed,revision,menu){
    if(world.display(seat,confirmed,revision,menu)&&!shown)shown=present(true,path)===true;
  }
  function stopEnding(){const old=ending;if(!old)return;ending=null;old.scope.abort();old.owner?.close();sounds?.finishEnding();}
  function clearSave(value){
    stopEnding();if(!startReplaySave(value,false,true))onNativeResult('end',value);
  }
  function sealed(value,scene){
    terminal=value.result==='team-over'?value:null;
    if(value.result!=='cleared'){onLocalResult(value);return;}
    if(!scene?.startEnding||!mainEndingStages.includes(scene.state().stageId)){clearSave(value);return;}
    const body={scope:new AbortController(),owner:null,value};ending=body;
    const endingSignal=AbortSignal.any([live,body.scope.signal]);
    void openEnding({scene,resources:soundResources,signal:endingSignal,view,sound:sounds,
      onComplete:()=>{if(ending===body&&!live.aborted)clearSave(value);}}).then(owner=>{
        if(ending===body&&!live.aborted)body.owner=owner;else owner.close();
      }).catch(error=>{if(!endingSignal.aborted){onStatus({endingError:String(error.message??error)});clearSave(value);}});
  }
  function drawEnding(now){
    if(!ending)return false;
    const body=ending;
    void body.owner?.advance(now,!document.hidden).catch(error=>{
      if(ending===body&&!live.aborted){onStatus({endingError:String(error.message??error)});clearSave(body.value);}
    });
    return true;
  }
  function requestFrame(frame){
    renderFrame=requestAnimationFrame(now=>{
      renderFrame=null;if(live.aborted)return;
      try{frame(now);if(!live.aborted)requestFrame(frame);}catch(error){fail(error);}
    });
  }
  function restart(){if(match?.status().startup.canStart)match.requestRestart();}
  function key(value){
    if(live.aborted)return false;
    if(replaySave)return (!replaySave.returning&&replaySave.menu?replaySave.menu.key(value):replaySaveKeys.has(value.code));
    if(ending){return ending.owner?.key(value)??true;}
    if(!player)return terminal&&viewerFrame?.world.outcome?.()==='team-over'?pauseMenu.key(value):false;
    adoptPlayerEpoch();
    if(pauseMenu&&match){const state=match.status();syncPlayerAudio(state);pauseMenu.sync(state);if(pauseMenu.key(value))return true;}
    if(value.code==='KeyR'){
      if(value.down&&!restartHeld&&!value.repeat)restart();restartHeld=value.down;return true;
    }
    return input.key(value.code,value.down);
  }
  function cancelTouch(){
    for(const id of contacts.keys())input.touch(id,0);
    contacts.clear();input.touch(controlContact,0);controlSerial=null;
  }
  function directTouch({type,id,x,y}){
    if(live.aborted||!player)return;
    adoptPlayerEpoch();
    if(type==='down'&&contacts.size<16)contacts.set(id,[x,y]);
    else if(type==='move'&&contacts.has(id)){
      const [startX,startY]=contacts.get(id);input.touch(id,analogGamepadDirections((x-startX)*12,(y-startY)*12));
    }else if(type==='up'||type==='cancel'){contacts.delete(id);input.touch(id,0);}
  }
  function touchControls(value){
    if(live.aborted||!player)return;
    adoptPlayerEpoch();
    const buttons=(value.fireEnabled?1:0)|(value.focusEnabled?8:0);
    const joystick=analogGamepadDirections(value.joystickX/32767,value.joystickY/32767);
    input.touch(controlContact,buttons|joystick);
    if(controlSerial!==null&&controlSerial<value.bombSerial){input.key('KeyX',true);input.key('KeyX',false);}
    controlSerial=value.bombSerial;
  }
  function sample(){
    if(pauseMenu?.status())return 0;
    const mapped=mappedGamepadCodes(window);
    let bits=0;if(mapped){for(const code of mapped)bits|=TH128_HELD[code]??0;}
    const pads=!mapped&&document.hasFocus()?navigator.getGamepads?.()??[]:[];
    return input.read(pads)|bits;
  }
  function attachKeyboard(){
    const options={signal:live};
    function local(e,down){
      if(e.isTrusted&&!e.altKey&&!e.metaKey&&(!e.ctrlKey||Object.hasOwn(TH128_HELD,e.code))&&(e.target===canvas||e.target===document.body||!down)&&key({code:e.code,down,repeat:e.repeat}))e.preventDefault();
    }
    window.addEventListener('keydown',event=>local(event,true),options);
    window.addEventListener('keyup',event=>local(event,false),options);
    window.addEventListener('blur',()=>{if(input){input.focusLost();cancelTouch();}ending?.owner?.reset();replaySave?.menu?.clear();restartHeld=false;pauseMenu?.clearInput();},options);
    document.addEventListener('visibilitychange',()=>{if(document.hidden){readyClock?.reset();replaySave?.menu?.clear();clearInput();}},options);
  }

  async function startPlayer(){
    const publisher=room.seat===0;
    let presentedResult=null,offered=false,upload=null,observerFailed=false,capturedEpoch=room.rules.epoch-1;
    function reportArchive(){
      const receipt=match.replayReceipt();
      if(!receipt||receipt.epoch<=capturedEpoch)return;
      const archive=match.previousReplay();
      if(!archive)return;
      capturedEpoch=receipt.epoch;
      try{onReplay(archive);}catch{onStatus({replayUnavailable:true});}
    }
    function offer(){if(!offered){offered=true;match.announceStart();}}
    const createMatch=room.rules.mode==='same-stage-coop'?createCoopMatch:createPlayerMatch;
    match=await createMatch({rules:room.rules,localPlayer:room.seat,allowedMask:heldMask,signal:live,
      onConfirmed(rules,_row,effects){
        if(!sounds)return;
        if(audioEpoch!==rules.epoch){
          if(audioEpoch!==null)sounds.setBattlePhase('stopped');
          audioEpoch=rules.epoch;sounds.setBattlePhase('playing');
        }
        sounds.commit(effects?.sounds,effects);
        if(effects?.teamOver||effects?.cleared)sounds.setBattlePhase('stopped');
      },async openEngine(rules,ownerSignal){
        const owned=await view.openWorld(rules,ownerSignal);scenes.set(rules.epoch,owned);
        return Object.freeze({...owned,close(){if(scenes.get(rules.epoch)===owned)scenes.delete(rules.epoch);owned.close();}});
      },send:packet=>link.send(packet)});
    live.throwIfAborted();
    link=await openPeer(room,async(message,from)=>{
      offer();await match.receive(message,from);
      if(!live.aborted){const state=match.status();syncPlayerAudio(state);adoptPlayerEpoch(state);reportArchive();}
    },live);
    if(live.aborted){link.close();live.throwIfAborted();}
    readyClock=createMatchClock({stepFrame:options=>match.tick(sample,options)});attachKeyboard();

    async function publishSpectator(){
      if(!publisher||observerFailed||!match.status().startup.canStart)return;
      try{
        const current=match.spectatorSource();
        function drain(owner){
          if(owner.ended)return;
          const {nextFrame:through,end}=owner.source.status();
          if(through-owner.next>128)throw new Error('旁观发送队列已满');
          for(;owner.next<through;owner.next++)link.sendSpectator(owner.encoder.frame(owner.source.read(owner.next)));
          if(end){link.sendSpectator(owner.encoder.finish(end));owner.ended=true;}
        }
        if(upload?.source.header.rules.epoch!==current.header.rules.epoch){
          if(upload){drain(upload);if(!upload.ended)throw new Error('旧局没有已确认的旁观结束记录');}
          const encoder=await createTh128SpectatorEncoder(current.header,build);live.throwIfAborted();
          link.sendSpectator(encoder.announce());link.sendSpectator(encoder.open());upload={source:current,encoder,next:0,ended:false};
        }
        drain(upload);
      }catch(error){if(!live.aborted){observerFailed=true;onStatus({spectatorUnavailable:true});}}
    }
    let readyEpoch=null,deadline=performance.now()+25000;
    async function network(){
      try{
        const current=await link.pump();if(live.aborted)return;
        if(current.route!=='connecting'){
          if(path!==current.route){path=current.route;onStatus({netplayTransport:path});}
          offer();const state=match.flush();syncPlayerAudio(state);adoptPlayerEpoch(state);reportArchive();
          if(!document.hidden&&inputEpoch===state.epoch&&!state.history.end)readyClock.input();
          await publishSpectator();
        }
        const status=match.status();
        if(status.epoch!==readyEpoch){readyEpoch=status.epoch;deadline=performance.now()+25000;}
        if(!status.startup.canStart&&performance.now()>deadline)throw new Error('联机双方未在时限内就绪');
        if(!live.aborted)networkTask=setTimeout(network,8);
      }catch(error){
        // After a confirmed result, peers can independently return to their
        // rooms. A sealed tape is local and must survive the RTC lane closing.
        if(!closed&&cooperative){
          const state=match.status();
          if(state.result&&state.history.end&&match.replayReceipt()?.epoch===state.epoch)return;
        }
        fail(error);
      }
    }
    networkTask=setTimeout(network,0);
    requestFrame(now=>{
      const state=match.status();syncPlayerAudio(state);if(!state.startup.canStart)return;
      adoptPlayerEpoch(state);reportArchive();
      if(cooperative){
        const key=`${state.epoch}/${state.result??state.phase}`;
        if(key!==presentedResult){presentedResult=key;
          const replayBytes=state.result&&match.replayReceipt()?.epoch===state.epoch?match.previousReplay():null;
          const value={active:true,epoch:state.epoch,result:state.result,replayBytes};
          sealed(value,scenes.get(state.epoch));
          if(state.result==='left'&&saveOnFinish){saveOnFinish=false;startReplaySave(value,false);}
        }
        pauseMenu.sync(state);
        if(lastPaused!==(state.phase==='paused')){lastPaused=state.phase==='paused';input.clear();cancelTouch();readyClock.reset();}
      }
      const active=!document.hidden&&!state.history.end&&['running','voting'].includes(state.phase);
      if(readyClock.advance(now,active).stalled){input.presentationStall();cancelTouch();}
      // A restart promoted by a clock callback adopts its own scene before paint.
      const current=match.status();syncPlayerAudio(current);adoptPlayerEpoch(current);const world=scenes.get(current.epoch);
      if(drawEnding(now)||drawClearSave(current.epoch,now))return;
      if(world&&!document.hidden){
        pauseMenu?.sync(current);const selection=replaySaveSelection(pauseMenu?.status(),current.epoch,now);
        const art=pauseArt?.frame(selection,current.history.nextFrame,now);
        draw(world,room.seat,current.history.nextFrame,current.round.replayedFrames,art?{...art,...selection}:null);
      }
    });
  }

  async function startViewer(){
    const decoder=createTh128SpectatorDecoder({build,matchId:room.rules.matchId,expectedRules:room.rules});
    playback=createManagedSpectatorPlayback({run:room.run,signal:live,
      openWorld:(rules,owner)=>view.openWorld(rules,owner,{confirmedOnly:true,seatCues:false}),render:(scene,frame,epoch)=>{viewerFrame={world:scene,frame,epoch};draw(scene,0,frame,epoch);},
      onPlayedEnd:(track,end,scene,frame)=>{
        const replayBytes=track.replayBytes();onReplay(replayBytes);
        if(end!=='restart'&&cooperative){
          const result=scene.outcome?.()??(end==='completed'&&frame===TH128_COOP_FRAME_LIMIT?'time-limit':end);sounds?.setBattlePhase('stopped');
          const value={active:true,epoch:track.header.rules.epoch,result,replayBytes};
          sealed(value,scene);
        }
      },onState:spectatorPlayback=>onStatus({spectatorPlayback}),onError:fail});
    function addCheckpoint(){
      const track=decoder.retain();
      const selected=historyRules(track.header.rules),expected=historyWire(room.rules);
      const actual=historyWire(selected);
      if(JSON.stringify({...actual,epoch:expected.epoch})!==JSON.stringify(expected)||selected.rollbackWindow>2)
        throw new Error('旁观记录与房间规则不符');
      playback.enqueue(track,selected);
    }
    viewer=openTh128SpectatorRelay({url:room.relayUrl,players:room.rules.players,decoder,onError:fail,onPacket(kind){
      if(kind==='checkpoint')addCheckpoint();
      if(kind==='bootstrap'){const state=decoder.status();playback.bootstrap(state.epoch,state.nextFrame);}
    }});
    await viewer.ready;live.throwIfAborted();path='spectator-relay';onStatus({netplayTransport:'spectator'});
    window.addEventListener('keydown',event=>{
      const state=playback.status();
      if(shown&&state&&event.isTrusted&&event.code==='Space'&&!event.repeat){playback.command({run:state.run,action:state.paused?'play':'pause'});event.preventDefault();}
    },{signal:live});
    document.addEventListener('visibilitychange',()=>{if(document.hidden)playback.advance(performance.now(),false);},{signal:live});
    if(cooperative)attachKeyboard();
    requestFrame(now=>{
      if(drawEnding(now)||drawClearSave(viewerFrame?.epoch,now))return;
      playback.advance(now,!document.hidden);
      if(!pauseArt||document.hidden)return;
      const snapshot=playback.status(),owner=viewerFrame;
      const final=terminal&&owner?.epoch===terminal.epoch&&snapshot?.ended&&!snapshot.seeking&&owner.world.outcome?.()==='team-over';
      pauseMenu.sync({phase:final?'ended':'waiting',result:final?'team-over':null,epoch:owner?.epoch,startup:{canStart:true}});
      const selection=replaySaveSelection(pauseMenu.status(),owner?.epoch,now),art=pauseArt.frame(selection,owner?.frame,now);
      if(art)draw(owner.world,0,owner.frame,owner.epoch,{...art,...selection});
    });
  }
  try{
    if(cooperative&&soundResources)sounds=await openCoopSound({resources:soundResources,signal:live,eventTargets:audioEventTargets,musicEnabled,stageEnabled:player});
    if(pauseMenu&&openPauseArt)pauseArt=await openPauseArt(live);if(player)await startPlayer();else await startViewer();
  }
  catch(error){close();throw error;}
  return Object.freeze({close,key,requestFinish:()=>!live.aborted&&player&&match?.requestFinish?.()===true,clearInput,directTouch,touchControls,touchCancel:cancelTouch,
    ...(playback?{spectatorPlayback(value){
      if(!shown)throw Object.assign(new Error('The initial TH128 spectator frame has not presented'),{errno:'invalid-state'});
      const result=playback.command(value);stopEnding();if(replaySave?.postrun)closeReplaySave();return result;
    }}:{})});
}
