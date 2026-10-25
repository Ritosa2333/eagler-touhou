import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createHash} from 'node:crypto';
import {assertInterruptedPackageDownload,assessPackageUpdateInterruptionEvidence} from './package-update-interruption.mjs';
const product={game:'th08',variant:'normal'},sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'package-interruption-'));t.after(()=>rmSync(directory,{recursive:true,force:true}));
  const descriptor={game:'th08',revision:'new-revision',files:{a:{source:'games/th08/a.ogg',bytes:12,sha256:'a'.repeat(64)},b:{source:'games/th08/b.ogg',bytes:32,sha256:'b'.repeat(64)}}};
  const bytes=Buffer.from(JSON.stringify(descriptor)),path=join(directory,'package.json');writeFileSync(path,bytes);
  const generation={id:'old',descriptor:{revision:'old-revision'},files:{'game-data':{objectId:'data'}}};
  const before={installation:{currentGeneration:'old',pendingGeneration:null},generation};
  const descriptorUrl='https://fixture.invalid/package.json',url='https://fixture.invalid/games/th08/b.ogg';
  const aborted={url,status:200,contentLength:32,signalBefore:false,signalAfter:true,prefix:{bytes:4,sha256:'e'.repeat(64)},bodyError:{name:'AbortError',message:'aborted'}};
  const e={descriptor:{path,bytes:bytes.length,sha256:sha256(bytes)},transport:{url,bytes:32,sha256:'b'.repeat(64),holdAfterBytes:4,holdMs:30_000,prefixSha256:'e'.repeat(64)},
    probe:{schema:'eagler-touhou/interrupted-package-download/1',game:'th08',descriptor,descriptorUrl,before,after:structuredClone(before),
      progress:[{fileId:'a',found:true}],requests:[{url:'https://fixture.invalid/games/th08/a.ogg'},aborted],abortedRequest:aborted,error:{name:'AbortError',message:'aborted'},
      staged:{current:{...structuredClone(before),installation:{currentGeneration:'old',pendingGeneration:'pending'}},pending:{id:'pending',game:'th08',descriptor,files:{a:{objectId:'new-object'}}},fileId:'a',objectId:'new-object',bytes:12,sha256:'a'.repeat(64)},pendingAfter:null,stagedObjectAfter:null},
    pageErrors:[],httpErrors:[],consoleErrors:[],requestFailures:[{url,error:'net::ERR_ABORTED'}],
    oldReopen:{query:{gameGeneration:'old',runtimeEpoch:'1'},ready:{game:'th08'},first:{event:'first-frame',epoch:1},dataRead:{bytes:8,sha256:'d'.repeat(64)}}};
  const entry={id:'PACKAGE-03',complete:true,status:'passed',evidenceRefs:['/interruptedRemoteUpdate']},data=e.oldReopen.dataRead;
  const driver={schema:'eagler-touhou/offline-runtime-browser/1',scope:product,checks:[entry],runtimePackages:{data},
    interruptedRemoteUpdate:{...product,adaptationIds:['PACKAGE-03'],observation:e},
    packageUpdate:{retry:{status:'committed',oldGeneration:'old',atomicCurrentGeneration:'new',generation:{revision:'new-revision',pendingGeneration:null,data}}},
    onlineRuntimeAfterUpdate:{query:{gameGeneration:'new'},first:{event:'first-frame'}}};
  return{e,driver,entry,product};
}
test('staged HTTP abort requires a verified new object, rollback, actual browser failure and both Runtime generations',t=>{
  const args=fixture(t);assertInterruptedPackageDownload(args.e);assert.equal(assessPackageUpdateInterruptionEvidence(args).valid,true);
});
for(const [name,mutate] of Object.entries({
  'no object staged':a=>delete a.e.probe.staged,
  'old file reused':a=>a.e.probe.before.generation.files.old={objectId:'new-object'},
  'bad staged bytes':a=>a.e.probe.staged.sha256='c'.repeat(64),
  'premature commit':a=>a.e.probe.after.installation.currentGeneration='pending',
  'pending leaked':a=>a.e.probe.pendingAfter={},
  'object leaked':a=>a.e.probe.stagedObjectAfter={},
  'fake fetch rejection':a=>a.e.probe.abortedRequest.status=0,
  'completed response':a=>a.e.probe.abortedRequest.bodyCompleted=true,
  'no received body':a=>a.e.probe.abortedRequest.prefix.bytes=0,
  'wrong received prefix':a=>a.e.probe.abortedRequest.prefix.sha256='f'.repeat(64),
  'no browser abort':a=>a.e.requestFailures=[],
  'unrelated abort':a=>a.e.requestFailures[0].url+='?other',
  'unrelated error':a=>a.e.pageErrors.push('unexpected'),
  'wrong transfer content':a=>a.e.transport.sha256='c'.repeat(64),
  'wrong artifact':a=>writeFileSync(a.e.descriptor.path,'{}'),
  'old Runtime missing':a=>a.e.oldReopen.first=null,
  'wrong DATA':a=>a.e.oldReopen.dataRead={bytes:9,sha256:'e'.repeat(64)},
  'retry not committed':a=>a.driver.packageUpdate.retry.status='pending',
  'new Runtime old generation':a=>a.driver.onlineRuntimeAfterUpdate.query.gameGeneration='old',
  'wrong mode':a=>a.product={...product,variant:'multiplayer'},
}))test(`package interruption rejects ${name}`,t=>{const args=fixture(t);mutate(args);assert.equal(assessPackageUpdateInterruptionEvidence(args).valid,false);});
