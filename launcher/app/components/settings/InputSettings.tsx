import {useEffect,useId,useRef,useState} from 'react';
import type {GamepadMappingModel} from '../../../src/launcher/gamepad-mapping.mts';
import type {KeyboardPreferences} from '../../../src/launcher/keyboard-preferences.mts';
import {useLocale} from '../../i18n';
import {KeyboardSettings} from './KeyboardSettings';
import {GamepadSettings} from './GamepadSettings';
import type {Th09LocalInput} from '../../../src/launcher/th09-local-input.mts';
import {Th09LocalInputSettings} from './Th09LocalInputSettings';
import {MainSelect} from '../launcher/MainSelect';

/** Phosphor regular icons; license: public/assets/room-icons-LICENSE.txt. */
const deviceIcons={
  keyboard:'M224,48H32A16,16,0,0,0,16,64V192a16,16,0,0,0,16,16H224a16,16,0,0,0,16-16V64A16,16,0,0,0,224,48Zm0,144H32V64H224V192Zm-16-64a8,8,0,0,1-8,8H56a8,8,0,0,1,0-16H200A8,8,0,0,1,208,128Zm0-32a8,8,0,0,1-8,8H56a8,8,0,0,1,0-16H200A8,8,0,0,1,208,96ZM72,160a8,8,0,0,1-8,8H56a8,8,0,0,1,0-16h8A8,8,0,0,1,72,160Zm96,0a8,8,0,0,1-8,8H96a8,8,0,0,1,0-16h64A8,8,0,0,1,168,160Zm40,0a8,8,0,0,1-8,8h-8a8,8,0,0,1,0-16h8A8,8,0,0,1,208,160Z',
  gamepad:'M176,112H152a8,8,0,0,1,0-16h24a8,8,0,0,1,0,16ZM104,96H96V88a8,8,0,0,0-16,0v8H72a8,8,0,0,0,0,16h8v8a8,8,0,0,0,16,0v-8h8a8,8,0,0,0,0-16ZM241.48,200.65a36,36,0,0,1-54.94,4.81c-.12-.12-.24-.24-.35-.37L146.48,160h-37L69.81,205.09l-.35.37A36.08,36.08,0,0,1,44,216,36,36,0,0,1,8.56,173.75a.68.68,0,0,1,0-.14L24.93,89.52A59.88,59.88,0,0,1,83.89,40H172a60.08,60.08,0,0,1,59,49.25c0,.06,0,.12,0,.18l16.37,84.17a.68.68,0,0,1,0,.14A35.74,35.74,0,0,1,241.48,200.65ZM172,144a44,44,0,0,0,0-88H83.89A43.9,43.9,0,0,0,40.68,92.37l0,.13L24.3,176.59A20,20,0,0,0,58,194.3l41.92-47.59a8,8,0,0,1,6-2.71Zm59.7,32.59-8.74-45A60,60,0,0,1,172,160h-4.2L198,194.31a20.09,20.09,0,0,0,17.46,5.39,20,20,0,0,0,16.23-23.11Z',
};

/** Keep detailed capture in a bounded dialog, outside the options rail. */
export function InputSettings({keyboard,gamepad,th09Local}:{keyboard?:KeyboardPreferences;gamepad?:GamepadMappingModel;th09Local?:Th09LocalInput}){
  const {locale}=useLocale(),zh=locale==='zh-CN',id=useId(),dialog=useRef<HTMLDialogElement>(null);
  const [open,setOpen]=useState(true),[editing,setEditing]=useState(false),[closing,setClosing]=useState(false);
  const [device,setDevice]=useState<'keyboard'|'gamepad'>(keyboard?'keyboard':'gamepad');
  const [scope,setScope]=useState<'general'|'local'>('general');
  useEffect(()=>{if(!th09Local)setScope('general');},[th09Local]);
  useEffect(()=>{if(editing&&!dialog.current?.open)dialog.current?.showModal();},[editing]);
  useEffect(()=>{if(!closing)return;const timer=setTimeout(()=>{dialog.current?.close();setEditing(false);setClosing(false);},170);return()=>clearTimeout(timer);},[closing]);
  const title=zh?'输入设置':'Input settings';
  function edit(value:'keyboard'|'gamepad'){setDevice(value);setScope('general');setClosing(false);setEditing(true);}
  const choices=(['keyboard','gamepad'] as const).filter(value=>value==='keyboard'?keyboard:gamepad);
  const label=(value:'keyboard'|'gamepad')=>value==='keyboard'?(zh?'键盘改键':'Keyboard'):(zh?'手柄改键':'Gamepad');
  return <section className="options-group input-settings" data-open={open}>
    <button className="options-group-head input-settings-heading" type="button" aria-expanded={open} aria-controls={id} onClick={()=>setOpen(!open)}><span>{title}</span><i aria-hidden="true">⌄</i></button>
    <div id={id} className="input-settings-collapse" inert={!open} aria-hidden={!open}><div className="input-settings-content">
      <div className="input-settings-entries">{choices.map(value=><button className="input-settings-entry" type="button" key={value} onClick={()=>edit(value)} aria-haspopup="dialog">
        <svg viewBox="0 0 256 256" aria-hidden="true" focusable="false"><path d={deviceIcons[value]}/></svg>
        <strong>{label(value)}</strong><span aria-hidden="true">›</span>
      </button>)}</div>
    </div></div>
    {editing&&<dialog className={`input-settings-dialog settings-ui${closing?' is-closing':''}`} ref={dialog} aria-labelledby={id+'-title'} onCancel={event=>{event.preventDefault();setClosing(true);}} onClose={()=>{setEditing(false);setClosing(false);}} onClick={event=>{if(event.target!==event.currentTarget)return;const r=event.currentTarget.getBoundingClientRect();if(event.clientX<r.left||event.clientX>r.right||event.clientY<r.top||event.clientY>r.bottom)setClosing(true);}}>
      <header><h2 id={id+'-title'}>{title}</h2><button type="button" className="input-dialog-close" aria-label={zh?'关闭输入设置':'Close input settings'} onClick={()=>setClosing(true)}>×</button></header>
      {th09Local&&gamepad&&<div className="input-context-toolbar"><label htmlFor={id+'-scope'}>{zh?'配置场景':'Control context'}</label><MainSelect id={id+'-scope'} aria-label={zh?'配置场景':'Control context'} className="option-select" value={scope} onChange={e=>setScope(e.target.value==='local'?'local':'general')}><option value="general">{zh?'单人 / 联机':'Single player / Online'}</option><option value="local">{zh?'本地双人':'Local two-player'}</option></MainSelect></div>}
      {scope==='general'&&<div className="input-device-tabs" role="tablist" aria-label={zh?'输入设备':'Input device'}>{choices.map(value=><button key={value} id={id+'-'+value} type="button" role="tab" tabIndex={device===value?0:-1} aria-controls={id+'-panel'} aria-selected={device===value} onClick={()=>setDevice(value)} onKeyDown={event=>{
        if(!['ArrowLeft','ArrowRight','Home','End'].includes(event.key))return;event.preventDefault();
        const tabs=[...event.currentTarget.parentElement!.querySelectorAll<HTMLButtonElement>('[role=tab]')];
        const index=event.key==='Home'?0:event.key==='End'?tabs.length-1:(tabs.indexOf(event.currentTarget)+(event.key==='ArrowRight'?1:tabs.length-1))%tabs.length;
        tabs[index]?.focus();tabs[index]?.click();
      }}>{label(value)}</button>)}</div>}
      <div className="input-dialog-scroll"><div className="input-device-panel" role={scope==='general'?'tabpanel':undefined} id={id+'-panel'} aria-labelledby={scope==='general'?id+'-'+device:undefined} key={scope+device}>
        {scope==='local'&&th09Local&&gamepad?<Th09LocalInputSettings model={th09Local} gamepad={gamepad} active={!closing}/>:<>
          {device==='keyboard'&&keyboard&&<KeyboardSettings model={keyboard} active={!closing}/>}
          {device==='gamepad'&&gamepad&&<GamepadSettings model={gamepad} active={!closing}/>}
        </>}
      </div></div>
    </dialog>}
  </section>;
}
