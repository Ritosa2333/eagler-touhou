import {createHash} from 'node:crypto';
import {lstat,readFile,readdir,realpath,stat} from 'node:fs/promises';
import {resolve} from 'node:path';

const gameEnvironment={
  th09:[
    'TH09_BATTLE_DEEP_INTERVAL','TH09_BATTLE_PAIR','TH09_COMPARE_LASERS','TH09_DEMO_CASE',
    'TH09_GAMEPLAY_ONLY','TH09_ORACLE_CTOR_FPCW','TH09_ORACLE_FPCW','TH09_ORACLE_ONLY',
    'TH09_REPLAY_FILE','TH09_REPLAY_LIMIT','TH09_REPLAY_STAGE','TH09_REPLAY_VERIFIER_LIGHT',
    'TH09_TRACE','TH09_WORLD_CASE',
  ],
  th11:[
    'TH11_ANM_FILE','TH11_ANM_FRAMES','TH11_ANM_SCRIPT','TH11_BOMB_SELECTIONS','TH11_CAMPAIGN_SOAK',
    'TH11_CORE_TEST_FILE','TH11_CULL_DEBUG','TH11_DIALOGUE_SHOT','TH11_DIALOGUE_STAGE',
    'TH11_EQUIVALENCE_CORE','TH11_EXTERNAL_REPLAY','TH11_NATIVE_PREFIX','TH11_ORACLE_COUNT_INSTRUCTIONS',
    'TH11_ORACLE_TRACE','TH11_SCENE_SCHEDULE_REFERENCE','TH11_TEST_FONT_DIRECTORY',
    'TH11_PLAYER_FILE','TH11_PLAYER_RATE','TH11_PREFIX_CORE','TH11_SHOT_FILE','TH11_SHOT_GROUP',
    'TH11_STAGE_NUMBERS','TH11_VERIFIER_CAPTURE','TH11_VERIFIER_CASE','TH11_WORLD_CAMPAIGN',
    'TH11_WORLD_CHECKPOINT','TH11_WORLD_CHECKPOINT_FRAME','TH11_WORLD_DIAGNOSE','TH11_WORLD_DRAW_CLOCKS',
    'TH11_WORLD_FILE','TH11_WORLD_LIMIT','TH11_WORLD_NATIVE_RESUME','TH11_WORLD_REPLAYS',
    'TH11_WORLD_RESUME','TH11_WORLD_STAGE','TH11_WORLD_TRACE',
  ],
  th15:[
    'TH15_EMSDK','TH15_GATHER_SCENARIOS','TH15_LANGUAGE_PACKS','TH15_RUN_FLOW_CHARACTERS',
    'TH15_RUN_FLOW_DIFFICULTIES','TH15_RUN_FLOW_STAGE_LIMIT','TH15_RUN_FLOW_START_STAGE',
    'TH15_STAGE_GAMEPLAY_CHARACTER','TH15_STAGE_GAMEPLAY_FRAMES','TH15_STAGE_GAMEPLAY_STAGE',
    'TH15_ORACLE_FONT_DEFINITIONS','TH15_ORACLE_FONT_ASSETS',
  ],
};

const runnerEnvironment=['EAGLER_COMMON_ROOT','EAGLER_MAINTENANCE_OUTPUT','EAGLER_ORACLE_ROOT','NODE_OPTIONS','NODE_TEST_CONTEXT','TH11_CORE_TEST_FILE'];
const controlsByGame={
  th09:[
    '../../../tools/testing/oracle-inputs.mjs','../../../tools/testing/oracle-suite.mjs',
    '../../../tools/testing/native-inputs.mjs','../../../tools/testing/frozen-oracle.mjs',
    '../../../tools/testing/oracle-cli.mjs','../../../tools/testing/runner.mjs',
    '../../../tools/testing/supervise.py','../../../tools/testing/output-lock.mjs',
    '../../../tools/testing/byte-evidence.mjs',
  ],
  th11:[
    '../../../tools/testing/oracle-inputs.mjs','../../../tools/testing/oracle-suite.mjs',
    '../../../tools/testing/native-inputs.mjs','../../../tools/testing/frozen-oracle.mjs',
    '../../../tools/testing/oracle-cli.mjs','../../../tools/testing/runner.mjs',
    '../../../tools/testing/supervise.py','../../../tools/testing/output-lock.mjs',
    '../../../tools/testing/byte-evidence.mjs',
  ],
  th15:[
    '../../../tools/testing/oracle-inputs.mjs','../../../tools/testing/oracle-suite.mjs',
    '../../../tools/testing/native-inputs.mjs','../../../tools/testing/frozen-oracle.mjs',
    '../../../tools/testing/oracle-cli.mjs','../../../tools/testing/runner.mjs',
    '../../../tools/testing/supervise.py','../../../tools/testing/output-lock.mjs',
    '../../../tools/testing/byte-evidence.mjs',
  ],
};

const defaultsByGame={
  th09:{TH09_BATTLE_DEEP_INTERVAL:'60',TH09_REPLAY_LIMIT:'4294967295',...Object.fromEntries(gameEnvironment.th09
    .filter(key=>!['TH09_BATTLE_DEEP_INTERVAL','TH09_REPLAY_LIMIT'].includes(key)).map(key=>[key,null]))},
  th11:{TH11_ORACLE_COUNT_INSTRUCTIONS:'1',TH11_WORLD_DRAW_CLOCKS:'1',...Object.fromEntries(gameEnvironment.th11
    .filter(key=>!['TH11_ORACLE_COUNT_INSTRUCTIONS','TH11_WORLD_DRAW_CLOCKS'].includes(key)).map(key=>[key,null]))},
  th15:{TH15_GATHER_SCENARIOS:'4',TH15_RUN_FLOW_CHARACTERS:'0',TH15_RUN_FLOW_DIFFICULTIES:'1',
    TH15_RUN_FLOW_STAGE_LIMIT:'90000',TH15_RUN_FLOW_START_STAGE:'1',TH15_STAGE_GAMEPLAY_CHARACTER:'0',
    TH15_STAGE_GAMEPLAY_FRAMES:'3600',...Object.fromEntries(gameEnvironment.th15
      .filter(key=>!['TH15_GATHER_SCENARIOS','TH15_RUN_FLOW_CHARACTERS','TH15_RUN_FLOW_DIFFICULTIES',
        'TH15_RUN_FLOW_STAGE_LIMIT','TH15_RUN_FLOW_START_STAGE','TH15_STAGE_GAMEPLAY_CHARACTER',
        'TH15_STAGE_GAMEPLAY_FRAMES'].includes(key)).map(key=>[key,null]))},
};

const groupOverrides={
  th09:{},
  th11:{'assets-dat-native-world-replays':{TH11_WORLD_REPLAYS:'all'},'campaign-soak-dat':{TH11_CAMPAIGN_SOAK:'all'}},
  th15:{},
};

const controlFilesByGame={
  th09:[...controlsByGame.th09,'../../../common/testkit/replay-verifier/compare.mjs',
    '../../../common/testkit/replay-verifier/adapter.mjs','../../../common/testkit/replay-verifier/contracts.mjs',
    '../../../common/testkit/replay-verifier/capture-file.mjs',
    '../tools/replay-verifier/adapter.mjs','../tools/replay-verifier/run-demo-gate.mjs',
    '../tools/replay-verifier/summarize-demo-gate.mjs','sdl-runtime/motion-replay.mjs'],
  th11:[...controlsByGame.th11,'../../../common/testkit/replay-verifier/capture-file.mjs',
    '../tools/replay-verifier/observation.mjs','../tools/replay-verifier/adapter.mjs',
    '../tools/replay-verifier/state-schema.json'],
  th15:[...controlsByGame.th15,'../../../tools/testing/display-grid-evidence.mjs','../../../common/testkit/replay-verifier/file-adapter.mjs',
    '../../../common/testkit/replay-verifier/file-suite.mjs','../../../common/testkit/replay-verifier/join-traces.mjs',
    '../../../common/testkit/replay-verifier/capture-file.mjs','../tools/replay-verifier/observation.mjs',
    '../tools/replay-verifier/adapter.mjs','../tools/replay-verifier/state-schema.json'],
};

const cppTestHeadersByGame={
  th09:[],
  th11:['cpp/sdl/ThcrapLayout.hpp'],
  th15:['cpp/sdl/PresentationCamera.hpp','cpp/sdl/PresentationUv.hpp'],
};

const commonDirectoryInputs=['cpp/game','tests/cpp','scripts/cpp','scripts/native','../../../common/include',
  '../../../common/testkit/replay-verifier','../tools/replay-verifier',
  'node_modules/@alexaltea/unicorn-js','node_modules/koffi'];
const koffiPlatforms={
  win32:new Set(['x64','ia32','arm64']),linux:new Set(['arm','arm64','ia32','loong64','riscv64','x64']),
  darwin:new Set(['arm64','x64']),freebsd:new Set(['arm64','ia32','x64']),
  openbsd:new Set(['ia32','x64']),android:new Set(['arm64','x64']),
};

function sorted(value) { return [...value].sort(); }
function compareText(a,b) { return a<b?-1:a>b?1:0; }
function digest(bytes) { return createHash('sha256').update(bytes).digest('hex'); }
/** Canonical checked-in policy. Null means the variable is removed from the child environment. */
export function nativeEnvironmentPolicy(game) {
  if (!gameEnvironment[game]) throw new Error(`Unsupported native input game: ${game}`);
  const clearInherited=[...new Set([...gameEnvironment[game],...runnerEnvironment])].sort();
  return {profile:'full-native/v1',clearInherited,defaults:{...defaultsByGame[game]}};
}

export function validateNativeEnvironmentPolicy(suite) {
  const expected=nativeEnvironmentPolicy(suite.game),actual=suite.nativeEnvironment;
  if (!actual||actual.profile!==expected.profile||JSON.stringify(sorted(actual.clearInherited??[]))!==
      JSON.stringify(sorted(expected.clearInherited))||JSON.stringify(Object.entries(actual.defaults??{}).sort())!==
      JSON.stringify(Object.entries(expected.defaults).sort()))
    throw new Error(`Native environment policy does not match the complete ${suite.game} profile`);
  const groupIds=new Set((suite.groups??[]).map(group=>group.id));
  for (const id of Object.keys(groupOverrides[suite.game])) if (!groupIds.has(id))
    throw new Error(`Native full-set group is missing from the suite: ${id}`);
  for (const group of suite.groups??[]) {
    const overrides=group.environment??{},allowed=groupOverrides[suite.game][group.id]??{};
    if (JSON.stringify(Object.entries(overrides).sort())!==JSON.stringify(Object.entries(allowed).sort()))
      throw new Error(`Native group environment narrows the full profile: ${group.id}`);
  }
  return expected;
}

export function nativeGroupEnvironment(suite,group,receiptOverrides={},baseEnvironment=process.env) {
  const policy=validateNativeEnvironmentPolicy(suite),environment={...baseEnvironment};
  for (const key of policy.clearInherited) delete environment[key];
  for (const [key,value] of Object.entries(policy.defaults)) if (value!==null) environment[key]=value;
  Object.assign(environment,group?.environment??{},receiptOverrides);
  const effective=Object.fromEntries(Object.keys(policy.defaults).sort().map(key=>[key,environment[key]??null]));
  for (const key of runnerEnvironment) effective[key]=environment[key]??null;
  return {environment,effective,unsetEnvironment:[...policy.clearInherited]};
}

/** Return the complete native-only input closure for a candidate run. */
export function nativeInputPlan(root,suite,environment=process.env) {
  const game=suite.game;
  if (!controlFilesByGame[game]) throw new Error(`Unsupported native input game: ${game}`);
  const directories=[...commonDirectoryInputs,...(game==='th15'?['../portable/sdl','node_modules/fflate']:[])];
  const files=[
    'target.json','package.json','package-lock.json','scripts/cpp/build.mjs',
    '../../../tools/testing/supervise.py','../../../tools/testing/output-lock.mjs',
    '../../../tools/testing/oracle-inputs.mjs','../../../tools/testing/oracle-suite.mjs',
    '../../../tools/testing/native-inputs.mjs','../../../tools/testing/frozen-oracle.mjs',
    '../../../tools/testing/oracle-cli.mjs','../../../tools/testing/runner.mjs',
    '../../../tools/testing/byte-evidence.mjs',
    'artifacts/cpp/game-core-test.wasm','artifacts/cpp/game-core-test-build.json',
    ...controlFilesByGame[game],...cppTestHeadersByGame[game],
  ];
  const uniqueFiles=[...new Set(files)].sort();
  const uniqueDirectories=[...new Set(directories)].sort();
  const receipt=environment.EAGLER_ORACLE_INPUT_RECEIPT;
  return {directories:uniqueDirectories.map(path=>({path,allowRootLink:path.startsWith('node_modules/')})),
    files:uniqueFiles.map(path=>({path,optional:path==='package-lock.json'})),
    receiptPath:receipt?resolve(root,receipt):null,
    koffiBackendPackage:koffiPlatforms[process.platform]?.has(process.arch)?`koffi-${process.platform}-${process.arch}`:null};
}

async function directoryEntries(directory,baseLabel,allowRootLink) {
  const linkInfo=await lstat(directory);
  let physical=directory;
  if (linkInfo.isSymbolicLink()) {
    if (!allowRootLink) throw new Error(`Unexpected native input directory link: ${baseLabel}`);
    physical=await realpath(directory);
  } else if (!linkInfo.isDirectory()) throw new Error(`Expected native input directory: ${baseLabel}`);
  const resolvedTarget=await realpath(directory);
  const physicalInfo=await stat(physical);
  if (!physicalInfo.isDirectory()) throw new Error(`Expected native input directory: ${baseLabel}`);
  const entries=[];
  async function visit(current,prefix='') {
    const children=await readdir(current,{withFileTypes:true});
    for (const child of children) {
      const rel=prefix?`${prefix}/${child.name}`:child.name,path=resolve(current,child.name),label=`${baseLabel}/${rel}`;
      if (child.isSymbolicLink()) throw new Error(`Unexpected nested link in native input: ${label}`);
      if (child.isDirectory()) {entries.push({path:label,kind:'directory',target:null});await visit(path,rel);}
      else if (child.isFile()) {
        const bytes=await readFile(path);entries.push({path:label,kind:'file',bytes:bytes.length,sha256:digest(bytes)});
      } else throw new Error(`Non-regular native input: ${label}`);
    }
  }
  await visit(physical);
  entries.push({path:baseLabel,kind:'directory',target:resolvedTarget});
  return entries;
}

/** Hash only native execution inputs; unrelated browser, launcher, and documentation edits are outside this closure. */
export async function nativeInputSnapshot(root,suite,environment=process.env) {
  root=resolve(root);
  const plan=nativeInputPlan(root,suite,environment),entries=[];
  if (!plan.koffiBackendPackage) throw new Error(`Unsupported Koffi native platform: ${process.platform}/${process.arch}`);
  for (const item of plan.directories) entries.push(...await directoryEntries(resolve(root,item.path),item.path,item.allowRootLink));
  const koffiRoot=await realpath(resolve(root,'node_modules/koffi')),
    koffiBackend=resolve(koffiRoot,'..','@koromix',plan.koffiBackendPackage);
  entries.push(...await directoryEntries(koffiBackend,`node_modules/koffi-platform/${plan.koffiBackendPackage}`,true));
  for (const item of plan.files) {
    const path=resolve(root,item.path);
    try {
      const info=await lstat(path);
      if (info.isSymbolicLink()||!info.isFile()) throw new Error(`Native input must be an ordinary file: ${item.path}`);
      const bytes=await readFile(path);entries.push({path:item.path,kind:'file',bytes:bytes.length,sha256:digest(bytes)});
    } catch (error) {
      if (error.code==='ENOENT'&&item.optional) entries.push({path:item.path,kind:'missing'});
      else throw error;
    }
  }
  if (plan.receiptPath) {
    try {
      const info=await lstat(plan.receiptPath);
      if (info.isSymbolicLink()||!info.isFile()) throw new Error('Private oracle receipt must be an ordinary file');
      const bytes=await readFile(plan.receiptPath);
      entries.push({path:`private-receipt:${plan.receiptPath}`,kind:'file',bytes:bytes.length,sha256:digest(bytes)});
    } catch (error) {
      if (error.code==='ENOENT') entries.push({path:`private-receipt:${plan.receiptPath}`,kind:'missing'});
      else throw error;
    }
  } else entries.push({path:'private-receipt:unset',kind:'missing'});
  entries.sort((a,b)=>compareText(a.path,b.path)||compareText(a.kind,b.kind));
  const uniqueEntries=[];
  for (const entry of entries) {
    const previous=uniqueEntries.at(-1);
    if (previous?.path===entry.path&&previous.kind===entry.kind) {
      if (JSON.stringify(previous)!==JSON.stringify(entry)) throw new Error(`Conflicting native input identity: ${entry.path}`);
      continue;
    }
    uniqueEntries.push(entry);
  }
  const hash=createHash('sha256');let bytes=0,files=0;
  for (const entry of uniqueEntries) {
    hash.update(entry.path).update('\0').update(entry.kind).update('\0');
    if (entry.kind==='file') {bytes+=entry.bytes;files++;hash.update(String(entry.bytes)).update('\0').update(entry.sha256);}
    else if (entry.kind==='directory') hash.update(entry.target??'');
    hash.update('\n');
  }
  const runtime={node:process.version,platform:process.platform,arch:process.arch};
  hash.update(JSON.stringify(runtime));
  return {schema:'eagler-touhou/native-input-snapshot/1',sha256:hash.digest('hex'),bytes,files,
    entries:uniqueEntries.length,runtime,details:uniqueEntries};
}

export function nativeInputSnapshotsEqual(before,after) {
  return !!before&&!!after&&before.schema===after.schema&&before.sha256===after.sha256&&
    before.bytes===after.bytes&&before.files===after.files&&before.entries===after.entries&&
    JSON.stringify(before.runtime)===JSON.stringify(after.runtime);
}
