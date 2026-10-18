import {spawnSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,copyFileSync,openSync,closeSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';

const root=resolve(import.meta.dirname,'../..'),phase=process.argv[2],label=process.env.RUN_LABEL;
if(!label||!/^[A-Za-z0-9_-]+$/.test(label))throw Error('Set unique RUN_LABEL');
const out=resolve(root,'artifacts/rollback-journal-4x',label);
const sha=value=>createHash('sha256').update(value).digest('hex');
const json=path=>JSON.parse(readFileSync(path,'utf8'));
const write=(path,value)=>writeFileSync(path,JSON.stringify(value,null,2)+'\n');
const command=(executable,args,cwd,logPath,timeout=585000)=>{
  const fd=openSync(logPath,'wx'),started=Date.now();
  const result=spawnSync(executable,args,{cwd,stdio:['ignore',fd,fd],timeout});
  closeSync(fd);
  const receipt={executable,args,cwd,elapsedMs:Date.now()-started,status:result.status,error:result.error?.message};
  write(logPath+'.process.json',receipt);
  if(result.status!==0||result.error)throw Error('Command failed; see '+logPath);
  return receipt;
};
const gitHead=cwd=>{
  const result=spawnSync('git',['rev-parse','HEAD'],{cwd,encoding:'utf8'});
  if(result.status)throw Error(result.stderr);return result.stdout.trim();
};
const commonArgs=data=>['--data',data,'--production','--frames','1200','--players','2',
  '--cpu-rate','4','--slow-seats','0,1','--bullets','0','--one-way-ms','79','--jitter-ms','5',
  '--modes','frontier','--rounds','1','--timeout','180','--browser-channel','chrome',
  '--headed','--native-gpu','--wall-clock-touch','--touch-hz','120','--touch-seats','0,1',
  '--bomb-lazy','--shot-sparse','--record-sparse','--skip-resim-visual','--skip-resim-geometry',
  '--back-metadata-only'];
const phases=['A1','B1','B2','A2','spike-A','spike-B'];
if(phase==='freeze'){
  if(existsSync(out))throw Error('Experiment exists');mkdirSync(out,{recursive:true});
  if(!process.env.BASELINE_ROOT||!process.env.TH08_MP_DATA)throw Error('Set BASELINE_ROOT and TH08_MP_DATA');
  const baseline=resolve(process.env.BASELINE_ROOT),data=resolve(process.env.TH08_MP_DATA),python=process.env.TH_PYTHON||'python';
  if(baseline===root)throw Error('Baseline must be an independent tree');
  const baselineHead=gitHead(baseline),candidateHead=gitHead(root);
  if(process.env.BASELINE_COMMIT&&baselineHead!==process.env.BASELINE_COMMIT)throw Error('Wrong baseline HEAD');
  // These are test-only files in the independent reference checkout.
  for(const name of ['measure-smoothness.py','performance-hook.mjs'])
    copyFileSync(resolve(root,'portable/multiplayer',name),resolve(baseline,'portable/multiplayer',name));
  const protocol={label,baseline,root,baselineHead,candidateHead,python,data,
    runnerSha256:sha(readFileSync(import.meta.filename)),phases,commonArgs:commonArgs(data),
    purpose:'Formal serial A/B/B/A; optional fixed network spike A/B after the stable pair.',
    conditions:{cpuRate:4,slowSeats:[0,1],touchHzTarget:120,touchSeats:[0,1],frames:1200,
      measureFrom:240,sendDelayMs:79,jitterMs:5,history:8,localInputDelay:0,
      route:'RTC application-send impairment; local signaling relay; not wire RTT',
      profiling:false,audio:'music off; actual audio output not accepted'},
    boundaries:['Body starts at first observed frame >= 240, not necessarily exactly 240.',
      'Rows stop at 1200; final confirmation/reconcile tail is outside timing.',
      'Touch tapes are wall-clock submissions and differ across runs.',
      'Native callback wall time excludes rAF scheduling wait and observer tail.',
      'Present/advance gaps use observer times and omit the first anchor interval.']};
  write(resolve(out,'protocol.json'),protocol);
  for(const [side,cwd] of [['A',baseline],['B',root]]){
    command(python,[resolve(cwd,'portable/multiplayer/measure-smoothness.py'),
      '--output',resolve(out,'freeze-'+side),'--freeze-only',...protocol.commonArgs],
      cwd,resolve(out,'freeze-'+side+'.log'));
  }
  const a=json(resolve(out,'freeze-A/manifest.json')),b=json(resolve(out,'freeze-B/manifest.json'));
  const binaries=new Set(['th08-sdl.wasm','th08-sdl.mjs','runtime-build.json','build.json']);
  const keys=Object.keys(a).sort();
  if(JSON.stringify(keys)!==JSON.stringify(Object.keys(b).sort()))throw Error('Manifest file sets differ');
  const differences=keys.filter(name=>a[name]!==b[name]);
  if(differences.some(name=>!binaries.has(name)))throw Error('A/B non-build inputs differ: '+differences.join(', '));
  if(a['th08-sdl.wasm']===b['th08-sdl.wasm'])throw Error('A/B WASM unexpectedly identical');
  protocol.manifestFileCount=keys.length;protocol.nonBuildInputsEqual=true;protocol.differences=differences;
  protocol.wasm={A:a['th08-sdl.wasm'],B:b['th08-sdl.wasm']};
  protocol.manifestSha256={A:sha(readFileSync(resolve(out,'freeze-A/manifest.json'))),
    B:sha(readFileSync(resolve(out,'freeze-B/manifest.json')))};
  write(resolve(out,'protocol.json'),protocol);console.log(JSON.stringify(protocol));process.exit(0);
}
if(!phases.includes(phase))throw Error('Choose freeze, '+phases.join(', '));
const protocol=json(resolve(out,'protocol.json'));
if(sha(readFileSync(import.meta.filename))!==protocol.runnerSha256||!protocol.nonBuildInputsEqual)
  throw Error('Runner or frozen protocol changed');
const index=phases.indexOf(phase);
for(const previous of phases.slice(0,index)){
  const prior=json(resolve(out,previous,'1-frontier.json'));
  const qualified=json(resolve(out,previous+'.qualified.json'));
  if(!prior.passed||!qualified.passed)throw Error('Previous phase failed: '+previous);
}
const side=phase.includes('A')?'A':'B',target=resolve(out,phase);
if(existsSync(target))throw Error('Run already exists: '+phase);
const args=[resolve(root,'portable/multiplayer/measure-smoothness.py'),'--output',target,
  '--reuse-freeze',resolve(out,'freeze-'+side),...protocol.commonArgs];
if(phase.startsWith('spike-'))args.push('--spike-frame','450','--spike-ms','800','--spike-delay-ms','800','--spike-seat','1');
const receipt=command(protocol.python,args,root,resolve(out,phase+'.log'));
const report=json(resolve(target,'1-frontier.json')),teardown=json(resolve(target,'teardown.json'));
if(!report.passed||report.teardown?.browsersClosed!==2||report.teardown.errors.length||
   !teardown.serverThreadStopped||teardown.relayForced)throw Error('Run/cleanup qualification failed: '+phase);
write(resolve(out,phase+'.qualified.json'),{passed:true,phase,side,wasm:protocol.wasm[side],
  rawSha256:sha(readFileSync(resolve(target,'1-frontier.json'))),teardown,
  browserTeardown:report.teardown});
console.log(JSON.stringify({phase,side,wasm:protocol.wasm[side],elapsedMs:receipt.elapsedMs,passed:true,
  peers:report.summary.map((s,i)=>({seat:i,frames:s.frames,logicalFps:s.logicalFps,
    callbackMsPerLogicalTick:s.callbackMsPerLogicalTick,callbackMs:s.callbackMs,
    presentGapMs:s.presentGapMs,advanceGapMs:s.advanceGapMs,corrections:s.corrections,
    resimulated:s.resimulated,touchEvents:report.peers[i].touch.events,
    touchCoalesced:report.peers[i].touch.coalesced}))}));

