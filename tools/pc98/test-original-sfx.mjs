import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createServer} from 'node:http';
import {resolve,sep} from 'node:path';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../launcher/lib/chromium-executable.mjs';

const root=resolve(import.meta.dirname,'../..'),workspace=resolve(root,'..');
const require=createRequire(new URL('../../launcher/package.json',import.meta.url));
const puppeteer=require('puppeteer-core');
const disks={th02:'th02-coop-lab/web/th02-solo-jp.hdi.gz',th03:'th03-netplay-lab/web/disks/3-jp.hdi.gz',th04:'th04-coop-lab/web/th04-solo-jp.hdi.gz'};
const server=createServer(async(request,response)=>{
  const match=/^\/(th0[234])\/(.+)$/.exec(decodeURIComponent(request.url));
  if(!match){response.writeHead(404).end();return;}
  const [,game,name]=match,base=resolve(root,'ports',game,'build-eagler');
  const file=name==='disk.gz'?resolve(workspace,disks[game]):resolve(base,name);
  if(name!=='disk.gz'&&!file.startsWith(base+sep)){response.writeHead(400).end();return;}
  try{const data=await readFile(file);response.writeHead(200,{'Content-Type':name.endsWith('.wasm')?'application/wasm':/\.(m?js)$/.test(name)?'text/javascript':'application/octet-stream'});response.end(data);}
  catch{response.writeHead(404).end();}
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
try{
  for(const game of process.argv.slice(2).length?process.argv.slice(2):Object.keys(disks)){
    const page=await browser.newPage();await page.goto(`http://127.0.0.1:${server.address().port}/${game}/original-sfx.mjs`);
    const result=await page.evaluate(async game=>{
      const {OriginalSfx}=await import('./original-sfx.mjs');
      const {Fat12}=await import(game==='th03'?'./disk.js':'./solo-fat12.js');
      const response=await fetch('./disk.gz');
      const disk=new Uint8Array(await new Response(response.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());
      const sfx=await OriginalSfx.create(game,new Fat12(disk));
      for(let id=1;id<=8&&sfx.peak<0.001;id++){sfx.play(id);await new Promise(resolve=>setTimeout(resolve,500));}
      const peak=sfx.peak;sfx.dispose();return peak;
    },game);
    assert(result>0.001,`${game}: original sound effects must produce audible output`);
    console.log(`${game}: original sound effects PASS (peak ${result.toFixed(4)})`);await page.close();
  }
}finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
