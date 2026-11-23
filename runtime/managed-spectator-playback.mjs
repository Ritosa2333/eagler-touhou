import {createMatchClock} from './match-clock.mjs';

const restoreBatch=4,trackLimit=3,maxFrames=216000;
const integer=(value,min,max)=>Number.isSafeInteger(value)&&value>=min&&value<=max;
const invalid=message=>Object.assign(new Error(message),{errno:'invalid-field'});

// One read-only receiver owns its presentation cursor and temporary Worlds.
// None of these commands touch the decoder's received data or either player.
export function createManagedSpectatorPlayback({run,openWorld,render,onState,onError,onPlayedEnd=()=>{},signal}){
  if(!integer(run,1,0x7fffffff)||[openWorld,render,onState,onError].some(fn=>typeof fn!=='function'))
    throw new TypeError('TH128 spectator playback requires an owned room and renderer');
  signal?.throwIfAborted();
  const lifetime=new AbortController(),queue=[],exported=new WeakSet();
  let scene=null,opening=null,bootstrapped=false,closed=false,position=0,revision=0,reported=null;
  let paused=false,following=true,restore=null,liveGoal=null,initial=true,lastTime=null,lastReportAt=null;
  const track=()=>queue[0],runId=()=>`th128-${run}-${track().rules.epoch}`;
  const received=()=>track().track.status().nextFrame;
  function status(){
    if(closed||!bootstrapped||!track())return null;
    const {nextFrame:edge,end}=track().track.status(),{stageId,route}=track().rules;
    return {version:1,run:runId(),revision,min:0,position,received:edge,edge,target:restore?.frame??(following?edge:position),
      paused,seeking:restore!==null,following,ended:!!end&&position===edge&&queue.length===1,
      nodes:[{id:1,kind:'stage',frame:0,stage:stageId,bossSlots:0,label:route}]};
  }
  function publishPlayedTail(){const owned=track()?.track;if(!owned||initial||closed||restore||exported.has(owned))return;
    const {end,nextFrame}=owned.status();if(!end||position!==nextFrame)return;exported.add(owned);onPlayedEnd(owned,end,scene,position);}
  function report(force=false){
    const state=status();if(!state)return;publishPlayedTail();
    const timeDue=lastTime!==null&&(lastReportAt===null||lastTime<lastReportAt||lastTime-lastReportAt>=125);
    if(!force&&!timeDue&&reported?.run===state.run&&Math.abs(state.position-reported.position)<6&&state.received-reported.received<6&&
       reported.ended===state.ended&&!(state.position!==reported.position&&state.position===state.edge))return;
    if(reported&&Object.keys(state).filter(key=>key!=='nodes'&&key!=='revision').every(key=>state[key]===reported[key]))return;
    lastReportAt=lastTime;reported=Object.freeze({...state,revision:++revision});onState(reported);
  }
  function step(){
    const row=track().track.read(position);
    if(row?.frame!==position)throw new Error('TH128 spectator confirmed timeline changed');
    scene.step(row);position++;
  }
  const clock=createMatchClock({stepFrame(){
    if(position===received())return {advanced:false,reason:'waiting-input'};
    step();return {advanced:true};
  }});
  function closeScene(){const old=scene;scene=null;old?.close();}
  function close(){
    if(closed)return;closed=true;signal?.removeEventListener('abort',close);lifetime.abort(signal?.reason);clock.reset();
    try{closeScene();}finally{queue.length=0;}
  }
  function acquire(){
    const owned=track();
    opening=Promise.resolve().then(()=>openWorld(owned.rules,lifetime.signal)).then(value=>{
      if(closed||track()!==owned){value.close();return;}
      scene=value;
    }).catch(error=>{if(!closed){close();onError(error);}}).finally(()=>{opening=null;});
  }
  function enqueue(timeline,rules){
    if(closed)return;
    if(queue.length>=trackLimit)throw new Error('TH128 spectator retained track limit');
    const prior=queue.at(-1)?.rules.epoch;
    if(!integer(rules?.epoch,1,0x7fffffff)||!integer(rules.stageId,0,65535)||typeof rules.route!=='string'||
       prior!==undefined&&rules.epoch!==prior+1||typeof timeline?.status!=='function'||typeof timeline.read!=='function')
      throw invalid('Invalid TH128 spectator checkpoint');
    queue.push({track:timeline,rules});report();
  }
  function bootstrap(epoch,frame){
    if(closed||bootstrapped)return;
    if(!track()||track().rules.epoch!==epoch||!integer(frame,0,Math.min(maxFrames,received())))
      throw invalid('Invalid TH128 spectator bootstrap frontier');
    bootstrapped=true;restore={frame};report(true);
  }
  function seek(frame){
    if(frame<position){closeScene();position=0;}
    restore={frame};clock.reset();
  }
  function command(data){
    if(!status()||data?.run!==runId())throw invalid('Stale TH128 spectator playback command');
    if(initial)throw invalid('Initial confirmed TH128 spectator frame is still restoring');
    if(!['pause','play','seek','live'].includes(data.action))throw invalid('Invalid TH128 spectator playback action');
    if(data.action==='seek'){
      if(!integer(data.frame,0,Math.min(maxFrames,received())))throw invalid('Target is outside confirmed spectator history');
      following=false;liveGoal=null;seek(data.frame);
    }else if(data.action==='live'){
      const latest=queue.at(-1);
      paused=false;following=true;
      liveGoal=latest===track()?null:{epoch:latest.rules.epoch,frame:latest.track.status().nextFrame};
      seek(received());
    }else{paused=data.action==='pause';clock.reset();}
    report(true);
  }
  function advance(time,visible=true){
    if(closed||!bootstrapped||!track())return;
    if(!visible){clock.reset();return;}
    lastTime=time;
    if(!scene){if(!opening)acquire();return;}
    const {rules}=track();
    if(restore){
      const through=Math.min(restore.frame,position+restoreBatch);
      while(position<through)step();
      if(position===restore.frame){
        render(scene,position,rules.epoch);initial=false;restore=null;clock.reset();report(true);
      }else report();
      return;
    }
    if(!paused){
      if(clock.advance(time,true).advanced)render(scene,position,rules.epoch);
      if(queue.length>1&&track().track.status().end&&position===received()){
        publishPlayedTail();closeScene();queue.shift();position=0;clock.reset();
        const epoch=track().rules.epoch;
        restore={frame:liveGoal?.epoch===epoch?liveGoal.frame:0};
        if(liveGoal&&liveGoal.epoch<=epoch)liveGoal=null;
        report(true);return;
      }
    }
    report();
  }
  signal?.addEventListener('abort',close,{once:true});
  return Object.freeze({enqueue,bootstrap,command,advance,status,close,settled:()=>opening});
}
