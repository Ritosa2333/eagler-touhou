import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';

/** Only checks with structured observations emitted by the corresponding lane. */
export const DIAGNOSTIC_LANE_CHECKS=Object.freeze({
  presentation:Object.freeze(['DISPLAY-03']),
  quick:Object.freeze(['VERIFY-01','VERIFY-03','VERIFY-04','VERIFY-06','VERIFY-08']),
  daily:Object.freeze(['VERIFY-02','VERIFY-03','VERIFY-04','VERIFY-06','VERIFY-08']),
});

export const PAIRED_PRESENTATION_CHECKS=Object.freeze([
  'DISPLAY-01','DISPLAY-02','DISPLAY-03','DISPLAY-04','DISPLAY-05','DISPLAY-06','DISPLAY-07','DISPLAY-08',
  'REPLAY-07','AUDIO-06','VERIFY-07',
]);
export const PAIRED_CADENCE_DIFFERENCE_GROUPS=Object.freeze(['host-frame-cadence','synchronous-audio-operations']);

const LANE_NAMES=Object.freeze(Object.keys(DIAGNOSTIC_LANE_CHECKS));
const pathPattern=/^(?!\/)(?!.*(?:^|\/)\.\.?($|\/))[A-Za-z0-9._/-]+$/u;
const fileNamePattern=/^[A-Za-z0-9][A-Za-z0-9._-]*\.[A-Za-z0-9]+$/u;
const checkIdPattern=/^[A-Z]+(?:-[A-Z]+)*-\d+$/u;

function requireString(value,label){
  if(typeof value!=='string'||!value.trim())throw new Error(`Diagnostic profile requires ${label}`);
}

function requirePath(value,label){
  requireString(value,label);
  if(!pathPattern.test(value)||value.includes('\\'))throw new Error(`Diagnostic profile ${label} must be a safe repository-relative path`);
}

function requireCheckIds(value,label){
  if(!Array.isArray(value)||value.some(id=>typeof id!=='string'||!checkIdPattern.test(id))||new Set(value).size!==value.length)
    throw new Error(`Diagnostic profile ${label} must contain unique adaptation IDs`);
}

function requireUnique(values,label){
  if(new Set(values).size!==values.length)throw new Error(`Diagnostic profile ${label} must be unique`);
}

function rejectUndeclaredLaneFields(profile,lane,keys){
  if(!profile.lanes.includes(lane)&&keys.some(key=>Object.hasOwn(profile,key)))
    throw new Error(`Diagnostic profile has ${lane} configuration without declaring that lane`);
}

function validateProfile(profile){
  if(!profile||typeof profile!=='object'||Array.isArray(profile))throw new Error('Diagnostic profile must be an object');
  if(profile.schema!=='eagler-touhou/diagnostic-adapter/1')throw new Error('Unsupported diagnostic profile schema');
  if(typeof profile.game!=='string'||!PRODUCT_GAMES[profile.game])throw new Error(`Unknown diagnostic product ${profile.game}`);
  if(!['normal','multiplayer'].includes(profile.variant))throw new Error(`Unsupported diagnostic variant ${profile.variant}`);
  if(profile.variant==='multiplayer'&&!PRODUCT_GAMES[profile.game].multiplayerRuntime)
    throw new Error(`Diagnostic product ${profile.game} has no multiplayer Runtime`);
  if(!Array.isArray(profile.lanes)||!profile.lanes.length||profile.lanes.some(lane=>!LANE_NAMES.includes(lane)))
    throw new Error('Diagnostic profile must explicitly declare supported lanes');
  requireUnique(profile.lanes,'lanes');

  for(const key of ['buildIdentitySchema','wasmFile','runtimeProfile'])requireString(profile[key],key);
  if(!fileNamePattern.test(profile.wasmFile))throw new Error('Diagnostic profile wasmFile must be a safe filename');
  for(const key of ['buildReceipt','sourceRoot','presentationDirectory','serverScript','runtimeDirectory'])requirePath(profile[key],key);
  for(const key of ['font','data','inputRoot','snapshot']){
    const value=profile.envNames?.[key];
    if(typeof value!=='string'||! /^[A-Z][A-Z0-9_]*$/u.test(value))throw new Error(`Diagnostic profile requires an environment name for ${key}`);
  }
  rejectUndeclaredLaneFields(profile,'presentation',['browserVerifier','presentation']);
  rejectUndeclaredLaneFields(profile,'quick',['quickCapture','quickCaptureSchema','quickGate','quickFixtureCount','quickInputs','quickPartialChecks']);
  rejectUndeclaredLaneFields(profile,'daily',['dailyCapture','dailyCaptureSchema','dailyGate','dailyGateReplayOptions','dailyFixtures','dailyPartialChecks']);
  if(!profile.lanes.includes('quick')&&!profile.lanes.includes('daily')&&Object.hasOwn(profile,'goldenManifest'))
    throw new Error('Diagnostic profile has a golden manifest without a capture lane');
  for(const lane of ['presentation','quick','daily'])if(!profile.lanes.includes(lane)&&Object.hasOwn(profile.laneScope??{},lane))
    throw new Error(`Diagnostic profile has ${lane} laneScope without declaring that lane`);
  for(const lane of ['quick','daily'])if(!profile.lanes.includes(lane)&&Object.hasOwn(profile.decisionInputs??{},lane))
    throw new Error(`Diagnostic profile has decisionInputs.${lane} without declaring that lane`);

  for(const lane of profile.lanes){
    if(!Array.isArray(DIAGNOSTIC_LANE_CHECKS[lane])||!DIAGNOSTIC_LANE_CHECKS[lane].length)
      throw new Error(`Diagnostic lane ${lane} has no mapped checks`);
    requireString(profile.laneScope?.[lane],`${lane} laneScope`);
  }
  if(profile.lanes.includes('presentation')){
    for(const key of ['browserVerifier'])requirePath(profile[key],key);
    const contract=profile.presentation;
    if(!contract||typeof contract!=='object'||Array.isArray(contract))throw new Error('Presentation lane requires a presentation contract');
    requireString(contract.scope,'presentation.scope');
    requireString(contract.checkId,'presentation.checkId');
    if(!DIAGNOSTIC_LANE_CHECKS.presentation.includes(contract.checkId))throw new Error('Presentation checkId is not mapped to the presentation lane');
    for(const key of ['requiredNormalLabels','negativeLabels']){
      const value=contract[key];
      if(!Array.isArray(value)||!value.length||value.some(item=>typeof item!=='string'||!item.trim()))
        throw new Error(`Presentation contract requires ${key}`);
      requireUnique(value,`presentation.${key}`);
    }
    if(!Number.isSafeInteger(contract.playerOwnerId)||contract.playerOwnerId<0)throw new Error('Presentation playerOwnerId must be a nonnegative integer');
    for(const key of ['positionFieldId','audioMutationGroup','movementLabel','negativeMovementLabel','recoveredMovementLabel','pauseLabel'])
      requireString(contract[key],`presentation.${key}`);
    requireCheckIds(contract.partialChecks,'presentation.partialChecks');
    if(contract.partialChecks.includes(contract.checkId))throw new Error('Presentation checkId cannot also be declared partial');
    if(contract.pairedReplay!==undefined){
      const paired=contract.pairedReplay;
      if(!paired||typeof paired!=='object'||Array.isArray(paired)||
         paired.schema!=='eagler-presentation/replay-pair-capability/3')
        throw new Error('Presentation pairedReplay must declare its versioned capability');
      if(typeof paired.fixture!=='string'||!/^demo[0-3]$/u.test(paired.fixture)||
         typeof paired.inputFile!=='string'||paired.inputFile!==`${paired.fixture}.rpy`||
         !Number.isSafeInteger(paired.ticks)||paired.ticks<1||paired.ticks>3600)
        throw new Error('Presentation pairedReplay requires one named input and a bounded tick count');
      requireCheckIds(paired.checks,'presentation.pairedReplay.checks');
      if(paired.checks.length!==PAIRED_PRESENTATION_CHECKS.length||
         PAIRED_PRESENTATION_CHECKS.some(id=>!paired.checks.includes(id)))
        throw new Error('Presentation pairedReplay must map every paired observation check exactly once');
      requireUnique(paired.checks,'presentation.pairedReplay.checks');
      if(paired.boundary!==undefined){
        requireString(paired.boundary.reportSchema,'presentation.pairedReplay.boundary.reportSchema');
        for(const key of ['titleScene','gameScene','sceneSlot','loadingSlot','statusSceneSlot','errorSlot'])
          if(!Number.isSafeInteger(paired.boundary[key])||paired.boundary[key]<0||paired.boundary[key]>255)
            throw new Error(`Presentation boundary requires a bounded ${key}`);
        if(paired.boundary.titleScene===paired.boundary.gameScene)throw new Error('Presentation boundary scenes must differ');
      }
      if(!Array.isArray(paired.cadencesHz)||paired.cadencesHz.join(',')!=='60,120,144')
        throw new Error('Presentation pairedReplay must declare 60, 120 and 144 Hz native-loop cadences');
      if(!Number.isSafeInteger(paired.expectedAuthorityGroups)||paired.expectedAuthorityGroups<1)
        throw new Error('Presentation pairedReplay must declare its expected authority-group count');
      if(!Array.isArray(paired.cadenceDifferenceGroups)||
         paired.cadenceDifferenceGroups.length!==PAIRED_CADENCE_DIFFERENCE_GROUPS.length||
         PAIRED_CADENCE_DIFFERENCE_GROUPS.some(id=>!paired.cadenceDifferenceGroups.includes(id)))
        throw new Error('Presentation pairedReplay must declare only the two measured non-authority cadence groups');
      requireUnique(paired.cadenceDifferenceGroups,'presentation.pairedReplay.cadenceDifferenceGroups');
    }
  }
  if(profile.lanes.includes('quick')){
    for(const key of ['quickCapture','quickGate','goldenManifest'])requirePath(profile[key],key);
    requireString(profile.quickCaptureSchema,'quickCaptureSchema');
    if(!Number.isSafeInteger(profile.quickFixtureCount)||profile.quickFixtureCount<1||
      !Array.isArray(profile.quickInputs)||profile.quickInputs.length!==profile.quickFixtureCount)
      throw new Error('Quick lane requires a positive fixture count matching quickInputs');
    requireUnique(profile.quickInputs.map(item=>item?.id),'quick fixture ids');
    requireUnique(profile.quickInputs.map(item=>item?.index),'quick fixture indices');
    requireUnique(profile.quickInputs.map(item=>item?.captureFile),'quick capture files');
    if(!profile.quickInputs.every((item,index)=>typeof item?.id==='string'&&item.id.trim()&&item.index===index&&
      typeof item.file==='string'&&fileNamePattern.test(item.file)&&typeof item.captureFile==='string'&&fileNamePattern.test(item.captureFile)))
      throw new Error('Quick fixtures require ordered IDs, safe input files and safe capture files');
    requireCheckIds(profile.quickPartialChecks,'quickPartialChecks');
    const quickPrimary=DIAGNOSTIC_LANE_CHECKS.quick[0];
    const expectedQuickPartial=DIAGNOSTIC_LANE_CHECKS.quick.slice(1).filter(id=>id!=='VERIFY-04');
    if(profile.quickPartialChecks.includes(quickPrimary)||profile.quickPartialChecks.includes('VERIFY-04')||expectedQuickPartial.some(id=>!profile.quickPartialChecks.includes(id)))
      throw new Error('Quick lane must preserve comparison support as partial and assess reference integrity independently');
  }
  if(profile.lanes.includes('daily')){
    for(const key of ['dailyCapture','dailyGate','goldenManifest'])requirePath(profile[key],key);
    requireString(profile.dailyCaptureSchema,'dailyCaptureSchema');
    if(!Array.isArray(profile.dailyFixtures)||!profile.dailyFixtures.length)throw new Error('Daily lane requires dailyFixtures');
    if(!profile.dailyFixtures.every(item=>typeof item?.id==='string'&&item.id.trim()&&typeof item.replayKey==='string'&&item.replayKey.trim()&&
      Number.isSafeInteger(item.stage)&&item.stage>=0&&typeof item.captureFile==='string'&&fileNamePattern.test(item.captureFile)))
      throw new Error('Daily fixtures require IDs, Replay keys, stages and safe capture files');
    for(const key of ['id','replayKey','captureFile'])requireUnique(profile.dailyFixtures.map(item=>item[key]),`daily fixture ${key}s`);
    if(!Array.isArray(profile.dailyGateReplayOptions)||!profile.dailyGateReplayOptions.length)
      throw new Error('Daily lane requires dailyGateReplayOptions');
    const replayKeys=new Set(profile.dailyFixtures.map(item=>item.replayKey));
    if(profile.dailyGateReplayOptions.some(item=>typeof item?.option!=='string'||!/^--[a-z][a-z0-9-]*$/u.test(item.option)||
      typeof item.replayKey!=='string'||!replayKeys.has(item.replayKey)))throw new Error('Daily Replay options must map registered fixtures');
    requireUnique(profile.dailyGateReplayOptions.map(item=>item.replayKey),'daily Replay option keys');
    if(profile.dailyGateReplayOptions.length!==replayKeys.size)throw new Error('Daily Replay options must select every daily fixture exactly once');
    requireCheckIds(profile.dailyPartialChecks,'dailyPartialChecks');
    const dailyPrimary=DIAGNOSTIC_LANE_CHECKS.daily[0];
    const expectedDailyPartial=DIAGNOSTIC_LANE_CHECKS.daily.slice(1).filter(id=>id!=='VERIFY-04');
    if(profile.dailyPartialChecks.includes(dailyPrimary)||profile.dailyPartialChecks.includes('VERIFY-04')||expectedDailyPartial.some(id=>!profile.dailyPartialChecks.includes(id)))
      throw new Error('Daily lane must preserve comparison support as partial and assess reference integrity independently');
  }
  for(const lane of ['quick','daily'])if(profile.lanes.includes(lane)){
    const inputs=profile.decisionInputs?.[lane];
    if(!Array.isArray(inputs))throw new Error(`Diagnostic profile requires decisionInputs.${lane}`);
    for(const [index,path] of inputs.entries())requirePath(path,`decisionInputs.${lane}[${index}]`);
  }
  if(profile.replayReview!==undefined){
    requirePath(profile.replayReview.module,'replayReview.module');
    requireString(profile.replayReview.export,'replayReview.export');
    if(!/^[A-Za-z_$][A-Za-z0-9_$]*$/.test(profile.replayReview.export))throw new Error('Invalid Replay review export');
    if(!['quick','daily'].every(lane=>profile.lanes.includes(lane)))throw new Error('Replay review requires Quick and Daily lanes');
  }
}

function deepFreeze(value){
  if(value&&typeof value==='object'&&!Object.isFrozen(value)){
    for(const child of Object.values(value))deepFreeze(child);
    Object.freeze(value);
  }
  return value;
}

/** Build the title-owned profile index used by the shared diagnostic lanes. */
export function createDiagnosticsRegistry(profiles){
  if(!Array.isArray(profiles))throw new Error('Diagnostic profiles must be an array');
  for(const lane of LANE_NAMES){
    const ids=DIAGNOSTIC_LANE_CHECKS[lane];
    requireCheckIds(ids,`laneChecks.${lane}`);
    requireUnique(ids,`laneChecks.${lane}`);
  }
  const index=new Map(),snapshots=[];
  for(const source of profiles){
    validateProfile(source);
    const key=`${source.game}/${source.variant}`;
    if(index.has(key))throw new Error(`Duplicate diagnostic profile ${key}`);
    const profile=deepFreeze(JSON.parse(JSON.stringify(source)));
    index.set(key,profile);snapshots.push(profile);
  }
  const checks=DIAGNOSTIC_LANE_CHECKS;
  return Object.freeze({profiles:Object.freeze(snapshots),laneChecks:checks,
    resolve:(game,variant)=>index.get(`${game}/${variant}`)??null});
}

/** Split selected lane requests so each lane is either runnable or visibly blocked. */
export function partitionDiagnosticLanes(profile,lanes){
  const supported=new Set(profile?.lanes??[]);
  return{ready:lanes.filter(lane=>supported.has(lane)),blocked:lanes.filter(lane=>!supported.has(lane))};
}

const TH08_NORMAL_PROFILE={
  schema:'eagler-touhou/diagnostic-adapter/1',game:'th08',variant:'normal',lanes:['presentation','quick','daily'],
  buildIdentitySchema:'th08/presentation-lab-build/1',wasmFile:'th08-sdl.wasm',runtimeProfile:'presentation-lab',
  buildReceipt:'ports/th08/web/artifacts/presentation-lab/build.json',
  sourceRoot:'ports/th08',presentationDirectory:'ports/th08/portable/presentation-lab',
  serverScript:'ports/th08/portable/presentation-lab/serve.mjs',
  browserVerifier:'ports/th08/portable/presentation-lab/verify-browser.py',
  quickCapture:'ports/th08/tools/replay-verifier/capture-current-demos.py',
  quickCaptureSchema:'th08/current-replay-capture/v3',
  dailyCapture:'ports/th08/tools/replay-verifier/capture-current-replay.py',
  dailyCaptureSchema:'th08/current-replay-tick-capture/v3',
  quickGate:'ports/th08/tools/replay-verifier/run-current-candidate-gate.mjs',
  dailyGate:'ports/th08/tools/replay-verifier/run-daily-gate.mjs',
  goldenManifest:'ports/th08/tools/replay-verifier/golden/manifest.json',
  replayReview:{module:'tests/adaptation/adapters/titles/th08-replay.mjs',export:'createReplayReviewSuite'},
  decisionInputs:{
    quick:['ports/th08/tools/replay-verifier/corpus.json','common/testkit/replay-verifier/suite.mjs','common/testkit/replay-verifier/adapter.mjs'],
    daily:['ports/th08/tools/replay-verifier/corpus.json','common/testkit/replay-verifier/suite.mjs','common/testkit/replay-verifier/adapter.mjs'],
  },
  runtimeDirectory:'ports/th08/artifacts/presentation-lab/runtime',
  envNames:{font:'TH08_LAB_FONT',data:'TH08_LAB_DATA',inputRoot:'TH08_LAB_INPUT_ROOT',snapshot:'TH08_LAB_SNAPSHOT'},
  quickFixtureCount:4,
  quickInputs:[
    {id:'demo0',index:0,file:'demo0.rpy',captureFile:'demo0.json'},
    {id:'demo1',index:1,file:'demo1.rpy',captureFile:'demo1.json'},
    {id:'demo2',index:2,file:'demo2.rpy',captureFile:'demo2.json'},
    {id:'demo3',index:3,file:'demo3.rpy',captureFile:'demo3.json'},
  ],
  dailyGateReplayOptions:[
    {option:'--replay-lunatic',replayKey:'lunatic'},
    {option:'--replay-extra',replayKey:'extra'},
  ],
  dailyFixtures:[
    {id:'lunatic',replayKey:'lunatic',stage:0,captureFile:'lunatic.json'},
    {id:'extra',replayKey:'extra',stage:8,captureFile:'extra.json'},
  ],
  presentation:{
    scope:'DISPLAY-03 machine-side presentation purity',
    checkId:'DISPLAY-03',
    requiredNormalLabels:['title-idle','title-animate','menu-motion','negative-recovered',
      'stage1-motion','stage1-recovered','pause-menu'],
    negativeLabels:['negative-control','stage1-negative'],
    playerOwnerId:13,positionFieldId:'position',audioMutationGroup:'synchronous-audio-operations',
    movementLabel:'stage1-motion',negativeMovementLabel:'stage1-negative',recoveredMovementLabel:'stage1-recovered',
    pauseLabel:'pause-menu',
    partialChecks:['DISPLAY-01','DISPLAY-02','DISPLAY-04','DISPLAY-05','DISPLAY-06','DISPLAY-07','DISPLAY-08',
      'REPLAY-07','AUDIO-06','VERIFY-07'],
    pairedReplay:{schema:'eagler-presentation/replay-pair-capability/3',fixture:'demo0',inputFile:'demo0.rpy',ticks:600,
      expectedAuthorityGroups:17,
      boundary:{reportSchema:'th08/presentation-audit/1',titleScene:1,gameScene:2,
        sceneSlot:0,loadingSlot:3,statusSceneSlot:0,errorSlot:2},
      cadencesHz:[60,120,144],cadenceDifferenceGroups:[...PAIRED_CADENCE_DIFFERENCE_GROUPS],checks:[...PAIRED_PRESENTATION_CHECKS]},
  },
  laneScope:{
    presentation:'DISPLAY-03 draw purity plus a same-window Replay comparison on the production native frame loop at 60, 120 and 144 Hz',
    quick:'VERIFY-01 across the registered immutable demo captures',
    daily:'VERIFY-02 across the registered immutable daily captures',
    otherVerification:'VERIFY-04 independently checks immutable reference inputs; VERIFY-03, VERIFY-06 and VERIFY-08 retain their own completion boundaries',
  },
  quickPartialChecks:['VERIFY-03','VERIFY-06','VERIFY-08'],
  dailyPartialChecks:['VERIFY-03','VERIFY-06','VERIFY-08'],
};

export const DIAGNOSTIC_REGISTRY=createDiagnosticsRegistry([TH08_NORMAL_PROFILE]);
export const DIAGNOSTIC_PROFILES=DIAGNOSTIC_REGISTRY.profiles;
export function resolveDiagnosticProfile(game,variant){return DIAGNOSTIC_REGISTRY.resolve(game,variant);}
export function pairedPresentationCapability(profile){
  const paired=profile?.presentation?.pairedReplay;
  return paired?{status:'ready',capability:paired}:
    {status:'blocked',reason:`${profile?.game??'Selected title'}/${profile?.variant??'selected variant'} has no paired Replay presentation adapter`};
}
