import {createInputChannel, acceptInputChannel} from './netplay/input-transport.js';

// The Launcher owns membership. This adapter only owns one immutable run's
// gameplay and signaling sockets, using the main relay's selected route.
export function createManagedTransport(options, roles, fail, environment = globalThis) {
  const local = options.netplayPlayer, count = options.netplayPlayerCount;
  if (![2,3].includes(count) || !Number.isInteger(local) || local < 0 || local >= count) throw Error('联机座位无效');
  const base = new URL(options.netplayUrl);
  if (!['ws:','wss:'].includes(base.protocol) || !base.searchParams.get('member') || !base.searchParams.get('run')) throw Error('缺少主项目房间会话');
  base.searchParams.set('player', String(local)); base.searchParams.set('players', String(count));
  let data, signal, route = null, closed = false, rtcFailed = false, rtcReported = false;
  let earlyBytes = 0, timeout, signalRetry;
  const pendingSignals=[];
  let endTask,endResolve,endReject,endTimer,endGrace,localEnded=false;
  const endSeen=new Set(),endAck=new Set();
  const peers = new Map(), early = [], encoder = new TextEncoder(), decoder = new TextDecoder('utf-8', {fatal:true});
  const fatal = error => { if (!closed) {
    if(localEnded&&endSeen.size===peers.size&&endAck.size===peers.size){clearTimeout(endTimer);clearTimeout(endGrace);endReject=null;endResolve?.();return;}
    status.failed=true;status.error=String(error.message||error);close(false);fail(error instanceof Error ? error : Error(String(error)));
  } };
  const guard = fn => (...args) => { if (!closed) try { const result = fn(...args); result?.catch?.(fatal); } catch (error) { fatal(error); } };
  function control(message){
    if(closed)return;
    const text=JSON.stringify(message);
    if(signal?.readyState===1)signal.send(text);
    else if(route==='rtc')pendingSignals.push(text);
  }
  function close(notifyPeers=true) {
    if (closed) return; closed = true;status.disconnected=true;clearTimeout(timeout);clearTimeout(signalRetry);
    pendingSignals.length=0;
    clearTimeout(endTimer);clearTimeout(endGrace);endReject?.(Error('本局网络在结束确认前关闭'));endReject=null;
    for (const peer of peers.values()) {
      clearRecovery(peer);
      peer.channel.readyState = 'closed'; if(notifyPeers)peer.channel.onclose?.();
      peer.rtc?.close(); peer.pc?.close();
    }
    early.length = 0; earlyBytes = 0;
    for (const socket of [data, signal]) if (socket) { socket.onopen = socket.onmessage = socket.onerror = socket.onclose = null; socket.close(); }
  }
  function fallback() {
    if (closed || route || rtcFailed) return;
    rtcFailed = true; control({type:'rtc-failed'});
  }
  function deliver(peer, text) {
    if (typeof text !== 'string' || encoder.encode(text).length > 16382) throw Error('同步消息过大');
    if (!route || peer.channel.readyState !== 'open') {
      earlyBytes += encoder.encode(text).length;
      if (early.length >= 512 || earlyBytes > 262144) throw Error('开局接收队列已满');
      early.push([peer,text]); return;
    }
    const message=JSON.parse(text);
    if(message.type==='eagler-run-ended'){
      endSeen.add(peer.id);peer.channel.send(JSON.stringify({type:'eagler-run-end-ack'}));checkEnd();return;
    }
    if(message.type==='eagler-run-end-ack'){endAck.add(peer.id);checkEnd();return;}
    peer.channel.onmessage?.({data:text});
  }
  function checkEnd(){
    if(!localEnded||endSeen.size!==peers.size||endAck.size!==peers.size||endGrace)return;
    // Every native engine has now confirmed its exit. Leave time for the
    // reliable acknowledgements to drain before either document is retired.
    endGrace=setTimeout(()=>{clearTimeout(endTimer);endReject=null;endResolve?.();},200);
  }
  function finish(){
    if(endTask)return endTask;
    if(closed||!route)return Promise.reject(Error('本局网络未就绪'));
    endTask=new Promise((resolve,reject)=>{endResolve=resolve;endReject=reject;endTimer=setTimeout(()=>reject(Error('等待全员结束确认超时')),10000);});
    localEnded=true;
    for(const peer of peers.values())peer.channel.send(JSON.stringify({type:'eagler-run-ended'}));
    checkEnd();return endTask;
  }
  function selected(mode,reason) {
    if(mode==='failed')throw Error(reason||'WebRTC 连接失败');
    if (!['rtc','relay'].includes(mode) || (route && route !== mode)) throw Error('无效网络路由');
    if (route) return;
    if (mode === 'rtc' && [...peers.values()].some(peer => peer.rtc?.readyState !== 'open')) throw Error('直连通道未就绪');
    route = mode; clearTimeout(timeout);
    if(mode==='rtc'&&!signal)signalRetry=setTimeout(connectSignal,1000);
    environment.__eaglerNetplayTransport=mode;environment.__eaglerNetplayPath=mode;
    for (const peer of peers.values()) {
      if (mode === 'relay') { peer.rtc?.close(); peer.pc?.close(); }
      peer.channel.readyState = 'open'; peer.channel.onopen?.();
    }
    for (const [peer,text] of early.splice(0)) deliver(peer,text);
    earlyBytes = 0;
  }
  function checkRtc() {
    if (!rtcReported && !rtcFailed && [...peers.values()].every(peer => peer.rtc?.readyState === 'open')) {
      rtcReported = true; control({type:'rtc-ready'});
    }
  }
  function attach(peer, rtc) {
    if (peer.rtc) throw Error('重复直连通道'); peer.rtc = rtc;
    rtc.onopen = checkRtc;
    rtc.onmessage = guard(event => { rtcProgress(peer);if (!route || route === 'rtc') deliver(peer,event.data); });
    rtc.oninputbatch = guard(batch => {
      rtcProgress(peer);
      if (route === 'rtc' && peer.channel.readyState === 'open' && peer.channel.oninputbatch) peer.channel.oninputbatch(batch);
      else if (!route || route === 'rtc') for (const text of batch) deliver(peer,text);
    });
    rtc.onerror = event => route === 'rtc' ? fatal(Error('直连通道失败：'+String(event?.error?.message||event?.error||'未知原因'))) : fallback();
    rtc.onclose = () => { if(peer.channel.readyState==='closed')return;if (route === 'rtc') fatal(Error('直连通道已关闭')); else if (!route) fallback(); };
  }
  async function addCandidate(pc,candidate) {
    // A stale trickled candidate (for example, from the previous ICE
    // generation) must not veto the other usable candidates.
    try { await pc.addIceCandidate(candidate); }
    catch (error) { if(error.name!=='OperationError')throw error; }
  }
  function clearRecovery(peer) {
    clearTimeout(peer.disconnectTimer);clearTimeout(peer.restartTimer);
    peer.disconnectTimer=peer.restartTimer=null;
  }
  function rtcHealthy(peer) {
    const {connectionState,iceConnectionState}=peer.pc;
    return connectionState==='connected'&&['connected','completed',undefined].includes(iceConnectionState);
  }
  function rtcProgress(peer) {
    // An ICE restart can complete without a new connection-state event,
    // especially when the old transport remained usable during negotiation.
    if((peer.disconnectTimer||peer.restartTimer)&&rtcHealthy(peer))clearRecovery(peer);
  }
  function restart(peer) {
    if(closed||route!=='rtc'||peer.channel.readyState!=='open'||peer.restartTimer)return;
    clearTimeout(peer.disconnectTimer);peer.disconnectTimer=null;
    // Restart the existing ICE session. DataChannel sequence numbers and
    // the unacknowledged rollback inputs remain on the same SCTP association.
    peer.restartTimer=setTimeout(()=>fatal(Error('玩家直连恢复超时')),6500);
    if(local>peer.id){control({type:'ice-restart-request',to:peer.id});return;}
    peer.chain=peer.chain.then(async()=>{
      if(closed||peer.channel.readyState!=='open')return;
      await peer.pc.setLocalDescription(await peer.pc.createOffer({iceRestart:true}));
      control({type:'signal',to:peer.id,description:peer.pc.localDescription.toJSON()});
    }).catch(error=>{if(peer.channel.readyState==='open')fatal(error);});
  }
  function rtcState(peer) {
    if(closed||route==='relay'||peer.channel.readyState==='closed')return;
    const {connectionState,iceConnectionState}=peer.pc;
    if(connectionState==='closed'){
      if(route==='rtc')fatal(Error('玩家直连断开'));else fallback();
    }else if(connectionState==='failed'||iceConnectionState==='failed'){
      if(route==='rtc')restart(peer);else fallback();
    }else if(route==='rtc'){
      if(rtcHealthy(peer))clearRecovery(peer);
      else if((connectionState==='disconnected'||iceConnectionState==='disconnected')&&!peer.disconnectTimer&&!peer.restartTimer){
        peer.disconnectTimer=setTimeout(()=>restart(peer),1500);
      }
    }
  }
  async function description(peer, value) {
    await peer.pc.setRemoteDescription(value);
    for (const candidate of peer.candidates.splice(0)) await addCandidate(peer.pc,candidate);
    if (value.type === 'offer') {
      await peer.pc.setLocalDescription(await peer.pc.createAnswer());
      control({type:'signal',to:peer.id,description:peer.pc.localDescription.toJSON()});
    }
  }
  async function setupRtc(message) {
    if (!environment.RTCPeerConnection) { fallback(); return; }
    const iceServers = Array.isArray(message.iceServers) ? message.iceServers : [];
    const relayOnly=message.initialIceRelayOnly===true||(message.turnMode!=='direct'&&options.netplayPreferTurn);
    for (const peer of peers.values()) {
      const pc = peer.pc = new environment.RTCPeerConnection({iceServers, ...(relayOnly ? {iceTransportPolicy:'relay'} : {})});
      pc.onicecandidate = guard(event => { if (event.candidate) control({type:'signal',to:peer.id,candidate:event.candidate.toJSON()}); });
      pc.onconnectionstatechange=pc.oniceconnectionstatechange=()=>rtcState(peer);
      pc.ondatachannel = guard(event => acceptInputChannel(pc,event.channel,'th03-inputs',rtc => attach(peer,rtc)));
      if (local < peer.id && message.peers?.includes(peer.id)) offer(peer);
    }
    if (message.route) selected(message.route,message.reason);
  }
  function offer(peer) {
    if (peer.rtc || !peer.pc || route) return;
    attach(peer,createInputChannel(peer.pc,'th03-inputs'));
    peer.chain = peer.chain.then(async () => {
      await peer.pc.setLocalDescription(await peer.pc.createOffer());
      control({type:'signal',to:peer.id,description:peer.pc.localDescription.toJSON()});
    }).catch(fallback);
  }
  for (let id = 0; id < count; id++) if (id !== local) {
    const peer = {id, candidates:[], chain:Promise.resolve(),get inputOpen(){return this.channel.readyState==='open';},get controlOpen(){return this.channel.readyState==='open';}};
    peer.channel = {
      label:'th03-inputs', readyState:'connecting',
      get bufferedAmount() { return route === 'rtc' ? peer.rtc?.bufferedAmount ?? 0 : data?.bufferedAmount ?? 0; },
      get transportStats() { return route === 'rtc' ? peer.rtc?.transportStats : {route:'relay'}; },
      send(text) {
        if (closed || this.readyState !== 'open') throw Error('数据通道未打开');
        const bytes = encoder.encode(text);
        if (bytes.length > 16382 || this.bufferedAmount > 262144) throw Error('同步发送积压');
        if (route === 'rtc') peer.rtc.send(text);
        else { const packet = new Uint8Array(bytes.length+2); packet.set([0xe7,id]); packet.set(bytes,2); data.send(packet); }
      },
      close() { if (this.readyState === 'closed') return; this.readyState='closed';clearRecovery(peer);peer.rtc?.close();peer.pc?.close(); },
    };
    peers.set(id,peer);
  }
  const status=environment.__th03PeerTransport={peers,disconnected:false,failed:false,error:'',get relay(){return data;},get rtcReadySent(){return rtcReported;}};
  function connectSignal(){
    if(closed)return;
    const signalUrl=new URL(base);signalUrl.searchParams.set('signal','1');
    const socket=signal=new environment.WebSocket(signalUrl);
    socket.onopen=guard(()=>{
      if(socket!==signal)return;
      for(const text of pendingSignals.splice(0))socket.send(text);
    });
    socket.onmessage=guard(event=>{
      if(socket!==signal)return;
      const message=JSON.parse(event.data);
      if(message.type==='peers'){
        if(route)return; // Existing RTC channels survive a signaling reconnect.
        return setupRtc(message).catch(fallback);
      }
      if(message.type==='peer-join'){const peer=peers.get(message.player);if(peer&&local<peer.id)offer(peer);return;}
      if(message.type==='route')return selected(message.mode,message.reason);
      if(message.type==='error')throw Error(message.error);
      const peer=peers.get(message.from);
      if(!peer?.pc||peer.channel.readyState==='closed'||route==='relay')return;
      if(message.type==='ice-restart-request'){
        if(local<peer.id)restart(peer);
        return;
      }
      if(message.type==='signal'){
        peer.chain=peer.chain.then(async()=>{
          if(closed||peer.channel.readyState==='closed'||route==='relay')return;
          if(message.description)await description(peer,message.description);
          if(message.candidate){
            if(peer.pc.remoteDescription)await addCandidate(peer.pc,message.candidate);
            else peer.candidates.push(message.candidate);
          }
        }).catch(error=>route==='rtc'?fatal(error):fallback());
      }
    });
    socket.onclose=()=>{
      if(closed||socket!==signal)return;
      signal=null;
      // Before a route is chosen, the data socket can still deliver the
      // opening barrier, so a signaling close is not fatal. After rtc or
      // relay is selected, reconnect signaling without touching gameplay.
      if(route)signalRetry=setTimeout(connectSignal,1000);
    };
  }
  return {
    channelFor(role) { const peer=peers.get(roles.indexOf(role)); if (!peer) throw Error('未知玩家'); return peer.channel; },
    connect() {
      if (data || closed) throw Error('重复连接');
      data = new environment.WebSocket(base); data.binaryType = 'arraybuffer';
      connectSignal();
      data.onmessage = guard(event => {
        if (typeof event.data === 'string') { const message=JSON.parse(event.data); if (message.type==='route') selected(message.mode,message.reason); else if(message.type==='error') throw Error(message.error); return; }
        if (route === 'rtc') return;
        const packet = new Uint8Array(event.data), peer = peers.get(packet[1]);
        // 0xe6 is added by the authenticated main relay, never by the sender.
        if (packet.length<3 || packet[0]!==0xe6 || !peer) throw Error('同步消息来源无效');
        deliver(peer,decoder.decode(packet.subarray(2)));
      });
      data.onclose=event=>{if(!closed&&route!=='rtc')fatal(Error(event?.reason||'本局网络连接已关闭'));};
      timeout=setTimeout(() => fatal(Error('等待主项目网络路由超时')),45000);
    }, close, finish,
  };
}
