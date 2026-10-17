import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join, resolve, sep} from 'node:path';
import {createPublicNicknames} from '../public-nicknames.mjs';
import {createDeepSeekNicknameReviewer} from '../nickname-review.mjs';
import {DatabaseSync} from 'node:sqlite';
import {PUBLIC_NICKNAME_POLICY} from '../nickname-policy.mjs';
import {createNicknameRequestGuard, nicknameAddressScope} from '../nickname-request-guard.mjs';

const DAY=86400000, id=n=>'anonymous_member_'+n, ip=n=>'203.0.113.'+n;
test('durable queue acknowledges immediately, survives restart and retries outages without spending user attempts',async()=>{
 const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-queue-')),file=join(dir,'names.sqlite');let time=100000,calls=0,offline=true;
 const updates=[],options={file,now:()=>time,hourlyLimit:1,review:async()=>{calls++;if(offline)throw Error('outage');return true;},onUpdate:(member,value)=>updates.push(value)};
 let service=createPublicNicknames(options);
 try{
  const queued=await service.enqueue(id(1),ip(1),'待审昵称');assert.equal(queued.status,'queued');assert.equal(queued.pendingName,'待审昵称');assert.equal(queued.name,'');assert.equal(calls,0);
  assert.equal((await service.enqueue(id(1),ip(1),'另一个草稿')).pendingName,'待审昵称');
  service.close();service=createPublicNicknames(options);assert.equal(service.current(id(1)).pendingName,'待审昵称');
  const db=new DatabaseSync(file,{readOnly:true}),job=db.prepare('SELECT * FROM review_queue').get();db.close();assert.notEqual(job.member,id(1));assert.notEqual(job.ip,ip(1));
  await service.processQueue();assert.equal(calls,1);assert.equal(service.current(id(1)).status,'queued');assert.equal(service.current(id(1)).attemptsLeft,null);assert.equal(service.current(id(1)).nextChangeAt,0);
  await service.processQueue();assert.equal(calls,1,'backoff prevents polling the provider');
  time+=30000;offline=false;await service.processQueue();assert.equal(calls,2);assert.equal(service.current(id(1)).name,'待审昵称');assert.equal(service.current(id(1)).status,'approved');assert.equal(updates.length,1);
  service.close();service=createPublicNicknames(options);assert.equal(service.current(id(1)).name,'待审昵称');await service.processQueue();assert.equal(calls,2);
 }finally{service.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
});

test('queued changes retain the approved public name and bounded admission is idempotent',async()=>{
 const service=createPublicNicknames({file:':memory:',maxQueued:1,hourlyLimit:2,review:async()=>true});
 try{
  await service.submit(id(1),ip(1),'原昵称');const queued=await service.enqueue(id(1),ip(1),'新昵称');
  assert.equal(queued.name,'原昵称');assert.equal(queued.pendingName,'新昵称');assert.equal(queued.status,'queued');
  assert.equal((await service.enqueue(id(1),ip(1),'新昵称')).status,'queued');
  assert.equal((await service.enqueue(id(2),ip(2),'其他昵称')).status,'unavailable');
  await service.processQueue();assert.equal(service.current(id(1)).name,'新昵称');assert.ok(service.current(id(1)).nextChangeAt>Date.now());
 }finally{service.close();}
});

test('queue completion commits rejection and dequeue atomically, charging the final retry once',async()=>{
 const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-queue-atomic-')),file=join(dir,'names.sqlite');let time=100000;
 const service=createPublicNicknames({file,now:()=>time,review:async()=>false});
 try{
  await service.submit(id(1),ip(1),'第一次拒绝');await service.enqueue(id(1),ip(1),'第二次拒绝');
  const db=new DatabaseSync(file);db.exec("CREATE TRIGGER fail_dequeue BEFORE DELETE ON review_queue BEGIN SELECT RAISE(ABORT,'test crash'); END;");
  await service.processQueue();assert.equal(service.current(id(1)).status,'queued');assert.equal(service.current(id(1)).attemptsLeft,1);
  db.exec('DROP TRIGGER fail_dequeue');db.close();time+=30000;await service.processQueue();
  assert.equal(service.current(id(1)).status,'cooldown');assert.equal(service.current(id(1)).attemptsLeft,0);assert.equal(service.current(id(1)).nextChangeAt,time+DAY);
  await service.processQueue();assert.equal(service.current(id(1)).nextChangeAt,time+DAY);
 }finally{service.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
});

test('queue reviews at most two names concurrently and respects the shared daily retry budget',async()=>{
 let release,active=0,peak=0,calls=0;const gate=new Promise(r=>release=r);
 const service=createPublicNicknames({file:':memory:',maxConcurrent:2,review:async()=>{calls++;peak=Math.max(peak,++active);await gate;active--;return true;}});
 try{
  for(let n=1;n<=3;n++)await service.enqueue(id(n),ip(n),'昵称'+n);
  const running=service.processQueue();await Promise.resolve();await Promise.resolve();assert.equal(calls,2);await service.processQueue();assert.equal(calls,2);
  release();await running;await service.processQueue();assert.equal(calls,3);assert.equal(peak,2);
 }finally{release();service.close();}
 let time=100000,retries=0;const limited=createPublicNicknames({file:':memory:',now:()=>time,reviewBudget:1,review:async()=>{retries++;throw Error('outage');}});
 try{await limited.enqueue(id(1),ip(1),'待审');await limited.processQueue();time+=30000;await limited.processQueue();time+=30000;await limited.processQueue();assert.equal(retries,1);assert.equal(limited.current(id(1)).status,'queued');}
 finally{limited.close();}
});
test('exact normalized spelling is reviewed once and cached across people, including refusals',async()=>{
  const calls=[],service=createPublicNicknames({file:':memory:',review:async name=>{calls.push(name);return name!=='拒绝';}});
  try{
    assert.equal((await service.submit(id(1),ip(1),' Ａ\u202eb ')).name,'Ab');
    assert.equal((await service.submit(id(2),ip(2),'Ab')).name,'Ab');
    assert.equal((await service.submit(id(3),ip(3),'拒绝')).status,'rejected');
    assert.equal((await service.submit(id(4),ip(4),'拒绝')).status,'rejected');
    assert.deepEqual(calls,['Ab','拒绝']);
    assert.equal((await service.submit(id(5),ip(5),'\u200b\u202e')).status,'rejected');
    assert.equal(calls.length,2);
  }finally{service.close();}
});
test('first approval permits one immediate change, then 24h cooldown persists across restart',async()=>{
  const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-')),file=join(dir,'names.sqlite');
  let time=100000,calls=0,service=createPublicNicknames({file,now:()=>time,review:async name=>{calls++;return name!=='拒绝';}});
  try{
    const first=await service.submit(id(1),ip(1),'甲');assert.equal(first.nextChangeAt,0);
    time++;const second=await service.submit(id(1),ip(1),'乙');assert.equal(second.name,'乙');assert.equal(second.nextChangeAt,time+DAY);
    const changed=second.nextChangeAt;
    time+=DAY-1;assert.equal((await service.submit(id(1),ip(2),'丙')).status,'cooldown');
    assert.equal((await service.submit(id(1),ip(1),'乙')).nextChangeAt,changed);
    service.close();service=createPublicNicknames({file,now:()=>time,review:async name=>{calls++;return name!=='拒绝';}});
    assert.equal(service.current(id(1)).nextChangeAt,changed);
    assert.equal((await service.submit(id(2),ip(2),'甲')).name,'甲');assert.equal(calls,2);
    time++;assert.equal((await service.submit(id(1),ip(1),'拒绝')).status,'rejected');
    assert.equal(service.current(id(1)).nextChangeAt,changed);
    assert.equal((await service.submit(id(1),ip(1),'丙')).name,'丙');
    assert.equal(service.current(id(1)).nextChangeAt,time+DAY);
  }finally{service.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
});

test('one declined review leaves exactly one persisted retry; a second refusal locks for 24h',async()=>{
 const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-retries-')),file=join(dir,'names.sqlite');let time=100000,calls=0;
 const options={file,now:()=>time,hourlyLimit:20,review:async name=>{calls++;return name==='允许';}};let service=createPublicNicknames(options);
 try{
  const failed=await service.submit(id(1),ip(1),'拒绝');assert.equal(failed.status,'rejected');assert.equal(failed.attemptsLeft,1);assert.equal(failed.name,'');
  service.close();service=createPublicNicknames(options);assert.equal(service.current(id(1)).attemptsLeft,1);
  time++;const exhausted=await service.submit(id(1),ip(1),'拒绝');assert.equal(exhausted.status,'cooldown');assert.equal(exhausted.attemptsLeft,0);assert.equal(exhausted.nextChangeAt,time+DAY);assert.equal(calls,1,'cached refusal still consumes the final attempt without another provider call');
  assert.equal((await service.submit(id(1),ip(1),'允许')).status,'cooldown');assert.equal(calls,1);
  service.close();service=createPublicNicknames(options);assert.equal(service.current(id(1)).attemptsLeft,0);
  time+=DAY;assert.equal((await service.submit(id(1),ip(1),'拒绝')).attemptsLeft,1);
  const accepted=await service.submit(id(1),ip(1),'允许');assert.equal(accepted.name,'允许');assert.equal(accepted.nextChangeAt,time+DAY,'final retry cannot grant another free first-name change');
 }finally{service.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
});

test('provider outages do not spend the remaining retry or expose a proposed name',async()=>{
 let fail=false;const service=createPublicNicknames({file:':memory:',review:async()=>{if(fail)throw Error('outage');return false;}});
 try{
  assert.equal((await service.submit(id(1),ip(1),'拒绝')).attemptsLeft,1);fail=true;
  const result=await service.submit(id(1),ip(1),'待审核');assert.equal(result.status,'unavailable');assert.equal(result.attemptsLeft,1);assert.equal(result.name,'');
  assert.equal(service.current(id(1)).attemptsLeft,1);
 }finally{service.close();}
});

test('existing approved profiles migrate without losing their name or immediate first change',async()=>{
 const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-migration-')),file=join(dir,'names.sqlite');let service=createPublicNicknames({file,review:async()=>true});
 try{
  await service.submit(id(1),ip(1),'旧昵称');service.close();
  const old=new DatabaseSync(file);old.exec('DROP INDEX profiles_empty_expiry; ALTER TABLE profiles DROP COLUMN approval_policy; ALTER TABLE profiles DROP COLUMN cooldown_until; ALTER TABLE profiles DROP COLUMN failures; ALTER TABLE profiles DROP COLUMN failure_until;');old.close();
  service=createPublicNicknames({file,review:async()=>true});assert.equal(service.current(id(1)).name,'旧昵称');assert.equal(service.current(id(1)).nextChangeAt,0);
  const updated=await service.submit(id(1),ip(1),'新昵称');assert.equal(updated.name,'新昵称');assert.ok(updated.nextChangeAt>Date.now());
 }finally{service.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
});
test('shared NAT identities get independent 24h cooldowns; generated identities cannot evade IP review budget',async()=>{
  let calls=0;const service=createPublicNicknames({file:':memory:',hourlyLimit:2,dailyLimit:2,review:async()=>{calls++;return true;}});
  try{
    assert.equal((await service.submit(id(1),ip(1),'甲')).status,'approved');
    assert.equal((await service.submit(id(2),ip(1),'乙')).status,'approved');
    assert.equal((await service.submit(id(3),ip(1),'丙')).status,'rate-limited');
    assert.equal((await service.submit(id(4),ip(2),'丙')).status,'approved');
    assert.equal(calls,3);
  }finally{service.close();}
});
test('concurrent identical names share one review, concurrent member edits never race the cooldown',async()=>{
  let release,calls=0;const gate=new Promise(r=>release=r),service=createPublicNicknames({file:':memory:',review:async()=>{calls++;await gate;return true;}});
  try{
    const first=service.submit(id(1),ip(1),'同名'),same=service.submit(id(1),ip(1),'同名'),other=service.submit(id(2),ip(2),'同名');
    assert.equal(first,same);
    assert.equal((await service.submit(id(1),ip(1),'改名')).status,'rate-limited');
    release();assert.equal((await first).name,'同名');assert.equal((await other).name,'同名');assert.equal(calls,1);
  }finally{release();service.close();}
});
test('provider errors fail closed, do not consume 24h, and a global daily budget caps fresh calls',async()=>{
  let calls=0,time=100000;const service=createPublicNicknames({file:':memory:',now:()=>time,reviewBudget:2,review:async()=>{calls++;throw Error('provider failed');}});
  try{
    assert.equal((await service.submit(id(1),ip(1),'甲')).status,'unavailable');assert.equal(service.current(id(1)).nextChangeAt,0);
    time+=30000;await service.submit(id(2),ip(2),'乙');time+=60000;assert.equal((await service.submit(id(3),ip(3),'丙')).status,'unavailable');assert.equal(calls,2);
  }finally{service.close();}
});
test('provider request disables thinking, sends only nickname, caps output and accepts only complete 0/1',async()=>{
  let body;
  const reviewer=createDeepSeekNicknameReviewer({apiKey:'test-only',fetch:async(url,request)=>{
    assert.equal(url,'https://api.deepseek.com/chat/completions');body=JSON.parse(request.body);
    return new Response(JSON.stringify({choices:[{finish_reason:'stop',message:{content:'1'}}]}));
  }});
  assert.equal(await reviewer('灵梦'),true);assert.equal(body.max_tokens,2);assert.deepEqual(body.thinking,{type:'disabled'});
  assert.match(body.messages[0].content,/中国大陆公开展示的游戏昵称审核/);
  assert.match(body.messages[0].content,/拒绝政治人物/);
  assert.equal(body.messages[1].content,'"灵梦"');assert.equal(body.messages.length,2);
  for(const payload of [{choices:[{finish_reason:'length',message:{content:'1'}}]}, {choices:[{finish_reason:'stop',message:{content:'Yes'}}]}]){
    const invalid=createDeepSeekNicknameReviewer({apiKey:'test-only',fetch:async()=>new Response(JSON.stringify(payload))});
    await assert.rejects(invalid('普通昵称'));
  }
});

test('reserved political names cannot be approved by a permissive reviewer or a forged cache',async()=>{
  const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-reserved-')),file=join(dir,'names.sqlite');let calls=0;
  const db=new DatabaseSync(file);db.exec('CREATE TABLE names (name TEXT NOT NULL, policy TEXT NOT NULL, approved INTEGER NOT NULL, PRIMARY KEY(name,policy));');
  db.prepare('INSERT INTO names VALUES (?,?,1)').run('习近平',PUBLIC_NICKNAME_POLICY);db.close();
  const service=createPublicNicknames({file,review:async()=>{calls++;return true;}});
  try{
    for(const [index,name] of ['习近平','習近平','习 近 平','Xi Jin Ping','我是习近平'].entries()){
      const result=await service.submit(id(index+1),ip(index+1),name);
      assert.equal(result.status,'rejected');assert.equal(result.name,'');assert.equal(result.attemptsLeft,1);
    }
    assert.equal(calls,0,'reserved spellings spend no provider tokens');
  }finally{service.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
  const reviewer=createDeepSeekNicknameReviewer({apiKey:'test-only',fetch:async()=>{throw Error('reserved name must not call provider');}});
  assert.equal(await reviewer('习近平'),false);
});

test('policy upgrades quarantine old approvals and remove the old rename lock until re-review',async()=>{
  const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-policy-')),file=join(dir,'names.sqlite');let calls=0;
  const options={file,review:async()=>{calls++;return true;}};let service=createPublicNicknames(options);
  try{
    await service.submit(id(1),ip(1),'旧昵称');await service.submit(id(1),ip(1),'灵梦');
    assert.ok(service.current(id(1)).nextChangeAt>Date.now());service.close();
    const db=new DatabaseSync(file);db.prepare('UPDATE names SET policy=?').run('old-policy');db.prepare('UPDATE profiles SET approval_policy=?').run('old-policy');db.close();
    service=createPublicNicknames(options);
    assert.equal(service.current(id(1)).name,'');assert.equal(service.current(id(1)).status,'idle');assert.equal(service.current(id(1)).nextChangeAt,0);
    const result=await service.submit(id(1),ip(1),'灵梦');assert.equal(result.status,'approved');assert.equal(result.name,'灵梦');assert.equal(calls,3);
    service.close();service=createPublicNicknames(options);assert.equal(service.current(id(1)).name,'灵梦');
    service.close();
    const invalid=new DatabaseSync(file);invalid.prepare('UPDATE profiles SET name=?,cooldown_until=?').run('习近平',Date.now()+DAY);
    invalid.prepare('INSERT INTO names VALUES (?,?,1)').run('习近平',PUBLIC_NICKNAME_POLICY);invalid.close();
    service=createPublicNicknames(options);assert.equal(service.current(id(1)).name,'');assert.equal(service.current(id(1)).nextChangeAt,0);
    assert.equal((await service.submit(id(1),ip(1),'习近平')).status,'rejected');assert.equal(calls,3);
  }finally{service.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
});

test('private provider base preserves /v1 and refuses insecure or credential-bearing endpoints',async()=>{
  const apiKey='test-only-never-public';
  const reviewer=createDeepSeekNicknameReviewer({apiKey,apiBase:'https://provider.example.invalid:8888/v1/',fetch:async(url,request)=>{
    assert.equal(url,'https://provider.example.invalid:8888/v1/chat/completions');
    assert.equal(request.redirect,'error');
    assert.equal(request.headers.Authorization,'Bearer '+apiKey);
    return new Response(JSON.stringify({choices:[{finish_reason:'stop',message:{content:'0'}}]}));
  }});
  assert.equal(await reviewer('拒绝昵称'),false);
  for(const apiBase of ['http://provider.invalid/v1','https://user:password@provider.invalid/v1','https://provider.invalid/v1?key=secret','https://provider.invalid/v1#key','invalid'])
    assert.throws(()=>createDeepSeekNicknameReviewer({apiKey,apiBase}),/plain HTTPS API base/);
});

test('default capacity clears 667 fresh names per day with one provider request at a time',async()=>{
 let calls=0,active=0,peak=0;
 const names=createPublicNicknames({file:':memory:',review:async()=>{calls++;peak=Math.max(peak,++active);await Promise.resolve();active--;return true;}});
 try{
  for(let n=0;n<667;n++) {
   const address=`198.18.${n>>8}.${n&255}`;
   await names.enqueue(id(n),address,'新昵称'+n);await names.processQueue();
   assert.equal(names.current(id(n)).status,'approved');
  }
  assert.equal(calls,667);assert.equal(peak,1);
 }finally{names.close();}
});

test('provider outage pauses the whole queue durably instead of retrying each spelling',async()=>{
 const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-circuit-')),file=join(dir,'names.sqlite');let time=100000,calls=0,offline=true;
 const options={file,now:()=>time,review:async()=>{calls++;if(offline)throw Error('offline');return true;}};
 let names=createPublicNicknames(options);
 try{
  for(let n=0;n<5;n++)await names.enqueue(id(n),ip(n+1),'排队'+n);
  await names.processQueue();assert.equal(calls,1);
  for(let n=0;n<20;n++)await names.processQueue();assert.equal(calls,1);
  names.close();names=createPublicNicknames(options);await names.processQueue();assert.equal(calls,1,'restart preserves provider backoff');
  time+=30000;await names.processQueue();assert.equal(calls,2);
  time+=30000;await names.processQueue();assert.equal(calls,2,'second failure doubles the shared pause');
  time+=30000;offline=false;await names.processQueue();assert.equal(calls,3);
  for(let n=0;n<5;n++)await names.processQueue();assert.equal(calls,7);
  for(let n=0;n<5;n++)assert.equal(names.current(id(n)).status,'approved');
 }finally{names.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
});

test('a source cannot multiply provider calls through many queued jobs and retries',async()=>{
 let time=100000,calls=0;
 const names=createPublicNicknames({file:':memory:',now:()=>time,hourlyLimit:20,providerIpBudget:3,review:async()=>{calls++;throw Error('offline');}});
 try{
  for(let n=0;n<20;n++)await names.enqueue(id(n),ip(1),'积压'+n);
  for(let n=0;n<100;n++){await names.processQueue();time+=60001;}
  assert.equal(calls,3);
  assert.equal(names.current(id(1)).attemptsLeft,null,'transport errors never spend a user retry');
 }finally{names.close();}
});

test('IPv6 /64, mapped IPv4 and reconnects share bounded admission',async()=>{
 assert.equal(nicknameAddressScope('2001:db8:abcd:1::1'),nicknameAddressScope('2001:0db8:abcd:0001:ffff:ffff:ffff:ffff'));
 assert.equal(nicknameAddressScope('::ffff:203.0.113.1'),'203.0.113.1');
 assert.equal(nicknameAddressScope('::ffff:cb00:7101'),'203.0.113.1');
 assert.equal(nicknameAddressScope('fe80::1%eth0'),nicknameAddressScope('fe80::2'));
 let time=100000;const gate=createNicknameRequestGuard({now:()=>time,maxAddresses:1});
 for(let n=0;n<10;n++)assert.equal(gate('2001:db8:1:1::'+(n+1).toString(16)),0);
 assert.ok(gate('2001:db8:1:1::ffff')>time);assert.ok(gate(ip(1))>time);
 time+=10000;assert.equal(gate(ip(1)),0);
 const names=createPublicNicknames({file:':memory:',review:async()=>true});
 try{
  let admitted=0;
  for(let n=0;n<1000;n++)admitted+=(await names.enqueue(id(n),'2001:db8:abcd:1::'+(n+1).toString(16),'占位'+n)).status==='queued';
  assert.equal(admitted,5);assert.equal((await names.enqueue(id(1001),ip(1),'正常用户')).status,'queued');
 }finally{names.close();}
});

test('cache eviction keeps approved profiles public and full profile capacity spends no API calls',async()=>{
 let calls=0;
 const names=createPublicNicknames({file:':memory:',maxNames:2,maxProfiles:3,review:async()=>{calls++;return true;}});
 try{
  for(let n=0;n<3;n++)await names.submit(id(n),ip(n+1),'名字'+n);
  assert.equal(names.current(id(0)).name,'名字0','profile approval is independent of cache retention');
  assert.equal((await names.enqueue(id(4),ip(4),'其他名字')).status,'unavailable');assert.equal(calls,3);
  assert.equal((await names.submit(id(4),ip(4),'其他名字')).status,'unavailable');assert.equal(calls,3);
 }finally{names.close();}
});

test('refusal-only profiles and expired IP buckets are pruned without an approval',async()=>{
 const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-expiry-')),file=join(dir,'names.sqlite');let time=100000;
 const names=createPublicNicknames({file,now:()=>time,review:async()=>false});
 try{
  for(let n=0;n<10;n++)await names.submit(id(n),ip(n+1),'拒绝名字');
  time+=2*DAY;await names.processQueue();
  const db=new DatabaseSync(file,{readOnly:true});
  try{assert.equal(db.prepare('SELECT COUNT(*) AS n FROM profiles').get().n,0);assert.equal(db.prepare('SELECT COUNT(*) AS n FROM buckets WHERE expires<=?').get(time).n,0);}
  finally{db.close();}
 }finally{names.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
});

test('overlapping workers claim a queued name once and a crashed claim becomes available',async()=>{
 const dir=mkdtempSync(join(tmpdir(),'eagler-nickname-lease-')),file=join(dir,'names.sqlite');let release,calls=0,time=100000;
 const gate=new Promise(r=>release=r),options={file,now:()=>time,review:async()=>{calls++;await gate;return false;}};
 const first=createPublicNicknames(options),second=createPublicNicknames(options);
 try{
  await first.enqueue(id(1),ip(1),'审核一次');
  const a=first.processQueue(),b=second.processQueue();await Promise.resolve();await Promise.resolve();assert.equal(calls,1);
  release();await Promise.all([a,b]);assert.equal(first.current(id(1)).attemptsLeft,1);
  await first.enqueue(id(2),ip(2),'崩溃恢复');
  const db=new DatabaseSync(file);db.prepare('UPDATE review_queue SET lease=?,next_attempt=?').run('crashed',time+60000);db.close();
  await second.processQueue();assert.equal(calls,1);time+=60000;await second.processQueue();assert.equal(calls,2);
 }finally{release();first.close();second.close();assert.ok(resolve(dir).startsWith(resolve(tmpdir())+sep));rmSync(dir,{recursive:true,force:true});}
});

test('review response reading stops and cancels at the byte limit, including error bodies',async()=>{
 let chunks=0,cancelled=false;
 const reviewer=createDeepSeekNicknameReviewer({apiKey:'test-only',fetch:async()=>new Response(new ReadableStream({
  pull(controller){chunks++;controller.enqueue(new Uint8Array(4096));},cancel(){cancelled=true;}
 }))});
 await assert.rejects(reviewer('测试昵称'));assert.ok(chunks<=6);assert.equal(cancelled,true);
 let pulls=0,errorCancelled=false;
 const failed=createDeepSeekNicknameReviewer({apiKey:'test-only',fetch:async()=>new Response(new ReadableStream({
  pull(controller){pulls++;controller.enqueue(new Uint8Array(4096));},cancel(){errorCancelled=true;}
 }),{status:503})});
 await assert.rejects(failed('测试昵称'));assert.ok(pulls<=1);assert.equal(errorCancelled,true);
});

test('cached queued verdicts can finish after the daily provider budget is exhausted',async()=>{
 let calls=0;const names=createPublicNicknames({file:':memory:',reviewBudget:1,review:async()=>{calls++;return true;}});
 try{
  await names.enqueue(id(1),ip(1),'同名缓存');await names.enqueue(id(2),ip(2),'同名缓存');
  await names.processQueue();await names.processQueue();
  assert.equal(calls,1);assert.equal(names.current(id(1)).name,'同名缓存');assert.equal(names.current(id(2)).name,'同名缓存');
 }finally{names.close();}
});
