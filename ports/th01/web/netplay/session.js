import {PROTOCOL,FrameQueue} from './queue.js';
import {createInputChannel,acceptInputChannel} from './input-transport.js';
import {createRecorder} from '../replay/recorder.js';
import {GameRuntime} from './runtime.js';
import {WorkerGameRuntime,supportsSimulationWorker} from './runtime-worker-client.js';
import {createRelay} from './relay.js';
import {loadRtcConfiguration,refreshRtcPath,countIceCandidate,describeIceCandidates,recordIceServerError,describeRtcConfiguration} from './connection.js';
import {deploymentIceConfiguration} from './udp-config.js';
import {describeNetwork} from './network-mode.js';

const ROLES=['host','guest','guest2'],TICK=1000*440/24830;
const HASH_INTERVAL=120,HASH_WINDOW=2*HASH_INTERVAL,DISCONNECT_GRACE=10000;
const BUFFER_PROBES=5,BUFFER_PROBE_TICK=120,BUFFER_PROBE_TIMEOUT=2500;
export class NetSession{
  constructor(identity,room,{canvas,api,status,buttons,render,failure}){
    Object.assign(this,{identity,room,api,status,buttons,render,failure});
    this.roles=ROLES.slice(0,room.settings.players||2);
    this.remotes=this.roles.filter(r=>r!==identity.role);this.links=identity.role==='host'?this.remotes:['host'];
    this.slot=room.seats[identity.role];this.peers=new Map();
    this.hellos=new Map();this.acks=new Set();this.sentHello=new Set();this.sentAck=new Set();
    const worker=new URLSearchParams(location.search).get('netWorker')!=='0'&&supportsSimulationWorker();
    this.game=new (worker?WorkerGameRuntime:GameRuntime)(canvas,room.seats.host,this.slot);
    this.recorder=createRecorder(document.querySelector('.toolbar'));globalThis.th01Replay=this.recorder;
    this.game.onFailure=e=>this.fail(e);
    this.autoBuffer=room.settings.delay===0;
    this.queue=new FrameQueue(this.slot,this.autoBuffer?(room.settings.network==='public'?6:3):room.settings.delay,this.roles.length);
    this.hashes=new Map();this.remoteHashes=new Map(this.remotes.map(r=>[r,new Map()]));this.iceCounts={};this.iceErrors=[];
    this.closed=false;this.running=false;this.connected=false;this.lastAdvance=performance.now();
    this.rtt=0;this.lastHash=0;this.waitHash=false;this.signalChain=Promise.resolve();
    this.waitStats={since:performance.now(),elapsed:0,input:0,hash:0};
  }
  sendTo(role,value,source=this.identity.role){
    const channel=this.peers.get(role)?.channel;
    if(this.closed||channel?.readyState!=='open')throw Error('输入通道尚未就绪');
    if(channel.bufferedAmount>262144)throw Error('网络发送积压，请重开房间');
    channel.send(JSON.stringify({...value,from:source,protocol:PROTOCOL,generation:this.room.generation,runId:this.room.runId}));
  }
  send(value){for(const role of this.links)this.sendTo(role,value);}
  get inputTransportStats(){return Object.fromEntries([...this.peers].map(([role,peer])=>[role,peer.channel?.transportStats??null]));}
  signal(target,message){
    this.signalChain=this.signalChain.then(()=>this.api('signal',{generation:this.room.generation,runId:this.room.runId,target,message}));
    this.signalChain.catch(e=>this.fail(e));return this.signalChain;
  }
  async prepare(){
    this.loadTask=this.game.load(this.room.settings,this.status).then(async hash=>{
      if(this.closed){this.game.stop();return;}
      await this.recorder.boot({settings:this.room.settings,players:this.roles.length,hostSlot:this.room.seats.host,opening:hash,tickMs:TICK});
      if(this.closed){this.recorder.close(true);return;}
      if(this.identity.role==='host')this.recorder.connectNetwork({...this.identity,generation:this.room.generation,runId:this.room.runId});
      this.loadedHash=hash;this.maybeHello();
    }).catch(e=>this.fail(e));
    if(this.room.transport==='ws'){
      this.relay=createRelay({...this.identity,runId:this.room.runId},this.room.generation,this.roles,e=>this.fail(e));
      for(const role of this.links){this.peers.set(role,{});this.attach(role,this.relay.channelFor(role));}
      this.relay.connect();
    }else{
      if(this.room.settings.network==='public'&&location.protocol!=='https:'&&
        !['localhost','127.0.0.1','[::1]'].includes(location.hostname))throw Error('公网 UDP 请使用 HTTPS 入口；可运行 start-public-tunnel.bat 获取地址');
      if(!globalThis.RTCPeerConnection)throw Error('浏览器不支持 WebRTC，请使用 Edge / Chrome 或备用转发');
      const network=this.room.settings.network??'lan',ice=this.room.settings.ice??'all';
      const config=await loadRtcConfiguration({...this.identity,network,ice},globalThis.th01NetplayUdp??deploymentIceConfiguration);
      if(network==='public'&&ice==='relay'&&config.iceTransportPolicy!=='relay')throw Error('本房间要求强制 TURN，连接配置未采用中继策略');
      this.iceConfigSummary=describeRtcConfiguration(config);
      if(this.closed)return;
      for(const role of this.links){
        const pc=new RTCPeerConnection(config),peer={pc,candidates:[],path:'正在建立 UDP 连接…'};this.peers.set(role,peer);
        pc.onicecandidate=e=>{if(e.candidate){countIceCandidate(this.iceCounts,e.candidate);this.signal(role,{type:'ice',candidate:e.candidate.toJSON()});}};
        pc.onicecandidateerror=e=>recordIceServerError(this.iceErrors,e);
        pc.onconnectionstatechange=()=>this.connectionChanged(role);
        pc.ondatachannel=e=>{
          try{acceptInputChannel(pc,e.channel,'th01-inputs',channel=>this.attach(role,channel));}catch(error){this.fail(error);}
        };
        if(this.identity.role==='host'){
          this.attach(role,createInputChannel(pc,'th01-inputs'));
          await pc.setLocalDescription(await pc.createOffer());
          await this.signal(role,{type:'offer',sdp:pc.localDescription.sdp});
        }
      }
    }
    this.connectTimer=setTimeout(()=>{if(!this.connected)this.fail(Error('打洞连接超过 30 秒：可重新建房使用 TURN（需已配置）或 TCP 服务器转发'));},30000);
    this.heartbeat=setInterval(()=>{
      if(!this.connected||this.closed)return;
      try{this.pingPeers();}catch(e){this.fail(e);}
      for(const peer of this.peers.values())if(peer.pc)refreshRtcPath(peer);
    },2000);
    this.loadTimer=setTimeout(()=>{if(!this.running)this.fail(Error('启动超过 120 秒：检查全员资源下载与版本'));},120000);
  }
  connectionChanged(role){
    const peer=this.peers.get(role);if(this.closed||!peer?.pc)return;
    const state=peer.pc.connectionState;
    if(state==='failed'||state==='closed'){
      this.fail(Error(`P${this.room.seats[role]+1} 的 UDP 连接断开，请重建房间或使用备用转发`));
    }else if(state==='disconnected'){
      // ICE can recover a brief interruption without replacing the input lane.
      // Keep reliable input delivery and the usual lockstep wait in place.
      if(peer.disconnectTimer==null){
        peer.disconnectedAt=performance.now();
        peer.disconnectTimer=setTimeout(()=>{
          if(!this.closed)this.fail(Error(`P${this.room.seats[role]+1} 的 UDP 连接中断超过 10 秒，请重建房间或使用备用转发`));
        },DISCONNECT_GRACE);
      }
    }else if(state==='connected'){
      clearTimeout(peer.disconnectTimer);peer.disconnectTimer=null;peer.disconnectedAt=null;refreshRtcPath(peer);
    }
  }
  pingPeers(){
    for(const role of this.links){
      const peer=this.peers.get(role),time=performance.now();
      peer.pings??=new Set();peer.pings.add(time);
      while(peer.pings.size>(this.running?4:24))peer.pings.delete(peer.pings.values().next().value);
      this.sendTo(role,{type:'ping',time});
    }
  }
  recordPong(role,time){
    const peer=this.peers.get(role);
    if(!Number.isFinite(time)||!peer?.pings?.delete(time))return;
    peer.rttAt=performance.now();
    const sample=performance.now()-time;if(sample<0||sample>10000)return;
    const previous=peer.rtt;
    peer.jitter=previous==null?0:(peer.jitter||0)*0.75+Math.abs(sample-previous)*0.25;
    peer.rtt=previous==null?sample:previous*0.75+sample*0.25;
    peer.samples=(peer.samples||0)+1;
    if(!this.running)peer.probePeak=Math.max(peer.probePeak||0,sample);
    this.rtt=Math.max(...[...this.peers.values()].map(p=>p.rtt||0));
    if(this.bufferTimer!=null)this.maybeHello();
  }
  async receiveSignal(message){
    if(this.closed)return;
    if(message.type==='ended'){this.fail(Error('队友停止了本局'),false);return;}
    const role=message.from,peer=this.peers.get(role),pc=peer?.pc;
    if(!pc)throw Error('RTC 信令来源或传输不匹配');
    if(message.type==='ice'){
      if(pc.remoteDescription)await pc.addIceCandidate(message.candidate);else peer.candidates.push(message.candidate);
    }else{
      const expected=this.identity.role==='host'?'answer':'offer';
      if(message.type!==expected||typeof message.sdp!=='string')throw Error('RTC 协商身份无效');
      await pc.setRemoteDescription({type:message.type,sdp:message.sdp});
      for(const candidate of peer.candidates)await pc.addIceCandidate(candidate);peer.candidates=[];
      if(expected==='offer'){await pc.setLocalDescription(await pc.createAnswer());await this.signal(role,{type:'answer',sdp:pc.localDescription.sdp});}
    }
  }
  attach(role,channel){
    this.peers.get(role).channel=channel;
    channel.onopen=()=>{try{
      this.connected=this.links.every(r=>this.peers.get(r)?.channel?.readyState==='open');
      if(this.connected){clearTimeout(this.connectTimer);this.status('连接已建立，等待全员资源校验…');}this.maybeHello();
    }catch(e){this.fail(e);}};
    channel.onmessage=e=>{try{this.receive(JSON.parse(e.data),role);}catch(error){this.fail(error);}};
    channel.onerror=()=>this.fail(Error('游戏输入通道发生错误'));
    channel.onclose=()=>{if(!this.closed)this.fail(Error('队友断线，本局已停止'));};
  }
  maybeHello(){
    if(this.closed||!this.loadedHash)return;
    for(const role of this.links){
      if(this.peers.get(role)?.channel?.readyState!=='open')continue;
      if(!this.sentHello.has(role)){this.sentHello.add(role);this.sendTo(role,{type:'hello',hash:this.loadedHash,slot:this.slot});}
      const hello=this.hellos.get(role);
      if(hello){
        if(hello.hash!==this.loadedHash)throw Error('游戏、运行时或设置不同，请刷新全员页面');
        if(!this.sentAck.has(role)){this.sentAck.add(role);this.sendTo(role,{type:'loaded'});}
      }
    }
    if(this.identity.role==='host'&&!this.running&&this.links.every(r=>this.sentAck.has(r)&&this.acks.has(r))){
      if(this.autoBuffer&&!this.bufferReady){
        if(!this.bufferProbing){
          this.bufferProbing=true;this.rtt=0;
          // Resource loading can block the main thread. Discard those old
          // heartbeat samples and outstanding pongs before startup probing.
          for(const peer of this.peers.values()){
            peer.pings?.clear();peer.rtt=null;peer.jitter=0;peer.samples=0;peer.probePeak=0;
          }
        }
        if(this.links.every(r=>(this.peers.get(r)?.samples||0)>=BUFFER_PROBES)){
          this.queue=new FrameQueue(this.slot,this.recommendedDelay(true),this.roles.length);this.bufferReady=true;
        }else{
          if(this.bufferTimer==null){
            this.status('测量连接延迟，选择输入缓冲…');
            this.bufferTimer=setInterval(()=>{try{this.pingPeers();}catch(e){this.fail(e);}},BUFFER_PROBE_TICK);
            this.bufferTimeout=setTimeout(()=>{
              try{
                if(this.closed||this.running)return;
                // Keep a conservative floor if a slow link did not finish probing.
                this.queue=new FrameQueue(this.slot,Math.max(this.queue.delay,this.recommendedDelay(true)),this.roles.length);
                this.bufferReady=true;this.bufferFallback=true;this.maybeHello();
              }catch(e){this.fail(e);}
            },BUFFER_PROBE_TIMEOUT);
            this.pingPeers();
          }
          return;
        }
      }
      // Pace frame zero together. Starting the host immediately would consume
      // part of the buffer while "go" is still travelling to the guests.
      const travel=role=>this.autoBuffer?(this.peers.get(role)?.probePeak||0)/2:0;
      const lead=this.autoBuffer?Math.max(...this.links.map(travel))+TICK:0;
      for(const role of this.links)this.sendTo(role,{type:'go',delay:this.queue.delay,
        fallback:!!this.bufferFallback,startAfter:lead-travel(role)});
      this.start(lead);
    }
  }
  receive(message,link){
    if(this.closed)return;
    if(message.protocol!==PROTOCOL||message.generation!==this.room.generation||message.runId!==this.room.runId)throw Error('联机协议或运行代次不同');
    const source=message.from;
    if(!this.links.includes(link)||!this.remotes.includes(source)||
      (this.identity.role==='host'&&source!==link)||
      (source!==link&&!['input','hash'].includes(message.type)))throw Error('联机来源无效');
    switch(message.type){
      case 'hello':
        if(this.hellos.has(source)||message.slot!==this.room.seats[source]||typeof message.hash!=='string')throw Error('队友校验身份无效');
        this.hellos.set(source,message);this.maybeHello();break;
      case 'loaded':
        if(!this.hellos.has(source)||this.acks.has(source))throw Error('开局握手顺序无效');
        this.acks.add(source);this.maybeHello();break;
      case 'go':
        if(this.identity.role==='host'||source!=='host'||!this.sentAck.has('host')||!this.acks.has('host')||this.running)throw Error('未完成校验就尝试开局');
        if(!Number.isInteger(message.delay)||message.delay<1||message.delay>12||
          (!this.autoBuffer&&message.delay!==this.room.settings.delay)||typeof message.fallback!=='boolean'||
          !Number.isFinite(message.startAfter)||message.startAfter<0||message.startAfter>10000)throw Error('开局输入缓冲无效');
        this.queue=new FrameQueue(this.slot,message.delay,this.roles.length);this.bufferFallback=message.fallback;
        this.start(message.startAfter);break;
      case 'input':{
        if(!this.running||message.slot!==this.room.seats[source])throw Error('输入座位或开局状态无效');
        const commands=message.buttons&~(255|4096);
        if(commands&&(source!=='host'||(!this.room.settings.qa&&commands!==256)))throw Error('队友发送了未授权控制命令');
        this.queue.receive(message.frame,message.buttons,message.slot);this.forward(message,source);this.pump();break;
      }
      case 'hash':{
        const hashes=this.remoteHashes.get(source);
        if(!this.running||!Number.isSafeInteger(message.frame)||message.frame<=0||message.frame%HASH_INTERVAL||message.frame>this.queue.frame+HASH_INTERVAL||
          message.frame<=this.lastHash||hashes.has(message.frame)||!/^[0-9a-f]{8}$/.test(message.hash)||typeof message.memory!=='boolean')throw Error('状态摘要帧无效');
        hashes.set(message.frame,message);this.forward(message,source);this.checkHashes();this.pump();break;
      }
      case 'ping':if(!Number.isFinite(message.time))throw Error('探测无效');this.sendTo(link,{type:'pong',time:message.time});break;
      case 'pong':this.recordPong(link,message.time);break;
      case 'ended':this.fail(Error('队友停止了本局'));break;
      default:throw Error('未知联机消息');
    }
  }
  forward(message,source){
    if(this.identity.role==='host')for(const role of this.links)if(role!==source)this.sendTo(role,message,source);
  }
  checkHashes(){
    for(const [frame,local] of this.hashes){
      // Confirm in sequence; a later match must never hide an earlier mismatch.
      if(!this.remotes.every(r=>this.remoteHashes.get(r).has(frame)))break;
      for(const role of this.remotes){
        const remote=this.remoteHashes.get(role).get(frame);
        if(local.hash!==remote.hash||local.memory!==remote.memory){
          this.diagnostic={frame,role,local,remote,state:this.game.state()};throw Error(`第 ${frame} 步状态不同，本局已停止`);
        }
      }
      this.lastHash=frame;this.hashes.delete(frame);for(const hashes of this.remoteHashes.values())hashes.delete(frame);
    }
    this.waitHash=this.queue.frame-this.lastHash>=HASH_WINDOW;
  }
  start(startAfter=0){
    clearInterval(this.bufferTimer);clearTimeout(this.bufferTimeout);this.bufferTimer=null;
    this.running=true;clearTimeout(this.loadTimer);this.game.start();
    this.status('联机运行中');this.lastAdvance=performance.now();this.nextTick=this.lastAdvance+startAfter;
    this.waitStats={since:this.lastAdvance,at:this.lastAdvance,elapsed:0,input:0,hash:0};
    this.performanceSample={at:this.lastAdvance,frame:this.game.frame,draws:this.game.displayFrames||0,visible:!document.hidden};
    this.timer=setInterval(()=>this.pump(),4);
  }
  async pump(){
    if(this.closed||!this.running||this.pumping)return;
    this.pumping=true;
    try{
      const now=performance.now();
      if(now-this.lastAdvance>30000)throw Error('等待队友输入超过 30 秒，请重开房间');
      this.recordWait(now);
      this.waitKind=null;
      let budget=4;const started=performance.now();
      while(now>=this.nextTick&&budget-->0&&(budget===3||performance.now()-started<6)){
        // Do not consume short taps while this immutable frame is already sampled.
        if(!this.queue.lanes[this.slot].has(this.queue.frame+this.queue.delay)){
          // Keep authority flowing while hidden; already published frames stay
          // immutable, and background pages never consume live device edges.
          const packet=this.queue.capture(document.hidden?0:this.buttons());if(packet)this.send(packet);
        }
        // Hashes normally compare in the background. Stop only at the bounded
        // horizon; still publish this frame's immutable input for slower peers.
        this.waitHash=this.queue.frame-this.lastHash>=HASH_WINDOW;
        if(this.waitHash){this.waitKind='hash';this.nextTick=Math.max(this.nextTick,now);break;}
        const inputs=this.queue.peek();
        if(!inputs){this.waitKind='input';this.nextTick=Math.max(this.nextTick,now);break;}
        const recordedFrame=this.queue.frame;
        if(this.game.asyncExecution)await this.game.execute(inputs,recordedFrame);else this.game.step(inputs);
        if(this.closed)return;
        this.recorder.stage(recordedFrame,{inputs,offlineMask:0,proof:this.game.replayProof(),hash:(recordedFrame+1)%120===0?this.game.checksum():null});
        this.recorder.confirm(recordedFrame+1,false);this.queue.commit();
        const nativeState=this.game.state();
        if(nativeState&&!nativeState.menu)this.sawGameplay=true;
        if(this.sawGameplay&&nativeState?.menu===1){this.fail(Error('已返回标题，本局结束。'),false);return;}this.lastAdvance=performance.now();this.nextTick+=TICK;
        if(this.nextTick<now-4*TICK)this.nextTick=now;
        if(this.queue.frame%HASH_INTERVAL===0){
          const checksum=this.game.checksum();this.hashes.set(checksum.frame,checksum);
          this.send(checksum);this.checkHashes();
        }
      }
      if(!document.hidden){
        this.game.requestPresent?.();
        if(now-(this.renderAt||0)>150){this.renderAt=now;this.render(this);}
      }
    }catch(error){this.fail(error);}finally{this.pumping=false;}
  }
  recordWait(now){
    const sample=this.performanceSample;
    if(sample){
      if(document.hidden)sample.visible=false;
      const duration=now-sample.at;
      if(duration>=5000){
        // Wall time includes foreground stalls. Suppress windows containing
        // background throttling, which are not representative of active play.
        this.performanceRates=sample.visible&&!document.hidden?{
          logic:1000*(this.game.frame-sample.frame)/duration,
          display:1000*((this.game.displayFrames||0)-sample.draws)/duration
        }:null;
        this.performanceSample={at:now,frame:this.game.frame,draws:this.game.displayFrames||0,visible:!document.hidden};
      }
    }
    const stats=this.waitStats,elapsed=now-(stats.at??now);stats.at=now;
    if(!document.hidden&&elapsed>=0&&elapsed<1000){
      stats.elapsed+=elapsed;
      if(this.waitKind==='hash')stats.hash+=elapsed;
      else if(this.waitKind==='input')stats.input+=elapsed;
    }
    if(now-stats.since>=5000){
      this.waitPercent=stats.elapsed?100*(stats.input+stats.hash)/stats.elapsed:0;
      this.inputWaitPercent=stats.elapsed?100*stats.input/stats.elapsed:0;
      this.hashWaitPercent=stats.elapsed?100*stats.hash/stats.elapsed:0;
      stats.since=now;stats.elapsed=stats.input=stats.hash=0;
    }
  }
  waitingText(){
    if(document.hidden)return '页面隐藏，继续同步（本机中性输入）…';
    const interrupted=[...this.peers].filter(([,p])=>p.disconnectTimer!=null).map(([r])=>`P${this.room.seats[r]+1}`);
    if(interrupted.length)return `等待队友 ${interrupted.join(' / ')} 的 UDP 连接恢复（最多 10 秒）…`;
    if(this.waitHash){
      const frame=this.hashes.keys().next().value;
      const names=this.remotes.filter(r=>!this.remoteHashes.get(r).has(frame)).map(r=>`P${this.room.seats[r]+1}`);
      return `等待队友 ${names.join(' / ')} 核对第 ${frame} 步状态…`;
    }
    if(performance.now()-this.lastAdvance>1000){
      const slots=this.queue.lanes.flatMap((lane,i)=>i!==this.slot&&!lane.has(this.queue.frame)?[`P${i+1}`]:[]);
      if(slots.length)return `等待队友 ${slots.join(' / ')} 的输入…`;
    }
    return '联机运行中';
  }
  recommendedDelay(startup=false){
    // The two longest host links cover the guest -> host -> guest path in 3P.
    // Use the probing peak at startup, then EWMA RTT for subsequent advice.
    const paths=[...this.peers.values()].filter(p=>p.samples).map(p=>
      (startup?p.probePeak??p.rtt:p.rtt)/2+(p.jitter||0)).sort((a,b)=>b-a);
    // A guest sees only its host link: double it for conservative 3P advice.
    const travel=this.roles.length===3?(paths.length===1?2*paths[0]:paths.slice(0,2).reduce((a,b)=>a+b,0)):(paths[0]||0);
    // One-step buffering is enough only when the complete input path plus
    // jitter fits one tick, including the 4 ms pump scheduling allowance.
    // Keep the existing safety margin on longer or unstable paths.
    if(paths.length&&travel+4<=TICK)return 1;
    return Math.max(2,Math.min(12,Math.ceil(travel/TICK)+1));
  }
  networkQualityText(){
    const peers=[...this.peers.values()].filter(p=>p.samples),jitter=Math.max(0,...peers.map(p=>p.jitter));
    const measured=peers.length===this.links.length;
    const canRecommend=measured&&(this.roles.length===2||this.identity.role==='host');
    const recommendation=this.recommendedDelay();
    return `${this.autoBuffer?'自动选定 · ':''}缓冲 ${this.queue.delay} 帧（约 ${(this.queue.delay*TICK).toFixed(0)} ms）${this.bufferFallback?' · 测量超时，采用保守缓冲':''}`+
      (measured?` · 最大往返 ${this.rtt.toFixed(0)} ms · 抖动约 ${jitter.toFixed(0)} ms`:' · 正在测量延迟')+
      (this.waitPercent!=null?` · 近 5 秒同步等待 ${this.waitPercent.toFixed(1)}%（输入 ${(this.inputWaitPercent||0).toFixed(1)}% / 校验 ${(this.hashWaitPercent||0).toFixed(1)}%）`:'')+
      (this.performanceRates?`\n实际逻辑 ${this.performanceRates.logic.toFixed(1)} 步/秒（正常约 56.4） · 本机画面 ${this.performanceRates.display.toFixed(1)} FPS · 绘图均值 ${(this.game.renderCost||0).toFixed(1)} ms`:'')+
      (this.performanceRates?` · ${this.game.renderBackend==='worker'?'独立绘图线程':'主线程绘图'} · 主线程绘图占用 ${(this.game.mainRenderCost||0).toFixed(1)} ms`:'')+
      (canRecommend&&recommendation>this.queue.delay?`\n下次开房建议缓冲 ${recommendation} 帧${recommendation===12?'；较高延迟仍可能需要等待':''}`:
        canRecommend&&recommendation<this.queue.delay&&this.waitPercent!=null&&this.waitPercent<1?`\n下次开房可选自动或 ${recommendation} 帧，减少操作等待`:'');
  }
  networkStatus(){
    const now=performance.now();
    return {phase:this.closed?'stopped':this.running?'connected':'connecting',bufferFrames:this.queue.delay,bufferMs:this.queue.delay*TICK,
      peers:[...this.peers].map(([role,p])=>({name:`P${this.room.seats[role]+1}`,path:this.room.transport==='ws'?'WebSocket 中继':p.path,
        state:p.disconnectTimer!=null?'recovering':p.channel?.readyState==='open'?'connected':'connecting',
        rttMs:p.samples?p.rtt:null,rttAgeMs:p.rttAt==null?null:now-p.rttAt,
        remainingMs:p.disconnectedAt==null?null:Math.max(0,DISCONNECT_GRACE-(now-p.disconnectedAt)),recovery:'等待网络自行恢复'}))};
  }
  connectionText(){
    const mode=describeNetwork({...this.room.settings,transport:this.room.transport});
    const links=[...this.peers].map(([role,p])=>`P${this.room.seats[role]+1}: ${this.room.transport==='ws'?'WebSocket':`${p.path} · ICE ${p.pc?.iceConnectionState||'new'}`} · DataChannel ${p.channel?.readyState||'等待'}${p.samples?` · 往返 ${p.rtt.toFixed(0)} ms`:''}${p.disconnectTimer!=null?' · 短暂中断，等待恢复':''}${p.iceDiagnostics?'\n'+p.iceDiagnostics:''}`).join('\n');
    return `${mode}\n`+(this.room.transport==='ws'?'':`${this.iceConfigSummary||'正在获取打洞配置…'}\n`)+links+`\n${this.networkQualityText()}`+(this.room.transport==='ws'?'':`\n${describeIceCandidates(this.iceCounts)}${this.iceErrors.length?'\n'+this.iceErrors.join('；'):''}`);
  }
  fail(error,notify=true){
    if(this.closed)return;
    if(notify)for(const role of this.links)if(this.peers.get(role)?.channel?.readyState==='open')try{this.sendTo(role,{type:'ended'});}catch{}
    this.close();

    this.failure(error.message||String(error),this);
  }
  close(){
    if(this.closed)return;this.closed=true;this.running=false;
    for(const timer of [this.timer,this.heartbeat,this.bufferTimer])clearInterval(timer);
    for(const timer of [this.connectTimer,this.loadTimer,this.bufferTimeout])clearTimeout(timer);
    this.recorder.finish('联机会话结束');this.game.stop();this.relay?.close();for(const peer of this.peers.values()){clearTimeout(peer.disconnectTimer);peer.disconnectTimer=null;peer.channel?.close();peer.pc?.close();}
  }
}
