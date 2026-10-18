// Matched-source build comparison after an explicit checkout line-ending fix.
// The initial comparison script/evidence and original builds remain frozen.
// Run only after both candidate builds finish; no compiler/browser is spawned.
import {readFileSync,writeFileSync,statSync,existsSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../../../',import.meta.url));
const old='D:/workspace/eagler/worktrees/th09-rollback-4x';
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const file=path=>{const bytes=readFileSync(path);return {path,bytes:bytes.length,sha256:digest(bytes)};};
const planPath=resolve(root,'docs/multiplayer/rollback-mobile-validation-plan.json');
const plan=JSON.parse(readFileSync(planPath));
const output=resolve(root,'docs/multiplayer/rollback-mobile-build-matched-evidence.json');
if(existsSync(output))throw Error('Retained mobile build evidence exists');
const canonicalFlags=(flags,title)=>flags.map(value=>value.replaceAll('\\','/').replaceAll(title.replaceAll('\\','/').replace(/\/$/,''),'TITLE_ROOT'));
const pairs=[];
for(const release of [true,false]){
 const name=release?'release':'diagnostic';
 const baselinePath=release?plan.baselines.release.path:plan.baselines.diagnostic.path;
 const candidatePath=resolve(root,'web/artifacts/'+(release?'mobile-rollback-release-matched':'mobile-rollback-live-matched'));
 const wasmName=release?'th09.wasm':'th09-presentation.wasm';
 const loaderName=release?'th09.mjs':'th09-presentation.mjs';
 const collect=directory=>{
  const manifestPath=resolve(directory,'build.json'),manifest=JSON.parse(readFileSync(manifestPath));
  return {manifest,identity:{manifest:file(manifestPath),wasm:file(resolve(directory,wasmName)),loader:file(resolve(directory,loaderName))}};
 };
 const baseline=collect(baselinePath),candidate=collect(candidatePath);
 const keys=[...new Set([...Object.keys(baseline.manifest.sources),...Object.keys(candidate.manifest.sources)])].sort();
 const changed=keys.filter(k=>baseline.manifest.sources[k]!==candidate.manifest.sources[k]).map(path=>({path,baseline:baseline.manifest.sources[path]??null,candidate:candidate.manifest.sources[path]??null}));
 const same=(a,b)=>JSON.stringify(a)===JSON.stringify(b);
 const expected=release?plan.baselines.release.wasmSha256:plan.baselines.diagnostic.wasmSha256;
 const checks={
  frozenBaselineWasm:baseline.identity.wasm.sha256===expected,
  baselineManifestWasm:baseline.manifest.sha256===baseline.identity.wasm.sha256&&baseline.manifest.bytes===baseline.identity.wasm.bytes,
  candidateManifestWasm:candidate.manifest.sha256===candidate.identity.wasm.sha256&&candidate.manifest.bytes===candidate.identity.wasm.bytes,
  onlyRollbackSessionSourceChanged:changed.length===1&&changed[0].path==='web/cpp/multiplayer/RollbackSession.cpp',
  sourceListsEqual:same(Object.keys(baseline.manifest.sources).sort(),Object.keys(candidate.manifest.sources).sort()),
  compilerFlagsEqualAfterTitleRootNormalization:same(canonicalFlags(baseline.manifest.compilerFlags,old),canonicalFlags(candidate.manifest.compilerFlags,root)),
  perSourceFlagsEqual:same(baseline.manifest.sourceCompilerFlags,candidate.manifest.sourceCompilerFlags),
  importsEqual:same(baseline.manifest.imports,candidate.manifest.imports),
  exportsEqual:same(baseline.manifest.exports,candidate.manifest.exports),
  audioSimdDisabled:baseline.manifest.audioSimd===false&&candidate.manifest.audioSimd===false,
  kindEqual:baseline.manifest.kind===candidate.manifest.kind,
  sourceMatchesCurrentFile:candidate.manifest.sources['web/cpp/multiplayer/RollbackSession.cpp']===file(resolve(root,'web/cpp/multiplayer/RollbackSession.cpp')).sha256
 };
 pairs.push({name,checks,passed:Object.values(checks).every(Boolean),sourceCount:keys.length,unchangedSourceCount:keys.length-changed.length,changed,
  wasmBytesDelta:candidate.identity.wasm.bytes-baseline.identity.wasm.bytes,
  baseline:baseline.identity,candidate:candidate.identity,
  compilerFlags:{baseline:baseline.manifest.compilerFlags,candidate:candidate.manifest.compilerFlags},
  sourceCompilerFlags:{baseline:baseline.manifest.sourceCompilerFlags,candidate:candidate.manifest.sourceCompilerFlags},
  imports:baseline.manifest.imports,exports:baseline.manifest.exports,
  builtAt:{baseline:baseline.manifest.builtAt,candidate:candidate.manifest.builtAt}});
}
const invocations=['mobile-rollback-build-release-matched-01','mobile-rollback-build-live-matched-01'].map(label=>{
 const path=resolve(root,'web/artifacts/multiplayer-tests/'+label+'-invocation.json');
 return {label,identity:file(path),...JSON.parse(readFileSync(path))};
});
const evidence={schema:'th09/mobile-build-evidence/1',createdAt:new Date().toISOString(),
 passed:pairs.every(x=>x.passed)&&invocations.every(x=>x.status===0&&x.scriptSha256===x.scriptAfterSha256&&JSON.stringify(x.identityBefore)===JSON.stringify(x.identityAfter)),
 scope:'Build inputs and binary identities; this is neither gameplay correctness nor a performance result. Root-path compiler include relocation is normalized explicitly; raw flags remain below.',
 initialComparison:file(resolve(root,'docs/multiplayer/rollback-mobile-build-evidence.json')),lineEndingControl:file(resolve(root,'docs/multiplayer/rollback-mobile-line-ending-control.json')),plan:file(planPath),analyzer:file(fileURLToPath(import.meta.url)),pairs,invocations};
writeFileSync(output,JSON.stringify(evidence,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({passed:evidence.passed,output,...file(output),pairs:pairs.map(({name,passed,checks,sourceCount,unchangedSourceCount,changed,wasmBytesDelta,candidate})=>({name,passed,checks,sourceCount,unchangedSourceCount,changed,wasmBytesDelta,candidate}))}));
if(!evidence.passed)process.exitCode=1;
