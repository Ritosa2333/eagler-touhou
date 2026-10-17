import {useLocale} from '../../i18n';
import type {keyboardActionCodes} from '../../../src/launcher/keyboard-preferences.mts';
export type InputAction=keyof typeof keyboardActionCodes;
export const inputActionOrder:readonly InputAction[]=['up','down','left','right','shoot','bomb','focus','action','pause','confirm','skip','restart'];
export const inputActionNames={shoot:['射击 / 确认','Shoot / Confirm'],bomb:['炸弹 / 取消','Bomb / Cancel'],focus:['低速','Focus'],pause:['暂停 / 菜单','Pause / Menu'],up:['上','Up'],down:['下','Down'],left:['左','Left'],right:['右','Right'],skip:['跳过对话','Skip dialogue'],confirm:['确认','Confirm'],action:['特殊动作','Special action'],restart:['重试','Retry']} as const;

/** Keyboard and controller use the same familiar action/binding table. */
export function InputBindingsTable({bindings,learning,onBind,onReset,resetLabel,isActive,actions=inputActionOrder,names}: {
  bindings:Record<InputAction,readonly string[]>;learning:InputAction|null;onBind(action:InputAction):void;
  onReset(action:InputAction):void;resetLabel:string;isActive?(action:InputAction):boolean;
  actions?:readonly InputAction[];names?:Partial<Record<InputAction,readonly [string,string]>>;
}){
  const {locale}=useLocale(),zh=locale==='zh-CN';
  return <table className="input-binding-table">
    <thead><tr><th scope="col">{zh?'动作':'Action'}</th><th scope="col">{zh?'按键':'Binding'}</th><th scope="col"><span className="input-visually-hidden">{resetLabel}</span></th></tr></thead>
    <tbody>{actions.map(action=>{
      const name=(names?.[action]??inputActionNames[action])[zh?0:1],keys=bindings[action],waiting=learning===action;
      return <tr key={action} data-action={action} data-learning={waiting} data-active={!!isActive?.(action)} className={action==='shoot'||action==='pause'?'input-action-divider':undefined}>
        <th scope="row">{name}</th><td><button type="button" className="input-binding-button" aria-label={waiting?`${name}：${zh?'等待按键，点击取消':'waiting for input, click to cancel'}`:`${name}：${keys.join(' / ')|| (zh?'未设置':'Unbound')}，${zh?'点击更改':'change binding'}`} aria-pressed={waiting} onClick={()=>onBind(action)} onKeyDown={event=>{if(!waiting&&event.key==='Delete'){event.preventDefault();onReset(action);}}}>
          {waiting?<span className="input-binding-listening">{zh?'按下按键…':'Press a control…'}</span>:keys.length?keys.map((key,n)=><kbd key={n} className={n?'input-keycap secondary':'input-keycap'}>{key}</kbd>):<span className="input-binding-empty">—</span>}
        </button></td><td><button type="button" className="input-binding-reset" title={`${resetLabel} · ${name}`} aria-label={`${resetLabel} · ${name}`} onClick={()=>onReset(action)}>↶</button></td>
      </tr>;
    })}</tbody>
  </table>;
}
