// Reliable logical peer channels survive replacement of their WebSocket.
// E7 target / E8 authenticated source envelopes remain server-owned.
export const RECONNECT_MS=30000;
export function createRelay(session,generation,roles,onFailure){
  const channels=new Map(),encoder=new TextEncoder(),decoder=new TextDecoder('utf-8',{fatal:true});
  // getRandomValues also works on plain HTTP LAN origins.
  const client=Array.from(crypto.getRandomValues(new Uint8Array(16)),n=>n.toString(16).padStart(2,'0')).join('');
  let socket,closed=false,released=false,retryTimer,handshakeTimer,watchdog,deadline=0,attempt=0,lastMessage=0;
  function end(){
    if(closed)return;closed=true;
    clearTimeout(retryTimer);clearTimeout(handshakeTimer);clearInterval(watchdog);
    globalThis.removeEventListener('online',online);
    for(const channel of channels.values())channel.close();
    socket?.close();
  }
  function fail(reason){if(closed)return;end();onFailure(Error(reason));}
  function wire(role,value){
    if(socket?.readyState!==WebSocket.OPEN)return;
    const bytes=encoder.encode(JSON.stringify(value)),packet=new Uint8Array(bytes.length+2);
    if(bytes.length>262144)throw Error('中继消息过大');
    if(socket.bufferedAmount>262144){lost(socket);return;}
    packet[0]=0xe7;packet[1]=roles.indexOf(role);packet.set(bytes,2);
    try{socket.send(packet);}catch{lost(socket);}
  }
  function channelFor(role){
    if(channels.has(role))throw Error('重复中继通道');
    const pending=new Map();let sent=0,received=0,pendingBytes=0;
    const channel={label:'th02-inputs',readyState:'connecting',
      get bufferedAmount(){return socket?.bufferedAmount||0;},
      send(text){
        if(channel.readyState!=='open')throw Error('中继通道未打开');
        const size=encoder.encode(text).length;
        if(size>65536||pendingBytes+size>524288||pending.size>=1024)throw Error('中继待确认消息过多');
        // Retain before send: a TCP close can race an apparently open socket.
        pending.set(++sent,{text,size});pendingBytes+=size;
        wire(role,{seq:sent,ack:received,text});
      },
      suspend(){
        if(channel.readyState!=='open')return;
        channel.readyState='connecting';channel.onreconnecting?.();
      },
      open(notify=true){
        if(channel.readyState!=='connecting')return;
        channel.readyState='open';
        // Replay in original order before newly generated hello/input packets.
        for(const [seq,{text}]of pending)wire(role,{seq,ack:received,text});
        wire(role,{ack:received});if(notify)channel.onopen?.();
      },
      receive(value){
        if(!value||!Number.isSafeInteger(value.ack)||value.ack<0||value.ack>sent)throw Error('无效中继消息确认');
        for(const [seq,item]of pending){if(seq>value.ack)break;pending.delete(seq);pendingBytes-=item.size;}
        if(value.seq===undefined)return;
        if(!Number.isSafeInteger(value.seq)||value.seq<1||typeof value.text!=='string'||encoder.encode(value.text).length>65536)throw Error('无效中继消息序号');
        if(value.seq>received+1)throw Error('中继恢复消息不连续');
        if(value.seq===received+1){received=value.seq;channel.onmessage?.({data:value.text});}
        // Duplicates include inputs, Bombs and run notices: acknowledge only.
        wire(role,{ack:received});
      },
      close(){if(channel.readyState==='closed')return;channel.readyState='closed';pending.clear();pendingBytes=0;channel.onclose?.();}
    };
    channels.set(role,channel);return channel;
  }
  function online(){
    if(closed||socket?.readyState===WebSocket.OPEN||socket?.readyState===WebSocket.CONNECTING)return;
    clearTimeout(retryTimer);connect();
  }
  function lost(current){
    if(closed||socket!==current)return;
    // Do not await the TCP close handshake on a dead network path.
    socket=null;clearTimeout(handshakeTimer);
    for(const channel of channels.values())channel.suspend();
    deadline||=performance.now()+RECONNECT_MS;
    try{current.close();}catch{}
    if(performance.now()>=deadline){fail('服务器转发连接超过 30 秒仍未恢复');return;}
    retryTimer=setTimeout(connect,Math.min(1000,250*2**Math.min(attempt++,2)));
  }
  function connect(){
    if(closed)return;
    deadline||=performance.now()+RECONNECT_MS;
    if(performance.now()>=deadline){fail('服务器转发在 30 秒内未恢复，请检查网络及服务／隧道窗口');return;}
    const url=new URL('/relay',location.href);url.protocol=location.protocol==='https:'?'wss:':'ws:';
    url.search=new URLSearchParams({room:session.room,token:session.token,runId:session.runId,client});
    socket=new WebSocket(url);socket.binaryType='arraybuffer';
    const current=socket;
    handshakeTimer=setTimeout(()=>{if(!closed&&socket===current&&current.readyState===WebSocket.CONNECTING)lost(current);},8000);
    socket.onopen=()=>{if(socket!==current)return;clearTimeout(handshakeTimer);lastMessage=performance.now();};
    socket.onmessage=event=>{
      if(closed||socket!==current)return;
      lastMessage=performance.now();
      try{
        if(typeof event.data==='string'){
          const message=JSON.parse(event.data);
          if(message.type==='route'){
            if(message.mode!=='relay'||message.generation!==generation)throw Error('中继开局路由不匹配');
            if(message.recovery!==1||!Array.isArray(message.peers))throw Error('请重启新版房间服务，并刷新所有玩家页面以启用断线恢复');
            released=true;deadline=0;attempt=0;
            // Flush every link before callbacks can broadcast new packets.
            const opening=[...channels].filter(([role,c])=>message.peers.includes(roles.indexOf(role))&&c.readyState==='connecting');
            for(const [,c]of opening)c.open(false);
            for(const [,c]of opening)c.onopen?.();
          }else if(message.type==='peer-left'||message.type==='peer-back'){
            const channel=channels.get(roles[message.peer]);
            if(!channel)throw Error('中继玩家身份无效');
            if(!released)return;
            if(message.type==='peer-left')channel.suspend();else channel.open();
          }else if(message.type!=='pong')throw Error(message.message||'中继服务返回未知消息');
        }else{
          const bytes=new Uint8Array(event.data),channel=channels.get(roles[bytes[1]]);
          if(!released||bytes.length<3||bytes[0]!==0xe8||!channel)throw Error('中继来源或消息封装无效');
          if(channel.readyState==='open')channel.receive(JSON.parse(decoder.decode(bytes.subarray(2))));
        }
      }catch(error){fail(error.message||String(error));}
    };
    socket.onerror=()=>lost(current);
    socket.onclose=()=>lost(current);
  }
  globalThis.addEventListener('online',online);
  watchdog=setInterval(()=>{
    if(closed||socket?.readyState!==WebSocket.OPEN)return;
    // A half-open TCP socket must not wait for the operating system timeout.
    if(performance.now()-lastMessage>5000){lost(socket);return;}
    try{socket.send(JSON.stringify({type:'ping'}));}catch{lost(socket);}
  },1000);
  return {channelFor,connect,close:end};
}
