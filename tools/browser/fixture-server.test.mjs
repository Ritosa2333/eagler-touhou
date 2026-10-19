import test from 'node:test';
import assert from 'node:assert/strict';
import {connect} from 'node:http2';
import {createFixtureServer} from './fixture-server.mjs';

function withTimeout(promise,timeoutMs,message){
  let timer;
  return Promise.race([promise,new Promise((_,reject)=>{timer=setTimeout(()=>reject(new Error(message)),timeoutMs);})])
    .finally(()=>clearTimeout(timer));
}

test('HTTPS fixture is loopback-only and its scoped fetch preserves exact HTML bytes',async()=>{
  const html=Buffer.from('<!doctype html><meta charset="utf-8"><title>fixture</title>');
  const site=await createFixtureServer((request,response)=>{
    assert.equal(request.url,'/host?runId=one');
    response.writeHead(200,{'Content-Type':'text/html','Content-Length':html.length,'Cache-Control':'no-store'});
    response.end(html);
  },{protocol:'https'});
  try{
    assert.match(site.base,/^https:\/\/127\.0\.0\.1:\d+$/);
    assert.ok(site.tlsIdentity.certificateSha256);
    assert.match(site.tlsIdentity.subjectAltName,/127\.0\.0\.1/);
    assert.match(site.tlsIdentity.subjectAltName,/localhost/);
    const response=await site.fetch(site.base+'/host?runId=one');
    assert.equal(response.status,200);
    assert.equal(response.headers.get('cache-control'),'no-store');
    assert.deepEqual(Buffer.from(await response.arrayBuffer()),html);
    await assert.rejects(site.fetch('https://example.com/'),/Fixture fetch is restricted to this loopback server/);
  }finally{await site.close();}
});

test('HTTP fixture uses the same loopback server contract without TLS state',async()=>{
  const body=Buffer.from([0,1,2,255]);
  const site=await createFixtureServer((_request,response)=>response.end(body));
  try{
    assert.match(site.base,/^http:\/\/127\.0\.0\.1:\d+$/);
    assert.equal(site.tlsIdentity,null);
    const response=await site.fetch(site.base+'/resource');
    assert.equal(response.status,200);
    assert.deepEqual(Buffer.from(await response.arrayBuffer()),body);
  }finally{await site.close();}
});

test('fixture server refuses non-loopback binds',async()=>{
  await assert.rejects(createFixtureServer(()=>{},{host:'0.0.0.0'}),/bind only IPv4 loopback/);
});

test('scoped fetch handles empty responses and bounds a stalled request',async()=>{
  const site=await createFixtureServer((request,response)=>{
    if(request.url==='/empty'){response.writeHead(204).end();return;}
  },{protocol:'https'});
  try{
    const empty=await site.fetch(site.base+'/empty');
    assert.equal(empty.status,204);
    assert.equal((await empty.arrayBuffer()).byteLength,0);
    await assert.rejects(site.fetch(site.base+'/stalled',{timeout:100}),/timed out after 100 ms/);
  }finally{await site.close();}
});

test('fixture close destroys active HTTPS HTTP/2 sessions',async()=>{
  const site=await createFixtureServer((_request,response)=>{
    response.writeHead(200,{'Content-Type':'text/plain'});
    response.write('held open');
  },{protocol:'https'});
  const client=connect(site.base,{rejectUnauthorized:false});
  client.on('error',()=>{});
  const stream=client.request({':path':'/held'});
  stream.on('error',()=>{});
  try{
    const response=withTimeout(new Promise(resolve=>stream.once('response',resolve)),2000,'HTTP/2 fixture request did not reach its handler');
    stream.end();
    await response;
    await withTimeout(site.close(),2000,'Fixture close hung on an active HTTP/2 session');
  }finally{
    client.destroy();
    await site.close();
  }
});
