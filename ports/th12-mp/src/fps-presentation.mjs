// TH12 recorded FPS:43bb20, %3d at383,450. Live FPS:41cc80,
// %2.1ffps at588,470, with the original +0.05 formatting adjustment.
export function fpsTextDescriptors({recordedFps=null,currentFps=null}={}) {
  const output=[],base={z:0,scaleX:1,scaleY:1,alignX:1,alignY:1};
  if(Number.isInteger(recordedFps)&&recordedFps>=0&&recordedFps<=255)
    output.push({...base,kind:'recorded-fps',text:String(recordedFps).padStart(3,' '),font:0,x:383,y:450,
      colorARGB:recordedFps<30?0xff5050ff:recordedFps<50?0xffa0a0ff:0xffffffff,drawPriority:69,drawFlag:0});
  if(Number.isFinite(currentFps)&&currentFps>=0) {
    const fps=Math.fround(currentFps),text=(fps===0?0:fps+.05).toFixed(1)+'fps';
    // Browser presentation can exceed the retail60 Hz limit. Keep extra
    // digits inside the canvas on high-refresh-rate screens.
    output.push({...base,kind:'current-fps',text,font:1,x:588-Math.max(0,text.length-7)*7,y:470,
      // 401720 sets the last TextRecord.font (+120) to1, not its drawFlag
      // (+128). Both FPS submissions use the normal priority69 font queue.
      colorARGB:fps<=30?0xff5050ff:fps<40?0xffa0a0ff:0xffffffff,drawPriority:69,drawFlag:0});
  }
  return output;
}

// Original41cb70 publishes a frame count over elapsed wall time once a
// second. Count real animation-frame callbacks, never replay simulation ticks.
export function createFrameRateMeter() {
  let since=null,frames=0,value=0;
  return {
    get value(){return value;},
    reset(now){since=Number.isFinite(now)?now:null;frames=0;},
    frame(now,presented=true){
      if(!Number.isFinite(now))return value;
      if(since===null||now<since){this.reset(now);return value;}
      if(presented)frames++;
      const elapsed=now-since;
      if(elapsed>=1000){value=Math.fround(frames*1000/elapsed);since=now;frames=0;}
      return value;
    }
  };
}

// Count new visible game time, including real interpolation subframes. Browser
// redraws of the same stalled world are not additional gameplay pictures.
export function createPresentationFrameRateMeter(){
  const meter=createFrameRateMeter();let endpoint=null,progress=null;
  return {
    get value(){return meter.value;},
    reset(now){meter.reset(now);endpoint=null;progress=null;},
    frame(now,previous,current,alpha=1,running=true){
      let fresh=true;
      if(running&&current){
        const identity=String(current.stage);
        const continuous=previous&&previous.epoch===current.epoch&&previous.stage===current.stage&&
          current.frame===previous.frame+1&&((previous.nativeMotion&&current.nativeMotion)||(previous.motion&&current.motion));
        const position=current.frame-(continuous?1-Math.max(0,Math.min(1,alpha)):0);
        fresh=endpoint!==identity||progress===null||position>progress+1e-7;
        if(endpoint!==identity){endpoint=identity;progress=position;}
        else progress=Math.max(progress??position,position);
      }else{endpoint=null;progress=null;}
      return meter.frame(now,fresh);
    }
  };
}
