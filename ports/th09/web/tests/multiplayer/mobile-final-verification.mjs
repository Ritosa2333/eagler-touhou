// Final byte-identity audit for the retained mobile rollback experiment.
// Read-only inputs; exclusive outputs. No game, browser, compiler or test rerun.
import {readFileSync,writeFileSync,readdirSync,existsSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {resolve,basename} from 'node:path';
import {fileURLToPath} from 'node:url';
import {isDeepStrictEqual} from 'node:util';
const self=fileURLToPath(import.meta.url),root=resolve(self,'../../../..');
const old=resolve(root,'../th09-rollback-4x');
const common=resolve(root,'../th09-rollback-input-batch-common');
const dir=resolve(root,'docs/multiplayer'),art=resolve(root,'web/artifacts/multiplayer-tests');
const output=resolve(dir,'rollback-mobile-final-verification.json');
const indexOutput=resolve(dir,'rollback-mobile-execution-index.json');
for(const p of [output,indexOutput])if(existsSync(p))throw Error('Retained output already exists: '+p);
const startedAt=new Date().toISOString(),started=performance.now();
const sha=b=>createHash('sha256').update(b).digest('hex');
const identities=new Map();
function file(p){p=resolve(root,p);if(!identities.has(p)){const b=readFileSync(p);identities.set(p,{path:p,bytes:b.length,sha256:sha(b)});}return identities.get(p);}
const parse=p=>JSON.parse(readFileSync(resolve(root,p),'utf8'));
const git=(cwd,...args)=>execFileSync('git',args,{cwd,encoding:'utf8',maxBuffer:1024*1024}).trim();
const checks=[];
function check(name,passed,details){checks.push({name,passed:passed===true,...(details===undefined?{}:{details})});}
function match(name,p,expected,bytes){try{const got=file(p);check(name,got.sha256===expected&&(bytes===undefined||got.bytes===bytes),{...got,expectedSha256:expected,...(bytes===undefined?{}:{expectedBytes:bytes})});return got;}catch(e){check(name,false,{path:p,error:String(e.message)});return null;}}
const labels=[
'mobile-managed-queue-characterization-b1','mobile-rollback-build-identity-01',
'mobile-rollback-build-identity-matched-01','mobile-rollback-build-live-01',
'mobile-rollback-build-live-matched-01','mobile-rollback-build-release-01',
'mobile-rollback-build-release-matched-01','mobile-rollback-burst120-c1',
'mobile-rollback-components-01','mobile-rollback-direct120-c1','mobile-rollback-direct120-c2',
'mobile-rollback-gap-dispatch-01','mobile-rollback-relay39-b1','mobile-rollback-relay39-b2',
'mobile-rollback-relay39-c1','mobile-rollback-relay39-c2','mobile-rollback-release-summary-01',
'mobile-rollback-stick-deep-c1','mobile-rollback-three-world-summary-01',
'mobile-rollback-three-world-summary-02','mobile-rollback-touch-deep-b1',
'mobile-rollback-touch-deep-c1'
].sort();
const failed={
'mobile-rollback-build-identity-01':'Initial checkout CRLF header broke exact source match; retained, normalized and rebuilt under new labels.',
'mobile-rollback-direct120-c1':'Relay-only trace option rejected before browser startup; corrected to RELAY_TRACE=0 under c2.'
};
const discovered=readdirSync(art).filter(n=>/^mobile.*-invocation\.json$/.test(n)).map(n=>n.replace(/-invocation\.json$/,'')).sort();
check('exactRetainedInvocationSet',isDeepStrictEqual(discovered,labels),{expected:labels,actual:discovered});
const superseded=[];
function currentIdentity(label,p,expected){
 let current=p;
 if(label==='mobile-rollback-three-world-summary-01'&&p==='web/tests/multiplayer/mobile-evidence-summary.mjs'){
  current='web/artifacts/multiplayer-tests/mobile-evidence-summary-v1.mjs';
  superseded.push({label,originalPath:p,retainedPath:current,reason:'Semantic field correction, v1 source retained verbatim.'});
 }
 if(p==='web/cpp/sdl/PerformanceDiagnostics.hpp'&&file(p).sha256!==expected&&
 ['mobile-rollback-build-live-01','mobile-rollback-build-release-01','mobile-rollback-build-identity-01'].includes(label)){
  current='web/artifacts/multiplayer-tests/mobile-performance-diagnostics-checkout-crlf.hpp';
  superseded.push({label,originalPath:p,retainedPath:current,reason:'Explicit checkout line-ending correction; original bytes retained.'});
 }
 return match('invocation.currentOrArchivedSource:'+label+':'+p,current,expected);
}
const invocations=[];
for(const label of labels){
 const p=resolve(art,label+'-invocation.json'),record=parse(p);
 check('invocation.label:'+label,record.runLabel===label);
 check('invocation.expectedExit:'+label,record.status===(failed[label]?1:0)&&record.signal===null,{status:record.status,signal:record.signal,retainedFailure:failed[label]??null});
 check('invocation.stableBeforeAfter:'+label,record.scriptSha256===record.scriptAfterSha256&&record.identityError===null&&isDeepStrictEqual(record.identityBefore,record.identityAfter));
 const log=match('invocation.log:'+label,record.logPath,record.logSha256,record.stdoutBytes+record.stderrBytes);
 for(const [p,hash] of Object.entries(record.identityBefore))currentIdentity(label,p,hash);
 const script=record.args[0];
 currentIdentity(label,script,record.scriptSha256);
 invocations.push({label,invocation:file(p),log,retainedFailure:failed[label]??null,record});
}
const jsPath=resolve(dir,'rollback-mobile-js-evidence.json'),js=parse(jsPath);
check('js.accepted',js.passed===true&&js.sourceStable===true&&js.results.length===4&&js.results.every(r=>r.status===0&&r.error===null));
check('js.stableBeforeAfter',isDeepStrictEqual(js.identityBefore,js.identityAfter));
match('js.log',js.log.path,js.log.sha256);
for(const [p,h] of Object.entries(js.identityBefore))match('js.currentSource:'+p,p,h);
const build=parse(resolve(dir,'rollback-mobile-build-matched-evidence.json'));
check('matchedBuildEvidenceAccepted',build.passed===true&&build.pairs.length===2);
for(const pair of build.pairs){
 check('build.onlyRollbackSession:'+pair.name,pair.passed===true&&pair.sourceCount===275&&pair.unchangedSourceCount===274&&pair.changed.length===1&&pair.changed[0].path==='web/cpp/multiplayer/RollbackSession.cpp');
 for(const [kind,buildRoot] of [['baseline',old],['candidate',root]]){
  const id=pair[kind];
  for(const part of ['manifest','wasm','loader'])match('build.binary:'+pair.name+':'+kind+':'+part,id[part].path,id[part].sha256,id[part].bytes);
  const manifest=parse(id.manifest.path);
  check('build.nativeSourceCount:'+pair.name+':'+kind,Object.keys(manifest.sources).length===275);
  check('build.manifestWasm:'+pair.name+':'+kind,manifest.sha256===id.wasm.sha256&&manifest.bytes===id.wasm.bytes);
  for(const [p,h] of Object.entries(manifest.sources))match('build.currentNative:'+pair.name+':'+kind+':'+p,resolve(buildRoot,p),h);
 }
}
const deep=parse(resolve(dir,'rollback-mobile-deep-corrected-evidence.json'));
const release=parse(resolve(dir,'rollback-mobile-release-evidence.json'));
check('gameplayRunCounts',deep.runs.length===3&&release.runs.length===6);
const raw=[];
for(const r of deep.runs){
 check('deep.declaredComplete:'+basename(r.path),r.available===true&&r.declaredPassed===true&&Object.values(r.checks).every(x=>x===true));
 raw.push(match('deep.raw:'+basename(r.path),r.path,r.rawSha256,r.rawBytes));
}
for(const r of release.runs){
 check('release.accepted:'+basename(r.raw.path),r.declaredPassed===true&&r.verificationPassed===true);
 raw.push(match('release.raw:'+basename(r.raw.path),r.raw.path,r.raw.sha256,r.raw.bytes));
}
const gap=parse(resolve(dir,'rollback-mobile-gap-dispatch-evidence.json'));
check('gap.qualified',gap.qualified===true&&gap.sourceStable===true&&gap.issues.length===0&&gap.runs.length===6);
const oldEvidencePath=resolve(old,'docs/multiplayer/rollback-enemy-index-final-verification.json'),oldEvidence=parse(oldEvidencePath);
check('old.previousVerification',oldEvidence.checksPassed===true&&oldEvidence.errors.length===0&&oldEvidence.checks.length===26);
for(const c of oldEvidence.checks)match('old.protected:'+c.path,resolve(old,c.path),c.expectedSha256,c.bytes);
const repositories={
 candidate:{root,head:git(root,'rev-parse','HEAD'),branch:git(root,'branch','--show-current'),status:git(root,'status','--short')},
 baseline:{root:old,head:git(old,'rev-parse','HEAD'),status:git(old,'status','--short')},
 common:{root:common,head:git(common,'rev-parse','HEAD'),status:git(common,'status','--short')}
};
check('old.frozenClean',repositories.baseline.head==='b243bb1d24335b1fdab3606e27c6f67e4c636ad9'&&repositories.baseline.status==='');
check('common.frozenClean',repositories.common.head==='512353cab9eaa4f6744f867b0eb07c61ee7f2b0e'&&repositories.common.status==='');
check('candidate.coreCommit',repositories.candidate.head==='a93fb876eb6be6c84c1cf2cf271153ee7fa4763e');
const inventory=readdirSync(dir).filter(n=>n.startsWith('rollback-mobile-')&&!['rollback-mobile-final-verification.json','rollback-mobile-execution-index.json'].includes(n)).sort().map(n=>file(resolve(dir,n)));
const selfBefore=file(self),selfAfterBytes=readFileSync(self);
check('self.stable',selfBefore.sha256===sha(selfAfterBytes));
const passed=checks.every(c=>c.passed),endedAt=new Date().toISOString(),elapsedMs=performance.now()-started;
const index={schema:'th09-mobile-execution-index/v1',startedAt,endedAt,elapsedMs,passed,
 scope:'Inventory and byte checks of retained invocations. Expected failed attempts are successful retention checks, not gameplay passes. No timing claim or test rerun.',
 repositories,invocations,jsGate:{evidence:file(jsPath),record:js},supersededSources:superseded,gameplayRaw:raw,evidenceInventory:inventory};
writeFileSync(indexOutput,JSON.stringify(index,null,2)+'\n',{flag:'wx'});
const result={schema:'th09-mobile-final-verification/v1',startedAt,endedAt,elapsedMs,passed,
 purpose:'Freeze mobile-phase evidence before the measurement/documentation commit. All game runs precede the core commit and retain their working-source identities. This file records byte-identity verification, not improved FPS or repaired managed input.',
 verifier:selfBefore,repositories,executionIndex:file(indexOutput),priorVerification:file(oldEvidencePath),evidenceInventory:inventory,
 counts:{invocations:invocations.length,expectedFailedAttempts:Object.keys(failed).length,jsSubcommands:js.results.length,gameplayRaw:raw.length,matchedBuilds:4,nativeSourceEntriesPerBuild:275,priorProtectedChecks:oldEvidence.checks.length,totalChecks:checks.length},
 failures:checks.filter(c=>!c.passed),checks};
writeFileSync(output,JSON.stringify(result,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({passed,counts:result.counts,elapsedMs,outputs:[file(indexOutput),file(output)],failures:result.failures.slice(0,12)},null,2));
if(!passed)process.exitCode=1;

