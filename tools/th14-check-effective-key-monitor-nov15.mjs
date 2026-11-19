import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {WASI} from 'node:wasi';
import {createHash} from 'node:crypto';
import {mkdirSync,mkdtempSync} from 'node:fs';
import {resolve,join} from 'node:path';
import {spawnSync} from 'node:child_process';
const root=resolve(process.env.TH14_SOURCE_ROOT??'ports/th14/runtimes/th12/th12_web/cpp');
const cache=resolve('.cache/th14-effective-key-monitor');mkdirSync(cache,{recursive:true});
const O=mkdtempSync(join(cache,'check-'));
const baseline=readFileSync(new URL('./fixtures/th14-effective-key-monitor-baseline.cpp',import.meta.url),'utf8');
function region(s,needle){const at=s.indexOf(needle);assert(at>=0,needle);const begin=s.indexOf('{',at);let k=begin+1,depth=1;while(depth){assert(k<s.length);depth+=(s[k]==='{')-(s[k]==='}');k++;}return s.slice(at,k);}
const ui=readFileSync(join(root,'sdl/PracticeUiTh14.cpp'),'utf8');
const battle=readFileSync(join(root,'game/GameBattle.cpp'),'utf8');
const session=readFileSync(join(root,'game/GameSession.cpp'),'utf8');
const wiring=session.match(/#if PORT_TITLE == 14\s+battle->practice_key_sample=practice_key_sample;\s+#endif\s+battle->set_input\(edges\);return true;/);assert(wiring,'Actual session callback wiring');
let candidate=baseline.replace(region(baseline,'void sample_keys('),region(ui,'void sample_keys(')).replace(region(baseline,'if(player_live&&player){'),region(battle,'if(player_live&&player){')).replace('static void configure_monitor(){}','static void configure_monitor(){'+region(ui,'s.practice_key_sample=[]')+';}').replace('void wire(GameBattle* battle,const GameSessionInput& edges){battle->set_input(edges);}','bool wire(GameBattle* battle,const GameSessionInput& edges){\n'+wiring[0]+'}');
const cxx=process.env.TH14_WASI_CXX??resolve('ports/th14/runtimes/th12/tools/wasi-sdk-34.0-x86_64-windows/bin/clang++.exe');
const include=resolve(process.env.TH14_COMMON_INCLUDE??'common/include');
for(const [mode,text] of [['baseline',baseline],['candidate',candidate]]){
 const cpp=join(O,mode+'-components.cpp');writeFileSync(cpp,text);const args=['-std=c++17','-O1','-fno-exceptions','-fno-rtti','-DPORT_TITLE=14','-mexec-model=reactor','-Wl,--export-all','-I'+include,cpp,'-o',join(O,mode+'-components.wasm')];
 const result=spawnSync(cxx,args,{encoding:'utf8',timeout:120000,env:{...process.env,TEMP:O,TMP:O,TMPDIR:O}});writeFileSync(join(O,mode+'-compile.log'),(result.stdout??'')+(result.stderr??''));assert.equal(result.status,0,'Set TH14_WASI_CXX to a WASI clang++ compiler. '+(result.error??result.stderr));
}
const mapped=n=>[16,32,64,128,1,2,0,0,512,8].reduce((v,m,i)=>v|((n&m)?1<<i:0),0);
const seq=Array.from({length:96},(_,i)=>[0,1,65,73,9,25,2,130,520][Math.floor(i/4)%9]);
const cases=[
 {name:'ordinary unchanged',raw:seq,effective:seq},
 {name:'auto shoot effective',raw:seq.map(()=>0),effective:seq.map(()=>1)},
 {name:'force focus effective',raw:seq,effective:seq.map(n=>n|8)},
 {name:'native replay effective',raw:seq.map(()=>0),effective:seq},
 {name:'hosted replay effective',raw:seq.map(()=>0),effective:seq,sdl:0},
 {name:'player absent',raw:seq,effective:seq,present:0},
 {name:'player callback disabled',raw:seq,effective:seq,alive:0},
 {name:'paused no player update',raw:seq,effective:seq,tick:0,paused:1},
 {name:'practice disabled',raw:seq,effective:seq,enabled:0},
 {name:'monitor disabled',raw:seq,effective:seq,monitor:0},
 {name:'lock timer unchanged',raw:seq,effective:seq.map(n=>n|1),lock:1},
 {name:'captured input actual updates',raw:seq,effective:seq.map(()=>0),sdl:0}
];
const modules={};for(const mode of ['baseline','candidate']){const wasi=new WASI({version:'preview1'});const module=new WebAssembly.Module(readFileSync(O+'/'+mode+'-components.wasm'));const x=new WebAssembly.Instance(module,{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(x);modules[mode]=x.exports;}
const results=[];
for(const spec of cases){const t={enabled:1,monitor:1,paused:0,lock:0,sdl:1,tick:1,alive:1,present:1,...spec};const expected=[];let prior=0,history=[];
 if(t.enabled&&t.monitor&&t.tick&&t.alive&&t.present)for(const value of t.effective){const keys=mapped(value);history.push([keys,((keys^prior).toString(2).match(/1/g)||[]).length]);prior=keys;if(history.length>60)history.shift();expected.push([keys,history.reduce((v,x)=>v+x[1],0)]);}
 const row={name:t.name};let previousLocks=null;
 for(const mode of ['baseline','candidate']){const c=modules[mode];c.reset(t.enabled,t.monitor,1,t.paused,t.lock);for(let i=0;i<t.raw.length;i++)c.step(t.raw[i],t.effective[i],t.sdl,t.tick,t.alive,t.present);const actual=Array.from({length:c.rows()},(_,i)=>[c.bits(i),c.aps(i)]);row[mode]=JSON.stringify(actual)===JSON.stringify(expected);if(mode==='candidate')assert.deepEqual(actual,expected,t.name);assert.equal(c.observed(),t.tick&&t.alive&&t.present?t.raw.length:0,t.name+' diagnostics');assert.equal(c.players(),c.observed());if(previousLocks===null)previousLocks=c.locks();else assert.equal(c.locks(),previousLocks,t.name+' lock timer');row.lockFrames=c.locks();}
 results.push(row);
}
const sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');const q={passed:true,candidateCases:results.length,baselineRejected:results.filter(x=>!x.baseline).length,unchangedControls:results.filter(x=>x.baseline).length,actualAuthoredCppSamplerAndHookFragments:true,realSharedKeyMonitorAlgorithm:true,sessionAndPlayerStandIns:true,fullGameSessionAccepted:false,diagnosticObserverPreserved:true,lockTimerControlsUnchanged:true,browserAccepted:false,productionChanged:false,completeGame:false,results,moduleHashes:Object.fromEntries(Object.keys(modules).map(k=>[k,sha(O+'/'+k+'-components.wasm')]))};writeFileSync(O+'/component-qualified.json',JSON.stringify(q,null,2)+'\n');console.log(JSON.stringify({...q,results:undefined,moduleHashes:undefined}));
