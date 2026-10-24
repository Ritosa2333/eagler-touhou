// Acceptance-only sender impairment. Keep this function self-contained: the
// browser runner serializes it into the same init script as installProbe.
export function createSenderDelayQueue({state,now=()=>performance.now(),
  setTimer=(callback,delay)=>setTimeout(callback,delay),clearTimer=timer=>clearTimeout(timer),
  maxQueued=1024,maxQueuedBytes=4*1024*1024}){
  const channels=new Map();let closed=false;
  const isOpen=socket=>socket.readyState===1||socket.readyState==='open';
  function detach(channel){
    if(channel.timer!==null){clearTimer(channel.timer);channel.timer=null;}
    channel.socket.removeEventListener?.('close',channel.onClose);
    channels.delete(channel.socket);
  }
  function release(item){
    if(item.done)return false;item.done=true;
    if(item.timer!==null){clearTimer(item.timer);item.timer=null;}
    item.channel.pending.delete(item);--state.queued;state.queuedBytes-=item.payload.length;
    if(!item.channel.pending.size)detach(item.channel);
    return true;
  }
  function cancel(channel,reason){
    channel.cancelled=true;
    for(const item of [...channel.pending]){
      item.entry.cancelled=reason;item.entry.cancelledAt=now();release(item);
    }
  }
  function deliver(item){
    if(item.done)return;
    if(!isOpen(item.channel.socket)){cancel(item.channel,'channel closed before due');return;}
    release(item);item.entry.actual=now();item.entry.lateness=item.entry.actual-item.entry.due;
    try{item.send.call(item.channel.socket,item.payload);}
    catch(error){item.entry.error=String(error);state.errors.push(String(error));}
  }
  function timerFailure(channel,error){
    const first=channel.pending.values().next().value;
    if(first)first.entry.error=String(error);
    state.errors.push(String(error));cancel(channel,'transport timer failed');
  }
  function drain(channel){
    if(channel.draining||channel.cancelled)return;channel.draining=true;
    try{
      while(channel.pending.size){
        const item=channel.pending.values().next().value,remaining=item.entry.due-now();
        if(remaining>0){
          // Only the head has a timer. Fractional delays can fire early; rearm
          // for the remaining time rather than deliver early or drop the head.
          channel.timer=setTimer(()=>{channel.timer=null;drain(channel);},Math.max(1,Math.ceil(remaining)));
          return;
        }
        deliver(item);
      }
    }catch(error){timerFailure(channel,error);throw error;}
    finally{channel.draining=false;}
  }
  function scheduleUnordered(item){
    if(item.done)return;
    try{
      const remaining=item.entry.due-now();
      if(remaining>0)item.timer=setTimer(()=>{item.timer=null;scheduleUnordered(item);},Math.max(1,Math.ceil(remaining)));
      else deliver(item);
    }catch(error){timerFailure(item.channel,error);throw error;}
  }
  return {
    enqueue({socket,payload,entry,send,ordered}){
      if(closed){entry.cancelled='transport injection closed';entry.cancelledAt=now();return;}
      // Reject before changing either counter; an overflow must not leave a
      // ghost entry or make queuedBytes disagree with queued packet count.
      if(state.queued+1>maxQueued||state.queuedBytes+payload.length>maxQueuedBytes){
        entry.error='acceptance transport queue overflow';
        state.errors.push('bounded transport injection queue overflow');throw Error(entry.error);
      }
      let channel=channels.get(socket);
      if(!channel){
        channel={socket,ordered:!!ordered,pending:new Set(),timer:null,due:0,draining:false,cancelled:false};
        channel.onClose=()=>cancel(channel,'channel closed before due');
        channels.set(socket,channel);socket.addEventListener?.('close',channel.onClose);
      }
      if(channel.ordered!==!!ordered)throw Error('transport ordering changed while packets were queued');
      // FIFO comes from the queue, never from tiny differences between due
      // timestamps. Independent sockets and unordered RTC lanes do not wait.
      if(ordered){entry.due=Math.max(entry.due,channel.due);channel.due=entry.due;}
      const item={channel,payload,entry,send,timer:null,done:false};channel.pending.add(item);
      ++state.queued;state.queuedBytes+=payload.length;
      state.maxQueued=Math.max(state.maxQueued,state.queued);
      state.maxQueuedBytes=Math.max(state.maxQueuedBytes,state.queuedBytes);
      if(ordered){if(channel.timer===null)drain(channel);}else scheduleUnordered(item);
    },
    close(){closed=true;for(const channel of [...channels.values()])cancel(channel,'transport injection closed');},
  };
}
