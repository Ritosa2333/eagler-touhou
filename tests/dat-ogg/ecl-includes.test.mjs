import test from 'node:test';
import assert from 'node:assert/strict';
import {parseEclIncludes} from '../../site/runtime/ecl-includes.mjs';
function fixture(){
 const b=new Uint8Array(108),v=new DataView(b.buffer),put=(p,s)=>b.set(new TextEncoder().encode(s),p);
 put(0,'SCPT');v.setUint16(4,1,true);v.setUint32(8,36,true);v.setUint16(6,72,true);
 put(36,'ANIM');v.setUint32(40,3,true);put(44,'enemy.anm\0enemy_bf.anm\0boss00.anm\0');
 put(80,'ECLI');v.setUint32(84,2,true);put(88,'default.ecl\0b.ecl\0');
 return b;
}
test('SCPT authored includes preserve animation bank order and ECL dependency order',()=>{
 assert.deepEqual(parseEclIncludes(fixture()),{format:'SCPT/1',includes:{ANIM:['enemy.anm','enemy_bf.anm','boss00.anm'],ECLI:['default.ecl','b.ecl']}});
});
test('invalid header, include extent and unsafe filenames are refused',()=>{
 assert.throws(()=>parseEclIncludes(new Uint8Array(8)),/truncated/);
 const b=fixture();new DataView(b.buffer).setUint16(6,65535,true);assert.throws(()=>parseEclIncludes(b),/bounds/);
 const c=fixture();c.set(new TextEncoder().encode('../bad.anm\0'),44);assert.throws(()=>parseEclIncludes(c),/unsafe/);
});
