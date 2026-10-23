// Generated from eagler-common/browser/{keyboard-owners,keyboard-input-bridge,directory-keyboard}.mjs.
// Generic browser ownership only. The adapter maps DOM events to logical bits.
export function keyboardLocation(event) {
  const code = event.code || "";
  if (/^(Shift|Control)(Left|Right)$/.test(code)) return code.endsWith("Left") ? 1 : 2;
  if (code.startsWith("Numpad")) return 3;
  return Number.isInteger(event.location) ? event.location : 0;
}

export function updateKeyboardOwners(owners, event, bit, down) {
  if (!bit) return false;
  const location = keyboardLocation(event);
  const code = event.code && event.code !== "Unidentified" ? event.code : "";
  const id = code ? `code:${code}` : `${bit}:${location}`;
  const candidates = [...owners.entries()].filter(([, owner]) => owner.bit === bit);
  const sameSide = candidates.filter(([, owner]) => owner.location === location);
  if (down) {
    const existing = owners.get(id) || (!code && sameSide.length === 1 ? sameSide[0][1]
      : !code && !location && candidates.length === 1 ? candidates[0][1] : null);
    // Lifecycle cancellation must not be undone by an old auto-repeat event.
    if (event.repeat && !existing) return false;
    if (!existing) owners.set(id, {bit, location, code});
    return true;
  }
  let released = owners.has(id) ? [[id, owners.get(id)]] : sameSide;
  if (!released.length) released = candidates.filter(([, owner]) => !owner.code && !owner.location);
  // With no physical evidence, release all candidates rather than arbitrarily
  // removing one and permanently stranding the other. Known sides stay distinct.
  if (!released.length && !code && !location) released = candidates;
  for (const [owner] of released) owners.delete(owner);
  return released.length > 0;
}

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


// Full browser keyboard boundary; gameplay bindings remain title-owned.
const names={escape:'Escape',esc:'Escape',backspace:'Backspace',tab:'Tab',enter:'Enter',
  shift:'Shift',control:'Control',alt:'Alt',meta:'Meta',os:'Meta',capslock:'CapsLock',
  numlock:'NumLock',scrolllock:'ScrollLock',pause:'Pause',printscreen:'PrintScreen',
  arrowup:'ArrowUp',arrowdown:'ArrowDown',arrowleft:'ArrowLeft',arrowright:'ArrowRight',
  home:'Home',end:'End',pageup:'PageUp',pagedown:'PageDown',insert:'Insert',delete:'Delete',
  contextmenu:'ContextMenu',' ':'Space','-':'Minus','=':'Equal','[':'BracketLeft',
  ']':'BracketRight',';':'Semicolon',"'":'Quote','`':'Backquote','\\':'Backslash',
  ',':'Comma','.':'Period','/':'Slash'};
const legacy={8:'Backspace',9:'Tab',13:'Enter',16:'Shift',17:'Control',18:'Alt',19:'Pause',
  20:'CapsLock',27:'Escape',32:'Space',33:'PageUp',34:'PageDown',35:'End',36:'Home',
  37:'ArrowLeft',38:'ArrowUp',39:'ArrowRight',40:'ArrowDown',44:'PrintScreen',45:'Insert',
  46:'Delete',91:'MetaLeft',92:'MetaRight',93:'ContextMenu',106:'NumpadMultiply',
  107:'NumpadAdd',109:'NumpadSubtract',110:'NumpadDecimal',111:'NumpadDivide',
  144:'NumLock',145:'ScrollLock',160:'ShiftLeft',161:'ShiftRight',162:'ControlLeft',
  163:'ControlRight',164:'AltLeft',165:'AltRight',186:'Semicolon',187:'Equal',188:'Comma',189:'Minus',
  190:'Period',191:'Slash',192:'Backquote',219:'BracketLeft',220:'Backslash',
  221:'BracketRight',222:'Quote'};
const aliases={ShiftLeft:'Shift',ShiftRight:'Shift',ControlLeft:'Control',ControlRight:'Control',
  AltLeft:'Alt',AltRight:'Alt',MetaLeft:'Meta',MetaRight:'Meta',NumpadEnter:'Enter',
  Numpad8:'ArrowUp',Numpad2:'ArrowDown',Numpad4:'ArrowLeft',Numpad6:'ArrowRight',
  Numpad7:'Home',Numpad1:'End',Numpad9:'PageUp',Numpad3:'PageDown',
  Numpad0:'Insert',NumpadDecimal:'Delete'};

export function runtimeKeyboardCode(event){
  const code=String(event.code||'');
  if(code&&code!=='Unidentified')return code;
  const key=String(event.key||'').toLowerCase(),number=Number(event.keyCode)||0;
  let value=(Object.hasOwn(names,key)?names[key]:'')||(/^[a-z]$/.test(key)?'Key'+key.toUpperCase():
    /^[0-9]$/.test(key)?'Digit'+key:/^f(?:[1-9]|1[0-9]|2[0-4])$/.test(key)?key.toUpperCase():'');
  if(!value)value=legacy[number]||(number>=65&&number<=90?'Key'+String.fromCharCode(number):
    number>=48&&number<=57?'Digit'+(number-48):number>=96&&number<=105?'Numpad'+(number-96):
    number>=112&&number<=135?'F'+(number-111):'');
  const location=Number(event.location)||0;
  if(/^(Shift|Control|Alt|Meta)$/.test(value))return value+(location===2?'Right':'Left');
  if(location===3){
    const operator={'*':'Multiply','+':'Add','-':'Subtract','/':'Divide','.':'Decimal'};
    if(Object.hasOwn(operator,key))return 'Numpad'+operator[key];
    if(value==='Enter')return 'NumpadEnter';
    if(/^Digit[0-9]$/.test(value))return 'Numpad'+value.slice(5);
    const numpad={ArrowUp:8,ArrowDown:2,ArrowLeft:4,ArrowRight:6,Home:7,End:1,
      PageUp:9,PageDown:3,Insert:0,Delete:'Decimal'};
    if(Object.hasOwn(numpad,value))return 'Numpad'+numpad[value];
  }
  return value;
}

export function createBrowserKeyboard({send,accept=()=>true,onClear=()=>{},hostContext}){
  const sources=new Map(),published=new Set();
  let generation=0;
  function publish(){
    const next=new Set([...sources.values()].flatMap(owners=>[...owners.values()].map(owner=>owner.output)));
    for(const code of published)if(!next.has(code))send(code,false);
    for(const code of next)if(!published.has(code))send(code,true);
    published.clear();for(const code of next)published.add(code);
  }
  let input;
  const keyboard={
    get generation(){return generation;},
    mapNative(event,down){return input?.event(event,down)??false;},
    event(event,down,source='native'){
      if(source==='hosted'&&input?.event(event,down,event.inputSource==='physical'?'physical':'virtual'))return true;
      const code=runtimeKeyboardCode(event);if(!code||!accept(code))return false;
      let owners=sources.get(source);if(!owners){owners=new Map();sources.set(source,owners);}
      // Match releases using the actual event evidence, never the synthesized
      // Left fallback. Save the resolved DOWN code to publish the eventual UP.
      const evidence={code:event.code,key:event.key,keyCode:event.keyCode,
        location:/^(Alt|Meta)(Left|Right)$/.test(event.code||'')?
          (event.code.endsWith('Right')?2:1):event.location,repeat:!!event.repeat};
      const family=Object.hasOwn(aliases,code)?aliases[code]:code;
      if(!updateKeyboardOwners(owners,evidence,family,!!down))return false;
      for(const owner of owners.values())if(!owner.output)owner.output=code;
      publish();return true;
    },
    clear(){
      input?.clear();
      ++generation;sources.clear();publish();onClear();
    }
  };
  if(hostContext)input=createKeyboardInputBridge({...hostContext,
    dispatch:(event,down)=>keyboard.event(event,down,'mapped'),reset:()=>hostContext.reset?hostContext.reset():keyboard.clear()});
  return keyboard;
}
