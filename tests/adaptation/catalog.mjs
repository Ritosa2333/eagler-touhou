import {existsSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {PRODUCT_GAMES,PRODUCT_IDS} from '../../common/contracts/product-catalog.mjs';
import {MUSIC_BROWSER_GAMES} from '../../tools/ports/music-browser-helpers.mjs';
import {musicAdaptationIds} from '../../tools/ports/music-browser-result.mjs';
import {desktopAdaptationIdsForMode} from '../../tools/ports/runtime-desktop-lifecycle.mjs';
import {practiceAdaptationIds} from '../../tools/ports/practice-observations.mjs';
import {resolveAdaptationAdapter,resolvePracticeAdapter} from './adapters/registry.mjs';
import {TITLE_FLOW_GAMES} from './adapters/title-flow.mjs';
import {resolveSfxLatencyContract} from './adapters/audio.mjs';
import {DIAGNOSTIC_LANE_CHECKS,DIAGNOSTIC_REGISTRY,PAIRED_PRESENTATION_CHECKS,partitionDiagnosticLanes} from './adapters/diagnostics.mjs';
import {notApplicable} from '../../tools/docs/new-adaptation.mjs';

const DEFAULT_ROOT=resolve(import.meta.dirname,'../..');
export const ADAPTATION_GROUPS=Object.freeze([
  'runtime','desktop-input','touch-motion','touch-actions','audio-latency','desktop-background','practice','language','replay','replay-recording','music','presentation','display-replay','verify-quick','verify-daily','offline','offline-update','installation-authority','build','single-game','source-review','rooms','spectator',
]);
const DIAGNOSTIC_GROUP_LANES=Object.freeze({presentation:'presentation','verify-quick':'quick','verify-daily':'daily'});
const UPDATE_CHECKS=Object.freeze(['PACKAGE-03','SAVE-06']);
const MUSIC_AUDIO_CHECKS=Object.freeze(['AUDIO-01','AUDIO-03','AUDIO-04','AUDIO-05']);
const MIDI_MUSIC_CHECKS=Object.freeze(['MIDI-01','MIDI-02','MIDI-03','MIDI-04']);
const OFFLINE_CHECKS=Object.freeze(['RUNTIME-05','PACKAGE-01','PACKAGE-02','PACKAGE-04','PACKAGE-05','SAVE-02','SAVE-03','SAVE-04','SAVE-05']);
const GROUP_CHECKS=Object.freeze({
  build:Object.freeze(['BUILD-01','BUILD-02','BUILD-03','BUILD-04']),
  'single-game':Object.freeze(['BUILD-05']),
  'source-review':Object.freeze(['BUILD-06','VERIFY-05']),
  'installation-authority':Object.freeze(['PACKAGE-06']),
  'display-replay':PAIRED_PRESENTATION_CHECKS,
  runtime:Object.freeze(['RUNTIME-01','RUNTIME-02','RUNTIME-03','RUNTIME-06','INPUT-04','INPUT-06','REPLAY-05','SAVE-01']),
  replay:Object.freeze(['REPLAY-01']),
  'replay-recording':Object.freeze(['REPLAY-02','REPLAY-03','REPLAY-04','REPLAY-06']),
  music:MUSIC_AUDIO_CHECKS,
  language:Object.freeze(['LANG-01','LANG-02','LANG-03','LANG-04','LANG-05','LANG-06']),
  offline:OFFLINE_CHECKS,
  'offline-update':UPDATE_CHECKS,
  rooms:Object.freeze(['MP-01','MP-02','MP-03','MP-04','MP-05','MP-10','MP-12','MP-15']),
  spectator:Object.freeze(['MP-06','MP-07','MP-08','MP-09']),
});
const INPUT={runtime:'EAGLER_TEST_RUNTIME_MANIFEST',packages:'EAGLER_TEST_PACKAGES',artwork:'EAGLER_TEST_ARTWORK',
  music:'EAGLER_TEST_MUSIC_MANIFEST',launcherUrl:'EAGLER_TEST_LAUNCHER_URL',update:'EAGLER_TEST_UPDATE_PACKAGES',
  diagnostics:'EAGLER_TEST_DIAGNOSTIC_MANIFEST',language:'EAGLER_TEST_LANGUAGE_MANIFEST',
  sourceReview:'EAGLER_TEST_SOURCE_REVIEW',minimalRuntime:'EAGLER_TEST_MINIMAL_RUNTIME'};
const NATIVE_GROUP_INTERFACES=Object.freeze({
  runtime:['runtime','observation','flow'],
  'desktop-input':['runtime','observation','flow','movement','clock','controlledScene','actions'],
  'touch-motion':['runtime','observation','flow','movement','clock','controlledScene'],
  'desktop-background':['runtime','observation','flow'],
});

function checksForGroup(group,game,requirements,product=PRODUCT_GAMES[game]){
  if(Object.hasOwn(DIAGNOSTIC_GROUP_LANES,group))return DIAGNOSTIC_LANE_CHECKS[DIAGNOSTIC_GROUP_LANES[group]];
  if(group==='music')return [...MUSIC_AUDIO_CHECKS,...(product?.musicCapabilities?.midi===true?MIDI_MUSIC_CHECKS:[])];
  if(group==='practice')return practiceAdaptationIds(game,'normal');
  if(group==='desktop-input'||group==='desktop-background'||group==='touch-motion'||group==='touch-actions'||group==='audio-latency'){
    const mode=group==='desktop-input'?'input-only':group==='desktop-background'?'background-only':group==='audio-latency'?'audio-latency-only':group==='touch-actions'?'actions-only':'touch-only';
    const rank=new Map(requirements.map((check,index)=>[check.id,index]));
    return desktopAdaptationIdsForMode(mode,{game}).sort((a,b)=>(rank.get(a)??Infinity)-(rank.get(b)??Infinity));
  }
  return GROUP_CHECKS[group];
}

function normalizeVariant(value='both'){
  const normalized=value==='mp'?'multiplayer':value;
  if(!['normal','multiplayer','both'].includes(normalized))throw new Error(`Unsupported adaptation variant ${value}`);
  return normalized;
}

function selectedProducts(games,variant){
  const values=games===undefined?Object.keys(PRODUCT_GAMES):Array.isArray(games)?games:typeof games==='string'?games.split(','):null;
  if(!values||values.length===0||values.some(value=>typeof value!=='string'||!value.trim()))throw new Error('games must be a nonempty list of product IDs');
  const products=[];const seen=new Set();
  for(const raw of values){
    const value=raw.trim();
    let game,mode,modes;
    if(Object.hasOwn(PRODUCT_GAMES,value)){
      game=value;modes=variant==='both'?['normal','multiplayer']:[variant];
    }else if(PRODUCT_IDS.includes(value)){
      mode=value.endsWith('mp')?'multiplayer':'normal';game=mode==='multiplayer'?value.slice(0,-2):value;modes=[mode];
      if(variant!=='both'&&variant!==mode)throw new Error(`Product ${value} conflicts with variant ${variant}`);
    }else throw new Error(`Unknown adaptation product ${value}`);
    for(const selectedMode of modes){
      const key=`${game}/${selectedMode}`;
      if(seen.has(key))throw new Error(`Duplicate adaptation product ${key}`);
      seen.add(key);products.push({game,variant:selectedMode,product:PRODUCT_GAMES[game]});
    }
  }
  return products;
}

function updateManifestState(environment,root){
  const path=environment[INPUT.update];
  if(typeof path!=='string'||!path.trim())return{available:false,reason:`${INPUT.update} is not set`};
  const absolute=resolve(root,path);
  if(!existsSync(absolute))return{available:false,path,reason:`${INPUT.update} does not identify an existing manifest`};
  try{
    const parsed=JSON.parse(readFileSync(absolute,'utf8'));
    const games=parsed?.games;
    if(!games||typeof games!=='object'||Array.isArray(games))throw new Error('manifest.games must be an object');
    const selected=Object.keys(games);
    if(selected.some(game=>!PRODUCT_GAMES[game]))throw new Error('manifest contains an unsupported product');
    return{available:true,path:absolute,games:new Set(selected)};
  }catch(error){return{available:false,path,reason:`Invalid update manifest: ${error.message}`};}
}

function makeCase({root,output,id,game,variant,group,scenarioIds,checks,script,args=[],requiredInputs=[],optionalInputs=[],timeoutMs=1800000,resources=['browser','runtime'],status='ready',reason,blockedScenarios=[],extra={}}){
  const environment=extra.environment??{};
  const inputNames=[...new Set([...requiredInputs,...optionalInputs])];
  const blockedInputs=requiredInputs.filter(name=>typeof environment[name]!=='string'||!environment[name].trim())
    .map(name=>({name,reason:`Required environment input ${name} is missing`}));
  const reportDirectory=resolve(output,'adaptation',id),reportPath=resolve(reportDirectory,'report.json');
  const resolvedArgs=args.map(argument=>typeof argument==='function'?argument(environment):argument);
  let task=null;
  if(status==='ready'&&!blockedInputs.length)task={command:process.execPath,args:[script,...resolvedArgs,`--output=${reportDirectory}`],cwd:root,timeoutMs,resources:[...resources]};
  if(blockedInputs.length&&status==='ready')status='blocked';
  const blockedChecks=[...new Set(blockedScenarios.flatMap(scenario=>scenario.checks))];
  return{
    id,game,variant,group,scenarioIds:[...scenarioIds],...(scenarioIds.length===1?{scenarioId:scenarioIds[0]}:{}),
    checks:[...checks],status,task,requiresLauncherBuild:['offline','offline-update','installation-authority','language'].includes(group),
    requiredInputs:[...requiredInputs],inputs:inputNames,blockedInputs,
    blockedScenarios:blockedScenarios.map(scenario=>({...scenario,checks:[...scenario.checks]})),blockedChecks,
    reportPath,...(reason?{reason}:{}),...extra.caseFields,
  };
}

function argFor(environment,name,option=name){
  const value=environment[name];return `--${option}=${typeof value==='string'&&value.trim()?value:`<${name}>`}`;
}

function addNormalCases({cases,game,product,groups,selectedChecks,root,output,environment,requirements,workspaceRepositories}){
  const baseGroups=['runtime','desktop-input','touch-motion','touch-actions','replay-recording','audio-latency','desktop-background','practice','music'];
  for(const group of baseGroups){
    if(!groups.has(group))continue;
    if(group==='touch-motion'&&groups.has('desktop-input'))continue;
    const supports=group==='practice'?product.features?.thprac===true:true;
    const scenarioId={runtime:'native-runtime-contract', 'desktop-input':'desktop-input-effects',
      'touch-motion':'native-touch-motion-contract','touch-actions':'native-touch-action-consumers','replay-recording':'finite-replay-recording-and-live-isolation','audio-latency':'native-sfx-queue-latency','desktop-background':'desktop-background-and-resume',practice:'native-practice-menu-and-locale',music:'title-ogg-pcm'}[group];
    const mapped=checksForGroup(group,game,requirements,product).filter(check=>selectedChecks===null||selectedChecks.has(check));
    if(selectedChecks!==null&&!mapped.length)continue;
    let common={root,output,game,variant:'normal',group,scenarioIds:[scenarioId],checks:mapped,extra:{environment}};
    if(group==='music'){
      const owner=workspaceRepositories?.[game];
      const ownerAvailable=typeof owner==='string'&&owner.trim()&&existsSync(resolve(root,owner));
      const packageMusic=product.package?.musicSourceDirectories?.ogg;
      const mountMusic=product.package?.musicMounts?.ogg;
      const hasOgg=typeof packageMusic==='string'&&packageMusic.trim()&&typeof mountMusic==='string'&&mountMusic.trim();
      const driverAvailable=MUSIC_BROWSER_GAMES.includes(game)&&ownerAvailable&&hasOgg;
      const implemented=new Set(musicAdaptationIds(game));
      if(!driverAvailable){
        const reason=!ownerAvailable?`Register the ${game} production owner in launcher/config/workspace.json`:
          !hasOgg?`Declare the ${game} OGG source directory and Runtime mount in the product catalog`:
            `Register a normal OGG browser scene for ${game}`;
        cases.push(makeCase({...common,id:`music-normal-${game}-blocked`,status:'blocked',reason}));
        continue;
      }
      const runnableChecks=mapped.filter(check=>implemented.has(check));
      const blockedChecks=mapped.filter(check=>!implemented.has(check));
      if(blockedChecks.length)cases.push(makeCase({...common,id:`music-normal-${game}-blocked`,checks:blockedChecks,status:'blocked',
        reason:`Register native ${game} observations and scenes for ${blockedChecks.join(', ')}`}));
      if(!runnableChecks.length)continue;
      common={...common,checks:runnableChecks};
    }
    if(Object.hasOwn(NATIVE_GROUP_INTERFACES,group)&&!(group==='touch-motion'&&product.inputProfile==='fighting')){
      const adapter=resolveAdaptationAdapter(game,'normal');
      const missing=NATIVE_GROUP_INTERFACES[group].filter(name=>!adapter?.[name]||
        name==='observation'&&adapter[name].kind==='unsupported');
      if(missing.length){
        cases.push(makeCase({...common,id:`${group}-normal-${game}`,status:'blocked',
          reason:`Register ${game}/normal native observations and scenario flow | missing: ${missing.join(', ')}`}));continue;
      }
    }
    if(group==='audio-latency'&&!resolveSfxLatencyContract(game,'normal')){
      cases.push(makeCase({...common,id:`audio-latency-normal-${game}`,status:'blocked',
        reason:'Register the native logical sound, production voice and input scene in the audio adapter'}));continue;
    }
    if(group==='desktop-input'&&mapped.includes('INPUT-07')){
      const adapter=resolveAdaptationAdapter(game,'normal');
      const missing=['originalMenu','dialogueTouchScene'].filter(name=>!adapter?.[name]);
      if(missing.length){cases.push(makeCase({...common,id:`desktop-input-normal-${game}`,status:'blocked',
        reason:`Register the original menu and loaded-dialogue input interfaces | ${missing.join(' | ')}`}));continue;}
    }
    if(group==='replay-recording'){
      const adapter=resolveAdaptationAdapter(game,'normal');
      if(!adapter?.replay||!adapter.clock||!adapter.parseReplayFrames){
        cases.push(makeCase({...common,id:`replay-recording-normal-${game}`,status:'blocked',
          reason:'Register the native Replay trace, independent file parser, recording semantics and production clock'}));continue;
      }
    }
    if((group==='touch-motion'||group==='touch-actions')&&product.inputProfile==='fighting'){
      cases.push(makeCase({...common,id:`${group}-normal-${game}`,status:'not-applicable',reason:'Fighting profile uses its directional movement contract'}));continue;
    }
    const actionAdapter=group==='touch-actions'?resolveAdaptationAdapter(game,'normal'):null;
    if(group==='touch-actions'&&(!actionAdapter?.actions||!actionAdapter.clock||!actionAdapter.controlledScene)){
      cases.push(makeCase({...common,id:`touch-actions-normal-${game}`,status:'blocked',
        reason:'Register the actual player-consumer snapshot, native input bits, production clock and controlled scene in the title adapter'}));continue;
    }
    if(group==='touch-actions'){
      const obligations={'TOUCH-09':['deathbomb'],'TOUCH-10':['runBoundaries','parseReplayMetadata'],
        'TOUCH-11':['doubleTapBomb'],'TOUCH-12':['originalMenu','dialogueTouchScene'],'TOUCH-13':['dialogueTouchScene']};
      const missing=mapped.flatMap(check=>(obligations[check]??[]).filter(capability=>!actionAdapter[capability])
        .map(capability=>`${check}: ${capability}`));
      if(missing.length){cases.push(makeCase({...common,id:`touch-actions-normal-${game}`,status:'blocked',
        reason:`Register the selected native behavior interfaces | ${missing.join(' | ')}`}));continue;}
    }
    if(!supports){cases.push(makeCase({...common,id:`${group}-normal-${game}`,status:'not-applicable',reason:'Product directory declares no practice feature'}));continue;}
    if(group==='practice'&&!resolvePracticeAdapter(game,'normal')){
      cases.push(makeCase({...common,id:`practice-normal-${game}`,status:'blocked',
        reason:'Register the native practice snapshot, independent parameter expectations and title menu operations'}));continue;
    }
    const script={runtime:'tools/ports/runtime-browser.mjs','desktop-input':'tools/ports/runtime-desktop-probes.mjs',
      'touch-motion':'tools/ports/runtime-desktop-probes.mjs','touch-actions':'tools/ports/runtime-desktop-probes.mjs','replay-recording':'tools/ports/runtime-desktop-probes.mjs','audio-latency':'tools/ports/runtime-desktop-probes.mjs','desktop-background':'tools/ports/runtime-desktop-probes.mjs',practice:'tools/ports/practice-browser.mjs',music:'tools/ports/music-browser.mjs'}[group];
    const args=[argFor(environment,INPUT.runtime,'manifest'),`--game=${game}`,'--variant=normal'];
    if(group==='runtime')args.push('--gamepad');
    if(group==='desktop-input')args.push('--input-only');
    if(group==='touch-motion')args.push('--touch-only');
    if(group==='touch-actions')args.push('--actions-only');
    if(group==='replay-recording')args.push('--replay-recording-only');
    if(group==='audio-latency')args.push('--audio-latency-only');
    if(group==='desktop-background')args.push('--background-only');
      if(selectedChecks!==null&&['desktop-input','touch-motion','touch-actions','audio-latency','desktop-background','replay-recording'].includes(group))
        args.push(`--check=${mapped.join(',')}`);
    if(group==='music')args.push(argFor(environment,INPUT.music,'music-manifest'));
    const requiredInputs=group==='music'?[INPUT.runtime,INPUT.music]:[INPUT.runtime];
    const resources=['browser','runtime'];
    cases.push(makeCase({...common,id:`${group}-normal-${game}`,script,args,requiredInputs,timeoutMs:1800000,resources}));
  }
}

function addDiagnosticCases({cases,products,root,output,environment,groups,selectedChecks,diagnosticRegistry}){
  for(const {game,variant,product} of products){
    const lanes=Object.entries(DIAGNOSTIC_GROUP_LANES).filter(([group,lane])=>groups.has(group)&&
      (selectedChecks===null||DIAGNOSTIC_LANE_CHECKS[lane].some(check=>selectedChecks.has(check)))).map(([,lane])=>lane);
    if(!lanes.length)continue;
    const profile=diagnosticRegistry.resolve(game,variant);
    const partition=partitionDiagnosticLanes(profile,lanes);
    const makeLaneCase=(caseLanes,{status='ready',reason}={})=>{
      const checks=[...new Set(caseLanes.flatMap(lane=>DIAGNOSTIC_LANE_CHECKS[lane]))]
        .filter(check=>selectedChecks===null||selectedChecks.has(check));
      const common={root,output,id:`diagnostic-${variant}-${game}${partition.ready.length&&partition.blocked.length&&status==='blocked'?'-blocked-lanes':''}`,
        game,variant,group:'diagnostic',scenarioIds:caseLanes.map(lane=>`diagnostic-${lane}`),checks,status,reason,
        extra:{environment,caseFields:{diagnosticLanes:caseLanes}}};
      if(status!=='ready')return makeCase(common);
      return makeCase({...common,script:'tools/ports/diagnostic-browser.mjs',
        args:[argFor(environment,INPUT.runtime,'manifest'),argFor(environment,INPUT.diagnostics,'diagnostics'),
          `--game=${game}`,`--variant=${variant}`,`--lanes=${caseLanes.join(',')}`],
        requiredInputs:[INPUT.runtime,INPUT.diagnostics],timeoutMs:10800000,resources:['browser','runtime','diagnostic-runtime']});
    };
    if(variant==='multiplayer'&&!product.multiplayerRuntime){
      cases.push(makeLaneCase(lanes,{status:'not-applicable',reason:'Product directory declares no multiplayer Runtime'}));continue;
    }
    if(partition.ready.length)cases.push(makeLaneCase(partition.ready));
    if(partition.blocked.length){
      const reason=profile?'Register the selected diagnostic lanes for this product and variant':
        'Register the diagnostic scene, capture adapters and accepted original golden inputs';
      cases.push(makeLaneCase(partition.blocked,{status:'blocked',reason}));
    }
  }
}

function addLanguageCases({cases,products,root,output,environment,groups,selectedChecks}){
  if(!groups.has('language'))return;
  for(const {game,variant,product} of products){
    const checks=GROUP_CHECKS.language.filter(id=>selectedChecks===null||selectedChecks.has(id));
    if(!checks.length)continue;
    const common={root,output,id:`language-${variant}-${game}`,game,variant,group:'language',
      scenarioIds:['language-cold-launch-and-fallback'],checks,extra:{environment}};
    if(!product.features.languages){cases.push(makeCase({...common,status:'not-applicable',reason:'Product declares no downloadable language packs'}));continue;}
    if(!resolveAdaptationAdapter(game,variant)?.language||variant!=='normal'){
      cases.push(makeCase({...common,status:'blocked',reason:'Register the selected mode language observation, text consumers and Launcher journey'}));continue;
    }
    cases.push(makeCase({...common,script:'tools/ports/language-browser.mjs',args:[argFor(environment,INPUT.runtime,'manifest'),
      argFor(environment,INPUT.language,'language-manifest'),argFor(environment,INPUT.artwork,'artwork-dir'),`--game=${game}`,`--variant=${variant}`],
      requiredInputs:[INPUT.runtime,INPUT.language,INPUT.artwork],resources:['browser','runtime','launcher-build'],timeoutMs:1800000}));
  }
}

function addBuildCases({cases,products,root,output,environment,groups,selectedChecks}){
  if(!groups.has('build'))return;
  for(const {game,variant} of products){
    const checks=GROUP_CHECKS.build.filter(id=>selectedChecks===null||selectedChecks.has(id));
    if(!checks.length)continue;
    cases.push(makeCase({root,output,id:`build-${variant}-${game}`,game,variant,group:'build',
      scenarioIds:['revalidate-current-runtime-build-and-package'],checks,
      script:'tools/ports/build-acceptance.mjs',args:[`--stage=verify-existing`,argFor(environment,INPUT.runtime,'runtime-manifest'),
        `--game=${game}`,`--variant=${variant}`],requiredInputs:[INPUT.runtime],timeoutMs:180000,
      resources:['runtime'],extra:{environment}}));
  }
}

function addSourceJourneyCases({cases,products,root,output,environment,groups,selectedChecks}){
  for(const group of ['single-game','source-review']){
    if(!groups.has(group))continue;
    const checks=GROUP_CHECKS[group].filter(id=>selectedChecks===null||selectedChecks.has(id));
    if(!checks.length)continue;
    for(const {game,variant,product} of products){
      const applicable=variant==='normal'||group==='source-review'&&!!product.multiplayerRuntime;
      const name=group==='single-game'?INPUT.minimalRuntime:INPUT.sourceReview;
      cases.push(makeCase({root,output,id:`${group}-${variant}-${game}`,game,variant,group,checks,
        scenarioIds:[group==='single-game'?'single-game-checkout-and-launcher':'reviewed-source-and-original-reference'],
        status:applicable?'ready':product.multiplayerRuntime?'blocked':'not-applicable',
        reason:applicable?undefined:product.multiplayerRuntime?'Register the room-owned minimal checkout journey':'No multiplayer Runtime',
        script:group==='single-game'?'tools/ports/minimal-runtime-browser.mjs':'tools/ports/adaptation-source-review.mjs',
        args:[argFor(environment,name,'manifest'),`--game=${game}`,`--variant=${variant}`,
          ...(group==='source-review'?[`--checks=${checks.join(',')}`]:[])],
        requiredInputs:[name],timeoutMs:group==='single-game'?1200000:120000,
        resources:group==='single-game'?['browser','runtime','launcher-build']:[],extra:{environment}}));
    }
  }
}

function addInstallationAuthorityCases({cases,products,root,output,environment,groups,selectedChecks,updateState}){
  if(!groups.has('installation-authority'))return;
  const checks=GROUP_CHECKS['installation-authority'].filter(id=>selectedChecks===null||selectedChecks.has(id));
  if(!checks.length)return;
  for(const {game,variant,product} of products){
    const applicable=variant==='normal',hasUpdate=updateState.available&&updateState.games.has(game);
    cases.push(makeCase({root,output,id:`installation-authority-${variant}-${game}`,game,variant,group:'installation-authority',
      scenarioIds:['one-package-installation-across-launcher-and-runtime'],checks,
      status:!applicable?(product.multiplayerRuntime?'blocked':'not-applicable'):hasUpdate?'ready':'blocked',
      reason:!applicable?'Register the room-owned installation journey':!hasUpdate?
        (updateState.reason??`Update manifest has no second package for ${game}`):undefined,
      script:'tools/ports/offline-runtime-browser.mjs',args:[argFor(environment,INPUT.runtime,'manifest'),argFor(environment,INPUT.packages,'packages'),
        argFor(environment,INPUT.artwork,'artwork-dir'),argFor(environment,INPUT.update,'update-manifest'),`--game=${game}`,'--installation-authority-only'],
      requiredInputs:[INPUT.runtime,INPUT.packages,INPUT.artwork,INPUT.update],timeoutMs:1200000,
      resources:['browser','runtime','launcher-build'],extra:{environment}}));
  }
}

function addPairedPresentationCases({cases,products,root,output,environment,groups,selectedChecks,diagnosticRegistry}){
  if(!groups.has('display-replay'))return;
  const checks=PAIRED_PRESENTATION_CHECKS.filter(id=>selectedChecks===null||selectedChecks.has(id));
  if(!checks.length)return;
  for(const {game,variant,product} of products){
    const profile=diagnosticRegistry.resolve(game,variant),capability=profile?.presentation?.pairedReplay;
    const applicable=variant==='normal'||!!product.multiplayerRuntime;
    cases.push(makeCase({root,output,id:`display-replay-${variant}-${game}`,game,variant,group:'diagnostic',checks,
      scenarioIds:['paired-replay-presentation-and-review-material'],
      status:!applicable?'not-applicable':capability?'ready':'blocked',
      reason:!applicable?'Product declares no multiplayer Runtime':!capability?'Register the paired Replay, authority observations and display review materials':undefined,
      script:'tools/ports/diagnostic-browser.mjs',args:[argFor(environment,INPUT.runtime,'manifest'),argFor(environment,INPUT.diagnostics,'diagnostics'),
        `--game=${game}`,`--variant=${variant}`,'--lanes=presentation','--paired-replay'],
      requiredInputs:[INPUT.runtime,INPUT.diagnostics],timeoutMs:1200000,resources:['browser','runtime','diagnostic-runtime'],
      extra:{environment,caseFields:{diagnosticLanes:['presentation'],diagnosticPairedReplay:true}}}));
  }
}

function addReplayCases({cases,productEntry,root,output,environment,groups,selectedChecks}){
  if(!groups.has('replay'))return;
  const {game,variant,product}=productEntry;
  const candidates=[...GROUP_CHECKS.replay,...(variant==='multiplayer'?['MP-11']:[])];
  const checks=candidates.filter(check=>selectedChecks===null||selectedChecks.has(check));
  if(selectedChecks!==null&&!checks.length)return;
  if(variant==='multiplayer'&&!product.multiplayerRuntime){cases.push(makeCase({root,output,id:`replay-multiplayer-${game}`,game,variant,group:'replay',scenarioIds:['native-replay-import-and-playback'],checks,
    status:'not-applicable',reason:'Product directory declares no multiplayer Runtime'}));return;}
  const manifest=environment[INPUT.runtime];
  const args=[`--manifest=${manifest||`<${INPUT.runtime}>`}`,`--game=${game}`,`--variant=${variant}`];
  cases.push(makeCase({root,output,id:`replay-${variant}-${game}`,game,variant,group:'replay',scenarioIds:['native-replay-import-and-playback'],checks,
    script:'tools/ports/replay-browser.mjs',args,requiredInputs:[INPUT.runtime],timeoutMs:3600000,resources:['browser','runtime'],extra:{environment}}));
}

function addOfflineUpdateOnlyCases({cases,products,root,output,environment,updateState,selectedChecks}){
  const mapped=UPDATE_CHECKS.filter(check=>selectedChecks===null||selectedChecks.has(check));
  if(!mapped.length)return;
  for(const {game} of products){
    const blocked=!updateState.available||!updateState.games.has(game);
    const blockedScenarios=blocked?[{id:'package-update-and-user-data-recovery',checks:mapped,
      reason:updateState.available?`Update manifest has no second package for ${game}`:updateState.reason}]:[];
    const hasBaseInputs=[INPUT.runtime,INPUT.packages,INPUT.artwork].every(name=>typeof environment[name]==='string'&&environment[name].trim());
    const taskReady=!blocked&&hasBaseInputs;
    cases.push(makeCase({root,output,id:`offline-update-normal-${game}`,game,variant:'normal',group:'offline-update',
      scenarioIds:['offline-import-reopen','package-update-and-user-data-recovery'],checks:mapped,
      script:taskReady?'tools/ports/offline-runtime-browser.mjs':undefined,
      args:taskReady?[argFor(environment,INPUT.runtime,'manifest'),argFor(environment,INPUT.packages,'packages'),
        argFor(environment,INPUT.artwork,'artwork-dir'),argFor(environment,INPUT.update,'update-manifest'),`--game=${game}`]:[],
      requiredInputs:[INPUT.runtime,INPUT.packages,INPUT.artwork,...(blocked?[]:[INPUT.update])],
      optionalInputs:blocked?[INPUT.update]:[],
      timeoutMs:900000,resources:['browser','runtime','launcher-build'],status:blocked?'blocked':'ready',
      reason:blocked?blockedScenarios[0]?.reason:hasBaseInputs?undefined:'Base offline inputs are missing',blockedScenarios,
      extra:{environment,caseFields:{executionIncludesBaseOffline:true}}}));
  }
}

function addRoomCases({cases,products,root,output,environment,groups,selectedChecks}){
  const roomInputNames=[INPUT.runtime,INPUT.launcherUrl];
  if(groups.has('rooms'))for(const {game,product} of products){
    const checks=GROUP_CHECKS.rooms.filter(check=>selectedChecks===null||selectedChecks.has(check));
    if(selectedChecks!==null&&!checks.length)continue;
    for(const players of [2,3])for(const route of ['rtc','relay']){
      const supported=!!product.multiplayerRuntime&&!!product.multiplayer?.playerCounts.includes(players);
      const scenarioId=`rooms-${players}p-${route}`;
      const id=`${scenarioId}-multiplayer-${game}`;
      if(!supported){cases.push(makeCase({root,output,id,game,variant:'multiplayer',group:'rooms',scenarioIds:[scenarioId],checks,
        status:'not-applicable',reason:!product.multiplayerRuntime?'Product directory declares no multiplayer Runtime':`Product directory does not support ${players} players`}));continue;}
      const args=[argFor(environment,INPUT.runtime,'manifest'),argFor(environment,INPUT.launcherUrl,'url'),`--route=${route}`,`--players=${players}`,`--game=${game}`,'--disconnect','--gamepad','--lifecycle'];
      if(environment[INPUT.packages])args.push(argFor(environment,INPUT.packages,'packages'));
      cases.push(makeCase({root,output,id,game,variant:'multiplayer',group:'rooms',scenarioIds:[scenarioId],checks,
        script:'tools/ports/room-browser.mjs',args,requiredInputs:roomInputNames,optionalInputs:environment[INPUT.packages]?[INPUT.packages]:[],
        timeoutMs:1800000,resources:['browser','runtime',...(route==='relay'?['relay-room']:[])],extra:{environment}}));
    }
  }
  if(groups.has('spectator'))for(const {game,product} of products){
    const checks=GROUP_CHECKS.spectator.filter(check=>selectedChecks===null||selectedChecks.has(check));
    if(selectedChecks!==null&&!checks.length)continue;
    const scenarioId='spectator-join-playback-and-input-isolation';
    if(!product.multiplayerRuntime||!product.multiplayer?.playerCounts.includes(2)){
      cases.push(makeCase({root,output,id:`spectator-multiplayer-${game}`,game,variant:'multiplayer',group:'spectator',scenarioIds:[scenarioId],checks,
        status:'not-applicable',reason:!product.multiplayerRuntime?'Product directory declares no multiplayer Runtime':'Product directory does not support 2-player rooms'}));continue;
    }
    const args=[`--manifest=${environment[INPUT.runtime]||`<${INPUT.runtime}>`}`,`--url=${environment[INPUT.launcherUrl]||`<${INPUT.launcherUrl}>`}`,
      '--route=relay','--players=2','--spectator',`--game=${game}`];
    if(environment[INPUT.packages])args.push(`--packages=${environment[INPUT.packages]}`);
    cases.push(makeCase({root,output,id:`spectator-multiplayer-${game}`,game,variant:'multiplayer',group:'spectator',scenarioIds:[scenarioId],checks,
      script:'tools/ports/room-browser.mjs',args,requiredInputs:roomInputNames,optionalInputs:environment[INPUT.packages]?[INPUT.packages]:[],timeoutMs:1800000,
      resources:['browser','runtime','relay-room'],extra:{environment}}));
  }
}

export function buildAdaptationPlan({root=DEFAULT_ROOT,output=resolve(root,'artifacts/validation/adaptation'),environment=process.env,
  games,variant='both',checks=[],groups=[],diagnosticRegistry=DIAGNOSTIC_REGISTRY}={}){
  root=resolve(root);output=resolve(output);variant=normalizeVariant(variant);
  if(!environment||typeof environment!=='object'||Array.isArray(environment))throw new Error('environment must be an object');
  if(!Array.isArray(checks)||checks.some(check=>typeof check!=='string'||!check.trim()))throw new Error('checks must be an array of adaptation IDs');
  if(new Set(checks).size!==checks.length)throw new Error('checks must not contain duplicates');
  const requirements=JSON.parse(readFileSync(resolve(root,'docs/adaptation/checklist.json'),'utf8')).checks;
  const workspaceRepositories=JSON.parse(readFileSync(resolve(root,'launcher/config/workspace.json'),'utf8')).repositories;
  const knownChecks=new Set(requirements.map(item=>item.id));
  for(const check of checks)if(!knownChecks.has(check))throw new Error(`Unknown adaptation check ${check}`);
  if(!Array.isArray(groups)||groups.some(group=>typeof group!=='string'||!ADAPTATION_GROUPS.includes(group)))throw new Error('groups contains an unknown adaptation group');
  const selectedGroups=new Set(groups.length?groups:ADAPTATION_GROUPS),selectedChecks=checks.length?new Set(checks):null;
  const products=selectedProducts(games,variant),cases=[],updateState=updateManifestState(environment,root);
  const updateSelected=selectedGroups.has('offline-update')||selectedGroups.has('offline')&&selectedChecks===null||
    !!selectedChecks&&UPDATE_CHECKS.some(check=>selectedChecks.has(check));
  const normalGames=[...new Set(products.filter(entry=>entry.variant==='normal').map(entry=>entry.game))];
  for(const game of normalGames)addNormalCases({cases,game,product:PRODUCT_GAMES[game],groups:selectedGroups,selectedChecks,root,output,environment,requirements,workspaceRepositories});
  if(selectedGroups.has('replay-recording'))for(const {game,variant:mode,product} of products.filter(entry=>entry.variant==='multiplayer')){
    const checks=GROUP_CHECKS['replay-recording'].filter(check=>selectedChecks===null||selectedChecks.has(check));
    if(selectedChecks!==null&&!checks.length)continue;
    cases.push(makeCase({root,output,id:`replay-recording-${mode}-${game}`,game,variant:mode,group:'replay-recording',
      scenarioIds:['finite-replay-recording-and-live-isolation'],checks,status:product.multiplayerRuntime?'blocked':'not-applicable',
      reason:product.multiplayerRuntime?'Register the room-owned recording, per-seat input and Replay trace scene':
        'Product directory declares no multiplayer Runtime'}));
  }
  for(const group of ['touch-motion','touch-actions'])if(selectedGroups.has(group))for(const {game,variant:mode,product} of products.filter(entry=>entry.variant==='multiplayer')){
    const checks=checksForGroup(group,game,requirements).filter(check=>selectedChecks===null||selectedChecks.has(check));
    if(selectedChecks!==null&&!checks.length)continue;
    const applicable=!!product.multiplayerRuntime&&product.inputProfile!=='fighting';
    cases.push(makeCase({root,output,id:`${group}-${mode}-${game}`,game,variant:mode,group,
      scenarioIds:[group==='touch-actions'?'native-touch-action-consumers':'native-touch-motion-contract'],checks,status:applicable?'blocked':'not-applicable',
      reason:applicable?group==='touch-actions'?'Register the room-owned player-consumer action snapshot and scene adapter':
        'Register the room-owned touch scene adapter and completed native movement trace':
        'This product does not declare multiplayer STG movement'}));
  }
  if(selectedGroups.has('audio-latency'))for(const {game,variant:mode,product} of products.filter(entry=>entry.variant==='multiplayer')){
    const checks=checksForGroup('audio-latency',game,requirements).filter(check=>selectedChecks===null||selectedChecks.has(check));
    if(selectedChecks!==null&&!checks.length)continue;
    cases.push(makeCase({root,output,id:`audio-latency-${mode}-${game}`,game,variant:mode,group:'audio-latency',
      scenarioIds:['native-sfx-queue-latency'],checks,status:product.multiplayerRuntime?'blocked':'not-applicable',
      reason:product.multiplayerRuntime?'Register the room-owned native sound event and PCM timing scene':
        'Product directory declares no multiplayer Runtime'}));
  }
  if(selectedGroups.has('music'))for(const {game,variant:mode,product} of products.filter(entry=>entry.variant==='multiplayer')){
    const checks=checksForGroup('music',game,requirements,product).filter(check=>selectedChecks===null||selectedChecks.has(check));
    if(selectedChecks!==null&&!checks.length)continue;
    const applicable=!!product.multiplayerRuntime;
    const mpOwner=workspaceRepositories?.[`${game}mp`];
    const ownerAvailable=typeof mpOwner==='string'&&mpOwner.trim()&&existsSync(resolve(root,mpOwner));
    cases.push(makeCase({root,output,id:`music-${mode}-${game}`,game,variant:mode,group:'music',
      scenarioIds:['multiplayer-music-output-and-lifecycle'],checks,status:applicable?'blocked':'not-applicable',
      reason:applicable&&!ownerAvailable?`Register the ${game}mp production owner in launcher/config/workspace.json`:
        applicable?'Register a room-owned music output, lifecycle and SFX-suppression scene':
        'Product directory declares no multiplayer Runtime'}));
  }
  if(selectedGroups.has('offline')){
    for(const game of normalGames){
      const baseChecks=OFFLINE_CHECKS.filter(check=>selectedChecks===null||selectedChecks.has(check));
      const updateChecks=updateSelected?UPDATE_CHECKS.filter(check=>selectedChecks===null||selectedChecks.has(check)):[];
      const includeBase=selectedChecks===null||baseChecks.length>0,includeUpdate=updateChecks.length>0;
      if(!includeBase&&!includeUpdate)continue;
      const updateBlocked=includeUpdate&&(!updateState.available||!updateState.games.has(game));
      const blockedScenarios=updateBlocked?[{id:'package-update-and-user-data-recovery',checks:updateChecks,
        reason:updateState.available?`Update manifest has no second package for ${game}`:updateState.reason}]:[];
      const updateAvailable=includeUpdate&&!updateBlocked,runTask=includeBase||updateAvailable;
      const scenarioIds=[];if(includeBase||updateAvailable)scenarioIds.push('offline-import-reopen');if(includeUpdate)scenarioIds.push('package-update-and-user-data-recovery');
      const args=runTask?[argFor(environment,INPUT.runtime,'manifest'),argFor(environment,INPUT.packages,'packages'),
        argFor(environment,INPUT.artwork,'artwork-dir'),`--game=${game}`]:[];
      if(updateAvailable)args.push(argFor(environment,INPUT.update,'update-manifest'));
      const checksForCase=[...(includeBase?baseChecks:[]),...updateChecks];
      cases.push(makeCase({root,output,id:`offline-normal-${game}`,game,variant:'normal',group:'offline',scenarioIds,checks:checksForCase,
        script:runTask?'tools/ports/offline-runtime-browser.mjs':undefined,args,
        requiredInputs:runTask?[INPUT.runtime,INPUT.packages,INPUT.artwork,...(updateAvailable?[INPUT.update]:[])]:[],
        optionalInputs:includeUpdate&&!updateAvailable?[INPUT.update]:[],timeoutMs:updateAvailable?900000:600000,resources:['browser','runtime','launcher-build'],
        status:runTask?'ready':'blocked',reason:runTask?undefined:blockedScenarios[0]?.reason,blockedScenarios,
        extra:{environment,caseFields:{executionIncludesBaseOffline:true}}}));
    }
  }
  if(variant!=='normal')for(const productEntry of products.filter(entry=>entry.variant==='multiplayer'))
    addReplayCases({cases,productEntry,root,output,environment,groups:selectedGroups,selectedChecks});
  for(const productEntry of products.filter(entry=>entry.variant==='normal'))
    addReplayCases({cases,productEntry,root,output,environment,groups:selectedGroups,selectedChecks});
  if(selectedGroups.has('offline-update')&&!selectedGroups.has('offline'))
    addOfflineUpdateOnlyCases({cases,products:products.filter(entry=>entry.variant==='normal'),root,output,environment,updateState,selectedChecks});
  const selectedBase=[...new Set(products.map(entry=>entry.game))].map(game=>({game,product:PRODUCT_GAMES[game]}));
  if(variant!=='normal')addRoomCases({cases,products:selectedBase,root,output,environment,groups:selectedGroups,selectedChecks});
  addDiagnosticCases({cases,products,root,output,environment,groups:selectedGroups,selectedChecks,diagnosticRegistry});
  addLanguageCases({cases,products,root,output,environment,groups:selectedGroups,selectedChecks});
  addBuildCases({cases,products,root,output,environment,groups:selectedGroups,selectedChecks});
  addSourceJourneyCases({cases,products,root,output,environment,groups:selectedGroups,selectedChecks});
  addInstallationAuthorityCases({cases,products,root,output,environment,groups:selectedGroups,selectedChecks,updateState});
  addPairedPresentationCases({cases,products,root,output,environment,groups:selectedGroups,selectedChecks,diagnosticRegistry});
  for(const item of cases)if(item.status==='ready'&&!['audio-latency','diagnostic','language','music','source-review','single-game'].includes(item.group)&&!TITLE_FLOW_GAMES.includes(item.game)){
    item.status='blocked';item.task=null;item.reason=`Register ${item.game} native observations and scenario flow before executing this driver`;
  }
  const mappedChecks=new Set(cases.flatMap(item=>item.checks));
  const unmappedChecks=checks.filter(check=>!mappedChecks.has(check)).map(check=>({id:check,reason:'No selected scenario group maps this adaptation check'}));
  const scope=checks.length?'checks':groups.length?'scenes':'product';
  const selectedRequirementIds=checks.length?new Set(checks):scope==='product'?knownChecks:
    new Set([...cases.flatMap(item=>item.checks),
      ...[...selectedGroups].flatMap(group=>products.flatMap(({game})=>checksForGroup(group,game,requirements)))]);
  const coverageTargets=products.map(({game,variant:mode,product})=>({game,variant:mode,
    requirements:requirements.filter(check=>selectedRequirementIds.has(check.id)).map(check=>({id:check.id,
      reason:mode==='multiplayer'&&!product.multiplayerRuntime?'Product directory declares no multiplayer Runtime':
        notApplicable(check,game,mode)}))}));
  return{cases,unmappedChecks,scope,coverageTargets};
}
