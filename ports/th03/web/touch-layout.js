// TH06 launcher workbench adapted to TH03 actions. Normalized safe-area centers,
// per-orientation drafts, 0.6..1.8 scale, stacking order and horizontal viewport.
export function mountTouchLayout(host,{reset,solo=false,storagePrefix='th03.touch'}){
  const panel=host.querySelector('.touch-layout-editor');
  panel.innerHTML=`<header class="touch-workbench-header"><div class="touch-layout-editor-copy"><span class="touch-workbench-grip">⠿</span><strong>按键与触控</strong><em data-orientation></em></div><button class="touch-workbench-collapse" data-collapse aria-label="收起面板" aria-expanded="true">⌃</button></header>
    <div class="touch-workbench-body"><div class="touch-workbench-scroll"><section class="touch-layout-arrangement"><div class="touch-layout-secondary-actions"><button data-orientation-help>切换横竖屏</button><button data-reset>恢复本方向默认</button></div><p class="touch-workbench-note" role="status">拖动按键移动，拖动右下角缩放。横屏与竖屏分别保存。</p><label>选中按键 <select data-selected></select></label><label class="touch-layout-size">大小 <input data-size type="range" min="0.6" max="1.8" step="0.05" value="1"><output data-scale>100%</output></label><div class="touch-layout-secondary-actions"><button data-back>移至底层</button><button data-front>移至顶层</button></div></section><hr class="touch-workbench-divider"><div class="layout-settings"></div><label>游戏画面水平位置 <input data-viewport type="range" min="-0.5" max="0.5" step="0.01" value="0"></label><button data-viewport-reset>画面复位</button><p class="touch-workbench-note">游戏不会自动暂停，请先暂停再修改。</p></div><footer class="touch-workbench-footer"><small>触控设置自动保存；布局点击保存后生效。</small><div class="touch-layout-editor-actions"><button data-close>退出</button><button data-save>保存布局</button></div></footer></div>`;
  const movement=host.querySelector('.player-movement');panel.querySelector('.layout-settings').append(movement);
  panel.querySelector('.layout-settings').append(host.querySelector('.player-toolbar'));
  const opener=document.createElement('button');opener.className='touch-help-open';opener.textContent='按键与触控';host.append(opener);
  const full=document.createElement('button');full.className='touch-full-open';full.textContent='⛶';full.setAttribute('aria-label','进入全屏');full.onclick=()=>host.querySelector('[data-full]').click();host.append(full);
  const safe=document.createElement('div');safe.className='touch-layout-safe-zone';host.append(safe);
  const hud=host.querySelector('.player-actions');
  const definitions=[['focus','[data-held="64"]','低速','按住低速'],['fire','[data-charge]','蓄力','按住蓄力'],['bomb','[data-pulse="16"]','X','攻击'],['escape','[data-pulse="128"]','ESC','']];
  if(hud.querySelector('[data-rapid]'))definitions.push(['rapid','[data-rapid]','连发','按住 · 最高速']);
  const controls=definitions.filter(([name])=>!solo||name!=='rescue').map(([name,selector,title,hint])=>{const button=hud.querySelector(selector);button.dataset.layoutControl=name;button.classList.add('touch-'+name);button.innerHTML=`<strong>${title}</strong><small>${hint}</small><i class="layout-resize" aria-hidden="true">↘</i>`;host.append(button);return button;});
  hud.remove();
  for(const button of movement.querySelectorAll('[data-pulse]')){button.dataset.layoutControl=({1:'up',2:'down',4:'left',8:'right'})[button.dataset.pulse];button.innerHTML=`<strong>${({1:'↑',2:'↓',4:'←',8:'→'})[button.dataset.pulse]}</strong><i class="layout-resize" aria-hidden="true">↘</i>`;host.append(button);controls.push(button);}
  const clone=value=>JSON.parse(JSON.stringify(value));
  const empty=()=>({landscape:{controls:{},viewport:0},portrait:{controls:{},viewport:0}});
  const valid=p=>p&&[p.x,p.y,p.scale].every(Number.isFinite)&&p.x>=0&&p.x<=1&&p.y>=0&&p.y<=1&&p.scale>=.6&&p.scale<=1.8;
  let profiles=empty(),draft=null,editing=false,selected=controls[1],gesture=null,orientation='',windows={};
  const storageKey=`${storagePrefix}.layout.v2`;
  try{
    const saved=JSON.parse(localStorage.getItem(storageKey));
    if(saved)for(const key of ['landscape','portrait']){
      const p=saved[key];if(!p)continue;
      for(const button of controls){const name=button.dataset.layoutControl;if(valid(p.controls?.[name]))profiles[key].controls[name]=p.controls[name];}
      if(Number.isFinite(p.viewport))profiles[key].viewport=Math.max(-.5,Math.min(.5,p.viewport));
    }
    // Preserve existing per-direction placements when upgrading the old editor.
    if(!saved){
      const legacy=JSON.parse(localStorage.getItem(`${storagePrefix}.layout.v1`));
      for(const key of ['landscape','portrait'])for(const button of controls){
        const name=button.dataset.layoutControl,p=legacy?.[key]?.[name];
        if(valid(p))profiles[key].controls[name]={...p,priority:controls.indexOf(button)};
      }
    }
    windows=JSON.parse(localStorage.getItem(`${storagePrefix}.layout.windows`))||{};
  }catch{}
  const select=panel.querySelector('[data-selected]'),size=panel.querySelector('[data-size]'),viewport=panel.querySelector('[data-viewport]');
  for(const button of controls)select.add(new Option(button.querySelector('strong').textContent,button.dataset.layoutControl));
  function current(){return (editing?draft:profiles)[orientation];}
  function safeRect(){return safe.getBoundingClientRect();}
  function placePanel(){
    const p=windows[orientation];panel.style.transform=p?'none':'';
    panel.style.left=p?`${Math.max(0,Math.min(1,Number(p.x)||0)) * Math.max(0,host.clientWidth-panel.offsetWidth)}px`:'';
    panel.style.top=p?`${Math.max(0,Math.min(1,Number(p.y)||0)) * Math.max(0,host.clientHeight-panel.offsetHeight)}px`:'';
  }
  function apply(){
    const next=host.clientWidth>=host.clientHeight?'landscape':'portrait';
    if(next!==orientation){orientation=next;gesture=null;placePanel();}
    panel.querySelector('[data-orientation]').textContent=orientation==='landscape'?'横屏':'竖屏';
    const area=safeRect(),h=host.getBoundingClientRect(),profile=current();
    for(const [index,button] of controls.entries()){
      const p=profile.controls[button.dataset.layoutControl];button.classList.toggle('custom-position',!!valid(p));
      button.classList.toggle('touch-layout-selected',editing&&button===selected);
      if(valid(p)){
        const mx=Math.min(.48,(button.offsetWidth*p.scale/2+6)/area.width),my=Math.min(.48,(button.offsetHeight*p.scale/2+6)/area.height);
        button.style.left=`${area.left-h.left+Math.max(mx,Math.min(1-mx,p.x))*area.width}px`;
        button.style.top=`${area.top-h.top+Math.max(my,Math.min(1-my,p.y))*area.height}px`;
        button.style.setProperty('--control-scale',p.scale);
      }else{button.style.left='';button.style.top='';button.style.removeProperty('--control-scale');}
      button.style.zIndex=String(31+(Number.isFinite(p?.priority)?Math.max(0,Math.min(controls.length-1,p.priority)):index));
    }
    host.style.setProperty('--viewport-offset',`${profile.viewport*host.clientWidth}px`);
    viewport.value=profile.viewport;select.value=selected.dataset.layoutControl;
    size.value=profile.controls[selected.dataset.layoutControl]?.scale||1;
    panel.querySelector('[data-scale]').value=`${Math.round(Number(size.value)*100)}%`;
  }
  function close(){editing=false;draft=null;gesture=null;panel.hidden=true;host.classList.remove('touch-layout-edit');apply();reset();}
  function open(){reset();draft=clone(profiles);editing=true;panel.hidden=false;host.classList.add('touch-layout-edit');apply();placePanel();}
  opener.onclick=open;host.querySelector('[data-layout]').onclick=()=>{select.focus();};
  panel.querySelector('[data-close]').onclick=close;
  panel.querySelector('[data-save]').onclick=()=>{profiles=clone(draft);try{localStorage.setItem(storageKey,JSON.stringify(profiles));close();}catch{panel.querySelector('[role=status]').textContent='已应用到本页；浏览器无法保存，刷新后会恢复。';}};
  panel.querySelector('[data-reset]').onclick=()=>{draft[orientation]={controls:{},viewport:0};apply();};
  panel.querySelector('[data-orientation-help]').onclick=()=>{panel.querySelector('[role=status]').textContent='请旋转设备切换横竖屏；如不能旋转，请关闭系统的旋转锁定。两种方向的布局分别保存。';};
  panel.querySelector('[data-collapse]').onclick=event=>{const body=panel.querySelector('.touch-workbench-body');body.hidden=!body.hidden;panel.classList.toggle('is-collapsed',body.hidden);event.currentTarget.setAttribute('aria-expanded',String(!body.hidden));event.currentTarget.setAttribute('aria-label',body.hidden?'展开面板':'收起面板');placePanel();};
  function placement(button){const r=button.getBoundingClientRect(),h=safeRect();return {x:Math.max(0,Math.min(1,(r.left+r.width/2-h.left)/h.width)),y:Math.max(0,Math.min(1,(r.top+r.height/2-h.top)/h.height)),scale:current().controls[button.dataset.layoutControl]?.scale||1,priority:current().controls[button.dataset.layoutControl]?.priority??controls.indexOf(button)};}
  select.onchange=()=>{selected=controls.find(b=>b.dataset.layoutControl===select.value);apply();};
  size.oninput=()=>{current().controls[selected.dataset.layoutControl]={...placement(selected),scale:Number(size.value)};apply();};
  function reorder(front){const ordered=controls.toSorted((a,b)=>placement(a).priority-placement(b).priority).filter(b=>b!==selected);front?ordered.push(selected):ordered.unshift(selected);ordered.forEach((button,priority)=>{current().controls[button.dataset.layoutControl]={...placement(button),priority};});apply();}
  panel.querySelector('[data-front]').onclick=()=>reorder(true);panel.querySelector('[data-back]').onclick=()=>reorder(false);
  viewport.oninput=()=>{current().viewport=Number(viewport.value);apply();};panel.querySelector('[data-viewport-reset]').onclick=()=>{current().viewport=0;apply();};
  for(const button of controls){
    button.addEventListener('pointerdown',event=>{if(!editing)return;event.preventDefault();event.stopImmediatePropagation();reset();selected=button;const p=placement(button);current().controls[button.dataset.layoutControl]=p;gesture={id:event.pointerId,x:event.clientX,y:event.clientY,p:{...p},resize:event.target.closest('.layout-resize')!==null};button.setPointerCapture(event.pointerId);apply();},true);
    button.addEventListener('pointermove',event=>{if(!editing||!gesture||gesture.id!==event.pointerId)return;event.preventDefault();event.stopImmediatePropagation();const g=gesture,r=safeRect();current().controls[button.dataset.layoutControl]=g.resize?{...g.p,scale:Math.max(.6,Math.min(1.8,g.p.scale+(event.clientX-g.x)/100))}:{...g.p,x:Math.max(0,Math.min(1,g.p.x+(event.clientX-g.x)/r.width)),y:Math.max(0,Math.min(1,g.p.y+(event.clientY-g.y)/r.height))};apply();},true);
    for(const type of ['pointerup','pointercancel','lostpointercapture'])button.addEventListener(type,event=>{if(gesture?.id===event.pointerId)gesture=null;});
    button.addEventListener('click',event=>{if(editing){event.preventDefault();event.stopImmediatePropagation();}},true);
  }
  const header=panel.querySelector('header');let panelDrag;
  header.onpointerdown=e=>{if(e.target.closest('button'))return;e.preventDefault();const r=panel.getBoundingClientRect(),h=host.getBoundingClientRect();panelDrag={id:e.pointerId,x:e.clientX,y:e.clientY,left:r.left-h.left,top:r.top-h.top};header.setPointerCapture(e.pointerId);};
  header.onpointermove=e=>{if(panelDrag?.id!==e.pointerId)return;const maxX=Math.max(0,host.clientWidth-panel.offsetWidth),maxY=Math.max(0,host.clientHeight-panel.offsetHeight);const left=Math.max(0,Math.min(maxX,panelDrag.left+e.clientX-panelDrag.x)),top=Math.max(0,Math.min(maxY,panelDrag.top+e.clientY-panelDrag.y));windows[orientation]={x:maxX?left/maxX:0,y:maxY?top/maxY:0};placePanel();};
  header.onpointerup=header.onpointercancel=()=>{panelDrag=null;try{localStorage.setItem(`${storagePrefix}.layout.windows`,JSON.stringify(windows));}catch{}};
  new ResizeObserver(()=>{reset();apply();placePanel();}).observe(host);
  apply();return {isEditing:()=>editing,open,close};
}
