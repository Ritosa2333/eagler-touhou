import {fork,spawn} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,existsSync,statSync} from 'node:fs';
import {createHash,randomUUID} from 'node:crypto';
import {createServer} from 'node:net';
import {resolve} from 'node:path';
const cliArgs=process.argv.slice(2);
const cliFlags=new Set(['--fixtures','--spectator-stage-two-controls','--spectator-first-diff-only',
 '--spectator-old-viewer-cross-stage','--spectator-lifecycle-reentry','--fresh-touch','--background-index','--bomb-lazy',
 '--bomb-lifecycle','--shot-sparse','--record-sparse','--texture-coalesce','--world-instancing',
 '--skip-resim-visual','--skip-resim-geometry','--title-spells-lazy','--projection-reuse',
 '--back-metadata-only','--analog']);
const cliValues=['--only=','--players=','--input-delay=','--adonis-mode=','--replay-frames=','--spectator-frames='];
const unknownOptions=cliArgs.filter(arg=>arg.startsWith('--')&&!cliFlags.has(arg)&&!cliValues.some(prefix=>arg.startsWith(prefix)));
if(unknownOptions.length)throw Error('Unknown option(s): '+unknownOptions.join(', '));
const root=resolve(import.meta.dirname,'../..'),fixtures=process.argv.includes('--fixtures');
const profile=fixtures?'multiplayer-fixtures':'multiplayer';
const buildRoot=resolve(process.env.TH08_MP_BUILD_ROOT||resolve(root,'th08_web/artifacts',profile)),
 build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
const hash=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const verify=()=>{if(build.variant!==profile||hash(resolve(buildRoot,'th08-sdl.wasm'))!==build.sha256)throw Error('Mismatched build');
 for(const [name,sha] of Object.entries(build.sourceFiles))if(hash(resolve(root,name))!==sha)throw Error('Stale source: '+name);};
verify();
const only=process.argv.find(s=>s.startsWith('--only='))?.slice(7);
const choices=['packets','rtc','relay','host','replay','spectator-rtc','spectator-relay'];
const modes=only?only.split(','):['packets','rtc','relay'];
if(!modes.length||modes.some(s=>!choices.includes(s)))throw Error('No/unknown cases');
const hostMode=modes.some(m=>m==='host'||m==='replay'||m.startsWith('spectator-'));
const count=process.argv.find(s=>s.startsWith('--players='))?.slice(10)||'all';
const spectatorStageTwo=process.argv.includes('--spectator-stage-two-controls');
const spectatorFirstDiffOnly=process.argv.includes('--spectator-first-diff-only');
const spectatorOldViewerCrossStage=process.argv.includes('--spectator-old-viewer-cross-stage');
const spectatorLifecycleReentry=process.argv.includes('--spectator-lifecycle-reentry');
const spectatorFrames=process.argv.find(s=>s.startsWith('--spectator-frames='))?.slice(19);
if(spectatorStageTwo&&!fixtures)throw Error('Stage-two spectator controls require the fixture-enabled TH08 native test build');
if(spectatorStageTwo&&!modes.includes('spectator-relay'))throw Error('Stage-two spectator controls require --only=spectator-relay');
if(spectatorFirstDiffOnly&&(!spectatorStageTwo||!fixtures))throw Error('First-difference probe requires --spectator-stage-two-controls and a fixture-enabled build');
if(spectatorOldViewerCrossStage&&(!fixtures||spectatorStageTwo||!modes.includes('spectator-relay')))throw Error('Old-viewer cross-stage proof requires a fixture-enabled standalone spectator-relay case');
if(spectatorLifecycleReentry&&(count!=='2'||spectatorStageTwo||spectatorOldViewerCrossStage||
 !modes.includes('spectator-relay')||modes.length!==1))throw Error('Lifecycle reentry requires a standalone two-player spectator-relay case');
const inputDelay=Number(process.argv.find(s=>s.startsWith('--input-delay='))?.slice(14)||0);
const adonisMode=Number(process.argv.find(s=>s.startsWith('--adonis-mode='))?.slice(14)||0);
if(!Number.isInteger(adonisMode)||adonisMode<0||adonisMode>2)throw Error('Invalid Adonis mode');
if(!Number.isInteger(inputDelay)||inputDelay<0||inputDelay>(adonisMode?9:8))throw Error('Invalid input delay');
if((inputDelay||adonisMode)&&modes.some(mode=>mode!=='host'))throw Error('Input-delay/Adonis acceptance currently requires --only=host');
const replayFrames=process.argv.find(s=>s.startsWith('--replay-frames='))?.slice(16);
const freshTouch=process.argv.includes('--fresh-touch');
const backgroundIndex=process.argv.includes('--background-index');
const bombLazy=process.argv.includes('--bomb-lazy');
const bombLifecycle=process.argv.includes('--bomb-lifecycle');
const shotSparse=process.argv.includes('--shot-sparse');
const recordSparse=process.argv.includes('--record-sparse');
const textureCoalesce=process.argv.includes('--texture-coalesce');
const worldInstancing=process.argv.includes('--world-instancing');
const skipResimVisual=process.argv.includes('--skip-resim-visual');
const skipResimGeometry=process.argv.includes('--skip-resim-geometry');
const titleSpellsLazy=process.argv.includes('--title-spells-lazy');
const projectionReuse=process.argv.includes('--projection-reuse');
const backMetadataOnly=process.argv.includes('--back-metadata-only');
const analog=process.argv.includes('--analog')||freshTouch;
if(freshTouch&&hostMode)throw Error('--fresh-touch selects packet/RTC/relay tests; Replay includes its own mixed input formats');
const spectatorCase=spectatorLifecycleReentry?'lifecycle-reentry':spectatorOldViewerCrossStage?'old-viewer-cross-stage':spectatorStageTwo?'stage-two-controls':'default';
const spectatorExpectedPhase=spectatorLifecycleReentry?'repeated-runtime-admission-and-idle-pause':
 spectatorOldViewerCrossStage?'old-viewer-stage-one-admission':
 spectatorStageTwo?'stage-two-late-admission':modes.some(m=>m.startsWith('spectator-'))?'delayed-admission':null;
const browserBlocked=new Set([2049,3659,4045,5060,5061,6000,6566,6665,6666,6667,6668,6669,6697,10080]);
const free=async()=>{for(let tries=0;tries<32;++tries){const p=await new Promise((ok,fail)=>{const s=createServer();s.once('error',fail);s.listen(0,'127.0.0.1',()=>{const port=s.address().port;s.close(e=>e?fail(e):ok(port));});});if(p>=1024&&!browserBlocked.has(p))return p;}throw Error('No browser-safe loopback port');};
const out=resolve(root,'artifacts/multiplayer-tests/network-'+randomUUID());mkdirSync(out,{recursive:true});
const report={passed:false,profile,analog,freshTouch,adonisMode,inputDelay,wasm:build.sha256,
 spectatorRequest:{case:spectatorCase,stageTwoControls:spectatorStageTwo,
  oldViewerCrossStage:spectatorOldViewerCrossStage,firstDiffOnly:spectatorFirstDiffOnly,
  lifecycleReentry:spectatorLifecycleReentry,frames:spectatorFrames?Number(spectatorFrames):null,
  expectedPhase:spectatorExpectedPhase},harness:{},cases:[]};
if(process.env.TH08_MP_DATA&&existsSync(process.env.TH08_MP_DATA))report.inputData={path:process.env.TH08_MP_DATA,bytes:statSync(process.env.TH08_MP_DATA).size,sha256:hash(process.env.TH08_MP_DATA)};
for(const file of ['check-network.mjs','check-network.py','smoke.mjs','serve.mjs'])report.harness[file]=hash(resolve(import.meta.dirname,file));
if(hostMode)for(const file of ['check-runtime-host.py','runtime-host.html','host-resources.json'])report.harness[file]=hash(resolve(import.meta.dirname,file));
if(modes.some(m=>m.startsWith('spectator-')))report.harness['check-spectator.py']=hash(resolve(import.meta.dirname,'check-spectator.py'));
if(modes.includes('replay'))report.harness['check-replay.py']=hash(resolve(import.meta.dirname,'check-replay.py'));
if(hostMode)for(const file of ['th08.html','shell.mjs','startup-branding.mjs','eagler-host.mjs','save-storage.mjs','multiplayer-host.mjs','practice.mjs','practice-config.mjs','practice-sections.mjs']){
 const key='../../th08_web/sdl-runtime/'+file;report.harness[key]=hash(resolve(import.meta.dirname,key));
}
let server,relay;
const children=[];
const ready=(child,pattern)=>new Promise((done,reject)=>{const t=setTimeout(()=>reject(Error('Server timeout')),15000);
 child.once('error',reject);child.once('exit',c=>reject(Error('Server exit '+c)));
 child.stdout.on('data',b=>{process.stdout.write(b);if(pattern&&String(b).includes(pattern)){clearTimeout(t);done();}});
 child.stderr.on('data',b=>process.stderr.write(b));
 child.on('message',m=>{if(!pattern&&m?.type==='ready'){clearTimeout(t);done();}});});
try{
 console.log('TH08 spectator request '+JSON.stringify(report.spectatorRequest));
 const port=await free(),relayPort=await free();
 if(modes.some(m=>m!=='packets')){
  const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../eagler-multiplayer'));
  const file=resolve(launcher,'server/netplay-relay.mjs');if(!existsSync(file))throw Error('Relay missing');
  report.relay={path:file,sha256:hash(file)};
  relay=fork(file,[],{cwd:launcher,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(relayPort),EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe','ipc']});
  await ready(relay,'relay listening');
 }
 server=fork(resolve(import.meta.dirname,'serve.mjs'),[],{cwd:root,env:{...process.env,PORT:String(port),TH08_MP_FIXTURES:fixtures?'1':'0'},stdio:['ignore','pipe','pipe','ipc']});await ready(server);
 for(const mode of modes){
  const output=resolve(out,mode+'.json');console.log('BEGIN TH08 '+mode+' players='+count);
  const code=await new Promise((done,reject)=>{
   const c=spawn(process.env.TH_PYTHON||'python',['-u',resolve(import.meta.dirname,mode==='host'?'check-runtime-host.py':mode==='replay'?'check-replay.py':mode.startsWith('spectator-')?'check-spectator.py':'check-network.py'),'--url','http://127.0.0.1:'+port+'/','--relay','ws://127.0.0.1:'+relayPort,'--mode',mode,'--players',count,'--output',output,...(mode==='host'?['--input-delay',String(inputDelay),'--adonis-mode',String(adonisMode)]:[]),...(spectatorStageTwo&&mode.startsWith('spectator-')?['--stage-two-controls','--frames',spectatorFrames||'900']:[]),...(spectatorFirstDiffOnly&&mode.startsWith('spectator-')?['--first-diff-only']:[]),...(spectatorOldViewerCrossStage&&mode.startsWith('spectator-')?['--old-viewer-cross-stage']:[]),...(spectatorLifecycleReentry&&mode.startsWith('spectator-')?['--lifecycle-reentry']:[]),...(analog?['--analog']:[]),...(freshTouch?['--fresh-touch']:[]),...(backgroundIndex&&['packets','rtc','relay'].includes(mode)?['--background-index']:[]),...(bombLazy&&['packets','rtc','relay'].includes(mode)?['--bomb-lazy']:[]),...(shotSparse&&['packets','rtc','relay'].includes(mode)?['--shot-sparse']:[]),...(recordSparse&&['packets','rtc','relay'].includes(mode)?['--record-sparse']:[]),...(titleSpellsLazy&&['packets','rtc','relay'].includes(mode)?['--title-spells-lazy']:[]),...(projectionReuse&&mode==='packets'?['--projection-reuse']:[]),...(textureCoalesce&&mode==='packets'?['--texture-coalesce']:[]),...(backMetadataOnly&&mode==='packets'?['--back-metadata-only']:[]),...(worldInstancing&&mode==='packets'?['--world-instancing']:[]),...(skipResimVisual&&mode==='packets'?['--skip-resim-visual']:[]),...(skipResimGeometry&&mode==='packets'?['--skip-resim-geometry']:[]),...(bombLifecycle&&mode==='packets'?['--bomb-lifecycle']:[]),...(mode==='replay'&&replayFrames?['--frames',replayFrames]:[])],{cwd:root,stdio:['ignore','pipe','pipe'],windowsHide:true});
   children.push(c);c.stdout.on('data',b=>process.stdout.write(b));c.stderr.on('data',b=>process.stderr.write(b));c.once('error',reject);c.once('exit',done);
  });
  const produced=JSON.parse(readFileSync(output));
  if(mode.startsWith('spectator-'))for(const result of produced.cases||[]){
   if(result.requestedCase!==spectatorCase||result.expectedPhase!==spectatorExpectedPhase)
    throw Error('Spectator branch mismatch: requested '+spectatorCase+'/'+spectatorExpectedPhase+
      ', report '+result.requestedCase+'/'+result.expectedPhase+' actual phase '+result.phase);
  }
  report.cases.push({mode,code,report:output,requestedCase:spectatorCase,expectedPhase:spectatorExpectedPhase,
   actualPhases:(produced.cases||[]).map(result=>result.phase)});
  writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2));
  if(code!==0||!produced.passed)throw Error('Failed '+mode);
 }
 verify();for(const [name,sha] of Object.entries(report.harness))if(hash(resolve(import.meta.dirname,name))!==sha)throw Error('Harness changed: '+name);
 report.passed=true;
}catch(e){report.error=String(e);process.exitCode=1;}
finally{for(const c of children)if(c.exitCode===null)c.kill();server?.kill();relay?.kill();writeFileSync(resolve(out,'suite.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({passed:report.passed,report:resolve(out,'suite.json'),error:report.error}));}
