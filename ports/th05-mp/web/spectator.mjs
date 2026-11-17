import {applyDisplay} from './display.mjs';
// The shared E5SV/E5NP spectator lane contains scalar stage initializers and
// confirmed physical inputs. Native heap snapshots never cross an instance.
export const SPECTATOR_ABI=0x54483501,MARKER=0x35;
const header=(count,session,frame,stage,serial,kind,size=32)=>{const bytes=new Uint8Array(size),v=new DataView(bytes.buffer);bytes.set([69,MARKER,83,86,2,kind,count,0]);v.setBigUint64(8,session,true);v.setUint32(16,SPECTATOR_ABI,true);v.setUint32(20,frame,true);v.setUint32(24,stage,true);v.setUint32(28,serial,true);return bytes;};
export function createSpectatorPublisher(module,transport,count,seed,rows,emit=()=>{}){
 const random=new Uint32Array(2);crypto.getRandomValues(random);const session=(BigInt(random[0])<<32n|BigInt(random[1]))||1n;
 let frame=0,serial=0,stage=0,stopped=false,initialized=false;
 const packet=payload=>transport.sendSpectator(payload);
 function checkpoint(at){const n=module._th05_spectator_size(),ptr=module._th05_spectator_state();if(n<4||n>4096)throw Error('Invalid TH05 stage initializer');const state=module.HEAPU8.slice(ptr,ptr+n);stage=new DataView(state.buffer).getInt32(8,true);const bytes=header(count,session,at,stage,++serial,1,40+n),v=new DataView(bytes.buffer);v.setUint32(32,1,true);v.setUint32(36,n,true);bytes.set(state,40);packet(bytes);}
 let entrySerial=0;
 const safely=fn=>{if(stopped)return;try{fn();}catch(reason){stopped=true;try{transport.stopSpectator();}catch{}emit('netplay-status',{state:'spectator-unavailable',message:reason.message});}};
 return {
  begin(){safely(()=>{transport.beginSpectatorSession({type:'spectator-session',session:session.toString(16).padStart(16,'0'),generation:0,seed});entrySerial=module._th05_spectator_serial();checkpoint(0);initialized=true;});},
  flush(confirmed){safely(()=>{
   if(!initialized)return;const next=module._th05_net_stat(5),changed=entrySerial!==module._th05_spectator_serial();
   while(frame<=confirmed){const bytes=new Uint8Array(24+count*12),v=new DataView(bytes.buffer);bytes.set([69,MARKER,78,80,4,3,count,0]);v.setBigUint64(8,session,true);v.setUint32(16,frame,true);v.setUint32(20,SPECTATOR_ABI,true);
    for(let seat=0;seat<count;seat++){const row=rows[seat].get(frame);if(!row)throw Error('TH05 confirmed spectator input retired too soon');const at=24+seat*12;v.setUint16(at,row[0],true);bytes[at+2]=row[3];v.setFloat32(at+4,row[1],true);v.setFloat32(at+8,row[2],true);}packet(bytes);frame++;
   }
   if(changed){if(frame!==next)throw Error('TH05 checkpoint crossed an unconfirmed boundary');entrySerial=module._th05_spectator_serial();checkpoint(frame);}
  });},
  end(){safely(()=>{if(initialized)packet(header(count,session,frame,stage,++serial,3));stopped=true;});},
 };
}
export function createSpectator(options,{module,emit=()=>{},fail,now=()=>performance.now(),socketFactory=url=>new WebSocket(url)}){
 const count=options.netplayPlayerCount,url=new URL(options.netplayUrl);url.searchParams.delete('player');url.searchParams.delete('signal');url.searchParams.set('spectator',options.netplaySpectatorId);url.searchParams.set('players',String(count));
 let api;let socket,closed=false,session=null,serial=0,frame=0,revision=0,target=null,complete=false,queue=[],index=0,bytes=0,stage=0,resolveStart,rejectStart,started=false;
 let minimum=0,frontier=0,paused=false,following=true,seekTarget=null,rewinding=false,lastReport=0,reportRevision=0;
 const run=()=>session===null?null:"s"+session.toString(16).padStart(16,"0")+"_0";
 function report(force=false){if(!started||closed||!force&&now()-lastReport<200)return;lastReport=now();const state={version:1,run:run(),revision:++reportRevision,min:minimum,position:frame,received:frontier,edge:frontier,target:seekTarget??(following?frontier:frame),paused,seeking:seekTarget!==null||target===null||following&&frame<target-8,following,ended:complete,nodes:[{id:1,kind:"stage",frame:minimum,stage:stage+1,bossSlots:0}]};emit("runtime-info",{spectatorPlayback:state});return state;}
 // The shared Launcher removes its loading cover only after the native
 // playback report and this title-owned receive socket are both ready.
 const status=globalThis.__th05PeerTransport={peers:new Map(),disconnected:false,failed:false,error:'',get relay(){return socket;},isRecovering:()=>target===null||rewinding||seekTarget!==null};
 globalThis.__eaglerNetplayFailed=false;globalThis.__eaglerNetplayError='';globalThis.__eaglerNetplaySpectator=true;globalThis.__eaglerNetplayTransport='spectator';globalThis.__eaglerNetplayPath='spectator';
 const ready=new Promise((a,b)=>{resolveStart=a;rejectStart=b;});ready.catch(()=>{});
 function close(){if(closed)return;closed=true;status.disconnected=true;if(socket){socket.onmessage=socket.onerror=socket.onclose=null;socket.close();}queue=[];emit("runtime-info",{spectatorPlayback:null});module.eaglerAudio?.playback(false);globalThis.__eaglerNetplayLanActive=false;}
 const fatal=error=>{if(closed)return;status.failed=true;status.error=error.message;globalThis.__eaglerNetplayFailed=true;globalThis.__eaglerNetplayError=error.message;close();rejectStart(error);fail(error);};
 const push=packet=>{bytes+=packet.length;if(bytes>16*1024*1024)throw Error('TH05 spectator backlog exceeded');queue.push(packet);};
 function receive(buffer){const p=new Uint8Array(buffer),v=new DataView(p.buffer,p.byteOffset,p.byteLength);
  if(p.length<24||p[0]!==69||p[1]!==MARKER||p[6]!==count||p[7])throw Error('Invalid TH05 spectator packet');
  const extension=p[2]===83&&p[3]===86;
  if(extension){if(p.length<32||p[4]!==2||v.getBigUint64(8,true)!==session||v.getUint32(16,true)!==SPECTATOR_ABI)throw Error('TH05 spectator stage identity differs');
   if(p[5]===4){target=v.getUint32(20,true);frontier=Math.max(frontier,target);return;}
   if(![1,3].includes(p[5])||v.getUint32(28,true)<=serial||v.getUint32(24,true)>6)throw Error('Invalid TH05 spectator event');
   if(p[5]===1&&(p.length<44||v.getUint32(32,true)!==1||v.getUint32(36,true)!==p.length-40||p.length>4136))throw Error('Invalid TH05 spectator initializer');
   if(p[5]===3&&p.length!==32)throw Error('Invalid TH05 spectator end');serial=v.getUint32(28,true);frontier=Math.max(frontier,v.getUint32(20,true));if(p[5]===3)complete=true;push(p);if(!started)api.bootstrap();
  }else{if(p[2]!==78||p[3]!==80||p[4]!==4||p[5]!==3||p.length!==24+count*12||v.getBigUint64(8,true)!==session||v.getUint32(20,true)!==SPECTATOR_ABI)throw Error('Invalid TH05 spectator input');
   for(let seat=0;seat<count;seat++){const at=24+seat*12;if(p[at+2]>3||p[at+3]||![v.getFloat32(at+4,true),v.getFloat32(at+8,true)].every(n=>Number.isFinite(n)&&Math.abs(n)<=8192&&Number.isInteger(n*256)))throw Error('Invalid TH05 spectator motion');}frontier=Math.max(frontier,v.getUint32(16,true)+1);push(p);
  }
 }
 return api={
  start(){module.eaglerAudio?.playback(true);socket=socketFactory(url.href);socket.binaryType='arraybuffer';socket.onmessage=event=>{if(closed)return;try{if(typeof event.data==='string'){const message=JSON.parse(event.data);if(message.type==='spectator-session'){if(!/^[a-f0-9]{16}$/.test(message.session)||message.seed!==options.netplaySeed||session!==null)throw Error('TH05 spectator session differs');session=BigInt('0x'+message.session);}else if(message.type==='error')throw Error(message.error);return;}receive(event.data);}catch(e){fatal(e);}};socket.onerror=()=>fatal(Error('TH05 spectator connection failed'));socket.onclose=()=>{if(!closed&&!complete)fatal(Error('TH05 spectator stream disconnected'));};return ready;},
  advance(){if(closed)return false;report();if(paused&&seekTarget===null){module.eaglerAudio?.playback(true);return false;}const start=now();let advanced=0;const catching=seekTarget!==null||following&&(target===null||frame<frontier-8||queue.length-index>12);module.eaglerAudio?.playback(catching||paused);
   try{for(let n=0;n<(catching?120:1)&&index<queue.length&&now()-start<4;n++){
    const p=queue[index++],v=new DataView(p.buffer,p.byteOffset,p.byteLength);
    if(p[2]===83){if(p[5]===1){const checkpoint=v.getUint32(20,true);if(started&&!rewinding&&checkpoint!==frame)throw Error('TH05 spectator checkpoint gap');const ptr=module._malloc(p.length-40);try{module.HEAPU8.set(p.subarray(40),ptr);if(!module._th05_spectator_load(ptr,p.length-40))throw Error('TH05 spectator state rejected');applyDisplay(module,options.th05MultiplayerDisplay);}finally{module._free(ptr);}rewinding=false;frame=checkpoint;minimum=checkpoint;
      // Retain the current stage initializer and inputs for local seeking.
      if(index>1){queue=queue.slice(index-1);index=1;bytes=queue.reduce((sum,p)=>sum+p.length,0);}
      stage=v.getUint32(24,true);revision++;if(!started){started=true;globalThis.__eaglerNetplayLanActive=true;globalThis.__eaglerNetplayTransport='relay';resolveStart();}advanced++;if(seekTarget!==null&&frame>=seekTarget){seekTarget=null;report(true);break;}}
     else{if(v.getUint32(20,true)!==frame)throw Error('TH05 spectator end gap');complete=true;emit('netplay-status',{state:'spectator-complete'});}continue;
    }
    if(!started||v.getUint32(16,true)!==frame)throw Error('TH05 spectator input gap');for(let seat=0;seat<count;seat++){const at=24+seat*12;module._th05_coop_input(seat,v.getUint16(at,true),v.getFloat32(at+4,true),v.getFloat32(at+8,true),p[at+2]);}
    if(!module._th05_tick(0))throw Error('TH05 spectator simulation failed');frame++;revision++;advanced++;if(seekTarget!==null&&frame>=seekTarget){seekTarget=null;report(true);break;}if(frame%120===0){const hashes=globalThis.__th05SpectatorHashes??=new Map();hashes.set(frame,module._th05_coop_checksum());while(hashes.size>8)hashes.delete(hashes.keys().next().value);}
   }
   if(seekTarget!==null&&frame===seekTarget){seekTarget=null;report(true);}globalThis.__eaglerNetplayLanFrame=globalThis.__eaglerNetplayFrame=frame;globalThis.__eaglerNetplayLanConfirmed=globalThis.__eaglerNetplayConfirmed=frame-1;report();return advanced>0;
   }catch(e){fatal(e);return false;}
  },
  // A first checkpoint must be consumed before launch awaits its readiness.
  bootstrap(){return this.advance();},
  command(message){
   if(!started||closed||message.run!==run())throw Error('Spectator playback run has changed');
   if(!['pause','play','seek','live'].includes(message.action))throw Error('Invalid spectator playback action');
   if(message.action==='pause'){paused=true;following=false;}
   if(message.action==='play'){paused=false;following=false;}
   if(message.action==='seek'){
    if(!Number.isInteger(message.frame)||message.frame<minimum||message.frame>frontier)throw Error('Spectator seek is outside received history');
    following=false;seekTarget=message.frame;index=0;rewinding=true;
   }
   if(message.action==='live'){paused=false;following=true;seekTarget=frontier;}
   report(true);return {};
  },
  async end(){close();emit('netplay-ended');},close,get revision(){return revision;},get recovering(){return rewinding||seekTarget!==null||following&&(target===null||frame<frontier-8);},get delay(){return 0;},get confirmed(){return frame-1;},get frame(){return frame;},get active(){return started&&!closed;},
 };
}
