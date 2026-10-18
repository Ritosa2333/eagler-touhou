import {execFileSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {runTasks} from './runner.mjs';
import {sourceSnapshot} from './source-snapshot.mjs';
import {oracleInputs} from './oracle-inputs.mjs';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const sorted=values=>[...values].sort();
const sameList=(actual,expected)=>Array.isArray(actual)&&
  JSON.stringify(sorted(actual))===JSON.stringify(sorted(expected));

function gitRoot(root) {
  return execFileSync('git',['rev-parse','--show-toplevel'],{cwd:root,encoding:'utf8',windowsHide:true}).trim();
}

async function buildInputs(root,manifest) {
  const target=JSON.parse(await readFile(resolve(root,'target.json'),'utf8'));
  if (!['th09','th11','th15'].includes(target.game)) throw new Error(`Unsupported oracle target: ${target.game}`);
  if (manifest.kind!==`${target.game}-original-comparison-fixture`||manifest.completeGame!==false)
    throw new Error('Core/build target identity mismatch');
  const expected=oracleInputs(root,{portableSdlHeaders:target.game==='th15'});
  for (const field of ['sources','headers','controlFiles']) {
    if (!sameList(manifest[field],expected[field])) throw new Error(`Native build inventory mismatch: ${field}`);
  }
  const expectedKeys=sorted([...expected.sources,...expected.headers,...expected.controlFiles]);
  if (!manifest.sourceHashes||typeof manifest.sourceHashes!=='object'||Array.isArray(manifest.sourceHashes)||
      JSON.stringify(sorted(Object.keys(manifest.sourceHashes)))!==JSON.stringify(expectedKeys))
    throw new Error('Build manifest source hash key set mismatch');
  const changed=[];
  for (const path of expectedKeys) {
    if (hash(await readFile(resolve(root,path)))!==manifest.sourceHashes[path]) changed.push(path);
  }
  return {expected,changed};
}

/** Execute the frozen native comparison suite against its real compiled core. */
export async function runFrozenOracle({root,name='full-suite',concurrency=2,timeoutMs=7200000}) {
  if (!/^[a-z0-9-]+$/.test(name)) throw new Error('Invalid evidence name');
  if (!Number.isSafeInteger(concurrency)||concurrency<1||concurrency>4) throw new Error('Test concurrency must be 1..4');
  const repository=gitRoot(root),directory=resolve(root,'artifacts/cpp/verification'),core=resolve(root,'artifacts/cpp/game-core-test.wasm');
  const manifest=JSON.parse(await readFile(resolve(root,'artifacts/cpp/game-core-test-build.json'),'utf8'));
  const coreSha256=hash(await readFile(core));
  if (coreSha256!==manifest.sha256) throw new Error('Core/build manifest mismatch');
  const initialInputs=await buildInputs(root,manifest);
  if (initialInputs.changed.length) throw new Error(`Sources changed after build: ${initialInputs.changed.join(', ')}`);
  const entries=async()=> (await readdir(resolve(root,'tests/cpp'))).filter(file=>file.endsWith('.test.mjs')).sort();
  const tests=await entries();
  if (!tests.length) throw new Error('No oracle tests selected');
  const testHashes={};
  for (const file of tests) testHashes[file]=hash(await readFile(resolve(root,'tests/cpp',file)));
  await mkdir(directory,{recursive:true});
  await writeFile(resolve(directory,name+'-build.json'),JSON.stringify({...manifest,tests,testHashes},null,2)+'\n');

  const before=sourceSnapshot(repository);
  const report=await runTasks([{id:'oracle',command:process.execPath,args:['--test','--test-concurrency='+concurrency,
    '--test-reporter=spec',...tests.map(file=>resolve(root,'tests/cpp',file))],cwd:root,timeoutMs}],
  {output:resolve(directory,name),concurrency:1,environment:{NO_COLOR:'1',NODE_DISABLE_COLORS:'1',NODE_TEST_CONTEXT:undefined,
    TH11_CORE_TEST_FILE:core,EAGLER_ORACLE_ROOT:root}});
  const task=report.results[0],text=await readFile(task.log,'utf8');
  const count=key=>Number(text.match(new RegExp('(?:^|\\n)(?:ℹ )?'+key+' ([0-9.]+)'))?.[1]??0);
  const sourceChanges=[],testChanges=[];
  let buildInventoryError=null,afterBuildInputs=null;
  try {
    afterBuildInputs=await buildInputs(root,manifest);
    sourceChanges.push(...afterBuildInputs.changed);
  } catch (error) {
    buildInventoryError=error.message;
  }
  for (const file of tests) {
    try {if (hash(await readFile(resolve(root,'tests/cpp',file)))!==testHashes[file]) testChanges.push(file);}
    catch {testChanges.push(file);}
  }
  let current=[],inventoryChanged=false;
  try {current=await entries();inventoryChanged=JSON.stringify(current)!==JSON.stringify(tests);}
  catch {inventoryChanged=true;}
  let after=null,snapshotError=null;
  try {after=sourceSnapshot(repository);}
  catch (error) {snapshotError=error.message;}
  const sourceTreeChanged=!after||before.revision!==after.revision||before.sha256!==after.sha256;
  const counts=Object.fromEntries(['tests','pass','fail','cancelled','skipped','todo'].map(key=>[key,count(key)]));
  const passed=report.status==='passed'&&counts.tests>0&&counts.pass===counts.tests&&counts.fail===0&&
    counts.cancelled===0&&counts.skipped===0&&counts.todo===0&&hash(await readFile(core))===coreSha256&&
    !sourceChanges.length&&!buildInventoryError&&!testChanges.length&&!inventoryChanged&&!sourceTreeChanged;
  const result={passed,exitCode:task.code,testFiles:tests.length,...counts,durationMs:report.durationMs,
    coreSha256,sourceChanges,buildInventoryError,testChanges,inventoryChanged,sourceTreeChanged,
    sourceSnapshot:{before:{revision:before.revision,sha256:before.sha256,files:before.files},
      after:after&&{revision:after.revision,sha256:after.sha256,files:after.files},error:snapshotError},
    report:resolve(directory,name,'report.json'),log:task.log};
  await writeFile(resolve(directory,name+'-result.json'),JSON.stringify(result,null,2)+'\n');
  return result;
}
