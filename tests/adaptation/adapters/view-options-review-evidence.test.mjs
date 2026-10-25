import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,rmSync} from 'node:fs';
import {join,dirname,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {createHash} from 'node:crypto';
import {assertViewOptionsReviewApproval,viewOptionsReviewMaterials} from './view-options-review-evidence.mjs';
import {assessCompleteEvidence} from '../complete-evidence.mjs';

function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'view-options-review-'));
  t.after(()=>{assert.equal(dirname(resolve(directory)),resolve(tmpdir()));rmSync(directory,{recursive:true,force:true});});
  const materials=['disabled','focused','enabled'].map((name,index)=>{
    const png=Buffer.alloc(25);Buffer.from('89504e470d0a1a0a','hex').copy(png);png.write('IHDR',12);
    png.writeUInt32BE(640,16);png.writeUInt32BE(480,20);png[24]=index;
    const path=join(directory,name+'.png');writeFileSync(path,png);
    return{name,path,bytes:png.length,sha256:createHash('sha256').update(png).digest('hex')};
  }),product={game:'th08',variant:'normal'},finishedAt='2026-10-25T00:00:00Z';
  return{directory,product,materials,finishedAt,human:{schema:'eagler-touhou/scene-review/1',...product,reviewer:'test reviewer',recordedAt:'2026-10-25T01:00:00Z',
    groups:[{id:'player-marker',verdict:'passed',materials:structuredClone(materials)}]}};
}

test('one player-marker conclusion binds all three exact views',t=>{
  const f=fixture(t),result=assertViewOptionsReviewApproval(f);
  assert.equal(result.materials.length,3);assert.equal(result.group,'player-marker');
});
for(const [name,change]of Object.entries({
  pending:f=>f.human.groups[0].verdict='pending',
  'missing view':f=>f.human.groups[0].materials.pop(),
  'duplicate view':f=>f.human.groups[0].materials[2]=f.human.groups[0].materials[0],
  'other execution':f=>f.human.groups[0].materials[0].path=f.human.groups[0].materials[1].path,
  'changed pixels':f=>writeFileSync(f.materials[0].path,Buffer.alloc(f.materials[0].bytes)),
  'changed digest':f=>f.human.groups[0].materials[0].sha256='0'.repeat(64),
  'before capture':f=>f.human.recordedAt='2026-10-24T00:00:00Z',
  'missing reviewer':f=>f.human.reviewer='',
  'other title':f=>f.human.game='th10',
  'other variant':f=>f.human.variant='multiplayer',
  'extra group verdict':f=>f.human.groups.push(structuredClone(f.human.groups[0])),
}))test('player-marker review rejects '+name,t=>{const f=fixture(t);change(f);assert.throws(()=>assertViewOptionsReviewApproval(f));});

test('three approved pictures cannot replace the original machine proof',t=>{
  const f=fixture(t),path=join(f.directory,'source.json'),bytes=Buffer.from(JSON.stringify({schema:'eagler-touhou/offline-runtime-browser/1',
    status:'passed',scope:f.product,startedAt:f.finishedAt,finishedAt:f.finishedAt,checks:[{id:'RUNTIME-06',machineComplete:true}]}));
  writeFileSync(path,bytes);
  assert.throws(()=>viewOptionsReviewMaterials({product:f.product,sourceReport:{path,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')}}));
});

test('central completion rejects a claimed human review without bound original artifacts',()=>{
  const entry={id:'RUNTIME-06',status:'passed',complete:true,observation:{}},driver={schema:'eagler-touhou/view-options-review/1',status:'passed',
    scope:{game:'th08',variant:'normal'},newBrowserRun:false,checks:[entry],evidence:{}};
  const proof=assessCompleteEvidence({driver,entry,product:driver.scope});assert.equal(proof.available,true);assert.equal(proof.valid,false);
});
