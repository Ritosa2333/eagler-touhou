// A two-player transport on the monorepo Relay signaling protocol. Reliable
// ordered SCTP keeps the existing TH19 frame stream intact on RTC and TURN.
export class PeerSocket {
 constructor(address,options={}){
  this.readyState=0;this.pending=[];this.candidates=[];this.closed=false;this.options=options;
  this.iceServers=Array.isArray(options.netplayIceServers)?options.netplayIceServers:[];
  this.url=new URL(address);this.side=Number(this.url.searchParams.get('player'));this.peer=1-this.side;
  const signalUrl=new URL(this.url);signalUrl.searchParams.set('signal','1');signalUrl.searchParams.set('players','2');
  this.signal=new WebSocket(signalUrl);this.serial=Promise.resolve();
  this.signal.onmessage=event=>{this.serial=this.serial.then(()=>this.control(JSON.parse(event.data))).catch(error=>this.fail(error));};
  this.signal.onerror=()=>{if(this.readyState!==1)this.fail(Error('无法连接房间信令'));};
  this.signal.onclose=()=>{if(!this.closed&&this.readyState!==1)this.fail(Error('房间信令连接已关闭'));};
  this.timeout=setTimeout(()=>this.fail(Error('WebRTC 连接超时')),45000);
  // Both relay streams must exist before the server can release its fallback
  // barrier. Opening one only after receiving that barrier would deadlock.
  if(options.netplayAllowWebsocketFallback===true)this.openRelay();
 }
 get bufferedAmount(){return (this.route==='relay'?this.relay:this.channel)?.bufferedAmount||0;}
 signalMessage(value){if(this.signal.readyState===WebSocket.OPEN)this.signal.send(JSON.stringify(value));}
 async createPeer(){
  if(this.pc)return;
  this.pc=new RTCPeerConnection({iceServers:this.iceServers,iceTransportPolicy:this.options.netplayPreferTurn?'relay':'all'});
  this.pc.onicecandidate=event=>{if(event.candidate)this.signalMessage({type:'signal',to:this.peer,candidate:event.candidate.toJSON()});};
  this.pc.onconnectionstatechange=()=>{if(this.pc.connectionState==='failed'){if(this.readyState===1)this.fail(Error('WebRTC 连接中断'));else this.signalMessage({type:'rtc-failed'});}};
  this.pc.ondatachannel=event=>this.attach(event.channel);
  if(this.side===0){
   this.attach(this.pc.createDataChannel('th19-input',{ordered:true}));
   await this.pc.setLocalDescription(await this.pc.createOffer());
   this.signalMessage({type:'signal',to:this.peer,description:this.pc.localDescription});
  }
 }
 attach(channel){
  if(this.channel&&this.channel!==channel){channel.close();return;}
  this.channel=channel;channel.binaryType='arraybuffer';
  channel.onopen=()=>{this.signalMessage({type:'rtc-ready'});this.open();};
  channel.onmessage=event=>{if(this.readyState===1)this.onmessage?.(event);else{if(this.pending.length>=256)return this.fail(Error('RTC 启动消息过多'));this.pending.push(event);}};
  channel.onclose=()=>{if(!this.closed&&this.route==='rtc')this.fail(Error('对手的数据通道已关闭'));};
  channel.onerror=()=>{if(!this.closed&&this.route==='rtc')this.fail(Error('对局数据通道错误'));};
 }
 async control(message){
  if(this.closed)return;
  if(message.type==='peers'){
   // Creating an offer waits for the other signaling endpoint to exist.
   if(Array.isArray(message.iceServers))this.iceServers=message.iceServers;
   if(message.peers?.includes(this.peer))await this.createPeer();
   if(message.route)this.select(message.route);
  }else if(message.type==='peer-join'){
   if(message.player===this.peer)await this.createPeer();
  }else if(message.type==='signal'&&message.from===this.peer){
   await this.createPeer();
   if(message.description){
    if(message.description.type!==(this.side===0?'answer':'offer'))throw Error('对局 SDP 身份错误');
    await this.pc.setRemoteDescription(message.description);
    for(const candidate of this.candidates.splice(0))await this.pc.addIceCandidate(candidate);
    if(this.side===1){await this.pc.setLocalDescription(await this.pc.createAnswer());this.signalMessage({type:'signal',to:this.peer,description:this.pc.localDescription});}
   }
   if(message.candidate){if(this.pc.remoteDescription)await this.pc.addIceCandidate(message.candidate);else{if(this.candidates.length>=128)throw Error('ICE 候选过多');this.candidates.push(message.candidate);}}
  }else if(message.type==='route')this.select(message.mode||message.route);
  else if(message.type==='error')throw Error(message.error||'房间连接失败');
  else if(message.type==='peer-disconnected')throw Error('对手已退出对局');
 }
 select(route){
  if(!['rtc','relay'].includes(route))return;
  if(this.route&&this.route!==route)throw Error('对局线路发生冲突');
  if(route==='relay'&&this.options.netplayAllowWebsocketFallback!==true)throw Error('此房间未启用 WebSocket 回退');
  this.route=route;
  if(route==='relay'){
   this.openRelay();
   this.channel?.close();this.pc?.close();
  }else{
   this.relay?.close();
  }
  this.open();
 }
 openRelay(){
  if(this.relay)return;
  this.relay=new WebSocket(this.url);this.relay.binaryType='arraybuffer';
  this.relay.onmessage=event=>{
   if(this.closed)return;
   try{
    if(typeof event.data==='string'){
     const message=JSON.parse(event.data);
     if(message.type==='route')this.select(message.mode);
     else if(message.type==='error')throw Error(message.error||'中继连接失败');
    }else if(this.route==='relay'&&this.readyState===1)this.onmessage?.(event);
    else if(!this.route){
     if(this.pending.length>=256)throw Error('中继启动消息过多');
     this.pending.push(event);
    }
   }catch(error){this.fail(error);}
  };
  this.relay.onopen=()=>this.open();
  this.relay.onclose=()=>{if(!this.closed&&this.route==='relay')this.fail(Error('中继连接已关闭'));};
  this.relay.onerror=()=>{if(this.route==='relay')this.fail(Error('中继连接失败'));};
 }
 open(){
  if(this.closed||this.readyState===1||!this.route)return;
  if(this.route==='rtc'&&this.channel?.readyState!=='open'||this.route==='relay'&&this.relay?.readyState!==WebSocket.OPEN)return;
  clearTimeout(this.timeout);this.readyState=1;this.onopen?.();
  this.onmessage?.({data:JSON.stringify({type:'route',mode:this.route})});
  for(const event of this.pending.splice(0))this.onmessage?.(event);
 }
 send(bytes){if(this.readyState!==1)throw Error('对局通道未就绪');if(this.bufferedAmount>256*1024)throw Error('对局通道拥塞');(this.route==='rtc'?this.channel:this.relay).send(bytes);}
 fail(error){if(this.closed)return;this.error=error;this.onerror?.({error});this.close();}
 close(){if(this.closed)return;this.closed=true;this.readyState=3;clearTimeout(this.timeout);this.channel?.close();this.pc?.close();this.relay?.close();this.signal?.close();this.pending.length=0;this.onclose?.({reason:this.error?.message||''});}
}
