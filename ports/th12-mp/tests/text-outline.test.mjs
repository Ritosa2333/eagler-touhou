import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import path from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

test('ending and dialogue rasterize one contour and one foreground at the same baseline',async()=>{
  const root=path.resolve(import.meta.dirname,'..');
  const server=createServer(async(req,res)=>{
    try{
      const pathname=new URL(req.url,'http://localhost').pathname;
      if(pathname==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas width="640" height="480"></canvas>');return;}
      const file=path.resolve(root,'.'+decodeURIComponent(pathname));
      if(!file.startsWith(root+path.sep)||!file.endsWith('.mjs'))throw Error('module path');
      res.setHeader('Content-Type','text/javascript');res.end(await readFile(file));
    }catch(error){res.statusCode=404;res.end(error.message);}
  });
  await new Promise(done=>server.listen(0,'127.0.0.1',done));
  let browser;
  try{
    browser=await launchBrowser();const page=await browser.newPage(),errors=[];
    page.on('pageerror',error=>errors.push(error.message));
    await page.goto('http://127.0.0.1:'+server.address().port);
    const rows=await page.evaluate(async()=>{
      const {Renderer}=await import('/src/renderer.mjs'),renderer=new Renderer(document.querySelector('canvas'),{});
      const {prepareLocalization}=await import('/src/localization/loader.mjs');
      const overlay={schema:'th12-localization-overlay/2',game:'th12',language:'lang_zh-hans',sourceKind:'thcrap',sourceExeSha256:'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417',dialogue:{},dialoguePatches:{},endings:{},endingPatches:{},spells:{},textures:{},musicComments:{}};
      const raw=new TextEncoder().encode(JSON.stringify(overlay)),digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',raw)),b=>b.toString(16).padStart(2,'0')).join('');
      const localization=await prepareLocalization({language:'lang_zh-hans',fontFamily:'monospace',manifest:{schema:'eagler-touhou/thcrap-static-pack/1',game:'th12',language:'lang_zh-hans',runtimeVersion:'independent',sourceRevision:'0'.repeat(24),files:[{path:'/thcrap/th12/overlay.json',bytes:raw.length,sha256:digest}]},readFile:async()=>raw});
      const proto=Object.getPrototypeOf(new OffscreenCanvas(1,1).getContext('2d'));
      const original={fillText:proto.fillText,strokeText:proto.strokeText},calls=[];
      for(const name of Object.keys(original))proto[name]=function(text,x,y,...rest){
        calls.push({name,text,x,y,color:name==='strokeText'?this.strokeStyle:this.fillStyle,width:this.lineWidth,join:this.lineJoin});
        return original[name].call(this,text,x,y,...rest);
      };
      const rows=[];
      try{
        const record={style:0,color:0xffffff,offset:0,spacing:0},metrics={width:512,height:17,sourceWidth:1024,sourceHeight:34};
        for(const localized of [false,true]){
        if(localized)localization.activate();else localization.deactivate();
        for(const [name,raster,expectedY]of [
          ['ending',{menu:true,fontFamily:'monospace',fontWeight:400,fontPixelHeight:32,sourceX:0,sourceY:0,foregroundColor:0x000080,outlineOffsets:[[2,2],[-2,2],[2,-2],[-2,-2]]},0],
          ['dialogue',null,2],
          ['spell',{sourceX:0,sourceY:2,foregroundColor:0xffffff,outlineColor:0x123456,outlineOffsets:[[2,4],[-2,4],[2,0],[-2,0]]},2],
          ['plain',{sourceX:0,sourceY:2,outlineOffsets:[]},2]
        ]){
          calls.length=0;renderer.begin([1,1,1,1]);
          const pose={text:'田日 I 汉字',x:40,y:100,alpha:1,a:{sx:1,sy:1,alpha:1,mode:0,blend:0,anchorX:1,anchorY:1},metrics,record,raster};
          renderer.dialogueLine(pose);renderer.flush();
          const draws=structuredClone(calls),count=calls.length;
          const first=renderer.dialogueTexture(pose.text,record,metrics,raster),second=renderer.dialogueTexture(pose.text,record,metrics,raster);
          const g=renderer.gl,pixels=new Uint8Array(640*480*4);g.readPixels(0,0,640,480,g.RGBA,g.UNSIGNED_BYTE,pixels);
          let changed=0;for(let i=0;i<pixels.length;i+=4)if(pixels[i]<250||pixels[i+1]<250||pixels[i+2]<250)changed++;
          rows.push({name,localized,expectedY,draws,reused:first===second,cacheDraws:calls.length-count,changed,error:g.getError()});
        }
        }
      }finally{localization.deactivate();Object.assign(proto,original);}
      return rows;
    });
    assert.deepEqual(errors,[]);
    for(const row of rows){
      const fills=row.draws.filter(c=>c.name==='fillText'),outlines=row.draws.filter(c=>c.name==='strokeText');
      assert.equal(fills.length,1,row.name+' draws the foreground once');
      assert.equal(outlines.length,row.name==='plain'?0:1,row.name+' has a single contour');
      assert.equal(fills[0].y,row.expectedY,row.name+' foreground baseline');
      if(outlines.length){
        assert.deepEqual([outlines[0].x,outlines[0].y],[fills[0].x,fills[0].y],row.name+' contour follows the glyph');
        assert.equal(outlines[0].width,2);assert.equal(outlines[0].join,'round');
        if(row.name==='spell')assert.equal(outlines[0].color,'#123456');
      }
      assert.ok(row.changed>20,row.name+' reaches WebGL pixels');
      assert.equal(row.error,0);assert.ok(row.reused);assert.equal(row.cacheDraws,0,'cached text is not rasterized again');
    }
  }finally{await browser?.close();await new Promise(done=>server.close(done));}
});
