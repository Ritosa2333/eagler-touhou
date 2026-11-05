// Read-only acceptance audit. A partial checkpoint run cannot satisfy the
// complete-campaign gate, and old reports cannot validate a changed core.
import assert from 'node:assert/strict';
import {readFileSync,readdirSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const root=fileURLToPath(new URL('../',import.meta.url));
const json=p=>JSON.parse(readFileSync(resolve(root,p),'utf8'));
const sha=p=>createHash('sha256').update(readFileSync(resolve(root,p))).digest('hex');
const coreSha=sha('artifacts/cpp/game-core-test.wasm'),target=json('target.json');
const current=(r,label)=>{assert.equal(r.passed,true,label+' passed');assert.equal(r.coreSha256,coreSha,label+' core');};
const coreBuild=json('artifacts/cpp/game-core-test-build.json');
assert.equal(coreBuild.sha256,coreSha);
for(const [p,hash]of Object.entries(coreBuild.sourceHashes))assert.equal(sha(p),hash,'stale core '+p);
const equivalence=json('artifacts/cpp/verification/gameplay-core-equivalence.json');
function nativeProof(name,complete){
 const r=json('artifacts/cpp/verification/'+name+'.json');assert.equal(r.passed,true,name);
 assert.equal(r.target.sha256,target.sha256,name+' original executable');
 assert.ok(!r.nativeResumeOnly,name+' must not be a diagnostic tail');
 assert.equal(r.stageDrawClocks,true,name+' original draw clocks');
 if(r.segments){let boundary=0;for(const segment of r.segments){assert.equal(segment.start,boundary,name+' contiguous evidence');assert.equal(sha('artifacts/cpp/verification/'+segment.report),segment.sha256,name+' unchanged component report');boundary=segment.end;}assert.equal(boundary,r.checks,name+' all frames covered');}
 if(r.coreSha256!==coreSha){current(equivalence,'equivalence');assert.equal(equivalence.baselineCoreSha256,r.coreSha256);assert.ok(equivalence.cases.some(x=>x.replaySha256===r.replaySha256&&x.checks===r.checks),name+' whole-replay equivalence');}
 if(complete){assert.equal(r.completed,true,name+' completed');assert.equal(r.finalState.phase,4);assert.equal(r.firstInactiveFrame+1,r.checks);assert.equal(r.stageDrawClocks,true);}
 else assert.equal(r.checks,r.frames,name+' every requested frame');
 return {name,checks:r.checks,coreSha256:r.coreSha256,finalState:r.finalState};
}
const native=[nativeProof('world-replay-external-campaign',true),nativeProof('world-replay-external-extra',true)];
for(let i=0;i<4;++i)native.push(nativeProof('world-replay-demo'+i,false));
const required=['damage-protection','enemy-frame','battle-integration','shot-damage','spell-controller','stage-transition','session-replays','campaign-recording','extra-recording','external-campaign','external-extra','restart-session','frame-statistics'];
for(const name of required)current(json('artifacts/cpp/verification/'+name+'.json'),name);
const modules=[];
for(const name of readdirSync(resolve(root,'artifacts/cpp/verification')).filter(n=>n.endsWith('.json'))){const r=json('artifacts/cpp/verification/'+name);if(r.passed&&r.coreSha256===coreSha)modules.push(name);}
const dev=json('artifacts/sdl3/build.json'),release=json('artifacts/sdl-release/build.json');
for(const [name,b]of [['development',dev],['release',release]])for(const [p,hash]of Object.entries({...b.sources,...b.headers}))assert.equal(sha('../'+p),hash,'stale '+name+' '+p);
for(const path of ['audio/campaign.json','audio/extra.json','persistence/report.json']){const r=json('artifacts/sdl3/browser/'+path);assert.equal(r.passed,true,path);assert.equal(r.wasmSha256,dev.sha256,path+' SDL core');}
const preview=json('artifacts/'+(process.argv.includes('--final')?'sdl-release':'architecture-preview')+'/site/manifest.json'),launcher=json('artifacts/sdl3/browser/launcher/report.json');
assert.equal(preview.execution.sha256,release.sha256);assert.equal(launcher.passed,true);assert.equal(launcher.build,preview.version,'launcher build');
const result={passed:true,checkedAt:new Date().toISOString(),coreSha256:coreSha,releaseSha256:release.sha256,native,modules,launcherBuild:preview.version,physicalDevice:false,scope:'Implementation acceptance on fixed original samples and desktop browsers. Does not assert all inputs are bug-free or physical-phone performance.'};
mkdirSync(resolve(root,'artifacts/sdl-release/verification'),{recursive:true});writeFileSync(resolve(root,'artifacts/sdl-release/verification/acceptance.json'),JSON.stringify(result,null,2)+'\n');
console.log(JSON.stringify({passed:true,coreSha256:coreSha,releaseSha256:release.sha256,nativeChecks:native.reduce((n,r)=>n+r.checks,0),moduleReports:modules.length}));
