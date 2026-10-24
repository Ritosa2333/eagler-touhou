import {HostedKeyboard, hostedKeyCodes, type HostedKeyboardEvent} from './hosted-keyboard.mjs';
import type {HostedKeySpec, TouchRuntimeContext} from './touch-runtime-protocol.mjs';
import {defaultKeyboardAliases} from './keyboard-preferences.mjs';
import {Th09LocalKeyboard, type Th09LocalInput} from './th09-local-input.mjs';

export const keyboardMappingSchema = 'eagler-touhou/keyboard-input/1';
const codes: Record<string, [string, number]> = {...hostedKeyCodes,
  KeyW: ['w',87], KeyA: ['a',65], KeyJ: ['j',74], KeyK: ['k',75], KeyL: ['l',76], KeyH: ['h',72], Space: [' ',32],
  KeyT: ['t',84], KeyF: ['f',70], KeyB: ['b',66],
  Numpad8: ['arrowup',104],Numpad2: ['arrowdown',98],Numpad4: ['arrowleft',100],Numpad6: ['arrowright',102]};
codes.Numpad5=['numpad5',101];
for(let n=65;n<=90;n++)codes['Key'+String.fromCharCode(n)]=[String.fromCharCode(n).toLowerCase(),n];
for(let n=0;n<=9;n++)codes['Digit'+n]=[String(n),48+n];
for(let n=0;n<=9;n++)if(!codes['Numpad'+n])codes['Numpad'+n]=[String(n),96+n];
Object.assign(codes,{Minus:['-',189],Period:['.',190],NumpadSubtract:['-',109],NumpadDecimal:['.',110],
  Delete:['delete',46],Insert:['insert',45],End:['end',35],PageUp:['pageup',33],PageDown:['pagedown',34]});
for(let n=1;n<=12;n++)codes['F'+n]=['f'+n,111+n];
export interface CanonicalKeyTransition extends HostedKeySpec {down: boolean; location: number;}
export interface KeyboardMappingPort {
  schema: typeof keyboardMappingSchema; protocol: string; game: string; epoch: number; origin: string;
  map(event: HostedKeyboardEvent, down: boolean, source: string, profile: string): CanonicalKeyTransition[] | null;
  clear(): CanonicalKeyTransition[];
  supports(code: string): boolean;
  sampleLocal?(seat:number):number;
}
function spec(raw: HostedKeySpec): HostedKeySpec {
  const candidates = Object.keys(codes);
  const key = raw.key.toLowerCase();
  const preferred = (code: string) => raw.location === 2 ? code.endsWith('Right') : raw.location === 3 ? code.startsWith('Numpad') : true;
  const byKey = candidates.filter(code => codes[code]![0] === key);
  const byNumber = candidates.filter(code => codes[code]![1] === raw.keyCode);
  const code = Object.hasOwn(codes, raw.code) ? raw.code : byKey.find(preferred) ?? byKey[0] ?? byNumber.find(preferred) ?? byNumber[0] ?? raw.code;
  const [name,keyCode] = codes[code] ?? [raw.key,raw.keyCode];
  return {code,key:name,keyCode,location: /^(Shift|Control)Right$/.test(code) ? 2 : /^(Shift|Control)Left$/.test(code) ? 1 : code.startsWith('Numpad') ? 3 : 0};
}
function mapped(raw: HostedKeySpec, profile: string, aliases:Readonly<Record<string,string>>) {
  const original = spec(raw);
  let code = aliases[original.code];
  if (code && profile === 'th09-left') code = ({ArrowUp:'KeyT',ArrowDown:'KeyB',ArrowLeft:'KeyF',ArrowRight:'KeyH'} as Record<string,string>)[code] ?? code;
  if (code && profile === 'th09-right') code = ({ArrowUp:'Numpad8',ArrowDown:'Numpad2',ArrowLeft:'Numpad4',ArrowRight:'Numpad6',KeyZ:'ArrowDown',KeyX:'ArrowRight',ShiftLeft:'ArrowLeft'} as Record<string,string>)[code] ?? code;
  return code ? spec({code,key:'',keyCode:0}) : original;
}
/** One owner per physical key or virtual control. Only changes to the merged
 * canonical state cross into Runtime, so releasing K cannot release held Z. */
export class KeyboardMapping {
  private getAliases:()=>Readonly<Record<string,string>>;
  private local?:Th09LocalKeyboard;
  constructor(getAliases=()=>defaultKeyboardAliases,local?:Th09LocalInput){this.getAliases=getAliases;if(local)this.local=new Th09LocalKeyboard(local);}
  sampleLocal(seat:number,context:TouchRuntimeContext){return this.local?.sample(seat,context)??-1;}
  private physical = new HostedKeyboard(codes);
  private held = new Map<HostedKeySpec, HostedKeySpec>();
  private virtual = new Map<string,HostedKeySpec>();
  private published = new Map<string,HostedKeySpec>();
  private context: TouchRuntimeContext | null = null;
  supports(code: string): boolean { return Object.hasOwn(codes,code); }
  recognizes(event: HostedKeySpec): boolean { return this.physical.recognizes(event); }
  private publish(): CanonicalKeyTransition[] {
    const next = new Map([...this.held.values(),...this.virtual.values()].map(key=>[key.code,key]));
    const changes: CanonicalKeyTransition[] = [];
    for (const [code,key] of this.published) if (!next.has(code)) changes.push({...key,location:key.location??0,down:false});
    for (const [code,key] of next) if (!this.published.has(code)) changes.push({...key,location:key.location??0,down:true});
    this.published = next; return changes;
  }
  map(event: HostedKeyboardEvent, down: boolean, source: string, profile: string, context: TouchRuntimeContext): CanonicalKeyTransition[] | null {
    if (this.context?.target !== context.target || this.context?.game !== context.game || this.context?.epoch !== context.epoch) this.clear();
    this.context = context;
    if (!context.launched || context.spectator) return this.clear();
    if (source === 'virtual') {
      const key = spec(event);
      if (down) this.virtual.set(key.code,key); else this.virtual.delete(key.code);
      return this.publish();
    }
    if(profile==='th09-local')return this.local?.map(event,down,context)?[]:null;
    if (!this.physical.recognizes(event)) return null;
    const owners = this.physical.forward({...event,type:down?'keydown':'keyup'},context,!!event.launcherOwnsFocus);
    const orphaned: HostedKeySpec[] = [];
    for (const raw of owners) {
      if (down) {if (!this.held.has(raw)) this.held.set(raw,mapped(raw,profile,this.getAliases()));}
      else if (!this.held.delete(raw)) orphaned.push(mapped(raw,profile,this.getAliases()));
    }
    const changes = this.publish();
    for (const key of orphaned) if (!this.published.has(key.code)) changes.push({...key,location:key.location??0,down:false});
    return changes;
  }
  clear(): CanonicalKeyTransition[] {
    this.physical.clear(); this.held.clear(); this.virtual.clear();this.local?.clear();this.context = null;return this.publish();
  }
}
