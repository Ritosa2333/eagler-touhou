export async function verifyPc98IceRestart(game,live,playerFrame,wait){
  const before=await Promise.all(live.map(f=>f.evaluate(playerFrame)));
  for(const f of live)await f.evaluate(game=>{
    const peer=[...globalThis['__'+game+'PeerTransport'].peers.values()][0];
    const ufrag=sdp=>/^a=ice-ufrag:(\S+)/m.exec(sdp??'')?.[1];
    globalThis.qaRtc={peer,pc:peer.pc,channel:peer.channel,ufrag,remote:ufrag(peer.pc.remoteDescription?.sdp)};
    if(!globalThis.qaRtc.remote||globalThis.__eaglerNetplayTransport!=='rtc')throw Error('RTC restart requires a live RTC pair');
  },game);
  // Drive the browser's normal recovery path from the higher seat; the
  // real relay forwards the request and the lower seat negotiates new ICE.
  await live[1].evaluate(()=>{
    const {pc}=globalThis.qaRtc;
    Object.defineProperty(pc,'connectionState',{value:'failed',configurable:true});
    try{pc.onconnectionstatechange();}finally{delete pc.connectionState;}
  });
  await wait(async()=>(await Promise.all(live.map(f=>f.evaluate(()=>{
    const {peer,pc,channel,ufrag,remote}=globalThis.qaRtc;
    return peer.pc===pc&&peer.channel===channel&&channel.readyState==='open'&&
      ufrag(pc.remoteDescription?.sdp)!==remote&&['connected','completed'].includes(pc.iceConnectionState);
  })))).every(Boolean),'RTC restart on the same gameplay channel',10000);
  await wait(async()=>(await Promise.all(live.map(f=>f.evaluate(playerFrame)))).every((frame,seat)=>frame>=before[seat]+120),
    'both players confirm gameplay after ICE restart',15000);
  console.log('PASS '+game+' ICE restart kept both gameplay channels');
}
