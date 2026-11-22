import {decodeDrawBuffer} from './draw.mjs';
import {FrameStepper} from './timing.mjs';

// Main 1.00a: 0x445770 selects this table at 1800 idle ticks; 0x426e0d
// starts the return at 3600 gameplay ticks. The RPYs extend beyond that point.
export const originalTitleDemos=Object.freeze([
  Object.freeze({name:'demo0.rpy',stage:1,frames:6686}),
  Object.freeze({name:'demo1.rpy',stage:6,frames:6722}),
  Object.freeze({name:'demo2.rpy',stage:11,frames:6094}),
]);
export const originalTitleDemoLimit=3600;
export const originalTitleDemoInputs=new Set(['KeyZ','KeyX','KeyC','ControlLeft','ControlRight','Enter','Escape']);
export class OriginalTitleDemoClock {
  #idle=0;
  #next=0;
  reset(){this.#idle=0;}
  tick(active,input){
    if(!active||input){this.reset();return null;}
    if(++this.#idle<1800)return null;
    this.reset();const demo=originalTitleDemos[this.#next];
    this.#next=(this.#next+1)%originalTitleDemos.length;
    return demo;
  }
}

export const originalStartupSignatureBank=26;
const asciiBank=11,frontBank=5;
const coreError=module=>module.UTF8ToString(module._th128_error());
function drawRecords(module){
  const count=module._th128_draw_count(),failure=coreError(module);
  if(failure)throw new Error(failure);
  return decodeDrawBuffer(module.HEAPU8.buffer,module._th128_draw_ptr(),count,module._th128_draw_stride());
}
function bind(module,bank,script,key){
  if(module._th128_bind(bank,script,key)!==1)throw new Error(coreError(module)||'Original title animation binding rejected');
}
function step(module){
  if(module._th128_tick()!==1)throw new Error(coreError(module)||'Original title animation tick rejected');
}

function bindOriginalLoadingScene(module,signature){
  // Both 0x433510 (boot) and 0x413296 (scene change) bind ASCII S22 at (480,392).
  // Only boot registers signature S0, which spawns its right-hand tile.
  module._th128_reset();
  if(coreError(module))throw new Error(coreError(module));
  if(signature)bind(module,originalStartupSignatureBank,0,0x683);
  bind(module,asciiBank,22,0x684);
  return {
    step:()=>step(module),
    draws:()=>drawRecords(module).map(record=>record.bank===asciiBank?{...record,x:record.x+480,y:record.y+392}:record),
  };
}
export const bindOriginalStartupScene=module=>bindOriginalLoadingScene(module,true);
export const bindOriginalTransitionScene=module=>bindOriginalLoadingScene(module,false);

function showOriginalLoading({module,renderer,document,window,signal,onPresent},signature){
  signal.throwIfAborted();
  const scene=bindOriginalLoadingScene(module,signature),clock=new FrameStepper();
  let id=null,closed=false,frames=0,settle,fail;
  const ready=new Promise((resolve,reject)=>{settle=resolve;fail=reject;});
  // The load may fail before it gets to await ready.
  ready.catch(()=>{});
  function close(){
    if(closed)return;closed=true;
    if(id!==null)window.cancelAnimationFrame(id);
    signal.removeEventListener('abort',abort);
    document.removeEventListener('visibilitychange',visibility);
    settle();
  }
  function abort(){fail(signal.reason);close();}
  function visibility(){clock.reset();if(document.hidden)settle();}
  function present(timestamp){
    if(closed)return;
    try{
      const active=!document.hidden,{steps}=clock.advance(timestamp,active);
      if(active){
        for(let n=0;n<steps;n++){scene.step();frames++;}
        if(steps){
          // Direct render, no 640×480 menu raster or blit.
          renderer.render(scene.draws());onPresent?.(frames);
          if(frames>=24)settle();
        }
      }
      id=window.requestAnimationFrame(present);
    }catch(error){fail(error);close();}
  }
  signal.addEventListener('abort',abort,{once:true});document.addEventListener('visibilitychange',visibility);
  visibility();id=window.requestAnimationFrame(present);
  return {ready,close};
}
export const showOriginalStartup=options=>showOriginalLoading(options,true);
export const showOriginalTransition=options=>showOriginalLoading(options,false);

export function duckOriginalDemoMusic(output){
  const {gain}=output,original=gain.value;
  gain.value=original*.25;
  let live=true;
  return ()=>{if(live){live=false;gain.value=original;}};
}

export function bindOriginalDemoPlay(module,front){
  // A stateless presentation scene, independent of the replay's native World.
  module._th128_reset();
  if(coreError(module))throw new Error(coreError(module));
  const pointer=module._malloc(front.length);
  if(!pointer)throw new Error('Original Demo ANM memory unavailable');
  try{
    module.HEAPU8.set(front,pointer);
    if(module._th128_load_anm(frontBank,pointer,front.length)!==1)throw new Error(coreError(module));
  }finally{module._free(pointer);}
  bind(module,frontBank,79,0x685);
  return {step:()=>step(module),draws:()=>drawRecords(module)};
}
