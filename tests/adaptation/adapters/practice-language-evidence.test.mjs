import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,rmSync} from 'node:fs';
import {join} from 'node:path';
import {tmpdir} from 'node:os';
import {createHash} from 'node:crypto';
import {assertPracticeLanguageReview} from './practice-language-evidence.mjs';
const product={game:'th08',variant:'normal'},hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const audit=()=>({status:'passed',channels:{pageErrors:[],consoleErrors:[],httpErrors:[],protocolErrors:[],responseErrors:[],teardownErrors:[]}});
function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'practice-language-'));t.after(()=>rmSync(directory,{recursive:true,force:true}));
  const write=(name,bytes)=>{const path=join(directory,name);writeFileSync(path,bytes);return{path,bytes:bytes.length,sha256:hash(bytes)};};
  const font=write('unifont.otf',Buffer.from('font fixture')),identity={runtime:{wasmSha256:'a'.repeat(64)},data:{bytes:1,sha256:'b'.repeat(64)}};
  const source={schema:'eagler-touhou/practice-browser/1',status:'passed',startedAt:'2026-10-24T00:00:00Z',finishedAt:'2026-10-24T00:10:00Z',
    finalErrorAudit:audit(),teardownErrors:[],httpFailures:[],sharedInputs:{'/shared/unifont.otf':font},sharedInputsPostflight:{'/shared/unifont.otf':font},
    targets:[{...product,status:'passed',feature:true,identity,postflight:identity,finalErrorAudit:audit(),locales:[]}]};
  const human={schema:'eagler-touhou/scene-review/1',...product,reviewer:'human',recordedAt:'2026-10-24T01:00:00Z',groups:[{id:'practice-text',verdict:'passed',materials:[]}]};
  const png=Buffer.alloc(24);Buffer.from('89504e470d0a1a0a','hex').copy(png);png.writeUInt32BE(640,16);png.writeUInt32BE(480,20);
  for(const [index,locale] of ['ja-JP','en-US','zh-CN'].entries()){
    const image=write(locale+'.png',png);human.groups[0].materials.push({locale,...image});
    source.targets[0].locales.push({epoch:index+1,enabled:true,locale,status:'passed',screenshot:locale+'.png',before:{open:false},
      opened:{open:true,options:{thpracEnabled:true,thpracLocale:locale},events:[{open:true}]},closed:{open:false,events:[{open:true},{open:false}]},
      gameplay:{game:'th08',phase:{name:'stage'}},practiceSnapshot:{state:{enabled:true}},
      responseIntegrity:{responses:[{url:'https://fixture.invalid/shared/unifont.otf',status:200,contentIdentity:'passed',actual:{bytes:font.bytes,sha256:font.sha256}}]}});
  }
  const save=()=>({product,sourceReport:write('report.json',Buffer.from(JSON.stringify(source))),review:write('review.json',Buffer.from(JSON.stringify(human)))});
  return{source,human,font,save};
}
test('one grouped review binds exact screenshots, native locales, loaded font and original capture time',t=>{
  const f=fixture(t),result=assertPracticeLanguageReview(f.save());assert.equal(result.observations.length,3);
  assert.equal(result.sourceRun.startedAt,f.source.startedAt);assert.equal(result.reviewedAt,f.human.recordedAt);
});
for(const [name,mutate] of Object.entries({
  'no human approval':f=>f.human.groups[0].verdict='pending',
  'different game':f=>f.human.game='th10',
  'no reviewer':f=>f.human.reviewer='',
  'missing locale':f=>f.human.groups[0].materials.pop(),
  'duplicate locale':f=>f.human.groups[0].materials[2]=f.human.groups[0].materials[1],
  'changed pixels':f=>writeFileSync(f.human.groups[0].materials[0].path,'new image'),
  'other execution':f=>f.source.targets[0].locales[0].screenshot='other.png',
  'bad native locale':f=>f.source.targets[0].locales[0].opened.options.thpracLocale='en-US',
  'menu did not open':f=>f.source.targets[0].locales[0].opened.open=false,
  'menu did not close':f=>f.source.targets[0].locales[0].closed.open=true,
  'no native menu event':f=>f.source.targets[0].locales[0].opened.events=[],
  'font not delivered':f=>f.source.targets[0].locales[0].responseIntegrity.responses=[],
  'font wrong bytes':f=>f.source.targets[0].locales[0].responseIntegrity.responses[0].actual.sha256='c'.repeat(64),
  'font changed on disk':f=>writeFileSync(f.font.path,'other'),
  'failed browser audit':f=>f.source.targets[0].finalErrorAudit.status='failed',
  'hidden raw error':f=>f.source.finalErrorAudit.channels.pageErrors.push('error despite passed summary'),
  'late target error':f=>f.source.targets[0].finalErrorAudit.channels.pageErrors.push('late error'),
  'review before capture':f=>f.human.recordedAt='2026-10-23T00:00:00Z',
  'changed Runtime':f=>f.source.targets[0].postflight={runtime:{wasmSha256:'c'.repeat(64)}},
}))test(`practice locale review rejects ${name}`,t=>{const f=fixture(t);mutate(f);assert.throws(()=>assertPracticeLanguageReview(f.save()));});
