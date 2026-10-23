import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {readFile,readdir,stat,realpath} from 'node:fs/promises';
import {lstatSync} from 'node:fs';
import {basename,dirname,isAbsolute,relative,resolve,sep} from 'node:path';
import {nativeGroupEnvironment,validateNativeEnvironmentPolicy} from './native-inputs.mjs';

const sha256Pattern=/^[a-f0-9]{64}$/;
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const sorted=items=>[...items].sort();
const posix=path=>path.split(sep).join('/');

function inputError(message) { throw new Error(message); }

function ownerFor(root) { return basename(dirname(resolve(root))); }

function assertSafeRelative(path,label) {
  if (typeof path!=='string'||!path||isAbsolute(path)||path.includes('\0'))
    inputError(`${label} must be a non-empty relative path`);
  const normalized=path.replaceAll('\\','/');
  return normalized;
}

async function shaFile(path) {
  const bytes=await readFile(path);
  return {sha256:digest(bytes),bytes:bytes.length};
}

export async function directorySnapshot(directory) {
  const files=[];
  async function visit(current,prefix='') {
    for (const entry of await readdir(current,{withFileTypes:true})) {
      const name=prefix?`${prefix}/${entry.name}`:entry.name;
      const path=resolve(current,entry.name);
      if (entry.isSymbolicLink()) throw new Error(`Symbolic link inside input directory is not allowed: ${name}`);
      if (entry.isDirectory()) await visit(path,name);
      else if (entry.isFile()) {
        const file=await shaFile(path);
        files.push({path:name.replaceAll('\\','/'),...file});
      } else throw new Error(`Non-regular input entry: ${name}`);
    }
  }
  const info=await stat(directory);
  if (!info.isDirectory()) throw new Error(`Expected directory: ${directory}`);
  await visit(directory);
  files.sort((a,b)=>a.path.localeCompare(b.path));
  const hash=createHash('sha256');
  let bytes=0;
  for (const file of files) {
    bytes+=file.bytes;
    hash.update(file.path).update('\0').update(String(file.bytes)).update('\0').update(file.sha256).update('\n');
  }
  return {sha256:hash.digest('hex'),bytes,files:files.length,entries:files};
}

function normalizeManifestEntries(document,spec) {
  let entries=spec.entriesPath?spec.entriesPath.split('.').reduce((value,key)=>value?.[key],document):document;
  if (!Array.isArray(entries)) throw new Error(`Accepted input manifest entries are not an array: ${spec.manifestPath}`);
  const nameField=spec.nameField??'name',sizeField=spec.sizeField??'size',shaField=spec.shaField??'sha256';
  const values=entries.map((entry,index)=>{
    const path=entry?.[nameField],size=entry?.[sizeField],sha256=entry?.[shaField];
    if (typeof path!=='string'||!path||path.startsWith('/')||path.split(/[\\/]/).includes('..'))
      throw new Error(`Invalid accepted manifest path at entry ${index}`);
    if (!Number.isSafeInteger(size)||size<0||!sha256Pattern.test(sha256))
      throw new Error(`Invalid accepted manifest digest at entry ${index}: ${path}`);
    return {path:path.replaceAll('\\','/'),bytes:size,sha256};
  }).sort((a,b)=>a.path.localeCompare(b.path));
  if (new Set(values.map(value=>value.path)).size!==values.length)
    throw new Error(`Duplicate accepted manifest path: ${spec.manifestPath}`);
  return values;
}

async function manifestTreeSnapshot(root,spec) {
  const manifestPath=resolve(root,assertSafeRelative(spec.manifestPath,'manifestPath'));
  const manifestBytes=await readFile(manifestPath),manifestSha256=digest(manifestBytes);
  if (!sha256Pattern.test(spec.manifestSha256))
    throw new Error(`Authoritative manifest SHA-256 is required: ${spec.manifestPath}`);
  if (manifestSha256!==spec.manifestSha256)
    throw new Error(`Accepted manifest SHA-256 mismatch: ${spec.manifestPath}`);
  const expected=normalizeManifestEntries(JSON.parse(manifestBytes.toString('utf8')),spec);
  const rootPath=resolve(root,assertSafeRelative(spec.rootPath,'rootPath'));
  const actual=await directorySnapshot(rootPath);
  const expectedNames=expected.map(value=>value.path),actualNames=actual.entries.map(value=>value.path);
  if (JSON.stringify(expectedNames)!==JSON.stringify(actualNames))
    throw new Error(`Accepted input file inventory mismatch: ${spec.rootPath}`);
  const actualByPath=new Map(actual.entries.map(value=>[value.path,value]));
  for (const expectedFile of expected) {
    const actualFile=actualByPath.get(expectedFile.path);
    if (actualFile.bytes!==expectedFile.bytes||actualFile.sha256!==expectedFile.sha256)
      throw new Error(`Accepted input hash mismatch: ${spec.rootPath}/${expectedFile.path}`);
  }
  return {path:rootPath,sha256:actual.sha256,bytes:actual.bytes,files:actual.files,
    manifestPath,manifestSha256};
}

async function loadReceipt(root,owner,environment) {
  const receiptPath=environment.EAGLER_ORACLE_INPUT_RECEIPT;
  if (!receiptPath) return {error:'EAGLER_ORACLE_INPUT_RECEIPT is required for private inputs'};
  const absolute=resolve(root,receiptPath);
  try {
    const bytes=await readFile(absolute),document=JSON.parse(bytes.toString('utf8'));
    if (document.schema!=='eagler-touhou/oracle-private-inputs/1'||document.owner!==owner||
        !document.inputs||Array.isArray(document.inputs)||typeof document.inputs!=='object')
      return {error:`Invalid private input receipt for ${owner}`};
    return {path:absolute,sha256:digest(bytes),document};
  } catch (error) {
    return {error:`Cannot read private input receipt: ${error.message}`};
  }
}

function receiptInput(receipt,spec,environment) {
  if (receipt.error) throw new Error(receipt.error);
  const input=receipt.document.inputs[spec.receiptId];
  if (!input||!['file','directory'].includes(input.kind)||typeof input.path!=='string'||
      !input.path||!sha256Pattern.test(input.sha256))
    throw new Error(`Private receipt lacks authoritative path/hash: ${spec.receiptId}`);
  if (spec.inputKind&&spec.inputKind!==input.kind)
    throw new Error(`Private receipt input kind mismatch: ${spec.receiptId}`);
  const path=resolve(rootForReceipt(receipt),input.path);
  let environmentPath=path;
  if (input.envPath!==undefined) {
    if (typeof input.envPath!=='string'||!input.envPath)
      throw new Error(`Private receipt envPath must be a non-empty path: ${spec.receiptId}`);
    environmentPath=resolve(rootForReceipt(receipt),input.envPath);
    if (environmentPath!==path)
      throw new Error(`Private receipt envPath must resolve to its hashed path: ${spec.receiptId}`);
  }
  if (spec.expectedPath&&path!==resolve(rootForReceipt(receipt),assertSafeRelative(spec.expectedPath,'expectedPath')))
    throw new Error(`Private receipt path mismatch: ${spec.receiptId}`);
  if (spec.pathEnv) {
    const existing=environment[spec.pathEnv];
    if (existing&&resolve(existing)!==environmentPath)
      throw new Error(`${spec.pathEnv} conflicts with private input receipt`);
  }
  return {input,path,expectedSha256:input.sha256,receiptSha256:receipt.sha256,
    setEnv:spec.pathEnv?{[spec.pathEnv]:environmentPath}: {}};
}

function isWithin(parent,path) {
  const rel=relative(parent,path);
  return !rel||(!isAbsolute(rel)&&rel!=='..'&&!rel.startsWith(`..${sep}`));
}

function sameResolvedPath(first,second) {
  const normalize=value=>{
    const path=resolve(value);
    return process.platform==='win32'?path.toLowerCase():path;
  };
  return normalize(first)===normalize(second);
}

async function sameFileIdentity(first,second) {
  const [a,b]=await Promise.all([stat(first,{bigint:true}),stat(second,{bigint:true})]);
  return a.dev!==0n&&b.dev!==0n&&a.ino!==0n&&b.ino!==0n&&a.dev===b.dev&&a.ino===b.ino;
}

async function captureReferenceIdentity(root,spec,resolved,actual,receipt,environment) {
  const policy=spec.referenceIdentity;
  if (!policy) return null;
  if (policy.kind==='accepted-reference-file') {
    const reference=resolved.input.referenceSource;
    if (!reference||typeof reference.path!=='string'||!reference.path||!sha256Pattern.test(reference.sha256)||
        typeof reference.provenance!=='string'||!reference.provenance.trim())
      throw new Error(`Private receipt lacks accepted reference source identity: ${spec.receiptId}`);
    const sourcePath=resolve(root,assertSafeRelative(reference.path,'referenceSource.path'));
    const referenceRoot=resolve(root,'reference');
    if (!isWithin(referenceRoot,sourcePath)||sourcePath===referenceRoot)
      throw new Error(`Reference source must be located under reference/: ${reference.path}`);
    if (!lstatSync(sourcePath,{throwIfNoEntry:false})?.isFile())
      throw new Error(`Reference source must be a regular file: ${reference.path}`);
    const sourceReal=await realpath(sourcePath);
    const source=await shaFile(sourceReal);
    if (source.sha256!==reference.sha256||source.sha256!==actual.sha256)
      throw new Error(`Accepted reference source hash mismatch: ${reference.path}`);
    return {kind:policy.kind,path:sourceReal,sha256:source.sha256,provenance:reference.provenance,
      receiptPath:receipt.path,receiptSha256:receipt.sha256};
  }
  if (policy.kind==='receipt-json-field') {
    const sourceSpec={receiptId:policy.receiptId,inputKind:'directory'};
    const source=receiptInput(receipt,sourceSpec,environment);
    const tree=await directorySnapshot(source.path);
    if (tree.sha256!==source.expectedSha256)
      throw new Error(`Reference identity receipt hash mismatch: ${policy.receiptId}`);
    const statePath=resolve(source.path,assertSafeRelative(policy.file,'referenceIdentity.file'));
    if (!isWithin(source.path,statePath)||statePath===source.path)
      throw new Error('Reference identity file must stay inside its receipt directory');
    const state=JSON.parse(await readFile(statePath,'utf8'));
    const value=policy.jsonPath.split('.').reduce((current,key)=>current?.[key],state);
    if (value!==actual.sha256)
      throw new Error(`Reference identity field does not match ${spec.receiptId}: ${policy.receiptId}/${policy.file}#${policy.jsonPath}`);
    return {kind:policy.kind,receiptId:policy.receiptId,path:statePath,receiptPath:source.path,
      receiptSha256:source.expectedSha256,file:statePath,jsonPath:policy.jsonPath,sha256:value};
  }
  throw new Error(`Unsupported reference identity policy: ${policy.kind}`);
}

async function compareCandidateArtifact(root,spec,inputPath,inputSha256,referenceIdentity) {
  if (!spec.distinctFromCandidate) return null;
  const candidatePath=resolve(root,assertSafeRelative(spec.distinctFromCandidate,'distinctFromCandidate'));
  if (!isWithin(root,candidatePath)||candidatePath===root)
    throw new Error(`Candidate artifact path must stay inside the title checkout: ${spec.distinctFromCandidate}`);
  let inputReal,candidateReal,candidate;
  try {
    [inputReal,candidateReal]=await Promise.all([realpath(inputPath),realpath(candidatePath)]);
    candidate=await shaFile(candidateReal);
  } catch (error) {
    throw new Error(`Cannot verify baseline independence from candidate ${spec.distinctFromCandidate}: ${error.message}`);
  }
  if (sameResolvedPath(inputPath,candidatePath)||sameResolvedPath(inputReal,candidateReal)||
      await sameFileIdentity(inputReal,candidateReal))
    throw new Error(`Reference input aliases the candidate artifact: ${spec.distinctFromCandidate}`);
  if (referenceIdentity&&(sameResolvedPath(referenceIdentity.path,candidatePath)||
      sameResolvedPath(referenceIdentity.path,candidateReal)||await sameFileIdentity(referenceIdentity.path,candidateReal)))
    throw new Error(`Reference source aliases the candidate artifact: ${spec.distinctFromCandidate}`);
  return {path:candidateReal,sha256:candidate.sha256,sameSha256:candidate.sha256===inputSha256,
    referenceIdentityBound:Boolean(referenceIdentity)};
}

const receiptRoots=new WeakMap();
function rootForReceipt(receipt) { return receiptRoots.get(receipt); }

async function captureInput(root,spec,receipt,environment) {
  if (spec.kind==='target-executable') {
    const target=JSON.parse(await readFile(resolve(root,'target.json'),'utf8'));
    if (typeof target.executable!=='string'||!sha256Pattern.test(target.sha256))
      throw new Error('target.json does not provide executable path and authoritative SHA-256');
    const path=resolve(root,target.executable),actual=await shaFile(path);
    if (actual.sha256!==target.sha256) throw new Error('Original executable does not match target.json SHA-256');
    return {id:spec.id,kind:spec.kind,path, ...actual};
  }
  if (spec.kind==='accepted-manifest-tree') {
    return {id:spec.id,kind:spec.kind,...await manifestTreeSnapshot(root,spec)};
  }
  if (spec.kind==='receipt') {
    const resolved=receiptInput(receipt,spec,environment);
    const actual=resolved.input.kind==='file'?await shaFile(resolved.path):await directorySnapshot(resolved.path);
    if (actual.sha256!==resolved.expectedSha256)
      throw new Error(`Private input hash mismatch: ${spec.receiptId}`);
    const referenceIdentity=await captureReferenceIdentity(root,spec,resolved,actual,receipt,environment);
    const candidateComparison=await compareCandidateArtifact(root,spec,resolved.path,actual.sha256,referenceIdentity);
    if (candidateComparison?.sameSha256&&!referenceIdentity)
      throw new Error(`Baseline matches candidate bytes without independent reference identity: ${spec.id}`);
    return {id:spec.id,kind:spec.kind,receiptId:spec.receiptId,path:resolved.path,
      sha256:actual.sha256,bytes:actual.bytes,files:actual.files,receiptSha256:resolved.receiptSha256,
      ...(referenceIdentity?{referenceIdentity}:{}),...(candidateComparison?{candidateComparison}:{}),
      setEnv:resolved.setEnv};
  }
  if (spec.kind==='file'||spec.kind==='directory') {
    const configured=spec.pathEnv?environment[spec.pathEnv]:spec.path;
    if (!configured) throw new Error(`${spec.pathEnv??spec.path} is required`);
    const path=spec.pathEnv?resolve(configured):resolve(root,assertSafeRelative(spec.path,'input path'));
    const actual=spec.kind==='file'?await shaFile(path):await directorySnapshot(path);
    const expected=spec.sha256??(spec.sha256Env?environment[spec.sha256Env]:null);
    if (!sha256Pattern.test(expected??'')) throw new Error(`Authoritative SHA-256 is required for ${spec.id}`);
    if (actual.sha256!==expected) throw new Error(`Private input hash mismatch: ${spec.id}`);
    return {id:spec.id,kind:spec.kind,path,...actual};
  }
  if (spec.kind==='env-value') {
    const actual=environment[spec.name];
    if (actual!==spec.value) throw new Error(`${spec.name} must equal ${JSON.stringify(spec.value)}`);
    return {id:spec.id,kind:spec.kind,name:spec.name,value:actual};
  }
  throw new Error(`Unsupported oracle input kind: ${spec.kind}`);
}

function validateInput(spec,label) {
  if (!spec||typeof spec!=='object'||typeof spec.id!=='string'||!spec.id||typeof spec.kind!=='string')
    inputError(`${label} requires id and kind`);
  if (spec.kind==='target-executable') return;
  if (spec.kind==='accepted-manifest-tree') {
    assertSafeRelative(spec.manifestPath,`${label}.manifestPath`);
    assertSafeRelative(spec.rootPath,`${label}.rootPath`);
    if (!sha256Pattern.test(spec.manifestSha256)) inputError(`${label} requires an accepted manifest SHA-256`);
    return;
  }
  if (spec.kind==='receipt') {
    if (typeof spec.receiptId!=='string'||!spec.receiptId) inputError(`${label} requires receiptId`);
    if (spec.inputKind&&!['file','directory'].includes(spec.inputKind)) inputError(`${label}.inputKind is invalid`);
    if (spec.pathEnv&&typeof spec.pathEnv!=='string') inputError(`${label}.pathEnv must be a string`);
    if (spec.expectedPath) assertSafeRelative(spec.expectedPath,`${label}.expectedPath`);
    if (spec.distinctFromCandidate) {
      if (spec.inputKind&&spec.inputKind!=='file') inputError(`${label}.distinctFromCandidate requires a file input`);
      assertSafeRelative(spec.distinctFromCandidate,`${label}.distinctFromCandidate`);
      if (!spec.referenceIdentity||!['accepted-reference-file','receipt-json-field'].includes(spec.referenceIdentity.kind))
        inputError(`${label} requires a supported referenceIdentity to compare against the candidate`);
    }
    if (spec.referenceIdentity&&spec.inputKind!=='file')
      inputError(`${label}.referenceIdentity requires a file input`);
    if (spec.referenceIdentity?.kind==='accepted-reference-file') {
      if (spec.referenceIdentity.receiptId!==undefined||spec.referenceIdentity.file!==undefined)
        inputError(`${label}.referenceIdentity accepted-reference-file has unexpected fields`);
    } else if (spec.referenceIdentity?.kind==='receipt-json-field') {
      const identity=spec.referenceIdentity;
      if (typeof identity.receiptId!=='string'||!identity.receiptId||typeof identity.file!=='string'||!identity.file||
          typeof identity.jsonPath!=='string'||!/^[$A-Z_a-z][$\w]*(?:\.[$A-Z_a-z][$\w]*)*$/.test(identity.jsonPath))
        inputError(`${label}.referenceIdentity receipt-json-field requires receiptId, file and jsonPath`);
      assertSafeRelative(identity.file,`${label}.referenceIdentity.file`);
    } else if (spec.referenceIdentity!==undefined) inputError(`${label}.referenceIdentity has unsupported kind`);
    return;
  }
  if (spec.kind==='file'||spec.kind==='directory') {
    if (spec.path) assertSafeRelative(spec.path,`${label}.path`);
    if (!spec.path&&!spec.pathEnv) inputError(`${label} requires path or pathEnv`);
    if (spec.sha256&&!sha256Pattern.test(spec.sha256)) inputError(`${label}.sha256 is invalid`);
    if (!spec.sha256&&!spec.sha256Env) inputError(`${label} requires an authoritative SHA-256`);
    return;
  }
  if (spec.kind==='env-value') {
    if (typeof spec.name!=='string'||!spec.name||typeof spec.value!=='string') inputError(`${label} requires name and value`);
    return;
  }
  inputError(`${label} has unsupported kind ${spec.kind}`);
}

/** Load the checked-in catalog and require one explicit classification for every discovered test file. */
export async function loadOracleSuite(root) {
  const owner=ownerFor(root),suitePath=resolve(root,'tests/cpp/oracle-suite.json');
  const suite=JSON.parse(await readFile(suitePath,'utf8'));
  if (suite.schema!=='eagler-touhou/native-oracle-suite/1') throw new Error('Unsupported native oracle suite schema');
  const target=JSON.parse(await readFile(resolve(root,'target.json'),'utf8'));
  const game=owner.replace(/-mp$/,'');
  const variant=owner.endsWith('-mp')?'mp':'normal';
  if (suite.owner!==owner||suite.game!==game||suite.variant!==variant||target.game!==game)
    throw new Error(`Oracle suite target identity mismatch for ${owner}`);
  validateNativeEnvironmentPolicy(suite);
  if (suite.discovery?.directory!=='tests/cpp'||suite.discovery?.pattern!=='*.test.mjs')
    throw new Error('Native oracle discovery must use tests/cpp/*.test.mjs');
  if (!Array.isArray(suite.tests)||!suite.tests.length||!Array.isArray(suite.groups)||!suite.groups.length||
      !Array.isArray(suite.baseInputs)) throw new Error('Native oracle suite requires tests, groups and baseInputs');
  const discovered=(await readdir(resolve(root,'tests/cpp'),{withFileTypes:true}))
    .filter(entry=>entry.isFile()&&entry.name.endsWith('.test.mjs')).map(entry=>entry.name).sort();
  const seen=new Set(),groupIds=new Set(suite.groups.map(group=>group.id));
  if (groupIds.size!==suite.groups.length) throw new Error('Duplicate oracle group id');
  const files=[];
  for (const [index,item] of suite.tests.entries()) {
    if (!item||typeof item.file!=='string'||seen.has(item.file)) throw new Error(`Invalid or duplicate test classification at index ${index}`);
    seen.add(item.file);files.push(item.file);
    if (item.role==='candidate') {
      if (typeof item.group!=='string'||!groupIds.has(item.group)) throw new Error(`Candidate test has no valid group: ${item.file}`);
    } else if (item.role==='maintenance') {
      if (typeof item.task!=='string'||!suite.maintenance?.some(task=>task.id===item.task&&task.file===item.file))
        throw new Error(`Maintenance test has no explicit task: ${item.file}`);
    } else throw new Error(`Invalid test role: ${item.file}`);
  }
  if (JSON.stringify(sorted(files))!==JSON.stringify(discovered)) {
    const missing=discovered.filter(file=>!seen.has(file)),extra=files.filter(file=>!discovered.includes(file));
    throw new Error(`Oracle test classification mismatch; unclassified=${missing.join(',')||'none'}; absent=${extra.join(',')||'none'}`);
  }
  for (const group of suite.groups) {
    if (!/^[a-z0-9][a-z0-9-]*$/.test(group.id)||!Array.isArray(group.requiredInputs)) throw new Error(`Invalid oracle group: ${group.id}`);
    if (!suite.tests.some(item=>item.role==='candidate'&&item.group===group.id)) throw new Error(`Empty oracle candidate group: ${group.id}`);
    for (const [index,input] of group.requiredInputs.entries()) validateInput(input,`${group.id}.requiredInputs[${index}]`);
    for (const input of group.requiredInputs) if (input.referenceIdentity?.kind==='receipt-json-field'&&
        !group.requiredInputs.some(source=>source.kind==='receipt'&&source.receiptId===input.referenceIdentity.receiptId&&source.inputKind==='directory'))
      throw new Error(`Reference identity receipt must also be a required directory input: ${group.id}/${input.id}`);
    const ids=group.requiredInputs.map(input=>input.id);
    if (new Set(ids).size!==ids.length) throw new Error(`Duplicate required input id: ${group.id}`);
    if (group.environment!==undefined&&(!group.environment||Array.isArray(group.environment)||typeof group.environment!=='object'||
        Object.entries(group.environment).some(([key,value])=>!/^[-A-Z0-9_]+$/.test(key)||typeof value!=='string')))
      throw new Error(`Invalid oracle environment overrides: ${group.id}`);
  }
  for (const [index,input] of suite.baseInputs.entries()) validateInput(input,`baseInputs[${index}]`);
  const baseIds=suite.baseInputs.map(input=>input.id);
  if (new Set(baseIds).size!==baseIds.length) throw new Error('Duplicate base input id');
  const maintenanceIds=new Set();
  for (const task of suite.maintenance??[]) {
    if (!task||!/^[a-z0-9][a-z0-9-]*$/.test(task.id??'')||maintenanceIds.has(task.id)||task.kind!=='test'||
        typeof task.file!=='string'||task.file.includes('/')||task.file.includes('\\')||!task.file.endsWith('.mjs')||
        typeof task.outputRoot!=='string') throw new Error('Invalid oracle maintenance task');
    maintenanceIds.add(task.id);
    if (task.outputRoot!=='artifacts/validation/oracle-maintenance') throw new Error(`Unsafe maintenance output root: ${task.outputRoot}`);
    assertSafeRelative(task.file,'maintenance.file');
    for (const [index,input] of (task.requiredInputs??[]).entries()) validateInput(input,`${task.id}.requiredInputs[${index}]`);
  }
  return {...suite,root:resolve(root),owner,target,discovered,
    candidateGroups:suite.groups.map(group=>({...group,tests:suite.tests.filter(item=>item.role==='candidate'&&item.group===group.id).map(item=>item.file)}))};
}

/** Capture each declared input and return values to expose its receipt-bound environment paths. */
function inputCacheKey(spec,receipt,environment) {
  const relevant={spec,receipt:receipt.error??receipt.sha256};
  if (spec.pathEnv) relevant.path=environment[spec.pathEnv]??null;
  if (spec.sha256Env) relevant.sha256=environment[spec.sha256Env]??null;
  if (spec.kind==='env-value') relevant.value=environment[spec.name]??null;
  return JSON.stringify(relevant);
}

export async function preflightOracleInputs(root,suite,requiredInputs,environment=process.env,cache=null) {
  const receipt=await loadReceipt(root,suite.owner,environment);
  if (!receipt.error) receiptRoots.set(receipt,resolve(root));
  const snapshots=[],envOverrides={},blocked=[];
  for (const spec of requiredInputs) {
    try {
      const key=cache&&inputCacheKey(spec,receipt,environment);
      let snapshot=key?cache.get(key):null;
      if (!snapshot) {
        snapshot=await captureInput(root,spec,receipt,environment);
        if (key) cache.set(key,snapshot);
      }
      snapshots.push(snapshot);
      Object.assign(envOverrides,snapshot.setEnv??{});
    } catch (error) {
      blocked.push({input:spec.id,reason:error.message});
    }
  }
  return {status:blocked.length?'blocked':'ready',blocked,inputs:snapshots,envOverrides,
    receipt:receipt.error?null:{path:receipt.path,sha256:receipt.sha256}};
}

export async function preflightOracleSuite(root,environment=process.env) {
  const suite=await loadOracleSuite(root),cache=new Map(),baseEnvironment=nativeGroupEnvironment(suite,null,{},environment).environment,
    base=await preflightOracleInputs(root,suite,suite.baseInputs,baseEnvironment,cache),groups=[];
  for (const group of suite.candidateGroups) {
    const configured=nativeGroupEnvironment(suite,group,base.envOverrides,environment);
    const extra=await preflightOracleInputs(root,suite,group.requiredInputs,configured.environment,cache);
    const groupEnvironment=nativeGroupEnvironment(suite,group,{...base.envOverrides,...extra.envOverrides},environment);
    groups.push({id:group.id,status:base.status==='ready'&&extra.status==='ready'?'ready':'blocked',
      tests:group.tests.length,blocked:[...base.blocked,...extra.blocked],inputs:[...base.inputs,...extra.inputs],
      envOverrides:{...base.envOverrides,...extra.envOverrides},effectiveScope:groupEnvironment.effective});
  }
  return {owner:suite.owner,game:suite.game,variant:suite.variant,discoveredFiles:suite.discovered.length,
    candidateFiles:suite.tests.filter(item=>item.role==='candidate').length,
    maintenanceFiles:suite.tests.filter(item=>item.role==='maintenance').length,
    receipt:base.receipt,groups};
}

export function inputSnapshotsEqual(before,after) {
  const normalize=items=>items.map(({setEnv,...value})=>value).sort((a,b)=>a.id.localeCompare(b.id));
  return JSON.stringify(normalize(before))===JSON.stringify(normalize(after));
}

function declaredReferencePaths(root,suite) {
  const reference=resolve(root,'reference'),paths=[];
  const inputs=[...(suite.baseInputs??[]),...(suite.groups??[]).flatMap(group=>group.requiredInputs??[]),
    ...(suite.maintenance??[]).flatMap(task=>task.requiredInputs??[])];
  for (const spec of inputs) for (const value of [spec.expectedPath,spec.rootPath,spec.path]) {
    if (typeof value!=='string'||!value) continue;
    let absolute;
    try {absolute=resolve(root,assertSafeRelative(value,'accepted reference input path'));} catch {continue;}
    const path=relative(root,absolute).split(sep).join('/');
    if (path==='reference'||path.startsWith('reference/')) paths.push(path);
  }
  return [...new Set(paths)];
}

/** Snapshot accepted reference files, including ignored content and declared junction roots. */
export async function acceptedReferenceSnapshot(root,suite=null) {
  root=resolve(root);
  suite??=await loadOracleSuite(root);
  const directory=resolve(root,'reference');
  let info;
  try {info=lstatSync(directory);}
  catch(error) {
    if(error.code==='ENOENT')return {path:'reference',exists:false,sha256:null,bytes:0,files:0};
    throw error;
  }
  if(info.isSymbolicLink()||!info.isDirectory())throw new Error('Accepted reference root must be an ordinary directory');
  const target=await realpath(directory),declared=declaredReferencePaths(root,suite),records=[],links=[];
  async function visit(current,prefix='reference') {
    for (const entry of await readdir(current,{withFileTypes:true})) {
      const name=`${prefix}/${entry.name}`,path=resolve(current,entry.name);
      if (entry.isSymbolicLink()) {
        if (!declared.some(item=>item===name||item.startsWith(name+'/')))
          throw new Error(`Undeclared symbolic link inside accepted reference: ${name}`);
        const linkTarget=await realpath(path),targetInfo=await stat(path);
        if(!targetInfo.isDirectory())throw new Error(`Declared reference link must target a directory: ${name}`);
        const snapshot=await directorySnapshot(path),link={path:name,target:linkTarget,sha256:snapshot.sha256,
          bytes:snapshot.bytes,files:snapshot.files};
        links.push(link);records.push({...link,kind:'link'});
      } else if (entry.isDirectory()) {
        records.push({path:name,kind:'directory'});await visit(path,name);
      } else if (entry.isFile()) {
        const file=await shaFile(path);records.push({path:name,kind:'file',...file});
      } else throw new Error(`Non-regular accepted reference entry: ${name}`);
    }
  }
  await visit(directory);
  records.sort((a,b)=>a.path.localeCompare(b.path));links.sort((a,b)=>a.path.localeCompare(b.path));
  const hash=createHash('sha256');let bytes=0,files=0;
  for(const record of records) {
    hash.update(record.path).update('\0').update(record.kind).update('\0');
    if(record.kind==='file') {bytes+=record.bytes;files++;hash.update(String(record.bytes)).update('\0').update(record.sha256);}
    else if(record.kind==='link') {
      bytes+=record.bytes;files+=record.files;
      hash.update(record.target).update('\0').update(String(record.bytes)).update('\0').update(record.sha256);
    }
    hash.update('\n');
  }
  return {path:'reference',exists:true,rootKind:'directory',target,sha256:hash.digest('hex'),bytes,files,links};
}

export function acceptedReferenceSnapshotsEqual(before,after) {
  return !!before&&!!after&&before.path===after.path&&before.exists===after.exists&&
    before.rootKind===after.rootKind&&before.target===after.target&&
    before.sha256===after.sha256&&before.bytes===after.bytes&&before.files===after.files;
}

export function maintenanceOutputPath(root,task,name) {
  root=resolve(root);
  if(!/^[a-z0-9][a-z0-9-]*$/.test(task?.id??'')||!/^[a-z0-9][a-z0-9-]*$/.test(name??''))
    throw new Error('Maintenance output task and evidence name must be safe identifiers');
  const base=resolve(root,task.outputRoot),baseRelative=relative(root,base),path=resolve(base,task.id,name);
  if(!baseRelative||isAbsolute(baseRelative)||baseRelative==='..'||baseRelative.startsWith('..'+sep))
    throw new Error('Maintenance output root must stay inside the title checkout');
  if (path!==base&&!path.startsWith(base+sep)) throw new Error('Maintenance output escapes its ignored staging root');
  const rootInfo=lstatSync(root,{throwIfNoEntry:false});
  if(!rootInfo?.isDirectory()||rootInfo.isSymbolicLink())throw new Error('Maintenance checkout root must be an ordinary directory');
  let current=root;
  for(const part of relative(root,path).split(sep)) {
    current=resolve(current,part);
    const info=lstatSync(current,{throwIfNoEntry:false});
    if(!info)break;
    if(info.isSymbolicLink())throw new Error(`Maintenance output path cannot cross a symbolic link: ${current}`);
    if(!info.isDirectory())throw new Error(`Maintenance output path contains a non-directory entry: ${current}`);
  }
  const relativePath=relative(root,path).split(sep).join('/');
  try {
    execFileSync('git',['check-ignore','-q','--',relativePath],{cwd:root,stdio:'ignore',windowsHide:true});
  } catch { throw new Error(`Maintenance output must be git-ignored: ${relativePath}`); }
  return path;
}
