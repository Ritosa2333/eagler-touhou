import assert from 'node:assert/strict';
import {createPc98MatchIndex} from './pc98-match-index.mjs';

const index=createPc98MatchIndex('03');
index.append([{scene:2},{scene:0},{scene:1},{scene:1},{scene:0},{scene:1}],0);
assert.deepEqual(index.nodes.map(node=>node.frame),[2],'multiple rounds form one match');
index.append([{scene:0},{scene:2},{scene:2},{scene:0},{scene:1}],6);
assert.deepEqual(index.nodes.map(node=>node.frame),[2,10],'next selection starts a new match');
assert.deepEqual(index.nodes.map(node=>node.label),['第 1 局','第 2 局']);
const other=createPc98MatchIndex('04');other.append([{scene:2},{scene:1}],0);assert.deepEqual(other.nodes,[]);
console.log('PASS TH03 match boundaries');
