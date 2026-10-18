// The Launcher owns remapping. Sample its semantic pad snapshot on the game
// tick, so a browser rendering frame never becomes an authoritative input tick.
export class ManagedInput {
  constructor(environment=window){this.environment=environment;this.epoch=Number(new URL(environment.location.href).searchParams.get('runtimeEpoch'));}
  owner(name,schema){
    const owner=this.environment[name];
    return owner&&owner.schema===schema&&owner.protocol==='eagler-touhou/1'&&owner.game==='th12'&&owner.epoch===this.epoch&&owner.origin===this.environment.location.origin?owner:null;
  }
  key(raw,down,source,dispatch){
    const owner=this.owner('__eaglerKeyboardMapping','eagler-touhou/keyboard-input/1');
    if(typeof owner?.map!=='function')return false;
    const events=owner.map({code:raw.code,key:raw.key??'',keyCode:raw.keyCode??0,location:raw.location??0,repeat:!!raw.repeat,altKey:!!raw.altKey,metaKey:!!raw.metaKey,ctrlKey:!!raw.ctrlKey,launcherOwnsFocus:!!raw.launcherOwnsFocus},down,source,'standard');
    if(events===null)return false;
    if(!Array.isArray(events)||events.some(event=>typeof event?.code!=='string'||typeof event.down!=='boolean'))throw Error('启动器键盘映射无效');
    for(const event of events)dispatch(event.code,event.down,'host');return true;
  }
  pad(){
    const owner=this.owner('__eaglerGamepadMapping','eagler-touhou/gamepad-input/1');
    if(typeof owner?.enabled!=='function'||!owner.enabled())return null;
    if(typeof owner.sample!=='function')throw Error('启动器手柄映射无效');
    const actions=owner.sample(0);if(!Number.isInteger(actions)||actions<0||actions>4095)return 0;
    const native=[1,2,8,256,16,32,64,128,512,0x80000,4,0x200000];
    return native.reduce((mask,bit,index)=>mask|((actions&(1<<index))?bit:0),0);
  }
  clear(){
    for(const [name,schema]of [['__eaglerKeyboardMapping','eagler-touhou/keyboard-input/1'],['__eaglerGamepadMapping','eagler-touhou/gamepad-input/1']])this.owner(name,schema)?.clear?.();
  }
}
