import {test} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp,writeFile,readFile,rm,mkdir} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {developmentResourceRoutes} from '../lib/development-resource-routes.mjs';

test("test-development-resource-routes", async () => {
const directory=await mkdtemp(resolve(tmpdir(),'eagler-development-inputs-'));
try{
  const outside=resolve(directory,'private 素材');await mkdir(outside);
  const font=resolve(outside,'font #日本語%.otf'),data=resolve(outside,'game.data'),music=resolve(outside,'track.ogg');
  await Promise.all([[font,'font'],[data,'retail fixture'],[music,'audio']].map(([path,text])=>writeFile(path,text)));
  const input={shared:{vanillaFont:font,unicodeFont:font},games:{th10:{runtime:'../ports/th10/game.html',
    gameData:{source:data,sha256:'retained-data-identity'},music:{ogg:{base:outside,files:['track.ogg']},midi:{files:[]}}}}};
  const original=structuredClone(input),mapped=developmentResourceRoutes(input,{sourceRoot:resolve(directory,'checkout/launcher')});
  assert.deepEqual(input,original);
  for(const [url,expected] of [[mapped.manifest.shared.vanillaFont,'font'],[mapped.manifest.games.th10.gameData.source,'retail fixture'],
    [mapped.manifest.games.th10.music.ogg.base+'track.ogg','audio']]){
    const pathname=decodeURIComponent(new URL(url,'http://127.0.0.1/').pathname);
    assert.ok(pathname.startsWith('/__development-content/'));
    assert.equal(await readFile(mapped.files.get(pathname),'utf8'),expected);
  }
  assert.equal(mapped.files.size,3);
  assert.equal(mapped.manifest.games.th10.runtime,input.games.th10.runtime);
  assert.equal(mapped.manifest.games.th10.gameData.sha256,'retained-data-identity');
  assert.equal(mapped.files.has('/__development-content/../unlisted.txt'),false);
  input.games.th10.music.ogg.files=['../unlisted.txt'];
  assert.throws(()=>developmentResourceRoutes(input,{sourceRoot:directory}),/outside declared directory/);
  console.log('Development external inputs: PASS');
}finally{await rm(directory,{recursive:true,force:true});}
});
