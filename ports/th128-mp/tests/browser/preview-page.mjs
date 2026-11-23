import {createMultiplayerRules} from '../../../th128/site/runtime/multiplayer-rules.mjs';
import {orderDrawLayers} from '../../../th128/site/runtime/draw.mjs';
import {WebGlRenderer,StageBackground,parseStd} from '../../../th128/site/runtime/renderer.mjs';
import {openPinnedPreviewResources} from '../../runtime/preview-resources.mjs';
import {prepareA13Preview} from '../../runtime/preview-world.mjs';
import {createPlayerMatch,MATCH_RESTART_SCHEMA} from '../../runtime/player-match.mjs';
import {openRelayLink} from '../../runtime/relay-link.mjs';
import {readMultiplayerReplay} from '../../runtime/confirmed-history.mjs';

const seat=Number(location.pathname.match(/^\/seat-([01])$/)?.[1]);
const byId=id=>document.getElementById(id),screen=byId('game');
const abort=new AbortController(),{signal}=abort;
const counts={factories:0,backgroundFactories:0,worldStarts:0,worldTicks:0,backgroundTicks:0,rendered:0};
const worlds=new Map();
let config,resources,preview,match,link,renderer,phase='loading',lastError=null;
let held=0,delayNext=false,deferred=null,remoteOffer=false,received=0,sent=0,replay=null,replayBusy=false,native=null;
let lastRender='';
const read=async url=>{
  const response=await fetch(url,{signal,cache:'no-store',redirect:'error'});
  if(!response.ok)throw new Error(`Preview resource ${url}: HTTP ${response.status}`);
  return new Uint8Array(await response.arrayBuffer());
};
const set=(id,disabled)=>{byId(id).disabled=disabled;};
function publish(){
  const snapshot=match?.status()??null;
  byId('status').textContent=`P${seat+1} · ${phase}${lastError?` · ${lastError}`:''}`;
  byId('evidence').textContent=JSON.stringify({phase,seat,counts:{...counts},native,match:snapshot,
    held,deferred:deferred?.frame??null,delayingNext:delayNext,sent,received,replay,error:lastError});
  const player=phase==='playing'&&snapshot?.startup.canStart&&!replayBusy;
  const active=player&&['running','voting'].includes(snapshot.phase);
  for(const id of ['left','neutral','right','step'])set(id,!active);
  set('defer',!active||delayNext||deferred!==null);set('release',!active||!deferred);
  set('restart',!active||snapshot.votes[seat]||deferred!==null||delayNext);
  set('replay',!active||snapshot.epoch<2);set('join',phase!=='prepared');
}
function stop(){
  abort.abort();link?.close();link=null;
  try{match?.close();}finally{renderer?.dispose();renderer=null;}
}
function failed(error){
  if(phase==='closed'||phase==='failed')return;
  lastError=error instanceof Error?error.message:String(error);phase='failed';
  try{stop();}finally{publish();}
}
function nextBudget(key,max){if(counts[key]>=max)throw new Error(`Preview ${key} budget reached`);counts[key]++;}
function paint(){
  if(!match||phase!=='playing')return;
  const now=match.status(),world=worlds.get(now.epoch);
  if(!world)throw new Error('Active preview epoch has no native World');
  while(world.background.frame<now.history.nextFrame){nextBudget('backgroundTicks',config.limits.backgroundTicks);world.background.tick();}
  const states=[0,1].map(i=>world.engine.state(i));
  if(states.some(s=>s.normalRecord.bound||s.normalRecord.scoreEligible))throw new Error('Multiplayer must not own the ordinary recorder');
  const tag=JSON.stringify([now.epoch,states[seat].frame,now.history.nextFrame,now.round.replayedFrames]);
  if(tag!==lastRender){
    const draws=world.engine.draws(seat),nativeDraws=orderDrawLayers(draws,{world:true});
    renderer.render(nativeDraws,{world:true,background:world.background,shake:states[seat].screenShake});
    counts.rendered++;lastRender=tag;
    screen.dataset.frame=String(states[seat].frame);screen.dataset.epoch=String(now.epoch);
    native={epoch:now.epoch,draws:draws.length,players:states.map(s=>({frame:s.frame,x:s.playerX,y:s.playerY,record:s.normalRecord.bound})),
      backgroundFrames:world.background.frame};
  }
}
function wireSend(message){
  if(delayNext&&Number.isInteger(message.frame)){
    if(deferred)throw new Error('Only one deferred input is allowed');
    delayNext=false;deferred=message;return;
  }
  link.send(message);sent++;
}
async function openEpoch(rules,ownerSignal){
  const engine=await preview.openPair(rules,ownerSignal);
  try{
    const {module,std}=await preview.openBackground(ownerSignal);
    ownerSignal.throwIfAborted();
    const background=new StageBackground(parseStd(std),module),entry={engine,background};
    worlds.set(rules.epoch,entry);
    return Object.freeze({...engine,close(){if(worlds.get(rules.epoch)===entry)worlds.delete(rules.epoch);engine.close();}});
  }catch(error){engine.close();throw error;}
}
async function ready(){
  if(phase!=='prepared')return;
  phase='joining';publish();
  const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:config.matchId,epoch:1,mode:'parallel-stage',
    route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:2,...resources.build});
  match=await createPlayerMatch({rules,localPlayer:seat,allowedMask:763,openEngine:openEpoch,send:wireSend,signal});
  signal.throwIfAborted();
  const url=new URL(config.relay);
  url.search=new URLSearchParams({room:config.room,run:'1',players:'2',player:String(seat)});
  link=openRelayLink({url,localPlayer:seat,loopbackLegacy:true,onError:failed,async onMessage(message,from){
    received++;
    if(message.schema==='th128-mp-ready/1'&&message.kind==='ready'&&!remoteOffer){remoteOffer=true;match.announceStart();}
    await match.receive(message,from);
    if(message.schema===MATCH_RESTART_SCHEMA)await match.waitRestart();
    if(match.status().startup.canStart)phase='playing';paint();publish();
  }});
  await link.ready;signal.throwIfAborted();match.announceStart();publish();
}
async function checkReplay(){
  const bytes=match.previousReplay();if(!bytes)throw new Error('Previous match Replay missing');
  replayBusy=true;publish();
  let engine;
  try{
    const tape=await readMultiplayerReplay(bytes,resources.build),rules=createMultiplayerRules(tape.header.rules);
    if(tape.status().nextFrame>config.limits.roundFrames)throw new Error('Preview Replay frame limit');
    engine=await preview.openPair(rules,signal);
    for(let frame=0;frame<tape.status().nextFrame;frame++)engine.step(tape.read(frame));
    const players=[0,1].map(i=>{
      const value=engine.state(i);return {frame:value.frame,x:value.playerX,y:value.playerY,record:value.normalRecord.bound};
    });
    const hash=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),byte=>byte.toString(16).padStart(2,'0')).join('');
    replay={epoch:rules.epoch,frames:tape.status().nextFrame,end:tape.status().end,sha256:hash,players};
  }finally{engine?.close();replayBusy=false;publish();}
}
function handle(id,action){byId(id).addEventListener('click',()=>Promise.resolve().then(action).catch(failed));}
handle('join',ready);
for(const [id,value] of [['left',64],['neutral',0],['right',128]])handle(id,()=>{held=value;publish();});
handle('defer',()=>{delayNext=true;publish();});
handle('step',()=>{
  if(match.status().round.committedThrough+match.status().round.pendingFrames+1>=config.limits.roundFrames)throw new Error('Preview round frame limit');
  match.tick(held);paint();publish();
});
handle('release',()=>{const value=deferred;deferred=null;wireSend(value);publish();});
handle('restart',async()=>{match.requestRestart();publish();await match.waitRestart();paint();publish();});
handle('replay',checkReplay);
handle('close',()=>{phase='closed';stop();publish();});
window.addEventListener('pagehide',()=>{if(phase!=='closed'&&phase!=='failed'){phase='closed';stop();}},{once:true});

try{
  if(seat!==0&&seat!==1)throw new Error('Invalid local preview seat');
  config=JSON.parse(new TextDecoder().decode(await read('/preview-config.json')));
  resources=await openPinnedPreviewResources(config.files,read);
  const {default:factory}=await import(resources.url('core'));
  preview=await prepareA13Preview({resources,factory,signal,
    onFactory(kind){nextBudget('factories',config.limits.factories);if(kind==='background')counts.backgroundFactories++;},
    onWorldStart(){nextBudget('worldStarts',config.limits.worldStarts);},onTick(){nextBudget('worldTicks',config.limits.worldTicks);},
  });
  renderer=new WebGlRenderer(screen);
  for(const [bank,name] of preview.banks){
    const metadata=JSON.parse(new TextDecoder().decode(await resources.read(`anim_${name}`)));
    await renderer.loadBank(bank,metadata,new URL('/art/',location.href).href,{signal});
  }
  phase='prepared';publish();
}catch(error){failed(error);}
