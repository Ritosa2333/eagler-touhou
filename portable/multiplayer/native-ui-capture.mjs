import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve,extname} from 'node:path';
import {createServer} from 'node:http';
import {createRequire} from 'node:module';
import {createHash} from 'node:crypto';
import {captureNativeSaveMenus} from './native-ui-save-capture.mjs';
const here=import.meta.dirname,root=resolve(here,'../..');
const index=process.argv.indexOf('--phase'),phase=index<0?'after':process.argv[index+1];
if(!/^[a-zA-Z0-9_-]+$/.test(phase))throw Error('Invalid diagnostic phase');
const out=resolve(root,'artifacts/multiplayer-ui/20261009',phase);
const launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../../eagler-touhou')),{chromium}=createRequire(resolve(launcher,'package.json'))('playwright');
const archive=process.env.TH11_TEST_ARCHIVE||resolve(root,'../../th11-eagler/[th11] 东方地灵殿 (汉化版+日文版)/th11.dat');
const fonts=['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'];
const music=process.argv.includes('--mp-only')?[]:['th11_00.ogg','th11_01.ogg'];
const variants=process.argv.includes('--mp-only')?['multiplayer']:process.argv.includes('--ordinary-only')?['ordinary']:['ordinary','multiplayer'];
const files=new Map([['/th11.dat',archive],...variants.flatMap(x=>['mjs','wasm'].map(e=>['/'+x+'.'+e,resolve(out,x+'.'+e)])),...fonts.map(x=>['/fonts/'+x,resolve(process.env.TH11_TEST_FONTS||resolve(root,'build-eagler-multiplayer/fonts'),x)]),...music.map(x=>['/music/'+x,resolve(process.env.TH11_TEST_MUSIC_ROOT||resolve(root,'../../games/web-content/th11/music'),x)])]);
for(const p of files.values())if(!existsSync(p))throw Error('Missing '+p);
const evidence={diagnostic:true,phase,scope:'Same-process native GameSession, actual SDL/GLES WebGL renderer in Chromium SwiftShader; controlled seam fixtures; no network transport or full-playthrough claim',inputs:'Raw native MultiplayerInput confirmed-array shape; no protocol prediction',resourceDependencies:[...files.values()].map(path=>{const bytes=readFileSync(path);return {path,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')};}),shots:[],errors:[]};
const server=createServer((req,res)=>{const path=new URL(req.url,'http://localhost').pathname;res.setHeader('Cache-Control','no-store');if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<style>html,body{margin:0;background:#000}canvas{width:640px;height:480px;display:block}</style><canvas id="canvas" width="640" height="480"></canvas>');return;}const file=files.get(path);if(!file){res.writeHead(404).end();return;}res.setHeader('Content-Type',extname(file)==='.mjs'?'text/javascript':extname(file)==='.wasm'?'application/wasm':'application/octet-stream');res.end(readFileSync(file));});
await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
try{
 browser=await chromium.launch({headless:true,args:['--no-sandbox','--enable-unsafe-swiftshader','--use-gl=angle','--use-angle=swiftshader']});
 async function pageFor(variant){const page=await browser.newPage({viewport:{width:640,height:480}});page.on('pageerror',e=>evidence.errors.push(String(e)));page.on('console',m=>{if(m.type()==='error')console.log('browser '+m.text());});await page.goto('http://127.0.0.1:'+server.address().port);await page.evaluate(async({variant,fonts,music})=>{
  const {default:create}=await import('/'+variant+'.mjs');const c=window.core=window.Module=await create({canvas:document.querySelector('canvas')});
  c.eaglerOptions={};c.eaglerControls={};
  for(const path of ['/th11.dat',...fonts.map(x=>'/fonts/'+x),...(variant==='ordinary'?music.map(x=>'/music/'+x):[])]){c.FS.mkdirTree(path.slice(0,path.lastIndexOf('/'))||'/');c.FS.writeFile(path,new Uint8Array(await fetch(path).then(r=>r.arrayBuffer())));}
  window.str=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));window.err=()=>str(c._th11_error());window.state=()=>JSON.parse(str(c._mp_fixture_ui_state()));window.step=(n=1,h=0,p2=0,p3=0,pause=0)=>{if(!c._mp_fixture_ui_step(n,h,p2,p3,pause))throw Error(err());};window.begin=(stage=1,local=0)=>{if(!c._mp_fixture_ui_begin(stage,local))throw Error(err());step(125);};
  c._th11_music_enabled(0);if(!c._th11_initialize())throw Error(err());
 },{variant,fonts,music});return page;}
 async function shot(page,name,entry){const state=await page.evaluate(()=>window.state());await page.screenshot({path:resolve(out,name+'.png')});evidence.shots.push({name,entry,state});writeFileSync(resolve(out,process.argv.includes('--ordinary-only')?'ordinary-evidence.json':'screenshot-evidence.json'),JSON.stringify(evidence,null,2));console.log(name+' '+JSON.stringify(state));}
 if(!process.argv.includes('--mp-only')){const normal=await pageFor('ordinary');await normal.evaluate(()=>{begin();step(1,0,0,0,1);step(18);});await shot(normal,'ordinary-pause','ordinary Application::restart, neutral125, native pause edge, neutral18');
 await normal.evaluate(()=>{if(!core._mp_fixture_ui_replays())throw Error(err());step(30);});await shot(normal,'ordinary-replay-list','ordinary open_title(Replays), scan_replays, neutral30; empty ordinary save namespace');await normal.close();}
 if(process.argv.includes('--ordinary-only')){evidence.passed=true;}else{const page=await pageFor('multiplayer');
 await page.evaluate(()=>{begin();step(1,0,0,0,1);step(18);});await shot(page,'mp-pause','3P native begin, neutral125, P1 pause edge, neutral18');
 for(let local=0;local<3;++local){await page.evaluate(local=>begin(1,local),local);await shot(page,'mp-3p-local'+(local+1),'3P native begin {0,3,5}, localSeat '+local+', neutral125');}
 await page.evaluate(()=>{begin();if(!core._mp_fixture_ui_fixture(1))throw Error('life fixture');});let prior=0;
 for(const n of [30,60,89,90]){await page.evaluate(n=>step(n,8),n-prior);prior=n;await shot(page,'life-gift-'+n,'Controlled positions x0/10/100 y400, native invincibility100000, lives3/1/3; P1 Focus held '+n+' ticks');}
 await page.evaluate(()=>step(15));await shot(page,'life-gift-delivered','15 native neutral ticks after life transfer spawn');
 await page.evaluate(()=>{begin();if(!core._mp_fixture_ui_fixture(2))throw Error('ghost fixture');});prior=0;
 for(const n of [45,89,90]){await page.evaluate(n=>step(n,8),n-prior);prior=n;await shot(page,'ghost-rescue-'+n,'P2 native game_over(false); diagnostic pins ghost drift each tick; P1 Focus held '+n+' ticks');}
 await page.evaluate(()=>{begin();if(!core._mp_fixture_ui_fixture(3))throw Error('power fixture');});
 for(let n=1;n<=5;++n){await page.evaluate(()=>{step(1,1);});if(n>=3)await shot(page,'power-tap-'+n,'Native P1 Shoot press edge '+n+', each separated by 1 neutral tick; Power40/0/full');await page.evaluate(()=>step(1));}
 for(const [kind,name,stage]of [[0,'game-over',1],[1,'extra-result',7]]){await page.evaluate(({kind,stage})=>{begin(stage);if(!core._mp_fixture_ui_terminal(kind))throw Error('terminal seam');step(30);},{kind,stage});await shot(page,name,'StageExit '+(kind===0?'Title → native GameOver':'Results → native Extra front103')+', neutral30; UI-only seam');}
 await page.evaluate(()=>{begin();if(!core._mp_fixture_ui_terminal(2))throw Error('Ending seam');step(1);});await page.evaluate(()=>{for(let i=0;i<4000&&state().phase==='ending';++i)step(1,512|(i&1?1:0));if(state().phase!=='game_over')throw Error('Ending did not finish '+JSON.stringify(state()));step(30);});await shot(page,'ending-title-results','StageExit Ending, real native Ending and Staff interpreter, P1 Ctrl+alternatingShoot until completed, neutral30');
 await captureNativeSaveMenus({page,shot,out,evidence});
 await page.close();if(evidence.errors.length)throw Error(evidence.errors.join('\n'));evidence.passed=true;}
}catch(e){evidence.failure=String(e.stack||e);throw e;}finally{writeFileSync(resolve(out,process.argv.includes('--ordinary-only')?'ordinary-evidence.json':'screenshot-evidence.json'),JSON.stringify(evidence,null,2));await browser?.close();await new Promise(r=>server.close(r));}
