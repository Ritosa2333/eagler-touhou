import {useEffect,useState} from 'react';
import {createRoot} from 'react-dom/client';
import {LocaleProvider} from '../../app/i18n';
import {InputSettings} from '../../app/components/settings/InputSettings';
import {createKeyboardPreferences} from '../../src/launcher/keyboard-preferences.mts';
import {createGamepadMappingModel} from '../../src/launcher/gamepad-mapping.mts';
import {createTh09LocalInput} from '../../src/launcher/th09-local-input.mts';
import '../../app/ui/foundation.css';
import '../../app/components/launcher/book-ui.css';
import '../../app/components/launcher/book-surfaces.css';
import '../../app/ui/system.css';
import '../../app/ui/tailwind.css';
import '../../app/components/launcher/options-sheet.css';
import '../../app/components/settings/settings-ui.css';

// Physical device discovery is the only replaced input edge. Mapping, conflict
// checks, keyboard capture and persistence use the actual application models.
function models(){
  const values=new Map(),storage={getItem:key=>values.get(key)??null,setItem:(key,value)=>values.set(key,value)};
  const pad={id:'标准手柄示例',index:0,connected:true,mapping:'standard',buttons:Array.from({length:17},()=>({pressed:false,value:0})),axes:[0,0,0,0]};
  const gamepad=createGamepadMappingModel({storage,getGamepads:()=>[pad]});gamepad.refresh();gamepad.select(0,gamepad.getSnapshot().devices[0].key);gamepad.setEnabled(true);
  return {keyboard:createKeyboardPreferences(storage),gamepad,th09Local:createTh09LocalInput(storage)};
}
function Scenario(){
  const [input]=useState(models),[theme,setTheme]=useState('light');
  useEffect(()=>{document.documentElement.dataset.bookTheme=theme;document.documentElement.className=theme;document.body.classList.add('less-motion');},[theme]);
  return <LocaleProvider locale="zh-CN"><header className="book-masthead" hidden aria-hidden="true"/>
    <main className="input-specimen"><InputSettings {...input}/></main>
    <aside className="input-specimen-note">本地设备示例 | 内存偏好 | 不连接实体手柄 <button onClick={()=>setTheme(theme==='light'?'dark':'light')}>{theme==='light'?'深色':'亮色'}</button></aside>
  </LocaleProvider>;
}
const style=document.createElement('style');style.textContent='html,body{min-height:100dvh;background:var(--book-bg)}body{display:flow-root}.input-specimen{max-width:560px;margin:40px auto;padding:24px}.input-specimen-note{position:fixed;bottom:0;left:0;right:0;padding:6px 12px;min-height:32px;background:var(--book-surface);color:var(--book-ink);font:12px sans-serif}.input-specimen-note button{margin-left:12px}';document.head.append(style);
createRoot(document.getElementById('root')).render(<Scenario/>);
