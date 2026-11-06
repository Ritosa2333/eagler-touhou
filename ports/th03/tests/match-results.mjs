import assert from 'node:assert/strict';
import {ConfirmedMatchResults} from '../web/match-results.js';
const reported=[],r=new ConfirmedMatchResults(x=>reported.push(x));
r.stage(10,{generation:1,winner:0});r.rewind(10);r.confirm(11);assert.equal(reported.length,0);
r.stage(11,{generation:1,winner:1});r.stage(12,{generation:1,winner:1});r.confirm(13);
assert.deepEqual(reported,[{start:11,frames:1,winner:1}]);
r.stage(20,null,true);r.stage(21,{generation:1,winner:0});r.confirm(22);assert.equal(reported.length,2);
console.log('confirmed match outcomes and rollback PASS');
