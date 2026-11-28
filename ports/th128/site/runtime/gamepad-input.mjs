// The Launcher exposes the same semantic gamepad port to all directory players.
const schema='eagler-touhou/gamepad-input/1';
const mappedCodes=['KeyZ','KeyX','ShiftLeft','Escape','ArrowUp','ArrowDown','ArrowLeft','ArrowRight',null,'KeyZ','KeyC','KeyR'];
const directionCodes=['ArrowUp','ArrowDown','ArrowLeft','ArrowRight'];
const edgeOnly=new Set(['KeyZ','KeyX','Escape','KeyR']);

export function installLauncherGamepadInput({target,protocol,game,epoch,origin,isReady}){
  let live=true;
  const mapper=()=>{
    const value=target.__eaglerGamepadMapping;
    return value?.schema===schema&&value.protocol===protocol&&value.game===game&&value.epoch===epoch&&value.origin===origin
      &&['enabled','sample','clear'].every(method=>typeof value[method]==='function')?value:null;
  };
  const bridge=Object.freeze({schema,protocol,game,epoch,origin,
    owned:()=>live&&mapper()?.enabled()===true,
    sample(seat){
      if(!live)return 0;
      const owner=mapper();if(!owner||owner.enabled()!==true)return -1;
      if(!isReady()){owner.clear();return 0;}
      const bits=owner.sample(seat);return Number.isInteger(bits)&&bits>=0&&bits<=4095?bits:0;
    }
  });
  target.__eaglerGamepadInputBridge=bridge;
  return ()=>{live=false;if(target.__eaglerGamepadInputBridge===bridge)delete target.__eaglerGamepadInputBridge;};
}

export function readBrowserGamepad(navigator){
  try{return Array.from(navigator?.getGamepads?.()??[]).find(pad=>pad?.connected&&!/keyboard|\bkb\b/i.test(pad.id??''))??null;}
  catch{return null;}
}
const heldButton=(pad,index)=>!!(pad?.buttons?.[index]?.pressed||pad?.buttons?.[index]?.value>=.5);
export function analogGamepadDirections(x,y){
  if(!Number.isFinite(x)||!Number.isFinite(y)||x*x+y*y<.30*.30)return 0;
  // The boundaries of the eight sectors are 22.5 degrees from either axis.
  const ax=Math.abs(x),ay=Math.abs(y),boundary=Math.SQRT2-1;
  return (ay>=ax*boundary?(y<0?16:32):0)|(ax>=ay*boundary?(x<0?64:128):0);
}
export function rawGamepadDirections(pad){
  const [x=0,y=0]=pad?.axes??[];
  return analogGamepadDirections(x,y)|(heldButton(pad,12)?16:0)|(heldButton(pad,13)?32:0)
    |(heldButton(pad,14)?64:0)|(heldButton(pad,15)?128:0);
}
export function mappedGamepadCodes(window){
  const bits=window.__eaglerGamepadInputBridge?.sample(0)??-1;
  if(bits===-1)return null;
  return [...new Set(mappedCodes.filter((code,index)=>code&&(bits&(1<<index))))];
}
export function mountStageGamepad({window,document,input}){
  const owner=Symbol('th128-stage-gamepad');
  const blocked=new Set();
  const read=()=>{
    const mapped=mappedGamepadCodes(window);if(mapped)return mapped;
    const pad=readBrowserGamepad(window.navigator),directions=rawGamepadDirections(pad);
    const codes=directionCodes.filter((_,index)=>directions&(16<<index));
    for(const [code,index] of [['KeyZ',0],['KeyX',1],['KeyC',2],['ShiftLeft',4],['Escape',9]])if(heldButton(pad,index))codes.push(code);
    return codes;
  };
  const clear=()=>input.clearKeySource(owner);
  return {
    clear,
    suppressHeldActions(){for(const code of read())if(edgeOnly.has(code))blocked.add(code);clear();},
    poll(){
      if(document.hidden){clear();return;}
      const keys=read();
      for(const code of blocked)if(!keys.includes(code))blocked.delete(code);
      const next=keys.filter(code=>!blocked.has(code));
      if(next.length)input.setKeySource(owner,next);else clear();
    }
  };
}
