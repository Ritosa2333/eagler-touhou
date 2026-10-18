// Connect-only Android diagnosis. No explicit page creation/navigation/evaluate,
// input, reverse mapping, upload, download, or browser/device file API.
// Playwright initialization still attaches existing targets and enables domains.
// Local Node writes use this run's exclusive output; no debug protocol payload log.
// This version uses Playwright 1.63.0's public ConnectOverCDPTransport. That
// server branch forces isLocal:true; results diagnose initialization only.
import {createHash} from 'node:crypto';
import {existsSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {createRequire} from 'node:module';
import {dirname, isAbsolute, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {performance} from 'node:perf_hooks';
import {setImmediate as nextTurn} from 'node:timers/promises';

const LIMITS = Object.freeze({httpMs:10000, wsOpenMs:10000, connectMs:30000,
  closeMs:5000, terminateMs:1000, adbMs:20000, records:4096, pending:256,
  dispatchMessages:256, faults:16});
const label = process.env.RUN_LABEL;
if (!/^[a-z0-9_-]{1,100}$/.test(label || '')) throw Error('A safe exclusive RUN_LABEL is required');
const root = process.cwd(), self = fileURLToPath(import.meta.url);
const base = resolve(root, 'th09_web/artifacts/multiplayer-tests', label);
const reportPath = base + '-report.json';
if (existsSync(base) || existsSync(reportPath)) throw Error('Run output already exists');
mkdirSync(base);
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const identity = path => {const bytes=readFileSync(path);return {path,bytes:bytes.length,sha256:sha(bytes)};};
const clock = () => performance.now();
const safeAtom = value => typeof value==='string' && /^[A-Za-z0-9_.-]{1,80}$/.test(value) ? value : null;
const safeError = error => ({name:safeAtom(error?.name), code:Number.isSafeInteger(error?.code)
  ? error.code : safeAtom(error?.code)});
const fail = code => {const error=new Error();error.code=code;throw error;};
const report = {schema:'th09-managed-touch-android-connect/v1',runLabel:label,
  startedAt:new Date().toISOString(),clock:{name:'node:perf_hooks.performance',timeOrigin:performance.timeOrigin},
  scope:'Single owned CDP connection initialization only; no new page or game. Not Android gameplay, physical input, or FPS acceptance.',
  transport:{api:'public ConnectOverCDPTransport',serverForcesIsLocal:true,noDefaults:true,
    difference:'Transport entry forces local-filesystem optimization metadata; no filesystem-dependent browser API is used.',
    initializationBoundary:'Playwright still attaches existing targets and enables protocol domains. This is not a passive device observation.'},
  limits:LIMITS,stages:[],adbOperations:[],failures:[],cleanupErrors:[],
  sourceBefore:[],sourceAfter:[],connection:{attempted:false,connected:false},
  mapping:{attempted:false,acquired:false,port:null,outcome:'not-attempted'},
  metadataPolicy:'Only CDP id/method, Node clocks, response presence and numeric error code. No URL, params, result, error.message, raw frame, or stack is serialized.'};
const stage = (name, fields={}) => {const row={name,atMs:clock(),...fields};report.stages.push(row);console.log(JSON.stringify({stage:name,...fields}));};
let browser=null, transportOwner=null, closing=false, adbPath, deviceId, sourcePaths=[self];
let connectionPromise=null, connectionSettled=false, browserCloseAttempted=false;
let phase='initializing', sequence=0, overwrite=0, ringAt=0;
const records=[], pending=new Map();
const counters={sent:0,responses:0,events:0,unknownResponses:0,pendingOverflow:0,
  invalidMetadata:0,transportFaults:0,earlyQueued:0,dispatchQueuePeak:0,
  dispatchOverflow:0,notDeliveredAfterClose:0};
function append(row) {
  const value={seq:++sequence,phase,...row};
  if(records.length<LIMITS.records) records.push(value);
  else {records[ringAt]=value;ringAt=(ringAt+1)%LIMITS.records;overwrite++;}
}
function methodName(value) {
  return typeof value==='string' && value.length<=120 && /^[A-Za-z][A-Za-z0-9_]*(\.[A-Za-z][A-Za-z0-9_]*)+$/.test(value)
    ? value : null;
}
function observeSend(message) {
  const atMs=clock(), id=Number.isSafeInteger(message?.id)?message.id:null, method=methodName(message?.method);
  counters.sent++;if(id===null || !method)counters.invalidMetadata++;
  if(id!==null && method) {
    if(pending.has(id)) counters.invalidMetadata++;
    else if(pending.size<LIMITS.pending) pending.set(id,{id,method,sentAtMs:atMs});
    else counters.pendingOverflow++;
  }
  append({kind:'command',id,method,atMs});
}
function observeReceive(message, receivedAtMs) {
  const atMs=clock(), id=Number.isSafeInteger(message?.id)?message.id:null;
  if(id!==null) {
    counters.responses++;
    const request=pending.get(id);if(request)pending.delete(id);else counters.unknownResponses++;
    append({kind:'response',id,method:request?.method??null,atMs,receivedAtMs,
      responsePresent:true,errorPresent:!!message.error,
      errorCode:Number.isSafeInteger(message.error?.code)?message.error.code:null,
      observedResponseElapsedMs:request?atMs-request.sentAtMs:null,
      localDispatchAfterSocketCallbackMs:atMs-receivedAtMs});
  } else {
    counters.events++;const method=methodName(message?.method);if(!method)counters.invalidMetadata++;
    append({kind:'event',id:null,method,atMs,receivedAtMs});
  }
}
function capture() {
  const atMs=clock(), ordered=overwrite?[...records.slice(ringAt),...records.slice(0,ringAt)]:records;
  return {atMs,phase,counters:{...counters},totalRecords:sequence,retained:records.length,overwritten:overwrite,
    complete:overwrite===0&&counters.pendingOverflow===0&&counters.invalidMetadata===0&&
      counters.transportFaults===0&&counters.unknownResponses===0&&counters.dispatchOverflow===0&&counters.notDeliveredAfterClose===0,
    pendingComplete:counters.pendingOverflow===0&&counters.invalidMetadata===0,
    dispatchQueue:transportOwner?.queueSnapshot()??{queued:0,scheduled:0,oldestSocketCallbackAtMs:null},
    pending:[...pending.values()].map(row=>({...row,ageAtSnapshotMs:atMs-row.sentAtMs})),
    records:ordered.map(row=>({...row})),
    scope:'Elapsed time starts at Node send observation and ends at Node dispatch observation; it is not device CPU or wire RTT. Zero pending does not rule out Playwright internal navigation/context promises.'};
}
function fault(boundary,error) {
  counters.transportFaults++;
  if(report.failures.length<LIMITS.faults)report.failures.push({boundary,atMs:clock(),...safeError(error)});
  try{transportOwner?.requestClose();}catch(closeError){
    if(report.failures.length<LIMITS.faults)report.failures.push({boundary:'fault-close',atMs:clock(),...safeError(closeError)});
  }
}
async function bounded(promise, ms, code) {
  let timer;
  try{return await Promise.race([promise,new Promise((_,reject)=>{timer=setTimeout(()=>{
    const error=new Error();error.name='TimeoutError';error.code=code;reject(error);
  },ms);})]);}finally{clearTimeout(timer);}
}
function adb(operation,args) {
  const start=clock();
  const result=spawnSync(adbPath,['-s',deviceId,...args],{encoding:'utf8',windowsHide:true,
    timeout:LIMITS.adbMs,maxBuffer:1024*1024});
  report.adbOperations.push({operation,startedAtMs:start,returnedAtMs:clock(),status:result.status,
    signal:safeAtom(result.signal),error:safeError(result.error)});
  // stdout is consumed only for ownership checks; stdout/stderr are never copied to the report.
  if(result.error || result.status!==0) {
    const error=new Error();error.code='ADB_OPERATION_FAILED';throw error;
  }
  return (result.stdout||'').trim();
}
function forwardRows(value) {return value.split(/\r?\n/).map(line=>line.trim().split(/\s+/));}
function ownedTuple(rows) {return rows.some(row=>row[0]===deviceId&&
  row[1]==='tcp:'+report.mapping.port&&row[2]==='localabstract:chrome_devtools_remote');}
async function versionAt(endpoint) {
  const lifetime=new AbortController(), timer=setTimeout(()=>lifetime.abort(),LIMITS.httpMs);
  try {
    const response=await fetch(endpoint+'/json/version',{signal:lifetime.signal,redirect:'error'});
    report.devtoolsDiscovery={status:response.status};
    if(response.status!==200)fail('DEVTOOLS_HTTP_STATUS');
    const body=await response.text(), value=JSON.parse(body);
    const endpointUrl=new URL(endpoint), debuggerUrl=new URL(value.webSocketDebuggerUrl);
    if(debuggerUrl.protocol!=='ws:' ||
      !(debuggerUrl.pathname==='/devtools/browser' || debuggerUrl.pathname.startsWith('/devtools/browser/')))
      fail('DEVTOOLS_BROWSER_WEBSOCKET_REQUIRED');
    // Only the explicit forward can be reached, regardless of the advertised port.
    debuggerUrl.host=endpointUrl.host;debuggerUrl.username='';debuggerUrl.password='';
    report.devtoolsDiscovery.bodyParsed=true;
    report.devtoolsDiscovery.browserVersion=typeof value.Browser==='string'&&
      /^[A-Za-z][A-Za-z0-9_.-]*\/[0-9.]+$/.test(value.Browser)?value.Browser:null;
    report.devtoolsDiscovery.protocolVersion=typeof value['Protocol-Version']==='string'&&
      /^[0-9.]+$/.test(value['Protocol-Version'])?value['Protocol-Version']:null;
    return debuggerUrl.href;
  } finally {clearTimeout(timer);}
}
function makeTransport(WebSocket,url) {
  // Match the installed Playwright transport's message scheduling and compression.
  const socket=new WebSocket(url,[],{maxPayload:256*1024*1024,followRedirects:false,
    perMessageDeflate:{clientNoContextTakeover:true,zlibDeflateOptions:{level:3},
      zlibInflateOptions:{chunkSize:10*1024},threshold:10*1024}});
  let receiver, closed=false, opened=false, closeRequested=false;
  const queue=new Array(LIMITS.dispatchMessages);let head=0,size=0,scheduled=0;
  let resolveClosed, resolveOpen, rejectOpen;
  const closedPromise=new Promise(resolve=>{resolveClosed=resolve;});
  const openedPromise=new Promise((resolve,reject)=>{resolveOpen=resolve;rejectOpen=reject;});
  const transport={onclose:undefined,
    send(message){observeSend(message);try{socket.send(JSON.stringify(message));}
      catch(error){fault('websocket-send',error);throw error;}},
    close(){requestClose();return closedPromise;}};
  function requestClose(){closeRequested=true;if(socket.readyState!==WebSocket.CLOSED)socket.close();}
  function invoke(message,receivedAtMs) {
    observeReceive(message,receivedAtMs);
    try {
      const result=receiver.call(null,message);
      // Preserve every original response/event. A callback failure is recorded,
      // terminates this diagnostic connection, and is never serialized as text.
      if(result&&typeof result.then==='function')void result.catch(error=>fault('playwright-message-callback',error));
    } catch(error){fault('playwright-message-callback',error);}
  }
  function clearUndelivered() {
    counters.notDeliveredAfterClose+=size;
    while(size){queue[head]=undefined;head=(head+1)%queue.length;size--;}
  }
  function scheduleDelivery() {
    if(typeof receiver!=='function')return;
    // One FIFO for early and live messages prevents binding-time overtaking.
    // Each message gets its own setImmediate, as in the installed transport.
    while(scheduled<size) {
      scheduled++;
      setImmediate(()=>{
        scheduled--;
        if(typeof receiver!=='function'){if(closed||closing)clearUndelivered();return;}
        if(!size)return;
        const item=queue[head];queue[head]=undefined;head=(head+1)%queue.length;size--;
        let message;
        try{message=JSON.parse(item.data.toString());}
        catch(error){fault('websocket-json-decode',error);return;}
        invoke(message,item.receivedAtMs);
      });
    }
  }
  Object.defineProperty(transport,'onmessage',{enumerable:true,configurable:true,
    get:()=>receiver,set:next=>{receiver=next;scheduleDelivery();}});
  socket.on('open',()=>{opened=true;resolveOpen();});
  socket.on('message',data=>{
    const receivedAtMs=clock();
    if(typeof receiver!=='function'&&(closed||closing)){counters.notDeliveredAfterClose++;return;}
    if(size>=queue.length){counters.dispatchOverflow++;fault('protocol-dispatch-queue-overflow',{code:'DISPATCH_QUEUE_LIMIT'});return;}
    queue[(head+size)%queue.length]={data,receivedAtMs};size++;
    if(typeof receiver!=='function')counters.earlyQueued++;
    counters.dispatchQueuePeak=Math.max(counters.dispatchQueuePeak,size);scheduleDelivery();
  });
  socket.on('error',error=>{
    rejectOpen(error);fault('websocket-error',error);
  });
  socket.on('unexpected-response',(_request,response)=>{
    report.webSocketHandshakeStatus=response.statusCode??null;
    response.resume();const error=new Error();error.code='WEBSOCKET_HANDSHAKE_STATUS';
    rejectOpen(error);fault('websocket-handshake',error);
  });
  socket.on('close',(code,reason)=>{
    closed=true;resolveClosed();report.webSocketClose={atMs:clock(),code,requested:closeRequested};
    if(!opened){const error=new Error();error.code='WEBSOCKET_CLOSED_BEFORE_OPEN';rejectOpen(error);}
    try{transport.onclose?.call(null,reason.toString());}
    catch(error){fault('playwright-close-callback',error);}
    if(typeof receiver!=='function')clearUndelivered();
  });
  return {transport,openedPromise,closedPromise,requestClose,
    queueSnapshot:()=>({queued:size,scheduled,oldestSocketCallbackAtMs:size?queue[head].receivedAtMs:null}),
    isClosed:()=>closed,terminate:()=>socket.terminate()};
}
const onUnhandled=error=>fault('unhandled-rejection',error);
process.on('unhandledRejection',onUnhandled);
try {
  if(self!==resolve(root,'th09_web/tests/multiplayer/managed-touch-android-connect-probe.mjs'))fail('WRONG_WORKTREE_CWD');
  adbPath=process.env.TH09_ADB_PATH;deviceId=process.env.TH09_ANDROID_SERIAL;
  const dependenciesRoot=process.env.TH09_TEST_DEPENDENCIES_ROOT;
  if(!adbPath||!isAbsolute(adbPath)||!existsSync(adbPath)||!deviceId||
    !dependenciesRoot||!isAbsolute(dependenciesRoot))fail('EXPLICIT_EXISTING_DEPENDENCIES_REQUIRED');
  report.owner={adbPath,deviceId,package:'org.chromium.chrome'};
  report.environment={pwDebugPresent:!!process.env.PWDEBUG,
    attachToOtherPresent:!!process.env.PW_CHROMIUM_ATTACH_TO_OTHER,debugWasNonempty:!!process.env.DEBUG};
  // Full pw:protocol logging includes unrelated page payloads. Keep this process
  // metadata-only; do not enable Inspector/console helpers or other target types.
  if(report.environment.pwDebugPresent||report.environment.attachToOtherPresent)fail('UNSUPPORTED_PLAYWRIGHT_DEBUG_ENVIRONMENT');
  process.env.DEBUG='';
  const requireDependencies=createRequire(resolve(dependenciesRoot,'package.json'));
  const corePackage=requireDependencies.resolve('playwright-core/package.json');
  const coreRequire=createRequire(corePackage), coreDir=dirname(corePackage);
  const utilsPath=coreRequire.resolve('playwright-core/lib/utilsBundle');
  const playwrightPackage=requireDependencies.resolve('playwright/package.json');
  if(createRequire(playwrightPackage).resolve('playwright-core/package.json')!==corePackage)
    fail('PLAYWRIGHT_CORE_RESOLUTION_MISMATCH');
  sourcePaths=[self,playwrightPackage,requireDependencies.resolve('playwright'),corePackage,
    coreRequire.resolve('playwright-core'),coreRequire.resolve('playwright-core/lib/coreBundle'),utilsPath,
    resolve(coreDir,'types/types.d.ts'),resolve(coreDir,'types/protocol.d.ts')];
  report.sourceBefore=sourcePaths.map(identity);
  const packageInfo=JSON.parse(readFileSync(corePackage,'utf8'));
  if(packageInfo.version!=='1.63.0')fail('UNREVIEWED_PLAYWRIGHT_VERSION');
  report.playwrightVersion=packageInfo.version;
  const utilities=coreRequire('playwright-core/lib/utilsBundle');
  if(typeof utilities.debug?.disable!=='function'||typeof utilities.ws!=='function')fail('BUNDLED_TRANSPORT_UNAVAILABLE');
  utilities.debug.disable();
  const {chromium}=requireDependencies('playwright');
  const before=forwardRows(adb('forward-list-before',['forward','--list']));
  if(before.some(row=>row[0]===deviceId&&row[2]==='localabstract:chrome_devtools_remote'))
    fail('EXISTING_FORWARD_OWNER');
  const packages=adb('package-check',['shell','pm','list','packages','org.chromium.chrome']);
  if(!packages.split(/\r?\n/).includes('package:org.chromium.chrome'))fail('EXPECTED_CHROMIUM_PACKAGE_MISSING');
  const activity=adb('resolve-activity',['shell','cmd','package','resolve-activity','--brief','org.chromium.chrome'])
    .split(/\r?\n/).find(line=>/^org\.chromium\.chrome\/[A-Za-z0-9_.$]+$/.test(line));
  if(!activity)fail('EXPECTED_CHROMIUM_ACTIVITY_MISSING');
  const foreground=adb('foreground',['shell','am','start','-W','-a','android.intent.action.MAIN',
    '-c','android.intent.category.LAUNCHER','-n',activity]);
  report.foreground={commandReturned:true,statusOk:/^Status:\s*ok\s*$/m.test(foreground)};
  if(!report.foreground.statusOk)fail('ANDROID_START_STATUS_NOT_OK');
  stage('android-chromium-foreground-command-returned');
  report.mapping.attempted=true;report.mapping.outcome='pending';
  try {
    const output=adb('forward-create',['forward','--no-rebind','tcp:0','localabstract:chrome_devtools_remote']);
    if(!/^[0-9]+$/.test(output))fail('FORWARD_PORT_NOT_REPORTED');
    const port=Number(output);if(!Number.isSafeInteger(port)||port<1||port>65535)fail('FORWARD_PORT_INVALID');
    report.mapping.port=port;report.mapping.acquired=true;report.mapping.outcome='acquired';
  } catch(error){report.mapping.outcome='acquisition-unknown';throw error;}
  stage('owned-forward-ready');
  const websocketUrl=await versionAt('http://127.0.0.1:'+report.mapping.port);
  stage('devtools-version-ready');
  transportOwner=makeTransport(utilities.ws,websocketUrl);
  await bounded(transportOwner.openedPromise,LIMITS.wsOpenMs,'WEBSOCKET_OPEN_TIMEOUT');
  stage('owned-websocket-open');
  report.connection.attempted=true;report.connection.startedAtMs=clock();
  connectionPromise=chromium.connectOverCDP(transportOwner.transport,
    {timeout:LIMITS.connectMs,noDefaults:true,artifactsDir:base}).then(value=>{
      connectionSettled=true;browser=value;report.connection.settled='fulfilled';
      report.connection.connected=value.isConnected();report.connection.returnedAtMs=clock();
      report.connection.returnedAfterClosing=closing;return value;
    },error=>{
      connectionSettled=true;report.connection.settled='rejected';
      report.connection.rejectedAtMs=clock();report.connection.error=safeError(error);throw error;
    });
  await bounded(connectionPromise,LIMITS.connectMs,'PLAYWRIGHT_INITIALIZATION_TIMEOUT');
  stage('playwright-initialization-returned',{connected:report.connection.connected});
} catch(error) {
  report.failure={atMs:clock(),...safeError(error)};
  stage('retained-connection-failure',{name:report.failure.name,code:report.failure.code});
} finally {
  report.beforeCleanup=capture();phase='teardown';closing=true;
  if(browser)try{browserCloseAttempted=true;await bounded(browser.close(),LIMITS.closeMs,'BROWSER_DISCONNECT_TIMEOUT');}
    catch(error){report.cleanupErrors.push({boundary:'browser-disconnect',...safeError(error)});}
  if(transportOwner) {
    try{transportOwner.requestClose();}
    catch(error){report.cleanupErrors.push({boundary:'websocket-close-call',...safeError(error)});}
    try{await bounded(transportOwner.closedPromise,LIMITS.closeMs,'WEBSOCKET_CLOSE_TIMEOUT');}
    catch(error){
      report.cleanupErrors.push({boundary:'websocket-graceful-close',...safeError(error)});
      try{transportOwner.terminate();report.forcedSocketTermination=true;}
      catch(terminateError){report.cleanupErrors.push({boundary:'websocket-terminate-call',...safeError(terminateError)});}
      try{await bounded(transportOwner.closedPromise,LIMITS.terminateMs,'WEBSOCKET_TERMINATE_TIMEOUT');}
      catch(second){report.cleanupErrors.push({boundary:'websocket-terminate',...safeError(second)});}
    }
    report.socketClosed=transportOwner.isClosed();
  } else report.socketClosed=true;
  if(connectionPromise&&!connectionSettled)try{
    await bounded(connectionPromise.then(()=>{},()=>{}),LIMITS.closeMs,'PLAYWRIGHT_CONNECTION_SETTLE_TIMEOUT');
  }catch(error){report.cleanupErrors.push({boundary:'connection-settle',...safeError(error)});}
  if(browser&&!browserCloseAttempted)try{browserCloseAttempted=true;
    await bounded(browser.close(),LIMITS.closeMs,'LATE_BROWSER_DISCONNECT_TIMEOUT');
  }catch(error){report.cleanupErrors.push({boundary:'late-browser-disconnect',...safeError(error)});}
  report.connection.settledAfterCleanup=connectionSettled;
  if(report.mapping.acquired) {
    try {
      const current=forwardRows(adb('forward-list-cleanup',['forward','--list']));
      if(ownedTuple(current))adb('forward-remove-exact',['forward','--remove','tcp:'+report.mapping.port]);
      else report.mapping.cleanupSkipped='exact-owned-tuple-already-absent';
      const after=forwardRows(adb('forward-list-after',['forward','--list']));
      report.mapping.exactOwnedTupleAbsent=!ownedTuple(after);
      if(!report.mapping.exactOwnedTupleAbsent)fail('OWNED_FORWARD_REMAINS');
    } catch(error){report.cleanupErrors.push({boundary:'forward-cleanup',...safeError(error)});}
  } else if(report.mapping.attempted) {
    report.mapping.exactOwnedTupleAbsent=null;
    report.cleanupErrors.push({boundary:'forward-acquisition',code:'MAPPING_OUTCOME_UNKNOWN'});
  } else report.mapping.exactOwnedTupleAbsent=true;
  // Let callback rejections and close notifications settle before final metadata.
  await nextTurn();await nextTurn();
  report.afterCleanup=capture();
  try{report.sourceAfter=sourcePaths.map(identity);}
  catch(error){report.cleanupErrors.push({boundary:'source-identity',...safeError(error)});}
  report.sourceStable=report.sourceBefore.length>0&&JSON.stringify(report.sourceBefore)===JSON.stringify(report.sourceAfter);
  report.cleanupQualified=report.socketClosed&&report.mapping.exactOwnedTupleAbsent===true&&report.cleanupErrors.length===0;
  report.diagnosticComplete=report.connection.attempted&&connectionSettled&&report.beforeCleanup.complete&&
    report.afterCleanup.complete&&report.sourceStable&&report.cleanupQualified;
  report.passed=report.connection.connected&&!report.failure&&report.failures.length===0&&report.diagnosticComplete;
  report.endedAt=new Date().toISOString();report.endedAtMs=clock();
  writeFileSync(reportPath,JSON.stringify(report,null,2)+'\n',{flag:'wx'});
  console.log(JSON.stringify({report:identity(reportPath),passed:report.passed,
    diagnosticComplete:report.diagnosticComplete,connected:report.connection.connected,
    pendingBeforeCleanup:report.beforeCleanup.pending.length,pendingComplete:report.beforeCleanup.pendingComplete,
    sourceStable:report.sourceStable,cleanupQualified:report.cleanupQualified}));
  if(!report.passed)process.exitCode=1;
}

