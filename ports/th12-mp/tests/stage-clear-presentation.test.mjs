import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {stageClearAward,stageClearText} from '../src/stage-clear-presentation.mjs';
const native=JSON.parse(fs.readFileSync(new URL('./fixtures/stage-clear-original.json',import.meta.url)));
test('clear-screen digits use the separately sampled original award without adding score twice',()=>{
  for(const row of native.final){
    const h=Array(41).fill(0);h[39]=row.stage;h[19]=row.difficulty;h[4]=row.power;h[5]=row.lives;h[6]=row.bombs;h[20]=row.pointValueStored;
    //421393 writes the base into HUD+6d48;420bbc/420c65 add the final
    //bonus to that same display field after the independently sampled branch.
    const expected=native.base.find(r=>r.stage===row.stage).displayAward+row.displayAward;
    assert.equal(stageClearAward(h),expected,JSON.stringify(row));assert.equal(stageClearAward(h,true),row.stage*1000000);
    const before=[...h],pose={a:{name:'front',sprite:70,visible:true,alpha:.5}};
    const text=stageClearText(h,[pose]);assert.equal(text.text,expected.toLocaleString('en-US'));assert.equal(text.colorARGB>>>24,128);
    assert.deepEqual([text.font,text.x,text.y],[3,224,200]);assert.deepEqual(h,before);
  }
  assert.equal(stageClearText([],[]),null);
});
