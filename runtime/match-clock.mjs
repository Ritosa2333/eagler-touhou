const step=1000/60,epsilon=1e-7,discontinuityMs=250,maxDebt=step*3;

// Only presentation timestamps add game time. An input callback can spend an
// already due tick without waiting for the next draw, but never predicts.
// Wait one frame for actual input before taking the expensive WASM snapshot.
export function createMatchClock({stepFrame}){
  if(typeof stepFrame!=='function')throw new TypeError('A synchronous multiplayer frame consumer is required');
  let last=null,credit=0,inside=false;
  const idle=(stalled=false)=>({advanced:0,stalled,reason:null});
  function reset(){last=null;credit=0;}
  function pump(mayPredict){
    if(inside)throw new Error('Reentrant multiplayer clock');
    inside=true;let advanced=0,reason=null;
    try{
      while(credit+epsilon>=step){
        const result=stepFrame({allowPrediction:mayPredict&&credit+epsilon>=step*2});
        if(typeof result?.advanced!=='boolean')throw new Error('Multiplayer frame consumer must return its step status');
        if(!result.advanced){
          reason=result.reason??null;
          if(reason!=='waiting-input')reset();
          break;
        }
        credit=Math.max(0,credit-step);advanced++;
      }
    }finally{inside=false;}
    return {advanced,stalled:false,reason};
  }
  function advance(now,active){
    if(!Number.isFinite(now))throw new Error('Invalid multiplayer presentation timestamp');
    if(!active){reset();return idle();}
    if(last===null){last=now;return idle();}
    const elapsed=now-last;last=now;
    if(elapsed<0||elapsed>discontinuityMs){credit=0;return idle(true);}
    credit=Math.min(maxDebt,credit+elapsed);return pump(true);
  }
  return Object.freeze({advance,input:()=>last===null?idle():pump(false),reset});
}
