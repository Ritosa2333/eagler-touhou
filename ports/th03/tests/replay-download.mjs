import assert from 'node:assert/strict';
import {downloadReplay} from '../web/replay/download.js';
const calls=[],link={click(){calls.push(['click',this.href,this.download]);}};
const environment={URL:{createObjectURL(blob){calls.push(['create',blob.size]);return 'blob:replay';},revokeObjectURL(url){calls.push(['revoke',url]);}},document:{createElement(tag){assert.equal(tag,'a');return link;}},setTimeout(fn,delay){assert.equal(delay,30000);fn();}};
downloadReplay(new Blob(['data']),'th03-123.th03r',environment);
assert.deepEqual(calls,[['create',4],['click','blob:replay','th03-123.th03r'],['revoke','blob:replay']]);
assert.throws(()=>downloadReplay(new Blob([]),'th03-123.th03r',environment));
assert.throws(()=>downloadReplay(new Blob(['x']),'../th03.th03r',environment));
console.log('Replay browser download PASS');
