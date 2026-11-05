import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,root,report,sha} from './helpers.mjs';

function put(c, p, text) {
  const bytes=Buffer.from(text+'\0'); memory(c,p,bytes.length).set(bytes); return p;
}

test('TH11 GameResources opens the original archive and routes ANM, SHT and ECL by name', async()=>{
  const c=await core(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
  const manager=c.resources_create(),data=c.allocate(raw.length),name=c.allocate(128),out=c.allocate(64*1024*1024);
  memory(c,data,raw.length).set(raw); assert.equal(c.resources_open(manager,data,raw.length),1,'THA1 archive');
  assert.equal(c.resources_count(manager),184);
  const required=['ascii.anm','bullet.anm','enemy.anm','front.anm','pl00.anm','pl01.anm','pl00a.sht','pl00b.sht','pl00c.sht','pl01a.sht','pl01b.sht','pl01c.sht','default.ecl','stage01.ecl'];
  for(const item of required)assert.equal(c.resources_has(manager,put(c,name,item)),1,item);
  let checks=0;
  for(const item of ['ascii.anm','bullet.anm','enemy.anm','front.anm','pl00.anm','pl01.anm']){
    const bytes=readFileSync(resolve(root,'reference/assets',item)),used=c.resources_read(manager,put(c,name,item),out,64*1024*1024);
    assert.equal(used,bytes.length,item);assert.deepEqual(Buffer.from(memory(c,out,used)),bytes,item);++checks;
  }
  const anm=c.resources_anm_create(),sht=c.resources_sht_create(),ecl=c.resources_ecl_create();
  try{
    for(const item of ['ascii.anm','bullet.anm','enemy.anm','front.anm','pl00.anm','pl01.anm']){
      assert.equal(c.resources_anm(manager,put(c,name,item),anm),1,item);
      assert.ok(c.resources_anm_count(anm,0)>0,item+' textures');
      assert.ok(c.resources_anm_count(anm,1)>0,item+' sprites');
      assert.ok(c.resources_anm_count(anm,2)>0,item+' scripts'); ++checks;
    }
    for(const item of ['pl00a.sht','pl00b.sht','pl00c.sht','pl01a.sht','pl01b.sht','pl01c.sht']){
      assert.equal(c.resources_sht(manager,put(c,name,item),sht),1,item);
      assert.ok(c.resources_sht_count(sht)>0,item+' groups');++checks;
    }
    assert.equal(c.resources_ecl(manager,put(c,name,'stage01.ecl'),ecl),1);
    assert.ok(c.resources_ecl_count(ecl)>0);++checks;
    report('game-resources',{passed:true,checks,archiveEntries:c.resources_count(manager),anmResources:6,shtResources:6,eclResources:1,scope:'Named archive session boundary: original THA1 decode, six ANM files, six shipped SHT files and stage01 ECL loaded from the original archive. Gameplay construction and stage scheduling remain separate.'});
  } finally { c.resources_anm_delete(anm); c.resources_sht_delete(sht); c.resources_ecl_delete(ecl); c.resources_delete(manager); c.release(data); c.release(name); c.release(out); }
});
