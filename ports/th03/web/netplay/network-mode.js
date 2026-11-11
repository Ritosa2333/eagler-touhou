// Keep TH04 public links and the existing TH03 transport=relay links usable.
export function readNetworkMode(search=location.search){
  const params=new URLSearchParams(search),legacy=params.get('network')||'lan';
  return {
    network:legacy==='public'||legacy.startsWith('public-')||legacy==='direct-ws'?'public':'lan',
    mode:['ws','relay','tcp'].includes(params.get('transport'))||['public-ws','direct-ws'].includes(legacy)?'relay':'rtc',
    ice:params.get('ice')==='relay'||legacy==='public-turn'?'relay':'all'
  };
}
export function networkParams({network='lan',mode='rtc',ice='all'}){
  return new URLSearchParams({network,transport:mode==='relay'?'ws':'rtc',ice});
}
export function describeNetwork({network='lan',mode='rtc',ice='all'}){
  if(mode==='relay')return 'TCP 服务器转发（WebSocket）';
  if(network!=='public')return '局域网设备直连';
  return ice==='relay'?'公网 TURN 中继':'公网 UDP 打洞 + TURN';
}
