// Local options sheet following the upstream launcher hierarchy. Managed
// iframes receive the parent configuration without mounting another sheet.
import {readLocalPlayerSettings,writeLocalPlayerSettings} from './local-player-settings.mjs';
import {touchOptions} from './touch-options.mjs';
import {createReplayFileBridge} from './replay-file-bridge.mjs';
import {OwnedReplayStore} from './sound.mjs';
import {localInputActions,readInputBindings} from './local-input-bindings.mjs';

export function mountLocalLaunchPanel({window,loader,file,start,retry,hint,meter,status,signal}){
 const document=loader.ownerDocument;
 let settings=readLocalPlayerSettings(window),replays=null,busy=false;
 const build=(tag,parent,text)=>{const n=document.createElement(tag);if(text!==undefined)n.textContent=text;parent.append(n);return n;};
 const actions=loader.querySelector('.resource-actions');
 loader.classList.add('local-launch');
 loader.querySelector('h1').remove();
 const hero=build('header',loader);hero.className='launch-sheet-hero';
 const close=build('button',hero,'×');close.type='button';close.setAttribute('aria-label','收起启动设置');
 const sheet=build('div',loader);sheet.className='launch-sheet-body';
 const title=build('div',sheet);title.className='launch-sheet-title';build('small',title,'th128');build('h1',title,'妖精大战争');
 const progress=build('section',sheet);progress.className='launch-sheet-resource';progress.append(hint,meter,status);
 const fold=build('button',loader,'启动设置');fold.type='button';fold.className='launch-sheet-reopen';fold.hidden=true;
 function collapse(value){sheet.hidden=actions.hidden=hero.hidden=value;fold.hidden=!value;value?fold.focus():close.focus();}
 close.addEventListener('click',()=>collapse(true));fold.addEventListener('click',()=>collapse(false));
 function group(name){const section=build('details',sheet);section.className='launch-group';section.open=true;build('summary',section,name);const list=build('div',section);list.className='launch-group-content';return list;}
 function row(parent,label,extra){const n=build('label',parent);const text=build('span',n);build('strong',text,label);if(extra)build('small',text,extra);return n;}
 function changed(key,value){settings=writeLocalPlayerSettings(window,{[key]:value});}
 function toggle(parent,label,selected,onChange,note){const n=row(parent,label,note),input=build('input',n);input.type='checkbox';input.role='switch';input.checked=selected;input.setAttribute('aria-label',label);input.addEventListener('change',()=>onChange(input.checked));return input;}
 function select(parent,label,choices,value,onChange){const n=row(parent,label);n.classList.add('launch-select');const ctl=build('select',n);ctl.setAttribute('aria-label',label);for(const [key,text]of choices){const opt=build('option',ctl,text);opt.value=key;}ctl.value=value;ctl.addEventListener('change',()=>onChange(ctl.value));return ctl;}
 function volume(parent,label,key){const n=row(parent,label),input=build('input',n),out=build('output',n);Object.assign(input,{type:'range',min:0,max:100,value:settings[key]});input.setAttribute('aria-label',label);out.textContent=input.value+'%';input.addEventListener('input',()=>{changed(key,Number(input.value));out.textContent=input.value+'%';});}
 const files=group('存档 / 录像设置'),replayLine=build('div',files),recent=build('select',replayLine),filesStatus=build('small',files);
 replayLine.className='launch-replay-actions';recent.setAttribute('aria-label','录像文件');
 const empty=build('option',recent,'选择录像');empty.disabled=true;empty.value='';recent.value='';
 const refresh=build('button',replayLine,'管理录像'),score=build('button',replayLine,'下载存档'),download=build('button',replayLine,'下载录像');
 for(const n of [refresh,score,download])n.type='button';download.disabled=true;
 function bridge(){return replays??=createReplayFileBridge({signal});}
 const run=fn=>async()=>{if(busy)return;busy=true;try{await fn();}catch(e){if(!signal.aborted)filesStatus.textContent=e.errno===44?'此浏览器尚无对应的存档或录像。':e.message;}finally{busy=false;}};
 function save(bytes,name){const url=URL.createObjectURL(new Blob([Uint8Array.from(bytes)]));try{const link=document.createElement('a');link.href=url;link.download=name;link.click();}finally{window.setTimeout(()=>URL.revokeObjectURL(url),1000);}}
 refresh.onclick=run(async()=>{const {files}=await bridge().execute('list');const rows=files.filter(x=>x.path.endsWith('.rpy'));recent.replaceChildren(empty);for(const x of rows){const option=build('option',recent,x.path.split('/').pop());option.value=x.path;}recent.value=rows[0]?.path??'';download.disabled=!recent.value;filesStatus.textContent=rows.length?'已有 '+rows.length+' 份录像。':'当前没有已保存录像。';});
 recent.onchange=()=>{download.disabled=!recent.value;};
 download.onclick=run(async()=>{const {path,bytes}=await bridge().execute('read',{path:recent.value});save(bytes,path.split('/').pop());});
 score.onclick=run(async()=>{const {bytes}=await bridge().execute('read',{path:'scoreth128.dat'});save(bytes,'scoreth128.dat');});
 const replayImport=build('input',files);Object.assign(replayImport,{type:'file',accept:'.rpy'});replayImport.setAttribute('aria-label','导入原版录像到空槽');
 const replayImportLabel=build('small',files,'导入原版录像至第一个空槽。');replayImportLabel.className='launch-caption';
 replayImport.onchange=run(async()=>{const selected=replayImport.files?.[0];if(!selected)return;
  if(selected.size>16*1024*1024)throw Error('录像文件超出大小限制。');
  const store=new OwnedReplayStore();try{const rows=await store.observe(),slot=rows.findIndex(r=>r===null);if(slot<0)throw Error('25 个录像槽都已占用。');
   const data=await selected.arrayBuffer();signal.throwIfAborted();
   const receipt=await store.importAtSlot(slot,new Uint8Array(data),{fileName:selected.name,expectedDigest:null,signal,isCurrent:()=>!signal.aborted});
   if(!receipt.strictFullReadback)throw Error('录像读回校验未通过。');filesStatus.textContent='已导入到 No.'+String(slot+1).padStart(2,'0');
  }finally{await store.close();replayImport.value='';}
 });
 const show=group('显示设置');
 toggle(show,'高刷新率',settings.highRefresh===true,v=>changed('highRefresh',v),'画面跟随屏幕刷新，游戏逻辑仍为 60 Hz。');
 const language=select(show,'语言',[['zh','中文（简体）'],['ja','日本語'],['en','English']],settings.language,v=>changed('language',v));
 toggle(show,'点对点显示',settings.pixelPerfect,v=>changed('pixelPerfect',v),'使用整数倍和最近邻缩放。');
 const input=group('输入设置');
 const keyMap=build('details',input),padMap=build('details',input);keyMap.className=padMap.className='launch-input-map';
 build('summary',keyMap,'键盘改键');build('summary',padMap,'手柄改键');
 build('small',keyMap,'选一个按键按钮，再按要绑定的键。');build('small',padMap,'支持标准手柄；左摇杆也可移动。');
 for(const [code,label]of localInputActions){
  const line=row(keyMap,label),bind=build('button',line,settings.bindings.keyboard[code]??code);bind.type='button';
  const listenKey=event=>{
   if(!bind.hasAttribute('data-awaiting-key'))return;event.preventDefault();event.stopPropagation();
   bind.removeAttribute('data-awaiting-key');
   if(event.code!=='Escape'){
    const old=settings.bindings,candidate=readInputBindings({...old.keyboard,[code]:event.code},old.pad);
    if(candidate.keyboard[code]===event.code&&Object.keys(candidate.keyboard).length===localInputActions.length)changed('bindings',candidate);
   }
   bind.textContent=settings.bindings.keyboard[code]??code;
  };
  bind.addEventListener('click',()=>{bind.setAttribute('data-awaiting-key','1');bind.textContent='请按键';});bind.addEventListener('keydown',listenKey);
  bind.addEventListener('blur',()=>{bind.removeAttribute('data-awaiting-key');bind.textContent=settings.bindings.keyboard[code]??code;});
  select(padMap,label,Array.from({length:18},(_,n)=>[String(n),'按键 '+n]),String(settings.bindings.pad[code]),v=>changed('bindings',{...settings.bindings,pad:{...settings.bindings.pad,[code]:Number(v)}}));
 };
 const audio=group('音量设置');volume(audio,'游戏音乐','musicVolume');volume(audio,'游戏音效','effectsVolume');
 const advanced=group('高级设置');
 select(advanced,'音乐',[['ogg','OGG'],['none','关闭游戏音乐']],settings.music,v=>changed('music',v));
 toggle(advanced,'thprac',settings.thprac!==false,v=>changed('thprac',v),'高级练习器。');
 const touch=group('触控设置');const changedTouch=(key,value)=>changed('touch',touchOptions({[key]:value},settings.touch));
 toggle(touch,'启用触控功能',settings.touch.touchEnabled,v=>changedTouch('touchEnabled',v));
 select(touch,'移动方式',[['touch','直连（原版移速）'],['touch-unlimited','无限直连（作弊触控）'],['joystick','摇杆'],['joystick-free','自由摇杆']],settings.touch.touchMovementMode,v=>changedTouch('touchMovementMode',v));
 select(touch,'低速方式',[['hold-button','按钮（按住时低速）'],['toggle-button','按钮（切换低速）'],['two-finger','双指低速']],settings.touch.touchFocusMode,v=>changedTouch('touchFocusMode',v));
 toggle(touch,'双指缩放视口',settings.layoutZoom,v=>changed('layoutZoom',v));
 const launchLayout=build('button',touch,'按键布局与触控设置 ›');launchLayout.type='button';launchLayout.className='launch-layout-open';
 launchLayout.addEventListener('click',()=>{if(start.disabled)return;changed('openLayout',true);start.click();});
 const keySignal=(event)=>{if(event.code==='Escape'&&!sheet.hidden){event.preventDefault();collapse(true);}};loader.addEventListener('keydown',keySignal);
 loader.append(actions,fold);actions.classList.add('launch-sheet-footer');actions.prepend(start);start.classList.add('launch-play');file.classList.add('launch-import');retry.classList.add('launch-retry');
 signal.addEventListener('abort',()=>replays?.dispose(),{once:true});
 return{settings(){return readLocalPlayerSettings(window);},ready(variants){for(const option of language.options)option.disabled=!!variants&&!Object.hasOwn(variants,option.value);if(language.selectedOptions[0]?.disabled){const value=[...language.options].find(item=>!item.disabled)?.value;if(value){language.value=value;changed('language',value);}}launchLayout.disabled=start.disabled;}};
}
