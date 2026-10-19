export const keyboardActionCodes={shoot:'KeyZ',bomb:'KeyX',focus:'ShiftLeft',pause:'Escape',up:'ArrowUp',down:'ArrowDown',left:'ArrowLeft',right:'ArrowRight',skip:'ControlLeft',confirm:'Enter',action:'KeyC',restart:'KeyR'} as const;
export const defaultKeyboardAliases:Readonly<Record<string,string>>=Object.freeze({});
export const wasdKeyboardAliases:Readonly<Record<string,string>>=Object.freeze({KeyW:'ArrowUp',KeyA:'ArrowLeft',KeyS:'ArrowDown',KeyD:'ArrowRight',KeyJ:'KeyZ',KeyK:'KeyX',KeyL:'ShiftLeft',Space:'ShiftLeft',KeyH:'KeyC'});
export function configurableKeyboardCode(code:string){return /^(Key[A-Z]|Digit[0-9]|F(?:[1-9]|1[0-2])|Space|Enter|NumpadEnter|Escape|Tab|Backspace|Home|Arrow(?:Up|Down|Left|Right)|(?:Shift|Control)(?:Left|Right)|Numpad[1-9])$/.test(code);}
const legacyStorageKey='eagler-touhou-keyboard-mapping-v1';
const storageKey='eagler-touhou-keyboard-layouts-v2';
export type KeyboardPreset='arrows'|'wasd';
type Aliases=Readonly<Record<string,string>>;
export interface SavedKeyboardLayout {id:string;name:string;aliases:Aliases;base:KeyboardPreset}
export interface KeyboardLayoutSnapshot {selected:string;base:KeyboardPreset;saved:readonly SavedKeyboardLayout[];persisted:boolean}
function validAliases(value:unknown):value is Record<string,string>{
  return !!value&&typeof value==='object'&&!Array.isArray(value)&&Object.entries(value).every(([source,target])=>
    configurableKeyboardCode(source)&&Object.values(keyboardActionCodes).some(code=>code===target));
}
function presetAliases(preset:KeyboardPreset):Aliases{return preset==='arrows'?defaultKeyboardAliases:wasdKeyboardAliases;}
function identify(aliases:Aliases):'arrows'|'wasd'|'custom'{
  if(!Object.keys(aliases).length)return 'arrows';
  return Object.keys(aliases).length===Object.keys(wasdKeyboardAliases).length&&Object.entries(wasdKeyboardAliases).every(([key,value])=>aliases[key]===value)?'wasd':'custom';
}
/** Runtime retains its aliases-only read port. The editor owns preset/named
 * layout metadata separately, so saved layouts never enter the input protocol. */
export function createKeyboardPreferences(storage:Pick<Storage,'getItem'|'setItem'>|null){
  let aliases:Aliases=defaultKeyboardAliases,custom:Aliases=defaultKeyboardAliases;
  let selected='arrows',base:KeyboardPreset='arrows',customBase:KeyboardPreset='arrows',saved:SavedKeyboardLayout[]=[],serial=0,persisted=!!storage;
  const listeners=new Set<()=>void>();
  try{
    const value=JSON.parse(storage?.getItem(storageKey)||'null');
    if(value?.version===2&&validAliases(value.aliases)){
      aliases=Object.freeze({...value.aliases});custom=Object.freeze({... (validAliases(value.custom)?value.custom:value.aliases)});
      base=value.base==='arrows'?'arrows':'wasd';
      customBase=value.customBase==='arrows'?'arrows':value.customBase==='wasd'?'wasd':base;
      if(Array.isArray(value.saved))for(const item of value.saved.slice(0,32)){
        if(!item||typeof item.id!=='string'||!/^saved-[a-z0-9-]+$/.test(item.id)||saved.some(s=>s.id===item.id)||
          typeof item.name!=='string'||!item.name.trim()||item.name.length>32||!validAliases(item.aliases))continue;
        saved.push(Object.freeze({id:item.id,name:item.name,aliases:Object.freeze({...item.aliases}),base:item.base==='arrows'?'arrows':'wasd'}));
      }
      const named=saved.find(item=>item.id===value.selected);
      selected=named?.id??(value.selected==='custom'?'custom':identify(aliases));
      if(named){aliases=named.aliases;base=named.base;}
      else if(value.selected==='arrows'||value.selected==='wasd'){
        selected=value.selected;base=value.selected;aliases=presetAliases(base);
      }
    }else{
      const legacy=JSON.parse(storage?.getItem(legacyStorageKey)||'null');
      if(legacy?.version===1&&validAliases(legacy.aliases)){aliases=Object.freeze({...legacy.aliases});custom=aliases;selected=identify(aliases);base=selected==='arrows'?'arrows':'wasd';customBase=base;}
    }
  }catch{persisted=false;}
  let layout:KeyboardLayoutSnapshot;
  function publish(write=true){
    if(write){
      try{storage?.setItem(storageKey,JSON.stringify({version:2,aliases,custom,selected,base,customBase,saved}));persisted=!!storage;}catch{persisted=false;}
      // Older input-capable Launchers still read the active aliases after a
      // downgrade. Named layout metadata stays in the new storage record.
      try{storage?.setItem(legacyStorageKey,JSON.stringify({version:1,aliases}));}catch{}
    }
    layout=Object.freeze({selected,base,saved:Object.freeze([...saved]),persisted});
    for(const listener of listeners)listener();
  }
  function change(next:Record<string,string>){aliases=Object.freeze(next);custom=aliases;customBase=base;selected='custom';publish();}
  function selectLayout(id:string):boolean{
    if(id==='arrows'||id==='wasd'){base=id;aliases=presetAliases(id);}
    else if(id==='custom'){aliases=custom;base=customBase;}
    else {const item=saved.find(item=>item.id===id);if(!item)return false;aliases=item.aliases;base=item.base;}
    selected=id;publish();return true;
  }
  publish(false);
  return {
    subscribe(listener:()=>void){listeners.add(listener);return()=>{listeners.delete(listener);};},
    getSnapshot:()=>aliases,getLayoutSnapshot:()=>layout,selectLayout,
    bind(action:keyof typeof keyboardActionCodes,source:string){
      if(!configurableKeyboardCode(source))return;const target=keyboardActionCodes[action];
      change({...Object.fromEntries(Object.entries(aliases).filter(([,value])=>value!==target)),[source]:target});
    },
    clear(action:keyof typeof keyboardActionCodes){change(Object.fromEntries(Object.entries(aliases).filter(([,target])=>target!==keyboardActionCodes[action])));},
    saveLayout(name:string):string|null{
      name=name.trim().slice(0,32);if(!name)return null;
      const existing=saved.find(item=>item.name===name);if(!existing&&saved.length>=32)return null;
      let id=existing?.id??`saved-${Date.now().toString(36)}-${++serial}`;
      while(!existing&&saved.some(item=>item.id===id))id=`saved-${Date.now().toString(36)}-${++serial}`;
      const item=Object.freeze({id,name,aliases:Object.freeze({...aliases}),base});
      saved=existing?saved.map(value=>value.id===id?item:value):[...saved,item];selected=id;publish();return persisted?id:null;
    },
    deleteLayout(id:string){saved=saved.filter(item=>item.id!==id);if(selected===id){selected='custom';custom=aliases;customBase=base;}publish();},
    reset(){selectLayout(base);},
  };
}
export type KeyboardPreferences=ReturnType<typeof createKeyboardPreferences>;
