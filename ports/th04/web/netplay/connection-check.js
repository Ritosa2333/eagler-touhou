// Manual, two-device probe. No emulator, game loop or rollback modules imported.
// Uses the existing lobby API, but a distinct DataChannel label/wire handshake.
import {countIceCandidate,describeIceCandidates,describeIceAddresses,describeRtcConfiguration,recordIceServerError,refreshRtcPath} from './connection.js';
const PROTOCOL='th04-rollback/4',LABEL='th04-connection-check-v1';
const $=id=>document.getElementById(id);
let session,peer,channel,busy=false,stopped=false,pollTimer,deadline,heartbeat;
let localSlot;
let rtcConfig,mode;
let lobbyState=null,stage='检测程序已加载，请创建或加入检测房间',pollCount=0,lastPoll=0,lobbyWaitAt=0;
function progress(message){stage=message;$('status').textContent=message;if(!peer)render();}
let signalChain=Promise.resolve(),beganAt=0,pings=new Map(),sent=0,received=0,rtt=null,lastPong=0;
const candidates=[];
const other=()=>session.role==='host'?'guest':'host';
function render(){
  if(!peer){
    const seat=slot=>Number.isInteger(slot)?`P${slot+1}`:'未选座';
    const row=role=>`${role==='host'?'房主':'客机'}：${lobbyState.present[role]?'已加入':'未加入'} / ${seat(lobbyState.slots[role])} / ${lobbyState.ready[role]?'已准备':'未准备'}`;
    $('diagnostics').textContent=`诊断版 3 · ${stage}\n`
      +`本机：${session?`${session.role==='host'?'房主':'客机'} / ${seat(localSlot)}`:'尚未加入房间'}\n`
      +(lobbyState?`房间阶段：${lobbyState.phase}\n${row('host')}\n${row('guest')}\n`:'')
      +`房间查询成功 ${pollCount} 次${lastPoll?' / 最近 '+new Date(lastPoll).toLocaleTimeString():''}\n`
      +'数据通道检测尚未启动；以上是房间流程状态，不是 ICE 失败。';
    return;
  }
  const pc=peer.pc;
  $('diagnostics').textContent=`诊断版 3 · 身份：${session.role==='host'?'房主／连接发起端':'客机／连接应答端'} · 座位 P${localSlot+1}\n浏览器：${navigator.userAgent}\n`
    +`${describeRtcConfiguration(rtcConfig)} · 已用 ${Math.floor((performance.now()-beganAt)/1000)} 秒\n`
    +`连接 ${pc.connectionState} / ICE ${pc.iceConnectionState} / 协商 ${pc.signalingState} / 候选收集 ${pc.iceGatheringState}\n`
    +`数据通道 ${channel?.readyState||'未建立'} · ${peer.path||'路径待确认'}\n`
    +`本机：${describeIceCandidates(peer.localCandidateTypes)}\n收到：${describeIceCandidates(peer.remoteCandidateTypes)}\n`
    +`本机地址类别：${describeIceAddresses(peer.localCandidateTypes)}\n收到地址类别：${describeIceAddresses(peer.remoteCandidateTypes)}\n`
    +`候选提交 ${peer.applied||0} · 服务错误：${peer.iceServerErrors.join('；')||'未报告'}\n`
    +`${peer.iceDiagnostics||'ICE 探测统计尚未获取'}\n`
    +`数据通道发送探测 ${sent} / 收到回包 ${received} / 最近往返 ${rtt===null?'—':rtt.toFixed(1)+' ms'}\n`
    +`候选提示：${peer.warning||'无'}`;
}
function leave(){
  if(session)navigator.sendBeacon('/api/leave',new Blob([JSON.stringify(session)],{type:'application/json'}));
}
function stop(reason){
  if(stopped)return;
  render();stopped=true;
  clearTimeout(pollTimer);clearTimeout(deadline);clearInterval(heartbeat);
  channel?.close();peer?.pc.close();leave();
  $('status').textContent=reason+'。诊断保留停止前状态；点击“结束并重新检测”重试。';
  if(!peer)$('diagnostics').textContent+='\n'+$('status').textContent;
}
async function request(action,data={}){
  const response=await fetch(`/api/${action}`,{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({protocol:PROTOCOL,...session,...data}),signal:AbortSignal.timeout(10000)});
  const value=await response.json();if(!response.ok)throw Error(value.error||`HTTP ${response.status}`);return value;
}
const signal=message=>request('signal',{to:other(),message});
function fail(error){stop(`检测停止：${error.message||error}`);}
function attach(dc){
  if(channel||dc.label!==LABEL)throw Error('请确认双方都在连接检测页面，且未加入游戏房间');
  channel=dc;
  dc.onopen=()=>{if(stopped){dc.close();return;}$('status').textContent='通道已打开，正在确认双向消息…';render();};
  dc.onmessage=event=>{
    if(stopped)return;
    try{
      if(typeof event.data!=='string'||event.data.length>1024)throw Error('检测消息格式不符');
      const message=JSON.parse(event.data);
      if(message.protocol!==LABEL||!Number.isSafeInteger(message.id)||message.id<1)throw Error('请确认双方使用同版连接检测页');
      if(message.type==='ping')dc.send(JSON.stringify({protocol:LABEL,type:'pong',id:message.id}));
      else if(message.type==='pong'){
        const at=pings.get(message.id);if(at===undefined)return;
        rtt=performance.now()-at;pings.delete(message.id);received++;lastPong=performance.now();clearTimeout(deadline);
        $('status').textContent='双向数据收发成功。请保持两端前台约 30 秒，观察回包数是否持续增加，再对比游戏联机。';
      }else throw Error('未知检测消息');
      render();
    }catch(error){fail(error);}
  };
  dc.onerror=()=>{if(!stopped)stop('数据通道报告错误');};
  dc.onclose=()=>{if(!stopped)stop('数据通道已关闭');};
}
function submit(candidate){
  Promise.resolve().then(()=>peer.pc.addIceCandidate(candidate)).then(()=>{peer.applied=(peer.applied||0)+1;})
    .catch(()=>{peer.warning='有候选提交失败';}).finally(()=>{if(!stopped)render();});
}
async function description(message){
  const pc=peer.pc;
  if(message.mode!==mode)throw Error('双方检测模式或页面版本不同，请刷新并选择相同模式后重建房间');
  if(!['offer','answer'].includes(message.type)||message.description?.type!==message.type)throw Error('无效检测信令');
  if((message.type==='offer')!==(session.role==='guest')||pc.remoteDescription)throw Error('重复或不匹配的连接描述');
  await pc.setRemoteDescription(message.description);
  if(stopped)return;
  for(const candidate of candidates.splice(0))submit(candidate);
  if(message.type==='offer'){
    await pc.setLocalDescription(await pc.createAnswer());
    if(!stopped)await signal({type:'answer',mode,description:pc.localDescription.toJSON()});
  }
}
function dispatch(event){
  if(event?.from!==other()||!event.message)throw Error('未知检测信令来源');
  const message=event.message;
  if(message.type==='ice'){
    countIceCandidate(peer.remoteCandidateTypes,message.candidate);
    if(peer.pc.remoteDescription)submit(message.candidate);else candidates.push(message.candidate);
  }else signalChain=signalChain.then(()=>{if(!stopped)return description(message);}).catch(fail);
}
function begin(){
  beganAt=performance.now();
  const pc=new RTCPeerConnection(rtcConfig);
  peer={pc,localCandidateTypes:{},remoteCandidateTypes:{},iceServerErrors:[]};
  $('status').textContent='正在建立连接；此过程不加载 NP2 或游戏。';
  pc.onicecandidate=event=>{
    if(!event.candidate||stopped)return;
    countIceCandidate(peer.localCandidateTypes,event.candidate);
    signal({type:'ice',candidate:event.candidate.toJSON()}).catch(fail);render();
  };
  pc.onicecandidateerror=event=>{if(!stopped){recordIceServerError(peer.iceServerErrors,event);render();}};
  pc.onconnectionstatechange=pc.oniceconnectionstatechange=pc.onicegatheringstatechange=()=>{if(!stopped)render();};
  pc.ondatachannel=event=>{if(stopped){event.channel.close();return;}try{attach(event.channel);}catch(error){fail(error);}};
  deadline=setTimeout(()=>stop('30 秒内未完成双向数据收发'),30000);
  if(session.role==='host'){
    attach(pc.createDataChannel(LABEL,{ordered:true}));
    signalChain=Promise.resolve().then(async()=>{
      await pc.setLocalDescription(await pc.createOffer());
      if(!stopped)await signal({type:'offer',mode,description:pc.localDescription.toJSON()});
    }).catch(fail);
  }
  heartbeat=setInterval(()=>{
    if(stopped)return;
    try{
      void refreshRtcPath(peer).then(()=>{if(!stopped)render();});
      if(received&&performance.now()-lastPong>10000){stop('已连通后超过 10 秒没有回包');return;}
      if(channel?.readyState==='open'){
        const id=++sent;pings.set(id,performance.now());
        if(pings.size>30)pings.delete(pings.keys().next().value);
        channel.send(JSON.stringify({protocol:LABEL,type:'ping',id}));
      }
      render();
    }catch(error){fail(error);}
  },1000);
  render();
}
async function poll(){
  if(stopped)return;
  try{
    const q=new URLSearchParams({room:session.room,token:session.token});
    const response=await fetch(`/api/events?${q}`,{signal:AbortSignal.timeout(10000)});
    const result=await response.json();if(!response.ok)throw Error(result.error||`HTTP ${response.status}`);
    if(stopped)return;
    let state=result.state;
    lobbyState=state;pollCount++;lastPoll=Date.now();
    if(state.protocol!==PROTOCOL||state.settings.players!==2)throw Error('请使用双人检测房间');
    if(state.phase==='lobby'){
      progress('房间已响应，正在检查双方座位与准备状态');
      // Taking a seat resets everyone's ready flag. Re-read via the normal
      // room API rather than assuming the host's earlier readiness survived.
      if(!state.ready[session.role]){progress('正在提交本机准备');state=(await request('ready')).state;lobbyState=state;}
      if(session.role==='host'&&state.present.guest&&state.ready.host&&state.ready.guest){progress('双方已准备，房主正在请求开始检测');state=(await request('start')).state;lobbyState=state;}
      if(state.phase==='lobby'){
        progress(!state.present.guest?'等待另一台设备加入此检测房间':!state.ready.host?'等待房主重新准备；请保持电脑检测页在前台':!state.ready.guest?'等待客机选座并准备':'双方已准备，等待房主开始；请保持房主检测页在前台');
        if(state.present.host&&state.present.guest){
          lobbyWaitAt||=performance.now();
          if(performance.now()-lobbyWaitAt>30000)throw Error('双方加入后，房间准备／开始流程超过 30 秒；尚未进入 ICE 建连，请保留两端诊断');
        }else lobbyWaitAt=0;
      }
    }
    if(stopped)return;
    if(state.phase==='loading'&&!peer)begin();
    for(const event of result.events){if(stopped)break;dispatch(event);}
    if(!stopped)pollTimer=setTimeout(poll,400);
  }catch(error){if(!stopped)fail(Error(`房间／信令检测：${error.message||error}`));}
}
async function enter(action){
  if(busy||session||stopped)return;
  busy=true;$('create').disabled=$('join').disabled=$('seat').disabled=$('mode').disabled=true;
  mode=$('mode').value;
  rtcConfig={iceServers:mode==='stun'?[{urls:'stun:stun.cloudflare.com:3478'}]:[],iceTransportPolicy:'all'};
  progress(action==='create'?'正在创建检测房间':'正在提交加入请求');
  try{
    if(typeof RTCPeerConnection!=='function')throw Error('此浏览器不支持 WebRTC');
    const room=$('room').value.trim().toUpperCase();
    if(action==='join'&&!/^[0-9]{4}$/.test(room))throw Error('请输入建房端显示的四位数字检测房间号');
    const result=await request(action,action==='join'?{room}:{});
    session={room:result.room,token:result.token,role:result.role};
    lobbyState=result.state;
    if(stopped){leave();return;}
    if(!['host','guest'].includes(session.role))throw Error('检测仅支持两台设备');
    localSlot=$('seat').value==='auto'?(session.role==='host'?0:1):Number($('seat').value);
    $('room-info').textContent=`检测房间 ${session.room} · P${localSlot+1} · ${session.role==='host'?'本机是房主／连接发起端，请另一端在本页加入':'本机是客机／连接应答端，等待自动检测'}`;
    progress(`已加入，正在申请 P${localSlot+1} 座位`);
    lobbyState=(await request('seat',{slot:localSlot})).state;
    progress('座位已确认，正在查询房间状态');
    void poll();
  }catch(error){fail(error);}finally{busy=false;}
}
function pageWarning(message){
  const panel=$('page-warning');panel.hidden=false;
  panel.textContent='页面脚本异常（独立记录，不代表网络失败）：\n'+String(message).slice(0,1800);
}
window.addEventListener('error',event=>pageWarning(event.error?.stack||event.message));
window.addEventListener('unhandledrejection',event=>pageWarning(event.reason?.stack||event.reason));
window.addEventListener('pagehide',()=>{stopped=true;clearTimeout(pollTimer);clearTimeout(deadline);clearInterval(heartbeat);channel?.close();peer?.pc.close();leave();});
$('create').onclick=()=>enter('create');$('join').onclick=()=>enter('join');
$('reset').onclick=()=>location.reload();
progress('检测程序已加载（诊断版 3）。请创建或加入检测房间');
$('create').disabled=$('join').disabled=false;
