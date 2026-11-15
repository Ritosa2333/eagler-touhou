import {test} from 'vitest';
import assert from 'node:assert/strict';
import {englishScriptQuotes} from '../server/th075-english-static.mjs';
import {cryptTh075} from '../server/th075-thcrap-compiler.mjs';

test('English reflow preserves every word while retaining original page identities',()=>{
 const original=cryptTh075(Buffer.from('B0\nP(XX)"jp story"\nB20\nR4\nN()"jp page 1"\nR6\nN()"jp page 2"\n'));
 const english=Buffer.from('B0\nP(YY)"English story"\nB20\nR20\nN()"One two three"\nR30\nN()"four five six"\nR40\nN()"seven eight nine"\n');
 const result=englishScriptQuotes(original,english);
 assert.equal(result.quotes[0],'English story');
 assert.equal([result.quotes[1],result.quotes[2]].join(' '),'One two three four five six seven eight nine');
 assert.equal(result.reflowed,1);
 assert.throws(()=>englishScriptQuotes(original,Buffer.from(english.toString().replace('P(YY)','E(YY)'))),/speaker mismatch/);
});
