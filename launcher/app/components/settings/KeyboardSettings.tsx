import {useEffect,useState,useSyncExternalStore} from 'react';
import {configurableKeyboardCode,keyboardActionCodes,type KeyboardPreferences} from '../../../src/launcher/keyboard-preferences.mts';
import {useLocale} from '../../i18n';
import {MainSelect} from '../launcher/MainSelect';
import {InputBindingsTable,inputActionOrder,type InputAction} from './InputBindingsTable';
export function keyboardCodeName(code:string){return ({ArrowUp:'↑',ArrowDown:'↓',ArrowLeft:'←',ArrowRight:'→',ShiftLeft:'Shift',ShiftRight:'R Shift',ControlLeft:'Ctrl',ControlRight:'R Ctrl',NumpadEnter:'Num Enter'} as Record<string,string>)[code]??code.replace(/^Key|^Digit/,'');}
export function KeyboardSettings({model,active=true}:{model:KeyboardPreferences;active?:boolean}){
  const aliases=useSyncExternalStore(model.subscribe,model.getSnapshot,model.getSnapshot);
  const layout=useSyncExternalStore(model.subscribe,model.getLayoutSnapshot,model.getLayoutSnapshot),{locale}=useLocale(),zh=locale==='zh-CN';
  const [learning,setLearning]=useState<InputAction|null>(null),[unsupported,setUnsupported]=useState(false);
  const [naming,setNaming]=useState(false),[name,setName]=useState(''),[feedback,setFeedback]=useState('');
  const text=(cn:string,en:string)=>zh?cn:en;
  useEffect(()=>{if(!active){setLearning(null);return;}if(!learning)return;
    const capture=(event:KeyboardEvent)=>{event.preventDefault();event.stopImmediatePropagation();if(event.repeat)return;
      if(event.code==='Escape'){setLearning(null);return;}if(event.altKey||event.metaKey||!configurableKeyboardCode(event.code)){setUnsupported(true);return;}
      model.bind(learning,event.code);setLearning(null);setUnsupported(false);setFeedback('');};
    document.addEventListener('keydown',capture,true);return()=>document.removeEventListener('keydown',capture,true);
  },[active,learning,model]);
  const bindings={} as Record<InputAction,string[]>;
  for(const action of inputActionOrder){
    const target=keyboardActionCodes[action],extra=Object.entries(aliases).filter(([,value])=>value===target).map(([source])=>source);
    const original=aliases[target]&&aliases[target]!==target?[]:[target];
    bindings[action]=[...new Set([...extra,...original])].map(keyboardCodeName);
  }
  function save(){const id=model.saveLayout(name);if(id){setNaming(false);setName('');setFeedback(text('布局已保存','Layout saved'));}else setFeedback(model.getLayoutSnapshot().persisted?text('请输入名称，最多保存 32 套布局','Enter a name. Up to 32 layouts can be saved.'):text('浏览器未能保存布局','The browser could not save this layout'));}
  return <div className="keyboard-settings input-config">
    <div className="input-layout-toolbar"><label htmlFor="keyboardLayoutSelect">{text('键位布局','Layout')}</label>
      <MainSelect id="keyboardLayoutSelect" className="option-select" aria-label={text('键位布局','Keyboard layout')} value={layout.selected} onChange={event=>{setLearning(null);setFeedback('');model.selectLayout(event.target.value);}}>
        <optgroup label={text('预设','Presets')}><option value="arrows">{text('上下左右','Arrow keys')}</option><option value="wasd">WASD</option><option value="custom">{text('自定义','Custom')}</option></optgroup>
        {!!layout.saved.length&&<optgroup label={text('我的布局','My layouts')}>{layout.saved.map(item=><option key={item.id} value={item.id}>{item.name}</option>)}</optgroup>}
      </MainSelect>
      <button type="button" className="input-config-button" onClick={()=>{setLearning(null);setNaming(!naming);setName(layout.saved.find(item=>item.id===layout.selected)?.name??'');setFeedback('');}}>{text('保存布局','Save layout')}</button>
    </div>
    {naming&&<form className="input-save-layout" onSubmit={event=>{event.preventDefault();save();}}><input aria-label={text('布局名称','Layout name')} placeholder={text('布局名称','Layout name')} value={name} maxLength={32} autoFocus onChange={event=>setName(event.target.value)}/><button className="input-config-button primary" type="submit" disabled={!name.trim()}>{text('保存','Save')}</button><button className="input-config-button" type="button" onClick={()=>setNaming(false)}>{text('取消','Cancel')}</button></form>}
    <p className="input-config-hint">{text('点击按键修改。新键位优先，原键位同时可用。','Click a binding to change it. New keys take priority; original keys remain available.')}</p>
    <InputBindingsTable bindings={bindings} learning={learning} resetLabel={text('恢复原键位','Restore original binding')} onBind={action=>{setNaming(false);setUnsupported(false);setLearning(learning===action?null:action);}} onReset={action=>{model.clear(action);setLearning(null);setFeedback('');}}/>
    <footer className="input-config-footer"><span role="status">{unsupported?text('此按键或组合键暂不支持，Esc 取消','This key or chord is not supported. Esc cancels.'):learning?text('Esc 取消，按下新键即可完成','Press a new key, or Esc to cancel'):feedback|| (layout.persisted?text('更改自动保存','Changes save automatically'):text('仅在当前页面生效','Changes apply only to this page'))}</span>
      {layout.selected.startsWith('saved-')&&<button className="input-config-button" type="button" onClick={()=>{model.deleteLayout(layout.selected);setFeedback(text('已删除保存的布局，当前键位仍保留','Saved layout deleted. Current bindings are retained.'));}}>{text('删除布局','Delete layout')}</button>}
      <button className="input-config-button" type="button" onClick={()=>{model.reset();setLearning(null);setNaming(false);setFeedback('');}}>{text('恢复预设','Reset preset')}</button>
    </footer>
  </div>;
}
