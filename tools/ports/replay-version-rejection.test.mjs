import test from 'node:test';
import assert from 'node:assert/strict';
import {TH08_REPLAY_SEMANTICS} from '../../tests/adaptation/adapters/titles/th08-replay-format.mjs';
import {unsupportedReplayVersion,assertReplayVersionRejection} from './replay-version-rejection.mjs';
const contract=TH08_REPLAY_SEMANTICS.pointerExtension.rejection;
function fixture(){
  const bytes=Buffer.alloc(100,71);bytes.writeUInt32LE(2,bytes.length-12);
  const {bytes:bad,mutation}=unsupportedReplayVersion(bytes,contract),sent={protocol:'eagler-touhou/1',game:'th08',epoch:3,command:'write',path:contract.path,request:'bad-version',bytes:Array.from(bad)};
  return{bytes,contract,game:'th08',epoch:3,e:{status:'passed',mutation,sent,responses:[{protocol:sent.protocol,game:'th08',epoch:3,request:sent.request,ok:false,error:contract.error}],
    barrier:{protocol:sent.protocol,game:'th08',epoch:3,request:'after-bad-version',ok:true},lists:{before:[{path:'replay/th8_01.rpyx',size:100}],after:[{path:'replay/th8_01.rpyx',size:100}]},originalBefore:Array.from(bytes),originalAfter:Array.from(bytes)}};
}
test('unknown version changes exactly four declared footer bytes and requires an explicit live rejection',()=>{
  const args=fixture(),expected=assertReplayVersionRejection(args.e,args);
  for(let i=0;i<args.bytes.length;i++)if(i<88||i>=92)assert.equal(expected.bytes[i],args.bytes[i]);
  assert.equal(expected.bytes.readUInt32LE(88),0xffffffff);
});
for(const [name,mutate] of Object.entries({
  'wrong sent bytes':a=>a.e.sent.bytes[0]^=1,
  'accepted import':a=>a.e.responses[0].ok=true,
  'unrelated failure':a=>a.e.responses[0].error='Error: Out of memory',
  'no explicit response':a=>a.e.responses=[],
  'foreign request':a=>a.e.responses[0].request='other',
  'foreign epoch':a=>a.e.responses[0].epoch=4,
  'channel stalled':a=>a.e.barrier.ok=false,
  'file created':a=>a.e.lists.after.push({path:contract.path,size:100}),
  'valid file changed':a=>a.e.originalAfter[0]^=1,
  'false mutation receipt':a=>a.e.mutation.offset++,
}))test(`version rejection refuses ${name}`,()=>{const a=fixture();mutate(a);assert.throws(()=>assertReplayVersionRejection(a.e,a));});
