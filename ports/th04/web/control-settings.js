// Device preferences stay on this browser; only logical action bits go online.
export const actions=[['up','上',1],['down','下',2],['left','左',4],['right','右',8],['bomb','炸弹 / 返回',16],['shot','射击',32],['focus','低速 / 救援',64],['pause','暂停 / 继续',128],['confirm','确认',256]];
export const syntheticInputEvents=new WeakSet();
const storageKey='th04.controls.v1';
const primary={up:['ArrowUp'],down:['ArrowDown'],left:['ArrowLeft'],right:['ArrowRight'],bomb:['KeyX'],shot:['KeyZ'],focus:['ShiftLeft','ShiftRight'],pause:['Escape','KeyP'],confirm:['Enter']};
const secondary={up:['KeyW'],down:['KeyS'],left:['KeyA'],right:['KeyD'],bomb:['KeyL'],shot:['KeyJ'],focus:['KeyK'],pause:[],confirm:[]};
const buttons={up:12,down:13,left:14,right:15,bomb:1,shot:0,focus:5,pause:9,confirm:-1};
const defaults={local1:{keys:primary,pad:buttons},local2:{keys:secondary,pad:buttons},online:{keys:primary,pad:buttons},solo:{keys:primary,pad:buttons}};
const clone=value=>JSON.parse(JSON.stringify(value));
const validCode=code=>typeof code==='string'&&/^(Key[A-Z]|Digit[0-9]|Arrow(Up|Down|Left|Right)|Shift(Left|Right)|Control(Left|Right)|Alt(Left|Right)|Space|Enter|Escape|Tab|Backspace|CapsLock|Backquote|Minus|Equal|BracketLeft|BracketRight|Backslash|Semicolon|Quote|Comma|Period|Slash|Insert|Delete|Home|End|PageUp|PageDown|Numpad[0-9]|Numpad(Add|Subtract|Multiply|Divide|Decimal|Enter)|F([1-9]|1[0-2]))$/.test(code);
function validate(value){
  const result=clone(defaults);
  if(![1,2].includes(value?.version))return result;
  for(const id of Object.keys(defaults)){
    const source=value.profiles?.[id];if(!source)continue;
    const used=new Set();
    // Invalid or conflicting stored keyboard data falls back as a whole profile.
    if(actions.every(([a])=>Array.isArray(source.keys?.[a])&&source.keys[a].length<=2&&source.keys[a].every(code=>{if(!validCode(code)||used.has(code))return false;used.add(code);return true;})))result[id].keys=clone(source.keys);
    const padUsed=new Set();
    if(actions.every(([a])=>{const n=source.pad?.[a];if(!Number.isInteger(n)||n< -1||n>31||n>=0&&padUsed.has(n))return false;if(n>=0)padUsed.add(n);return true;}))result[id].pad=clone(source.pad);
  }
  // A physical key cannot own both local seats.
  const first=new Set(Object.values(result.local1.keys).flat());
  if(Object.values(result.local2.keys).flat().some(code=>first.has(code))){result.local1=clone(defaults.local1);result.local2=clone(defaults.local2);}
  // Upgrade old Esc-only defaults only when P is free. Explicit v2 bindings,
  // custom pause keys and the other local player's keys remain untouched.
  if(value.version===1)for(const id of ['local1','online','solo']){
    const keys=result[id].keys,owners=id==='local1'?[keys,result.local2.keys]:[keys];
    if(keys.pause.length===1&&keys.pause[0]==='Escape'&&!owners.some(map=>Object.values(map).flat().includes('KeyP')))keys.pause.push('KeyP');
  }
  return result;
}
let profiles=clone(defaults);
try{profiles=validate(JSON.parse(localStorage.getItem(storageKey)));}catch{}
export function controlSnapshot(){return {version:2,profiles:clone(profiles)};}
export function useControlSnapshot(value){profiles=validate(value);}
export function keyboardBits(keys,id='online'){
  let bits=0;for(const [action,,bit]of actions)if(profiles[id].keys[action].some(code=>keys.has(code)))bits|=bit;return bits;
}
export function isBound(code,id='online'){return actions.some(([a])=>profiles[id].keys[a].includes(code));}
export function padBits(pad,id='online',deadzone=.3){
  if(!pad)return 0;
  let bits=0;for(const [action,,bit]of actions)if(pad.buttons[profiles[id].pad[action]]?.pressed)bits|=bit;
  const x=pad.axes[0]||0,y=pad.axes[1]||0;
  return bits|(y< -deadzone?1:0)|(y>deadzone?2:0)|(x< -deadzone?4:0)|(x>deadzone?8:0);
}
export function isFormTarget(target){return !!target?.closest?.('input,select,textarea,button,[contenteditable="true"]');}
export function keyLabel(code){return ({ArrowUp:'↑',ArrowDown:'↓',ArrowLeft:'←',ArrowRight:'→',ShiftLeft:'左 Shift',ShiftRight:'右 Shift',ControlLeft:'左 Ctrl',ControlRight:'右 Ctrl',AltLeft:'左 Alt',AltRight:'右 Alt',Space:'空格',Escape:'Esc'})[code]||code.replace(/^Key|^Digit/,'');}
export function controlsSummary(id){return actions.map(([a,label])=>`${id==='solo'&&a==='focus'?'低速':label}：${profiles[id].keys[a].map(keyLabel).join(' / ')||'未绑定'}`).join(' · ');}

export function mountControlSettings(container,{local=false,solo=false,onChange=()=>{},onEditing=()=>{}}={}){
  const trigger=document.createElement('button');trigger.textContent='自定义操作';trigger.type='button';
  const dialog=document.createElement('dialog');dialog.className='control-dialog';
  dialog.innerHTML='<h2>自定义操作</h2><p>点击键位后按下新按键。Esc 取消录入；要绑定 Esc，请点「设为 Esc」。重复键位会提示冲突。</p><label>操作对象 <select class="profile"></select></label><div class="control-rows"></div><p>手柄左摇杆保持移动；按钮可修改。手柄用于本地 P2，联机时控制自己所选的座位。</p><p class="control-message" role="status" aria-live="polite"></p><div class="control-footer"><button type="button" class="reset">恢复当前默认</button><button type="button" class="save">保存</button><button type="button" class="cancel">取消</button></div><small>仅保存在当前浏览器及网址。游戏不会因打开设置自动暂停，请先暂停再修改。</small>';
  if(solo)dialog.querySelectorAll('p')[1].textContent='键盘、手柄与触屏均控制单机玩家。手柄左摇杆保持移动；按钮可修改。';
  container.append(trigger,dialog);
  const pauseHint=document.createElement('small');
  pauseHint.textContent='Esc 暂停／继续，P 为备用键；F11 切换浏览器全屏，右上角按钮退出游戏全屏布局。';
  container.append(pauseHint);
  const select=dialog.querySelector('.profile'),rows=dialog.querySelector('.control-rows'),message=dialog.querySelector('.control-message');
  for(const [id,label]of local?[['local1','本地 P1 · 键盘'],['local2','本地 P2 · 键盘 / 手柄']]:solo?[['solo','单机 · 键盘 / 手柄']]:[['online','联机 · 当前玩家']])select.add(new Option(label,id));
  let draft,pending=null;
  const conflict=(id,action,code)=>{
    for(const profile of local?['local1','local2']:[solo?'solo':'online'])for(const [a,label]of actions)if((profile!==id||a!==action)&&draft[profile].keys[a].includes(code))return `${profile==='local1'?'本地 P1':profile==='local2'?'本地 P2':'当前玩家'}的「${label}」`;
    return '';
  };
  function bind(action,code){
    const owner=conflict(select.value,action,code);
    if(owner){message.textContent=`${keyLabel(code)} 已用于${owner}，请先修改或清除原绑定。`;return;}
    draft[select.value].keys[action]=[code];pending=null;message.textContent='已修改，点击保存后生效。';render();
  }
  function render(){
    rows.replaceChildren();
    const id=select.value;
    for(const [action,label]of actions){
      const row=document.createElement('div');row.className='control-row';
      const name=document.createElement('span');name.textContent=solo&&action==='focus'?'低速':label;
      const key=document.createElement('button');key.type='button';key.textContent=pending===action?'请按新键…':draft[id].keys[action].map(keyLabel).join(' / ')||'未绑定';key.setAttribute('aria-label',`${label}键位`);
      key.onclick=()=>{pending=action;message.textContent='按下一个键（不支持组合快捷键），Esc 取消录入。';render();};
      const esc=document.createElement('button');esc.type='button';esc.textContent='设为 Esc';esc.onclick=()=>bind(action,'Escape');
      const clear=document.createElement('button');clear.type='button';clear.textContent='清除';clear.onclick=()=>{draft[id].keys[action]=[];pending=null;render();};
      row.append(name,key,esc,clear);
      if(id!=='local1'){
        const pad=document.createElement('select');pad.setAttribute('aria-label',`${label}手柄按钮`);
        const names={0:'A / ×',1:'B / ○',2:'X / □',3:'Y / △',4:'LB / L1',5:'RB / R1',6:'LT / L2',7:'RT / R2',8:'Back / Select',9:'Start',10:'左摇杆按下',11:'右摇杆按下',12:'十字键上',13:'十字键下',14:'十字键左',15:'十字键右',16:'Home'};
        pad.add(new Option('手柄：未绑定','-1'));
        for(let n=0;n<32;n++)pad.add(new Option(`手柄 ${names[n]||`按钮 ${n}`}`,String(n)));
        pad.value=draft[id].pad[action];pad.onchange=()=>{
          const n=Number(pad.value),other=actions.find(([a])=>a!==action&&n>=0&&draft[id].pad[a]===n);
          if(other){message.textContent=`这个手柄按钮已用于「${other[1]}」，请先清除原绑定。`;pad.value=draft[id].pad[action];return;}
          draft[id].pad[action]=n;message.textContent='已修改，点击保存后生效。';
        };row.append(pad);
      }
      rows.append(row);
    }
  }
  trigger.onclick=()=>{draft=clone(profiles);pending=null;message.textContent='';render();dialog.showModal();onEditing(true);};
  select.onchange=()=>{pending=null;message.textContent='';render();};
  dialog.querySelector('.reset').onclick=()=>{
    const id=select.value;
    // Reject reset if the other local seat now uses this profile's default keys.
    for(const [a]of actions)for(const code of defaults[id].keys[a]){
      const other=local?(id==='local1'?'local2':'local1'):null;
      if(other&&Object.values(draft[other].keys).flat().includes(code)){message.textContent=`默认键 ${keyLabel(code)} 已被另一位本地玩家使用，请先清除该绑定。`;return;}
    }
    draft[id]=clone(defaults[id]);pending=null;message.textContent='已恢复默认，点击保存后生效。';render();
  };
  dialog.querySelector('.save').onclick=()=>{
    profiles=clone(draft);let saved=true;
    try{localStorage.setItem(storageKey,JSON.stringify(controlSnapshot()));}catch{saved=false;}
    pending=null;dialog.close();onChange(controlSnapshot());
    container.querySelector('.control-save-status')?.remove();
    const status=document.createElement('span');status.className='control-save-status';status.setAttribute('role','status');status.textContent=saved?' 操作设置已保存':' 已应用；浏览器禁止保存，刷新后会恢复。';container.append(status);
  };
  dialog.querySelector('.cancel').onclick=()=>dialog.close();
  dialog.addEventListener('close',()=>{pending=null;onEditing(false);});
  // Capture before game/SDL listeners. Keyup is swallowed too, including the
  // key that just completed a binding, so editing never fires a game action.
  window.addEventListener('keydown',event=>{
    if(!dialog.open||syntheticInputEvents.has(event))return;
    event.stopImmediatePropagation();
    if(!pending)return;
    event.preventDefault();if(event.repeat)return;
    if(event.code==='Escape'){pending=null;message.textContent='已取消录入。';render();return;}
    if(!validCode(event.code)||event.metaKey||((event.ctrlKey||event.altKey)&&! /^(Control|Alt)/.test(event.code))){message.textContent='请选择单个键；不支持此键或组合快捷键。';return;}
    bind(pending,event.code);
  },true);
  window.addEventListener('keyup',event=>{if(dialog.open&&!syntheticInputEvents.has(event))event.stopImmediatePropagation();},true);
  return {isEditing:()=>dialog.open};
}
