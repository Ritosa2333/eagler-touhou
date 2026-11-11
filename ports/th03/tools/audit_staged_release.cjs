// Verify exact Git blobs, not worktree bytes subject to checkout conversion.
const fs=require('node:fs'),path=require('node:path'),crypto=require('node:crypto');
const {spawnSync}=require('node:child_process'),assert=require('node:assert/strict');
const root=path.resolve(__dirname,'..'),sha=b=>crypto.createHash('sha256').update(b).digest('hex');
const revision=process.argv[2]||'';
function read(name){
 const result=spawnSync('git',['show',revision+':'+name],{cwd:root,maxBuffer:64*1024*1024});
 if(result.status!==0)throw Error('Missing Git resource '+name+': '+result.stderr);
 return result.stdout;
}
const network=JSON.parse(read('web/lockstep-runtime.json'));
const vendor=JSON.parse(read('web/vendor/dosbox-x/runtime.json'));
const build=JSON.parse(read('web/vendor/dosbox-x/core-build.json'));
const checked=new Set();
function verify(name,hash){assert.equal(sha(read(name)),hash,'Git resource fingerprint '+name);checked.add(name);}
for(const [name,hash]of Object.entries(network.files))verify('web/'+name,hash);
for(const [name,hash]of Object.entries(vendor.files))verify('web/vendor/dosbox-x/'+name,hash);
for(const [name,hash]of Object.entries({...build.localRuntimeFiles,...build.files}))verify('web/vendor/dosbox-x/'+name,hash);
for(const [name,hash]of Object.entries(build.behaviorFiles))verify('web/'+name,hash);
assert.equal(network.cpu_cycles,16000);assert.equal(network.snapshotStride,2);
assert.equal(network.snapshotScan,'adaptive-word8');
const report={gitRevision:revision||'index',checkedResources:checked.size,allGitBlobHashesMatch:true,
 sourceWasm:vendor.sourceWasm,snapshotScan:network.snapshotScan,gameplayRun:false};
fs.mkdirSync(path.join(root,'reports'),{recursive:true});
fs.writeFileSync(path.join(root,'reports/th03-main-git-blobs-20261015.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify(report,null,2));
