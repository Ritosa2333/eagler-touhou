// The page owns game/native state; this controller owns only the explicit TH128 form and pending launch.
import {createThprac128Session,THPRAC128_DEFAULTS,THPRAC128_STAGE_MAP,THPRAC128_SECTIONS,thprac128SectionChoices,thprac128Label} from './thprac128.mjs';
const selectNames=['mode','startPoint','path','stage','difficulty','warp','section','phase','locale'];
const resourceNames=['score','motivation','perfect_freeze','ice_power','ice_area'];
const text={zh:{title:'练习选项',ready:'选择起点、章节或Boss与初始资源；练习局不计正常成绩与解锁。',replace:'请先确认替换当前局；尚未保存的进度不会保留。',busy:'练习入口尚未就绪或正在处理其他操作。',off:'thprac 已关闭；正常游戏菜单仍可使用。',loading:'正在准备 thprac…',started:'thprac 练习已开始，不计正常成绩与解锁。',normal:'已启动原版流程。',failed:'练习启动失败：',cancelled:'已取消练习启动。'},en:{title:'Practice Options',ready:'Choose a stage, section and resources. Practice does not qualify for normal scores or unlocks.',replace:'Confirm replacing the current run and losing its unsaved progress.',busy:'Practice runtime is unavailable or busy.',off:'thprac is off. The normal game menu remains available.',loading:'Preparing thprac…',started:'Practice started; normal scores and unlocks are excluded.',normal:'Original game flow started.',failed:'Practice start failed: ',cancelled:'Practice launch cancelled.'},ja:{title:'練習オプション',ready:'ステージ・区間・初期値を選択。練習は通常スコアや解禁の対象外です。',replace:'現在のプレイを破棄することを確認してください。',busy:'練習の準備ができていないか、処理中です。',off:'thprac は無効です。通常メニューは利用できます。',loading:'thprac を準備中…',started:'練習を開始しました。通常スコア・解禁の対象外です。',normal:'通常プレイを開始しました。',failed:'練習を開始できません：',cancelled:'練習の開始を中止しました。'}};
const menuCopy={zh:{enabled:'启用 thprac',locale:'界面语言',replace:'替换当前局，放弃尚未保存的进度',notice:'练习局不计正常成绩与解锁。资源输入使用原始单位：干劲、完美冻结、冰冻力量每100为1%，冰冻面积每1为1%。'},en:{enabled:'Enable thprac',locale:'Language',replace:'Replace the current run and discard unsaved progress',notice:'Practice excludes normal scores and unlocks. Resource units: 100 = 1% for Motivation, Perfect Freeze and Ice Power; 1 = 1% for Ice Area.'},ja:{enabled:'thprac を有効にする',locale:'言語',replace:'現在のプレイと未保存の進行を破棄する',notice:'練習は通常スコア・解禁の対象外です。やる気・PF・こおりパワーは100で1%、面積は1で1%です。'}};
export function thprac128ResourceText(key,value){if(!Number.isSafeInteger(value)||value<0)throw new Error('Invalid practice resource');return key==='score'?String(value):String(key==='ice_area'?value:Math.floor(value/100))+'%';}
export function mountThprac128Menu({root,controls=root,opener,getAvailability,onOpen=()=>{},onFocusGame=()=>{},onCancel=()=>{},onRouteKey=()=>{},onRouteChange=()=>{},settings={enabled:true,locale:"zh"},onSettings=()=>{},launch}){
 if(!root||!opener||typeof getAvailability!=='function'||typeof launch!=='function')throw new Error('TH128 thprac menu consumers unavailable');
 const node={};for(const key of [...selectNames,...resourceNames,'enabled','dlg','start','restart','cancel','status','replace','replaceRow']){node[key]=root.querySelector('[data-thprac="'+key+'"]')??controls.querySelector('[data-thprac="'+key+'"]');if(!node[key])throw new Error('TH128 control missing: '+key);}
 const listeners=[],listen=(target,event,fn)=>{target.addEventListener(event,fn);listeners.push(()=>target.removeEventListener(event,fn));};
 let disposed=false,opening=false,pending=null,ticket=0,routeOpen=false;
 const language=()=>['zh','en','ja'].includes(node.locale.value)?node.locale.value:'zh';
 const message=key=>text[language()][key];
 const options=(target,rows,preferred)=>{const wanted=String(preferred??target.value);target.replaceChildren(...rows.map(row=>{const o=root.ownerDocument.createElement('option');o.value=String(row.value);o.textContent=row.label;return o;}));target.value=rows.some(r=>String(r.value)===wanted)?wanted:String(rows[0]?.value??'');};
 const index=(key,fallback=0)=>{const n=Number(node[key].value);return Number.isInteger(n)&&n>=0?n:fallback;};
 function stage(){return THPRAC128_STAGE_MAP[index('startPoint')][index('path')][index('stage')];}
 function refresh({routeDifficulty}={}){
  if(disposed)return;const locale=language(),label=key=>thprac128Label(key,locale),a=getAvailability(),blocked=!a.ready||a.busy||opening||!!pending;
  options(node.locale,[{value:'zh',label:'中文'},{value:'en',label:'English'},{value:'ja',label:'日本語'}],locale);
  options(node.mode,[{value:0,label:label('TH128_NORMAL_PLAY')},{value:1,label:label('TH128_PRACTICE_MODE')}],node.mode.value||'0');
  options(node.startPoint,['A','B','C','EX'].map((key,value)=>({value,label:label('TH128_START_'+key)})));
  const point=index('startPoint'),extra=point===3,practice=index('mode')===1;
  options(node.path,[0,1].map(value=>({value,label:extra?'Extra':label('TH128_PATHDESC_'+String.fromCharCode(65+point)+(value+1))})));
  options(node.stage,(extra?[0]:[0,1,2]).map(value=>({value,label:extra?'Extra':String(value+1)})));
  options(node.difficulty,(extra?[4]:[0,1,2,3]).map(value=>({value,label:['Easy','Normal','Hard','Lunatic','Extra'][value]})),extra?4:routeDifficulty??(node.difficulty.value||'1'));
  options(node.warp,['TH_NONE','TH_STAGE_PORTION','TH_MID_STAGE','TH_END_STAGE','TH_NONSPELL','TH_SPELL'].map((key,value)=>({value,label:label(key)})));
  root.setAttribute('data-mode',practice?'practice':'normal');root.setAttribute('lang',locale==='zh'?'zh-CN':locale==='ja'?'ja':'en');
  const warp=index('warp');options(node.section,thprac128SectionChoices(stage(),warp,locale));
  const sectionLabel=root.querySelector('[data-thprac-section-label]');if(sectionLabel)sectionLabel.setAttribute('data-thprac-label',['TH_STAGE_PORTION','TH_STAGE_PORTION','TH_MID_STAGE','TH_END_STAGE','TH_NONSPELL','TH_SPELL'][warp]);
  const selected=warp>=2?THPRAC128_SECTIONS[Number(node.section.value)-1]:null,max=selected?.phaseMax??0;
  options(node.phase,Array.from({length:max+1},(_,value)=>({value,label:label('TH_P'+(value+1))})));
  if(!selected?.dlg)node.dlg.checked=false;
  opener.disabled=blocked;node.start.disabled=blocked||!node.enabled.checked;
  node.restart.disabled=blocked||!node.enabled.checked||!a.practiceSession;
  node.replaceRow.hidden=!a.inRun;
  for(const key of [...selectNames,...resourceNames,'enabled','dlg'])node[key].disabled=opening||!!pending;
  for(const key of ['path','stage'])node[key].disabled||=!practice||extra;
  for(const key of ['warp','section','phase','dlg',...resourceNames])node[key].disabled||=!practice;
  node.section.disabled||=warp===0;node.phase.disabled||=!max;node.dlg.disabled||=!selected?.dlg;
  for(const key of ['path','stage','warp','section','phase','dlg',...resourceNames]){const row=node[key].closest?.('[data-thprac-row]');if(row)row.hidden=!practice||(['path','stage'].includes(key)&&extra)||(key==='section'&&warp===0)||(key==='phase'&&!max)||(key==='dlg'&&!selected?.dlg);}
  for(const key of resourceNames){const output=root.querySelector('[data-thprac-value="'+key+'"]');if(output)output.textContent=thprac128ResourceText(key,Number(node[key].value));}
  for(const item of root.querySelectorAll('[data-thprac-label]'))item.textContent=label(item.getAttribute('data-thprac-label'));
  for(const item of root.querySelectorAll('[data-thprac-copy]'))item.textContent=menuCopy[language()][item.getAttribute('data-thprac-copy')]??'';
  const title=root.querySelector('[data-thprac-title]');if(title)title.textContent=message('title');
  node.start.textContent=label('TH_BEGIN');node.restart.textContent=label('TH_RESTART');node.cancel.textContent=label('TH_CLOSE');
 }
 function close({focus=true}={}){
  ++ticket;pending?.controller.abort();root.hidden=true;opener.setAttribute('aria-expanded','false');node.replace.checked=false;
  if(pending)node.status.textContent=message('cancelled');if(!disposed)refresh();if(focus)onFocusGame();
 }
 async function open({route=false}={}){
  if(disposed||opening||pending)return;const a=getAvailability();if(!a.ready||a.busy){node.status.textContent=message('busy');refresh();return;}
  routeOpen=route;opening=true;root.hidden=false;node.replace.checked=false;opener.setAttribute('aria-expanded','true');node.status.textContent=message('ready');refresh();const mine=ticket;
  try{await onOpen({route});if(!route&&!disposed&&ticket===mine&&!root.hidden)node.startPoint.focus();}
  catch(e){if(!disposed&&ticket===mine)node.status.textContent=message('failed')+e.message;}
  finally{opening=false;if(!disposed)refresh();}
 }
 function strictNumber(key){const value=node[key].value;if(!/^(0|[1-9][0-9]*)$/.test(value))throw new TypeError(key+' requires an integer / 请输入整数');const n=Number(value);if(!Number.isSafeInteger(n))throw new TypeError(key+' integer out of range');return n;}
 function selectedSession(){
  const mode=strictNumber('mode'),actualStage=stage(),warp=strictNumber('warp');let section=0;
  if(mode&&warp)section=warp===1?10000+(actualStage+1)*100+strictNumber('section'):strictNumber('section');
  const params={mode,stage:actualStage,section,phase:mode&&!node.phase.disabled?strictNumber('phase'):0,dlg:mode&&!node.dlg.disabled&&node.dlg.checked};
  for(const key of resourceNames)params[key]=mode?strictNumber(key):THPRAC128_DEFAULTS[key];
  return createThprac128Session(params,strictNumber('difficulty'));
 }
 async function begin(restart=false){
  if(disposed||root.hidden||pending||opening)return;const a=getAvailability();
  if(!a.ready||a.busy){node.status.textContent=message('busy');refresh();return;}
  if(!node.enabled.checked){node.status.textContent=message('off');return;}
  if(a.inRun&&!node.replace.checked){node.status.textContent=message('replace');return;}
  let selected;try{if(restart&&!a.practiceSession)return;selected=restart?createThprac128Session(a.practiceSession.params,a.practiceSession.difficulty):selectedSession();}
  catch(e){node.status.textContent=message('failed')+e.message;return;}
  const controller=new AbortController(),mine=++ticket,owner={controller},current=()=>!disposed&&ticket===mine&&!controller.signal.aborted;
  pending=owner;node.status.textContent=message('loading');refresh();
  try{
   const receipt=await launch(selected,{signal:controller.signal,isCurrent:current});if(!current())return;
   if(receipt?.stageId!==selected.params.stage+1||receipt.difficulty!==selected.difficulty||JSON.stringify(receipt.session?.params)!==JSON.stringify(selected.params)||selected.params.mode&&(receipt.nativeTainted!==true||receipt.normalModeClear!==false))throw new Error('TH128 native practice receipt / 身份与 taint 未确认');
   node.status.textContent=message(selected.params.mode?'started':'normal');root.hidden=true;node.replace.checked=false;opener.setAttribute('aria-expanded','false');onFocusGame();
  }catch(e){if(current())node.status.textContent=message('failed')+e.message;}
  finally{if(pending===owner)pending=null;if(!disposed)refresh();}
 }
 node.mode.value='0';options(node.locale,[{value:'zh',label:'中文'},{value:'en',label:'English'},{value:'ja',label:'日本語'}],settings.locale);node.startPoint.value=node.path.value=node.stage.value=node.warp.value='0';node.difficulty.value='1';node.enabled.checked=settings.enabled;
 for(const key of resourceNames)node[key].value=String(THPRAC128_DEFAULTS[key]);
 root.hidden=true;opener.setAttribute('aria-expanded','false');node.status.textContent=message('ready');
 listen(opener,'click',()=>{void open();});listen(node.start,'click',()=>{void begin();});listen(node.restart,'click',()=>{void begin(true);});listen(node.cancel,'click',back);
 for(const key of ['mode','startPoint','path','stage','warp'])listen(node[key],'change',()=>{if(key==='startPoint'){node.path.value=node.stage.value='0';node.difficulty.value=node.startPoint.value==='3'?'4':'1';if(routeOpen)onRouteChange(Number(node.startPoint.value));}node.section.value='';node.phase.value='0';node.replace.checked=false;refresh();});
 listen(node.section,'change',()=>{node.phase.value='0';node.replace.checked=false;refresh();});
 for(const key of resourceNames)listen(node[key],'input',()=>{const output=root.querySelector('[data-thprac-value="'+key+'"]');if(output)output.textContent=thprac128ResourceText(key,Number(node[key].value));});
 for(const key of ['locale','difficulty','enabled'])listen(node[key],'change',()=>{if(key!=='difficulty')onSettings({enabled:node.enabled.checked,locale:language()});refresh();});
 function back(){close();onCancel();}
 function syncRoute(selection,difficulty){
  if(!routeOpen||root.hidden||!Number.isInteger(selection)||selection<0||selection>3)return;
  const point=String(selection),rank=String(selection===3?4:difficulty);
  if(node.startPoint.value===point&&node.difficulty.value===rank)return;
  node.startPoint.value=point;refresh({routeDifficulty:rank});
 }
 function keyDown(event,fromRoot=false){
  if(disposed||root.hidden)return false;const code=event.code;
  if(routeOpen&&index('mode')===0&&['ArrowLeft','ArrowRight','KeyZ','Enter'].includes(code)){
   if(['KeyZ','Enter'].includes(code))close({focus:false});
   if(fromRoot){event.preventDefault();event.stopPropagation();onRouteKey(code);return true;}return false;
  }
  if(!['KeyZ','Enter','KeyX','Escape','Tab'].includes(code)){if(fromRoot)event.stopPropagation();return true;}
  event.stopPropagation();
  if(['KeyZ','Enter'].includes(code)){event.preventDefault();void begin();}
  else if(['Escape','KeyX'].includes(code)){event.preventDefault();back();}
  else if(code==='Tab'){const nodes=[...root.querySelectorAll('button,select,input')].filter(n=>!n.disabled&&!n.hidden);const active=root.ownerDocument.activeElement;if(nodes.length&&(event.shiftKey?active===nodes[0]:active===nodes.at(-1))){event.preventDefault();(event.shiftKey?nodes.at(-1):nodes[0]).focus();}}return true;
 }
 listen(root,'keydown',event=>keyDown(event,true));
 listen(root,'keyup',event=>event.stopPropagation());refresh();
 return Object.freeze({get isOpen(){return !disposed&&!root.hidden;},refresh,open,close,keyDown,back,syncRoute,dispose(){if(disposed)return;disposed=true;close({focus:false});for(const remove of listeners)remove();opener.disabled=true;}});
}
