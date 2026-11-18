// Standalone equivalent of the launcher's fullscreen/help/options rails.
const icons={fullscreen:'M9 4H4v5m11-5h5v5M4 15v5h5m11-5v5h-5',restore:'M4 9h5V4m11 5h-5V4M9 20v-5H4m11 5v-5h5',help:'M22 12a10 10 0 1 1-20 0 10 10 0 1 1 20 0M9.1 9a3 3 0 0 1 5.8 1c0 2-3 2.8-3 4m.1 3h.01',settings:'M12 8.5a3.5 3.5 0 1 0 0 7 3.5 3.5 0 1 0 0-7ZM10 2l-.8 2.6-2 .9-2.4-.9-2 3.5 1.9 1.8v2.3l-1.9 1.8 2 3.5 2.4-.9 2 .9.8 2.5h4l.8-2.5 2-.9 2.4.9 2-3.5-1.9-1.8V9.9l1.9-1.8-2-3.5-2.4.9-2-.9L14 2z'};
function icon(document,name){
 const svg=document.createElementNS('http://www.w3.org/2000/svg','svg'),path=document.createElementNS(svg.namespaceURI,'path');
 svg.setAttribute('aria-hidden','true');svg.setAttribute('viewBox','0 0 24 24');svg.append(path);
 path.setAttribute('d',icons[name]);return svg;
}
export function mountLocalPlayerTools({document,screen,canvas,input,onExit,getVolumes,setVolume,togglePractice,focusGame=()=>canvas.focus({preventScroll:true}),onError=()=>{}}){
 const rail=document.querySelector('#player-tools'),help=document.querySelector('#player-help'),settings=document.querySelector('#player-settings');
 const full=rail.querySelector('[data-player-tool="fullscreen"]'),helpOpen=rail.querySelector('[data-player-tool="help"]'),gear=rail.querySelector('[data-player-tool="settings"]');
 for(const [button,name]of[[full,'fullscreen'],[helpOpen,'help'],[gear,'settings']])button.append(icon(document,name));
 let open=null,closing=false;
 function isExpanded(){return document.fullscreenElement===screen||screen.classList.contains('player-viewport-full');}
 const syncFullscreen=()=>{
  const active=isExpanded();full.replaceChildren(icon(document,active?'restore':'fullscreen'));
  full.setAttribute('aria-label',active?'退出全屏':'全屏');full.title=active?'退出全屏':'全屏';full.setAttribute('aria-pressed',String(active));
  document.dispatchEvent(new Event('th128-view-change'));
 };
 async function fullscreen(){
  if(screen.classList.contains('player-viewport-full')){screen.classList.remove('player-viewport-full');syncFullscreen();return;}
  if(document.fullscreenElement===screen){await document.exitFullscreen();return;}
  if(!document.fullscreenElement&&document.fullscreenEnabled&&screen.requestFullscreen){
   try{await screen.requestFullscreen();if(document.fullscreenElement===screen)return;}catch{}
  }
  screen.classList.add('player-viewport-full');syncFullscreen();
 }
 function syncVolumes(){for(const range of settings.querySelectorAll('[data-player-volume]')){
   range.value=String(getVolumes()[range.dataset.playerVolume]);range.nextElementSibling.textContent=range.value+'%';
 }}
 function show(next,{restore=false}={}){
  if(open===next&&!restore)return;
  if(next)input.cancelTouch();open=next;
  settings.hidden=next!=='settings';help.hidden=next!=='help';
  gear.setAttribute('aria-expanded',String(next==='settings'));helpOpen.setAttribute('aria-expanded',String(next==='help'));
  if(next==='settings')syncVolumes();else if(next==='help')help.querySelector('[data-help-close]').focus({preventScroll:true});
  else if(restore)focusGame();
 }
 rail.hidden=false;full.addEventListener('click',()=>{input.cancelTouch();void fullscreen().catch(onError);});
 document.addEventListener('fullscreenchange',syncFullscreen);syncFullscreen();
 document.addEventListener('keydown',event=>{
  if(event.code!=='Escape'||!screen.classList.contains('player-viewport-full')||open)return;
  event.preventDefault();event.stopPropagation();void fullscreen().catch(onError);
 },true);
 helpOpen.addEventListener('click',()=>show(open==='help'?null:'help',{restore:open==='help'}));gear.addEventListener('click',()=>show(open==='settings'?null:'settings',{restore:open==='settings'}));
 for(const panel of [rail,help,settings])for(const name of ['keydown','keyup'])panel.addEventListener(name,event=>{
  if(event.code==='Escape'&&open){event.preventDefault();event.stopPropagation();show(null,{restore:true});}
  else event.stopPropagation();
 });
 document.addEventListener('pointerdown',event=>{if(open&&!event.target.closest?.('#player-tools,#player-help,#player-settings'))show(null);},true);
 help.querySelector('[data-help-close]').addEventListener('click',()=>show(null,{restore:true}));help.querySelector('[data-help-background]').addEventListener('click',()=>show(null,{restore:true}));
 for(const range of settings.querySelectorAll('[data-player-volume]'))range.addEventListener('input',()=>{setVolume(range.dataset.playerVolume,Number(range.value));syncVolumes();});
 settings.querySelector('[data-player-action="touch"]').addEventListener('click',()=>{show(null);input.openTouchSettings();});
 settings.querySelector('[data-player-action="layout"]').addEventListener('click',()=>{show(null);input.beginTouchLayout();});
 settings.querySelector('[data-player-action="practice"]').addEventListener('click',()=>{show(null);togglePractice();});
 settings.querySelector('[data-player-action="exit"]').addEventListener('click',async()=>{if(closing)return;closing=true;try{await onExit();}catch(error){closing=false;onError(error);}});
 return{syncVolumes};
}
