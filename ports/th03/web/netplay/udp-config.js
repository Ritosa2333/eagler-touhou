// Deployment settings only. Never put a permanent TURN secret in web/.
export const udpConfig={credentialEndpoint:'/api/ice'};

export async function loadRtcConfiguration(session,signal){
  if(session.network!=='public')return {iceServers:[],iceTransportPolicy:'all'};
  const url=new URL(udpConfig.credentialEndpoint,location.href);
  if(url.origin!==location.origin)throw Error('TURN 凭据接口必须与网页同源');
  const response=await fetch(url,{method:'POST',headers:{'Content-Type':'application/json'},
    credentials:'same-origin',redirect:'error',cache:'no-store',signal,
    body:JSON.stringify({protocol:'th03-lan/1',room:session.room,token:session.token,role:session.role})});
  const config=await response.json();
  if(!response.ok)throw Error(config.error||`获取连接配置失败 (${response.status})`);
  if(!config||!Array.isArray(config.iceServers))throw Error('无效 ICE 配置');
  const iceServers=config.iceServers.map(server=>{
    const urls=Array.isArray(server?.urls)?server.urls:[server?.urls];
    if(!urls.length||urls.some(url=>typeof url!=='string'||!/^(stun|stuns|turn|turns):\S+$/i.test(url)))
      throw Error('ICE 地址必须使用 STUN 或 TURN 协议');
    const entry={urls};
    if(urls.some(url=>/^turns?:/i.test(url))){
      for(const key of ['username','credential']){
        if(typeof server[key]!=='string'||!server[key])throw Error('TURN 短期凭据缺失');
        entry[key]=server[key];
      }
    }
    return entry;
  });
  const iceTransportPolicy=session.ice==='relay'?'relay':config.iceTransportPolicy??'all';
  if(!['all','relay'].includes(iceTransportPolicy))throw Error('无效 ICE 策略');
  if(iceTransportPolicy==='relay'&&!hasTurn({iceServers}))throw Error('尚未配置 TURN，无法强制中继');
  return {iceServers,iceTransportPolicy};
}
export function hasTurn(config){
  return config.iceServers.some(server=>server.urls.some(url=>/^turns?:/i.test(url)));
}

// A gathered candidate is not proof of connectivity. Report the selected,
// successful pair only, with no IP addresses, SDP or credentials.
export function describeRtcPath(stats){
  let pair;
  for(const report of stats.values())if(report.type==='transport'&&report.selectedCandidatePairId){
    pair=stats.get(report.selectedCandidatePairId);if(pair)break;
  }
  if(!pair)pair=[...stats.values()].find(report=>report.type==='candidate-pair'&&report.state==='succeeded'&&(report.selected||report.nominated));
  if(!pair||pair.state!=='succeeded')return 'WebRTC 已连接 · 路径待确认';
  const local=stats.get(pair.localCandidateId),remote=stats.get(pair.remoteCandidateId);
  const types=[local?.candidateType,remote?.candidateType];
  const kind=types.includes('relay')?'TURN 中继':types.every(type=>['host','srflx','prflx'].includes(type))?'设备直连':'路径类型未知';
  const transport=local?.protocol||remote?.protocol||'未知协议';
  const relay=local?.relayProtocol||remote?.relayProtocol;
  const rtt=Number.isFinite(pair.currentRoundTripTime)?` · RTT ${Math.round(pair.currentRoundTripTime*1000)} ms`:'';
  return `${kind} · ${transport.toUpperCase()}${relay?`（TURN ${relay.toUpperCase()}）`:''}${rtt}`;
}
