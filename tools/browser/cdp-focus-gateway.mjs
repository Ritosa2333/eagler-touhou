import {createHash, randomBytes} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {access, readFile, stat} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {basename, dirname, resolve} from 'node:path';
import {setTimeout as delay} from 'node:timers/promises';
import {performance} from 'node:perf_hooks';

const packageRequire = createRequire(new URL('../../launcher/package.json', import.meta.url));
const {WebSocket, WebSocketServer} = packageRequire('ws');
const backgroundFlags = Object.freeze([
  '--disable-backgrounding-occluded-windows',
  '--disable-renderer-backgrounding',
  '--disable-background-timer-throttling',
]);
const timeout = (promise, ms, label) => {
  let timer;
  const elapsed=new Promise((_,reject)=>{timer=setTimeout(()=>reject(new Error(`${label} timed out after ${ms} ms`)),ms);});
  return Promise.race([promise,elapsed]).finally(()=>clearTimeout(timer));
};
const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');
const isLoopbackHost = host => ['127.0.0.1','::1','[::1]','localhost'].includes(host);
const isOwnedLoopbackUrl = (value, protocol) => {
  const url=new URL(value);
  if(url.protocol!==protocol||!isLoopbackHost(url.hostname)||url.username||url.password||url.search||url.hash)
    throw new Error(`Expected a local ${protocol} endpoint without credentials or extra URL components: ${url.href}`);
  return url;
};

export function rewriteFocusEmulationMessage(text, {rewrite=true}={}) {
  let message;
  try {message=JSON.parse(text);}
  catch (error) {return {text, error:`Invalid CDP JSON: ${error.message}`};}
  if (message?.method !== 'Emulation.setFocusEmulationEnabled' || message.params?.enabled !== true)
    return {text, message, rewritten:false};
  if (!rewrite) return {text, message, rewritten:false, focusCommand:true};
  const forwarded={...message,params:{...message.params,enabled:false}};
  return {text:JSON.stringify(forwarded),message,rewritten:true,forwarded,focusCommand:true};
}

const commandKey=message=>`${message?.sessionId??''}\u0000${message?.id??''}`;

export function assessFocusRewriteForTarget(snapshot,targetId) {
  const matching=(snapshot?.rewrites??[]).filter(item=>item?.rewritten===true&&item?.target?.type==='page'&&item.target.targetId===targetId);
  const acknowledged=matching.filter(item=>item.status==='acknowledged');
  const errors=matching.filter(item=>item.status==='error');
  const pending=matching.filter(item=>item.status==='pending');
  const ready=Boolean(targetId)&&matching.length>0&&acknowledged.length>0&&!errors.length&&!pending.length&&matching.length===acknowledged.length;
  return{ready,targetId:targetId??null,matchingRewrites:matching.length,acknowledgedRewrites:acknowledged.length,
    pendingRewrites:pending.length,erroredRewrites:errors.length,
    reason:ready?null:!targetId?'The actual page target ID is unavailable.':matching.length===0?
      'No rewritten focus command was associated with the actual page target.':pending.length?
        'A rewritten focus command for the actual page target has no successful Chrome acknowledgement.':
        errors.length?'Chrome rejected a rewritten focus command for the actual page target.':
          'The actual page target has no fully acknowledged focus rewrite.'};
}

export async function identifyPlaywrightSources({workspaceRoot}) {
  const require = createRequire(resolve(workspaceRoot,'launcher/package.json'));
  let entry;
  try {entry=require.resolve('playwright');}
  catch (error) {
    if (!['ERR_MODULE_NOT_FOUND','MODULE_NOT_FOUND'].includes(error.code)) throw error;
    const python=process.env.PYTHON|| (process.platform==='win32'?'python':'python3');
    entry=execFileSync(python,['-c',
      "import pathlib,playwright; print(pathlib.Path(playwright.__file__).parent/'driver'/'package'/'index.mjs')"],
    {encoding:'utf8',windowsHide:true}).trim();
  }
  const packageRequire=createRequire(entry);
  let packageRoot,coreEntry;
  const entryBytes=await readFile(entry);
  const entryPackageRoot=dirname(entry);
  const entryPackage=JSON.parse(await readFile(resolve(entryPackageRoot,'package.json'),'utf8'));
  if(entryPackage.name==='playwright-core'){
    packageRoot=entryPackageRoot;coreEntry=entry;
  }else{
    try {coreEntry=packageRequire.resolve('playwright-core');packageRoot=dirname(coreEntry);}
    catch(error){
      if(!['ERR_MODULE_NOT_FOUND','MODULE_NOT_FOUND'].includes(error.code))throw error;
      throw new Error(`Could not resolve the Playwright core package loaded by ${entry}.`);
    }
  }
  const packageMetadataBytes=await readFile(resolve(packageRoot,'package.json'));
  const packageMetadata=JSON.parse(packageMetadataBytes.toString('utf8'));
  if(packageMetadata.name!=='playwright-core')throw new Error(`Resolved Playwright source is not playwright-core: ${packageRoot}`);
  const relativeFiles=[
    'lib/browserServerImpl.js',
    'lib/server/browserType.js',
    'lib/server/chromium/chromium.js',
    'lib/server/chromium/crBrowser.js',
    'lib/server/chromium/crPage.js',
    'lib/client/browserType.js',
  ];
  const files={};
  for (const relative of relativeFiles) {
    const path=resolve(packageRoot,relative),bytes=await readFile(path);
    files[relative]={path,bytes:bytes.length,sha256:sha256(bytes)};
  }
  const coreEntryBytes=await readFile(coreEntry);
  return {entry,coreEntry,packageRoot,packageName:packageMetadata.name,version:packageMetadata.version??null,
    entryIdentity:{path:entry,bytes:entryBytes.length,sha256:sha256(entryBytes)},
    coreEntryIdentity:{path:coreEntry,bytes:coreEntryBytes.length,sha256:sha256(coreEntryBytes)},
    packageIdentity:{path:resolve(packageRoot,'package.json'),bytes:packageMetadataBytes.length,sha256:sha256(packageMetadataBytes)},files};
}

export async function resolveFullChromeExecutable(chromium, {env=process.env}={}) {
  const explicit=env.PLAYWRIGHT_CHROMIUM_EXECUTABLE;
  const candidate=explicit||chromium.executablePath();
  if (!candidate) throw new Error('Playwright did not resolve a full Chromium executable. Headed lifecycle runs do not fall back to chrome-headless-shell.');
  if (/headless[-_]?shell/i.test(basename(candidate)))
    throw new Error(`Headed lifecycle runs require full Chrome; refusing ${candidate}.`);
  try {
    await access(candidate);
    if(!(await stat(candidate)).isFile())throw new Error('not a regular file');
  }
  catch {throw new Error(`Full Chrome executable is unavailable at ${candidate}; headed lifecycle runs do not fall back to chrome-headless-shell.`);}
  return resolve(candidate);
}

function waitForEmitter(emitter,event,ms,label) {
  return new Promise((accept,reject)=>{
    const timer=setTimeout(()=>{cleanup();reject(new Error(`${label} timed out after ${ms} ms`));},ms);
    const failed=error=>{cleanup();reject(error);};
    const completed=(...args)=>{cleanup();accept(args);};
    const cleanup=()=>{clearTimeout(timer);emitter.off('error',failed);emitter.off(event,completed);};
    emitter.once('error',failed);emitter.once(event,completed);
  });
}

export function validateDevToolsEndpoint({port,wsPath,webSocketDebuggerUrl}) {
  if(!Number.isSafeInteger(port)||port<1||!wsPath?.startsWith('/devtools/browser/'))
    throw new Error('Chrome wrote malformed DevToolsActivePort data.');
  const endpoint=new URL(webSocketDebuggerUrl);
  if(endpoint.protocol!=='ws:'||!isLoopbackHost(endpoint.hostname))
    throw new Error(`Chrome DevTools endpoint must use loopback ws: ${endpoint.href}`);
  if(endpoint.port!==String(port)||endpoint.pathname!==wsPath||endpoint.username||endpoint.password||endpoint.search||endpoint.hash)
    throw new Error(`Chrome DevTools endpoint does not match its owned DevToolsActivePort (${port}${wsPath}): ${endpoint.href}`);
  return endpoint.href;
}

async function readDevToolsEndpoint(profileDir,child,timeoutMs) {
  const activePortPath=resolve(profileDir,'DevToolsActivePort'),deadline=Date.now()+timeoutMs;
  let lastError;
  while(Date.now()<deadline) {
    if(child.exitCode!==null||child.signalCode!==null)
      throw new Error(`Owned Chrome exited before DevTools was ready (exit=${child.exitCode}, signal=${child.signalCode}).`);
    try {
      const [portText,wsPath]=(await readFile(activePortPath,'utf8')).trim().split(/\r?\n/);
      const port=Number(portText);
      if(!Number.isSafeInteger(port)||port<1||!wsPath?.startsWith('/devtools/browser/'))
        throw new Error('Chrome wrote malformed DevToolsActivePort data.');
      const response=await fetch(`http://127.0.0.1:${port}/json/version`,{signal:AbortSignal.timeout(1200)});
      if(!response.ok)throw new Error(`Chrome /json/version returned HTTP ${response.status}.`);
      const version=await response.json();
      validateDevToolsEndpoint({port,wsPath,webSocketDebuggerUrl:version.webSocketDebuggerUrl});
      return {port,wsPath,endpoint:version.webSocketDebuggerUrl,version};
    } catch(error) {
      lastError=error;
      if(error.message?.startsWith('Chrome wrote malformed')||error.message?.startsWith('Chrome DevTools endpoint'))throw error;
      if(error.message?.startsWith('Chrome DevTools endpoint does not match'))throw error;
    }
    await delay(100);
  }
  throw new Error(`Timed out waiting for Chrome DevToolsActivePort under ${profileDir}${lastError?`: ${lastError.message}`:''}.`);
}

export async function createCdpFocusGateway(upstreamUrl,{closeTimeoutMs=2500,handshakeTimeoutMs=5000,maxQueueBytes=8*1024*1024,maxPayloadBytes=384*1024*1024,maxBufferedBytes=512*1024*1024,maxCommandRecords=4096}={}) {
  const upstreamEndpoint=isOwnedLoopbackUrl(upstreamUrl,'ws:');
  for(const [name,value] of Object.entries({closeTimeoutMs,handshakeTimeoutMs,maxQueueBytes,maxPayloadBytes,maxBufferedBytes,maxCommandRecords}))
    if(!Number.isSafeInteger(value)||value<1)throw new TypeError(`${name} must be a positive safe integer.`);
  // Reserve the largest WebSocket frame header, including a client masking key.
  const frameOverheadBytes=14;
  if(maxPayloadBytes>maxBufferedBytes-frameOverheadBytes)
    throw new TypeError('maxBufferedBytes must cover maxPayloadBytes and its 14-byte WebSocket frame header.');
  const route=`/cdp-${randomBytes(24).toString('hex')}`;
  const server=new WebSocketServer({host:'127.0.0.1',port:0,path:route,maxPayload:maxPayloadBytes});
  try {await waitForEmitter(server,'listening',5000,'CDP gateway listener');}
  catch(error){try{server.close();}catch{}throw error;}
  const address=server.address();
  if(!address||typeof address==='string'){
    await timeout(new Promise(accept=>server.close(()=>accept())),closeTimeoutMs,'CDP gateway listener cleanup').catch(()=>{});
    throw new Error('CDP gateway did not bind a TCP loopback port.');
  }
  const audit={endpoint:`ws://127.0.0.1:${address.port}${route}`,upstreamEndpoint:upstreamEndpoint.href,
    policy:{handshakeTimeoutMs,maxQueueBytes,maxPayloadBytes,maxBufferedBytes,maxCommandRecords},rewrites:[],targets:[],errors:[],clientConnections:0,
    commandClock:{source:'node:perf_hooks.performance.now',timeOriginMs:performance.timeOrigin},commands:[],commandRecordsDropped:0};
  const sessions=new Map(),connections=new Set();
  let rewriteEnabled=true,closed=false;
  const recordError=(where,error)=>{audit.errors.push({at:new Date().toISOString(),where,error:String(error?.message||error)});};
  const forward=(socket,data,binary)=>{
    if(socket.readyState!==WebSocket.OPEN)throw new Error('CDP gateway attempted to write to a closed socket.');
    const messageBytes=Buffer.byteLength(data);
    if(messageBytes>maxPayloadBytes)throw new Error(`CDP gateway message exceeded the ${maxPayloadBytes} byte payload limit.`);
    if(messageBytes>maxBufferedBytes-socket.bufferedAmount-frameOverheadBytes)
      throw new Error(`CDP gateway send would exceed the ${maxBufferedBytes} byte buffered-data limit (queued ${socket.bufferedAmount}, message ${messageBytes}, frame allowance ${frameOverheadBytes}).`);
    socket.send(data,{binary});
  };
  const closeSocket=(socket,code=1011,reason='CDP gateway protocol failure.')=>{
    if(socket.readyState===WebSocket.OPEN)socket.close(code,reason);
    else if(socket.readyState===WebSocket.CONNECTING)socket.terminate();
  };
  server.on('connection',(client,request)=>{
    audit.clientConnections++;
    if(audit.clientConnections!==1||!['127.0.0.1','::ffff:127.0.0.1'].includes(request.socket.remoteAddress)) {
      recordError('client-policy',new Error('Expected one IPv4 loopback controller connection.'));
      client.terminate();
      return;
    }
    const upstream=new WebSocket(upstreamUrl,{maxPayload:maxPayloadBytes,handshakeTimeout:handshakeTimeoutMs});
    const connection={client,upstream,queued:[],queuedBytes:0,pendingFocus:new Map(),pendingCommands:new Map()};connections.add(connection);
    const forwardCommand=bytes=>{
      forward(upstream,bytes,false);
      const message=JSON.parse(bytes.toString('utf8')),method=message.method;
      if(!['Page.bringToFront','Target.activateTarget','Browser.setWindowBounds','Runtime.evaluate','Runtime.callFunctionOn'].includes(method)&&!method?.startsWith('Input.'))return;
      if(audit.commands.length>=maxCommandRecords){audit.commandRecordsDropped++;return;}
      const record={id:message.id,sessionId:message.sessionId??null,method,
        targetId:message.params?.targetId??sessions.get(message.sessionId)?.targetId??null,
        sentAtMonotonicMs:performance.now(),status:'pending'};
      if(method==='Runtime.evaluate'||method==='Runtime.callFunctionOn')record.userGesture=typeof message.params?.userGesture==='boolean'?message.params.userGesture:null;
      if(method==='Browser.setWindowBounds'){
        record.windowId=message.params?.windowId??null;record.windowState=message.params?.bounds?.windowState??null;
      }
      audit.commands.push(record);connection.pendingCommands.set(commandKey(message),record);
    };
    let upstreamReady=false,fatalSeen=false;
    const maybeForget=()=>{if(client.readyState===WebSocket.CLOSED&&upstream.readyState===WebSocket.CLOSED)connections.delete(connection);};
    const fatal=(where,error)=>{
      if(!fatalSeen){fatalSeen=true;recordError(where,error);}
      closeSocket(client);closeSocket(upstream);
    };
    client.on('message',(data,binary)=>{
      if(binary){fatal('client-binary-message',new Error('Binary CDP messages are unsupported.'));return;}
      const decoded=rewriteFocusEmulationMessage(data.toString(),{rewrite:rewriteEnabled});
      if(decoded.error){fatal('client-invalid-json',new Error(decoded.error));return;}
      if(decoded.focusCommand){
        const record={at:new Date().toISOString(),id:decoded.message.id,sessionId:decoded.message.sessionId??null,
          target:sessions.get(decoded.message.sessionId)??null,requestedEnabled:true,
          forwardedEnabled:decoded.rewritten?false:true,rewritten:decoded.rewritten,
          status:decoded.rewritten?'pending':'forwarded-without-rewrite'};
        audit.rewrites.push(record);
        if(decoded.rewritten){
          const key=commandKey(decoded.message);
          if(connection.pendingFocus.has(key)){fatal('focus-command-id-collision',new Error(`A focus command is already pending for CDP id ${decoded.message.id} and session ${decoded.message.sessionId??'<root>'}.`));return;}
          connection.pendingFocus.set(key,record);
        }
      }
      const bytes=Buffer.from(decoded.text,'utf8');
      if(!upstreamReady){
        connection.queuedBytes+=bytes.length;
        if(connection.queuedBytes>maxQueueBytes){fatal('upstream-queue-limit',new Error(`Controller queued more than ${maxQueueBytes} bytes before Chrome CDP connected.`));return;}
        connection.queued.push(bytes);
      }else try{forwardCommand(bytes);}catch(error){fatal('client-forward',error);}
    });
    upstream.on('unexpected-response',(_request,response)=>fatal('upstream-handshake',new Error(`Chrome CDP returned HTTP ${response.statusCode} during WebSocket handshake.`)));
    upstream.on('open',()=>{
      upstreamReady=true;
      const queued=connection.queued.splice(0);connection.queuedBytes=0;
      for(const bytes of queued)try{forwardCommand(bytes);}catch(error){fatal('queued-forward',error);break;}
    });
    upstream.on('message',(data,binary)=>{
      if(binary){fatal('browser-binary-message',new Error('Binary CDP messages are unsupported.'));return;}
      let message;
      try{message=JSON.parse(data.toString());}
      catch(error){fatal('browser-invalid-json',error);return;}
      if(message.method==='Target.attachedToTarget'){
        const sessionId=message.params?.sessionId,info=message.params?.targetInfo;
        if(sessionId&&info){const target={targetId:info.targetId,type:info.type,url:info.url};sessions.set(sessionId,target);audit.targets.push({at:new Date().toISOString(),sessionId,...target});}
      }else if(message.method==='Target.detachedFromTarget')sessions.delete(message.params?.sessionId);
      if(Number.isSafeInteger(message.id)){
        const commandRecord=connection.pendingCommands.get(commandKey(message));
        if(commandRecord){
          connection.pendingCommands.delete(commandKey(message));
          commandRecord.responseAtMonotonicMs=performance.now();commandRecord.status=message.error?'error':'acknowledged';
          if(message.error)commandRecord.error={code:message.error.code??null,message:message.error.message??'Unknown CDP error'};
        }
        const key=commandKey(message),record=connection.pendingFocus.get(key);
        if(record){
          connection.pendingFocus.delete(key);
          record.responseAt=new Date().toISOString();
          if(message.error){
            record.status='error';record.responseError={code:message.error.code??null,message:message.error.message??'Unknown CDP error',data:message.error.data??null};
            const error=new Error(`Chrome rejected Emulation.setFocusEmulationEnabled for ${record.target?.targetId??'unresolved target'}: ${record.responseError.message}`);
            error.cdpError=record.responseError;fatal('focus-command-response',error);return;
          }
          if(!Object.hasOwn(message,'result')){
            record.status='error';record.responseError={code:null,message:'Matching Chrome response had neither result nor error.'};
            fatal('focus-command-response',new Error(record.responseError.message));return;
          }
          record.status='acknowledged';record.response={result:message.result??null};
        }
      }
      try{forward(client,data,false);}catch(error){fatal('browser-forward',error);}
    });
    for(const [socket,peer,name] of [[client,upstream,'client'],[upstream,client,'browser']]){
      socket.on('error',error=>fatal(`${name}-socket`,error.code==='WS_ERR_UNSUPPORTED_MESSAGE_LENGTH'?
        new Error(`CDP gateway ${name} message exceeded the ${maxPayloadBytes} byte payload limit.`):error));
      socket.on('close',()=>{connection.queued.length=0;connection.queuedBytes=0;closeSocket(peer,1000,'Peer connection closed.');maybeForget();});
    }
  });
  server.on('wsClientError',error=>recordError('gateway-handshake',error));
  server.on('error',error=>recordError('gateway-server',error));
  return {
    endpoint:audit.endpoint,
    snapshot:()=>structuredClone(audit),
    setRewriteEnabled(value){if(typeof value!=='boolean')throw new TypeError('rewriteEnabled must be boolean');rewriteEnabled=value;},
    async close(){
      if(closed){
        if(audit.errors.length)throw Object.assign(new Error(`CDP gateway closed with ${audit.errors.length} protocol or cleanup error(s).`),{gatewaySnapshot:structuredClone(audit)});
        return;
      }
      closed=true;
      const current=[...connections];
      const waitForClosed=(socket,label)=>socket.readyState===WebSocket.CLOSED?Promise.resolve():waitForEmitter(socket,'close',closeTimeoutMs,label);
      for(const {client,upstream} of current){closeSocket(client,1000,'Gateway closed.');closeSocket(upstream,1000,'Gateway closed.');}
      const closeWaits=current.flatMap(({client,upstream})=>[
        waitForClosed(client,'CDP controller close'),waitForClosed(upstream,'Chrome CDP close'),
      ]);
      try {await timeout(Promise.all(closeWaits),closeTimeoutMs,'CDP gateway socket cleanup');}
      catch(error){
        recordError('gateway-close',error);
        for(const {client,upstream} of current)for(const socket of [client,upstream])if(socket.readyState!==WebSocket.CLOSED)socket.terminate();
        const forcedWaits=current.flatMap(({client,upstream})=>[waitForClosed(client,'forced controller close'),waitForClosed(upstream,'forced Chrome CDP close')]);
        try{await timeout(Promise.all(forcedWaits),Math.min(closeTimeoutMs,1000),'CDP gateway forced socket cleanup');}
        catch(forcedError){recordError('gateway-forced-close',forcedError);}
      }
      await timeout(new Promise((accept,reject)=>server.close(error=>error?reject(error):accept())),closeTimeoutMs,'CDP gateway server close')
        .catch(error=>recordError('gateway-server-close',error));
      if(connections.size)recordError('gateway-connections-remain',new Error(`${connections.size} WebSocket connection(s) remained after gateway shutdown.`));
      if(audit.errors.length)throw Object.assign(new Error(`CDP gateway closed with ${audit.errors.length} protocol or cleanup error(s).`),{gatewaySnapshot:structuredClone(audit)});
    },
  };
}

export async function launchFocusControlledChromium({chromium,args=[],timeoutMs=15_000}) {
  if(!chromium||typeof chromium.launchServer!=='function'||typeof chromium.connectOverCDP!=='function')
    throw new TypeError('A Chromium BrowserType with public launchServer/connectOverCDP APIs is required.');
  const executablePath=await resolveFullChromeExecutable(chromium);
  let server,gateway,browser,browserCdp,child;
  try {
    server=await chromium.launchServer({headless:false,executablePath,args:[...args,'--remote-debugging-port=0'],
      ignoreDefaultArgs:[...backgroundFlags],timeout:timeoutMs});
    child=server.process();const spawnargs=[...(child.spawnargs??[])];
    const actualExecutable=spawnargs[0]?resolve(spawnargs[0]):'';
    const executableMatches=process.platform==='win32'?actualExecutable.toLowerCase()===executablePath.toLowerCase():actualExecutable===executablePath;
    if(!child.pid||!spawnargs.length||!executableMatches)
      throw new Error('The owned Playwright BrowserServer process does not match the selected full Chrome executable.');
    const profileArg=spawnargs.find(value=>value.startsWith('--user-data-dir='));
    if(!profileArg)throw new Error('BrowserServer.process().spawnargs does not expose the owned user-data-dir.');
    const profileDir=profileArg.slice('--user-data-dir='.length).replace(/^"|"$/g,'');
    const devtools=await readDevToolsEndpoint(profileDir,child,timeoutMs);
    gateway=await createCdpFocusGateway(devtools.endpoint);
    browser=await chromium.connectOverCDP(gateway.endpoint,{timeout:timeoutMs,isLocal:true});
    browserCdp=await browser.newBrowserCDPSession();
    const version=await timeout(browserCdp.send('Browser.getVersion'),timeoutMs,'Chrome version request');
    if(/headless/i.test(version.userAgent)||/headless[-_]?shell/i.test(spawnargs[0]))
      throw new Error(`Headed desktop lifecycle run resolved a headless browser (${spawnargs[0]}, ${version.userAgent}).`);
    const metadata={executablePath,processId:child.pid,processArguments:spawnargs,profileDir,
      devtools:{port:devtools.port,wsPath:devtools.wsPath},browserVersion:version,
      removedBackgroundFlags:[...backgroundFlags],gatewayEndpoint:gateway.endpoint};
    return {browser,browserCdp,server,gateway,metadata,
      gatewaySnapshot:()=>gateway.snapshot(),
      async close(){
        const errors=[];
        try{await browserCdp.detach();}catch(error){errors.push(`CDP session detach: ${error.message}`);}
        try{await timeout(browser.close(),timeoutMs,'Playwright CDP disconnect');}catch(error){errors.push(`Playwright CDP disconnect: ${error.message}`);}
        try{await gateway.close();}catch(error){errors.push(`CDP gateway close: ${error.message}`);}
        try{await timeout(server.close(),timeoutMs,'Owned Chrome shutdown');}
        catch(error){errors.push(`Owned Chrome shutdown: ${error.message}`);try{await timeout(server.kill(),timeoutMs,'Owned Chrome forced shutdown');}
          catch(killError){errors.push(`Owned Chrome forced shutdown: ${killError.message}`);}}
        try{if(child.exitCode===null&&child.signalCode===null)await waitForEmitter(child,'close',timeoutMs,'Owned Chrome process exit');}
        catch(error){errors.push(`Owned Chrome process exit: ${error.message}`);}
        const result={processId:child.pid,exitCode:child.exitCode,signalCode:child.signalCode,
          gateway:gateway.snapshot(),errors};
        if(errors.length)throw Object.assign(new Error(errors.join('\n')),{cleanupResult:result});
        return result;
      }};
  } catch(error) {
    const cleanupErrors=[];
    try{if(browserCdp)await browserCdp.detach();}catch(failure){cleanupErrors.push(`CDP session detach: ${failure.message}`);}
    try{if(browser)await timeout(browser.close(),timeoutMs,'Playwright CDP failure cleanup');}catch(failure){cleanupErrors.push(`Playwright CDP failure cleanup: ${failure.message}`);}
    try{if(gateway)await gateway.close();}catch(failure){cleanupErrors.push(`CDP gateway failure cleanup: ${failure.message}`);}
    try{if(server)await timeout(server.close(),timeoutMs,'Owned Chrome failure cleanup');}
    catch(failure){cleanupErrors.push(`Owned Chrome failure cleanup: ${failure.message}`);try{await timeout(server.kill(),timeoutMs,'Owned Chrome forced failure cleanup');}
      catch(killError){cleanupErrors.push(`Owned Chrome forced failure cleanup: ${killError.message}`);}}
    try{if(child&&child.exitCode===null&&child.signalCode===null)await waitForEmitter(child,'close',timeoutMs,'Owned Chrome failure process exit');}
    catch(failure){cleanupErrors.push(`Owned Chrome failure process exit: ${failure.message}`);}
    if(cleanupErrors.length)error.cleanupErrors=cleanupErrors;
    throw error;
  }
}

export {backgroundFlags};
