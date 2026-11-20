const assert = require('node:assert/strict');
const { createHash } = require('node:crypto');
const { readFileSync } = require('node:fs');
const { createServer } = require('node:http');
const { basename, resolve, sep } = require('node:path');
const puppeteer = require('../../../launcher/node_modules/puppeteer-core');

// Build TH02 first. Run separately with arguments `jp 3` and `cn 2`.
const [language = 'jp', playersText = '3'] = process.argv.slice(2);
const players = Number(playersText);
assert(['jp', 'cn'].includes(language) && [2, 3].includes(players));
const port = resolve(__dirname, '..');
const root = resolve(port, 'build-eagler');
const content = resolve(port, 'content');
const pack = readFileSync(resolve(content, 'th02.data'));
const headerSize = 12 + pack.readUInt32LE(8);
const files = new Map(JSON.parse(pack.subarray(12, headerSize)).files.map(entry => [entry.path, entry]));

// Exercise the actual MAIN.EXE through the Worker and the real stage-clear
// transition. Only stocks, invincibility and the fourth boss's damage are
// supplied by the harness, so resource loading is identical to a match.
const harness = `
{
let frame=0,stage4Start,marisaStart,issued=0,pending=null,shared,bonusAt;
const artCounts=[0,0,0],piSizes=[24416,44928,71040];
sync.resourceQA=(steps)=>{
 const m=emulator.module,h=m.HEAPU8,v=new DataView(h.buffer);
 for(let i=0;i<steps;i++){
  const state=sync.snapshot(),cs=mailbox-patch.entries.mailbox;
  const ds=cs+(patch.dataSegment-patch.codeSegment)*16;
  const word=offset=>v.getUint16(ds+offset,true);
  const put=(at,size,value)=>{m.thDirtyMark?.(at,size);if(size===1)h[at]=value;else v.setInt16(at,value,true);};
  const stage=state?.stage,phase=h[ds+0x19f6],bomb=h[ds+0x3e44];
  const input=Array(players).fill((phase||stage===4)&&frame%8<4?32:0);
  if(bonusActive()){
   bonusAt??=frame;
   if(sync.canPredict()||sync.touchContext().play)throw Error('Stage bonus is not a confirmed menu');
   input.fill(0);if(frame===bonusAt+24){input[1]=32;observed.bonusConfirmed=true;}
  }
  shared??=word(0x3e50);
  if(stage===3){
   if(word(0x3e50)!==shared)throw Error('BOMBS.BFT was reallocated');
   stage4Start??=state.gameFrame;
   if(pending!==null&&bomb&&word(0x3f5a)===[0x34b8,0x398b,0x373c][pending]){
    const art=word(0x22f2),block=guestMemoryBase+(art-1)*16;
    const allocated=v.getUint16(block,true)===1&&(v.getUint16(block+2,true)-art)*16>=piSizes[pending];
    if(!allocated)throw Error('Bomb '+pending+' did not load its native PI');
    if(word(0x862)!==word(0x1ad8))throw Error('Bomb PI decoder did not release its workspace');
    artCounts[pending]++;pending=null;
   }
   if(!bomb&&pending===null&&issued<24&&state.gameFrame-stage4Start>50+issued*175){
    const seat=1+issued++%(players-1);
    pending=config['p'+(seat+1)];input[seat]|=16;
    put(cs+(seat===1?patch.entries.guest_bank:patch.entries.third_bank)+13,1,5);
   }
   if(phase===1){marisaStart??=frame;if(frame-marisaStart>450)put(ds+0x2bea,2,1000);}
  }
  const rewind=frame%113===0;
  if(rewind)capture(frame);
  step(input,{frame});
  if(rewind){
   const proof=JSON.stringify(checksum());restore(frame);step(input,{frame,replay:true});
   if(JSON.stringify(checksum())!==proof)throw Error('Rollback diverged at frame '+frame);
  }
  confirm(++frame);
  if(stage===4){
   const script=word(0x5568),schedule=word(0x91ca),mima=word(0x91f8);
   const enemies=Array.from({length:25},(_,n)=>h[ds+0x7b5e+n*0x26]).filter(x=>x===1).length;
   if(script!==14||schedule!==76)throw Error('Broken stage 5 enemy script: '+script+'/'+schedule);
   const picture=m.framePresenter.capture();
   let backgroundPixels=0,bossPixels=0;
   if(picture&&(enemies||mima>=2)){
    const {frame:rgb,frameWidth:w}=picture;
    for(let y=36;y<164;y++)for(let x=32;x<416;x++){
     const at=rgb+(y*w+x)*3,r=h[at],g=h[at+1],b=h[at+2];
     if((x<66||x>=382)&&b>=120&&b>=r+50)backgroundPixels++;
     if(x>=80&&x<380&&g>=120&&g>=r+40&&g>=b+40)bossPixels++;
    }
   }
   observed.enemies=Math.max(observed.enemies,enemies);
   if(!phase)observed.background=Math.max(observed.background,backgroundPixels);
   if(mima>=2)observed.boss=Math.max(observed.boss,bossPixels);
   if(observed.enemies&&observed.background>200&&observed.boss>100)break;
  }
 }
 return {frame,issued,artCounts,...observed};
};
const observed={enemies:0,background:0,boss:0,bonusConfirmed:false};
}
`;

const source = readFileSync(resolve(root, 'netplay/runtime.js'), 'utf8');
const inputHook = ' if(mailbox>=0)m.HEAPU8[mailbox+patch.focus.pointOffset]';
const workerHook = 'if(workerHost)workerHost.runtime=sync;';
assert(source.includes(inputHook) && source.includes(workerHook));
const runtime = source
  .replace(inputHook, ' if(mailbox>=0)m.HEAPU8[mailbox+patch.debug.invincibilityMailboxOffset]=1;\n' + inputHook)
  .replace(workerHook, harness + workerHook);
const manifest = JSON.parse(readFileSync(resolve(root, 'netplay/runtime.json'), 'utf8'));
manifest.assets['netplay/runtime.js'] = createHash('sha256').update(runtime).digest('hex');

const server = createServer((req, res) => {
  try {
    const name = decodeURIComponent(new URL(req.url, 'http://localhost').pathname.slice(1));
    if (!name) return res.setHeader('content-type', 'text/html').end('<!doctype html><title>TH02 stage regression</title>');
    const asset = resolve(root, name);
    if (!asset.startsWith(root + sep)) { res.statusCode = 404; return res.end(); }
    const entry = files.get(name);
    const bytes = name === 'netplay/runtime.js' ? runtime
      : name === 'netplay/runtime.json' ? JSON.stringify(manifest)
      : entry ? pack.subarray(headerSize + entry.offset, headerSize + entry.offset + entry.bytes)
      : name.endsWith('.hdi.gz') ? readFileSync(resolve(content, 'disks', basename(name)))
      : readFileSync(asset);
    const type = /\.m?js$/.test(name) ? 'text/javascript' : name.endsWith('.wasm') ? 'application/wasm' : 'application/octet-stream';
    res.setHeader('content-type', type).end(bytes);
  } catch (error) {
    res.statusCode = 404;
    res.end(String(error));
  }
});

(async () => {
  await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
  const { findChromiumExecutable } = await import('../../../launcher/lib/chromium-executable.mjs');
  const browser = await puppeteer.launch({ headless: true, executablePath: await findChromiumExecutable(), args: ['--enable-unsafe-swiftshader'] });
  try {
    const page = await browser.newPage();
    await page.goto('http://127.0.0.1:' + server.address().port);
    await page.evaluate(() => {
      const worker = new Worker('/netplay/runtime-worker.js', { type: 'module' });
      let id = 0;
      const pending = new Map();
      worker.onmessage = ({ data }) => {
        if (!['result', 'error'].includes(data.type)) return;
        const { resolve, reject } = pending.get(data.id);
        pending.delete(data.id);
        if (data.type === 'error') reject(Error(data.message));
        else resolve(data.result);
      };
      window.call = (type, args = [], extra = {}) => new Promise((resolve, reject) => {
        pending.set(++id, { resolve, reject });
        worker.postMessage({ type, id, args, ...extra });
      });
    });
    const settings = { language, players, stage: 3, difficulty: 1, p1: 0, p2: players === 2 ? 2 : 1, p3: 2, lives: 5, bombs: 3, rollback: true };
    await page.evaluate(settings => call('load', [], { configure: [0, 0, settings.players], settings }), settings);
    await page.evaluate(() => call('start'));
    let outcome;
    for (let i = 0; i < 110; i++) {
      outcome = await page.evaluate(() => call('resourceQA', [180]));
      if (outcome.enemies && outcome.background > 200 && outcome.boss > 100) break;
    }
    assert.equal(outcome.issued, 24);
    assert.equal(outcome.bonusConfirmed, true);
    assert.deepEqual(outcome.artCounts, players === 2 ? [0, 0, 24] : [0, 12, 12]);
    assert(outcome.enemies && outcome.background > 200 && outcome.boss > 100, JSON.stringify(outcome));
    console.log('PASS', language, players, outcome);
  } finally {
    await browser.close();
    server.close();
  }
})().catch(error => { console.error(error); process.exitCode = 1; server.close(); });
