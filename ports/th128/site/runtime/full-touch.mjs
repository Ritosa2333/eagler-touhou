import{TouchInput}from'./upstream-touch-input.mjs';
import{createTouchLayoutModel}from'./touch-layout.mjs';
import{touchLayoutControlMeta,touchLayoutControlNames,hasTouchButtonShape}from'./touch-layout-model.mjs';
import{bindPlayerJoystick}from'./touch-joystick.mjs';
import{createGameZoomController}from'./game-zoom.mjs';
import{TOUCH_OPTIONS_KEY,TOUCH_DEFAULTS,touchOptions,touchStorage}from'./touch-options.mjs';
import{bindFloatingTouchWindow}from'./touch-floating-window.mjs';

export function mountFullTouch({canvas,container,layer,document,window,input,getGameContext,onTouchMotion,storage,options={},onSettingsOpen=()=>{},viewport=canvas,zoomEnabled=true,hostedControls=false}){
 const practiceEnabled=options.thpracEnabled!==false;
 const layoutControls=practiceEnabled?touchLayoutControlNames:touchLayoutControlNames.filter(name=>name!=='thpracMenu'&&name!=='thpracTab');
 const source=Symbol('fixed-touch-input'),hotkeys=new Map(),listeners=[],controls={fireEnabled:true,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0};
 let saved;try{saved=JSON.parse(storage?.getItem(TOUCH_OPTIONS_KEY)??'null');}catch{}
 let settings;try{settings=touchOptions({touchEnabled:!!window.matchMedia?.('(pointer: coarse)').matches||Number(window.navigator?.maxTouchPoints)>0,...saved,...options});}catch{settings=touchOptions(options);}
 const controller=new TouchInput(),layout=createTouchLayoutModel({storage:touchStorage(storage)}),nodes={},points=new Map();let disposed=false,editing=false,settingsOpen=false,viewportEditing=false,overlayActive=false,lastMotion=null,lastKey=null,layoutGesture=null,viewportGesture=null,menuVisible=false;
 const listen=(node,type,fn,config={passive:false})=>{
  node.addEventListener(type,fn,config);listeners.push([node,type,fn,config]);
  const fallback={pointerdown:'touchstart',pointermove:'touchmove',pointerup:'touchend',pointercancel:'touchcancel'}[type];
  let fallbackFn;
  if(fallback&&!window.PointerEvent&&/iP(?:hone|ad|od)/.test(window.navigator?.userAgent??'')){
   fallbackFn=event=>{for(const touch of Array.from(event.changedTouches??[]))fn({type,pointerId:touch.identifier,pointerType:'touch',button:0,clientX:touch.clientX,clientY:touch.clientY,target:touch.target??event.target,currentTarget:node,preventDefault:()=>event.preventDefault(),stopPropagation:()=>event.stopPropagation?.()});};
   node.addEventListener(fallback,fallbackFn,config);listeners.push([node,fallback,fallbackFn,config]);
  }
  return()=>{node.removeEventListener(type,fn,config);if(fallbackFn)node.removeEventListener(fallback,fallbackFn,config);};
 };
 const create=(tag,id,cls,parent=layer,text)=>{const n=document.createElement(tag);if(id)n.setAttribute('id',id);if(cls)n.className=cls;if(text!==undefined)n.textContent=text;parent.appendChild(n);return n;};
 const classes=(node,name,value)=>node.classList?.toggle(name,value);
 layer.className='web-input web-input-full player';
 const direct=create('div','touchDirectSurface','touch-direct-surface'),safe=create('div','touchLayoutSafeZone','touch-layout-safe-zone'),hud=create('nav','touchHud','touch-hud');
 const contactSurface=hostedControls?document.documentElement:direct;
 const labelButton=(id,cls,name,small,parent=hud)=>{const b=create('button',id,cls,parent);b.type='button';const strong=create('strong',null,null,b,name);if(small)create('small',null,null,b,small);return b;};
 nodes.focus=labelButton('touchFocus','touch-focus','低速','按住 / 切换');nodes.fire=labelButton('touchFire','touch-fire','自动开火','切换');
 nodes.function=labelButton('touchFunction','touch-function touch-fire','Z','蓄力冻结');nodes.bomb=labelButton('touchBomb','touch-bomb','X','完美冻结');
 nodes.joystick=create('div','touchJoystick','touch-joystick');nodes.joystick.setAttribute('role','application');const base=create('div',null,'touch-joystick-base',nodes.joystick);create('i',null,'touch-joystick-ring',base);const knob=create('b','touchJoystickKnob','touch-joystick-knob',base);
 nodes.escape=labelButton('touchEscape','touch-escape','ESC',null,layer);nodes.restart=labelButton('touchRestart','touch-restart','R',null,layer);
 nodes.thpracTab=labelButton('touchThpracTab','touch-thprac-tab','Tab','Tracker',layer);
 nodes.thpracMenu=create('div','touchThpracMenu','touch-thprac-menu');const mobile=labelButton('touchThpracBackspace','touch-thprac-backspace','作弊和高级设置',null,nodes.thpracMenu),functionKeys=create('div','touchThpracFunctionKeys','touch-thprac-function-keys',nodes.thpracMenu);functionKeys.hidden=true;
 const functionNodes=[];for(const[key,label]of[['F12','高级设置'],['F1','无敌'],['F2','无限干劲'],['F3','无限完美冻结'],['F4','无限力量'],['F5','锁定时间'],['F6','自动完美冻结'],['F7','永续BGM']]){const b=labelButton(key==='F12'?'touchAdvanced':null,null,key,label,functionKeys);functionNodes.push([b,key]);}
 for(const[name,node]of Object.entries(nodes)){node.setAttribute('data-touch-layout-control',name);const resize=create('span',null,'touch-layout-resize-handle',node,'↘');resize.setAttribute('data-touch-resize',name);}
 const utility=create('nav',null,'touch-utility-row');const settingsButton=labelButton('touchSettingsOpen',null,'触控设置',null,utility),layoutButton=labelButton('touchLayoutOpen',null,'调整布局',null,utility),zoomButton=labelButton('gameZoomToggle','game-zoom-toggle','视口复原',null,utility),zoomLabel=create('small','gameZoomScale',null,zoomButton,'100%');
 const settingsHost=container.parentElement??container,windowsRoot=create('div','touchWindowsRoot','web-input-full touch-windows-root',document.body??settingsHost);
 settingsButton.hidden=layoutButton.hidden=options.touchUtilityShortcuts===false;
 for(const keyEvent of ['keydown','keyup'])listen(windowsRoot,keyEvent,event=>event.stopPropagation());
 const moveWindows=()=>{const host=document.fullscreenElement===container||container.classList.contains('player-viewport-full')?container:document.body??settingsHost;if(windowsRoot.parentElement!==host)host.appendChild(windowsRoot);settingsFloat.clamp();editorFloat.clamp();};listen(document,'fullscreenchange',moveWindows);listen(document,'th128-view-change',moveWindows);
 const panel=create('section','touchSettings','touch-layout-settings touch-settings-fields touch-floating-window',windowsRoot);panel.hidden=true;panel.setAttribute('role','dialog');panel.setAttribute('aria-modal','false');panel.setAttribute('aria-label','触控设置');
 const settingsHeader=create('header',null,'touch-workbench-header',panel),settingsHandle=create('strong','touchSettingsDragHandle','touch-layout-editor-copy',settingsHeader,'触控设置'),settingsCollapse=labelButton('touchSettingsCollapse','touch-workbench-collapse','折叠',null,settingsHeader),settingsClose=labelButton('touchSettingsClose','touch-window-close','关闭',null,settingsHeader),settingsBody=create('div','touchSettingsBody','touch-workbench-body',panel),settingsScroll=create('div',null,'touch-workbench-scroll',settingsBody);
 const fields=create('div',null,'touch-layout-settings-list',settingsScroll),inputs={};
 const row=(label,node)=>{const r=create('label',null,'touch-layout-setting-row',fields);create('span',null,null,r,label);r.appendChild(node);};
 function select(key,label,choices){const n=document.createElement('select');n.setAttribute('id',key);for(const[value,text]of choices){const o=create('option',null,null,n,text);o.value=value;}n.value=settings[key];inputs[key]=n;row(label,n);listen(n,'change',()=>configureTouch({[key]:n.value},true));}
 function toggle(key,label){const n=document.createElement('input');n.type='checkbox';n.checked=settings[key];n.setAttribute('id',key);inputs[key]=n;row(label,n);listen(n,'change',()=>configureTouch({[key]:n.checked},true));}
 toggle('touchEnabled','启用触控');select('touchMovementMode','移动方式',[['touch','直连（原版移速）'],['touch-unlimited','无限直连（作弊触控）'],['joystick','摇杆'],['joystick-free','自由摇杆']]);select('touchFocusMode','低速方式',[['hold-button','按住'],['toggle-button','点击切换'],['two-finger','双指']]);
 for(const[key,label,min,max,step]of[['touchSensitivity','灵敏度',100,300,1],['touchControlOpacity','控件不透明度',20,100,5]]){const group=create('span',null,null),n=create('input',key,null,group),output=create('output',null,null,group);n.type='range';Object.assign(n,{min,max,step,value:String(settings[key])});output.textContent=settings[key]+'%';inputs[key]=n;row(label,group);listen(n,'input',()=>{configureTouch({[key]:Number(n.value)});output.textContent=n.value+'%';});listen(n,'change',()=>configureTouch({[key]:Number(n.value)},true));}
 toggle('doubleTapBombEnabled','双击完美冻结');toggle('restartButtonEnabled','显示 R');if(practiceEnabled)toggle('thpracTouchControlsEnabled','thprac 触控按钮');
 const editPanel=create('section','touchLayoutEditor','touch-layout-editor touch-floating-window',windowsRoot);editPanel.hidden=true;editPanel.setAttribute('role','dialog');editPanel.setAttribute('aria-modal','false');editPanel.setAttribute('aria-label','触控布局工作台');
 const editorHeader=create('header',null,'touch-workbench-header',editPanel),editorHandle=create('div','touchLayoutEditorDragHandle','touch-layout-editor-copy',editorHeader);create('span',null,'touch-workbench-grip',editorHandle,'⠿');create('strong',null,null,editorHandle,'触控布局');const orientationLabel=create('em','touchLayoutOrientation',null,editorHandle),editorCollapse=labelButton('touchLayoutCollapse','touch-workbench-collapse','折叠',null,editorHeader),editorClose=labelButton('touchLayoutExit','touch-window-close','退出',null,editorHeader),editorBody=create('div','touchWorkbenchBody','touch-workbench-body',editPanel),editorScroll=create('div',null,'touch-workbench-scroll',editorBody);
 const arrangement=create('section',null,'touch-layout-arrangement',editorScroll),selector=create('select','touchLayoutSelected',null,arrangement);selector.setAttribute('aria-label','选择控件');for(const name of layoutControls){const o=create('option',null,null,selector,touchLayoutControlMeta[name].title);o.value=name;}const scale=create('input','touchLayoutScale',null,arrangement);Object.assign(scale,{type:'range',min:60,max:300,step:1,value:100});scale.setAttribute('aria-label','控件缩放');const scaleValue=create('output',null,null,arrangement,'100%'),viewX=create('input','touchViewportX',null,arrangement);Object.assign(viewX,{type:'range',min:-50,max:50,step:1,value:0});viewX.setAttribute('aria-label','视口水平位置');
 const adjustViewport=labelButton('touchViewportAdjust',null,'调整视口',null,arrangement),resetViewport=labelButton('touchViewportReset',null,'复原视口',null,arrangement),reset=labelButton('touchLayoutReset',null,'恢复当前方向',null,arrangement),editorFooter=create('footer',null,'touch-workbench-footer',editorBody),editorActions=create('div',null,'touch-layout-editor-actions',editorFooter),save=labelButton('touchLayoutSave',null,'保存布局',null,editorActions),cancel=labelButton('touchLayoutCancel',null,'退出',null,editorActions),feedback=create('output',null,null,editorFooter);
 const viewportSurface=create('div','touchViewportDragSurface','touch-viewport-drag-surface',layer),viewportDone=labelButton('touchViewportDone','touch-viewport-done','完成视口调整',null,layer);viewportSurface.hidden=viewportDone.hidden=true;
 const shapeRow=create('label',null,'touch-layout-shape',arrangement);create('span',null,null,shapeRow,'按键形状');
 const shape=create('select','touchLayoutShape',null,shapeRow);shape.setAttribute('aria-label','按键形状');
 for(const [value,label] of [['circle','圆形'],['square','方形'],['rectangle','长方形']])create('option',null,null,shape,label).value=value;
 const orientation=()=>((window.visualViewport?.width??window.innerWidth??container.clientWidth??1280)>=(window.visualViewport?.height??window.innerHeight??container.clientHeight??960)?'landscape':'portrait');
 const settingsFloat=bindFloatingTouchWindow({node:panel,host:windowsRoot,handle:settingsHandle,collapse:settingsCollapse,body:settingsBody,window,document,store:layout.windowPositions,orientation,listen,kind:'settings'}),editorFloat=bindFloatingTouchWindow({node:editPanel,host:windowsRoot,handle:editorHandle,collapse:editorCollapse,body:editorBody,window,document,store:layout.windowPositions,orientation:()=>editing?layout.getSnapshot().orientation:orientation(),listen,kind:'editor'});
 function defaults(){
  // Measure responsive buttons even if touch or a particular control is currently off.
  const display=layer.style.display,custom=layer.classList.contains('touch-layout-custom'),focusHidden=nodes.focus.hidden;
  layer.style.display='block';classes(layer,'touch-layout-custom',false);classes(layer,'touch-enabled',true);nodes.focus.hidden=false;
  try{
   const r=layer.getBoundingClientRect(),controls={};
   for(const [name,node] of Object.entries(nodes)){
    const b=node.getBoundingClientRect();
    controls[name]={x:Math.max(0,Math.min(1,(b.left+b.width/2-r.left)/Math.max(1,r.width))),y:Math.max(0,Math.min(1,(b.top+b.height/2-r.top)/Math.max(1,r.height))),scale:1,priority:touchLayoutControlMeta[name].priority};
   }
   return{controls,viewport:{x:0}};
  }finally{nodes.focus.hidden=focusHidden;classes(layer,'touch-layout-custom',custom);classes(layer,'touch-enabled',settings.touchEnabled);layer.style.display=display;}
 }
 const zoom=createGameZoomController({player:layer,frame:canvas,viewport,toggle:zoomButton,toggleLabel:zoomButton.children[0],scaleLabel:zoomLabel,directSurface:direct,getBaseOffset:()=>{const s=layout.getSnapshot(),p=editing?s.draft?.profiles[s.orientation]:s.saved?.profiles[orientation()];return{x:(p?.viewport.x??0)*(layer.clientWidth??layer.getBoundingClientRect().width),y:0};},getResetLabel:()=>'视口复原',isAvailable:()=>zoomEnabled&&settings.touchEnabled&&!editing});
 function applyLayout(){
  const s=layout.getSnapshot(),p=editing?s.draft?.profiles[s.orientation]:s.saved?.profiles[orientation()];
  layer.style.display=settings.touchEnabled||editing?'block':'none';
  classes(layer,'touch-layout-custom',!!p);classes(layer,'touch-layout-edit',editing);classes(layer,'touch-viewport-edit',viewportEditing);
  direct.style.pointerEvents=editing||overlayActive?'none':'auto';
  nodes.focus.hidden=!editing&&settings.touchFocusMode==='two-finger';
  for(const [name,node] of Object.entries(nodes)){
   const item=p?.controls[name];
   if(item){node.style.setProperty('--touch-layout-x',item.x*100+'%');node.style.setProperty('--touch-layout-y',item.y*100+'%');node.style.setProperty('--touch-layout-scale',String(item.scale));}
   else for(const key of ['x','y','scale'])node.style.removeProperty('--touch-layout-'+key);
   node.style.zIndex=item?String(31+item.priority):'';
   if(hasTouchButtonShape(name))node.setAttribute('data-touch-shape',item?.shape??'circle');
   classes(node,'touch-layout-selected',editing&&s.selected===name);
  }
  editPanel.hidden=!editing||viewportEditing;viewportSurface.hidden=viewportDone.hidden=!editing||!viewportEditing;
  if(editing){
   const selected=p?.controls[s.selected];orientationLabel.textContent=s.orientation==='landscape'?'横屏':'竖屏';selector.value=s.selected;
   shapeRow.hidden=!hasTouchButtonShape(s.selected);shape.value=selected?.shape??'circle';
   scale.value=String(Math.round((selected?.scale??1)*100));scaleValue.textContent=scale.value+'%';viewX.value=String((p?.viewport.x??0)*100);
  }
  zoom.applyTransform();
 }
 layout.subscribe(applyLayout);layout.hydrate();
 const game=()=>getGameContext();
 const can=()=>!disposed&&settings.touchEnabled&&!editing&&!overlayActive&&!settingsOpen&&!document.hidden;
 const keyState=(codes)=>input.setKeySource(source,codes);
 function clearMotion(){const s=game();if(s?.kind==='game'&&!s.paused&&lastMotion&&lastMotion.generation===s.generation)onTouchMotion?.({mode:0,x:0,y:0,generation:s.generation});lastMotion=null;}
 function cancelTouches(){controller.cancel();layoutGesture=viewportGesture=null;joystick.reset();const active=[...points.keys()];points.clear();for(const id of active){try{if(contactSurface.hasPointerCapture?.(id))contactSurface.releasePointerCapture(id);}catch{}}controls.focusEnabled=false;controls.joystickX=controls.joystickY=0;keyState([]);for(const owner of hotkeys.values())input.clearKeySource(owner);hotkeys.clear();clearMotion();zoom.cancelGesture();}
 function configureTouch(value,persist=false){settings=touchOptions(value,settings);controller.configure(settings);if(!settings.touchEnabled)cancelTouches();nodes.joystick.hidden=!settings.touchMovementMode.startsWith('joystick');nodes.restart.hidden=!settings.restartButtonEnabled;nodes.thpracMenu.hidden=!practiceEnabled||!settings.thpracTouchControlsEnabled;nodes.thpracTab.hidden=!practiceEnabled||game()?.trackerAvailable!==true;classes(layer,'touch-enabled',settings.touchEnabled);classes(layer,'touch-joystick-enabled',settings.touchMovementMode.startsWith('joystick'));layer.style.setProperty?.('--touch-control-opacity',String(settings.touchControlOpacity/100));for(const[key,n]of Object.entries(inputs)){if(n.type==='checkbox')n.checked=settings[key];else n.value=String(settings[key]);}if(persist&&storage){storage.setItem(TOUCH_OPTIONS_KEY,JSON.stringify(settings));if(storage.getItem(TOUCH_OPTIONS_KEY)!==JSON.stringify(settings))throw Error('Touch settings readback differs');}applyLayout();zoom.refreshUi();return settings;}
 const codeForBit=new Map([[1,'KeyZ'],[2,'KeyX'],[8,'ShiftLeft'],[16,'ArrowUp'],[32,'ArrowDown'],[64,'ArrowLeft'],[128,'ArrowRight'],[256,'Escape'],[512,'KeyC']]);
 const arrowsHeld=()=>['ArrowUp','ArrowDown','ArrowLeft','ArrowRight'].some(code=>input.physicalKeyDown(code));
 const focusHeld=()=>input.physicalKeyDown('ShiftLeft')||input.physicalKeyDown('ShiftRight');
 function previewDirectMotion(phase){
  if(!can()||!controller.primary)return null;
  const s=game();return s?controller.previewDirectMotion(s,phase,{arrows:s.arrowsHeld||arrowsHeld(),keyFocused:s.keyFocused||controls.focusEnabled||focusHeld()}):null;
 }
 function sampleTouch(){const s=game();if(!s)return;if(lastKey!==s.key){controller.sync(s);lastKey=s.key;lastMotion=null;}if(!can()||s.kind==='replay'){keyState([]);if(s.kind!=='replay')clearMotion();return;}
  controller.controls(controls,s);const sample=controller.sample(s,{arrows:s.arrowsHeld||arrowsHeld(),keyFocused:s.keyFocused||focusHeld()});let held=sample.held;
  if(s.kind==='game'&&controls.fireEnabled)held=(held&~1)|512;const codes=[...codeForBit].filter(([bit])=>held&bit).map(([,code])=>code);keyState(codes);
  if(s.kind==='game'&&s.ready&&!s.paused){const motion={...(sample.motion??{mode:0,x:0,y:0}),generation:s.generation};onTouchMotion?.(motion);lastMotion=motion;}
  classes(nodes.fire,'is-on',controls.fireEnabled);classes(nodes.focus,'is-on',controller.focused());
 }
 function gamePoint(clientX,clientY){
  const rect=canvas.getBoundingClientRect(),scale=Math.min(rect.width/640,rect.height/480);
  if(!(scale>0))return null;
  // The canvas uses object-fit: contain. Exclude the bars even when zoomed.
  return{x:(clientX-rect.left-(rect.width-640*scale)/2)/scale,
   y:(clientY-rect.top-(rect.height-480*scale)/2)/scale};
 }
 function pointer(event,type){
  if(!can()||event.pointerType==='mouse')return;
  const id=event.pointerId;
  if(type==='down'){
   // A contact that starts in a practice control belongs to the browser.
   if(hostedControls&&(settings.touchMovementMode.startsWith('joystick')||event.target?.closest?.('.thprac-layer')))return;
  }else if(!points.has(id))return;
  const s=game(),point=type==='cancel'?{x:0,y:0}:gamePoint(event.clientX,event.clientY);
  if(!s||!point)return;
  event.preventDefault?.();
  if(type==='down'){
   points.set(id,true);
   try{contactSurface.setPointerCapture(id);}catch{}
   zoom.beginPointer(event);
  }else if(type==='move')zoom.movePointer(event);
  else{points.delete(id);zoom.endPointer(event);}
  controller.pointer({type,id,...point},s);
 }
 for(const[type,next]of[['pointerdown','down'],['pointermove','move'],['pointerup','up'],['pointercancel','cancel'],['lostpointercapture','cancel']])listen(contactSurface,type,event=>pointer(event,next));
 if(hostedControls)contactSurface.dataset.th128FrameTouch='true';
 const press=(node,code)=>{const owner=Symbol(code);listen(node,'pointerdown',event=>{if(!can()&&!(code==='Escape'&&overlayActive&&settings.touchEnabled&&!disposed))return;event.preventDefault();if(['Escape','KeyR','F12'].includes(code))cancelTouches();try{node.setPointerCapture(event.pointerId);}catch{}hotkeys.set(node,owner);input.setKeySource(owner,[code]);});const up=()=>{input.clearKeySource(owner);hotkeys.delete(node);};for(const event of ['pointerup','pointercancel','lostpointercapture'])listen(node,event,up);};
 press(nodes.function,'KeyZ');press(nodes.bomb,'KeyX');press(nodes.escape,'Escape');press(nodes.restart,'KeyR');if(practiceEnabled){press(nodes.thpracTab,'Tab');press(mobile,'Backspace');for(const[n,key]of functionNodes)press(n,key);}
 if(practiceEnabled)listen(mobile,'pointerdown',()=>{if(!can())return;menuVisible=!menuVisible;functionKeys.hidden=!menuVisible;mobile.setAttribute('aria-expanded',String(menuVisible));});
 listen(nodes.fire,'pointerdown',event=>{if(!can())return;event.preventDefault();controls.fireEnabled=!controls.fireEnabled;classes(nodes.fire,'is-on',controls.fireEnabled);});
 listen(nodes.focus,'pointerdown',event=>{if(!can()||settings.touchFocusMode==='two-finger')return;event.preventDefault();controls.focusEnabled=settings.touchFocusMode==='hold-button'||!controls.focusEnabled;try{nodes.focus.setPointerCapture(event.pointerId);}catch{}});for(const type of ['pointerup','pointercancel','lostpointercapture'])listen(nodes.focus,type,()=>{if(settings.touchFocusMode==='hold-button')controls.focusEnabled=false;});
 const joystick=bindPlayerJoystick({element:nodes.joystick,knob,allowed:can,movement:()=>settings.touchMovementMode,axes(x,y){controls.joystickX=x;controls.joystickY=y;},schedule(){},listen});
 function setSettingsOpen(value){cancelTouches();settingsOpen=value===true;if(settingsOpen)settingsFloat.show();else settingsFloat.hide();onSettingsOpen(settingsOpen);}
 function beginTouchLayout(direction=orientation()){cancelTouches();setSettingsOpen(false);editing=true;viewportEditing=false;editorScroll.appendChild(fields);layout.begin(direction,defaults(),layoutControls);applyLayout();editorFloat.show();onSettingsOpen(true);return layout.getSnapshot();}
 function endTouchLayout(){editorFloat.hide();editing=false;viewportEditing=false;layoutGesture=viewportGesture=null;settingsScroll.appendChild(fields);layout.discard();cancelTouches();applyLayout();onSettingsOpen(false);}
 function setViewportEditing(value){viewportGesture=null;viewportEditing=editing&&value===true;cancelTouches();applyLayout();if(editing&&!viewportEditing)editorFloat.show();}
 function updateTouchLayout(name,value){layout.updateControl(name,value);}
 function saveTouchLayout(){return layout.save();}
 for(const[node,fn]of[[settingsButton,()=>{if(editing)endTouchLayout();setSettingsOpen(!settingsOpen);}],[settingsClose,()=>setSettingsOpen(false)],[layoutButton,()=>beginTouchLayout()],[zoomButton,()=>zoom.reset()],[save,()=>{const result=saveTouchLayout();feedback.textContent=result.persisted?'已保存':'本次会话已保存';}],[cancel,endTouchLayout],[editorClose,endTouchLayout],[adjustViewport,()=>setViewportEditing(true)],[viewportDone,()=>setViewportEditing(false)],[resetViewport,()=>layout.setViewport(0)],[reset,()=>{layout.resetOrientation();layout.ensureOrientation(layout.getSnapshot().orientation,defaults());}]])listen(node,'click',fn);
 listen(selector,'change',()=>layout.selectControl(selector.value));listen(scale,'input',()=>layout.updateControl(selector.value,{scale:Number(scale.value)/100}));listen(viewX,'input',()=>layout.setViewport(Number(viewX.value)/100));
 listen(shape,'change',()=>layout.updateControl(selector.value,{shape:shape.value}));
 listen(viewportSurface,'pointerdown',event=>{if(!editing||!viewportEditing)return;event.preventDefault();viewportGesture={id:event.pointerId,x:event.clientX};try{viewportSurface.setPointerCapture(event.pointerId);}catch{}});
 listen(document,'pointermove',event=>{if(!viewportGesture||event.pointerId!==viewportGesture.id||!viewportEditing)return;event.preventDefault();const s=layout.getSnapshot(),x=s.draft.profiles[s.orientation].viewport.x;layout.setViewport(x+(event.clientX-viewportGesture.x)/Math.max(1,layer.clientWidth??layer.getBoundingClientRect().width));viewportGesture.x=event.clientX;});
 for(const type of ['pointerup','pointercancel'])listen(document,type,event=>{if(viewportGesture?.id===event.pointerId)viewportGesture=null;});
 for(const[name,node]of Object.entries(nodes)){listen(node,'pointerdown',event=>{if(!editing)return;event.preventDefault();event.stopPropagation?.();layout.selectControl(name);const p=layout.getSnapshot().draft.profiles[layout.getSnapshot().orientation].controls[name];layoutGesture={id:event.pointerId,name,x:event.clientX,y:event.clientY,start:{...p},resize:!!event.target?.getAttribute?.('data-touch-resize')};try{node.setPointerCapture(event.pointerId);}catch{}});listen(node,'pointermove',event=>{const g=layoutGesture;if(!g||g.id!==event.pointerId||!editing)return;event.preventDefault();const r=layer.getBoundingClientRect(),dx=event.clientX-g.x,dy=event.clientY-g.y;if(g.resize)layout.updateControl(g.name,{scale:g.start.scale+(dx+dy)/150});else layout.updateControl(g.name,{x:g.start.x+dx/Math.max(1,r.width),y:g.start.y+dy/Math.max(1,r.height)});});for(const type of ['pointerup','pointercancel','lostpointercapture'])listen(node,type,()=>{layoutGesture=null;});}
 const relayout=()=>{cancelTouches();if(editing){const before=layout.getSnapshot().orientation;layout.ensureOrientation(orientation(),defaults());if(before!==layout.getSnapshot().orientation)editorFloat.position();else editorFloat.clamp();}applyLayout();};listen(window,'resize',relayout);if(window.visualViewport)listen(window.visualViewport,'resize',relayout);if(window.screen?.orientation?.addEventListener)listen(window.screen.orientation,'change',relayout);listen(window,'blur',cancelTouches);listen(window,'pagehide',cancelTouches);listen(document,'visibilitychange',()=>{if(document.hidden)cancelTouches();});
 configureTouch(settings);applyLayout();
 return{sampleTouch,previewDirectMotion,openTouchSettings(){setSettingsOpen(true);},configureTouch,setOverlayActive(value){overlayActive=value===true;windowsRoot.hidden=overlayActive;cancelTouches();direct.style.pointerEvents=editing||overlayActive?'none':'auto';for(const n of [hud,nodes.joystick,nodes.thpracMenu,utility])n.style.visibility=overlayActive?'hidden':'';},
  syncTouchContext(){const s=game();if(s&&controller.context!==s.key){controller.sync(s);keyState([]);for(const owner of hotkeys.values())input.clearKeySource(owner);hotkeys.clear();clearMotion();lastKey=s.key;}},
  touchControls(value){if(value.thpracMobilePanelsEnabled!==undefined)configureTouch({thpracTouchControlsEnabled:value.thpracMobilePanelsEnabled});if(value.thpracMobilePanelsVisible!==undefined){menuVisible=value.thpracMobilePanelsVisible;functionKeys.hidden=!menuVisible;mobile.setAttribute('aria-expanded',String(menuVisible));}for(const key of Object.keys(controls))if(value[key]!==undefined)controls[key]=value[key];if(value.touchSensitivity!==undefined)configureTouch({touchSensitivity:value.touchSensitivity});},
  directTouch({id,type,x,y}){
   // Launcher coordinates are normalized to the iframe, including its margins.
   const point=gamePoint(x*window.innerWidth,y*window.innerHeight),state=game();
   if(point&&state)controller.pointer({id,type,...point},state);
  },
  cancelTouch:cancelTouches,beginTouchLayout,endTouchLayout,updateTouchLayout,saveTouchLayout,touchSnapshot:()=>({options:settings,controls:{...controls},layout:layout.getSnapshot(),windows:layout.windowPositions.snapshot(),zoom:zoom.snapshot()}),
  dispose(){if(disposed)return;cancelTouches();disposed=true;if(hostedControls)delete contactSurface.dataset.th128FrameTouch;settingsFloat.dispose();editorFloat.dispose();joystick.dispose();zoom.uninstallInputBridge();for(const[node,type,fn,config]of listeners)node.removeEventListener(type,fn,config);windowsRoot.remove();}};
}
