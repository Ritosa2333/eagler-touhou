// LAN defaults to host candidates; public rooms fetch authenticated STUN/TURN.
// Expose the same HTTPS room/signaling service to every player.
// Never put permanent TURN credentials in browser assets.
export const udpConfig={iceServers:[],iceTransportPolicy:'all',credentialEndpoint:''};
export async function deploymentIceConfiguration(session,signal){
  if(udpConfig.credentialEndpoint){
    const url=new URL(udpConfig.credentialEndpoint,location.href);
    if(url.origin!==location.origin)throw Error('TURN 凭据接口必须同源');
    const response=await fetch(url,{method:'POST',credentials:'same-origin',redirect:'error',signal,
      headers:{'Content-Type':'application/json'},body:JSON.stringify(session)});
    const value=await response.json();
    if(!response.ok)throw Error(value.error||'获取 ICE 凭据失败');
    return value;
  }
  if(session.network==='public'){
    const response=await fetch('/api/ice',{method:'POST',headers:{'Content-Type':'application/json'},
      credentials:'same-origin',redirect:'error',cache:'no-store',signal,
      body:JSON.stringify({protocol:'th01-native-lockstep/1',room:session.room,token:session.token,role:session.role})});
    const value=await response.json();
    if(!response.ok)throw Error(value.error||`获取公网打洞配置失败 (${response.status})`);
    return value;
  }
  if(udpConfig.iceServers.length)return {iceServers:udpConfig.iceServers,iceTransportPolicy:udpConfig.iceTransportPolicy};
  // Server --stun configuration is shared by all players.
  const response=await fetch('/api/ice',{cache:'no-store',signal});
  if(!response.ok)throw Error('获取 UDP 配置失败，请检查局域网服务');
  return response.json();
}
