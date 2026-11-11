// Deployment-owned settings. No permanent TURN secrets in this file.
// Fill either iceServers (STUN only), or a same-origin authenticated endpoint
// returning {iceServers, iceTransportPolicy} with short-lived TURN credentials.
export const udpConfig={
  credentialEndpoint:'',
  iceServers:[{urls:'stun:stun.cloudflare.com:3478'}],
  iceTransportPolicy:'all'
};

export async function deploymentIceConfiguration(session,signal){
  if(udpConfig.credentialEndpoint){
    const url=new URL(udpConfig.credentialEndpoint,location.href);
    if(url.origin!==location.origin)throw Error('UDP 凭据接口必须与网页同源，请使用反向代理');
    const response=await fetch(url,{method:'POST',headers:{'Content-Type':'application/json'},
      credentials:'same-origin',redirect:'error',signal,
      body:JSON.stringify({room:session.room,token:session.token,role:session.role})});
    if(!response.ok)throw Error(`UDP 凭据接口请求失败：HTTP ${response.status}`);
    return response.json();
  }
  if(!udpConfig.iceServers.length)throw Error('尚未配置公网 UDP：请负责人填写 netplay/udp-config.js 的 STUN 地址或凭据接口');
  return {iceServers:udpConfig.iceServers,iceTransportPolicy:udpConfig.iceTransportPolicy};
}
