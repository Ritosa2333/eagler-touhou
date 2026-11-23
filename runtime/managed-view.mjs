import {createManagedStageText} from './managed-stage-text.mjs';
import {cooperativeArt} from './coop-hud.mjs';
import {applyCirnoPalettes,chooseCirnoPalettes,cirnoPalettes,loadCirnoPalette} from './coop-cirno-palettes.mjs';
import {WebGlRenderer,StageBackground,parseStd} from '../../th128/site/runtime/renderer-owned-ending-v2.mjs';
import {safeAssetUrl} from '../../th128/site/inspector.mjs';
import {loadOriginalHelpPage,orderDrawLayers,originalHelpPagePins,OriginalPresentationFPS,originalFPSDraws} from '../../th128/site/runtime/draw.mjs';
import {installResourceFetchProvider} from '../../th128/site/resource-fetch.mjs';
import {createManagedTitlePresenter} from './managed-title-presenter.mjs';
import {openManagedRoomTitle} from './managed-stage-resources.mjs';
import {prepareA13Preview,prepareCoopStage} from './preview-world.mjs';

const json=async(owner,key)=>JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(await owner.read(key)));

export function createManagedFps(){
  const meter=new OriginalPresentationFPS();let records=[];
  return Object.freeze({present(now,paused=false){
    if(paused)meter.reset();
    else {if(meter.last!==null&&now-meter.last>1000)meter.reset();
      const rate=meter.observe(now);if(rate!==null)records=originalFPSDraws(rate);}
    return records;
  }});
}

export function confirmedGameOver(pair,seat,confirmed){
  const terminal=pair.terminal?.(seat);
  return terminal?.reason==='game-over'&&terminal.frame<confirmed?terminal:null;
}

export function managedSceneStep(pair,background,{confirmedOnly=false,players=2}={}){
  if(!confirmedOnly)return row=>pair.step(row);
  return row=>{
    if(row?.frame!==background.frame||row.inputs?.length!==players||row.inputs.some(input=>input?.predicted!==false))
      throw new Error('Replay rendering requires contiguous confirmed inputs');
    pair.step(row);background.tick();
  };
}

// Presentation is not a rollback owner. Only switch textures and STD once the first
// successor row is confirmed, even if the simulation has already predicted it.
export function createCoopStageScreen({id,background,signal,open}){
 let current=id,bg=background,pending=null,closed=false,failed=null;
 const life=new AbortController(),live=AbortSignal.any([life.signal,signal]);
 function at(state,confirmed){live.throwIfAborted();if(closed||failed)throw failed??Error('Stage presentation is closed');
  const target=Math.max(0,Math.min(state.stageFrame,confirmed-state.stageStart));
  if(state.stageId!==current||target<bg.frame||pending){
   if(confirmed<=state.stageStart)return null;
   if(!pending||pending.id!==state.stageId){pending?.controller.abort();const controller=new AbortController(),task={id:state.stageId,controller};pending=task;
    Promise.resolve(open(task.id,AbortSignal.any([live,controller.signal]))).then(next=>{if(pending===task&&!live.aborted){bg=next;current=task.id;pending=null;}},e=>{if(pending===task&&!live.aborted){failed=e;pending=null;}});}
   return null;
  }
  while(bg.frame<target)bg.tick();return bg;
 }
 return Object.freeze({at,close(){closed=true;life.abort();pending?.controller.abort();pending=null;bg=null;}});
}
export async function createManagedView({canvas,textCanvas,resources,factory,coopFactory,signal,onError=()=>{}}){
  installResourceFetchProvider(resources);
  const renderer=new WebGlRenderer(canvas),stageText=createManagedStageText({canvas:textCanvas,resources,signal}),none=()=>{};
  let title=null,stage=null,closed=false;
  let titlePresenter=null,helpTexture=null,helpPending=null,helpError=null;
  const check=()=>{signal.throwIfAborted();if(closed||renderer.gl.isContextLost())throw new Error('画面已经失去所有权');};
  const loadedPalettes=new Map();
  function preparePalette(id){
    if(!loadedPalettes.has(id))loadedPalettes.set(id,loadCirnoPalette(renderer,cirnoPalettes[id-1],signal).catch(error=>{loadedPalettes.delete(id);throw error;}));
    return loadedPalettes.get(id);
  }
  const loadedBanks=new Map();async function loadBanks(rows,owner=signal){
    for(const [bank,asset] of rows){
      owner.throwIfAborted();if(loadedBanks.get(bank)===asset)continue;loadedBanks.delete(bank);const metadata=await json(resources,`anim_${asset}`);check();owner.throwIfAborted();
      await renderer.loadBank(bank,metadata,resources.base,{signal:owner});check();owner.throwIfAborted();loadedBanks.set(bank,asset);
    }
  }
  function close(){
    if(closed)return;closed=true;signal.removeEventListener('abort',close);
    titlePresenter?.close();stageText.close();helpPending?.scope.abort();
    try{title?.close();}finally{renderer.dispose();}
  }
  signal.addEventListener('abort',close,{once:true});
  try{
    check();renderer.setResolution(1280,960);
    title=await openManagedRoomTitle({resources,factory,onFactory:none,onTitleTick:none,signal});
    check();if(resources.build?.gameplay!=='same-stage-coop')stage=await prepareA13Preview({resources,factory,onFactory:none,onWorldStart:none,onTick:none,signal});
    // Verify both sets before reporting ready, so launch does not start a
    // player transport while a peer is still waiting for its graphics.
    if(stage)await loadBanks(stage.banks);await loadBanks(title.banks);
  }catch(error){close();throw error;}

  titlePresenter=createManagedTitlePresenter({title,visible:()=>!document.hidden,requestFrame:callback=>requestAnimationFrame(callback),
    cancelFrame:id=>cancelAnimationFrame(id),prepare:owner=>loadBanks(title.banks,owner),onError,draw(){
      check();const records=title.draws();if(!records.length)return false;
      renderer.render(records,{nativeMenu:true});stageText.hide();check();
      canvas.dataset.scene='title';canvas.dataset.frame=String(title.ticks());delete canvas.dataset.gameOver;delete canvas.dataset.resultMenu;delete canvas.dataset.helpPage;delete canvas.dataset.sharedPause;delete canvas.dataset.pauseMenu;delete canvas.dataset.pauseConfirm;delete canvas.dataset.replaySave;delete canvas.dataset.replaySaveSlot;return true;
    }});
  document.addEventListener('visibilitychange',titlePresenter.resetClock,{signal});
  function presentTitle(presentationSignal=signal,options){check();return titlePresenter.present(presentationSignal,options);}
  function stopTitle(){titlePresenter.stop();}
  function helpPage(topic,owner){
    if(helpError)throw helpError;
    if(helpTexture===topic)return true;
    if(helpPending?.topic!==topic){
      helpPending?.scope.abort();
      const request={topic,scope:new AbortController()};helpPending=request;
      const pin=originalHelpPagePins.zh[topic];
      loadOriginalHelpPage(renderer,{...pin,url:safeAssetUrl(resources.base,pin.file)},
        {signal:AbortSignal.any([owner,request.scope.signal]),isCurrent:()=>helpPending===request&&!closed}).then(()=>{
        if(helpPending===request){helpTexture=topic;helpPending=null;}
      }).catch(error=>{if(helpPending===request){helpPending=null;if(!request.scope.signal.aborted&&!owner.aborted)helpError=error;}});
    }
    return false;
  }
  async function openWorld(rules,runSignal,{confirmedOnly=false,seatCues=true}={}){
    check();runSignal.throwIfAborted();stopTitle();const cooperative=rules.mode==='same-stage-coop';
    if(cooperative!==(resources.build?.gameplay==='same-stage-coop'))throw Error('Room mode does not match the loaded battle program');
    const prepared=cooperative?await prepareCoopStage({rules,resources,factory:coopFactory,backgroundFactory:factory,onFactory:none,onWorldStart:none,onTick:none,signal:runSignal}):stage;
    const palettes=cooperative?chooseCirnoPalettes(rules):null;
    if(cooperative){await Promise.all([loadBanks([...prepared.banks,[46,'help']],runSignal),stageText.prepare(),...palettes.filter(Boolean).map(preparePalette)]);runSignal.throwIfAborted();}
    const pair=cooperative?await prepared.openCoopStage(rules,runSignal):await prepared.openPair(rules,runSignal);
    let background,stageScreen=null,lastTag='';const fps=createManagedFps();
    try{
      const initial=cooperative?await prepared.openBackground(rules.stageId,runSignal):await prepared.openBackground();runSignal.throwIfAborted();check();background=new StageBackground(parseStd(initial.std),initial.module);
      if(cooperative)stageScreen=createCoopStageScreen({id:rules.stageId,background,signal:runSignal,async open(id,owner){
        await loadBanks(prepared.banksFor(id),owner);const next=await prepared.openBackground(id,owner);owner.throwIfAborted();check();return new StageBackground(parseStd(next.std),next.module);
      }});
    }catch(error){pair.close();throw error;}
    function display(seat,confirmed,revision,pause=null){
      check();runSignal.throwIfAborted();
      const terminal=pair.terminal?.(seat);
      if(terminal&&terminal.frame>=confirmed)return false;
      const state=pair.state(seat);
      const helpReady=pause?.help?.page?helpPage(pause.help.topic,runSignal):false;
      if(!pause?.help?.page&&helpPending){helpPending.scope.abort();helpPending=null;}
      const next=`${seat}/${state.frame}/${confirmed}/${revision}/${pause?.revision??0}/${helpReady}`;
      if(lastTag===next&&canvas.dataset.scene==='match'&&canvas.dataset.epoch===String(rules.epoch))return false;
      if(cooperative){const active=stageScreen.at(state,confirmed);if(!active)return false;background=active;}else{
        if(confirmedOnly&&background.frame!==confirmed)throw new Error('Replay draw must match the stepped confirmed background');
        while(background.frame<confirmed)background.tick();}
      const native=pair.draws(seat);if(!native.length)return false;
      const personal=cooperative?applyCirnoPalettes(cooperativeArt(native,state,seatCues?seat:null),palettes):native;
      const fpsRecords=fps.present(performance.now(),Boolean(pause)||Boolean(pair.outcome?.()));
      const draws=orderDrawLayers(fpsRecords.length?[...personal,...fpsRecords]:personal,{world:true});
      const textState=cooperative?pair.presentation():state;
      renderer.render(draws,{world:true,background,shake:textState.screenShake});
      if(cooperative&&!pause)stageText.paint(textState);else stageText.hide();
      if(pause){
        renderer.renderOriginalPauseFrame(pause.help?.page&&!helpReady?pause.originalRecords.filter(r=>r.bank!==46):pause.originalRecords,pause.snapshot);
        if(pause.kind==='game-over'){delete canvas.dataset.sharedPause;canvas.dataset.resultMenu='team-over';}
        else{canvas.dataset.sharedPause='true';delete canvas.dataset.resultMenu;}
        canvas.dataset.pauseMenu=pause.selection;canvas.dataset.helpPage=pause.help?.page?String(pause.help.topic):'';canvas.dataset.pauseConfirm=pause.confirm?(pause.yes?'yes':'no'):'';
      }else{delete canvas.dataset.sharedPause;delete canvas.dataset.resultMenu;delete canvas.dataset.helpPage;delete canvas.dataset.pauseMenu;delete canvas.dataset.pauseConfirm;}
      if(pause?.replaySave){canvas.dataset.replaySave=String(pause.replaySave.phase);canvas.dataset.replaySaveSlot=String(pause.replaySave.slot);}
      else{delete canvas.dataset.replaySave;delete canvas.dataset.replaySaveSlot;}
      if(cooperative)canvas.dataset.stage=String(state.stageId);check();
      lastTag=next;canvas.dataset.scene='match';canvas.dataset.epoch=String(rules.epoch);canvas.dataset.frame=String(state.frame);
      const confirmedTerminal=confirmedGameOver(pair,seat,confirmed);
      if(confirmedTerminal)canvas.dataset.gameOver=String(seat+1);else delete canvas.dataset.gameOver;
      return true;
    }
    const timeline=cooperative?{frame:0,tick(){this.frame++;}}:background;
    return Object.freeze({...pair,step:managedSceneStep(pair,timeline,{confirmedOnly,players:rules.players}),display,close(){stageScreen?.close();pair.close();}});
  }
  function displayReplaySave(save,records){
    check();stageText.hide();renderer.render(records,{nativeMenu:true});
    canvas.dataset.scene='replay-save';canvas.dataset.replaySave=String(save.phase);canvas.dataset.replaySaveSlot=String(save.slot);
    delete canvas.dataset.resultMenu;delete canvas.dataset.sharedPause;delete canvas.dataset.endingFrame;
  }
  function displayEnding(pixels,state){
    check();stageText.hide();renderer.renderEndingPixels(pixels,'th128-owned-ending-mp');
    canvas.dataset.scene='ending';canvas.dataset.endingFrame=String(state.endingTicks);
    delete canvas.dataset.sharedPause;delete canvas.dataset.resultMenu;
  }
  return Object.freeze({presentTitle,stopTitle,openWorld,displayEnding,displayReplaySave,close});
}
