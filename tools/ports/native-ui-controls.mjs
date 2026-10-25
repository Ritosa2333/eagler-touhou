import assert from 'node:assert/strict';
import {decodeNativeControls,nativeControlPoint} from '../../tests/adaptation/adapters/native-controls.mjs';
import {readNativeSnapshotRaw} from '../../tests/adaptation/adapters/native-snapshot.mjs';
import {practiceSnapshotRequest} from '../../tests/adaptation/adapters/practice.mjs';

/** One physical pointer path for native options across titles. */
export function createNativeUiControls({page,frame,runtime,contract,advance,save=async()=>{}}){
  const evidence={schema:'eagler-touhou/native-ui-controls/1',operations:[]};
  const request=practiceSnapshotRequest({runtime,contract:{snapshot:contract.snapshot}});
  const read=async()=>{const raw=await frame.evaluate(readNativeSnapshotRaw,request),state=decodeNativeControls(raw);
    const identity=await frame.evaluate(runtime=>({app:globalThis[runtime.global][runtime.app],documentTimeOrigin:performance.timeOrigin,url:location.href}),runtime);
    return{raw,state,identity};};
  const until=async(label,predicate)=>{const end=Date.now()+5000;let row;do{row=await read();if(predicate(row.state))return row;await (advance?advance():page.waitForTimeout(20));}while(Date.now()<end);
    throw Object.assign(Error(label),{nativeUiEvidence:row});};
  const point=async logical=>{const canvas=await frame.locator('canvas').boundingBox();assert.ok(canvas?.width>0&&canvas.height>0);
    const scale=Math.min(canvas.width/contract.width,canvas.height/contract.height);
    return{canvas,logical,page:{x:canvas.x+(canvas.width-contract.width*scale)/2+logical.x*scale,y:canvas.y+(canvas.height-contract.height*scale)/2+logical.y*scale}};};
  const locate=async(semantic,{allowDisabled=false}={})=>{
    let popupFrames=0,boundaryFrames=0;
    for(let n=0;n<32;n++){
      const row=await until('Native options have not completed drawing',s=>s.open&&s.generation===s.renderedGeneration),item=row.state.controls.find(c=>c.semantic===semantic);
      assert.ok(item,'Native option is not in its current section: '+semantic);
      if(item.visible){try{nativeControlPoint(row.state,semantic,{...contract,allowDisabled});return row;}catch(error){if(!item.enabled&&!allowDisabled)throw error;}}
      if(item.popup){
        assert.ok(popupFrames++<8,`Native popup did not expose a visible target: ${JSON.stringify(item)}`);
        await until('Native popup has not completed its next layout',s=>s.generation>row.state.generation);continue;
      }
      const c=row.state.scroll,direction=item.rect.y<c.top?-1:1;
      if(!(direction<0?c.offset>0:c.offset<c.maximum)){
        assert.ok(boundaryFrames++<3,`Native option is outside its actual scroll range: ${JSON.stringify({item,scroll:c})}`);
        await until('Native child layout did not update after expansion',s=>s.generation>row.state.generation);continue;
      }
      boundaryFrames=0;
      const target=await point({x:(c.left+c.right)/2,y:(c.top+c.bottom)/2});
      await page.mouse.move(target.page.x,target.page.y);await page.mouse.wheel(0,direction*160);
      const after=await until('Native options did not consume browser scrolling',s=>s.scroll.offset!==c.offset);
      evidence.operations.push({kind:'scroll',semantic,before:row,after,target,direction});await save(evidence);
    }throw Error('Native option exceeds the bounded scroll journey: '+semantic);
  };
  return{evidence,read,until,locate,
    async clickDisabled(semantic){
      const before=await locate(semantic,{allowDisabled:true}),item=before.state.controls.find(c=>c.semantic===semantic);assert.equal(item.enabled,false);
      const target=await point(nativeControlPoint(before.state,semantic,{...contract,allowDisabled:true}));await page.mouse.click(target.page.x,target.page.y);
      const after=await until('Disabled native widget did not complete its pointer frames',s=>s.generation>=before.state.generation+2);
      const current=after.state.controls.find(c=>c.semantic===semantic);assert.equal(current.enabled,false);assert.equal(current.activations,item.activations);
      assert.deepEqual(after.identity,before.identity);const receipt={kind:'disabled-click',semantic,before,after,target};evidence.operations.push(receipt);await save(evidence);return receipt;
    },
    async click(semantic){
      const before=await locate(semantic),item=before.state.controls.find(c=>c.semantic===semantic),target=await point(nativeControlPoint(before.state,semantic,contract));
      const hit=await frame.evaluate(({x,y})=>document.elementFromPoint(x,y)===document.querySelector('canvas'),
        await frame.locator('canvas').evaluate((canvas,{logical,width,height})=>{const r=canvas.getBoundingClientRect(),scale=Math.min(r.width/width,r.height/height);return{x:r.x+(r.width-width*scale)/2+logical.x*scale,y:r.y+(r.height-height*scale)/2+logical.y*scale};},
          {logical:target.logical,width:contract.width,height:contract.height}));
      assert.equal(hit,true,'Native control pointer is obstructed');
      await page.mouse.click(target.page.x,target.page.y);
      const after=await until('Native control did not consume its physical click: '+semantic,s=>s.controls.some(c=>c.semantic===semantic&&c.activations>item.activations));
      assert.deepEqual(after.identity,before.identity);const receipt={kind:'click',semantic,before,after,target};evidence.operations.push(receipt);await save(evidence);return receipt;
    },
  };
}
