// New required atomic update failure: actual online/install/provider code; explicit in-memory archive-store seam.
import test from'node:test';import assert from'node:assert/strict';
import{loadOnlineResources}from'../../site/runtime/online-resources.mjs';
import{fixture,packageZip,refreshDescriptor,sha}from'../dat-ogg/file-fixture.mjs';
test('a fully hashed archive whose descriptor revision disagrees with the deployment never replaces the accepted cache',async()=>{
 const old=fixture(),original=packageZip(old),next=fixture();next.descriptor.revision='wrong-revision-not-the-publication';refreshDescriptor(next);const archive=packageZip(next);
 const accepted={revision:old.descriptor.revision,bytes:original.length,sha256:sha(original),blob:new Blob([original])};
 let saved=accepted,commits=0;const store={async readCurrent(){return saved;},async commit(value){commits++;saved=value;}};
 const deployment={schema:'th128-online-deployment/1',game:'th128',resource:{url:'/new.zip',revision:'expected-publication-revision',bytes:archive.length,sha256:sha(archive)}};
 await assert.rejects(loadOnlineResources(deployment,{pageURL:'https://th128.invalid/index.html',store,fetchImpl:async()=>new Response(archive,{headers:{'Content-Length':String(archive.length)}})}),/revision/i);
 assert.equal(commits,0,'descriptor mismatch must fail before persistent commit');assert.equal(saved,accepted);
 assert.deepEqual(new Uint8Array(await saved.blob.arrayBuffer()),original);
});
test('a matching fully verified deployment still installs and reopens through the real provider',async()=>{
 const f=fixture(),archive=packageZip(f);let saved=null,commits=0,downloads=0;
 const store={async readCurrent(){return saved;},async commit(v){commits++;saved=v;}},deployment={schema:'th128-online-deployment/1',game:'th128',resource:{url:'/new.zip',revision:f.descriptor.revision,bytes:archive.length,sha256:sha(archive)}};
 const options={pageURL:'https://th128.invalid/',store,fetchImpl:async()=>{downloads++;return new Response(archive,{headers:{'Content-Length':String(archive.length)}});}};
 const first=await loadOnlineResources(deployment,options);assert.equal(first.source,'download');assert.equal(first.provider.revision,f.descriptor.revision);first.provider.dispose();
 const second=await loadOnlineResources(deployment,options);assert.equal(second.source,'cache');assert.equal(downloads,1);assert.equal(commits,1);
 assert.deepEqual([...new Uint8Array(await(await second.provider.fetch('https://th128.invalid/assets/runtime/front.anm')).arrayBuffer())],[11,22,33,44,55,66,77,88]);second.provider.dispose();
});
