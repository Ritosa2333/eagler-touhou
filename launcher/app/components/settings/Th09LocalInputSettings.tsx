import {useEffect,useState,useSyncExternalStore} from 'react';
import {localInputActions,type Th09LocalInput} from '../../../src/launcher/th09-local-input.mts';
import type {GamepadMappingModel} from '../../../src/launcher/gamepad-mapping.mts';
import {configurableKeyboardCode} from '../../../src/launcher/keyboard-preferences.mts';
import {useLocale} from '../../i18n';
import {MainSelect} from '../launcher/MainSelect';
import {InputBindingsTable,type InputAction} from './InputBindingsTable';
import {keyboardCodeName} from './KeyboardSettings';
import {GamepadSettings} from './GamepadSettings';

export function Th09LocalInputSettings({model,gamepad,active}:{model:Th09LocalInput;gamepad:GamepadMappingModel;active:boolean}){
  const state=useSyncExternalStore(model.subscribe,model.getSnapshot,model.getSnapshot),{locale}=useLocale(),zh=locale==='zh-CN';
  const [seat,setSeat]=useState(0),[learning,setLearning]=useState<InputAction|null>(null),[feedback,setFeedback]=useState(''),[naming,setNaming]=useState(false),[name,setName]=useState('');
  const p=state.players[seat]!,text=(cn:string,en:string)=>zh?cn:en;
  function result(error:string|null){setFeedback(error?(error==='gamepad'?text('这个手柄已分配给另一位玩家','This controller belongs to the other player'):text(`${keyboardCodeName(error)} 已被另一位玩家使用`,`${keyboardCodeName(error)} is used by the other player`)):'');}
  useEffect(()=>{setLearning(null);setFeedback('');setNaming(false);gamepad.cancelLearning();},[seat,gamepad]);
  useEffect(()=>{if(!active||!learning)return;const capture=(event:KeyboardEvent)=>{event.preventDefault();event.stopImmediatePropagation();if(event.repeat)return;
    if(event.code==='Escape'){setLearning(null);return;}if(event.altKey||event.metaKey||!configurableKeyboardCode(event.code)){setFeedback(text('此按键或组合键暂不支持','This key or chord is not supported'));return;}
    const error=model.bind(seat,learning,event.code);result(error);if(!error)setLearning(null);};
    document.addEventListener('keydown',capture,true);return()=>document.removeEventListener('keydown',capture,true);
  },[active,learning,model,seat,zh]);
  const bindings=Object.fromEntries(Object.entries(p.bindings).map(([a,keys])=>[a,keys.map(keyboardCodeName)])) as Record<InputAction,string[]>;
  return <div className="th09-local-input input-config">
    <label className="input-gamepad-enable"><input type="checkbox" checked={state.enabled} onChange={e=>model.setEnabled(e.target.checked)}/>{text('启用本地双人配置','Enable local two-player controls')}</label>
    <p className="input-config-hint">{text('仅本机双人对战生效，启用后接管双方设备与键位。','Only for local versus. Overrides both players’ in-game device settings.')}</p>
    <div className="input-device-tabs input-local-players" role="tablist" aria-label={text('本地玩家','Local player')}>{[0,1].map(n=><button type="button" key={n} role="tab" aria-selected={seat===n} tabIndex={seat===n?0:-1} onClick={()=>setSeat(n)} onKeyDown={e=>{if(e.key==='ArrowLeft'||e.key==='ArrowRight'){e.preventDefault();setSeat(1-seat);(e.currentTarget.parentElement?.children[1-seat] as HTMLButtonElement)?.focus();}}}>{text(`玩家 ${n+1}`,`Player ${n+1}`)}</button>)}</div>
    <div className="input-layout-toolbar"><label htmlFor="th09LocalDevice">{text('输入设备','Input device')}</label><MainSelect id="th09LocalDevice" className="option-select" aria-label={text('玩家输入设备','Player input device')} value={p.device} onChange={e=>{setLearning(null);result(model.setDevice(seat,e.target.value==='gamepad'?'gamepad':'keyboard'));}}><option value="keyboard">{text('键盘','Keyboard')}</option><option value="gamepad">{text('手柄','Gamepad')}</option></MainSelect></div>
    {p.device==='gamepad'?<GamepadSettings model={gamepad} active={active} assignment={{key:p.gamepad,onChange:key=>result(model.setDevice(seat,'gamepad',key))}}/>:<>
      <div className="input-layout-toolbar"><label htmlFor="th09LocalLayout">{text('键位布局','Layout')}</label><MainSelect id="th09LocalLayout" className="option-select" aria-label={text('双人键位布局','Two-player keyboard layout')} value={p.selected} onChange={e=>{setLearning(null);result(model.selectLayout(seat,e.target.value));}}>
        <option value="original">{text(seat===0?'原版左区':'原版右区',seat===0?'Original left':'Original right')}</option><option value="wasd">WASD</option><option value="arrows">{text('上下左右','Arrow keys')}</option>{p.selected==='custom'&&<option value="custom" disabled>{text('自定义','Custom')}</option>}{p.saved.map(s=><option key={s.id} value={s.id}>{s.name}</option>)}
      </MainSelect><button type="button" className="input-config-button" onClick={()=>{setLearning(null);setNaming(!naming);setName('');}}>{text('保存布局','Save layout')}</button></div>
      {naming&&<form className="input-save-layout" onSubmit={e=>{e.preventDefault();if(model.saveLayout(seat,name)){setNaming(false);setFeedback(text('布局已保存','Layout saved'));}}}><input autoFocus aria-label={text('双人布局名称','Two-player layout name')} maxLength={32} value={name} onChange={e=>setName(e.target.value)}/><button className="input-config-button primary" type="submit" disabled={!name.trim()}>{text('保存','Save')}</button><button className="input-config-button" type="button" onClick={()=>setNaming(false)}>{text('取消','Cancel')}</button></form>}
      <InputBindingsTable bindings={bindings} learning={learning} actions={localInputActions} names={{bomb:['快速攻击 / 取消','Quick attack / Cancel'],focus:['低速 / 蓄力','Focus / Charge']}} resetLabel={text('清除按键','Clear binding')} onBind={action=>{setNaming(false);setFeedback('');setLearning(learning===action?null:action);}} onReset={action=>{model.clear(seat,action);setLearning(null);}}/>
      <footer className="input-config-footer"><span>{learning?text('Esc 取消，按下新键即可完成','Press a key, or Esc to cancel'):state.persisted?text('按玩家自动保存','Saved automatically for each player'):text('仅在当前页面生效','Changes apply only to this page')}</span>{p.selected.startsWith('saved-')&&<button type="button" className="input-config-button" onClick={()=>model.deleteLayout(seat,p.selected)}>{text('删除布局','Delete layout')}</button>}<button type="button" className="input-config-button" onClick={()=>{setLearning(null);result(model.selectLayout(seat,'original'));}}>{text('恢复原版','Restore original')}</button></footer>
    </>}
    <p className="input-config-hint" role="status">{feedback}</p>
  </div>;
}
