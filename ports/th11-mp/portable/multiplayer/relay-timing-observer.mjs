// Test-only timing shim in the independent, unchanged relay process. The
// service's per-packet jitter can invert two control packets on one TCP stream.
// Keep a conservative global FIFO across delayed binary sends: an earlier
// timer must fire/cancel before a later packet is delivered. Never alter bytes.
export function installRelayTimingObserver(target=globalThis,channel=process){
  const originalSet=target.setTimeout,originalClear=target.clearTimeout;
  const rows=[],pending=new Map();let phase='startup',overflow=0,sequence=0;
  const delivery=[];let head=0,draining=false;
  const now=()=>performance.now();
  const drain=()=>{
    if(draining)return;draining=true;
    try{while(head<delivery.length){const next=delivery[head];
      if(next.row.cancelled!==null){++head;continue;}
      if(!next.ready)break;
      ++head;pending.delete(next.timer);next.row.fired=now();
      try{next.callback(...next.args);}finally{next.row.returned=now();}
    }}finally{draining=false;if(head>1024){delivery.splice(0,head);head=0;}}
  };
  target.setTimeout=function(callback,delay,...args){
    // Bound to the pinned createBoundedRelaySender callback, not arbitrary
    // relay heartbeat / route / lifecycle timers with similar delays.
    const source=typeof callback==='function'?Function.prototype.toString.call(callback):'';
    if(!(delay>0&&source.includes('state.timers.delete(timer)')&&source.includes('send(socket, retained')))
      return originalSet(callback,delay,...args);
    if(rows.length>=20000){++overflow;return originalSet(callback,delay,...args);}
    const row={id:++sequence,phase,scheduled:now(),delay:Number(delay),timerFired:null,fired:null,returned:null,cancelled:null};
    rows.push(row);const entry={row,callback,args,ready:false,timer:null};delivery.push(entry);
    const timer=originalSet(function(...values){entry.ready=true;entry.args=values;row.timerFired=now();drain();},delay,...args);
    entry.timer=timer;pending.set(timer,entry);return timer;
  };
  target.clearTimeout=function(timer){
    const entry=pending.get(timer);if(entry){entry.row.cancelled=now();pending.delete(timer);}
    const result=originalClear(timer);drain();return result;
  };
  const message=m=>{
    if(m?.kind==='relay-delay-phase'&&['measure','drain','settled'].includes(m.phase))phase=m.phase;
    if(m?.kind==='relay-delay-snapshot')channel.send?.({kind:'relay-delay-observation',clock:'relay performance.now',
      phase,pending:pending.size,overflow,rows,note:'Original timer plus conservative global FIFO delivery; not client RTT or kernel delivery. Cross-socket FIFO is a fixture restriction.'});
  };
  channel.on?.('message',message);
  return ()=>{target.setTimeout=originalSet;target.clearTimeout=originalClear;channel.off?.('message',message);};
}
