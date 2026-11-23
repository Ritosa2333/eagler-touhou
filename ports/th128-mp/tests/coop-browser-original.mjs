import assert from 'node:assert/strict';
import {mkdir,readdir,writeFile} from 'node:fs/promises';
import {join} from 'node:path';

export async function verifyOriginalOpening({pages,child,until,rank,work}){
  const views=await Promise.all(pages.map(async page=>(await child(page)).f));
  await Promise.all(views.map(f=>f.evaluate(async()=>{
    const url=new URL('ports/th128/site/runtime/renderer-owned-ending-v2.mjs',location.href),{WebGlRenderer}=await import(url.href);
    const render=WebGlRenderer.prototype.render;
    const proof=window.__originalOpening={logos:[],ranks:[],fps:null};
    WebGlRenderer.prototype.render=function(rows,options){
      const result=render.call(this,rows,options);
      if(options?.world&&this.canvas.id==='game')for(const r of rows){
        if(r.bank===30&&!proof.logos.includes(r.script))proof.logos.push(r.script);
        if(r.bank===5&&r.script>=66&&r.script<=70&&r.y<42&&!proof.ranks.includes(r.script))proof.ranks.push(r.script);
        if(r.drawPass==='original-fps')proof.fps=rows.filter(x=>x.drawPass==='original-fps').map(x=>String.fromCharCode(x.sprite-98+32)).join('');
      }
      return result;
    };
  })));
  const result=await until('original stage prompt, top difficulty and measured FPS in real renderer',async()=>{
    const list=await Promise.all(views.map(f=>f.evaluate(()=>window.__originalOpening)));
    return list.every(r=>r.logos.includes(2)&&r.logos.includes(3)&&r.ranks.includes(66+rank)&&/^\d+\.\d+fps$/.test(r.fps))?list:null;
  },18000);
  if(rank!==4){
    const layout=await pages[0].evaluate(()=>{const tool=document.querySelector('.room-workspace-viewport-tools[data-th128-coop]'),view=tool?.closest('.game-viewport');
      if(!tool?.getClientRects().length||!view)return true; // The full-screen launcher uses separate upper-edge controls.
      return tool.getBoundingClientRect().right<view.getBoundingClientRect().left+view.getBoundingClientRect().width*.4;
    });
    assert.equal(layout,true,'room tools no longer cover the lower right HUD');
  }
  await (await views[0].$('#game')).screenshot({path:join(work,'opening-and-fps.png')});
  await writeFile(join(work,'original-opening.json'),JSON.stringify(result,null,2));return result;
}

export async function verifyInGameReplaySave({page,frame,work,until,gameOver=false}){
  const dir=join(work,'unexpected-menu-downloads');await mkdir(dir);
  const cdp=await page.createCDPSession();await cdp.send('Browser.setDownloadBehavior',{behavior:'allow',downloadPath:dir});
  const status=await frame.$eval('#game',c=>c.dataset.frame);
  const wait=async(phase,slot)=>until(`original replay menu phase ${phase} slot ${slot}`,()=>frame.$eval('#game',(c,p,s)=>c.dataset.replaySave===String(p)&&c.dataset.replaySaveSlot===String(s),phase,slot));
  const key=k=>page.keyboard.press(k);
  try{
    await wait(1,0);await frame.focus('#game');
    await (await frame.$('#game')).screenshot({path:join(work,'original-replay-slots.png')});
    // Screenshot and a further 300ms ensure the authored entry gate has completed, even on a slow renderer.
    await new Promise(r=>setTimeout(r,320));await key('ArrowDown');await wait(1,1);await key('Enter');await wait(2,1);
    await (await frame.$('#game')).screenshot({path:join(work,'original-replay-name.png')});
    await new Promise(r=>setTimeout(r,320));await key('z');
    // First cell is A. From it wrap to row seven and the last (OK) column.
    await key('ArrowUp');await key('ArrowLeft');await key('Enter');
    await until('commit No.02 to browser storage',()=>frame.evaluate(()=>new Promise((resolve,reject)=>{
      const opening=indexedDB.open('th128-multiplayer-replay-shelf-v1');opening.onerror=()=>reject(opening.error);
      opening.onsuccess=()=>{const db=opening.result,tx=db.transaction('slots'),rows=tx.objectStore('slots').getAll();
        tx.oncomplete=()=>{db.close();resolve(rows.result.some(row=>row.slot===1&&row.playerName==='A'));};
        tx.onabort=()=>{db.close();reject(tx.error);};};
    })));
    assert.doesNotMatch(await frame.$eval('#status',el=>el.textContent),/录像已保存/,'success is silent');
    if(gameOver){await wait(1,1);await new Promise(r=>setTimeout(r,320));await key('Escape');}
    await until('leave local replay menu without closing the game',()=>frame.$eval('#game',c=>!c.dataset.replaySave));
    assert.equal(await frame.$eval('#game',c=>c.dataset.frame),status,'save keeps the sealed World on its original frame');
    assert.deepEqual(await readdir(dir),[],'native Save must not download a file');
    if(gameOver)assert.equal(await frame.$eval('#game',c=>c.dataset.resultMenu),'team-over','Escape returns to the original Game Over screen');
    await writeFile(join(work,'original-save-verified.json'),JSON.stringify({slot:2,gameOver,frame:Number(status),autoDownloads:0,successToast:false}));
  }finally{await cdp.detach();}
}
