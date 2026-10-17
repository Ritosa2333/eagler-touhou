import {useEffect,useSyncExternalStore} from 'react';
import {defaultGamepadProfile,gamepadActions,type GamepadMappingModel,type GamepadControl} from '../../../src/launcher/gamepad-mapping.mts';
import {useLocale} from '../../i18n';
import {MainSelect} from '../launcher/MainSelect';
import {InputBindingsTable,type InputAction} from './InputBindingsTable';
import {localInputActions} from '../../../src/launcher/th09-local-input.mts';
export {inputActionNames} from './InputBindingsTable';
export function GamepadSettings({model,active=true,assignment}:{model:GamepadMappingModel;active?:boolean;assignment?:{key:string;onChange(key:string):void}}) {
  const state=useSyncExternalStore(model.subscribe,model.getSnapshot,model.getSnapshot),{locale}=useLocale();
  const zh=locale==='zh-CN',text=(cn:string,en:string)=>zh?cn:en;
  useEffect(()=>{if(!active)return;model.refresh();const timer=setInterval(()=>model.refresh(),50);return()=>{clearInterval(timer);model.cancelLearning();};},[active,model]);
  const key=assignment?.key??state.seats[0]??'',device=state.devices.find(d=>d.key===key),profile=state.profiles[key]??defaultGamepadProfile(device?.standard===true);
  function controlName(c:GamepadControl){return c.kind==='button'?text(`按钮 ${c.index+1}`,`Button ${c.index+1}`):text(`轴 ${c.index+1} ${c.value>c.rest?'+':'−'}`,`Axis ${c.index+1} ${c.value>c.rest?'+':'−'}`);}
  const bindings=Object.fromEntries(gamepadActions.map(action=>[action,profile.bindings[action].map(controlName)])) as Record<InputAction,string[]>;
  return <div className="gamepad-settings input-config">
    {!assignment&&<label className="input-gamepad-enable"><input type="checkbox" checked={state.enabled} disabled={!state.supported} onChange={event=>model.setEnabled(event.target.checked)}/>{text('启用手柄改键','Enable gamepad mapping')}</label>}
    <div className="input-layout-toolbar"><label htmlFor="inputGamepadDevice">{text('手柄','Controller')}</label><MainSelect id="inputGamepadDevice" className="option-select" aria-label={text('手柄设备','Gamepad device')} value={key} onChange={event=>{model.cancelLearning();if(assignment)assignment.onChange(event.target.value);else model.select(0,event.target.value);}}>
      <option value="">{text('选择手柄','Select a controller')}</option>{key&&!device&&<option value={key}>{text('已保存的手柄未连接','Saved controller disconnected')}</option>}
      {state.devices.map(d=><option value={d.key} key={d.key}>{d.name} (#{d.index+1})</option>)}
    </MainSelect></div>
    <p className="input-config-hint">{!state.supported?text('当前浏览器无法读取手柄。','This browser cannot read gamepads.'):!device?text('连接手柄后，先按一下按钮以识别设备。','Connect your controller and press a button to make it visible.'):text('点击按键，再按手柄按钮或推动摇杆。','Click a binding, then press a button or move an axis.')}</p>
    {device&&<>
      {!device.standard&&<p className="input-config-hint">{text('此手柄没有标准布局，请逐项设置。','This controller has no standard layout. Set each action.')}</p>}
      <InputBindingsTable bindings={bindings} learning={state.learning} actions={assignment?localInputActions:undefined} names={assignment?{bomb:['快速攻击 / 取消','Quick attack / Cancel'],focus:['低速 / 蓄力','Focus / Charge']}:undefined} resetLabel={text('清除按键','Clear binding')} isActive={action=>!!((state.deviceActive[key]??0)&(1<<gamepadActions.indexOf(action)))} onBind={action=>state.learning===action?model.cancelLearning():model.learn(key,action)} onReset={action=>{model.cancelLearning();model.bind(key,action,[]);}}/>
      <label className="input-axis-threshold"><span>{text('摇杆 / 扳机触发阈值','Stick / trigger threshold')}</span><output>{Math.round(profile.threshold*100)}%</output><input aria-label={text('摇杆 / 扳机触发阈值','Stick / trigger threshold')} type="range" min="10" max="90" value={Math.round(profile.threshold*100)} onChange={event=>model.setThreshold(key,Number(event.target.value)/100)}/></label>
      <footer className="input-config-footer"><span role="status">{state.learning?text('先松开，再按下要绑定的按键','Release controls, then press the new control'):text('按设备自动保存','Saved automatically for this controller')}</span><button className="input-config-button" type="button" onClick={()=>{model.cancelLearning();model.reset(key);}}>{text('恢复默认','Reset defaults')}</button></footer>
    </>}
  </div>;
}
