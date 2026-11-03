import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {SITE_BRAND_FACES,SITE_BRAND_DEFAULT,chooseSiteBrandFace,siteBrandPath} from '../../lib/site-brand.mjs';
import {STATIC_FRONTEND_PACKAGE_FILES,STATIC_APP_SHELL_FILES} from '../../lib/frontend-static-manifest.mjs';

test('approved character pool has exact weighted boundaries and reaches every ordinary character',()=>{
  assert.equal(SITE_BRAND_FACES.length,87);assert.equal(new Set(SITE_BRAND_FACES).size,87);
  assert.equal(chooseSiteBrandFace(()=>0),'face-F02B');assert.equal(chooseSiteBrandFace(()=>.099999),'face-F02B');
  assert.equal(chooseSiteBrandFace(()=>.1),'face-F055');assert.equal(chooseSiteBrandFace(()=>.199999),'face-F055');
  const ordinary=SITE_BRAND_FACES.filter(id=>!['face-F02B','face-F055'].includes(id));
  for(let i=0;i<85;i++){let call=0;assert.equal(chooseSiteBrandFace(()=>call++?.5/85+i/85:.2),ordinary[i]);}
  assert.throws(()=>siteBrandPath('unapproved'));
});

test('every advertised logo ships with attribution, face geometry and both theme palettes',async()=>{
  for(const id of SITE_BRAND_FACES){
    const path=siteBrandPath(id),svg=await readFile(new URL('../../public/'+path,import.meta.url),'utf8');
    assert.ok(STATIC_FRONTEND_PACKAGE_FILES.includes(path));
    assert.ok(svg.includes('plasticparakeet')&&svg.includes('CC BY-NC-SA 3.0'));
    assert.ok(svg.includes('data-face='));
    assert.ok(svg.includes('<style>.wordmark{fill:#202124}@media(prefers-color-scheme:dark){.wordmark{fill:#f0f0f2}}</style>'));
    assert.match(svg,/viewBox="0 0 1000 320"/);
  }
  assert.ok(STATIC_APP_SHELL_FILES.includes(siteBrandPath(SITE_BRAND_DEFAULT)));
  assert.equal(SITE_BRAND_FACES.filter(id=>STATIC_APP_SHELL_FILES.includes(siteBrandPath(id))).length,1);
});
