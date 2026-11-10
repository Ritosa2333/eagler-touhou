// Replay the actual captured input archive through the production exact
// playback path. No substitute simulation, input rewrite or state injection.
import assert from 'node:assert/strict';
import {writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {installFramebufferProof,comparePlayfield} from './framebuffer-proof.mjs';

const fields=game=>[...game.slice(1,5),...game.slice(8)];
export async function replayPrefixOracle({browser,livePage,origin,expected,out,seat,runtimePath='/runtime/',pixels=false,sharedResources=[]}){
  const captured=await livePage.evaluate(pixels=>{
    const w=document.querySelector('iframe').contentWindow,c=w.core;
    const bytes=c.th11_mp_replay_size();
    if(!bytes||bytes>4*1024*1024)throw Error('Missing/bounded native Replay export');
    const view=new Uint8Array(c.memory.buffer,c.th11_mp_replay_data(),bytes);
    let binary='';for(let i=0;i<view.length;i+=32768)binary+=String.fromCharCode(...view.subarray(i,i+32768));
    return {base64:btoa(binary),pixels:pixels?w.__framebufferProof.read():null,recovery:typeof c.th11_mp_recovery_statistics==='function'?
      Array.from(new Uint32Array(c.memory.buffer,c.th11_mp_recovery_statistics(),8)):[]};
  },pixels);
  const bytes=Buffer.from(captured.base64,'base64'),file=resolve(out,`peer-${seat}-actual-inputs.rpy`);
  writeFileSync(file,bytes,{flag:'wx'});
  const result={seat,file,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex'),
    frame:expected.game[4],liveRecovery:captured.recovery,passed:false,runtimePath,
    scope:'Exact production playback vs corrected live exported native fields at the same recorded frame. Not full-world or pixel equality; 1x correctness lane only.'};
  const context=await browser.newContext({viewport:{width:660,height:510},serviceWorkers:'block'});
  try{
    const page=await context.newPage();const cdp=await context.newCDPSession(page);
    await cdp.send('Emulation.setCPUThrottlingRate',{rate:1});
    await page.goto(origin);await page.evaluate(path=>host.open(path),runtimePath);
    await page.waitForFunction(()=>host.ready(),null,{timeout:60000});
    await page.evaluate(async resources=>{await host.configure({replayViewer:true},resources);await host.launch();},sharedResources);
    if(pixels)await page.evaluate(installFramebufferProof);
    await page.evaluate(({base64,frame})=>{
      const w=document.querySelector('iframe').contentWindow,c=w.core;
      const text=p=>{const h=new Uint8Array(c.memory.buffer);let n=p;while(h[n])++n;return new TextDecoder().decode(h.subarray(p,n));};
      c.th11_loop_stop();
      const bytes=Uint8Array.from(atob(base64),x=>x.charCodeAt(0)),p=c.malloc(bytes.length);
      if(!p)throw Error('Replay fixture allocation failed');
      try{new Uint8Array(c.memory.buffer,p,bytes.length).set(bytes);
        if(!c.th11_mp_replay_load(p,bytes.length))throw Error(text(c.th11_mp_error())||'Native Replay load rejected');
      }finally{c.free(p);}
      if(!c.th11_mp_replay_seek(frame))throw Error('Captured boundary outside exported Replay');
      w.__prefixReplay={done:false,frame,error:null,actual:null};
      const original=w.Module.onGameFrame;
      w.Module.onGameFrame=(ok,ms,ticks)=>{
        const proof=w.__prefixReplay;
        if(!ok){proof.error=text(c.th11_mp_error())||text(c.th11_error());proof.done=true;c.th11_loop_stop();return;}
        const now=Array.from(new Uint32Array(c.memory.buffer,c.th11_mp_game_status(),56));
        if(now[4]>=frame){proof.actual=now;proof.done=true;if(now[4]!==frame)proof.error='Playback overshot exact target';c.th11_loop_stop();return;}
        original?.(ok,ms,ticks);
      };
      c.th11_loop_start();
    },{base64:captured.base64,frame:result.frame});
    await page.waitForFunction(()=>document.querySelector('iframe').contentWindow.__prefixReplay?.done,null,{timeout:90000});
    const proof=await page.evaluate(()=>document.querySelector('iframe').contentWindow.__prefixReplay);
    assert.equal(proof.error,null);result.actual=proof.actual;result.expected=expected.game;
    writeFileSync(resolve(out,`peer-${seat}-replay-oracle-fields.json`),JSON.stringify(result,null,2)+'\n',{flag:'wx'});
    assert.deepEqual(fields(proof.actual),fields(expected.game),'Exact Replay and corrected live prefix disagree');
    if(pixels){
      const replay=await page.evaluate(()=>document.querySelector('iframe').contentWindow.__framebufferProof.read());
      const liveBytes=Buffer.from(captured.pixels.base64,'base64'),replayBytes=Buffer.from(replay.base64,'base64');
      for(const [kind,data]of [['live',liveBytes],['replay',replayBytes]])
        writeFileSync(resolve(out,`peer-${seat}-${kind}.rgba`),data,{flag:'wx'});
      result.pixels={...comparePlayfield(liveBytes,replayBytes),
        liveSha256:createHash('sha256').update(liveBytes).digest('hex'),replaySha256:createHash('sha256').update(replayBytes).digest('hex')};
      writeFileSync(resolve(out,`peer-${seat}-pixel-oracle.json`),JSON.stringify(result.pixels,null,2)+'\n',{flag:'wx'});
      assert.equal(result.pixels.equal,true,'Corrected live playfield pixels and exact Replay disagree');
    }
    result.passed=true;return result;
  }finally{await context.close();}
}
