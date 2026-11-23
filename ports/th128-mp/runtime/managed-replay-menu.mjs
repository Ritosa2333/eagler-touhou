import {readTh128ReplayFile} from './replay-file.mjs';
import {openTh128ReplayPlayback} from './replay-playback.mjs';

/** Document-local presentation owner. The Launcher room retains priority and
 * can cancel every read, candidate World and pending title restoration. */
export function createManagedReplayMenu({build,signal,openWorld,render,restoreTitle,onChange=()=>{},readFile=readTh128ReplayFile}){
  if([openWorld,render,restoreTitle,onChange,readFile].some(fn=>typeof fn!=='function'))throw new TypeError('Incomplete Replay menu owner');
  signal?.throwIfAborted();
  let enabled=false,away=false,closed=false,pending=null,active=null,returning=null,error=null,last='';
  const ticket=()=>({scope:new AbortController()});
  const status=()=>Object.freeze({enabled,away,loading:!!pending,returning:!!returning,filename:active?.name??null,
    playback:active?.playback.status()??null,error});
  function report(){
    const state=status(),key=JSON.stringify(state);if(last===key)return;
    last=key;onChange(state);
  }
  function retire(){
    const owners=[pending,returning,active];pending=returning=active=null;
    for(const owner of owners)owner?.scope.abort();
  }
  function suspend(){enabled=false;away=false;error=null;retire();report();}
  function close(){
    if(closed)return;
    closed=true;signal?.removeEventListener('abort',close);suspend();
  }
  function enableTitle(){if(!closed){retire();explicit=false;enabled=true;away=false;error=null;report();}}
  let explicit=false;
  function enter(){if(!closed){retire();explicit=true;enabled=away=true;error=null;report();}}
  const same=(owner,kind)=>!closed&&!owner.scope.signal.aborted&&(kind==='open'?pending===owner:returning===owner);
  async function open(file){
    if(!enabled||closed||returning)return false;
    pending?.scope.abort();const owner=ticket();pending=owner;error=null;report();
    try{
      const parsed=await readFile(file,build);
      if(!same(owner,'open'))return false;
      const playback=await openTh128ReplayPlayback({tape:parsed.tape,openWorld,signal:owner.scope.signal,
        render(scene,seat,frame){if(same(owner,'open')||active===owner&&!owner.scope.signal.aborted)render(scene,seat,frame);}});
      if(!same(owner,'open')){playback.close();return false;}
      const previous=active;owner.playback=playback;owner.name=parsed.name;active=owner;away=true;previous?.scope.abort();
      return true;
    }catch(failure){if(same(owner,'open')){error=String(failure?.message??failure);owner.scope.abort();}return false;}
    finally{if(pending===owner){pending=null;report();}}
  }
  async function act(action,value){
    if(!active||!enabled||pending||returning)return false;
    const owner=active,playback=owner.playback;error=null;
    try{
      const operation=action==='seat'?playback.selectSeat(value):action==='seek'?playback.seek(value):
        action==='play'?playback.play():action==='pause'?playback.pause():action==='restart'?playback.restart():false;
      report();const accepted=await operation;
      return active===owner&&accepted;
    }catch(failure){if(active===owner)error=String(failure?.message??failure);return false;}
    finally{if(active===owner)report();}
  }
  function advance(time,visible=true){
    const owner=active;if(!owner)return;
    try{owner.playback.advance(time,visible&&!pending);}
    catch(failure){if(active===owner){active=null;owner.scope.abort();error=String(failure?.message??failure);}}
    report();
  }
  async function returnToTitle(){
    if(closed||!enabled||!away||returning)return false;
    retire();const owner=ticket();returning=owner;error=null;report();
    try{
      const shown=await restoreTitle(owner.scope.signal);
      if(!same(owner,'return'))return false;
      if(shown!==true)throw new Error('The native TH128 title did not present');
      away=false;if(explicit)enabled=false;return true;
    }catch(failure){if(same(owner,'return'))error=String(failure?.message??failure);return false;}
    finally{if(returning===owner){returning=null;report();}}
  }
  signal?.addEventListener('abort',close,{once:true});
  return Object.freeze({status,enter,enableTitle,suspend,open,act,advance,returnToTitle,close});
}
