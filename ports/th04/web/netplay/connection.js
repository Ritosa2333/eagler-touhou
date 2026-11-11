// Connection facilities only. No frame/input/snapshot or membership policy here.
// A hosting application may set th04NetplayUdp before starting a room.
// th04NetplayConnection remains accepted as a compatibility alias.
export const UDP_HOLE_PUNCH_API='th04-udp-hole-punch/1';
export async function loadRtcConfiguration(session,source=globalThis.th04NetplayUdp??globalThis.th04NetplayConnection){
  if(source===undefined)return {iceServers:[],iceTransportPolicy:'all'};
  const controller=new AbortController();
  let timer;
  try{
    const config=await Promise.race([
      Promise.resolve().then(()=>typeof source==='function'?source({...session},controller.signal):source),
      new Promise((_,reject)=>{timer=setTimeout(()=>{
        reject(Error('获取连接配置超过 10 秒，请检查配置服务'));
        controller.abort();
      },10000);})
    ]);
    if(!config||typeof config!=='object'||Array.isArray(config))throw Error('连接配置必须是对象');
    if(Object.keys(config).some(key=>!['iceServers','iceTransportPolicy'].includes(key)))throw Error('连接配置仅支持 iceServers 和 iceTransportPolicy');
    const policy=config.iceTransportPolicy??'all';
    if(!['all','relay'].includes(policy))throw Error('无效 ICE 传输策略');
    const servers=config.iceServers??[];
    if(!Array.isArray(servers))throw Error('iceServers 必须是数组');
    const iceServers=servers.map(server=>{
      if(!server||typeof server!=='object')throw Error('无效 ICE 服务配置');
      const urls=Array.isArray(server.urls)?server.urls.slice():[server.urls];
      if(!urls.length||urls.some(url=>typeof url!=='string'||! /^(stun|stuns|turn|turns):\S+$/i.test(url)))throw Error('ICE 服务地址必须使用 STUN 或 TURN 协议');
      const value={urls};
      for(const key of ['username','credential'])if(server[key]!==undefined){
        if(typeof server[key]!=='string')throw Error('ICE 凭据必须是字符串');
        value[key]=server[key];
      }
      return value;
    });
    if(policy==='relay'&&!iceServers.some(server=>server.urls.some(url=>/^turns?:/i.test(url))))throw Error('强制中继需要配置 TURN 服务');
    return {iceServers,iceTransportPolicy:policy};
  }finally{clearTimeout(timer);}
}

export function describeRtcConfiguration(config){
  const urls=config.iceServers.flatMap(server=>Array.isArray(server.urls)?server.urls:[server.urls]);
  const stun=urls.filter(url=>/^stuns?:/i.test(url)).length;
  const turn=urls.filter(url=>/^turns?:/i.test(url)).length;
  return `${stun||turn?`STUN 地址 ${stun} / TURN 地址 ${turn}`:'未配置 STUN/TURN（局域网默认）'} · 策略 ${config.iceTransportPolicy}`;
}

// Count trickled candidate types, not addresses. JSON candidates received via
// signaling do not necessarily include the browser's .type convenience field.
export function countIceCandidate(counts,candidate){
  const parsed=typeof candidate?.candidate==='string'?candidate.candidate.match(/\btyp\s+(host|srflx|prflx|relay)\b/i)?.[1]?.toLowerCase():undefined;
  const type=['host','srflx','prflx','relay'].includes(candidate?.type)?candidate.type:parsed||'unknown';
  counts[type]=(counts[type]||0)+1;
  const address=candidate?.address||candidate?.candidate?.trim().split(/\s+/)[4]||'';
  const category=/\.local\.?$/i.test(address)?'mdns':/^\d{1,3}(\.\d{1,3}){3}$/.test(address)?'ipv4':address.includes(':')?'ipv6':'unreported';
  counts[category]=(counts[category]||0)+1;
}

export function describeIceAddresses(counts){
  return `mDNS ${counts.mdns||0} / IPv4 ${counts.ipv4||0} / IPv6 ${counts.ipv6||0} / 未识别 ${counts.unreported||0}`;
}

export function describeIceCandidates(counts){
  return `host ${counts.host||0} / srflx ${counts.srflx||0} / relay ${counts.relay||0} / prflx ${counts.prflx||0}${counts.unknown?` / 未知 ${counts.unknown}`:''}`;
}

export function recordIceServerError(errors,event){
  // Browser errorText/url can contain network addresses. Keep only service
  // kind and numeric code; do not log URLs, SDP or credentials.
  const service=typeof event.url==='string'?event.url.match(/^(stun|stuns|turn|turns):/i)?.[1]?.toUpperCase():undefined;
  const code=Number.isInteger(event.errorCode)?event.errorCode:'未知';
  const entry=`${service||'ICE'} 错误 ${code}`;
  if(errors.includes(entry))return;
  if(errors.length>=5)errors.shift();
  errors.push(entry);
}

// Only describe a selected pair; gathered candidates do not prove connectivity.
// Do not expose addresses, SDP, room tokens or TURN credentials in diagnostics.
export function describeRtcPath(stats){
  let pair;
  for(const report of stats.values())if(report.type==='transport'&&report.selectedCandidatePairId){
    pair=stats.get(report.selectedCandidatePairId);if(pair)break;
  }
  if(!pair){
    const selected=[...stats.values()].filter(report=>report.type==='candidate-pair'&&report.state==='succeeded'&&(report.selected||report.nominated));
    if(selected.length===1)pair=selected[0];
  }
  if(!pair)return '路径待确认';
  const local=stats.get(pair.localCandidateId),remote=stats.get(pair.remoteCandidateId);
  const types=[local?.candidateType,remote?.candidateType];
  const kind=types.includes('relay')?'TURN 中继':types.every(type=>['host','srflx','prflx'].includes(type))?'直连':'路径类型未知';
  const protocol=local?.protocol||remote?.protocol||'未知传输';
  const relayProtocol=local?.relayProtocol||remote?.relayProtocol;
  return `${kind}（${types.map(type=>type||'?').join(' / ')}；${protocol}${relayProtocol?`；TURN ${relayProtocol}`:''}）`;
}

export async function refreshRtcPath(peer){
  if(peer.pathPending||peer.offline||peer.pc.connectionState==='closed')return;
  peer.pathPending=true;
  try{
    const stats=await peer.pc.getStats();
    peer.path=describeRtcPath(stats);
    peer.iceDiagnostics=describeIceChecks(stats);
  }
  catch{peer.path='路径统计暂不可用';peer.iceDiagnostics='ICE 探测统计暂不可用';}
  finally{peer.pathPending=false;}
}

// getStats may omit failed/unresolved pairs or individual counters. A missing
// report is not evidence that a packet was never sent or mDNS resolution failed.
export function describeIceChecks(stats){
  const reports=[...stats.values()],pairs=reports.filter(r=>r.type==='candidate-pair');
  const states=['frozen','waiting','in-progress','failed','succeeded'];
  const summary=states.map(state=>`${state} ${pairs.filter(p=>p.state===state).length}`).join(' / ');
  const counter=key=>{
    const values=pairs.map(pair=>pair[key]).filter(Number.isFinite);
    return values.length?`${values.reduce((a,b)=>a+b,0)}${values.length<pairs.length?'（部分）':''}`:'未提供';
  };
  const dtls=reports.filter(r=>r.type==='transport').map(r=>r.dtlsState).filter(Boolean);
  return `候选对 ${pairs.length}${pairs.length?'：'+summary:'（浏览器未提供配对记录）'}\n`
    +`ICE 探测请求 发 ${counter('requestsSent')} / 收 ${counter('requestsReceived')}；应答 发 ${counter('responsesSent')} / 收 ${counter('responsesReceived')}\n`
    +`DTLS：${dtls.length?[...new Set(dtls)].join(' / '):'未提供'}（以上为浏览器公开统计，未提供不等于零）`;
}
