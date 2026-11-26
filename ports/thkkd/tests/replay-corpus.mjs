import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdirSync,readFileSync,readdirSync,writeFileSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {nativeReplay} from '../web/replay.mjs';
const archive=process.argv[2];if(!archive)throw Error('请提供已有授权录像所在的历史归档目录');
const report={schema:'thkkd/replay-parser-corpus/1',cases:[]};
for(const folder of ['replay','额外录像'])for(const name of readdirSync(resolve(archive,'original-game',folder)).filter(name=>name.endsWith('.rpy')).sort()){
 const bytes=new Uint8Array(readFileSync(resolve(archive,'original-game',folder,name))),decoded=nativeReplay(bytes);assert.deepEqual(decoded.native,bytes);report.cases.push({id:report.cases.length+1,sha256:createHash('sha256').update(bytes).digest('hex'),count:decoded.count,character:decoded.description.character,shot:decoded.description.shot,difficulty:decoded.description.difficulty,lastStage:decoded.description.lastStage});
}
assert.equal(report.cases.length,17);report.result='passed';const output=resolve(import.meta.dirname,'../build/quality-mp/replay-corpus.json');mkdirSync(dirname(output),{recursive:true});writeFileSync(output,JSON.stringify(report,null,2));console.log(JSON.stringify({result:report.result,cases:report.cases.length}));
