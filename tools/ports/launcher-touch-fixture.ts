import {createPlayerTouch} from '../../launcher/app/services/player-touch';
import {createPlayerGamepad} from '../../launcher/app/services/player-gamepad';
import {createGamepadMappingModel} from '../../launcher/src/launcher/gamepad-mapping.mts';
import {virtualGamepad,virtualNeutral,virtualButton} from '../../launcher/tests/fixtures/virtual-gamepads.mjs';
import {createTouchLayoutModel} from '../../launcher/app/models/touch-layout';
import {touchLayoutControlMeta,touchLayoutControlNames,touchLayoutStorageKey} from '../../launcher/src/launcher/touch-layout-model.mts';
import {applySavedPlayerTouchLayout} from '../../launcher/app/services/player-input';
import {captureTouchLayoutDefaults,visibleTouchControls} from '../../launcher/app/components/settings/touch-layout-geometry';
import {createGameZoomController} from '../../launcher/src/launcher/game-zoom.mts';

type TouchFixtureDependencies={document?:Document;window?:Window;location?:Location;
  createTouch?:typeof createPlayerTouch};

function memoryStorage(){
  const values=new Map<string,string>();
  return{getItem:(key:string)=>values.get(key)??null,setItem:(key:string,value:string)=>{values.set(key,String(value));},
    removeItem:(key:string)=>{values.delete(key);}};
}

function requirePlayerElement<T extends HTMLElement=HTMLElement>(player:HTMLElement,id:string):T{
  const element=player.querySelector<T>(`#${id}`);
  if(!element)throw Error(`Production Player scaffold is missing #${id}`);
  return element;
}

function ensurePlayerLayoutTree(player:HTMLElement){
  const safe=requirePlayerElement(player,'touchLayoutSafeZone');
  for(const name of touchLayoutControlNames){
    const id=touchLayoutControlMeta[name].id;
    requirePlayerElement(player,id);
  }
  return safe;
}

/** Production controller on a measured DOM surface, connected to the real iframe.
 * Layout and physical-device presentation are reviewed in the Launcher itself. */
export function install({game,epoch,movement='touch',focus='toggle-button'}:{game:string;epoch:number;movement?:string;focus?:string},
  dependencies:TouchFixtureDependencies={}) {
  const hostDocument=dependencies.document??document;
  const hostWindow=(dependencies.window??window) as Window & {__acceptanceTouch?:any};
  const hostLocation=dependencies.location??location;
  const createTouch=dependencies.createTouch??createPlayerTouch;
  dispose(hostWindow);
  const player=hostDocument.getElementById('player') as HTMLElement|null;
  if(!player)throw Error('Touch fixture needs the selected Player surface');
  const frame=requirePlayerElement<HTMLIFrameElement>(player,'gameFrame');
  if(!frame.contentWindow)throw Error('Touch fixture needs the Runtime iframe inside the selected Player surface');
  const owner:{touch:ReturnType<typeof createPlayerTouch>|null;
    nodes:Record<string,HTMLElement>;settings:any;context:any;gamepadModel:any;gamepadService:any;gamepads:any[];
    gamepadStorage:any;gamepadSamples:any[];gamepadSampleDrops:number;layout:any;layoutApi:any;layoutStorage:any;zoom:any;magnifier:any;
    pointerEvents:any[];pointerEventDrops:number;cleanup: (()=>void)[];originalViewportTransform:string;
    originalZoomToggleHidden:boolean;originalZoomLabel:string;originalZoomScaleLabel:string;originalZoomEditing:boolean;
    zoomAvailable:boolean;layoutOriginalCustom:boolean;layoutOriginalStyles:any[];layoutFallbackNodes:HTMLElement[];
    originalTouchEnabled:boolean;originalJoystickEnabled:boolean;originalDirectSurfaceHidden:boolean;originalJoystickHidden:boolean;
    layoutSource:'player-controls'}={touch:null,nodes:{},settings:null,context:null,
      gamepadModel:null,gamepadService:null,gamepads:[],gamepadStorage:null,gamepadSamples:[],gamepadSampleDrops:0,
      layout:null,layoutApi:null,layoutStorage:null,zoom:null,magnifier:null,pointerEvents:[],pointerEventDrops:0,cleanup:[],originalViewportTransform:'',
      originalZoomToggleHidden:true,originalZoomLabel:'',originalZoomScaleLabel:'',originalZoomEditing:false,
      zoomAvailable:false,layoutOriginalCustom:false,layoutOriginalStyles:[],layoutFallbackNodes:[],originalTouchEnabled:false,
      originalDirectSurfaceHidden:true,originalJoystickHidden:true,
      originalJoystickEnabled:false,layoutSource:'player-controls'};
  hostWindow.__acceptanceTouch=owner;
  try{
    const safe=ensurePlayerLayoutTree(player);
    owner.originalTouchEnabled=player.classList.contains('touch-enabled');
    owner.originalJoystickEnabled=player.classList.contains('touch-joystick-enabled');
    player.classList.add('touch-enabled');
    player.classList.toggle('touch-joystick-enabled',movement==='joystick'||movement==='joystick-free');
    const nodes={directSurface:requirePlayerElement(player,'touchDirectSurface'),
      fire:requirePlayerElement(player,'touchFire'),focus:requirePlayerElement(player,'touchFocus'),
      functionKey:requirePlayerElement(player,'touchFunction'),escape:requirePlayerElement(player,'touchEscape'),
      bomb:requirePlayerElement(player,'touchBomb'),restart:requirePlayerElement(player,'touchRestart'),
      joystick:requirePlayerElement(player,'touchJoystick'),joystickKnob:requirePlayerElement(player,'touchJoystickKnob'),
      thpracBackspace:requirePlayerElement(player,'touchThpracBackspace'),
      thpracKeys:Array.from(player.querySelectorAll<HTMLElement>('[data-thprac-key]'))};
    owner.originalDirectSurfaceHidden=nodes.directSurface.hidden;
    owner.originalJoystickHidden=nodes.joystick.hidden;
    nodes.directSurface.hidden=false;
    nodes.joystick.hidden=!(movement==='joystick'||movement==='joystick-free');
    owner.nodes=nodes;
    let enabled=false;
    const settings={gameId:game,options:{touchEnabled:true,touchMovementMode:movement,touchFocusMode:focus,
      thpracTouchControlsEnabled:true,touchSensitivity:1}};
    owner.settings=settings;
    const context={target:frame.contentWindow,targetOrigin:hostLocation.origin,protocol:'eagler-touhou/1',game,epoch,ready:true,launched:true,spectator:false};
    owner.context=context;
    const observePointer=(event:PointerEvent)=>{
      if(owner.pointerEvents.length>=512){owner.pointerEventDrops++;return;}
      owner.pointerEvents.push({type:event.type,pointerId:event.pointerId,pointerType:event.pointerType,
        clientX:event.clientX,clientY:event.clientY,isTrusted:event.isTrusted,timeStamp:event.timeStamp,
        surfaceOwner:player.contains(nodes.directSurface)?'player':'outside-player',
        surfaceTarget:event.target===nodes.directSurface?'touchDirectSurface':'descendant'});
    };
    for(const type of ['pointerdown','pointermove','pointerup','pointercancel']){
      nodes.directSurface.addEventListener(type,observePointer as EventListener,true);
      owner.cleanup.push(()=>nodes.directSurface.removeEventListener(type,observePointer as EventListener,true));
    }
    const viewport=hostDocument.getElementById('gameViewport') as HTMLElement|null;
    const toggle=hostDocument.getElementById('gameZoomToggle') as HTMLElement|null;
    const toggleLabel=toggle?.querySelector('strong') as HTMLElement|null;
    const scaleLabel=hostDocument.getElementById('gameZoomScale') as HTMLElement|null;
    owner.originalViewportTransform=viewport?.style.transform??'';
    owner.originalZoomToggleHidden=toggle?.hidden??true;
    owner.originalZoomLabel=toggleLabel?.textContent??'';owner.originalZoomScaleLabel=scaleLabel?.textContent??'';
    owner.originalZoomEditing=player?.classList.contains('game-zoom-editing')??false;
    const zoom=createGameZoomController({player,frame,viewport:viewport??frame.parentElement!,
      toggle:toggle??nodes.fire,toggleLabel:toggleLabel??nodes.fire,scaleLabel:scaleLabel??nodes.fire,directSurface:nodes.directSurface,
      getBaseOffset:()=>({x:0,y:0}),getResetLabel:()=> 'Reset',isAvailable:()=>owner.zoomAvailable,minScale:1,maxScale:3});
    owner.zoom=zoom;zoom.refreshUi();
    const layoutPlayer=player,layoutSafe=safe;
    owner.layoutStorage=memoryStorage();
    let layoutModel=createTouchLayoutModel({storage:owner.layoutStorage});layoutModel.hydrate();owner.layout=layoutModel;
    owner.layoutOriginalCustom=layoutPlayer.classList.contains('touch-layout-custom');
    owner.layoutOriginalStyles=touchLayoutControlNames.flatMap(name=>{
      const element=layoutPlayer.querySelector<HTMLElement>(`#${touchLayoutControlMeta[name].id}`);
      return element?[{element,style:element.getAttribute('style')}]:[];
    });
    const touch=createTouch({runtime:{getInputContext:()=>context,getMidiEventContext:()=>null,
      getSnapshot:()=>context,subscribe:()=>()=>{}},getSettings:()=>settings,
      fireState:{getEnabled:()=>enabled,setEnabled:value=>{enabled=value;}},editing:()=>false,playerOpen:()=>true,
      iosWebKitTouch:false,refocus:()=>{},elements:{frame,...nodes,thpracKeys:nodes.thpracKeys} as any,
      zoom,document:hostDocument,window:hostWindow} as any);
    owner.touch=touch;touch.syncControls();
    // Layout testing uses the same production model and applicator as the
    // visible editor, against the current Player control tree when available.
    const layoutStorageKey=touchLayoutStorageKey;
    owner.layout={
      begin(orientation:string){
        const defaults=captureTouchLayoutDefaults(layoutPlayer,'Touch layout defaults are unavailable');
        const visible=visibleTouchControls(layoutPlayer);
        if(layoutModel.getSnapshot().isEditing)layoutModel.ensureOrientation(orientation as any,defaults);
        else layoutModel.begin(orientation as any,defaults,visible);
        return{...layoutModel.getSnapshot(),target:'player',source:owner.layoutSource,
          controls:touchLayoutControlNames.map(name=>({name,id:touchLayoutControlMeta[name].id,
            mounted:!!layoutPlayer.querySelector(`#${touchLayoutControlMeta[name].id}`),owner:'player'}))};
      },
      updateControl(name:string,placement:any){layoutModel.updateControl(name as any,placement);return layoutModel.getSnapshot();},
      save(){return layoutModel.save();},
      reload(){layoutModel.discard();layoutModel=createTouchLayoutModel({storage:owner.layoutStorage});layoutModel.hydrate();owner.layout=owner.layoutApi;return layoutModel.getSnapshot();},
      apply(orientation:string,controlName='fire'){
        const profile=layoutModel.getSnapshot().saved?.profiles[orientation as any]??null;
        if(!profile)return{available:false,orientation};
        applySavedPlayerTouchLayout(layoutPlayer,layoutSafe,profile);
        const control=layoutPlayer.querySelector<HTMLElement>(`#${touchLayoutControlMeta[controlName as keyof typeof touchLayoutControlMeta].id}`);
        const safeRect=layoutSafe.getBoundingClientRect(),controlRect=control?.getBoundingClientRect();
        return{available:!!controlRect,target:'player',source:owner.layoutSource,orientation,storageKey:layoutStorageKey,placement:profile.controls[controlName as keyof typeof touchLayoutControlMeta],
          safeRect:{left:safeRect.left,top:safeRect.top,width:safeRect.width,height:safeRect.height},
          controlRect:controlRect&&{left:controlRect.left,top:controlRect.top,width:controlRect.width,height:controlRect.height},
          renderedScale:Number.parseFloat(control?.style.getPropertyValue('--touch-layout-scale')??'NaN')};
      },
      storageSnapshot(){const raw=owner.layoutStorage.getItem(layoutStorageKey);return{key:layoutStorageKey,value:raw?JSON.parse(raw):null};},
      getSnapshot(){return layoutModel.getSnapshot();},
      dispose(){
        layoutModel.discard();layoutPlayer.classList.toggle('touch-layout-custom',owner.layoutOriginalCustom);
        for(const {element,style} of owner.layoutOriginalStyles){if(style==null)element.removeAttribute('style');else element.setAttribute('style',style);}
      }
    };
    owner.layoutApi=owner.layout;
    // Keep the direct controller's real displayed geometry observable while
    // allowing the browser driver to inject trusted screen-coordinate touch.
    owner.magnifier={
      snapshot:()=>({...zoom.snapshot(),target:'player',surfaceOwner:player.contains(nodes.directSurface)?'player':'outside-player',surfaceTarget:'touchDirectSurface'}),frameRect:()=>{const rect=frame.getBoundingClientRect();return{left:rect.left,top:rect.top,width:rect.width,height:rect.height};},
      setAvailable(value:boolean){owner.zoomAvailable=value;zoom.refreshUi();return zoom.snapshot();},
      applyTransform:(scale:number,x=0,y=0)=>zoom.applyTransform(scale,x,y),
      pointerCursor:()=>owner.pointerEvents.length,
      pointerEventsSince:(cursor:number)=>({events:owner.pointerEvents.slice(cursor),drops:owner.pointerEventDrops}),
      pointerAt:({type,pointerId,clientX,clientY}:{type:string;pointerId:number;clientX:number;clientY:number})=>{
        nodes.directSurface.dispatchEvent(new PointerEvent(type,{bubbles:true,cancelable:true,pointerType:'touch',pointerId,
          clientX,clientY,buttons:type==='pointerup'||type==='pointercancel'?0:1}));
        return{type,pointerId,clientX,clientY,isTrusted:false};
      },
      reset:()=>zoom.reset(),cancel:()=>zoom.cancelGesture(),
    };
    return{game,epoch,movement,focus,layoutTarget:'player',layoutSource:owner.layoutSource,
      layoutFallbackControls:[],bounds:nodes.joystick.getBoundingClientRect().toJSON()};
  }catch(error){
    try{dispose(hostWindow);}catch(cleanupError){
      const original=error instanceof Error?error:new Error(String(error));
      (original as any).cleanupErrors=[...((original as any).cleanupErrors??[]),String(cleanupError)];
      throw original;
    }
    throw error;
  }
}

function bindFixtureGamepad(owner:any,hostWindow:Window,hostDocument:Document,frame:HTMLIFrameElement){
  owner.gamepadService?.dispose?.();owner.gamepadService=null;
  const model=createGamepadMappingModel({storage:owner.gamepadStorage,getGamepads:()=>owner.gamepads});
  model.refresh();
  const sample=model.sample.bind(model);
  model.sample=(seat:number,allowed:boolean)=>{
    const bits=sample(seat,allowed),snapshot=model.getSnapshot(),key=snapshot.seats[seat]??'',device=snapshot.devices.find(item=>item.key===key);
    if(owner.gamepadSamples.length<10000)owner.gamepadSamples.push({seat,bits,selectedKey:key,selectedIndex:device?.index??null,
      devices:snapshot.devices.map(({key,index})=>{
        const pad=owner.gamepads.find((item:any)=>item.index===index);
        return{key,index,buttons:pad?.buttons.map((button:any,buttonIndex:number)=>({index:buttonIndex,pressed:!!button.pressed,value:button.value})).filter((button:any)=>button.pressed||button.value!==0)??null,
          axes:pad?[...pad.axes]:null};
      })});else owner.gamepadSampleDrops++;
    return bits;
  };
  const context=owner.context;
  const runtime={getInputContext:()=>context,subscribe:()=>()=>{}};
  owner.gamepadModel=model;
  owner.gamepadService=createPlayerGamepad({runtime,model,window:hostWindow,document:hostDocument,playerOpen:()=>true});
  return model.getSnapshot();
}

export function installGamepadMapping({firstIndex=0,secondIndex=1}:{firstIndex?:number;secondIndex?:number}={}){
  const owner=(window as any).__acceptanceTouch;if(!owner)throw Error('Install the production touch fixture before gamepad mapping');
  owner.gamepadStorage??=memoryStorage();owner.gamepadSamples=[];owner.gamepadSampleDrops=0;
  const first=virtualGamepad('xbox',firstIndex),second=virtualGamepad('dualsense',secondIndex);
  owner.gamepads=[first,second];
  const snapshot=bindFixtureGamepad(owner,window,document,document.querySelector('iframe') as HTMLIFrameElement);
  return{devices:snapshot.devices,seat:snapshot.seats[0]};
}

function currentGamepad(owner:any,key:string){
  const device=owner.gamepadModel?.getSnapshot().devices.find((item:any)=>item.key===key);
  const pad=owner.gamepads.find((item:any)=>item.index===device?.index);
  if(!pad||!device)throw Error('Unknown production gamepad identity '+key);
  return{device,pad};
}

export function gamepadMapping(operation:string,value:any={}){
  const owner=(window as any).__acceptanceTouch,model=owner?.gamepadModel;if(!owner||!model)throw Error('Production gamepad mapping fixture is absent');
  if(operation==='refresh'){model.refresh();return model.getSnapshot();}
  if(operation==='snapshot')return model.getSnapshot();
  if(operation==='setEnabled'){model.setEnabled(value.enabled===true);return model.getSnapshot();}
  if(operation==='select'){model.select(value.seat,value.key);return model.getSnapshot();}
  if(operation==='bind'){model.bind(value.key,value.action,value.controls);return model.getSnapshot();}
  if(operation==='button'){const {pad}=currentGamepad(owner,value.key);virtualButton(pad,value.index,value.pressed?1:0);return{key:value.key,index:value.index,pressed:!!value.pressed};}
  if(operation==='axis'){const {pad}=currentGamepad(owner,value.key);pad.axes[value.index]=value.value;pad.timestamp++;return{key:value.key,index:value.index,value:value.value};}
  if(operation==='setIndex'){const {pad}=currentGamepad(owner,value.key);pad.index=value.index;model.refresh();return model.getSnapshot();}
  if(operation==='reorder'){
    const assignments=value.assignments;
    if(!Array.isArray(assignments)||assignments.length<2)throw Error('Gamepad reorder needs at least two identity/index pairs');
    const pads=assignments.map((assignment:any)=>({assignment,pad:currentGamepad(owner,assignment.key).pad}));
    if(new Set(assignments.map((assignment:any)=>assignment.index)).size!==assignments.length)throw Error('Gamepad reorder indexes must be unique');
    for(const {assignment,pad} of pads)pad.index=assignment.index;
    model.refresh();return model.getSnapshot();
  }
  if(operation==='cursor')return owner.gamepadSamples.length;
  if(operation==='samplesSince'){
    if(owner.gamepadSampleDrops)throw Error(`Production gamepad sample journal dropped ${owner.gamepadSampleDrops} observations`);
    return owner.gamepadSamples.slice(value.cursor);
  }
  if(operation==='neutralize'){for(const pad of owner.gamepads)virtualNeutral(pad,{buttons:pad.buttons.length,axes:Array.from(pad.axes,()=>0)});return owner.gamepads.map((pad:any)=>({id:pad.id,index:pad.index,buttons:pad.buttons.map((button:any)=>button.value),axes:[...pad.axes]}));}
  if(operation==='reload'){
    for(const pad of owner.gamepads)virtualNeutral(pad,{buttons:pad.buttons.length,axes:Array.from(pad.axes,()=>0)});
    const snapshot=bindFixtureGamepad(owner,window,document,document.querySelector('iframe') as HTMLIFrameElement);
    return snapshot;
  }
  if(operation==='dispose'){
    owner.gamepadService?.dispose?.();owner.gamepadService=null;owner.gamepadModel=null;owner.gamepads=[];return{disposed:true};
  }
  throw Error('Unknown production gamepad mapping operation '+operation);
}

export function layoutScene(operation:string,value:any={}){
  const owner=(window as any).__acceptanceTouch,layout=owner?.layout;if(!owner||!layout)throw Error('Production touch layout fixture is absent');
  if(operation==='begin')return layout.begin(value.orientation);
  if(operation==='updateControl')return layout.updateControl(value.name,value.placement);
  if(operation==='save')return layout.save();
  if(operation==='reload')return layout.reload();
  if(operation==='apply')return layout.apply(value.orientation,value.control??'fire');
  if(operation==='storageSnapshot')return layout.storageSnapshot();
  if(operation==='snapshot')return layout.getSnapshot();
  if(operation==='dispose')return layout.dispose();
  throw Error('Unknown production layout operation '+operation);
}

export function magnifierScene(operation:string,value:any={}){
  const owner=(window as any).__acceptanceTouch,magnifier=owner?.magnifier;if(!owner||!magnifier)throw Error('Production game zoom fixture is absent');
  if(operation==='snapshot')return magnifier.snapshot();
  if(operation==='frameRect')return magnifier.frameRect();
  if(operation==='setAvailable')return magnifier.setAvailable(value.available===true);
  if(operation==='applyTransform')return magnifier.applyTransform(value.scale,value.x??0,value.y??0);
  if(operation==='pointerCursor')return magnifier.pointerCursor();
  if(operation==='pointerEventsSince')return magnifier.pointerEventsSince(value.cursor);
  if(operation==='pointerAt')return magnifier.pointerAt(value);
  if(operation==='reset')return magnifier.reset();
  if(operation==='cancel')return magnifier.cancel();
  throw Error('Unknown production magnifier operation '+operation);
}

export async function pointer({control,type,id=7301,x=.5,y=.5}:{control:string;type:string;id?:number;x?:number;y?:number}) {
  const owner=(window as any).__acceptanceTouch;
  if(!owner?.nodes[control])throw Error('Unknown production touch control '+control);
  const element=owner.nodes[control],rect=element.getBoundingClientRect();
  element.dispatchEvent(new PointerEvent(type,{bubbles:true,cancelable:true,pointerType:'touch',pointerId:id,
    clientX:rect.left+x*rect.width,clientY:rect.top+y*rect.height,buttons:type==='pointerup'||type==='pointercancel'?0:1}));
  // The production joystick intentionally batches controls with requestAnimationFrame.
  await new Promise<void>(resolve=>requestAnimationFrame(()=>resolve()));
  return owner.touch.getSnapshot();
}

export function cancel(){const owner=(window as any).__acceptanceTouch;if(!owner)throw Error('Touch fixture is absent');owner.touch.cancelPointers();}
export function dispose(targetWindow:Window & {__acceptanceTouch?:any}=window as any){
  const owner=targetWindow.__acceptanceTouch;if(!owner)return;
  const errors=[];
  for(const cleanup of owner.cleanup??[])try{cleanup();}catch(error){errors.push(error);}
  try{owner.gamepadService?.dispose?.();}catch(error){errors.push(error);}
  try{owner.layout?.dispose?.();}catch(error){errors.push(error);}
  try{owner.zoom?.cancelGesture?.();owner.zoom?.uninstallInputBridge?.();}catch(error){errors.push(error);}
  try{const viewport=(targetWindow.document?.getElementById('gameViewport') as HTMLElement|null);if(viewport)viewport.style.transform=owner.originalViewportTransform??'';}catch(error){errors.push(error);}
  try{
    const toggle=(targetWindow.document?.getElementById('gameZoomToggle') as HTMLElement|null),label=toggle?.querySelector('strong') as HTMLElement|null,
      scale=targetWindow.document?.getElementById('gameZoomScale') as HTMLElement|null,player=targetWindow.document?.getElementById('player'),
      direct=targetWindow.document?.getElementById('touchDirectSurface') as HTMLElement|null,
      joystick=targetWindow.document?.getElementById('touchJoystick') as HTMLElement|null;
    if(toggle)toggle.hidden=owner.originalZoomToggleHidden??toggle.hidden;if(label)label.textContent=owner.originalZoomLabel??label.textContent;
    if(scale)scale.textContent=owner.originalZoomScaleLabel??scale.textContent;
    if(direct)direct.hidden=owner.originalDirectSurfaceHidden??direct.hidden;
    if(joystick)joystick.hidden=owner.originalJoystickHidden??joystick.hidden;
    if(player){player.classList.toggle('game-zoom-editing',owner.originalZoomEditing===true);
      player.classList.toggle('touch-enabled',owner.originalTouchEnabled===true);
      player.classList.toggle('touch-joystick-enabled',owner.originalJoystickEnabled===true);}
  }catch(error){errors.push(error);}
  try{owner.touch?.dispose?.();}catch(error){errors.push(error);}
  for(const element of owner.layoutFallbackNodes??[])try{element.remove();}catch(error){errors.push(error);}
  try{delete targetWindow.__acceptanceTouch;}catch(error){errors.push(error);}
  if(errors.length)throw new AggregateError(errors,'Touch fixture cleanup failed');
}
