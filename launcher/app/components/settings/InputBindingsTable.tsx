import {RotateCcw} from 'lucide-react';
import {Button as UiButton} from '../../ui/button';
import {useLocale} from '../../i18n';
import type {keyboardActionCodes} from '../../../src/launcher/keyboard-preferences.mts';
export type InputAction=keyof typeof keyboardActionCodes;
export const inputActionOrder:readonly InputAction[]=['up','down','left','right','shoot','bomb','focus','action','pause','confirm','skip','restart'];
export const inputActionNames={shoot:'input.action.shoot',bomb:'input.action.bomb',focus:'input.action.focus',pause:'input.action.pause',up:'input.action.up',down:'input.action.down',left:'input.action.left',right:'input.action.right',skip:'input.action.skip',confirm:'input.action.confirm',action:'input.action.action',restart:'input.action.restart'} as const;

/** Keyboard and controller use the same familiar action/binding table. */
export function InputBindingsTable({bindings,learning,onBind,onReset,resetLabel,isActive,actions=inputActionOrder,names}: {
  bindings:Record<InputAction,readonly string[]>;learning:InputAction|null;onBind(action:InputAction):void;
  onReset(action:InputAction):void;resetLabel:string;isActive?(action:InputAction):boolean;
  actions?:readonly InputAction[];names?:Partial<Record<InputAction,string>>;
}){
  const {t}=useLocale();
  return <table className="input-binding-table">
    <thead><tr><th scope="col">{t('input.text.action')}</th><th scope="col">{t('input.text.binding')}</th><th scope="col"><span className="input-visually-hidden">{resetLabel}</span></th></tr></thead>
    <tbody>{actions.map(action=>{
      const name=names?.[action]??t(inputActionNames[action]),keys=bindings[action],waiting=learning===action;
      return <tr key={action} data-action={action} data-learning={waiting} data-active={!!isActive?.(action)} className={action==='shoot'||action==='pause'?'input-action-divider':undefined}>
        <th scope="row">{name}</th><td><UiButton variant="ghost" type="button" className="input-binding-button h-auto justify-start gap-1.5 whitespace-normal border-0 px-0 py-1" aria-label={waiting?t('input.bindingListening',{name,hint:t('input.text.waiting_for_input_click_to_cancel')}):t('input.bindingChange',{name,keys:keys.join(' / ')||t('input.text.unbound'),hint:t('input.text.change_binding')})} aria-pressed={waiting} onClick={()=>onBind(action)} onKeyDown={event=>{if(!waiting&&event.key==='Delete'){event.preventDefault();onReset(action);}}}>
          {/* biome-ignore lint/suspicious/noArrayIndexKey: Keycaps are stateless ordered display positions; duplicate human-readable binding labels are valid. */}
          {waiting?<span className="input-binding-listening">{t('input.text.press_a_control')}</span>:keys.length?keys.map((key,n)=><kbd key={n} className={n?'input-keycap secondary':'input-keycap'}>{key}</kbd>):<span className="input-binding-empty">—</span>}
        </UiButton></td><td><UiButton variant="ghost" size="icon" type="button" className="input-binding-reset" title={`${resetLabel} | ${name}`} aria-label={`${resetLabel} | ${name}`} onClick={()=>onReset(action)}><RotateCcw aria-hidden="true"/></UiButton></td>
      </tr>;
    })}</tbody>
  </table>;
}
