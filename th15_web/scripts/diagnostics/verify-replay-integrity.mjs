// Candidate-to-candidate Replay integrity. This does not substitute for a retail golden trace.
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import assert from 'node:assert/strict';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
import {digestCanonicalJson,recordsFromSegments} from '../../../third_party/eagler-common/testkit/replay-verifier/adapter.mjs';
import {writeTraceJsonLines,readTraceJsonLines} from '../../../third_party/eagler-common/testkit/replay-verifier/trace-jsonl.mjs';
import {compareTraces} from '../../../third_party/eagler-common/testkit/replay-verifier/compare.mjs';

const root=fileURLToPath(new URL('../../',import.meta.url));
const dat=process.env.TH15_ORIGINAL_DAT,replay=process.env.TH15_VERIFY_REPLAY;
assert.ok(dat&&replay,'Set TH15_ORIGINAL_DAT and TH15_VERIFY_REPLAY');
const sha=b=>createHash('sha256').update(b).digest('hex');
const output=resolve(root,'artifacts/replay-verifier');await mkdir(output,{recursive:true});
const fonts=(await readdir(resolve(root,'assets/sdl-native/fonts'))).filter(n=>n.endsWith('.bin'));
const files=new Map([['/module.mjs',resolve(root,'artifacts/sdl-application/th15-application.mjs')],['/th15-application.wasm',resolve(root,'artifacts/sdl-application/th15-application.wasm')],['/th15.dat',dat],['/input.rpy',replay],...fonts.map(n=>['/fonts/'+n,resolve(root,'assets/sdl-native/fonts',n)])]);
const comparisonIdentity={game:'th15',profile:'candidate-presentation-integrity',stateSchema:'th15/replay-world-spell-pause/v1',traceCodec:'canonical-json/v1',digestAlgorithm:'sha256-truncated-128',replaySha256:sha(await readFile(replay)),executableSha256:JSON.parse(await readFile(resolve(root,'target.json'),'utf8')).sha256,resourceSha256:sha(await readFile(dat))};
const requiredCategories=['world-probe','replay-cursor-and-input','spell-clock','pause-lifecycle'];
const coverage={requiredCategories,optionalCategories:[]};
const limitations=['Candidate 60 Hz compared with candidate presentation at higher rates; original executable golden traces are unavailable locally.','World probes cover input, player coordinates/life, score/stocks/power, game RNG, enemy and bullet counts; complete object payloads and visual RNG are not covered.','Logical ticks use the real stopped browser runtime update/Draw; additional display submissions are injected at the specified cadence. Browser RAF wall timing and music decoding are separate tests.'];
const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas" width="1280" height="960"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas")});</script>');return;}if(!files.has(path)){res.writeHead(404).end();return;}res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':path.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(await readFile(files.get(path)));}catch(e){res.writeHead(500).end(String(e));}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
const captures=[];
try{
 browser=await launchBrowser({args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required']});
 for(const rate of [60,144,240]){
  const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(String(e)));
  await page.goto('http://127.0.0.1:'+server.address().port+'/');await page.waitForFunction(()=>window.fixture,null,{timeout:90000});
  await page.evaluate(async({fonts,rate})=>{
   const c=fixture;for(const dir of ['fonts','save','save/replay','music'])c.FS.mkdir('/'+dir);
   for(const name of fonts)c.FS.writeFile('/fonts/'+name,new Uint8Array(await(await fetch('/fonts/'+name)).arrayBuffer()));
   c.FS.writeFile('/th15.dat',new Uint8Array(await(await fetch('/th15.dat')).arrayBuffer()));c.FS.writeFile('/save/replay/th15_01.rpy',new Uint8Array(await(await fetch('/input.rpy')).arrayBuffer()));
   c._th15_music_enabled(0);c._th15_render_scale(2);
   window.read32=(pointer,n,unsigned=false)=>Array.from((unsigned?c.HEAPU32:c.HEAP32).subarray(pointer/4,pointer/4+n));
   window.state=()=>read32(c._th15_probe_state(),16);window.replayState=()=>read32(c._th15_probe_replay_state(),24);
   window.tick=()=>{if(!c._th15_probe_tick()){const p=c._th15_error();throw Error(new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p))));}};
   window.key=scan=>{c._th15_key(scan,1);tick();tick();c._th15_key(scan,0);tick();tick();};
   window.menu=(screen,phase=2)=>{for(let n=0;n<350;n++){if(state()[8]===screen&&state()[9]===phase)return;tick();}throw Error('Menu unavailable '+state());};
   if(!c._th15_initialize())throw Error('Initialize failed');menu(1);while(state()[10]!==3)key(208);key(28);menu(12);key(28);menu(12,4);for(let n=0;n<30;n++)tick();key(28);
   for(let n=0;state()[0]===0&&n<250;n++)tick();if(state()[0]!==1||!replayState()[0])throw Error('Replay did not start '+state());
   c._th15_probe_presentation_enable(+(rate>60));window.displayCredit=0;window.totalTicks=0;window.displaySubmissions=0;window.captureDone=false;window.firstRow=true;
  },{fonts,rate});
  const segments=[];let segment=null,ticks=0,done=false,metadata;
  while(!done&&ticks<200000){
   const batch=await page.evaluate(rate=>{const c=fixture,rows=[];c._sdl_defer(1);for(let n=0;n<120&&!captureDone;n++){
    if(firstRow)firstRow=false;else tick();const app=state(),r=replayState(),w=read32(c._th15_probe_world_state(),16,true),spell=read32(c._th15_probe_spell_state(),6),pause=read32(c._th15_probe_pause_state(),7);
    if(!r[0]||app[0]!==1)throw Error('Replay owner vanished before terminal marker '+app);
    if(rate>60){displayCredit+=rate/60;const count=Math.floor(displayCredit+1e-9);displayCredit-=count;for(let i=0;i<count;i++)if(c._th15_probe_presentation_draw((i+1)/count))displaySubmissions++;}
    const after=[read32(c._th15_probe_world_state(),16,true),replayState(),read32(c._th15_probe_spell_state(),6),read32(c._th15_probe_pause_state(),7)];
    if(JSON.stringify(after)!==JSON.stringify([w,r,spell,pause]))throw Error('Presentation changed Replay state at '+app[2]+'/'+app[5]);
    rows.push({app,w,r,spell,pause});totalTicks++;captureDone=!!r[4]||(pause[0]===1&&pause[1]===6&&pause[2]===1);
   }c._sdl_commit();c._sdl_defer(0);return {rows,done:captureDone,displaySubmissions};},rate);
   for(const row of batch.rows){
    const stage=row.app[2];if(!segment||segment.stage!==stage){segment={stage,route:'stage-'+stage,segmentId:'stage-'+stage,ticks:[],reason:'stage-transition',complete:true};segments.push(segment);}
    segment.ticks.push({replaySampleIndex:row.r[1],clockDisposition:row.r[4]?'terminal-marker':row.r[2]<0?'inactive':'active',appliedInput:{held:row.r[6],pressed:row.r[7],released:row.r[8],repeated:row.r[9]},clocks:{stageFrame:row.app[5],replayClock:row.r[2],recordedFps:row.r[3]},scalars:{stage,flags:row.app[6],modeFlags:row.app[12]},categories:{'world-probe':digestCanonicalJson(row.w),'replay-cursor-and-input':digestCanonicalJson(row.r),'spell-clock':digestCanonicalJson(row.spell),'pause-lifecycle':digestCanonicalJson(row.pause)}});
    ticks++;metadata=row.r;
   }
   done=batch.done;if(ticks%1200===0||done)console.log(JSON.stringify({rate,ticks,stage:segment?.stage,frame:batch.rows.at(-1)?.app[5],done}));
  }
  assert.ok(done,'Replay completion must be reached');assert.equal(segments.length,metadata[5],'All declared stages must play');segment.reason=metadata[4]?'terminal-marker':'game-completion-menu';
  const lifecycle=await page.evaluate(()=>{for(let n=0;n<25;n++)tick();const p=read32(fixture._th15_probe_pause_state(),7);if(p[1]!==6||p[2]!==1)throw Error('Replay completion menu missing '+p);key(28);for(let n=0;n<350&&state()[0]!==0;n++)tick();menu(12);return {pause:p,returned:state()};});
  assert.deepEqual(errors,[]);const path=resolve(output,'candidate-'+rate+'.jsonl');
  await writeTraceJsonLines(path,recordsFromSegments({comparisonIdentity,coverage,provenance:{provider:'th15/browser-development',rate,wasmSha256:sha(await readFile(files.get('/th15-application.wasm'))),limitations},segments,runReason:'game-completion-and-return-to-replay-menu'}));
  captures.push({rate,path,ticks,stages:segments.map(s=>({stage:s.stage,ticks:s.ticks.length,declaredInputFrames:metadata[16+s.stage]})),terminalMarker:!!metadata[4],finalReplayCursor:metadata[1],finalReplayClock:metadata[2],lifecycle});await writeFile(resolve(output,'captures.json'),JSON.stringify(captures,null,2)+'\n');await page.close();
 }
 const results=[];for(const actual of captures.slice(1)){const result=await compareTraces(readTraceJsonLines(captures[0].path),readTraceJsonLines(actual.path));results.push({rate:actual.rate,...result});}
 const first=JSON.parse((await readFile(captures[0].path,'utf8')).split('\n')[2]);const mutation={...first,appliedInput:{...first.appliedInput,held:first.appliedInput.held^1}};
 const rows=(await readFile(captures[0].path,'utf8')).trim().split('\n').map(JSON.parse);rows[2]=mutation;
 const negativeControl=await compareTraces(readTraceJsonLines(captures[0].path),rows);assert.equal(negativeControl.status,'DIVERGED');
 const truncatedControl=await compareTraces(readTraceJsonLines(captures[0].path),rows.slice(0,2));assert.equal(truncatedControl.status,'INCOMPLETE');
 const report={overallStatus:'INCOMPLETE',candidateConsistencyPassed:results.every(r=>r.status==='PASS'),originalComparison:{status:'INCOMPLETE',reason:'missing-retail-golden-trace'},comparisonIdentity,coverage,limitations,captures,results,negativeControl,truncatedControl};await writeFile(resolve(output,'result.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify({candidateConsistencyPassed:report.candidateConsistencyPassed,results,negativeControl,truncatedControl}));assert.ok(report.candidateConsistencyPassed,'common Replay verifier found divergence');
}finally{await browser?.close();await new Promise(r=>server.close(r));}
