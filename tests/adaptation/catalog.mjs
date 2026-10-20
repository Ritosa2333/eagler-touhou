import {existsSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {PRODUCT_GAMES,PRODUCT_IDS} from '../../common/contracts/product-catalog.mjs';
import {MUSIC_BROWSER_GAMES} from '../../tools/ports/music-browser-helpers.mjs';

const DEFAULT_ROOT=resolve(import.meta.dirname,'../..');
export const ADAPTATION_GROUPS=Object.freeze([
  'runtime','desktop-input','desktop-background','practice','replay','music','offline','offline-update','rooms','spectator',
]);
const UPDATE_GAMES=Object.freeze([...MUSIC_BROWSER_GAMES]);
const UPDATE_CHECKS=Object.freeze(['PACKAGE-03','SAVE-06']);
const OFFLINE_CHECKS=Object.freeze(['PACKAGE-02','PACKAGE-04','PACKAGE-05','SAVE-02','SAVE-03','SAVE-04','SAVE-05']);
const GROUP_CHECKS=Object.freeze({
  runtime:Object.freeze(['RUNTIME-01','RUNTIME-02','RUNTIME-03','RUNTIME-05','RUNTIME-06']),
  'desktop-input':Object.freeze(['INPUT-01','INPUT-02','INPUT-03','INPUT-04','INPUT-05','INPUT-07',...Array.from({length:19},(_,index)=>`TOUCH-${String(index+1).padStart(2,'0')}`)]),
  'desktop-background':Object.freeze(['RUNTIME-04','INPUT-06','TOUCH-16','AUDIO-04']),
  practice:Object.freeze(['PRACTICE-01','PRACTICE-02','PRACTICE-03','PRACTICE-07']),
  replay:Object.freeze(['REPLAY-01']),
  music:Object.freeze(['AUDIO-01']),
  offline:OFFLINE_CHECKS,
  'offline-update':UPDATE_CHECKS,
  rooms:Object.freeze(['MP-01','MP-02','MP-03','MP-04','MP-05','MP-10','MP-12','MP-15']),
  spectator:Object.freeze(['MP-06','MP-07','MP-08','MP-09']),
});
const INPUT={runtime:'EAGLER_TEST_RUNTIME_MANIFEST',packages:'EAGLER_TEST_PACKAGES',artwork:'EAGLER_TEST_ARTWORK',
  music:'EAGLER_TEST_MUSIC_MANIFEST',launcherUrl:'EAGLER_TEST_LAUNCHER_URL',update:'EAGLER_TEST_UPDATE_PACKAGES'};

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
    checks:[...checks],status,task,requiredInputs:[...requiredInputs],inputs:inputNames,blockedInputs,
    blockedScenarios:blockedScenarios.map(scenario=>({...scenario,checks:[...scenario.checks]})),blockedChecks,
    reportPath,...(reason?{reason}:{}),...extra.caseFields,
  };
}

function argFor(environment,name,option=name){
  const value=environment[name];return `--${option}=${typeof value==='string'&&value.trim()?value:`<${name}>`}`;
}

function addNormalCases({cases,game,product,groups,selectedChecks,root,output,environment}){
  const baseGroups=['runtime','desktop-input','desktop-background','practice','music'];
  for(const group of baseGroups){
    if(!groups.has(group))continue;
    if(group==='music'&&!MUSIC_BROWSER_GAMES.includes(game))continue;
    const supports=group==='practice'?product.features?.thprac===true:true;
    const scenarioId={runtime:'native-runtime-contract', 'desktop-input':'desktop-input-effects',
      'desktop-background':'desktop-background-and-resume',practice:'native-practice-menu-and-locale',music:'title-ogg-pcm'}[group];
    const mapped=GROUP_CHECKS[group].filter(check=>selectedChecks===null||selectedChecks.has(check));
    if(selectedChecks!==null&&!mapped.length)continue;
    const common={root,output,game,variant:'normal',group,scenarioIds:[scenarioId],checks:mapped,extra:{environment}};
    if(!supports){cases.push(makeCase({...common,id:`${group}-normal-${game}`,status:'not-applicable',reason:'Product directory declares no practice feature'}));continue;}
    const script={runtime:'tools/ports/runtime-browser.mjs','desktop-input':'tools/ports/runtime-desktop-probes.mjs',
      'desktop-background':'tools/ports/runtime-desktop-probes.mjs',practice:'tools/ports/practice-browser.mjs',music:'tools/ports/music-browser.mjs'}[group];
    const args=[argFor(environment,INPUT.runtime,'manifest'),`--game=${game}`,'--variant=normal'];
    if(group==='runtime')args.push('--gamepad');
    if(group==='desktop-input')args.push('--input-only');
    if(group==='desktop-background')args.push('--background-only');
    if(group==='music')args.push(argFor(environment,INPUT.music,'music-manifest'));
    const requiredInputs=group==='music'?[INPUT.runtime,INPUT.music]:[INPUT.runtime];
    const resources=['browser','runtime'];
    cases.push(makeCase({...common,id:`${group}-normal-${game}`,script,args,requiredInputs,timeoutMs:1800000,resources}));
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
    if(!UPDATE_GAMES.includes(game))continue;
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
      requiredInputs:blocked?[]:[INPUT.runtime,INPUT.packages,INPUT.artwork,INPUT.update],
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
  games,variant='both',checks=[],groups=[]}={}){
  root=resolve(root);output=resolve(output);variant=normalizeVariant(variant);
  if(!environment||typeof environment!=='object'||Array.isArray(environment))throw new Error('environment must be an object');
  if(!Array.isArray(checks)||checks.some(check=>typeof check!=='string'||!check.trim()))throw new Error('checks must be an array of adaptation IDs');
  if(new Set(checks).size!==checks.length)throw new Error('checks must not contain duplicates');
  const knownChecks=new Set(JSON.parse(readFileSync(resolve(root,'docs/adaptation/checklist.json'),'utf8')).checks.map(item=>item.id));
  for(const check of checks)if(!knownChecks.has(check))throw new Error(`Unknown adaptation check ${check}`);
  if(!Array.isArray(groups)||groups.some(group=>typeof group!=='string'||!ADAPTATION_GROUPS.includes(group)))throw new Error('groups contains an unknown adaptation group');
  const selectedGroups=new Set(groups.length?groups:ADAPTATION_GROUPS),selectedChecks=checks.length?new Set(checks):null;
  const products=selectedProducts(games,variant),cases=[],updateState=updateManifestState(environment,root);
  const updateSelected=selectedGroups.has('offline-update')||selectedGroups.has('offline')&&selectedChecks===null||
    !!selectedChecks&&UPDATE_CHECKS.some(check=>selectedChecks.has(check));
  const normalGames=[...new Set(products.filter(entry=>entry.variant==='normal').map(entry=>entry.game))];
  for(const game of normalGames)addNormalCases({cases,game,product:PRODUCT_GAMES[game],groups:selectedGroups,selectedChecks,root,output,environment});
  if(selectedGroups.has('offline')){
    for(const game of normalGames){
      const baseChecks=OFFLINE_CHECKS.filter(check=>selectedChecks===null||selectedChecks.has(check));
      const updateChecks=updateSelected&&UPDATE_GAMES.includes(game)?UPDATE_CHECKS.filter(check=>selectedChecks===null||selectedChecks.has(check)):[];
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
  const mappedChecks=new Set(cases.flatMap(item=>item.checks));
  const unmappedChecks=checks.filter(check=>!mappedChecks.has(check)).map(check=>({id:check,reason:'No selected scenario group maps this adaptation check'}));
  return{cases,unmappedChecks};
}
