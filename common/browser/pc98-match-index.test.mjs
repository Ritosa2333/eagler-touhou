import test from 'node:test';import assert from 'node:assert/strict';
import {createPc98MatchIndex} from './pc98-match-index.mjs';
test('TH03 indexes matches separately from successive rounds',()=>{
 const index=createPc98MatchIndex('03');index.append([{scene:2,round:-1},{scene:1,round:0},{scene:1,round:0},{scene:1,round:1},{scene:0,round:-1},{scene:2,round:-1},{scene:1,round:0}],0);
 assert.deepEqual(index.nodes.map(n=>[n.frame,n.label]),[[1,'第 1 局'],[1,'第 1 回合'],[3,'第 2 回合'],[6,'第 2 局'],[6,'第 1 回合']]);
});
