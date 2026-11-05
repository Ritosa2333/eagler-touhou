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
  let earlyBytes = 0, timeout;
  let endTask,endResolve,endReject,endTimer,endGrace,localEnded=false;
  const endSeen=new Set(),endAck=new Set();
  const peers = new Map(), early = [], encoder = new TextEncoder(), decoder = new TextDecoder('utf-8', {fatal:true});
  const fatal = error => { if (!closed) {
    if(localEnded&&endSeen.size===peers.size&&endAck.size===peers.size){clearTimeout(endTimer);clearTimeout(endGrace);endReject=null;endResolve?.();return;}
    status.failed=true;status.error=String(error.message||error);close(); fail(error instanceof Error ? error : Error(String(error)));
  } };
  const guard = fn => (...args) => { if (!closed) try { const result = fn(...args); result?.catch?.(fatal); } catch (error) { fatal(error); } };
  const control = message => { if (!closed && signal?.readyState === 1) signal.send(JSON.stringify(message)); };
  function close() {
    if (closed) return; closed = true;status.disconnected=true;clearTimeout(timeout);
    clearTimeout(endTimer);clearTimeout(endGrace);endReject?.(Error('本局网络在结束确认前关闭'));endReject=null;
    for (const peer of peers.values()) {
      peer.channel.readyState = 'closed'; peer.channel.onclose?.();
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
  function selected(mode) {
    if (!['rtc','relay'].includes(mode) || (route && route !== mode)) throw Error('无效网络路由');
    if (route) return;
    if (mode === 'rtc' && [...peers.values()].some(peer => peer.rtc?.readyState !== 'open')) throw Error('直连通道未就绪');
    route = mode; clearTimeout(timeout);
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
    rtc.onmessage = guard(event => { if (!route || route === 'rtc') deliver(peer,event.data); });
    rtc.onerror = () => route === 'rtc' ? fatal(Error('直连通道失败')) : fallback();
    rtc.onclose = () => { if (route === 'rtc') fatal(Error('直连通道已关闭')); else if (!route) fallback(); };
  }
  async function description(peer, value) {
    await peer.pc.setRemoteDescription(value);
    for (const candidate of peer.candidates.splice(0)) await peer.pc.addIceCandidate(candidate);
    if (value.type === 'offer') {
      await peer.pc.setLocalDescription(await peer.pc.createAnswer());
      control({type:'signal',to:peer.id,description:peer.pc.localDescription.toJSON()});
    }
  }
  async function setupRtc(message) {
    if (!environment.RTCPeerConnection) { fallback(); return; }
    const iceServers = Array.isArray(message.iceServers) ? message.iceServers : [];
    for (const peer of peers.values()) {
      const pc = peer.pc = new environment.RTCPeerConnection({iceServers, ...(options.netplayPreferTurn ? {iceTransportPolicy:'relay'} : {})});
      pc.onicecandidate = guard(event => { if (event.candidate) control({type:'signal',to:peer.id,candidate:event.candidate.toJSON()}); });
      pc.onconnectionstatechange = () => { if (['failed','closed'].includes(pc.connectionState)) { if (route === 'rtc') fatal(Error('玩家直连断开')); else if (!route) fallback(); } };
      pc.ondatachannel = guard(event => acceptInputChannel(pc,event.channel,'th04-inputs',rtc => attach(peer,rtc)));
      if (local < peer.id && message.peers?.includes(peer.id)) offer(peer);
    }
    if (message.route) selected(message.route);
  }
  function offer(peer) {
    if (peer.rtc || !peer.pc || route) return;
    attach(peer,createInputChannel(peer.pc,'th04-inputs'));
    peer.chain = peer.chain.then(async () => {
      await peer.pc.setLocalDescription(await peer.pc.createOffer());
      control({type:'signal',to:peer.id,description:peer.pc.localDescription.toJSON()});
    }).catch(fallback);
  }
  for (let id = 0; id < count; id++) if (id !== local) {
    const peer = {id, candidates:[], chain:Promise.resolve(),get inputOpen(){return this.channel.readyState==='open';},get controlOpen(){return this.channel.readyState==='open';}};
    peer.channel = {
      label:'th04-inputs', readyState:'connecting',
      get bufferedAmount() { return route === 'rtc' ? peer.rtc?.bufferedAmount ?? 0 : data?.bufferedAmount ?? 0; },
      get transportStats() { return route === 'rtc' ? peer.rtc?.transportStats : {route:'relay'}; },
      send(text) {
        if (closed || this.readyState !== 'open') throw Error('数据通道未打开');
        const bytes = encoder.encode(text);
        if (bytes.length > 16382 || this.bufferedAmount > 262144) throw Error('同步发送积压');
        if (route === 'rtc') peer.rtc.send(text);
        else { const packet = new Uint8Array(bytes.length+2); packet.set([0xe7,id]); packet.set(bytes,2); data.send(packet); }
      },
      close() { if (this.readyState === 'closed') return; this.readyState='closed'; peer.rtc?.close(); peer.pc?.close(); },
    };
    peers.set(id,peer);
  }
  const status=environment.__th04PeerTransport={peers,disconnected:false,failed:false,error:'',get relay(){return data;},get rtcReadySent(){return rtcReported;}};
  return {
    channelFor(role) { const peer=peers.get(roles.indexOf(role)); if (!peer) throw Error('未知玩家'); return peer.channel; },
    connect() {
      if (data || closed) throw Error('重复连接');
      data = new environment.WebSocket(base); data.binaryType = 'arraybuffer';
      const signalUrl = new URL(base); signalUrl.searchParams.set('signal','1');
      signal = new environment.WebSocket(signalUrl);
      data.onmessage = guard(event => {
        if (typeof event.data === 'string') { const message=JSON.parse(event.data); if (message.type==='route') selected(message.mode); else if(message.type==='error') throw Error(message.error); return; }
        if (route === 'rtc') return;
        const packet = new Uint8Array(event.data), peer = peers.get(packet[1]);
        // 0xe6 is added by the authenticated main relay, never by the sender.
        if (packet.length<3 || packet[0]!==0xe6 || !peer) throw Error('同步消息来源无效');
        deliver(peer,decoder.decode(packet.subarray(2)));
      });
      signal.onmessage = guard(event => {
        const message = JSON.parse(event.data);
        if (message.type === 'peers') return setupRtc(message).catch(fallback);
        if (message.type === 'peer-join') { const peer=peers.get(message.player); if (peer && local<peer.id) offer(peer); return; }
        if (message.type === 'route') return selected(message.mode);
        if (message.type === 'error') throw Error(message.error);
        if (message.type === 'signal') {
          const peer = peers.get(message.from); if (!peer || !peer.pc || route === 'relay') return;
          peer.chain = peer.chain.then(async () => {
            if (closed || route === 'relay') return;
            if (message.description) await description(peer,message.description);
            if (message.candidate) { if (peer.pc.remoteDescription) await peer.pc.addIceCandidate(message.candidate); else peer.candidates.push(message.candidate); }
          }).catch(fallback);
        }
      });
      for (const socket of [data,signal]) { socket.onerror=() => { if (!route) fallback(); }; socket.onclose=() => { if (!closed && (route !== 'rtc' || socket === signal)) fatal(Error('本局网络连接已关闭')); }; }
      timeout=setTimeout(() => fatal(Error('等待主项目网络路由超时')),45000);
    }, close, finish,
  };
}
