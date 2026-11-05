import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import path from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

test('Extra green UFO thin warning survives WebGL rasterization at the native pixel centers',async()=>{
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
      const {Renderer}=await import('/src/renderer.mjs');
      // stgenm07 script44: sprite51 is 512x32 in a 256x256 texture,
      // scale(1,.032), additive blend, left anchor, rotating mode1.
      // A synthetic soft alpha profile makes the pixel-center contract
      // portable; the original image stays outside the repository.
      const r=new Renderer(document.querySelector('canvas'),{stgenm07:{sprites:{51:{entry:17,x:0,y:0,w:512,h:32}}}}),g=r.gl;
      const texture=g.createTexture(),pixels=new Uint8Array(256*256*4);
      for(let y=0;y<32;y++)for(let x=0;x<256;x++)pixels.set([255,255,255,y>=8&&y<24?255:0],(y*256+x)*4);
      g.bindTexture(g.TEXTURE_2D,texture);g.texImage2D(g.TEXTURE_2D,0,g.RGBA,256,256,0,g.RGBA,g.UNSIGNED_BYTE,pixels);
      r.textures.set('stgenm07:17',{texture,w:256,h:256,defaultWrap:g.REPEAT});
      const a={name:'stgenm07',sprite:51,layer:12,mode:1,blend:1,rotation:0,rotate:true,x:0,y:0,sx:1,sy:Math.fround(.032),u:.95,v:0,visible:true,alpha:1,r:1,g:1,b:1,anchorX:1,anchorY:0};
      const rows=[];
      for(const angle of [0,Math.PI/2,.35])for(const alpha of [1,.25,0]){
        a.rotation=Math.fround(angle);a.alpha=alpha;const before=JSON.stringify(a);
        r.begin([0,0,0,1]);r.state(a,100,100);r.flush();
        const out=new Uint8Array(640*480*4);g.readPixels(0,0,640,480,g.RGBA,g.UNSIGNED_BYTE,out);
        let maximum=0,lit=0;
        for(let y=105;y<440;y++)for(let x=105;x<600;x++){
          // Include the two axis-aligned center lines as well.
          const value=out[((479-y)*640+x)*4];maximum=Math.max(maximum,value);if(value>200*alpha&&alpha>0)lit++;
        }
        for(let n=120;n<400;n++)for(const [x,y]of [[n,100],[100,n]]){
          const value=out[((479-y)*640+x)*4];maximum=Math.max(maximum,value);if(value>200*alpha&&alpha>0)lit++;
        }
        rows.push({angle,alpha,maximum,lit,unchanged:before===JSON.stringify(a),error:g.getError()});
      }
      return rows;
    });
    assert.deepEqual(errors,[]);
    for(const row of rows){
      assert.equal(row.error,0,JSON.stringify(row));assert.ok(row.unchanged,'drawing preserves ANM state');
      if(row.alpha===0)assert.equal(row.maximum,0,'faded warnings remain hidden');
      else{
        assert.ok(Math.abs(row.maximum-255*row.alpha)<2,JSON.stringify(row));
        assert.ok(row.lit>200,JSON.stringify(row));
      }
    }
  }finally{await browser?.close();await new Promise(done=>server.close(done));}
});
