import assert from 'node:assert/strict';
import {createHash,X509Certificate} from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {createServer as createHttpServer,request as requestHttp} from 'node:http';
import {request as requestHttps} from 'node:https';
import {createSecureServer} from 'node:http2';
import {existsSync} from 'node:fs';
import {mkdtemp,readFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {basename,dirname,join,resolve} from 'node:path';

const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const TEMP_CERT_PREFIX='eagler-fixture-https-';

/** Create a one-day self-signed certificate restricted to the fixture loopback names. */
export function createEphemeralLocalhostCertificate(directory){
  const absoluteDirectory=resolve(directory);
  if(!existsSync(absoluteDirectory))throw new Error(`Certificate directory does not exist: ${absoluteDirectory}`);
  const key=resolve(absoluteDirectory,'localhost-key.pem'),cert=resolve(absoluteDirectory,'localhost-cert.pem');
  const result=spawnSync(process.env.EAGLER_OPENSSL||'openssl',[
    'req','-x509','-newkey','rsa:2048','-nodes','-keyout',key,'-out',cert,'-days','1',
    '-subj','/CN=localhost','-addext','subjectAltName=DNS:localhost,IP:127.0.0.1',
  ],{encoding:'utf8',windowsHide:true});
  if(result.status!==0)throw new Error(`OpenSSL is required to create the ephemeral localhost certificate: ${result.stderr||result.error||'unknown error'}`);
  return{key,cert};
}

function listen(server,port,host){
  return new Promise((accept,reject)=>{server.once('error',reject);server.listen(port,host,accept);});
}

function closeServer(server,sessions=new Set(),sockets=new Set()){
  return new Promise((accept,reject)=>{
    try{server.close(error=>error?reject(error):accept());}
    catch(error){if(error.code==='ERR_SERVER_NOT_RUNNING')accept();else reject(error);return;}
    for(const session of sessions)session.destroy();
    server.closeAllConnections?.();
    for(const socket of sockets)socket.destroy();
  });
}

function requestResponse(request,url,init,certificate){
  return new Promise((accept,reject)=>{
    const options={method:init.method??'GET',headers:init.headers};
    if(certificate)options.ca=certificate;
    if(init.signal)options.signal=init.signal;
    const requestObject=request(url,options,response=>{
      const chunks=[];
      response.on('data',chunk=>chunks.push(Buffer.from(chunk)));
      response.once('error',reject);
      response.once('end',()=>{
        const status=response.statusCode??500,body=[204,205,304].includes(status)?null:Buffer.concat(chunks);
        accept(new Response(body,{status,headers:response.headers}));
      });
    });
    requestObject.once('error',reject);
    const timeout=init.timeout??5000;
    assert.ok(Number.isFinite(timeout)&&timeout>0,'Fixture fetch timeout must be a positive number of milliseconds');
    requestObject.setTimeout(timeout,()=>requestObject.destroy(new Error(`Fixture fetch timed out after ${timeout} ms: ${url}`)));
    if(init.body!==undefined&&init.body!==null)requestObject.write(init.body);
    requestObject.end();
  });
}

/**
 * Start a fixture server on IPv4 loopback. HTTPS uses its own temporary,
 * self-signed certificate; fetch() trusts only that certificate for this
 * server, while browser clients should scope ignoreHTTPSErrors to one context.
 */
export async function createFixtureServer(handler,{protocol='http',port=0,host='127.0.0.1'}={}){
  assert.equal(host,'127.0.0.1','Fixture servers must bind only IPv4 loopback');
  assert.ok(protocol==='http'||protocol==='https','Fixture protocol must be http or https');
  assert.ok(Number.isInteger(port)&&port>=0&&port<=65535,'Fixture port must be an integer from 0 through 65535');
  assert.equal(typeof handler,'function','Fixture request handler is required');

  let certificateDirectory=null,certificateBytes=null,tlsIdentity=null,server,closePromise=null;
  const sessions=new Set(),sockets=new Set();
  try{
    if(protocol==='https'){
      certificateDirectory=await mkdtemp(join(tmpdir(),TEMP_CERT_PREFIX));
      const paths=createEphemeralLocalhostCertificate(certificateDirectory);
      const [key,cert]=await Promise.all([readFile(paths.key),readFile(paths.cert)]);
      const parsed=new X509Certificate(cert);
      assert.ok(parsed.checkIP(host),'Ephemeral certificate must cover the bound loopback IP');
      certificateBytes=cert;
      const spkiSha256=createHash('sha256').update(parsed.publicKey.export({type:'spki',format:'der'})).digest('base64');
      tlsIdentity={certificateSha256:sha256(cert),spkiSha256,
        subjectAltName:parsed.subjectAltName,validTo:parsed.validTo,
        scope:'ephemeral self-signed localhost cert; this fixture only'};
      server=createSecureServer({allowHTTP1:true,key,cert},handler);
      server.on('session',session=>{sessions.add(session);session.once('close',()=>sessions.delete(session));});
    }else server=createHttpServer(handler);
    server.on('connection',socket=>{sockets.add(socket);socket.once('close',()=>sockets.delete(socket));});

    await listen(server,port,host);
    const address=server.address();
    assert.ok(address&&typeof address==='object','Fixture server did not expose its bound address');
    const base=`${protocol}://${host}:${address.port}`;
    const request=protocol==='https'?requestHttps:requestHttp;
    return{
      server,base,protocol,host,port:address.port,tlsIdentity,
      async fetch(input,init={}){
        const url=new URL(input,base);
        assert.equal(url.origin,base,'Fixture fetch is restricted to this loopback server');
        return requestResponse(request,url,init,certificateBytes);
      },
      async close(){
        if(closePromise)return closePromise;
        closePromise=(async()=>{
          await closeServer(server,sessions,sockets);
          if(certificateDirectory){
            const expectedParent=resolve(tmpdir()),actualParent=resolve(dirname(certificateDirectory));
            assert.equal(actualParent.toLowerCase(),expectedParent.toLowerCase(),'Temporary certificate must remain directly under the OS temp directory');
            assert.ok(basename(certificateDirectory).startsWith(TEMP_CERT_PREFIX),'Temporary certificate directory has an unexpected name');
            await rm(certificateDirectory,{recursive:true,force:true});
            certificateDirectory=null;
          }
        })();
        return closePromise;
      },
    };
  }catch(error){
    if(server?.listening)await closeServer(server,sessions,sockets).catch(()=>{});
    if(certificateDirectory){
      const expectedParent=resolve(tmpdir()),actualParent=resolve(dirname(certificateDirectory));
      if(actualParent.toLowerCase()===expectedParent.toLowerCase()&&basename(certificateDirectory).startsWith(TEMP_CERT_PREFIX))
        await rm(certificateDirectory,{recursive:true,force:true}).catch(()=>{});
    }
    throw error;
  }
}
