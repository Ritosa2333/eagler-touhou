import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';

export async function loadTh08PracticeLicense(root){
  const path=resolve(root,'ports/th08/web/cpp/game/PracticeLicense.hpp'),source=await readFile(path);
  const match=source.toString('utf8').match(/practice_license=("(?:[^"\\]|\\.)*");/);assert.ok(match);
  const text=JSON.parse(match[1]);assert.ok(text.startsWith('MIT License\r\n')&&text.endsWith('SOFTWARE.'));
  return{text,source:{path,bytes:source.length,sha256:createHash('sha256').update(source).digest('hex')}};
}

export const TH08_PRACTICE_DISPLAY=Object.freeze({menuKey:'F12',hitbox:19,about:25,license:26,offset:{x:32,y:16},
  cases:[{id:'overlay-disabled',enabled:false},{id:'overlay-enabled',enabled:true}],loadLicense:loadTh08PracticeLicense,
  fields:{option:49,generation:76,bullets:89,lasers:90,index:91,age:92,x:93,y:94,hitX:95,hitY:96,playerWidth:97,left:98,vertices:102,
    aboutLines:103,licenseBytes:108,licenseHash:109,licenseVertices:110,licenseFlag:63},
  source:'ThpracUi::draw_advanced/license_text | PracticeHitbox::RenderBtHitbox | real ImGui vertex submission',
});
