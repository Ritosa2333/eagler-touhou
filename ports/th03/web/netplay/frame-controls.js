import {FireControl} from '../fire-control.js';
import {limitTouch} from '../touch-input.js';
// Same native key map and accepted-frame fire clock as controls.applyFrame.
const native=[
  {up:'KeyT',down:'KeyB',left:'KeyF',right:'KeyH',shot:'KeyZ',attack:'KeyX'},
  {up:'Numpad8',down:'Numpad2',left:'Numpad4',right:'Numpad6',shot:'ArrowLeft',attack:'ArrowRight'},
];
export function createFrameControls(emulator,pause,{focusEnabled=true,touchUnlimitedAllowed=false}={}){
  const fire=[new FireControl(),new FireControl()];let previous=new Set(),fireFrame=0;
  return {
    applyFrame(inputs){
      const target=new Set();let mask=0;
      inputs.forEach((input,slot)=>{
        for(const action of input.actions)if(action!=='rapid'&&action!=='shot')target.add(`${slot}:${action}`);
        if(fire[slot].sample(input.actions.includes('rapid'),input.actions.includes('shot'),fireFrame*1000/60))target.add(`${slot}:shot`);
        if(focusEnabled&&input.actions.includes('focus'))mask|=1<<slot;
      });
      fireFrame++;pause.setFocus(mask,0);pause.setTouch(inputs.map(input=>limitTouch(input.touch,touchUnlimitedAllowed)));
      for(const token of new Set([...previous,...target])){
        if(token.endsWith(':focus')||previous.has(token)===target.has(token))continue;
        emulator.module.netKey(target.has(token)?'keydown':'keyup',{code:native[Number(token[0])][token.slice(2)]});
      }
      previous=target;
    },
    capture:()=>({previous:[...previous],fireFrame,fire:fire.map(f=>({...f.state}))}),
    restore(saved){previous=new Set(saved?.previous||[]);fireFrame=saved?.fireFrame||0;fire.forEach((f,i)=>{if(saved?.fire?.[i])f.state={...saved.fire[i]};else f.reset();});},
  };
}
