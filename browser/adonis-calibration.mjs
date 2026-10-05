// Frontend mirror only. Native peers own measurement, choice and frame zero.
export function createAdonisCalibration({core,getApp,getOptions,emit,game,build,onError,pause,target=globalThis}) {
 let stopped=false,busy=false,timer=null,route='',key=-1,reported=false,lastNotice=0;
 const state=()=>{
  const app=getApp();if(!app||typeof core.multiplayer_calibration_status!=='function')return null;
  return Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_calibration_status(app),32));
 };
 const stop=()=>{stopped=true;clearTimeout(timer);const transport=target.__eaglerPeerTransport;if(transport?.onReceive===pump)transport.onReceive=null;};
 const pump=()=>{
  if(stopped||busy||!getApp()||!getOptions().netplayAdonisMode||getOptions().netplaySpectator)return;
  busy=true;clearTimeout(timer);
  try{
   let s=state();if(!s||!s[1])return;
   const transport=target.__eaglerPeerTransport;
   if(s[11]!==key){key=s[11];reported=false;route='';}
   if(s[1]>1&&s[1]<5){
    if(target.document?.hidden)throw Error('测量期间页面隐藏，请回房间重新开始');
    const actual=transport?.route;
    if(actual&&actual!=='connecting'){if(route&&route!==actual)throw Error('测量期间游戏链路改变，请回房间重新开始');route=actual;}
   }
   // Gameplay's normal pump owns the wire after the committed handoff.
   // This frontend timer only measures and publishes the frozen result.
   if(s[1]!==5){
    if(!core.multiplayer_network_poll(getApp()))throw Error(transport?.error||'实际游戏输入通道测量或网络连接失败');
    s=state();if(!s)return;
   }
   if(s[1]===6)throw Error('实际游戏输入通道测量失败，请回房间重新开始');
   const players=Array.from({length:s[4]},(_,player)=>{
    const at=12+player*6;return {player,p95Us:s[at],samples:s[at+1],lost:s[at+2],minUs:s[at+3],maxUs:s[at+4],meanUs:s[at+5]};
   });
   const now=performance.now();
   if(s[1]===5&&!reported){
    reported=true;const o=getOptions(),timing={phase:'ready',automatic:!!s[7],adonisMode:s[6],
     inputDelay:s[8],fullDelay:s[9],predictionReserve:s[10],rttP95Us:Math.max(...players.map(p=>p.p95Us)),
     samples:Math.min(...players.map(p=>p.samples)),lost:players.reduce((n,p)=>n+p.lost,0),route:route||transport?.route,
     calibration:{game:game+'mp',localPlayer:s[5],completedAt:new Date().toISOString(),build,
      method:'adonis2-129-probes-16ms-tail200ms',stabilizeMs:1000,intervalMs:16,tailWaitMs:200,probes:129,windowStart:10,windowEnd:129,
      scope:s[4]===3?'worst outgoing input link per participant':'input link per participant',players}};
    if(o.netplaySpectator)timing.route='spectator';
    target.__eaglerNetplayTiming=timing;target.__eaglerNetplayInputDelayFrames=s[8];
    target.__eaglerNetplayAdonisMode=s[6];emit('runtime-info',{netplayTiming:timing});
    stop();
   }else if(s[1]<5&&now-lastNotice>=100){
    lastNotice=now;emit('runtime-info',{netplayTiming:{phase:s[1]<=2?'waiting':s[1]===3?(s[2]?'measuring':'stabilizing'):'negotiating',
     automatic:!!s[7],adonisMode:s[6],probes:s[2],replies:s[3],route,players}});
   }
  }catch(e){stopped=true;pause();onError(e);}
  finally{busy=false;if(!stopped){const s=state();timer=setTimeout(pump,Math.max(1,Math.ceil((s?.[31]||16000)/1000)));}}
 };
 return {start(){stopped=false;key=-1;reported=false;route='';const transport=target.__eaglerPeerTransport;if(transport)transport.onReceive=pump;pump();},
  stop,pump};
}
