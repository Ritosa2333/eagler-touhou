import {TH128_COOP_FRAME_LIMIT} from './coop-rules.mjs';

// Ordered control channel; rows inside the pause are never sampled, written to Replay or sent to viewers.
// All seats stop at max(already simulated cursor), then publish only real missing inputs to reach it.
export const PAUSE_WIRE='th128-coop-pause/1';
export function createCoopPause({players,seat,epoch,key,send,position,confirmed}){
 let serial=0,active=null,queued=new Map();
 const emit=(phase,frame)=>send({schema:PAUSE_WIRE,key,epoch,seat,serial:active.id,phase,frame});
 function begin(){if(active)return;const f=position();active={id:serial+1,target:null,offers:new Map([[seat,f]]),arrivals:new Map(),resumes:new Set(),wantResume:false,since:performance.now()};emit('stop',f);}
 function set(m,s,v){if(m.has(s)&&m.get(s)!==v)throw Error('Pause cursor changed');m.set(s,v);}
 function handle(msg,from){if(!active){if(msg.phase!=='stop')throw Error('Pause without an offered frontier');begin();}
  const a=active;if(msg.phase==='stop')set(a.offers,from,msg.frame);
  else{if(!a.offers.has(from))throw Error('Sender has no pause frontier');if(msg.phase==='held')set(a.arrivals,from,msg.frame);
    else{if(a.arrivals.get(from)!==msg.frame)throw Error('Unexpected pause release');a.wantResume=true;a.resumes.add(from);}}
 }
 function progress(){const a=active;if(!a)return;
  if(!paused()&&performance.now()-a.since>30000)throw Error('Co-op pause frontier timed out');
  if(a.offers.size===players&&a.target===null){a.target=Math.max(...a.offers.values());if(a.target-Math.min(...a.offers.values())>60)throw Error('Pause outside co-op frame window');}
  if(a.target===null)return;
  for(const f of a.arrivals.values())if(f!==a.target)throw Error('Co-op pause confirmed a different frame');
  if(confirmed()>a.target)throw Error('Co-op pause passed its fence');
  if(confirmed()===a.target&&!a.arrivals.has(seat)){a.arrivals.set(seat,a.target);emit('held',a.target);}
  if(!paused()||!a.wantResume)return;
  if(!a.resumes.has(seat)){a.resumes.add(seat);emit('resume',a.target);}
  if(a.resumes.size===players){serial=a.id;active=null;const early=queued;queued=new Map();for(const [who,off] of early)handle(off,who);if(active)progress();}
 }
 function paused(){return !!active&&active.arrivals.size===players&&active.target!==null;}
 function toggle(){if(!active)begin();else if(paused())active.wantResume=true;progress();}
 function receive(msg,from){if(msg?.schema!==PAUSE_WIRE||msg.epoch!==epoch||msg.key!==key||msg.seat!==from||!Number.isInteger(from)||from===seat||from<0||from>=players||
   !['stop','held','resume'].includes(msg.phase)||!Number.isInteger(msg.frame)||msg.frame<0||msg.frame>TH128_COOP_FRAME_LIMIT||!Number.isSafeInteger(msg.serial)||msg.serial<1)throw Error('Invalid cooperative pause packet');
  if(msg.serial<=serial)return; // reliable-channel duplicates cannot reopen an already resumed pause
  if(msg.serial!==serial+1){if(msg.serial!==serial+2||msg.phase!=='stop'||!active?.resumes.has(from))throw Error('Unexpected future pause');const prev=queued.get(from);if(prev&&prev.frame!==msg.frame)throw Error('Future pause frontier changed');queued.set(from,msg);return;}
  handle(msg,from);progress();
 }
 return Object.freeze({receive,toggle,progress,paused,catchingUp:()=>!!active&&!paused(),mayStep:()=>!active||(active.target!==null&&position()<active.target)});
}
