import {copyRect,rectChange,layoutEntryChange,addChange} from './layout-watch-core.mjs';
import {inspectInputAlignment,inspectMastheadLayers,inspectPopupLayers,inspectSurfaceMotion,inspectLayoutMotion} from './probe.mjs';

const $=id=>document.getElementById(id),frame=$('preview'),records=[];
const clock=()=>performance.now();
let target,cleanup=()=>{},recording=true,sequence=0,selected=null,renderTimer=0,markTimer=0;
const escape=value=>String(value).replaceAll('&','&amp;').replaceAll('<','&lt;').replaceAll('"','&quot;');
const route=()=>{
  const url=new URL(target.location.href),game=url.searchParams.get('game');
  return url.pathname+(game?'?game='+encodeURIComponent(game):'');
};
function label(node){
  const element=node?.nodeType===1?node:node?.parentElement;
  if(!element)return '页面';
  const own=element.getAttribute('aria-label')||element.getAttribute('title');
  if(own)return own.slice(0,80);
  if(element.matches('input,textarea,select'))return element.labels?.[0]?.textContent.trim()||element.id||'输入控件';
  if(element.matches('button,a,[role=tab],[role=switch]'))return element.textContent.trim().replace(/\s+/g,' ').slice(0,60)||element.id||'图标按钮';
  const heading=element.querySelector(':scope>header h2,:scope>header h3,:scope>h2,:scope>h3');
  const names={'book-masthead':'页面顶栏','book-library':'游戏书库','book-card':'作品封面','input-binding-table':'按键列表','input-config-hint':'键位说明','input-layout-toolbar':'布局选择栏','input-config-footer':'底部操作','input-device-tabs':'设备标签栏','options-scroll':'选项内容','site-footer':'页脚','tools-head':'作品封面与标题'};
  return heading?.textContent.trim()||Object.entries(names).find(([name])=>element.classList.contains(name))?.[1]||element.id||element.classList[0]||element.tagName.toLowerCase();
}
function describe(delta){
  return Object.entries(delta).filter(([,value])=>Math.abs(value)>=1).map(([key,value])=>`${({x:'横移',y:'纵移',width:'宽度',height:'高度'})[key]} ${value>0?'+':''}${Math.round(value)} px`).join(' | ');
}
function scheduleRender(){if(!renderTimer)renderTimer=setTimeout(()=>{renderTimer=0;render();},120);}
function remember(kind,sources,detail={},action='无直接操作'){
  if(!recording&&kind!=='inspection')return;
  const record=addChange(records,{id:++sequence,time:clock(),kind,path:route(),action,sources,amount:Math.max(0,...sources.map(s=>s.amount)),...detail});
  scheduleRender();
  if(!$('freeze').checked)showMarks(record,1200);
}
function render(){
  $('changeCount').textContent=records.filter(record=>!record.expected).length;
  $('watchToggle').dataset.paused=String(!recording);
  $('pause').textContent=recording?'暂停':'继续记录';
  $('empty').hidden=records.length>0;
  $('changes').innerHTML=[...records].reverse().map(record=>{
    const title=({geometry:'位置与尺寸变化',layout:'浏览器布局偏移',inspection:'页面检查'})[record.kind];
    const lines=record.issues?.join('；')||record.sources.slice(0,3).map(source=>`${source.label}：${describe(source.delta??{})}`).join('；');
    return `<li data-id="${record.id}" data-expected="${record.expected}" data-selected="${selected===record.id}"><button class="watch-record" data-select="${record.id}"><strong>${title}<span>${record.amount?Math.round(record.amount)+' px':''}</span></strong><p>${escape(lines||'页面布局发生变化')}</p><small>${escape(record.action)} | ${escape(record.path)}</small></button><button class="watch-expected" data-expected="${record.id}">${record.expected?'取消预期标记':'标为预期变化'}</button></li>`;
  }).join('');
}
function showMarks(record,duration=0){
  clearTimeout(markTimer);$('marks').replaceChildren();
  if(!record||!target||record.path!==route())return;
  for(const source of record.sources.slice(0,5))for(const state of ['before','after']){
    const rect=source[state];if(!rect?.width||!rect?.height)continue;
    const box=document.createElement('div');box.className='watch-mark'+(state==='before'?' before':'');
    Object.assign(box.style,{left:rect.x+'px',top:rect.y+'px',width:rect.width+'px',height:rect.height+'px'});
    if(state==='after'){const caption=document.createElement('span');caption.className='watch-mark-label';caption.textContent=source.label;box.append(caption);}
    $('marks').append(box);
  }
  if(duration)markTimer=setTimeout(()=>$('marks').replaceChildren(),duration);
}
const issueNames={
  'layout-transition':'控件存在会触发排版的过渡','layout-animation':'控件正在反复排版',
  'feedback-duration':'控件反馈时长被其他样式覆盖',
  'surface-css-transition':'浮层仍有重复的 CSS 过渡','surface-css-animation':'浮层仍有重复的 CSS 动画',
  'masthead-behind-page':'顶栏被页面内容盖住','popup-behind-dialog':'下拉菜单被弹窗盖住','input save/capture status hidden':'保存或等待按键的提示被隐藏','input horizontal overflow':'输入设置发生横向溢出',
  'field control column':'选择器起点不一致','field right edge':'字段右边界不一致','binding column':'键帽与表头未对齐','binding/reset center':'按键与恢复按钮未居中','control center':'同排控件未居中','control height':'同排控件高度不同','footer actions right':'底部操作未右对齐','threshold left':'滑块左边界不一致','threshold right':'滑块右边界不一致',
};
function inspect(){
  if(!target)return;
  const results=[inspectInputAlignment(target),inspectMastheadLayers(target),inspectPopupLayers(target),inspectSurfaceMotion(target),inspectLayoutMotion(target)];
  const issues=results.flatMap(result=>result.errors).map(error=>{const [key,...values]=error.split(':');return (issueNames[key]||'内容左边界不一致')+(values.length?'：'+values.join(':'):'');});
  if(issues.length){
    const header=target.document.querySelector('.book-masthead'),input=target.document.querySelector('.input-settings-dialog');
    const node=input||header,r=node&&copyRect(node.getBoundingClientRect());
    remember('inspection',r?[{key:'inspection',label:input?'输入设置':'页面顶栏',before:r,after:r,amount:0}]:[],{issues},'检查当前页');
  }
  $('watchStatus').textContent=issues.length?`${issues.length} 处待确认`:'当前检查通过';
}
function connect(){
  cleanup();target=null;
  try{target=frame.contentWindow;if(target.location.origin!==location.origin)throw new Error('origin');}
  catch{target=null;$('watchStatus').textContent='请回到本地启动器';return;}
  $('watchStatus').textContent=recording?'正在记录':'已暂停';
  const doc=target.document,win=target,off=[];
  let nodes=[],previous=new Map(),keys=new WeakMap(),keySequence=0,raf=0,until=0,lastSample=0,rebuild=true,lastScroll=0,settleTimer=0;
  let lastAction={time:-Infinity,label:'页面加载'},lastResize=-Infinity,size={width:win.innerWidth,height:win.innerHeight};
  const listen=(node,type,handler,options)=>{node.addEventListener(type,handler,options);off.push(()=>node.removeEventListener(type,handler,options));};
  const key=node=>{if(!keys.has(node))keys.set(node,'element-'+(++keySequence));return keys.get(node);};
  const visible=node=>{const r=node.getBoundingClientRect();return r.width>0&&r.height>0&&r.bottom>0&&r.right>0&&r.top<win.innerHeight&&r.left<win.innerWidth&&win.getComputedStyle(node).visibility!=='hidden';};
  const structural='.book-masthead,.book-library,#bookGameGrid,.tools.is-open,.tools-head,.options-scroll,.input-settings-dialog,.input-settings-body>header,.input-device-tabs,.input-layout-toolbar,.input-binding-table,.input-config-footer,.book-directory,.profile-wrap,#mpRoomView,#player.replay-workspace,[role=dialog],[role=tablist]';
  function collect(){
    const unique=new Set([...doc.querySelectorAll(structural),...doc.querySelectorAll('button,a[href],input:not([type=hidden]),[role=slider]')]);
    nodes=[...unique].filter(node=>!node.closest('[hidden],[inert],[aria-hidden="true"]')&&visible(node)).slice(0,180);rebuild=false;
  }
  function wake(duration=650){
    if(!recording||doc.hidden)return;
    until=Math.max(until,clock()+duration);
    if(!raf)raf=requestAnimationFrame(sample);
    clearTimeout(settleTimer);settleTimer=setTimeout(()=>{if(recording)inspectSilently();},duration+100);
  }
  let lastIssues='';
  function inspectSilently(){
    const problems=[...inspectInputAlignment(win).errors,...inspectMastheadLayers(win).errors,...inspectPopupLayers(win).errors,...inspectLayoutMotion(win).errors];
    const signature=problems.join('|');
    if(signature&&signature!==lastIssues){inspect();}
    lastIssues=signature;
  }
  function moving(node,cache){
    if(!node||node===doc.body)return false;
    if(cache.has(node))return cache.get(node);
    const own=node.getAnimations?.().some(animation=>animation.playState==='running'&&animation.effect?.getKeyframes().some(frame=>Object.keys(frame).some(key=>/^(transform|translate|scale|rotate|width|height|top|left|right|bottom|maxHeight|gridTemplateRows)$/.test(key))));
    const value=!!own||moving(node.parentElement,cache);cache.set(node,value);return value;
  }
  function sample(time){
    raf=0;if(!recording)return;
    if(time-lastSample<32){raf=requestAnimationFrame(sample);return;}lastSample=time;
    if(rebuild)collect();
    document.documentElement.dataset.theme=doc.documentElement.dataset.bookTheme||'light';
    const resizing=size.width!==win.innerWidth||size.height!==win.innerHeight||clock()-lastResize<250;
    size={width:win.innerWidth,height:win.innerHeight};
    const next=new Map(),changes=[],motion=new Map(),scrolling=clock()-lastScroll<150;
    for(const node of nodes){
      if(!node.isConnected)continue;
      const rect=copyRect(node.getBoundingClientRect());next.set(node,rect);
      if(!$('includeMotion').checked&&moving(node,motion))continue;
      const change=rectChange(previous.get(node),rect,{threshold:+$('threshold').value,scrolling,resizing});
      if(change)changes.push({key:key(node),label:label(node),...change});
    }
    previous=next;
    if(changes.length)remember('geometry',changes.sort((a,b)=>b.amount-a.amount).slice(0,8),{},clock()-lastAction.time<2000?lastAction.label:'无直接操作');
    if(clock()<until)raf=requestAnimationFrame(sample);
  }
  let layoutObserver;
  if(win.PerformanceObserver?.supportedEntryTypes.includes('layout-shift')){
    layoutObserver=new win.PerformanceObserver(list=>{
      if(!recording)return;
      for(const entry of list.getEntries()){
        if(entry.value<.00001||clock()-lastResize<250)continue;
        const sources=entry.sources.map(source=>{
          const change=layoutEntryChange(source.previousRect,source.currentRect,source.node?.getBoundingClientRect?.(),previous.get(source.node),+$('threshold').value);
          return change?{key:source.node?key(source.node):'unknown',label:label(source.node),...change}:null;
        }).filter(Boolean);
        if(!sources.length)continue;
        remember('layout',sources,{score:entry.value,hadRecentInput:entry.hadRecentInput},clock()-lastAction.time<2000?lastAction.label:'无直接操作');
      }
    });layoutObserver.observe({type:'layout-shift',buffered:false});
  }
  const mutation=new win.MutationObserver(()=>{rebuild=true;wake();});
  mutation.observe(doc.documentElement,{subtree:true,childList:true,attributes:true,attributeFilter:['class','style','hidden','open','aria-expanded']});
  const resize=new win.ResizeObserver(()=>wake());resize.observe(doc.documentElement);
  listen(doc,'pointerdown',event=>{const control=event.target.closest('button,a,input,select,[role=tab],[role=switch]');lastAction={time:clock(),label:'点击 '+label(control||event.target)};wake();},true);
  listen(doc,'keydown',event=>{lastAction={time:clock(),label:'键盘操作 | '+label(event.target)};wake();},true);
  listen(doc,'scroll',()=>{lastScroll=clock();wake(200);},true);
  for(const type of ['transitionrun','animationstart','transitionend','animationend'])listen(doc,type,()=>wake(800),true);
  listen(win,'resize',()=>{lastResize=clock();previous.clear();rebuild=true;wake();});
  listen(doc,'visibilitychange',()=>{if(!doc.hidden){previous.clear();wake();}});
  cleanup=()=>{off.forEach(stop=>stop());mutation.disconnect();resize.disconnect();layoutObserver?.disconnect();cancelAnimationFrame(raf);clearTimeout(settleTimer);};
  wake(1200);
}
$('watchToggle').addEventListener('click',()=>{const open=$('watchPanel').hidden;$('watchPanel').hidden=!open;$('watchToggle').setAttribute('aria-expanded',String(open));});
$('collapse').addEventListener('click',()=>{$('watchPanel').hidden=true;$('watchToggle').setAttribute('aria-expanded','false');});
$('pause').addEventListener('click',()=>{recording=!recording;if(recording)connect();else{cleanup();$('watchStatus').textContent='已暂停';}render();});
$('inspect').addEventListener('click',inspect);
$('clear').addEventListener('click',()=>{records.length=0;selected=null;showMarks(null);render();$('watchStatus').textContent=recording?'正在记录':'已暂停';});
$('freeze').addEventListener('change',()=>{if(!$('freeze').checked)showMarks(null);});
$('changes').addEventListener('click',event=>{
  const select=event.target.closest('[data-select]'),expected=event.target.closest('button[data-expected]');
  const id=+(select?.dataset.select??expected?.dataset.expected??0),record=records.find(row=>row.id===id);if(!record)return;
  if(expected)record.expected=!record.expected;
  else{selected=id;$('freeze').checked=true;showMarks(record);}
  render();
});
$('export').addEventListener('click',()=>{
  const data={schema:'eagler-ui-layout-watch/1',date:new Date().toISOString(),threshold:+$('threshold').value,records};
  const url=URL.createObjectURL(new Blob([JSON.stringify(data,null,2)],{type:'application/json'}));
  const anchor=document.createElement('a');anchor.href=url;anchor.download='launcher-layout-changes.json';anchor.click();setTimeout(()=>URL.revokeObjectURL(url),1000);
});
frame.addEventListener('load',connect);
const page=new URLSearchParams(location.search).get('page');
frame.src=document.body.dataset.appBase+({lobby:'lobby.html',profile:'profile.html',en:'en.html'}[page]||'');
window.addEventListener('pagehide',()=>cleanup());
