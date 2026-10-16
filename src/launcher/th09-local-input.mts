import {HostedKeyboard, hostedKeyCodes, type HostedKeyboardEvent} from './hosted-keyboard.mjs';
import {configurableKeyboardCode} from './keyboard-preferences.mjs';
import {gamepadActions, type GamepadAction} from './gamepad-mapping.mjs';
import type {HostedKeySpec, TouchRuntimeContext} from './touch-runtime-protocol.mjs';

export type LocalKeyboardBindings=Record<GamepadAction,readonly string[]>;
export type LocalKeyboardPreset='original'|'wasd'|'arrows';
export interface LocalKeyboardLayout {id:string;name:string;bindings:LocalKeyboardBindings}
export interface LocalInputPlayer {device:'keyboard'|'gamepad';gamepad:string;selected:string;bindings:LocalKeyboardBindings;saved:readonly LocalKeyboardLayout[]}
export interface Th09LocalInputSnapshot {enabled:boolean;players:readonly LocalInputPlayer[];revision:number;persisted:boolean}
const storageKey='eagler-touhou-th09-local-input-v1';
const shared=new Set<GamepadAction>(['pause','confirm','skip','restart']);
export const localInputActions=gamepadActions.filter(action=>action!=='action');
export function localKeyboardPreset(seat:number,preset:LocalKeyboardPreset):LocalKeyboardBindings {
  const b=Object.fromEntries(gamepadActions.map(a=>[a,[]])) as unknown as LocalKeyboardBindings;
  Object.assign(b,{pause:['Escape'],confirm:['Enter','NumpadEnter'],skip:['ControlLeft','ControlRight'],restart:['Backspace']});
  if(preset==='wasd')Object.assign(b,{up:['KeyW'],down:['KeyS'],left:['KeyA'],right:['KeyD'],shoot:['KeyF'],bomb:['KeyG'],focus:['ShiftLeft']});
  else if(preset==='arrows')Object.assign(b,{up:['ArrowUp'],down:['ArrowDown'],left:['ArrowLeft'],right:['ArrowRight'],shoot:['Numpad1'],bomb:['Numpad2'],focus:['Numpad3']});
  else if(seat===0)Object.assign(b,{up:['KeyT','KeyR','KeyY'],down:['KeyB','KeyV','KeyN'],left:['KeyF','KeyR','KeyV'],right:['KeyH','KeyY','KeyN'],shoot:['KeyZ'],bomb:['KeyX'],focus:['ShiftLeft','ShiftRight']});
  else Object.assign(b,{up:['Numpad8','Numpad7','Numpad9'],down:['Numpad2','Numpad1','Numpad3'],left:['Numpad4','Numpad7','Numpad1'],right:['Numpad6','Numpad9','Numpad3'],shoot:['ArrowDown'],bomb:['ArrowRight'],focus:['ArrowLeft']});
  return b;
}
function validBindings(value:unknown):value is LocalKeyboardBindings {
  return !!value&&typeof value==='object'&&gamepadActions.every(a=>Array.isArray((value as LocalKeyboardBindings)[a])&&(value as LocalKeyboardBindings)[a].length<=8&&(value as LocalKeyboardBindings)[a].every(configurableKeyboardCode));
}
const copy=(bindings:LocalKeyboardBindings):LocalKeyboardBindings=>Object.fromEntries(gamepadActions.map(a=>[a,[...bindings[a]]])) as unknown as LocalKeyboardBindings;
function collision(a:LocalInputPlayer,b:LocalInputPlayer):string|null {
  if(a.device!=='keyboard'||b.device!=='keyboard')return null;
  for(const action of gamepadActions)for(const key of a.bindings[action])for(const other of gamepadActions)
    if(b.bindings[other].includes(key)&&!(action===other&&shared.has(action)))return key;
  return null;
}
/** TH09 local versus owns two independent devices/layouts. General single-player
 * aliases and controller assignments never overwrite this saved pair. */
export function createTh09LocalInput(storage:Pick<Storage,'getItem'|'setItem'>|null){
  let enabled=false,revision=0,persisted=!!storage,serial=0;
  let players:LocalInputPlayer[]=[0,1].map(seat=>({device:'keyboard',gamepad:'',selected:'original',bindings:localKeyboardPreset(seat,'original'),saved:[]}));
  try{const saved=JSON.parse(storage?.getItem(storageKey)||'null');
    if(saved?.version===1&&Array.isArray(saved.players)&&saved.players.length===2){
      const next=saved.players.map((p:LocalInputPlayer,seat:number)=>({device:p?.device==='gamepad'?'gamepad':'keyboard',gamepad:typeof p?.gamepad==='string'?p.gamepad.slice(0,1024):'',selected:typeof p?.selected==='string'?p.selected:'original',bindings:validBindings(p?.bindings)?copy(p.bindings):localKeyboardPreset(seat,'original'),saved:Array.isArray(p?.saved)?p.saved.slice(0,32).filter(s=>s&&typeof s.id==='string'&&s.id.startsWith('saved-')&&typeof s.name==='string'&&s.name.trim()&&s.name.length<=32&&validBindings(s.bindings)).map(s=>({...s,bindings:copy(s.bindings)})):[]})) as LocalInputPlayer[];
      if(!collision(next[0]!,next[1]!)&&!(next[0]!.device==='gamepad'&&next[1]!.device==='gamepad'&&next[0]!.gamepad&&next[0]!.gamepad===next[1]!.gamepad)){players=next;enabled=saved.enabled===true;}
    }
  }catch{persisted=false;}
  const listeners=new Set<()=>void>();let snapshot:Th09LocalInputSnapshot;
  function publish(write=true){if(write){revision++;try{storage?.setItem(storageKey,JSON.stringify({version:1,enabled,players}));persisted=!!storage;}catch{persisted=false;}}
    snapshot={enabled,players,revision,persisted};for(const listener of listeners)listener();}
  function change(seat:number,next:LocalInputPlayer):string|null {
    const other=players[1-seat];if(!other||!players[seat])return 'invalid';
    const key=collision(next,other);if(key)return key;
    if(next.device==='gamepad'&&other.device==='gamepad'&&next.gamepad&&next.gamepad===other.gamepad)return 'gamepad';
    players=players.map((p,n)=>n===seat?next:p);publish();return null;
  }
  publish(false);
  return {
    subscribe(listener:()=>void){listeners.add(listener);return()=>{listeners.delete(listener);};},getSnapshot:()=>snapshot,
    setEnabled(value:boolean){enabled=value;publish();},
    setDevice(seat:number,device:'keyboard'|'gamepad',gamepad=players[seat]?.gamepad??''){const p=players[seat];return p?change(seat,{...p,device,gamepad}):'invalid';},
    selectLayout(seat:number,id:string){const p=players[seat];if(!p)return 'invalid';const saved=p.saved.find(s=>s.id===id);
      const bindings=saved?.bindings??(['original','wasd','arrows'].includes(id)?localKeyboardPreset(seat,id as LocalKeyboardPreset):null);
      return bindings?change(seat,{...p,selected:id,bindings:copy(bindings)}):'invalid';},
    bind(seat:number,action:GamepadAction,key:string){const p=players[seat];if(!p||!configurableKeyboardCode(key))return 'invalid';const bindings=copy(p.bindings);
      for(const a of gamepadActions)bindings[a]=bindings[a].filter(k=>k!==key);bindings[action]=[key];return change(seat,{...p,selected:'custom',bindings});},
    clear(seat:number,action:GamepadAction){const p=players[seat];return p?change(seat,{...p,selected:'custom',bindings:{...p.bindings,[action]:[]}}):'invalid';},
    saveLayout(seat:number,name:string){const p=players[seat];name=name.trim().slice(0,32);if(!p||!name)return null;const existing=p.saved.find(s=>s.name===name);if(!existing&&p.saved.length>=32)return null;
      const id=existing?.id??`saved-${Date.now().toString(36)}-${++serial}`,item={id,name,bindings:copy(p.bindings)};
      change(seat,{...p,selected:id,saved:existing?p.saved.map(s=>s.id===id?item:s):[...p.saved,item]});return id;},
    deleteLayout(seat:number,id:string){const p=players[seat];if(p)change(seat,{...p,selected:p.selected===id?'custom':p.selected,saved:p.saved.filter(s=>s.id!==id)});},
  };
}
export type Th09LocalInput=ReturnType<typeof createTh09LocalInput>;

const keyCodes={...hostedKeyCodes};
keyCodes.Space=[' ',32];
for(let n=65;n<=90;n++)keyCodes['Key'+String.fromCharCode(n)]=[String.fromCharCode(n).toLowerCase(),n];
for(let n=0;n<=9;n++)keyCodes['Digit'+n]=[String(n),48+n];
for(let n=1;n<=12;n++)keyCodes['F'+n]=['f'+n,111+n];
for(let n=1;n<=9;n++)keyCodes['Numpad'+n]=['numpad'+n,96+n];
/** Physical ownership is separate from the two sampled semantic words. An
 * unseen short Fire/movement stroke survives one capture; observed holds do not
 * gain a release frame. Replay/network still consume only Runtime's words. */
export class Th09LocalKeyboard {
  private physical=new HostedKeyboard(keyCodes);
  private held=new Map<HostedKeySpec,readonly number[]>();
  private pending=[0,0];private revision=-1;private identity='';
  constructor(private model:Th09LocalInput){}
  clear(){this.physical.clear();this.held.clear();this.pending=[0,0];}
  private sync(context:TouchRuntimeContext){const state=this.model.getSnapshot(),identity=context.game+':'+context.epoch;
    if(state.revision!==this.revision||identity!==this.identity){this.clear();this.revision=state.revision;this.identity=identity;}return state;}
  map(event:HostedKeyboardEvent,down:boolean,context:TouchRuntimeContext):boolean {
    const state=this.sync(context);if(!state.enabled)return false;
    const owners=this.physical.forward({...event,type:down?'keydown':'keyup'},context,!!event.launcherOwnsFocus);
    let consumed=false;
    for(const raw of owners){
      if(!down){consumed=this.held.delete(raw)||consumed;continue;}
      if(this.held.has(raw)){consumed=true;continue;}
      const bits=state.players.map(p=>p.device==='keyboard'?gamepadActions.reduce((mask,a,n)=>mask|(p.bindings[a].includes(raw.code)?1<<n:0),0):0);
      if(bits.some(Boolean)){const previous=[0,1].map(seat=>this.bits(seat));this.held.set(raw,bits);for(let seat=0;seat<2;seat++)this.pending[seat]!|=bits[seat]!&~previous[seat]!;consumed=true;}
    }
    // Repeats retired by a reset stay consumed instead of entering native input.
    return consumed||state.players.some(p=>p.device==='keyboard'&&gamepadActions.some(a=>p.bindings[a].includes(event.code)));
  }
  private bits(seat:number){let bits=0;for(const held of this.held.values())bits|=held[seat]??0;return bits;}
  sample(seat:number,context:TouchRuntimeContext):number {const state=this.sync(context);if(!state.enabled)return -1;if(seat!==0&&seat!==1)return 0;
    if(!context.launched||context.spectator){this.clear();return 0;}const bits=this.bits(seat)|(this.pending[seat]!&0xf1);this.pending[seat]=0;return bits;}
}
