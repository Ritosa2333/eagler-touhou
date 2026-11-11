import {packTouch, unpackTouch, ALWAYS_POINT, MAX_INPUT} from './touch-input.js';
import {neutral} from './frame-queue.js';
import {createInputChannel,acceptInputChannel} from './netplay/input-transport.js';
import {loadRtcConfiguration,hasTurn,describeRtcPath} from './netplay/udp-config.js';
import {SYNC_PROTOCOL,RTT_SAMPLES,validInputReport,validInputPolicy,chooseInputPolicy} from './netplay/input-policy.js';
const ACTIONS=['up','down','left','right','shot','rapid','attack','focus'], protocol='th03-lan/1';
export class Netplay {
  constructor({room,token,role,mode,runId=0,sampleLocal,onMetrics=()=>{},onState=()=>{},onError=()=>{}}){this.room=room;this.runId=runId;this.token=token;this.role=role;this.mode=mode;this.sampleLocal=sampleLocal;this.localSeat=role==='host'?0:1;this.onMetrics=onMetrics;this.onState=onState;this.onError=onError;this.remote=new Set();this.seq=0;this.lastRemote=-1;this.socket=null;this.pc=null;this.rtcPoll=null;this.rtcTimer=null;this.rtcResolve=null;this.rtcReject=null;this.pendingIce=[];this.remoteDescriptionReady=false;}
  async start(){
    this.metricsTimer=setInterval(()=>this.observeConnection(),500);
    try{
      const state=await this.state();
      if(this.closed)throw Error('连接已关闭');
      if(!state.started||state.mode!==this.mode)throw Error('房间尚未开始或模式不一致');
      // Server room policy wins over invite/query values on both devices.
      this.localSeat=(state.settings.hostSeat===1?1:0)^(this.role==='guest'?1:0);
      this.network=state.network||'lan';this.ice=state.ice||'all';
      this.heartbeat=setInterval(()=>this.state().catch(error=>this.fail(error)),5000);
      // This instance is created after native boot. A faster device must not
      // spend its transport timeout waiting for its peer to finish booting.
      this.onState('本机资源已就绪，等待另一端完成游戏启动');
      await this.waitForPlayers();
      if(this.closed)throw Error('连接已关闭');
      return await (this.mode==='relay'?this.startRelay():this.startRtc());
    }catch(error){this.close();throw error;}
  }
  async state(){const query=new URLSearchParams({protocol,room:this.room,token:this.token,runId:this.runId});const response=await fetch(`/api/state?${query}`,{cache:'no-store'});const state=await response.json();if(!response.ok)throw Error(state.error||'房间已结束');if(state.runId!==this.runId||!state.started)throw Error('本局已结束');if(!state.present.host||!state.present.guest)throw Error('另一位玩家已离开');return state;}
  async waitForPlayers(){const response=await fetch('/api/progress',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({protocol,room:this.room,token:this.token,runId:this.runId,message:'本机资源已就绪',loaded:true})});if(!response.ok)throw Error('房间准备确认失败');const deadline=performance.now()+600000;while(!this.closed){const state=await this.state();if(state.startup.host?.error||state.startup.guest?.error)throw Error('另一端加载失败，请结束后重新建房');if(state.startup.host?.loaded&&state.startup.guest?.loaded)return;if(performance.now()>deadline)throw Error('等待另一端加载超时');await new Promise(resolve=>setTimeout(resolve,200));}throw Error('连接已关闭');}
  fail(error){if(this.closed)return;this.rtcReject?.(error);this.relayReject?.(error);this.rtcResolve=this.rtcReject=this.relayReject=null;this.close();this.onState(error.message);this.onError(error);}
  get isLockstep(){return true;}
  get inputTransportStats(){return this.channel?.transportStats??null;}
  connectionMetrics(){
    const now=performance.now(),recovery=this.disconnectedAt!=null;
    const frames=this.inputPolicy?(this.inputPolicy.rollbackEnabled?this.inputPolicy.inputDelay:2):null;
    return {phase:this.closed?'stopped':this.synchronized?'connected':'connecting',bufferFrames:frames,bufferMs:frames===null?null:frames*1000/60,
      peers:[{name:`P${(this.localSeat^1)+1}`,path:this.mode==='relay'?'WebSocket 中继':this.path||'WebRTC',
        state:recovery?'recovering':this.channel?.readyState==='open'?'connected':'connecting',
        rttMs:this.liveRtt,rttAgeMs:this.rttAt==null?null:now-this.rttAt,packetAgeMs:this.lastPacketAt==null?null:now-this.lastPacketAt,
        remainingMs:recovery?Math.max(0,10000-(now-this.disconnectedAt)):null,recovery:'等待网络自行恢复'}]};
  }
  observeConnection(){
    this.onMetrics?.(this.connectionMetrics());
    if(this.closed||!this.synchronized||this.channel?.readyState!=='open'||this.metricProbe||performance.now()-(this.lastMetricProbe??-Infinity)<2000)return;
    this.metricProbe=true;this.lastMetricProbe=performance.now();
    // Reuse the existing ping/pong; this sample never renegotiates game input.
    this.probeRtt().then(()=>{},()=>{}).finally(()=>{this.metricProbe=false;});
  }
  clearLocal(){this.localActions=[];this.localTaps=new Set();this.localTouch=null;}
  setLocal(actions,touch=0,options={}){
    // Retain short presses observed between accepted network frames. A fresh
    // sample must not erase a keyboard/gamepad edge or consumed touch pulse.
    this.localTaps??=new Set();
    for(const action of options.cancelled||[])this.localTaps.delete(action);
    for(const action of actions)if(!this.localActions?.includes(action))this.localTaps.add(action);
    this.localActions=[...actions];this.localOptions=options;
    const next=unpackTouch(touch),old=this.localTouch;
    this.localTouch={...next,alwaysPoint:Math.floor(touch/ALWAYS_POINT)%2===1,active:next.active,begin:next.begin||!!(next.active&&old?.begin),
      unlimited:next.unlimited,
      x:next.active?Math.max(-8192,Math.min(8191,(old?.active&&!next.begin?old.x:0)+next.x)):0,
      y:next.active?Math.max(-8192,Math.min(8191,(old?.active&&!next.begin?old.y:0)+next.y)):0};
  }
  command(value){this.commands??=[];if(this.commands.length<16)this.commands.push(value);}
  capture(){
    // Sample at the accepted input boundary, independent of display refresh.
    // Read the live keyboard, gamepad and touch state before freezing a frame.
    this.sampleLocal?.();
    const actions=new Set([...(this.localActions||[]),...(this.localTaps||[])]);
    for(const [a,b] of [['up','down'],['left','right']])if(actions.has(a)&&actions.has(b)){
      // A current hold wins over an older tap in the opposite direction.
      const heldA=this.localActions?.includes(a),heldB=this.localActions?.includes(b);
      if(heldA!==heldB)actions.delete(heldA?b:a);
      else{actions.delete(a);actions.delete(b);}
    }
    const touch=this.localTouch,input={...neutral(),actions:[...actions],touch:touch?packTouch(0,touch):0,
    focusEnabled:this.localOptions?.focusEnabled??true,points:this.localOptions?.points??true,commands:this.commands?.splice(0)||[]};
    this.localTaps?.clear();if(touch){touch.x=touch.y=0;touch.begin=false;}return input;}
  sendPacket(packet){if(this.channel?.readyState!=='open')throw Error('同步通道未连接');
    if((this.channel.bufferedAmount||this.socket?.bufferedAmount||0)>524288)throw Error('联机发送队列拥塞，请结束后重新建房');
    this.channel.send(JSON.stringify({...packet,sync:SYNC_PROTOCOL}));}
  async synchronize(identity){
    this.sendPacket({type:'sync-ready',identity});
    const deadline=performance.now()+30000;
    while(!this.closed){
      if(this.peerIdentity){if(JSON.stringify(this.peerIdentity)!==JSON.stringify(identity))throw Error('双方游戏、补丁或同步引擎版本不同，请刷新后重新建房');
        this.synchronized=true;return;}
      if(performance.now()>deadline)throw Error('另一端未进入帧同步，请双方刷新后重新建房');
      await new Promise(resolve=>setTimeout(resolve,20));
    }
    throw Error('同步连接已关闭');
  }
  async waitForSync(predicate,message){
    const deadline=performance.now()+30000;
    while(!this.closed){if(predicate())return;if(performance.now()>deadline)throw Error(message);await new Promise(resolve=>setTimeout(resolve,10));}
    throw Error('同步连接已关闭');
  }
  probeRtt(){
    this.probes??=new Map();const id=this.probeSerial=(this.probeSerial??0)+1;
    return new Promise((resolve,reject)=>{
      const pending={at:performance.now(),resolve,reject,timer:setTimeout(()=>{this.probes.delete(id);reject(Error('连接延迟采样超时，请重新建房'));},5000)};
      this.probes.set(id,pending);
      try{this.sendPacket({type:'buffer-ping',id});}catch(error){clearTimeout(pending.timer);this.probes.delete(id);reject(error);}
    });
  }
  async negotiateInputPolicy({rollbackEnabled}){
    if(typeof rollbackEnabled!=='boolean'||!this.synchronized||this.negotiating)throw Error('输入缓冲协商状态无效');
    this.negotiating=true;
    this.onState(rollbackEnabled?'正在测量连接延迟并确认输入缓冲':'正在确认双方关闭回滚');
    const samples=[];if(rollbackEnabled)for(let n=0;n<RTT_SAMPLES;n++)samples.push(await this.probeRtt());
    const sorted=[...samples].sort((a,b)=>a-b);
    const report={rollbackEnabled,rttMs:rollbackEnabled?Math.ceil(sorted[sorted.length>>1]):0,maxRttMs:rollbackEnabled?Math.ceil(Math.max(...samples)):0};
    this.localInputReport=Object.freeze(report);this.rttSamples=Object.freeze(samples);
    this.sendPacket({type:'buffer-report',report});
    await this.waitForSync(()=>this.peerInputReport,'等待对方连接延迟采样超时');
    const expected=chooseInputPolicy(report,this.peerInputReport);
    if(this.role==='host'){
      this.sendPacket({type:'buffer-policy',policy:expected});
      await this.waitForSync(()=>this.peerPolicyAck,'等待对方确认输入缓冲超时');
      if(JSON.stringify(this.peerPolicyAck)!==JSON.stringify(expected))throw Error('对方输入缓冲确认不一致');
    }else{
      await this.waitForSync(()=>this.hostInputPolicy,'等待房主输入缓冲超时');
      if(JSON.stringify(this.hostInputPolicy)!==JSON.stringify(expected))throw Error('房主输入缓冲与双方测量不一致');
      this.sendPacket({type:'buffer-ack',policy:expected});
    }
    this.inputPolicy=Object.freeze(expected);
    this.onState(rollbackEnabled?`双方输入缓冲 ${expected.inputDelay} 帧（约 ${Math.round(expected.inputDelay*1000/60)} ms）· 预测预算 ${expected.predictionSteps} 帧 · 领先上限 ${expected.predictionLead} 帧 · 开局 RTT ${expected.rttMs} ms`:'回滚关闭 · 使用 2 帧确认锁步');
    return this.inputPolicy;
  }
  setPacketHandler(handler){
    if(typeof handler!=='function'||this.onPacket)throw Error('帧同步接收器重复安装');
    this.onPacket=handler;for(const packet of this.pendingPackets??[])handler(packet);this.pendingPackets=[];
  }
  rememberSync(name,value){
    if(this[name]&&JSON.stringify(this[name])!==JSON.stringify(value))throw Error('收到冲突的开局协商消息');
    this[name]=structuredClone(value);
  }
  send(value,touch=0){const message=JSON.stringify({type:'input',seq:this.seq++,actions:[...value],touch});if(this.channel?.readyState==='open')this.channel.send(message);}
  receive(message){
    this.lastPacketAt=performance.now();
    if(message.sync&&message.sync!==SYNC_PROTOCOL)throw Error('双方同步协议版本不同，请更新服务、刷新页面后重新建房');
    if(message.sync===SYNC_PROTOCOL){
      if(message.type==='sync-ready'){
        if(!message.identity||typeof message.identity!=='object'||Array.isArray(message.identity))throw Error('同步资源身份无效');
        this.rememberSync('peerIdentity',message.identity);return;
      }
      if(message.type==='buffer-ping'||message.type==='buffer-pong'){
        if(!Number.isSafeInteger(message.id)||message.id<1)throw Error('延迟采样消息无效');
        if(message.type==='buffer-ping')this.sendPacket({type:'buffer-pong',id:message.id});
        else{const pending=this.probes?.get(message.id);if(pending){this.probes.delete(message.id);clearTimeout(pending.timer);this.rttAt=performance.now();this.liveRtt=Math.max(0,this.rttAt-pending.at);pending.resolve(this.liveRtt);}}
        return;
      }
      if(message.type==='buffer-report'){
        if(!validInputReport(message.report))throw Error('连接延迟报告无效');
        this.rememberSync('peerInputReport',message.report);return;
      }
      if(message.type==='buffer-policy'||message.type==='buffer-ack'){
        if(!validInputPolicy(message.policy)||(message.type==='buffer-policy'?this.role!=='guest':this.role!=='host'))throw Error('输入缓冲协商消息无效');
        this.rememberSync(message.type==='buffer-policy'?'hostInputPolicy':'peerPolicyAck',message.policy);return;
      }
      if(message.type==='frame'||message.type==='check'||message.type==='rollback'){
        if(this.onPacket)this.onPacket(message);
        else{this.pendingPackets??=[];if(this.pendingPackets.length>=256)throw Error('开局同步消息过多');this.pendingPackets.push(structuredClone(message));}
        return;
      }
      throw Error('未知同步消息');
    }
    if(message.type!=='input'||!Number.isSafeInteger(message.seq)||message.seq<=this.lastRemote||!Array.isArray(message.actions))return;
    const packed=message.touch??0;
    if(!Number.isSafeInteger(packed)||packed<0||packed>MAX_INPUT)return;
    this.lastRemote=message.seq;this.remote=new Set(message.actions.filter(value=>ACTIONS.includes(value)));
    const next=unpackTouch(packed),old=this.remoteTouch;
    this.remoteTouch={...next,alwaysPoint:Math.floor(packed/ALWAYS_POINT)%2===1,begin:next.begin||!!(next.active&&old?.begin),
      x:next.active?Math.max(-8192,Math.min(8191,(old?.active&&!next.begin?old.x:0)+next.x)):0,
      y:next.active?Math.max(-8192,Math.min(8191,(old?.active&&!next.begin?old.y:0)+next.y)):0};
  }
  takeTouch(){const touch=this.closed?null:this.remoteTouch;if(!touch)return 0;const value=packTouch(0,touch);touch.x=touch.y=0;touch.begin=false;return value;}
  startRelay(){
    const url=new URL('/relay',location.href);url.protocol=location.protocol==='https:'?'wss:':'ws:';
    url.search=new URLSearchParams({room:this.room,token:this.token,runId:this.runId,role:this.role,protocol});
    this.onState('正在连接 TCP 服务器转发');this.socket=new WebSocket(url);
    const owner=this;
    this.channel={get readyState(){return owner.socket?.readyState===WebSocket.OPEN?'open':'connecting';},
      get bufferedAmount(){return owner.socket?.bufferedAmount||0;},send(value){owner.socket.send(value);}};
    return new Promise((resolve,reject)=>{
      this.relayReject=reject;
      this.relayTimer=setTimeout(()=>this.fail(Error('TCP 转发连接超时，请检查服务器或隧道')),30000);
      this.socket.onmessage=event=>{
        if(this.closed)return;
        try{
          const value=JSON.parse(event.data);
          if(value.type==='ready'){
            this.relayReject=null;clearTimeout(this.relayTimer);
            this.onState(`TCP 服务器转发 · ${url.protocol==='wss:'?'WSS':'WebSocket'} 已认证`);resolve();
          }else if(['error','peer-left'].includes(value.type))throw Error(value.message||'另一位玩家已断开');
          else this.receive(value);
        }catch(error){this.fail(error);}
      };
      this.socket.onerror=()=>this.fail(Error('TCP 转发连接失败，请检查公网入口、WebSocket 代理及 install-relay.bat 依赖'));
      this.socket.onclose=()=>this.fail(Error('TCP 转发连接已断开，请重新建房'));
    });
  }
  async signal(target,message){if(this.closed)return;const response=await fetch('/api/signal',{method:'POST',headers:{'Content-Type':'application/json'},signal:this.rtcAbort?.signal,body:JSON.stringify({protocol,room:this.room,token:this.token,runId:this.runId,to:target,message})});if(!response.ok)throw Error(`信令发送失败 (${response.status})`);}
  async startRtc(){
    if(typeof RTCPeerConnection!=='function')throw Error('浏览器不支持 WebRTC，请用现代浏览器或 TCP 服务器转发');
    this.rtcAbort=new AbortController();
    this.onState(this.network==='public'?'正在获取 STUN / TURN 连接配置':'正在连接局域网设备');
    const configTimer=setTimeout(()=>this.rtcAbort.abort(),10000);
    let config;
    try{config=await loadRtcConfiguration(this,this.rtcAbort.signal);}
    catch(error){if(error.name==='AbortError')throw Error('获取连接配置超时或已取消，请检查 TURN 服务');throw error;}
    finally{clearTimeout(configTimer);}
    if(this.closed)throw Error('连接已关闭');
    this.rtcHasTurn=hasTurn(config);
    this.onState(this.network!=='public'?'正在连接局域网设备':this.ice==='relay'?'正在连接 TURN 中继':
      this.rtcHasTurn?'正在尝试 UDP 打洞 / TURN 中继':'正在尝试 UDP 打洞（尚未配置 TURN，失败可改用 TCP）');
    this.pc=new RTCPeerConnection(config);
    this.localIceCount=0;this.remoteIceCount=0;
    this.pc.onicecandidate=e=>{
      if(e.candidate&&!this.closed){
        this.localIceCount++;
        this.signal(this.role==='host'?'guest':'host',{type:'ice',candidate:e.candidate.toJSON()}).catch(error=>this.fail(error));
        this.rtcProgress();
      }
    };
    this.pc.onicecandidateerror=e=>{this.lastIceError=Number.isInteger(e.errorCode)?`（ICE 错误 ${e.errorCode}）`:'';};
    this.pc.oniceconnectionstatechange=()=>this.rtcProgress();
    this.pc.onconnectionstatechange=()=>{
      if(this.closed)return;
      const state=this.pc.connectionState;
      if(state==='failed'||state==='closed')this.fail(Error(this.rtcFailure()));
      else if(state==='disconnected'){
        this.disconnectedAt??=performance.now();
        this.onState('网络暂时中断，正在等待恢复');
        this.disconnectTimer??=setTimeout(()=>this.fail(Error(this.rtcFailure())),10000);
      }else if(state==='connected'){
        this.disconnectedAt=null;
        clearTimeout(this.disconnectTimer);this.disconnectTimer=null;this.refreshPath();
      }
    };
    this.pc.ondatachannel=e=>{try{acceptInputChannel(this.pc,e.channel,'th03-inputs',channel=>this.attach(channel));}catch(error){this.fail(error);}};
    const connected=new Promise((resolve,reject)=>{this.rtcResolve=resolve;this.rtcReject=reject;});
    this.rtcTimer=setTimeout(()=>this.fail(Error(this.rtcFailure())),45000);
    this.readSignals();
    if(this.role==='host'){
      // The transport retains immutable frame packets, including touch deltas
      // and commands, until acknowledged; duplicate arrivals execute once.
      Promise.resolve().then(async()=>{
        this.attach(createInputChannel(this.pc,'th03-inputs'));
        await this.pc.setLocalDescription(await this.pc.createOffer());
        await this.signal('guest',{type:'offer',description:this.pc.localDescription});
      }).catch(error=>this.fail(error));
    }
    return connected;
  }
  rtcFailure(){
    const detail=this.rtcDetail();
    const help=this.network!=='public'?'请检查设备间 UDP 通信、防火墙或 Wi-Fi 设备隔离，或改用 TCP 服务器转发。':
      !this.rtcHasTurn?'当前未配置 TURN；请配置 TURN 或改用 TCP。':'请检查 STUN / TURN 网络，或改用 TCP 服务器转发。';
    return `WebRTC 连接失败或超时${this.lastIceError||''}（${detail}）。${help}结束后重新建房再试。`;
  }
  rtcDetail(){
    const phase=this.remoteDescriptionReady?'已收到对方协商':this.role==='host'?'等待客机应答':'等待房主连接请求';
    return `${phase}；ICE ${this.pc?.iceConnectionState||'new'}；候选发送 ${this.localIceCount||0} / 收到 ${this.remoteIceCount||0}`;
  }
  rtcProgress(){
    if(this.closed||this.channel?.readyState==='open')return;
    this.onState(`正在建立${this.network==='public'?'公网':'局域网'}连接：${this.rtcDetail()}`);
  }
  attach(channel){
    if(this.closed){channel.close();return;}
    if(this.channel){channel.close();this.fail(Error('收到重复同步通道'));return;}
    this.channel=channel;
    channel.onopen=()=>{
      if(this.closed)return;
      clearTimeout(this.rtcTimer);this.rtcResolve?.();this.rtcResolve=this.rtcReject=null;
      this.onState('WebRTC 已连接 · 正在确认路径');this.refreshPath();
      this.pathTimer=setInterval(()=>this.refreshPath(),5000);
    };
    channel.onmessage=e=>{try{this.receive(JSON.parse(e.data));}catch(error){this.fail(error);}};
    channel.onerror=()=>this.fail(Error('WebRTC 数据通道错误'));
    channel.onclose=()=>this.fail(Error('WebRTC 数据通道已断开，请重新建房'));
  }
  async refreshPath(){
    if(this.closed||!this.pc||this.pathPending)return;
    this.pathPending=true;
    try{const stats=await this.pc.getStats();if(!this.closed&&this.pc.connectionState==='connected'&&this.channel?.readyState==='open'){this.path=describeRtcPath(stats).replace(/ · RTT .*$/,'');this.onState(describeRtcPath(stats));}}
    catch{}finally{this.pathPending=false;}
  }
  async readSignals(){
    if(this.closed)return;
    try{
      const query=new URLSearchParams({protocol,room:this.room,token:this.token,runId:this.runId});
      const response=await fetch(`/api/signals?${query}`,{cache:'no-store',signal:this.rtcAbort.signal});
      const value=await response.json();if(!response.ok)throw Error(value.error||'信令读取失败');
      for(const item of value.messages||[]){if(this.closed)return;await this.handleSignal(item.from,item.message);}
    }catch(error){if(!this.closed)this.fail(error);}
    // Continue receiving late trickled candidates after DataChannel opens.
    // Chained timeouts prevent overlapping polls from reordering offer/ICE.
    if(!this.closed)this.rtcPoll=setTimeout(()=>this.readSignals(),this.channel?.readyState==='open'?1000:300);
  }
  async handleSignal(from,message){
    if(message.type==='offer'&&this.role==='guest'){
      await this.pc.setRemoteDescription(message.description);this.remoteDescriptionReady=true;
      for(const candidate of this.pendingIce.splice(0))await this.pc.addIceCandidate(candidate);
      await this.pc.setLocalDescription(await this.pc.createAnswer());
      await this.signal('host',{type:'answer',description:this.pc.localDescription});
    }else if(message.type==='answer'&&this.role==='host'){
      await this.pc.setRemoteDescription(message.description);this.remoteDescriptionReady=true;
      for(const candidate of this.pendingIce.splice(0))await this.pc.addIceCandidate(candidate);
    }else if(message.type==='ice'){
      this.remoteIceCount++;
      if(this.remoteDescriptionReady)await this.pc.addIceCandidate(message.candidate);else this.pendingIce.push(message.candidate);
    }
    this.rtcProgress();
  }
  closeRtc(){clearTimeout(this.rtcPoll);clearTimeout(this.rtcTimer);clearTimeout(this.disconnectTimer);clearInterval(this.pathTimer);this.rtcAbort?.abort();this.rtcPoll=null;this.rtcTimer=null;}
  close(){if(this.closed)return;this.closed=true;clearInterval(this.metricsTimer);this.onMetrics?.(this.connectionMetrics());for(const pending of this.probes?.values()??[]){clearTimeout(pending.timer);pending.reject(Error('同步连接已关闭'));}this.probes?.clear();this.pendingPackets=[];this.rtcReject?.(Error('WebRTC 连接已结束'));this.relayReject?.(Error('TCP 连接已结束'));this.rtcResolve=this.rtcReject=this.relayReject=null;this.remote.clear();clearInterval(this.heartbeat);clearTimeout(this.relayTimer);this.closeRtc();this.socket?.close();this.channel?.close?.();this.pc?.close();}
}
