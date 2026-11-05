import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {asciiGlyphs} from '../src/ascii-presentation.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/ascii-glyphs-native.json',import.meta.url)));
test('Font1..4 sprite mapping, spacing, alignment and filtering match original glyph submission',()=>{
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  for(const [index,record] of native.records.entries()){
    const descriptor=structuredClone(record.input),before=structuredClone(descriptor);
    assert.deepEqual(asciiGlyphs(descriptor),record.glyphs,'native glyph case '+index);
    assert.deepEqual(descriptor,before);assert.equal(record.rngUnchanged,true);
  }
});
