// Native playback owns every cursor and restore. The shell only transports
// authenticated Launcher commands and publishes native reports.
export function createSpectatorPlaybackBridge({active,memory,state,control,allocate,deallocate,emit,
  replay=()=>globalThis.__eaglerPeerTransport?.spectatorReplay,
  complete,
  schedule=globalThis.setInterval,cancel=globalThis.clearInterval}) {
  let timer=null,last=null,menuSession=null;
  const clear=()=>{if(last){last=null;emit('runtime-info',{spectatorPlayback:null});}};
  const closeMenu=()=>{if(menuSession){menuSession=null;globalThis.__eaglerSpectatorMenu=false;emit('runtime-info',{spectatorMenu:false});}};
  function poll() {
    if(!active()){closeMenu();clear();return null;}
    const file=replay();
    if(menuSession){clear();return null;}
    try {
      const pointer=state();
      if(!pointer){clear();return null;}
      const heap=memory();
      if(!(heap instanceof Uint8Array)||!Number.isInteger(pointer)||pointer<1||pointer>=heap.length)
        throw Error('Invalid native spectator report address');
      const limit=Math.min(heap.length,pointer+131072);let end=pointer;
      while(end<limit&&heap[end])++end;
      if(end===limit)throw Error('Native spectator report exceeds its bound');
      const report=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(heap.subarray(pointer,end)));
      if(report?.version!==1||typeof report.run!=='string'||!/^[A-Za-z0-9_-]{1,64}$/.test(report.run)||
         !Number.isInteger(report.revision)||report.revision<1||report.revision>0xfffffffe||
         !Array.isArray(report.nodes)||report.nodes.length>512)throw Error('Invalid native spectator report');
      if(complete&&!menuSession&&report.ended&&!report.paused&&!report.seeking&&report.position>=report.received&&
         report.run.startsWith('s'+file?.session+'_')&&file?.bytes instanceof Uint8Array){
        const pointer=allocate(file.bytes.length);
        if(!Number.isInteger(pointer)||pointer<1)throw Error('Spectator result allocation failed');
        try{const heap=memory();if(pointer>heap.length-file.bytes.length)throw Error('Invalid spectator result allocation');heap.set(file.bytes,pointer);
          if(complete(pointer,file.bytes.length)){menuSession=file.session;globalThis.__eaglerSpectatorMenu=true;emit('runtime-info',{spectatorMenu:true});}
        }finally{deallocate(pointer);}
        if(menuSession){clear();return null;}
      }
      if(last?.run===report.run&&(report.revision<=last.revision||report.received<last.received))return last;
      last=report;emit('runtime-info',{spectatorPlayback:report});return report;
    } catch(error) {
      clear();return null;
    }
  }
  function command(message) {
    if(!active())throw Error('Spectator playback is not active');
    if(typeof message?.run!=='string'||!/^[A-Za-z0-9_-]{1,64}$/.test(message.run)||
       !['pause','play','seek','live'].includes(message.action)||
       (message.action==='seek'&&(!Number.isInteger(message.frame)||message.frame<0||message.frame>0xfffffffe)))
      throw Error('Invalid spectator playback command');
    const report=poll();
    if(!report||message.run!==report.run)throw Error('Spectator playback run has changed');
    if(message.action==='seek'&&(message.frame<report.min||message.frame>report.edge))
      throw Error('Spectator seek is outside received history');
    // Keep terminators as zero bytes. The classic inline HTML optimizer can
    // turn a JS NUL escape into a literal NUL, which HTML replaces by U+FFFD.
    const run=new TextEncoder().encode(message.run),action=new TextEncoder().encode(message.action);
    const actionOffset=run.length+1,bytes=new Uint8Array(actionOffset+action.length+1);
    bytes.set(run);bytes.set(action,actionOffset);
    const pointer=allocate(bytes.length);
    if(!Number.isInteger(pointer)||pointer<1)throw Error('Spectator command allocation failed');
    try {
      const heap=memory();
      if(pointer>heap.length-bytes.length)throw Error('Invalid spectator command allocation');
      heap.set(bytes,pointer);
      if(!control(pointer,pointer+actionOffset,message.action==='seek'?message.frame:0))
        throw Error('Spectator playback command could not be applied');
    } finally {deallocate(pointer);}
    poll();return {};
  }
  return Object.freeze({poll,command,
    start(){if(timer===null)timer=schedule(poll,250);poll();},
    stop(){if(timer!==null)cancel(timer);timer=null;closeMenu();clear();},
  });
}
