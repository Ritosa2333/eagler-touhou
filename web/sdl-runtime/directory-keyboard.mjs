// Generated from eagler-common/browser/{keyboard-owners,directory-keyboard}.mjs.
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

export function createBrowserKeyboard({send,accept=()=>true,onClear=()=>{}}){
  const sources=new Map(),published=new Set();
  let generation=0;
  function publish(){
    const next=new Set([...sources.values()].flatMap(owners=>[...owners.values()].map(owner=>owner.output)));
    for(const code of published)if(!next.has(code))send(code,false);
    for(const code of next)if(!published.has(code))send(code,true);
    published.clear();for(const code of next)published.add(code);
  }
  return {
    get generation(){return generation;},
    event(event,down,source='native'){
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
      ++generation;sources.clear();publish();onClear();
    }
  };
}
