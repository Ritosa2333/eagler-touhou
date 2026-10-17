// The Launcher owns remapping. Runtime adapters synchronously consume the
// resulting canonical key transitions before their next simulation sample.
export function createKeyboardInputBridge({target=globalThis,protocol,game,epoch,origin,ready,dispatch,reset,profile=()=> 'standard'}) {
  const schema='eagler-touhou/keyboard-input/1',held=new Set();
  function gamepadMapper(){
    const value=target.__eaglerGamepadMapping;
    return value&&value.schema==='eagler-touhou/gamepad-input/1'&&value.protocol===protocol&&value.game===game&&
      value.epoch===epoch&&value.origin===origin&&typeof value.sample==='function'&&typeof value.clear==='function'&&typeof value.enabled==='function'?value:null;
  }
  target.__eaglerGamepadInputBridge=Object.freeze({schema:'eagler-touhou/gamepad-input/1',protocol,game,epoch,origin,
    owned:()=>gamepadMapper()?.enabled()===true,
    sample(seat){const owner=gamepadMapper();if(!owner)return -1;
      if(!ready()){owner.clear();return 0;}
      const bits=owner.sample(seat);return Number.isInteger(bits)&&bits>=-1&&bits<=4095?bits:0;
    }});
  function mapper() {
    const value=target.__eaglerKeyboardMapping;
    return value&&value.schema===schema&&value.protocol===protocol&&value.game===game&&
      value.epoch===epoch&&value.origin===origin&&typeof value.map==='function'&&typeof value.clear==='function'&&typeof value.supports==='function'?value:null;
  }
  function apply(events) {
    if(!Array.isArray(events)||events.some(e=>!e||typeof e.code!=='string'||!e.code||typeof e.down!=='boolean'||typeof e.key!=='string'||!Number.isInteger(e.keyCode)||!Number.isInteger(e.location)))
      throw Error('Invalid canonical keyboard transitions');
    for(const event of events){if(event.down)held.add(event.code);else held.delete(event.code);dispatch(event,event.down);}
  }
  let lastProfile;
  function currentProfile(){const next=profile();if(lastProfile!==undefined&&lastProfile!==next){const owner=mapper();if(owner)apply(owner.clear());gamepadMapper()?.clear();}lastProfile=next;return next;}
  function event(raw,down,source='physical') {
    const owner=mapper();if(!owner||!ready())return false;
    const editing=!!raw.launcherOwnsFocus||!!raw.target?.closest?.('input,textarea,select,[contenteditable="true"]');
    if(source==='physical'&&down&&(raw.isComposing||editing))return false;
    const events=owner.map({code:String(raw.code||''),key:String(raw.key||''),keyCode:Number(raw.keyCode)||0,
      location:Number(raw.location)||0,repeat:!!raw.repeat,altKey:!!raw.altKey,metaKey:!!raw.metaKey,ctrlKey:!!raw.ctrlKey,
      launcherOwnsFocus:editing},!!down,source,currentProfile());
    if(events===null)return false;
    apply(events);return true;
  }
  const bridge=Object.freeze({schema,protocol,game,epoch,origin,event,
    profile:currentProfile,
    sampleLocal(seat){const mode=currentProfile(),owner=mapper();if(mode!=='th09-local'||typeof owner?.sampleLocal!=='function')return -1;
      if(!ready()){apply(owner.clear());return 0;}const bits=owner.sampleLocal(seat);return Number.isInteger(bits)&&bits>=-1&&bits<=4095?bits:0;},
    active:()=>!!mapper(),keyDown:code=>held.has(code),
    // SDL uses USB HID scancodes. Keep physical-only THPrac helpers on the
    // same canonical keyboard without sampling or consuming gameplay pulses.
    scancodeState(scan){
      const owner=mapper();if(!owner)return -1;
      const special={40:'Enter',41:'Escape',42:'Backspace',43:'Tab',44:'Space',74:'Home',
        79:'ArrowRight',80:'ArrowLeft',81:'ArrowDown',82:'ArrowUp',88:'NumpadEnter',
        224:'ControlLeft',225:'ShiftLeft',228:'ControlRight',229:'ShiftRight'};
      const code=scan>=4&&scan<=29?'Key'+String.fromCharCode(65+scan-4):scan>=30&&scan<=38?'Digit'+(scan-29):scan===39?'Digit0':
        scan>=58&&scan<=69?'F'+(scan-57):scan>=89&&scan<=97?'Numpad'+(scan-88):special[scan];
      return code&&owner.supports?.(code)?Number(held.has(code)):-1;
    },
    clear(){const owner=mapper();if(owner)apply(owner.clear());held.clear();gamepadMapper()?.clear();},
    submit(message){
      if(!mapper()||!message||message.protocol!==protocol||message.game!==game||message.epoch!==epoch||message.request!=null)return false;
      if(message.command==='keyboard-clear'){reset();return true;}
      if(message.command!=='keyboard'||!ready()||typeof message.down!=='boolean'||typeof message.code!=='string')return false;
      return event(message,message.down,message.inputSource==='physical'?'physical':'virtual');
    }
  });
  target.__eaglerKeyboardInputBridge=bridge;
  return bridge;
}
