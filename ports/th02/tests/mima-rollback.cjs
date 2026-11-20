const {createServer}=require('node:http'),fs=require('node:fs'),path=require('node:path');
const puppeteer=require('../../../launcher/node_modules/puppeteer-core');const assert=require('node:assert/strict');
const extra=process.argv.includes('--extra'),stage=extra?5:4,language=process.argv.includes('--jp')?'jp':'cn';
const challengeMode=process.argv.includes('--challenge'),bootOnly=process.argv.includes('--boot-only'),players=bootOnly?2:3;
process.chdir(path.resolve(__dirname,'../../..'));
const root=path.resolve('ports/th02/build-eagler'),content=path.resolve('ports/th02/content/disks');
const pack=fs.readFileSync('ports/th02/content/th02.data'),packSize=pack.readUInt32LE(8),packFiles=JSON.parse(pack.subarray(12,12+packSize)).files;
const inject=`sync.qa=function(action,arg){
 const h=emulator.module.HEAPU8,v=new DataView(h.buffer),m=mailbox,cb=m-patch.entries.mailbox,d=cb+(patch.dataSegment-patch.codeSegment)*16;
 if(action==='init'){
  globalThis.q={tick:0,d,extra:config.stage===5,ending:null,bonus:null,held:0};
  const fs=emulator.module.FS,open=fs.open;fs.open=function(path,...args){const result=open.call(this,path,...args);if(typeof path==='string'&&/maine\\.exe$/i.test(path))q.ending??=q.tick;return result;};
  return {state:sync.snapshot(),cb,d};
 }
 if(action==='advance'){
  for(let i=0;i<arg;i++){
   const n=q.tick++;
   if(q.extra&&(h[q.d+0x7b42]&1)&&!h[q.d+0x2bfa]&&v.getUint16(q.d+0x2be0,true)>90){
    // Supply damage, not phase transitions, so every native clear path runs.
    emulator.module.thDirtyMark?.(q.d+0x2bea,2);v.setInt16(q.d+0x2bea,2400,true);
   }
   if(sync.touchContext().key.endsWith(':bonus')){
    if(sync.touchContext().play||sync.canPredict())throw Error('Bonus must be confirmed-only and accept menu touch');
    if(q.bonus===null){q.bonus=n;q.held=previousInputs[0]&32;}
   }
   const confirmAt=q.bonus===null?-1:q.bonus+48;
   const input=[q.ending!==null?0:bonusActive()?q.held:n%60<30?32:0,n===confirmAt?256:32,32].slice(0,players);
   if(q.extra&&q.ending!==null&&n<=confirmAt)throw Error('Held shot skipped the Extra bonus');
   if(n===confirmAt){
    if(!bonusActive())throw Error('Held shot acknowledged the bonus');
    const {frame:rgb,frameWidth:w}=emulator.module.framePresenter.capture();let totalRowPixels=0;
    // Check the bottom total row, not just the title/first row of a partial draw.
    for(let y=288;y<304;y++)for(let x=64;x<416;x++){
     const at=rgb+(y*w+x)*3;if(h[at]>176&&h[at+1]>176&&h[at+2]>176)totalRowPixels++;
    }
    if(totalRowPixels<30)throw Error('Bonus total row is not displayed: '+totalRowPixels);
    // Delay P2's one-tick confirmation on this seat, then correct that frame.
    if(localSlot===1){capture(n);step([input[0],32,32].slice(0,players),{frame:n});if(!bonusActive())throw Error('Unconfirmed bonus advanced');restore(n);}
    step(input,{frame:n,replay:localSlot===1});
   }else if(n%100===0){
    capture(n);step(input,{frame:n});const expected=checksum();restore(n);step(input,{frame:n,replay:true});const actual=checksum();
    if(JSON.stringify(expected)!==JSON.stringify(actual))throw Error('Rollback differs at '+n+' '+JSON.stringify({expected,actual}));
   }else step(input,{frame:n});
   confirm(n+1);
  }
  return {tick:q.tick,state:sync.snapshot(),challenge:challenge?.snapshot(),ending:q.ending,bonus:q.bonus,phase:q.extra?h[q.d+0x7b42]:v.getUint16(q.d+0x91f8,true),stagePhase:h[q.d+0x19f6],hp:v.getInt16(q.d+0x2bea,true),timer:v.getUint16(q.d+0x2be0,true),check:checksum()};
 }
};\n`;
const runtime=fs.readFileSync(path.join(root,'netplay/runtime.js'),'utf8')
 .replace(' if(mailbox>=0)m.HEAPU8[mailbox+patch.focus.pointOffset]',(extra?' if(mailbox>=0&&!bonusActive())m.HEAPU8[mailbox+patch.debug.invincibilityMailboxOffset]=1;\n':'')+' if(mailbox>=0)m.HEAPU8[mailbox+patch.focus.pointOffset]')
 .replace('if(workerHost)workerHost.runtime=sync;',inject+'if(workerHost)workerHost.runtime=sync;');
const server=createServer((req,res)=>{try{let n=decodeURIComponent(new URL(req.url,'http://a').pathname);if(n==='/'){res.setHeader('content-type','text/html');return res.end('<!doctype html><title>QA</title>');}let f=n.endsWith('.hdi.gz')?path.join(content,path.basename(n)):path.join(root,n);const entry=packFiles.find(x=>'/'+x.path===n);let b=entry?pack.subarray(12+packSize+entry.offset,12+packSize+entry.offset+entry.bytes):fs.readFileSync(f);if(n==='/netplay/runtime.js')b=Buffer.from(runtime);if(n==='/netplay/runtime.json'){let j=JSON.parse(b);j.assets['netplay/runtime.js']=require('node:crypto').createHash('sha256').update(runtime).digest('hex');b=Buffer.from(JSON.stringify(j));}res.setHeader('content-type',n.endsWith('.js')||n.endsWith('.mjs')?'text/javascript':n.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(b);}catch(e){res.statusCode=404;res.end(String(e));}});
(async()=>{await new Promise(r=>server.listen(0,'127.0.0.1',r));const {findChromiumExecutable}=await import('../../../launcher/lib/chromium-executable.mjs');const b=await puppeteer.launch({headless:true,executablePath:await findChromiumExecutable(),args:['--enable-unsafe-swiftshader']});try{const p=await b.newPage();p.on('pageerror',e=>console.log('error',e));await p.goto('http://127.0.0.1:'+server.address().port+'/');await p.evaluate(()=>{let id=0;window.workers=[0,1,2].map(()=>{const pending=new Map(),w=new Worker('/netplay/runtime-worker.js',{type:'module'});w.onmessage=({data})=>{if(data.type==='error'){const a=pending.get(data.id);if(a){pending.delete(data.id);a.reject(Error(data.message));}}if(data.type==='result'){const a=pending.get(data.id);if(a){pending.delete(data.id);a.resolve(data.result);}}};return {w,pending};});window.call=(seat,type,args=[],extra={})=>new Promise((resolve,reject)=>{const n=++id,{w,pending}=workers[seat];pending.set(n,{resolve,reject});w.postMessage({type,id:n,args,...extra});});});
const seats=Array.from({length:players},(_,i)=>i);
const settings={players,rollback:true,language,stage,p1:0,p2:1,p3:2,difficulty:1,challengeMode,lives:challengeMode?2:5,bombs:challengeMode?0:3};
for(const i of seats){await p.evaluate((i,settings)=>call(i,'load',[],{configure:[0,i,settings.players],settings}),i,settings);await p.evaluate(i=>call(i,'start'),i);await p.evaluate(i=>call(i,'qa',['init']),i);}
console.log(`TH02 ${players}-player ${bootOnly?'startup':'ending'} simulation:`,extra?'Extra':'Mima',language,challengeMode?'challenge':'standard');let sawBoss=false,finished=false;
for(let i=0;i<350;i++){
 const results=await p.evaluate(seats=>Promise.all(seats.map(i=>call(i,'qa',['advance',100]))),seats);
 const {tick,phase,stagePhase,hp,timer,check,state,ending,bonus}=results[0];
 assert(results.every(x=>JSON.stringify(check)===JSON.stringify(x.check)&&ending===x.ending&&bonus===x.bonus),'Multiplayer checksum or ending differs at '+tick);
 if(bootOnly&&state?.gameFrame>=300){
  assert.equal(state.stage,stage);
  assert.equal(state.players.length,players);
  if(challengeMode){
   assert(results.every(x=>x.challenge.length===players),'Each seat must track the challenge counters');
   assert(state.players.every(x=>x.lives===2&&x.bombs===0),'Challenge stocks must be 2 lives and 0 Bombs');
  }
  finished=true;console.log('PLAYABLE',JSON.stringify({tick,stage:state.stage,frame:state.gameFrame,challenge:results[0].challenge,stocks:state.players.map(({lives,bombs})=>[lives,bombs])}));break;
 }
 if(bootOnly&&tick>=4500)break;
 if(phase>=1&&state)sawBoss=true;
 if(i%10===0)console.log('SYNC',tick,phase,stagePhase,hp,timer,check.gameFrame,bonus,ending);
 if(ending!==null&&tick>=ending+180){
  if(extra)assert(bonus!==null&&ending>bonus+48,'Extra must enter its ending after the remote confirmation');
  finished=true;console.log('ENDING',ending,'BONUS',bonus,'simulated through',tick);break;
 }
}
assert(finished&&(bootOnly||sawBoss),bootOnly?'Both workers must reach gameplay':'Boss fight must run through the original ending');
}finally{await b.close();server.close();}})().catch(e=>{console.error(e);process.exitCode=1;server.close()});
