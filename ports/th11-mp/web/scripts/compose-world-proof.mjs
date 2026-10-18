// Compose contiguous native evidence only after the rebuilt core has matched
// every compared prefix field, including the saved boundary state.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';import {createHash} from 'node:crypto';
const root=fileURLToPath(new URL('../',import.meta.url)),dir=resolve(root,'artifacts/cpp/verification');
const sha=b=>createHash('sha256').update(b).digest('hex'),read=p=>JSON.parse(readFileSync(p));
const prefixFile=resolve(dir,'world-prefix-equivalence.json'),tailFile=resolve(dir,'world-replay-external-campaign-tail.json');
const prefix=read(prefixFile),tail=read(tailFile),target=read(resolve(root,'target.json')),current=sha(readFileSync(resolve(root,'artifacts/cpp/game-core-test.wasm')));
const checkpoint=prefix.checkpoint,metadata=read(resolve(checkpoint.path,'state.json'));
for(const [name,r]of [['prefix',prefix],['tail',tail]]){assert.equal(r.passed,true,name);assert.equal(r.coreSha256,current,name+' current core');assert.equal(r.target.sha256,target.sha256,name+' original executable');}
assert.equal(prefix.replaySha256,tail.replaySha256);assert.equal(prefix.replaySha256,metadata.identity.replaySha256);
assert.equal(prefix.baselineCoreSha256,metadata.identity.coreSha256);assert.equal(metadata.identity.targetSha256,target.sha256);assert.equal(metadata.identity.stageDrawClocks,1);
for(const [name,key]of [['state.json','metadataSha256'],['native.gz','nativeSha256'],['cpp.gz','cppSha256']])assert.equal(sha(readFileSync(resolve(checkpoint.path,name))),checkpoint[key],'unchanged native prefix checkpoint '+name);
assert.equal(metadata.nextFrame,prefix.checks);assert.equal(metadata.checks,prefix.checks);assert.equal(metadata.activeTicks,prefix.checks);assert.equal(metadata.firstInactiveFrame,null);
assert.equal(tail.nativeResumeOnly,true);assert.equal(tail.stageDrawClocks,true);assert.equal(tail.startFrame,prefix.checks,'contiguous boundary');assert.equal(tail.finalState.phase,4);assert.equal(tail.checks+tail.startFrame,tail.firstInactiveFrame+1);assert.equal(tail.activeTicks+1,tail.checks);
for(const key of ['metadataSha256','nativeSha256'])assert.equal(tail.nativeCheckpoint[key],checkpoint[key],'same original boundary '+key);
const checks=prefix.checks+tail.checks;
const result={target,coreSha256:current,passed:true,completed:true,nativeResumeOnly:false,stageDrawClocks:true,verificationMode:'cached-native-prefix-with-complete-cpp-equivalence-and-contiguous-native-tail',replaySha256:prefix.replaySha256,checks,frames:checks,activeTicks:metadata.activeTicks+tail.activeTicks,firstInactiveFrame:tail.firstInactiveFrame,finalState:tail.finalState,selection:tail.selection,segments:[{start:0,end:prefix.checks,kind:'native prefix + all compared fields equivalent in current C++',report:'world-prefix-equivalence.json',sha256:sha(readFileSync(prefixFile))},{start:tail.startFrame,end:checks,kind:'current C++ versus original native tail',report:'world-replay-external-campaign-tail.json',sha256:sha(readFileSync(tailFile))}],scope:'Complete 100,672-frame sample coverage by composition, not a newly rerun native prefix: original-machine prefix checkpoint, every original-comparison field checked against current C++ at each prefix frame and at the checkpoint, then contiguous current-C++/original-machine tail through the terminal reward. No unverified frame gap. Coverage is the selected world fields documented in the component reports; it is not a pixel/audio or all-input proof.'};
writeFileSync(resolve(dir,'world-replay-external-campaign.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify({passed:true,checks,prefix:prefix.checks,tail:tail.checks,finalState:tail.finalState}));
