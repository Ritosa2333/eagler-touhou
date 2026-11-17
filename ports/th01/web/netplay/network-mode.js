// Room-owned network mode. URL values only choose the initial entry settings.
export function readNetworkMode(search=location.search){
  const query=new URLSearchParams(search),value=query.get('network');
  const network=['public','public-udp','public-turn','public-ws','direct-ws'].includes(value)?'public':'lan';
  const transport=['ws','relay'].includes(query.get('transport'))||['public-ws','direct-ws'].includes(value)?'ws':'rtc';
  const ice=network==='public'&&transport==='rtc'&&(query.get('ice')==='relay'||value==='public-turn')?'relay':'all';
  return {network,transport,ice};
}
export function networkParams({network='lan',transport='rtc',ice='all'},room){
  const query=new URLSearchParams({network});
  if(transport==='ws')query.set('transport','ws');
  else if(network==='public'&&ice==='relay')query.set('ice','relay');
  if(room)query.set('room',room);
  return query;
}
export function describeNetwork({network='lan',transport='rtc',ice='all'}){
  if(transport==='ws')return `${network==='public'?'公网':'局域网'} · TCP 服务器转发`;
  return network==='public'?`公网 · ${ice==='relay'?'强制 TURN 中继':'UDP 自动打洞'}`:'局域网 · UDP 直连';
}
