import {Input} from '../../ui/input';
import {Button as UiButton} from '../../ui/button';
import {useEffect,useState,useSyncExternalStore} from 'react';
import {configurableKeyboardCode,keyboardActionCodes,type KeyboardPreferences} from '../../../src/launcher/keyboard-preferences.mts';
import {useLocale, type UiMessageKey} from '../../i18n';
import {MainSelect} from '../launcher/MainSelect';
import {InputBindingsTable,inputActionOrder,type InputAction} from './InputBindingsTable';
import {useInputCapture} from './input-capture';
export function keyboardCodeName(code:string){return ({ArrowUp:'↑',ArrowDown:'↓',ArrowLeft:'←',ArrowRight:'→',ShiftLeft:'Shift',ShiftRight:'R Shift',ControlLeft:'Ctrl',ControlRight:'R Ctrl',NumpadEnter:'Num Enter'} as Record<string,string>)[code]??code.replace(/^Key|^Digit/,'');}
export function KeyboardSettings({model,active=true}:{model:KeyboardPreferences;active?:boolean}){
  const aliases=useSyncExternalStore(model.subscribe,model.getSnapshot,model.getSnapshot);
  const layout=useSyncExternalStore(model.subscribe,model.getLayoutSnapshot,model.getLayoutSnapshot),{t}=useLocale();
  const [learning,setLearning]=useState<InputAction|null>(null),[unsupported,setUnsupported]=useState(false);
  const [naming,setNaming]=useState(false),[name,setName]=useState(''),[feedback,setFeedback]=useState<UiMessageKey|null>(null);
  useInputCapture(active&&learning!==null,()=>setLearning(null));
  useEffect(()=>{if(!active){setLearning(null);return;}if(!learning)return;
    const capture=(event:KeyboardEvent)=>{event.preventDefault();event.stopImmediatePropagation();if(event.repeat)return;
      if(event.code==='Escape'){setLearning(null);return;}if(event.altKey||event.metaKey||!configurableKeyboardCode(event.code)){setUnsupported(true);return;}
      model.bind(learning,event.code);setLearning(null);setUnsupported(false);setFeedback(null);};
    document.addEventListener('keydown',capture,true);return()=>document.removeEventListener('keydown',capture,true);
  },[active,learning,model]);
  const bindings={} as Record<InputAction,string[]>;
  for(const action of inputActionOrder){
    const target=keyboardActionCodes[action],extra=Object.entries(aliases).filter(([,value])=>value===target).map(([source])=>source);
    const original=aliases[target]&&aliases[target]!==target?[]:[target];
    bindings[action]=[...new Set([...extra,...original])].map(keyboardCodeName);
  }
  function save(){const id=model.saveLayout(name);if(id){setNaming(false);setName('');setFeedback('input.text.layout_saved');}else setFeedback(model.getLayoutSnapshot().persisted?'input.text.enter_a_name_up_to_32_layouts_can_be_saved':'input.text.the_browser_could_not_save_this_layout');}
  return <div className="keyboard-settings input-config">
    <div className="input-layout-toolbar"><label htmlFor="keyboardLayoutSelect">{t('input.text.layout')}</label>
      <MainSelect id="keyboardLayoutSelect" className="option-select" aria-label={t('input.text.keyboard_layout')} value={layout.selected} onChange={event=>{setLearning(null);setFeedback(null);model.selectLayout(event.target.value);}}>
        <optgroup label={t('input.text.presets')}><option value="arrows">{t('input.text.arrow_keys')}</option><option value="wasd">WASD</option><option value="custom">{t('input.text.custom')}</option></optgroup>
        {!!layout.saved.length&&<optgroup label={t('input.text.my_layouts')}>{layout.saved.map(item=><option key={item.id} value={item.id}>{item.name}</option>)}</optgroup>}
      </MainSelect>
      <UiButton size="sm" variant="outline" type="button" className="input-config-button" onClick={()=>{setLearning(null);setNaming(!naming);setName(layout.saved.find(item=>item.id===layout.selected)?.name??'');setFeedback(null);}}>{t('input.text.save_layout')}</UiButton>
    </div>    {naming&&<form className="input-save-layout" onSubmit={event=>{event.preventDefault();save();}}><Input aria-label={t('input.text.layout_name')} placeholder={t('input.text.layout_name')} value={name} maxLength={32} autoFocus onChange={event=>setName(event.target.value)}/><UiButton size="sm" variant="default" className="input-config-button primary" type="submit" disabled={!name.trim()}>{t('input.text.save')}</UiButton><UiButton size="sm" variant="outline" className="input-config-button" type="button" onClick={()=>setNaming(false)}>{t('replay.ui.cancel')}</UiButton></form>}
    <p className="input-config-hint">{t('input.text.click_a_binding_to_change_it_new_keys_take_priority_original_keys')}</p>
    <InputBindingsTable bindings={bindings} learning={learning} resetLabel={t('input.text.restore_original_binding')} onBind={action=>{setNaming(false);setUnsupported(false);setLearning(learning===action?null:action);}} onReset={action=>{model.clear(action);setLearning(null);setFeedback(null);}}/>
    <footer className="input-config-footer"><span role="status">{unsupported?t('input.text.this_key_or_chord_is_not_supported_esc_cancels'):learning?t('input.text.press_a_new_key_or_esc_to_cancel'):feedback?t(feedback): (layout.persisted?t('input.text.changes_save_automatically'):t('input.text.changes_apply_only_to_this_page'))}</span>
      <div className="input-config-actions">{layout.selected.startsWith('saved-')&&<UiButton size="sm" variant="outline" className="input-config-button" type="button" onClick={()=>{model.deleteLayout(layout.selected);setFeedback('input.text.saved_layout_deleted_current_bindings_are_retained');}}>{t('input.text.delete_layout')}</UiButton>}
      <UiButton size="sm" variant="outline" className="input-config-button" type="button" onClick={()=>{model.reset();setLearning(null);setNaming(false);setFeedback(null);}}>{t('input.text.reset_preset')}</UiButton></div>
    </footer>
  </div>;
}
