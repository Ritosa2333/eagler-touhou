import {inspectReplay} from './replay.mjs';
import {userPath} from './storage.mjs';

// A selected native stage is the Replay workspace's timeline. Seeking always
// reconstructs its saved initial state and consumes the recorded input stream.
export function createPlayback(api){
 let state=null,busy=false,serial=0,lastPublished=0;
 const publish=()=>{if(state)api.emit('runtime-info',{playback:{...state}});};
 function inspect(path){path=userPath(path);const bytes=api.read(path),description=(api.inspect??inspectReplay)(bytes);if(api.validate&&!api.validate(path))throw Error('Native Replay validation failed');return {path,description};}
 function restart(refresh=false){api.clear();if(!api.open(state.path,state.stage-1,refresh))throw Error('Native Replay initialization failed');state.frame=0;state.ended=false;api.resetClock();}
 function tick(){
  if(!state||state.ended)return false;
  if(!api.step())throw Error('Native Replay frame failed');
  state.frame=Math.min(state.total,api.frame());
  if(state.frame>=state.total||api.scene&&api.scene()!==4||api.stage()!==state.stage-1){state.frame=state.total;state.ended=true;state.paused=true;api.gate(true);publish();}
  return true;
 }
 async function seek(value){
  if(!Number.isSafeInteger(value)||value<0||value>state.total)throw Error('Invalid Replay target');
  const operation=++serial;busy=true;state.seeking=true;state.target=value;api.gate(true);publish();
  try{restart();let iterations=0;while(state.frame<value&&!state.ended){for(let n=0;n<128&&state.frame<value&&!state.ended;n++){tick();if(++iterations>state.total+2000)throw Error('Replay timeline stopped progressing');}publish();await api.yield();if(operation!==serial)return;}}
  finally{if(operation===serial){busy=false;state.seeking=false;state.target=null;api.draw();api.resetClock();api.gate(state.paused||state.ended);publish();}}
 }
 return {
  active:()=>!!state,
  advancing:()=>!!state&&!busy&&!state.paused&&!state.ended,
  rate:()=>state?.rate??1,
  tick,
  update(time){if(state&&time-lastPublished>=100){lastPublished=time;publish();}},
  async command(message){
   if(message.action==='inspect')return {description:inspect(message.path).description};
   if(busy)throw Error('Replay is seeking');
   if(message.action==='open'){
    const {path,description}=inspect(message.path),stage=message.stage??description.stages[0].id,entry=description.stages.find(s=>s.id===stage);
    if(!entry||!entry.frames)throw Error('Replay stage is empty');
    state={kind:'replay',path,description,stage,frame:0,total:entry.frames,startFrame:0,paused:false,seeking:false,target:null,rate:1,ended:false,difficulty:description.difficulty,left:description.character,right:0};
    api.gate(true);restart(true);api.draw();api.gate(false);publish();return {playback:{...state}};
   }
   if(!state)throw Error('Open a Replay first');
   if(message.action==='pause'){state.paused=true;api.clear();api.gate(true);}
   else if(message.action==='play'){if(state.ended)await seek(0);state.paused=false;state.ended=false;api.resetClock();api.gate(false);}
   else if(message.action==='seek')await seek(message.value);
   else if(message.action==='rate'){if(![0,.5,1,2,4,8].includes(message.value))throw Error('Invalid Replay rate');state.rate=message.value;api.resetClock();}
   else throw Error('Invalid Replay action');
   publish();return {playback:{...state}};
  },
 };
}
