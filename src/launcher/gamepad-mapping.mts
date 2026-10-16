/** Browser controls are persisted only inside their device/mapping profile.
 * Runtime receives semantic actions, never raw device button numbers. */
export const gamepadMappingSchema = 'eagler-touhou/gamepad-input/1';
export const gamepadActions = ['shoot','bomb','focus','pause','up','down','left','right','skip','confirm','action','restart'] as const;
export type GamepadAction = typeof gamepadActions[number];
export type GamepadControl = {kind:'button'; index:number} | {kind:'axis'; index:number; rest:number; value:number};
export interface GamepadDevice {id:string; index:number; connected:boolean; mapping:string; buttons:readonly {pressed:boolean; value:number}[]; axes:readonly number[]}
export type GamepadBindings = Record<GamepadAction, GamepadControl[]>;
export interface GamepadProfile {bindings:GamepadBindings; threshold:number}
export interface GamepadChoice {key:string; name:string; index:number; standard:boolean}
export interface GamepadMappingSnapshot {enabled:boolean; seats:readonly string[]; profiles:Readonly<Record<string,GamepadProfile>>; devices:readonly GamepadChoice[]; supported:boolean; learning:GamepadAction|null; active:number;deviceActive:Readonly<Record<string,number>>}
const storageKey='eagler-touhou-gamepad-mapping-v1';
const emptyBindings=():GamepadBindings=>({shoot:[],bomb:[],focus:[],pause:[],up:[],down:[],left:[],right:[],skip:[],confirm:[],action:[],restart:[]});
export function defaultGamepadProfile(standard:boolean):GamepadProfile {
  const bindings=emptyBindings();
  if(standard){
    for(const [action,index] of Object.entries({shoot:0,bomb:1,focus:4,pause:9,skip:5,action:2})) bindings[action as GamepadAction]=[{kind:'button',index}];
    for(const [action,index,axis,value] of [['up',12,1,-1],['down',13,1,1],['left',14,0,-1],['right',15,0,1]] as const)
      bindings[action]=[{kind:'button',index},{kind:'axis',index:axis,rest:0,value}];
  }
  return {bindings,threshold:.5};
}
function controlsValid(value:unknown):value is GamepadControl[] {
  return Array.isArray(value)&&value.length<=4&&value.every(c=>c&&Number.isInteger(c.index)&&c.index>=0&&c.index<256&&
    (c.kind==='button'||c.kind==='axis'&&Number.isFinite(c.rest)&&Math.abs(c.rest)<=1&&Number.isFinite(c.value)&&Math.abs(c.value)<=1&&Math.abs(c.value-c.rest)>=.25));
}
export function gamepadActionBits(pad:GamepadDevice,profile:GamepadProfile):number {
  let bits=0;
  gamepadActions.forEach((action,n)=>{if(profile.bindings[action].some(c=>c.kind==='button'?
    !!pad.buttons[c.index]&&(Number.isFinite(pad.buttons[c.index]!.value)?pad.buttons[c.index]!.value>=profile.threshold:pad.buttons[c.index]!.pressed):
    Number.isFinite(pad.axes[c.index])&&(pad.axes[c.index]!-c.rest)/(c.value-c.rest)>=profile.threshold))bits|=1<<n;});
  return bits;
}
export interface GamepadMappingModel {
  subscribe(listener:()=>void):()=>void; getSnapshot():GamepadMappingSnapshot;
  refresh():void; setEnabled(value:boolean):void; select(seat:number,key:string):void;
  bind(key:string,action:GamepadAction,controls:GamepadControl[]):void; reset(key:string):void; setThreshold(key:string,value:number):void;
  learn(key:string,action:GamepadAction):void; cancelLearning():void; suspend():void;
  sample(seat:number,allowed:boolean):number;
  sampleDevice(seat:number,key:string,allowed:boolean):number;
}
/** No rendering timer feeds gameplay. sample() reads a fresh device snapshot
 * synchronously on the Runtime's input stack. refresh() is only for the editor. */
export function createGamepadMappingModel({storage,getGamepads}: {
  storage:Pick<Storage,'getItem'|'setItem'>|null; getGamepads?:()=>readonly (GamepadDevice|null)[];
}):GamepadMappingModel {
  const listeners=new Set<()=>void>();
  let enabled=false,seats=['',''],profiles:Record<string,GamepadProfile>={},devices:GamepadChoice[]=[],active=0,deviceActive:Record<string,number>={};
  let learning:{key:string;action:GamepadAction;buttons:boolean[];axes:number[];released:boolean}|null=null;
  const blocked=new Set([0,1]); const identities=new Map<number,string>();
  try {const saved=JSON.parse(storage?.getItem(storageKey)||'null');
    if(saved?.version===1){enabled=saved.enabled===true;if(Array.isArray(saved.seats)&&saved.seats.length===2&&saved.seats.every((v:unknown)=>typeof v==='string'))seats=saved.seats;
      if(saved.profiles&&typeof saved.profiles==='object')for(const [key,raw] of Object.entries(saved.profiles) as [string,GamepadProfile][])
        if(key.length<=1024&&raw&&Number.isFinite(raw.threshold)&&raw.threshold>=.1&&raw.threshold<=.9&&gamepadActions.every(a=>controlsValid(raw.bindings?.[a])))profiles[key]=raw;
    }
  }catch { /* A bad profile cannot become device input. */ }
  function read(){try{return [...(getGamepads?.()??[])].filter((p):p is GamepadDevice=>!!p&&p.connected&&!/keyboard|\bkb\b/i.test(p.id));}catch{return [];}}
  // Preserve the occurrence of every still-connected index. Filtering out a
  // disconnected twin must not rename its surviving peer and transfer a seat.
  const connectedIdentities=new Map<number,{base:string;ordinal:number}>();
  function enumerate(pads:GamepadDevice[]){
    const bases=new Map(pads.map(p=>[p.index,JSON.stringify([p.id,p.mapping,p.buttons.length,p.axes.length])]));
    for(const [index,identity] of connectedIdentities)if(bases.get(index)!==identity.base)connectedIdentities.delete(index);
    const used=new Map<string,Set<number>>();
    for(const {base,ordinal} of connectedIdentities.values()){if(!used.has(base))used.set(base,new Set());used.get(base)!.add(ordinal);}
    return pads.map(p=>{const base=bases.get(p.index)!;let identity=connectedIdentities.get(p.index);
      if(!identity){const ordinals=used.get(base)??new Set<number>();let ordinal=0;while(ordinals.has(ordinal))ordinal++;ordinals.add(ordinal);used.set(base,ordinals);identity={base,ordinal};connectedIdentities.set(p.index,identity);}
      return {key:base+'#'+identity.ordinal,name:p.id,index:p.index,standard:p.mapping==='standard'};
    });
  }
  let snapshot:GamepadMappingSnapshot;
  function publish(persist=false){snapshot={enabled,seats:[...seats],profiles:{...profiles},devices,supported:!!getGamepads,learning:learning?.action??null,active,deviceActive};
    if(persist)try{storage?.setItem(storageKey,JSON.stringify({version:1,enabled,seats,profiles}));}catch{}
    for(const listener of listeners)listener();}
  function profile(key:string,standard=devices.find(d=>d.key===key)?.standard===true){return profiles[key]??defaultGamepadProfile(standard);}
  function suspend(){blocked.add(0);blocked.add(1);}
  function mutate(key:string,value:GamepadProfile){profiles={...profiles,[key]:value};suspend();publish(true);}
  function refresh(){
    const pads=read(),next=enumerate(pads);let changed=JSON.stringify(next)!==JSON.stringify(devices);devices=next;
    if(learning){const pad=pads.find(p=>devices.find(d=>d.index===p.index)?.key===learning!.key);
      if(!pad){learning=null;changed=true;}else{
        // Arm only after buttons held while opening the editor have released.
        if(!learning.released){if(!pad.buttons.some(b=>b.pressed||b.value>.5)){learning.released=true;learning.axes=[...pad.axes];}}
        else {let control:GamepadControl|undefined;
          const button=pad.buttons.findIndex((b,i)=>(b.pressed||b.value>.5)&&!learning!.buttons[i]);
          if(button>=0)control={kind:'button',index:button};
          else {const axis=pad.axes.findIndex((v,i)=>Math.abs(v-(learning!.axes[i]??v))>=.5);
            if(axis>=0)control={kind:'axis',index:axis,rest:learning.axes[axis]!,value:pad.axes[axis]!>learning.axes[axis]!?1:-1};}
          if(control){const {key,action}=learning;learning=null;const current=profile(key);mutate(key,{...current,bindings:{...current.bindings,[action]:[control]}});return;}
        }
        if(learning)learning.buttons=pad.buttons.map(b=>b.pressed||b.value>.5);
      }
    }
    const preview=seats.reduce((bits,key)=>{const choice=devices.find(d=>d.key===key),pad=pads.find(p=>p.index===choice?.index);return bits|(pad?gamepadActionBits(pad,profile(key)):0);},0);
    const nextActive=Object.fromEntries(devices.map(d=>[d.key,gamepadActionBits(pads.find(p=>p.index===d.index)!,profile(d.key))]));
    if(JSON.stringify(nextActive)!==JSON.stringify(deviceActive)){deviceActive=nextActive;changed=true;}
    if(preview!==active){active=preview;changed=true;}if(changed)publish();
  }
  function sampleDevice(seat:number,key:string,allowed:boolean){
    if(seat!==0&&seat!==1)return 0;
    if(!allowed||learning){blocked.add(seat);return 0;}
    const pads=read(),choices=enumerate(pads),choice=choices.find(d=>d.key===key),pad=pads.find(p=>p.index===choice?.index);
    if(!pad||!choice){blocked.add(seat);identities.delete(seat);return 0;}
    const identity=choice.key+'@'+choice.index;if(identities.get(seat)!==identity){identities.set(seat,identity);blocked.add(seat);}
    const bits=gamepadActionBits(pad,profile(choice.key,choice.standard));
    if(blocked.has(seat)){if(!bits)blocked.delete(seat);return 0;}return bits;
  }
  publish();
  return {
    subscribe(listener){listeners.add(listener);return()=>{listeners.delete(listener);};},getSnapshot:()=>snapshot,refresh,
    setEnabled(value){enabled=value;suspend();publish(true);},
    select(seat,key){if(seat!==0&&seat!==1)return;if(key&&!devices.some(d=>d.key===key))return;
      seats[seat]=key;if(key){if(seats[1-seat]===key)seats[1-seat]='';if(!profiles[key])profiles={...profiles,[key]:profile(key)};}suspend();publish(true);},
    bind(key,action,controls){if(!controlsValid(controls))throw Error('Invalid gamepad binding');const current=profile(key);mutate(key,{...current,bindings:{...current.bindings,[action]:controls}});},
    reset(key){mutate(key,defaultGamepadProfile(devices.find(d=>d.key===key)?.standard===true));},
    setThreshold(key,value){if(!Number.isFinite(value)||value<.1||value>.9)return;mutate(key,{...profile(key),threshold:value});},
    learn(key,action){refresh();const choice=devices.find(d=>d.key===key),pad=read().find(p=>p.index===choice?.index);if(!pad)return;
      learning={key,action,buttons:pad.buttons.map(b=>b.pressed||b.value>.5),axes:[...pad.axes],released:false};suspend();publish();},
    cancelLearning(){learning=null;suspend();publish();},suspend,
    sample(seat,allowed){return enabled?sampleDevice(seat,seats[seat]??'',allowed):-1;},sampleDevice,
  };
}
