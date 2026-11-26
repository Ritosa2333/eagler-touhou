import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {productionBrowser,runtimeCommand} from './production-driver.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');return [value.slice(2,at),value.slice(at+1)];}));
if(!args.replays)throw Error('需要 --replays=已有授权录像目录');
const output=resolve(args.output||fileURLToPath(new URL('../build/rendering-corpus',import.meta.url)));
mkdirSync(output,{recursive:true});
const fixtures=JSON.parse(readFileSync(new URL('./fixtures/rendering.json',import.meta.url))),baseline=args.baseline?JSON.parse(readFileSync(resolve(args.baseline,'result.json'))).cases:null;
const report={schema:'thkkd/rendering-corpus/1',referenceExe:fixtures.originalExeSha256,cases:[]};
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:8174/',report,protocolTimeout:300000}),{page,browser}=driver;
try{
  await driver.library();const frame=await driver.start();
  for(const item of fixtures.cases){
    const bytes=readFileSync(resolve(args.replays,item.replay));await runtimeCommand(page,'write',{path:'replay/render-compare.rpy',bytes:[...bytes]});
    await runtimeCommand(page,'playback',{action:'open',path:'replay/render-compare.rpy'});await runtimeCommand(page,'playback',{action:'pause'});
    for(const originalFrame of item.originalFrames){
      const cursor=originalFrame-fixtures.outerToReplayOffset;await runtimeCommand(page,'playback',{action:'seek',value:cursor});
      await frame.waitForFunction(n=>__thkkdRuntime.core._thkkd_word(0xb43110)===n,{timeout:240000},cursor);
      const data=await frame.evaluate(()=>{
        const r=__thkkdRuntime,p=r.platform,w=a=>r.core._thkkd_word(a);p.renderer.present(0);
        let hash=2166136261;for(const a of [0xa74c08,0xb3e2ec,0xb3e2e8,0xb43110,0xa78e2c])hash=Math.imul(hash^w(a),16777619)>>>0;
        for(let i=0;i<60;i++)hash=Math.imul(hash^w(0xa8cd68+i*4),16777619)>>>0;
        for(let i=0;i<3000;i++){const b=0xa8e660+i*240;if(w(b))for(const o of [0,4,8,12,16,64,68,72,76,80,84,88,92])hash=Math.imul(hash^w(b+o),16777619)>>>0;}
        return {png:document.querySelector('canvas').toDataURL(),state:{cursor:w(0xb43110),stage:w(0xb3e2ec),frame:w(0xb3e2e8),random:w(0xa74c08),digest:hash.toString(16)},commands:p.renderer.commands.map(c=>({asset:[...p.assets.values()].find(a=>a.graph?.texture===c.texture.texture)?.source,text:c.texture.text,points:c.points,uv:c.uv,identity:c.identity,mode:c.mode,tint:c.tint,opaque:c.opaque}))};
      });
      const stem=`${item.name}-${originalFrame}`;writeFileSync(resolve(output,stem+'.png'),Buffer.from(data.png.split(',')[1],'base64'));delete data.png;writeFileSync(resolve(output,stem+'.json'),JSON.stringify(data));
      const row={name:item.name,originalFrame,...data.state};if(baseline)assert.deepEqual(row,baseline.find(c=>c.name===row.name&&c.originalFrame===originalFrame),'录像游标、计时、随机数与对象摘要');report.cases.push(row);console.log(JSON.stringify(row));
    }
  }
  assert.equal(report.cases.length,27);assert.equal(report.errors.length,0);report.result='passed';report.baselineStateMatched=!!baseline;
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);}
finally{await driver.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
