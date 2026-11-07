/** Read-only geometry used by browser regression checks and frame sampling. */
export function readUiLayout(source=window) {
  const {document,innerWidth,innerHeight,scrollX,scrollY}=source;
  const selectors=['.book-masthead','.book-library','#bookGameGrid','.book-card','.book-sort','.tools.is-open','.tools.is-open .options-scroll','.tools.is-open .tools-head','.input-settings-dialog','.input-settings-body>header','.input-context-toolbar','.input-settings-body>.input-device-tabs','.profile-wrap','#mpRoomView','#player.replay-workspace'];
  return {
    viewport:{width:innerWidth,height:innerHeight,contentWidth:document.documentElement.getBoundingClientRect().width},
    pageScroll:{x:scrollX,y:scrollY},
    body:{position:source.getComputedStyle(document.body).position,marginRight:source.getComputedStyle(document.body).marginRight},
    overlays:[...document.querySelectorAll('.ui-kit-dialog-overlay')].map(element=>{
      const {x,y,width,height}=element.getBoundingClientRect();return{x,y,width,height};
    }),
    regions:Object.fromEntries(selectors.map(selector=>{
      const element=document.querySelector(selector);
      if(!element)return [selector,null];
      const {x,y,width,height}=element.getBoundingClientRect();
      return [selector,{x,y,width,height,scrollTop:element.scrollTop}];
    })),
  };
}

export function layoutDifferences(before,after,tolerance=.5) {
  const failures=[];
  for(const overlay of after.overlays??[])if(overlay.width&&(
    overlay.x>tolerance||overlay.y>tolerance||overlay.x+overlay.width<after.viewport.contentWidth-tolerance||overlay.y+overlay.height<after.viewport.height-tolerance
  ))failures.push('dialog-overlay-gap');
  for(const key of ['x','y'])if(Math.abs(before.pageScroll[key]-after.pageScroll[key])>tolerance)failures.push(`page-scroll-${key}`);
  for(const [selector,original]of Object.entries(before.regions)){
    const current=after.regions[selector];
    if(!original||!current)continue;
    for(const key of ['x','y','width','height','scrollTop'])if(Math.abs(original[key]-current[key])>tolerance)failures.push(`${selector}:${key} ${original[key]} → ${current[key]}`);
  }
  return failures;
}

/** Shared surface presentation must not also run delayed CSS transitions. */
export function inspectSurfaceMotion(source=window) {
  const errors=[],properties=['opacity','translate','transform'];let checks=0;
  const seconds=value=>{const number=Number.parseFloat(value);return Number.isFinite(number)?number/(value.trim().endsWith('ms')?1000:1):0;};
  const selectors='.tools,.library-backdrop,.settings-room-backdrop,.mp-settings-room-drawer,.informational-dialog,.first-use-notice-dialog,.replay-dialog,.game-data-import-window,.game-data-link-window,.mp-room-panel,.toast';
  for(const element of source.document.querySelectorAll(selectors)) {
    checks++;
    const style=source.getComputedStyle(element),name=element.id||element.classList[0];
    const names=style.transitionProperty.split(',').map(value=>value.trim());
    const durations=style.transitionDuration.split(',').map(seconds),delays=style.transitionDelay.split(',').map(seconds);
    if(names.some((property,index)=>(property==='all'||properties.includes(property))&&
      (durations[index%durations.length]>0||delays[index%delays.length]>0)))errors.push('surface-css-transition:'+name);
    for(const animation of element.getAnimations?.()??[]) {
      if(!animation.animationName)continue;
      if(animation.effect?.getKeyframes().some(frame=>properties.some(property=>property in frame)))errors.push('surface-css-animation:'+name);
    }
  }
  return {errors,checks};
}

/** Inspect ordinary controls too: geometry tweens and overridden feedback. */
export function inspectLayoutMotion(source=window) {
  const errors=[];let checks=0;
  const layout=/^(?:width|height|min-width|max-width|min-height|max-height|top|right|bottom|left|inset(?:-.+)?|margin(?:-.+)?|padding(?:-.+)?|gap|row-gap|column-gap|grid-template-(?:rows|columns)|flex-basis|font-size|line-height|border(?:-.+)?-width)$/;
  const seconds=value=>{const n=Number.parseFloat(value);return Number.isFinite(n)?n/(value.trim().endsWith('ms')?1000:1):0;};
  for(const element of source.document.querySelectorAll('body *')) {
    if(element.closest('[hidden],[inert],[aria-hidden="true"]'))continue;
    const r=element.getBoundingClientRect();
    if(!r.width||!r.height||r.right<=0||r.bottom<=0||r.left>=source.innerWidth||r.top>=source.innerHeight)continue;
    const style=source.getComputedStyle(element);
    if(style.visibility==='hidden')continue;
    checks++;
    const name=element.id||element.classList[0]||element.tagName.toLowerCase();
    const names=style.transitionProperty.split(',').map(value=>value.trim());
    const durations=style.transitionDuration.split(',').map(seconds),delays=style.transitionDelay.split(',').map(seconds);
    if(element.matches?.('button[data-ui-variant],button[data-slot="button"]')) {
      const feedback=seconds(style.getPropertyValue('--ui-feedback-duration'));
      if(feedback>0&&durations.some(duration=>duration>0&&Math.abs(duration-feedback)>.001))errors.push('feedback-duration:'+name+':'+style.transitionDuration);
    }
    const unsafe=names.filter((property,index)=>(property==='all'||layout.test(property))&&
      (durations[index%durations.length]>0||delays[index%delays.length]>0));
    if(unsafe.length)errors.push('layout-transition:'+name+':'+unsafe.join(','));
    for(const animation of element.getAnimations?.()??[]) {
      if(animation.playState!=='running')continue;
      const properties=new Set((animation.effect?.getKeyframes()??[]).flatMap(frame=>Object.keys(frame))
        .map(property=>property.replace(/[A-Z]/g,char=>'-'+char.toLowerCase())).filter(property=>layout.test(property)));
      if(properties.size)errors.push('layout-animation:'+name+':'+[...properties].join(','));
    }
  }
  return {errors,checks};
}

/** Record a real interaction in the browser; this helper never clicks controls. */
export function captureSurfaceMotion({selector='.tools',duration=1000,waitForInput=true,source=window}={}) {
  return new Promise(resolve=>{
    const doc=source.document,frames=[],inputOptions={capture:true};
    let started=false,settled=false,raf=0,timer=0,startTime=0;
    const reduced=!!source.matchMedia?.('(prefers-reduced-motion: reduce)').matches||!!doc.body.closest('.less-motion');
    const read=()=>{
      const element=doc.querySelector(selector),style=element&&source.getComputedStyle(element),rect=element?.getBoundingClientRect();
      const visible=!!element&&!element.hidden&&style.visibility!=='hidden'&&style.display!=='none'&&rect.width>0&&rect.height>0;
      const closing=element&&(element.classList.contains('closing')||element.hasAttribute('inert')||element.matches('.tools')&&doc.body.classList.contains('library-tools-closing'));
      const animations=(element?.getAnimations?.()??[]).flatMap(animation=>{
        const properties=['opacity','translate','transform'].filter(property=>animation.effect?.getKeyframes().some(frame=>property in frame));
        return properties.length?[{type:animation.constructor.name,state:animation.playState,properties}]:[];
      });
      return {time:source.performance.now()-startTime,phase:visible?closing?'exit':'enter':'closed',visible,opacity:element?+style.opacity:0,animations};
    };
    const clear=()=>{
      source.cancelAnimationFrame(raf);source.clearTimeout(timer);
      doc.removeEventListener('pointerdown',start,inputOptions);doc.removeEventListener('click',start,inputOptions);doc.removeEventListener('keydown',start,inputOptions);
      source.removeEventListener('pagehide',depart);
    };
    const finish=error=>{
      if(settled)return;settled=true;clear();
      resolve({selector,reduced,theme:doc.documentElement?.dataset.bookTheme,inputMode:waitForInput?'interaction':'automatic',frames,error,viewport:{width:source.innerWidth,height:source.innerHeight}});
    };
    const sample=()=>{
      try {
        frames.push(read());
        if(source.performance.now()-startTime>=duration||frames.length>=600)finish();
        else raf=source.requestAnimationFrame(sample);
      } catch {finish('surface-scope-changed');}
    };
    function start(event) {
      if(started||event?.type==='keydown'&&!['Enter',' ','Escape'].includes(event.key))return;
      started=true;clear();
      if(doc.hidden){finish('document-hidden');return;}
      startTime=source.performance.now();sample();
      if(!settled){source.addEventListener('pagehide',depart,{once:true});timer=source.setTimeout(()=>finish('frame-sampling-stalled'),duration+1500);}
    }
    function depart(){finish('surface-scope-changed');}
    if(waitForInput){
      doc.addEventListener('pointerdown',start,inputOptions);doc.addEventListener('click',start,inputOptions);doc.addEventListener('keydown',start,inputOptions);
      timer=source.setTimeout(()=>finish('interaction-not-observed'),8000);
    } else start();
  });
}

/** State changes may reverse a fade; opacity reversals within one state may not. */
export function surfaceMotionFailures(trace,{end='open',expectMotion=true,tolerance=.03}={}) {
  const errors=[],frames=trace.frames??[],last=frames.at(-1);
  if(trace.error)errors.push(trace.error);
  if(!last)return [...errors,'surface-not-sampled'];
  const effects=frames.flatMap(frame=>frame.animations);
  if(effects.some(animation=>animation.type==='CSSTransition'||animation.type==='CSSAnimation'))errors.push('surface-css-motion-conflict');
  if(expectMotion&&trace.reduced)errors.push('normal-motion-disabled');
  if(expectMotion&&!effects.some(animation=>animation.type==='Animation'))errors.push('native-motion-not-observed');
  if(!expectMotion&&effects.length)errors.push('reduced-motion-not-immediate');
  if(trace.inputMode==='interaction'&&frames[0].visible===(end==='open'))errors.push('surface-initial-state');
  for(let index=1;index<frames.length;index++) {
    const before=frames[index-1],after=frames[index];
    if(!before.visible||!after.visible||before.phase!==after.phase)continue;
    const delta=after.opacity-before.opacity;
    if(after.phase==='enter'&&delta < -tolerance||after.phase==='exit'&&delta>tolerance)errors.push('surface-opacity-reversed');
  }
  if(last.animations.length)errors.push('surface-effects-retained');
  if(end==='open'&&(!last.visible||last.opacity<1-tolerance)||end==='closed'&&last.visible)errors.push('surface-final-state');
  return [...new Set(errors)];
}

/** Compare visible alignment anchors, including composed control internals. */
export function inspectInputAlignment(source=window) {
  const {document}=source,style=element=>source.getComputedStyle(element);
  const dialog=document.querySelector('.input-settings-dialog');
  if(!dialog)return {errors:[],checks:0};
  const errors=[];let checks=0;
  const rect=element=>element.getBoundingClientRect();
  const visible=element=>element&&rect(element).width>0&&rect(element).height>0;
  const compare=(name,a,b,tolerance=1)=>{checks++;if(Math.abs(a-b)>tolerance)errors.push(`${name}: ${a} / ${b}`);};
  const center=element=>rect(element).y+rect(element).height/2;
  const scroll=dialog.querySelector('.input-dialog-scroll');
  const left=rect(scroll).x+(+style(scroll).paddingLeft.replace('px',''));
  const right=rect(scroll).x+scroll.clientWidth-(+style(scroll).paddingRight.replace('px',''));
  for(const element of dialog.querySelectorAll('header h2,.input-settings-body>.input-device-tabs>[role=tab]:first-child,.input-context-toolbar>label,.input-config-hint,.input-layout-toolbar>label,.input-binding-table thead th:first-child,.input-config-footer')){
    if(visible(element))compare(element.className||element.tagName,rect(element).x,left);
  }
  const selects=[...dialog.querySelectorAll('.input-context-toolbar .ui-select,.input-layout-toolbar .ui-select')].filter(visible);
  for(const element of selects)compare('field control column',rect(element).x,rect(selects[0]).x);
  for(const element of dialog.querySelectorAll('.input-context-toolbar>.ui-select,.input-layout-toolbar>:last-child,.input-save-layout>:last-child')){
    if(visible(element))compare('field right edge',rect(element).right,right);
  }
  for(const row of dialog.querySelectorAll('.input-layout-toolbar,.input-save-layout')){
    const controls=[...row.querySelectorAll('.ui-select-trigger,.input-config-button,input')].filter(visible);
    for(const control of controls){compare('control center',center(control),center(controls[0]));compare('control height',rect(control).height,rect(controls[0]).height);}
  }
  for(const table of dialog.querySelectorAll('.input-binding-table')){
    const heading=table.querySelector('thead th:nth-child(2)');
    const bindingLeft=rect(heading).x+(+style(heading).paddingLeft.replace('px',''));
    for(const row of table.querySelectorAll('tbody tr')){
      const key=row.querySelector('.input-keycap,.input-binding-empty,.input-binding-listening');
      if(visible(key))compare('binding column',rect(key).x,bindingLeft);
      const button=row.querySelector('.input-binding-button'),reset=row.querySelector('.input-binding-reset');
      compare('binding/reset center',center(button),center(reset));
    }
  }
  for(const section of dialog.querySelectorAll('.input-axis-threshold')){
    const slider=section.querySelector('.ui-kit-slider');
    compare('threshold left',rect(slider).x,rect(section).x);
    compare('threshold right',rect(slider).right,rect(section).right);
  }
  for(const footer of dialog.querySelectorAll('.input-config-footer')){
    const status=footer.querySelector(':scope>span');
    checks++;if(status?.textContent.trim()&&!visible(status))errors.push('input save/capture status hidden');
    compare('footer actions right',rect(footer.lastElementChild).right,right);
  }
  checks++;if(scroll.scrollWidth>scroll.clientWidth+1)errors.push('input horizontal overflow');
  return {errors,checks};
}

/** Geometry alone cannot detect an opaque route painted over a sticky header. */
export function inspectMastheadLayers(source=window) {
  const {document,scrollY}=source;
  const header=document.querySelector('.book-masthead');
  const surface=document.querySelector('.launcher-page-surface[data-page="library"]');
  if(document.querySelector('.ui-kit-dialog-overlay'))return {errors:[]};
  if(!header||!surface||!document.body.classList.contains('library-tools-open'))return {errors:[]};
  const r=header.getBoundingClientRect();
  const x=Math.min(r.right-2,r.left+80),y=Math.max(r.top,0)+Math.min(8,r.height/2);
  const stack=document.elementsFromPoint(x,y);
  const headerIndex=stack.findIndex(e=>header.contains(e));
  const pageIndex=stack.findIndex(e=>surface.contains(e)&&!e.closest('.tools,.library-backdrop'));
  const errors=[];
  if(headerIndex<0||(pageIndex>=0&&pageIndex<headerIndex))errors.push('masthead-behind-page');
  return {errors,headerIndex,pageIndex,scrollY,point:{x,y}};
}

/** Pointer hit tests omit a modal while Select disables its pointer events. */
export function inspectPopupLayers(source=window) {
  const {document}=source,errors=[];let checks=0;
  for(const popup of document.querySelectorAll('.ui-select-content,.ui-kit-select-content,.ui-menu')){
    if(!popup.getBoundingClientRect().height)continue;
    const wrapper=popup.closest('[data-radix-popper-content-wrapper]')??popup;
    const scope=wrapper.parentElement;
    if(!scope)continue;
    for(const dialog of scope.querySelectorAll(':scope>.ui-kit-dialog')){
      if(!dialog.getBoundingClientRect().height)continue;
      checks++;
      const popupLevel=+source.getComputedStyle(wrapper).zIndex,dialogLevel=+source.getComputedStyle(dialog).zIndex;
      if(popupLevel<=dialogLevel)errors.push(`popup-behind-dialog: ${popupLevel} <= ${dialogLevel}`);
    }
  }
  return {errors,checks};
}
