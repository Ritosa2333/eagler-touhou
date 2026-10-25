import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,rmSync} from 'node:fs';
import {join,dirname,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {createHash} from 'node:crypto';
import {assertLanguageReviewApproval} from './language-review-evidence.mjs';

function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'language-review-'));
  t.after(()=>{assert.equal(dirname(resolve(directory)),resolve(tmpdir()));rmSync(directory,{recursive:true,force:true});});
  const png=Buffer.alloc(24);Buffer.from('89504e470d0a1a0a','hex').copy(png);png.writeUInt32BE(640,16);png.writeUInt32BE(480,20);
  const materials=['ja','en'].flatMap(locale=>['statistics','dialogue'].map(scene=>{
    const path=join(directory,locale+'-'+scene+'.png');writeFileSync(path,png);
    return{locale,scene,path,bytes:png.length,sha256:createHash('sha256').update(png).digest('hex')};
  })),product={game:'th08',variant:'normal'},finishedAt='2026-10-25T00:00:00Z';
  return{product,materials,finishedAt,human:{schema:'eagler-touhou/scene-review/1',...product,reviewer:'test reviewer',recordedAt:'2026-10-25T01:00:00Z',
    groups:[{id:'text-presentation',verdict:'passed',materials:structuredClone(materials)}]}};
}
test('one human conclusion binds every prepared locale and native scene without per-item verdicts',t=>{
  const f=fixture(t),result=assertLanguageReviewApproval(f);assert.equal(result.materials.length,4);assert.equal(result.group,'text-presentation');
});
for(const [name,change]of Object.entries({
  pending:f=>f.human.groups[0].verdict='pending',
  'missing scene':f=>f.human.groups[0].materials.pop(),
  'duplicate scene':f=>f.human.groups[0].materials[3]=f.human.groups[0].materials[0],
  'other execution':f=>f.human.groups[0].materials[0].path=f.human.groups[0].materials[1].path,
  'changed pixels':f=>writeFileSync(f.materials[0].path,Buffer.alloc(f.materials[0].bytes)),
  'before capture':f=>f.human.recordedAt='2026-10-24T00:00:00Z',
  'missing reviewer':f=>f.human.reviewer='',
  'other title':f=>f.human.game='th10',
}))test('language review rejects '+name,t=>{const f=fixture(t);change(f);assert.throws(()=>assertLanguageReviewApproval(f));});
