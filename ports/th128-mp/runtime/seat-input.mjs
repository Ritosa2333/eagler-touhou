import {rawGamepadDirections} from '../../th128/site/runtime/gamepad-input.mjs';

export const TH128_HELD=Object.freeze({KeyZ:1,KeyX:2,ShiftLeft:8,ShiftRight:8,ArrowUp:16,ArrowDown:32,
  ArrowLeft:64,ArrowRight:128,KeyC:512,ControlLeft:512});
const nativeMask=763,maxContacts=16;
const directionMask=16|32|64|128;
const padButtons=[[0,1],[1,2],[2,512],[4,8]];

export function createSeatInput({allowedMask,controllerIndex=0}){
  if(!Number.isInteger(allowedMask)||allowedMask<0||allowedMask>nativeMask||(allowedMask&~nativeMask)||
     !Number.isInteger(controllerIndex)||controllerIndex<0||controllerIndex>15)throw new RangeError('Invalid TH128 seat input policy');
  const keys=new Set(),contacts=new Map(),edges=new Map();
  function key(code,pressed){
    if(!Object.hasOwn(TH128_HELD,code)||!(TH128_HELD[code]&allowedMask))return false;
    if(pressed){if(!keys.has(code))edges.set(code,false);keys.add(code);}
    else{keys.delete(code);if(edges.has(code))edges.set(code,true);}
    return true;
  }
  function touch(id,bits){
    if(!Number.isSafeInteger(id)||!Number.isInteger(bits)||bits<0||bits>nativeMask||(bits&~allowedMask))
      throw new RangeError('Touch input exceeds the native mask');
    if(!bits){contacts.delete(id);return true;}
    if(!contacts.has(id)&&contacts.size>=maxContacts)return false;
    contacts.set(id,bits);return true;
  }
  function read(pads=[]){
    let held=0;
    for(const [code,released] of edges)if(released||keys.has(code))held|=TH128_HELD[code];
    edges.clear();
    for(const code of keys)held|=TH128_HELD[code];for(const bits of contacts.values())held|=bits;
    const pad=Array.from(pads).find(p=>p?.connected&&p.mapping==='standard'&&p.index===controllerIndex);
    if(pad){
      held|=rawGamepadDirections(pad);
      for(const [button,bit] of padButtons)if(pad.buttons?.[button]?.pressed||pad.buttons?.[button]?.value>=.5)held|=bit;
    }
    return held&allowedMask;
  }
  // The clock discards the gap. Keep at most one unsampled edge per mapping
  // until the next logical sample (or blur, hide or epoch change), but discard
  // held movement. A real keyup may complete that edge after the gap.
  function presentationStall(){keys.clear();contacts.clear();}
  function focusLost(){
    presentationStall();
    // A completed arrow still belongs to the seat that accepted it. Do not
    // preserve bombs, live holds or contacts when focus changes documents.
    for(const [code,released] of edges)if(!released||!(TH128_HELD[code]&directionMask))edges.delete(code);
  }
  function clear(){keys.clear();contacts.clear();edges.clear();}
  return Object.freeze({key,touch,read,presentationStall,focusLost,clear});
}
