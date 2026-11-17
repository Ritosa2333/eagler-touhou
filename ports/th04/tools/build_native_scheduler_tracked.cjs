const fs=require('node:fs'),path=require('node:path'),{spawnSync}=require('node:child_process');
const root=path.resolve(__dirname,'..'),stem='build/native-scheduler',runtime=path.join(root,stem,'runtime');
const env={...process.env,BINARYEN_CORES:'2',CORE_INPUT:stem+'/runtime/wdosbox-x-rollback.wasm',CORE_OUTPUT:stem+'/tracked-native.wasm',CORE_PAGES_OUTPUT:stem+'/tracked-pages.json',DIRTY_SOURCE:stem+'/runtime/dosbox-rollback.mjs',DIRTY_RUNTIME_DIR:stem+'/tracked-runtime',DIRTY_PAGES:stem+'/tracked-pages.json'};
function run(file,args){const r=spawnSync(file,args,{cwd:root,env,encoding:'utf8',maxBuffer:4*1024*1024});if(r.error||r.status)throw r.error||Error(r.stderr||r.stdout);console.log(r.stdout.trim());}
run(process.execPath,[path.join(__dirname,'build_dirty_core.mjs')]);
run(path.join(root,'build/binaryen-native-132/binaryen-version_132/bin/wasm-opt.exe'),[stem+'/tracked-native.wasm','--all-features','--inlining','--optimize-instructions','--vacuum','-o',stem+'/tracked-inline.wasm']);
run(process.execPath,[path.join(__dirname,'prepare_core_candidate.cjs'),stem+'/tracked-inline.wasm',stem+'/runtime/wdosbox-x-rollback-tracked.wasm']);
run(process.execPath,[path.join(__dirname,'prepare_dirty_runtime.cjs')]);
fs.copyFileSync(path.join(root,stem,'tracked-runtime/dosbox-rollback.mjs'),path.join(runtime,'dosbox-rollback-tracked.mjs'));
fs.copyFileSync(path.join(root,stem,'tracked-runtime/snapshot-diff.wasm'),path.join(runtime,'snapshot-diff-tracked.wasm'));
console.log('Prepared isolated tracked native scheduler runtime');
