// Production room component and stylesheet, with explicit presentation fixtures.
// Future machine signs are fixture data, never written to an actual room.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {authoredSourcesPlugin} from '../react-main/authored-sources.mjs';
const project=fileURLToPath(new URL('../../',import.meta.url));
const publicRoot=resolve(project,'public');
const out=resolve(process.env.TH09_CARD_TEST_OUTPUT || '.cache/th09-card-browser');
await mkdir(out,{recursive:true});
const entry=`
import React,{useState} from 'react';
import {createRoot} from 'react-dom/client';
import {DirectoryRoomRows} from './app/components/directory/DirectoryRoomRows.tsx';
import {LocaleProvider} from './app/i18n.tsx';
const seat=(wins)=>({initial:wins===2?'甲':'乙',online:true,ready:true,controlMode:null,stats:{wins,losses:3-wins,minutes:2}});
function Fixture(){
 const [mode,setMode]=useState('normal'),[joins,setJoins]=useState(0);
 const seats=[seat(2),seat(1)];
 if(mode==='long'||mode==='two'){seats[0].machineSign='这是用于验证手机省略显示的很长机签'.repeat(4);seats[1].machineSign='A very long public machine sign '.repeat(4);}
 if(mode==='unsafe')seats[0].machineSign='<img src=x onerror=alert(1)>';
 if(mode==='empty')seats[1]=null;
 if(mode==='offline')seats[1].online=false;
 const room={product:mode==='other'?'th06mp':'th09mp',code:'4017',capacity:2,players:seats.filter(Boolean).length,ready:2,difficulty:1,spectators:3,phase:mode==='empty'?'lobby':'playing',joinable:mode==='empty',spectatable:true,seats,disableCheatMovement:false,challengeMode:false};
 const snapshot={visibleRooms:mode==='two'?[room,{...room,code:'4018'}]:[room],connection:'live',mine:null,leaving:false};
 return <LocaleProvider locale="zh-CN"><div style={{padding:12}}><label>测试样例 <select id="sample" value={mode} onChange={e=>setMode(e.target.value)}><option value="normal">默认名称</option><option value="long">长机签</option><option value="empty">等待玩家</option><option value="offline">离线玩家</option><option value="unsafe">文本转义</option><option value="two">横屏两列</option><option value="other">其他作品</option></select></label><output id="joins">{joins}</output></div><main className="lobby-main" data-sample={mode}><section className={"lobby-list-surface"+(mode==="other"?"":" th09-room-surface")}><div role="list"><DirectoryRoomRows snapshot={snapshot} service={{joinRoom:()=>setJoins(n=>n+1)}} assetUrl={p=>'/'+p} initialRows /></div></section></main></LocaleProvider>;
}
createRoot(document.getElementById('root')).render(<Fixture/>);
`;
const bundle=await build({absWorkingDir:project,stdin:{contents:entry,loader:'tsx',resolveDir:project},bundle:true,format:'esm',platform:'browser',jsx:'automatic',write:false,plugins:[authoredSourcesPlugin(project)],logLevel:'silent'});
const css=new Set(['/styles.css','/features.css','/lobby.css','/ui-fonts.css','/ui-fonts-deferred.css']);
const server=createServer(async(req,res)=>{
 try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/'){res.setHeader('content-type','text/html; charset=utf-8');res.end('<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width"><link rel="stylesheet" href="/styles.css"><link rel="stylesheet" href="/ui-fonts.css"><link rel="stylesheet" href="/ui-fonts-deferred.css"><link rel="stylesheet" href="/features.css"><link rel="stylesheet" href="/lobby.css"><body class="lobby-page"><div id="root"></div><script type="module" src="/fixture.mjs"></script>');return;}
  if(path==='/fixture.mjs'){res.setHeader('content-type','text/javascript');res.end(bundle.outputFiles[0].text);return;}
  const file=resolve(publicRoot,'.'+path);
  if(!file.startsWith(publicRoot+sep)||!(css.has(path)||path.startsWith('/assets/')))throw Error('not a fixture asset');
  res.setHeader('content-type',path.endsWith('.css')?'text/css':path.endsWith('.svg')?'image/svg+xml':path.endsWith('.woff2')?'font/woff2':'application/octet-stream');
  res.end(await readFile(file));
 }catch{res.writeHead(404);res.end();}
});
await new Promise(r=>server.listen(Number(process.env.TH09_CARD_FIXTURE_PORT)||0,'127.0.0.1',r));
const url=`http://127.0.0.1:${server.address().port}/`;
const browser=await puppeteer.launch({headless:true,executablePath:await findChromiumExecutable()});
const report={url,passed:false,checks:[],errors:[]};
try{
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));
 for(const width of [1280,390,320]){
  await page.setViewport({width,height:460});await page.goto(url);await page.waitForSelector('.th09-room-row');await page.evaluate(async()=>document.fonts.ready);
  for(const mode of ['normal','long','empty','offline','unsafe']){
   await page.select('#sample',mode);await page.waitForFunction(m=>document.querySelector('main').dataset.sample===m,{},mode);
   const layout=await page.$eval('.th09-room-row',card=>{
    const box=n=>{const r=n.getBoundingClientRect();return {left:r.left,right:r.right,top:r.top,bottom:r.bottom,width:r.width,height:r.height};};
    const names=[...card.querySelectorAll('.th09-duel-name')];
    return {card:box(card),names:names.map(n=>({...box(n),font:getComputedStyle(n).fontFamily,size:getComputedStyle(n).fontSize,overflow:n.scrollWidth>n.clientWidth,ellipsis:getComputedStyle(n).textOverflow})),score:box(card.querySelector('.th09-room-score')),count:box(card.querySelector('.th09-room-spectators')),icons:[...card.querySelectorAll('img')].map(n=>({ok:n.complete&&n.naturalWidth>0,width:box(n).width})),text:card.textContent};
   });
   assert.ok(layout.card.right<=width&&layout.card.left>=0);
   assert.ok(layout.names[0].right<layout.score.left&&layout.names[1].left>layout.score.right);
   assert.ok(layout.count.left>layout.card.left+layout.card.width/2&&layout.count.top>layout.card.top+layout.card.height/2);
   assert.ok(layout.icons.every(i=>i.ok&&i.width<=24));
   assert.deepEqual(await page.$$eval('.th09-duel-avatar',nodes=>nodes.map(n=>n.textContent)),mode==='empty'?['甲','']:['甲','乙']);
   assert.ok(layout.names.every(n=>n.font.includes('ET Yatra')));
   if(width<=390){assert.ok(layout.card.height<=150);assert.ok(layout.names.every(n=>parseInt(n.size)<=15));}
   if(mode==='long')assert.ok(layout.names.every(n=>n.overflow&&n.ellipsis==='ellipsis'));
   if(mode==='unsafe')assert.equal(await page.$$eval('.th09-duel-name img',n=>n.length),0);
   assert.ok(layout.text.includes('Normal'));
   await (await page.$('.th09-room-row')).screenshot({path:resolve(out,`${width}-${mode}.png`)});
   report.checks.push({width,mode,...layout});
  }
 }
 await page.select('#sample','two');
 for(const [width,height] of [[844,390],[1280,720],[568,320],[390,844]]){
  await page.setViewport({width,height});await page.waitForFunction(()=>document.querySelectorAll('.th09-room-row').length===2);
  const cards=await page.$$eval('.th09-room-row',rows=>rows.map(n=>{const b=n.getBoundingClientRect();return {x:b.x,y:b.y,width:b.width,right:b.right};}));
  assert.ok(cards.every(c=>c.right<=width&&c.x>=0));
  if(width>height){assert.ok(Math.abs(cards[0].y-cards[1].y)<1);assert.ok(cards[0].right<cards[1].x);assert.ok(cards[0].width<width/2);}
  else {assert.ok(cards[1].y>cards[0].y);assert.equal(cards[0].x,cards[1].x);}
  await page.screenshot({path:resolve(out,width+'x'+height+'-two-rooms.png'),fullPage:true});
  report.checks.push({width,height,mode:'two',cards});
 }
 await page.select('#sample','other');assert.equal(await page.$('.th09-room-row'),null);assert.ok(await page.$('.lobby-cover'));
 assert.deepEqual(report.errors,[]);report.passed=true;
}finally{
 await browser.close();await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');
 console.log(JSON.stringify({passed:report.passed,url,output:out,states:report.checks.length}));
 if(process.env.TH09_CARD_KEEP_FIXTURE==='1'&&report.passed)console.log('Fixture remains available for in-app visual review');else await new Promise(r=>server.close(r));
}
