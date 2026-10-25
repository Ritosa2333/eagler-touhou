import test from 'node:test';
import assert from 'node:assert/strict';
import {normalizeLanguageConsumerState} from './language-consumer-state.mjs';

test('a different title maps its own navigation fields without changing shared reader code',()=>{
  const contract={uiSnapshot:{wordCount:5,fields:{page:4,canChoose:2,nativeTick:3},booleans:['canChoose']}};
  const raw={available:true,words:[1,1,1,81,7]};assert.deepEqual(normalizeLanguageConsumerState(raw,contract),{page:7,canChoose:true,nativeTick:81});
  raw.words[2]=2;assert.throws(()=>normalizeLanguageConsumerState(raw,contract));raw.words[2]=1;raw.words[0]=2;assert.throws(()=>normalizeLanguageConsumerState(raw,contract));
});
