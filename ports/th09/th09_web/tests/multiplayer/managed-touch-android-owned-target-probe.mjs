/**
 * Raw CDP diagnosis of one newly created about:blank target.
 * No Playwright browser initialization/auto-attach, game, input load, reverse
 * mapping, existing-target navigation/close, or browser/device file API.
 * Only ordinary wake and a live-confirmed non-secure keyguard dismissal are
 * permitted. No credentials, security/sleep setting change, restart, or install.
 * Metadata-only evidence is create-only under an exclusive RUN_LABEL.
 */
import {createHash} from 'node:crypto';
import {existsSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {createRequire} from 'node:module';
import {dirname, isAbsolute, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {performance} from 'node:perf_hooks';
import {setImmediate as nextTurn, setTimeout as delay} from 'node:timers/promises';

const LIMITS=Object.freeze({adbMs:20000,httpMs:10000,httpBytes:1024*1024,
  wsOpenMs:10000,wsPayloadBytes:1024*1024,commandMs:5000,createAckGraceMs:2000,
  closeMs:3000,terminateMs:1000,dispatchMessages:128,records:512,commands:24,
  faults:24,absenceChecks:3,absenceIntervalMs:100});
const label=process.env.RUN_LABEL;
if(!/^[a-z0-9_-]{1,100}$/.test(label||''))throw Error('A safe exclusive RUN_LABEL is required');
const root=process.cwd(),self=fileURLToPath(import.meta.url);
const base=resolve(root,'th09_web/artifacts/multiplayer-tests',label),reportPath=base+'-report.json';
if(existsSync(base)||existsSync(reportPath))throw Error('Run output already exists');
mkdirSync(base);
const clock=()=>performance.now();
const sha=value=>createHash('sha256').update(value).digest('hex');
const identity=path=>{const bytes=readFileSync(path);return {path,bytes:bytes.length,sha256:sha(bytes)};};
const atom=value=>typeof value==='string'&&/^[A-Za-z0-9_.-]{1,80}$/.test(value)?value:null;
const safeError=error=>({name:atom(error?.name),code:Number.isSafeInteger(error?.code)?error.code:atom(error?.code)});
const fail=code=>{const error=new Error();error.code=code;throw error;};
const opaqueId=value=>typeof value==='string'&&/^[A-Za-z0-9_-]{1,256}$/.test(value)?value:null;
const methodName=value=>typeof value==='string'&&value.length<=120&&
  /^[A-Za-z][A-Za-z0-9_]*(\.[A-Za-z][A-Za-z0-9_]*)+$/.test(value)?value:null;
const report={schema:'th09-managed-touch-android-owned-target/v1',runLabel:label,
  startedAt:new Date().toISOString(),clock:{name:'node:perf_hooks.performance',timeOrigin:performance.timeOrigin},
  scope:'Raw CDP only: one probe-created blank target. No Playwright existing-page initialization, gameplay, physical input, or performance acceptance.',
  metadataPolicy:'Only protocol id/method, connection/owned route, Node clocks, response/error-code flags, confirmed owned target identity, and derived validation booleans. No other target identity, URL, params/result, error.message, raw frame, or stack is persisted.',
  plan:{primary:'browser root control then owned flattened session',fallback:'owned page WebSocket only after primary failure',
    pageCommands:'Page.enable and fixed arithmetic evaluation are independent and have individual deadlines',
    interpretation:'A five-second timeout is an observed deadline, not proof of permanent failure; no generalized Playwright/gameplay conclusion',
    deviceStateScope:'Initial keyguard/power/activity observations, not continuous awake or foreground monitoring'},
  limits:LIMITS,stages:[],adbOperations:[],httpOperations:[],lookups:[],keyguardParses:[],commands:[],failures:[],cleanupIssues:[],
  sourceBefore:[],sourceAfter:[],connections:[],
  mapping:{attempted:false,acquired:false,port:null,outcome:'not-attempted'},
  owned:{creationAttempted:false,creationAcknowledged:false,targetId:null,flattenedSessionSha256:null,
    closeAttempted:false,closeResponseSucceeded:false,absentAfterCleanup:null},
  primary:{attempted:false},pageSocket:{attempted:false}};
let adbPath,deviceId,WebSocket,browserUrl,endpoint,rootClient,ownedSession;
let phase='preparing',sourcePaths=[self],sequence=0,ringAt=0,overwritten=0,recordingFaults=0;
const clients=[],records=[];
let resolveCreateAck;
const createAckObserved=new Promise(resolve=>{resolveCreateAck=resolve;});
let createHandle=null;
const stage=(name,fields={})=>{const row={name,atMs:clock(),...fields};report.stages.push(row);console.log(JSON.stringify(row));};
function append(value){
  const row={seq:++sequence,phase,...value};
  if(records.length<LIMITS.records)records.push(row);
  else{records[ringAt]=row;ringAt=(ringAt+1)%LIMITS.records;overwritten++;}
}
function fault(boundary,error){
  recordingFaults++;
  if(report.failures.length<LIMITS.faults)report.failures.push({boundary,atMs:clock(),...safeError(error)});
}
async function bounded(promise,ms,code){
  let timer;try{return await Promise.race([promise,new Promise((_,reject)=>{timer=setTimeout(()=>{
    const error=new Error();error.name='TimeoutError';error.code=code;reject(error);
  },ms);})]);}finally{clearTimeout(timer);}
}
function adb(operation,args){
  const startedAtMs=clock(),r=spawnSync(adbPath,['-s',deviceId,...args],
    {encoding:'utf8',windowsHide:true,timeout:LIMITS.adbMs,maxBuffer:1024*1024});
  report.adbOperations.push({operation,startedAtMs,returnedAtMs:clock(),status:r.status,
    signal:atom(r.signal),error:safeError(r.error)});
  if(r.error||r.status!==0)fail('ADB_OPERATION_FAILED');
  return (r.stdout||'').trim();
}
function forwards(text){return text.split(/\r?\n/).map(line=>line.trim().split(/\s+/));}
function ownedForward(rows){return rows.some(row=>row[0]===deviceId&&row[1]==='tcp:'+report.mapping.port&&
  row[2]==='localabstract:chrome_devtools_remote');}
function liveKeyguard(text){
  const lines=text.split(/\r?\n/),heads=[];
  for(let i=0;i<lines.length;i++)if(/^\s*KeyguardServiceDelegate\b/.test(lines[i]))heads.push(i);
  const parse={atMs:clock(),blockMatches:heads.length,fieldMatches:{}};report.keyguardParses.push(parse);
  if(heads.length!==1)fail('KEYGUARD_DELEGATE_BLOCK_AMBIGUOUS');
  const begin=heads[0],indent=lines[begin].match(/^\s*/)[0].length;let end=begin+1;
  while(end<lines.length&&(!lines[end].trim()||lines[end].match(/^\s*/)[0].length>indent))end++;
  const block=lines.slice(begin,end).join('\n'),out={};parse.blockLines=end-begin;parse.headerIndent=indent;
  for(const key of ['secure','showing','inputRestricted']){
    const values=[...block.matchAll(new RegExp('\\b'+key+'=(true|false)\\b','g'))].map(match=>match[1]);
    parse.fieldMatches[key]=values.length;if(values.length===1)out[key]=values[0]==='true';
  }
  if(Object.values(parse.fieldMatches).some(count=>count!==1))fail('KEYGUARD_FIELD_AMBIGUOUS');
  return out;
}
function livePower(text){
  const awake=[...text.matchAll(/\bmWakefulness=(Awake|Asleep|Dreaming|Dozing)\b/g)].map(match=>match[1]);
  const displayStates=[...new Set([...text.matchAll(/\bDisplay Power:\s*state=(ON|OFF|DOZE|DOZE_SUSPEND|ON_SUSPEND|UNKNOWN)\b/g)].map(match=>match[1]))];
  return {wakefulness:awake.length===1?awake[0]:null,displayStates:displayStates.slice(0,8),
    fieldsUnambiguous:awake.length===1&&displayStates.length===1};
}
async function readJson(path,purpose){
  const operation={purpose,startedAtMs:clock(),status:null,bodyParsed:false};report.httpOperations.push(operation);
  const abort=new AbortController(),timer=setTimeout(()=>abort.abort(),LIMITS.httpMs);let reader;
  try{
    const response=await fetch(endpoint+path,{signal:abort.signal,redirect:'error'});operation.status=response.status;
    if(response.status!==200||!response.body)fail('DEVTOOLS_HTTP_STATUS');
    reader=response.body.getReader();const chunks=[];let bytes=0;
    while(true){const part=await reader.read();if(part.done)break;
      bytes+=part.value.length;if(bytes>LIMITS.httpBytes){abort.abort();fail('DEVTOOLS_HTTP_BODY_LIMIT');}
      chunks.push(part.value);
    }
    const value=JSON.parse(Buffer.concat(chunks,bytes).toString('utf8'));
    operation.bodyParsed=true;return value;
  }catch(error){operation.error=safeError(error);throw error;}
  finally{clearTimeout(timer);abort.abort();reader?.releaseLock();operation.returnedAtMs=clock();}
}
function ownedSocketUrl(value,kind,targetId){
  if(typeof value!=='string')fail('DEVTOOLS_WEBSOCKET_MISSING');
  const url=new URL(value),baseUrl=new URL(endpoint),prefix='/devtools/'+kind;
  if(url.protocol!=='ws:'||!(url.pathname===prefix||url.pathname.startsWith(prefix+'/')))
    fail('DEVTOOLS_WEBSOCKET_KIND_INVALID');
  if(kind==='page'&&(!opaqueId(targetId)||url.pathname!==prefix+'/'+encodeURIComponent(targetId)))
    fail('OWNED_PAGE_WEBSOCKET_TARGET_MISMATCH');
  url.host=baseUrl.host;url.username='';url.password='';
  return url.href;
}
async function lookupOwned(purpose,needSocket=false){
  if(!report.owned.targetId)fail('OWNED_ID_REQUIRED');
  const list=await readJson('/json/list',purpose);
  if(!Array.isArray(list))fail('DEVTOOLS_TARGET_LIST_INVALID');
  const matches=list.filter(value=>value?.id===report.owned.targetId);
  const row={purpose,atMs:clock(),ownedMatches:matches.length,ownedTypePage:matches.length===1&&matches[0].type==='page'};
  report.lookups.push(row);
  if(matches.length>1)fail('OWNED_TARGET_ID_AMBIGUOUS');
  if(!needSocket)return matches.length===0;
  if(matches.length!==1||!row.ownedTypePage)fail('OWNED_PAGE_NOT_FOUND');
  return ownedSocketUrl(matches[0].webSocketDebuggerUrl,'page',report.owned.targetId);
}
function rawConnection(kind,url){
  const state={kind,createdAtMs:clock(),opened:false,closed:false,closeRequested:false,
    sent:0,responses:0,events:0,unknownResponses:0,duplicateResponses:0,invalidMetadata:0,
    sessionMismatches:0,dispatchOverflow:0,dispatchPeak:0};
  report.connections.push(state);
  const socket=new WebSocket(url,[],{maxPayload:LIMITS.wsPayloadBytes,followRedirects:false,
    perMessageDeflate:{clientNoContextTakeover:true,zlibDeflateOptions:{level:3},
      zlibInflateOptions:{chunkSize:10*1024},threshold:10*1024}});
  const entries=new Map(),queue=new Array(LIMITS.dispatchMessages);let head=0,size=0,scheduled=0,nextId=0;
  let resolveOpen,rejectOpen,resolveClosed;
  const opened=new Promise((resolve,reject)=>{resolveOpen=resolve;rejectOpen=reject;});
  const closed=new Promise(resolve=>{resolveClosed=resolve;});
  const isOpen=()=>socket.readyState===WebSocket.OPEN;
  function requestClose(){state.closeRequested=true;if(socket.readyState!==WebSocket.CLOSED)socket.close();}
  function transportFault(boundary,error){
    fault(kind+'-'+boundary,error);try{requestClose();}catch(second){fault(kind+'-fault-close',second);}
  }
  function issue(method,params={},route='browser-root',sessionId,observeResult){
    if(report.commands.length>=LIMITS.commands)fail('COMMAND_LIMIT');
    if(!methodName(method))fail('AUTHORED_METHOD_INVALID');
    const row={connection:kind,route,id:++nextId,method,issuedAtMs:clock(),sendAttempted:false,
      responsePresent:false,timedOut:false,settled:false};
    report.commands.push(row);
    let finish;
    const promise=new Promise(resolve=>{finish=resolve;});
    const entry={row,sessionId,observeResult,timer:null,finish};
    entries.set(row.id,entry);
    function settle(outcome){
      if(row.settled)return;
      row.settled=true;row.outcome=outcome;row.settledAtMs=clock();clearTimeout(entry.timer);
      finish({ok:outcome==='response',row});
    }
    entry.settle=settle;
    if(!isOpen()){settle('socket-not-open');return {row,promise};}
    const message={id:row.id,method,params};if(sessionId!==undefined)message.sessionId=sessionId;
    row.sendAttempted=true;row.sentAtMs=clock();state.sent++;
    append({kind:'command',connection:kind,route,id:row.id,method,atMs:row.sentAtMs});
    entry.timer=setTimeout(()=>{row.timedOut=true;settle('timeout');},LIMITS.commandMs);
    try{
      socket.send(JSON.stringify(message),error=>{
        row.sendCallbackAtMs=clock();row.sendCallbackOk=!error;
        if(error){row.sendError=safeError(error);settle('send-failed');}
      });
    }catch(error){row.sendError=safeError(error);settle('send-failed');}
    return {row,promise};
  }
  function receive(message,receivedAtMs){
    const atMs=clock(),id=Number.isSafeInteger(message?.id)?message.id:null;
    if(id===null){
      const method=methodName(message?.method);state.events++;if(!method)state.invalidMetadata++;
      append({kind:'event',connection:kind,id:null,method,atMs,receivedAtMs});return;
    }
    state.responses++;const entry=entries.get(id);
    const metadata={kind:'response',connection:kind,id,method:entry?.row.method??null,
      route:entry?.row.route??null,atMs,receivedAtMs,responsePresent:true,errorPresent:!!message.error,
      errorCode:Number.isSafeInteger(message.error?.code)?message.error.code:null};
    append(metadata);
    if(!entry){state.unknownResponses++;return;}
    const row=entry.row;
    if(row.responsePresent){state.duplicateResponses++;return;}
    const sessionMatches=entry.sessionId===undefined?(message.sessionId==null||message.sessionId===''):message.sessionId===entry.sessionId;
    row.responsePresent=true;row.responseAtMs=atMs;row.socketCallbackAtMs=receivedAtMs;
    row.responseElapsedMs=row.sentAtMs==null?null:atMs-row.sentAtMs;
    row.localDispatchAfterSocketCallbackMs=atMs-receivedAtMs;
    row.sessionMatches=sessionMatches;row.errorPresent=metadata.errorPresent;row.errorCode=metadata.errorCode;
    row.lateResponse=row.settled;
    let outcome='response';
    if(!sessionMatches){state.sessionMismatches++;outcome='session-mismatch';}
    else if(metadata.errorPresent)outcome='protocol-error';
    else if(!message.result||typeof message.result!=='object'||Array.isArray(message.result))outcome='invalid-result';
    else if(entry.observeResult){
      try{row.validation=entry.observeResult(message.result);if(row.validation?.valid!==true)outcome='invalid-result';}
      catch(error){row.validationError=safeError(error);outcome='invalid-result';}
    }
    row.responseOutcome=outcome;entry.settle(outcome);
  }
  function schedule(){
    while(scheduled<size){
      scheduled++;setImmediate(()=>{
        scheduled--;if(!size)return;
        const item=queue[head];queue[head]=undefined;head=(head+1)%queue.length;size--;
        let value;try{value=JSON.parse(item.data.toString());}
        catch(error){transportFault('json-decode',error);return;}
        try{receive(value,item.receivedAtMs);}
        catch(error){transportFault('message-processing',error);}
      });
    }
  }
  socket.on('message',data=>{
    if(size>=queue.length){state.dispatchOverflow++;transportFault('queue-overflow',{code:'DISPATCH_QUEUE_LIMIT'});return;}
    queue[(head+size)%queue.length]={data,receivedAtMs:clock()};size++;
    state.dispatchPeak=Math.max(state.dispatchPeak,size);schedule();
  });
  socket.on('open',()=>{state.opened=true;state.openedAtMs=clock();resolveOpen();});
  socket.on('error',error=>{state.socketError=safeError(error);rejectOpen(error);transportFault('socket-error',error);});
  socket.on('unexpected-response',(_request,response)=>{
    state.handshakeStatus=response.statusCode??null;response.resume();
    const error=new Error();error.code='WEBSOCKET_HANDSHAKE_STATUS';rejectOpen(error);transportFault('handshake',error);
  });
  socket.on('close',code=>{
    state.closed=true;state.closedAtMs=clock();state.closeCode=code;resolveClosed();
    if(!state.opened){const error=new Error();error.code='WEBSOCKET_CLOSED_BEFORE_OPEN';rejectOpen(error);}
    for(const entry of entries.values())if(!entry.row.settled)entry.settle('connection-closed');
  });
  return {kind,state,opened,closed,issue,isOpen,requestClose,
    terminate:()=>socket.terminate(),
    snapshot:()=>({...state,queued:size,scheduled,
      pending:[...entries.values()].filter(entry=>entry.row.sendAttempted&&!entry.row.responsePresent)
        .map(entry=>({id:entry.row.id,method:entry.row.method,route:entry.row.route,
          outcome:entry.row.outcome??null,ageAtSnapshotMs:clock()-entry.row.sentAtMs}))})};
}
async function connect(kind,url){
  const client=rawConnection(kind,url);clients.push(client);
  await bounded(client.opened,LIMITS.wsOpenMs,'WEBSOCKET_OPEN_TIMEOUT');return client;
}
async function pageChecks(client,route,sessionId){
  const enable=client.issue('Page.enable',{},route,sessionId);
  const evaluate=client.issue('Runtime.evaluate',{expression:'1+1',returnByValue:true},route,sessionId,result=>{
    const exceptionPresent=Object.hasOwn(result,'exceptionDetails');
    const evaluationEqualsTwo=result.result?.type==='number'&&result.result.value===2;
    return {valid:!exceptionPresent&&evaluationEqualsTwo,exceptionPresent,evaluationEqualsTwo};
  });
  const [a,b]=await Promise.all([enable.promise,evaluate.promise]);
  return {pageEnableCommand:{connection:client.kind,id:enable.row.id},evaluationCommand:{connection:client.kind,id:evaluate.row.id},
    pageEnableResponded:a.ok,evaluationValidated:b.ok,passed:a.ok&&b.ok};
}
function snapshot(){
  const ordered=overwritten?[...records.slice(ringAt),...records.slice(0,ringAt)]:records;
  return {atMs:clock(),phase,owned:{...report.owned},commands:structuredClone(report.commands),
    connections:clients.map(client=>client.snapshot()),retainedRecords:records.length,totalRecords:sequence,overwritten,
    records:ordered.map(row=>({...row})),
    complete:overwritten===0&&recordingFaults===0&&clients.every(client=>client.state.dispatchOverflow===0&&
      client.state.unknownResponses===0&&client.state.duplicateResponses===0&&client.state.invalidMetadata===0),
    scope:'Node send observation to Node receive dispatch; not device CPU, wire RTT, gameplay or FPS. Route mismatch/error/timeout stays distinct.'};
}
async function closeClient(client){
  if(client.state.closed)return;
  try{client.requestClose();}catch(error){report.cleanupIssues.push({boundary:client.kind+'-close-call',...safeError(error)});}
  try{await bounded(client.closed,LIMITS.closeMs,'WEBSOCKET_CLOSE_TIMEOUT');}
  catch(error){
    report.cleanupIssues.push({boundary:client.kind+'-graceful-close',...safeError(error)});
    try{client.terminate();client.state.forcedTermination=true;}catch(second){report.cleanupIssues.push({boundary:client.kind+'-terminate-call',...safeError(second)});}
    try{await bounded(client.closed,LIMITS.terminateMs,'WEBSOCKET_TERMINATE_TIMEOUT');}
    catch(second){report.cleanupIssues.push({boundary:client.kind+'-terminate-wait',...safeError(second)});}
  }
}
async function closeKnownTarget(){
  if(!report.owned.creationAcknowledged||!report.owned.targetId||report.owned.closeAttempted)return;
  report.owned.closeAttempted=true;report.owned.closeStartedAtMs=clock();
  try{
    const cleanupClient=rootClient?.isOpen()?rootClient:await connect('browser-cleanup',browserUrl);
    const close=cleanupClient.issue('Target.closeTarget',{targetId:report.owned.targetId},'browser-owned-cleanup',undefined,
      result=>({valid:result.success===true,closeSucceeded:result.success===true}));
    report.owned.closeCommand={connection:cleanupClient.kind,id:close.row.id};
    const result=await close.promise;report.owned.closeResponseSucceeded=result.ok;
  }catch(error){report.cleanupIssues.push({boundary:'owned-target-close',...safeError(error)});}
}
async function verifyOwnedAbsent(){
  if(!report.owned.targetId)return;
  try{
    for(let i=0;i<LIMITS.absenceChecks;i++){
      const absent=await lookupOwned('owned-absence-'+(i+1));
      report.owned.absentAfterCleanup=absent;report.owned.absenceObservedAtMs=clock();
      if(absent)return;
      if(i+1<LIMITS.absenceChecks)await delay(LIMITS.absenceIntervalMs);
    }
  }catch(error){report.owned.absentAfterCleanup=null;report.cleanupIssues.push({boundary:'owned-target-absence',...safeError(error)});}
}
const onUnhandled=error=>fault('unhandled-rejection',error);
process.on('unhandledRejection',onUnhandled);
try{
  if(self!==resolve(root,'th09_web/tests/multiplayer/managed-touch-android-owned-target-probe.mjs'))fail('WRONG_WORKTREE_CWD');
  adbPath=process.env.TH09_ADB_PATH;deviceId=process.env.TH09_ANDROID_SERIAL;
  const dependenciesRoot=process.env.TH09_TEST_DEPENDENCIES_ROOT;
  if(!adbPath||!isAbsolute(adbPath)||!existsSync(adbPath)||!deviceId||
    !dependenciesRoot||!isAbsolute(dependenciesRoot))fail('EXPLICIT_EXISTING_DEPENDENCIES_REQUIRED');
  report.owner={adbPath,deviceId,package:'org.chromium.chrome'};
  report.environment={pwDebugPresent:!!process.env.PWDEBUG,attachToOtherPresent:!!process.env.PW_CHROMIUM_ATTACH_TO_OTHER,
    debugWasNonempty:!!process.env.DEBUG};
  if(report.environment.pwDebugPresent||report.environment.attachToOtherPresent)fail('UNSUPPORTED_DEBUG_ENVIRONMENT');
  process.env.DEBUG='';
  const dependencies=createRequire(resolve(dependenciesRoot,'package.json'));
  const corePackage=dependencies.resolve('playwright-core/package.json'),coreRequire=createRequire(corePackage),coreDir=dirname(corePackage);
  const utilsPath=coreRequire.resolve('playwright-core/lib/utilsBundle');
  sourcePaths=[self,corePackage,utilsPath,resolve(coreDir,'types/protocol.d.ts')];report.sourceBefore=sourcePaths.map(identity);
  const info=JSON.parse(readFileSync(corePackage,'utf8'));if(info.version!=='1.63.0')fail('UNREVIEWED_BUNDLED_WS_VERSION');
  report.playwrightCoreVersion=info.version;report.transport='Raw CDP using installed bundled ws only; no Playwright browser API';
  const utilities=coreRequire('playwright-core/lib/utilsBundle');
  if(typeof utilities.ws!=='function'||typeof utilities.debug?.disable!=='function')fail('BUNDLED_WS_UNAVAILABLE');
  utilities.debug.disable();WebSocket=utilities.ws;
  const before=forwards(adb('forward-list-before',['forward','--list']));
  if(before.some(row=>row[0]===deviceId&&row[2]==='localabstract:chrome_devtools_remote'))fail('EXISTING_FORWARD_OWNER');
  const packages=adb('package-check',['shell','pm','list','packages','org.chromium.chrome']);
  if(!packages.split(/\r?\n/).includes('package:org.chromium.chrome'))fail('EXPECTED_CHROMIUM_PACKAGE_MISSING');
  const activity=adb('resolve-activity',['shell','cmd','package','resolve-activity','--brief','org.chromium.chrome'])
    .split(/\r?\n/).find(line=>/^org\.chromium\.chrome\/[A-Za-z0-9_.$]+$/.test(line));
  if(!activity)fail('EXPECTED_CHROMIUM_ACTIVITY_MISSING');
  adb('ordinary-wake',['shell','input','keyevent','KEYCODE_WAKEUP']);
  report.keyguardBefore=liveKeyguard(adb('keyguard-before',['shell','dumpsys','window','policy']));
  if(report.keyguardBefore.showing||report.keyguardBefore.inputRestricted){
    if(report.keyguardBefore.secure!==false)fail('SECURE_KEYGUARD_BLOCKING');
    adb('dismiss-live-nonsecure-keyguard',['shell','wm','dismiss-keyguard']);report.nonsecureDismissRequested=true;
  }else report.nonsecureDismissRequested=false;
  const foreground=adb('foreground',['shell','am','start','-W','-a','android.intent.action.MAIN',
    '-c','android.intent.category.LAUNCHER','-n',activity]);
  if(!/^Status:\s*ok\s*$/m.test(foreground))fail('ANDROID_START_STATUS_NOT_OK');
  report.keyguardAfter=liveKeyguard(adb('keyguard-after',['shell','dumpsys','window','policy']));
  report.power=livePower(adb('power-state',['shell','dumpsys','power']));
  const resumed=adb('resumed-activity',['shell','dumpsys','activity','activities']).split(/\r?\n/)
    .filter(line=>/\bmResumedActivity\b/.test(line));
  report.foreground={commandReturned:true,resumedLines:resumed.length,
    chromiumResumed:resumed.length>0&&resumed.every(line=>/\borg\.chromium\.chrome\//.test(line))};
  report.deviceReady=!report.keyguardAfter.showing&&!report.keyguardAfter.inputRestricted&&
    report.power.fieldsUnambiguous&&report.power.wakefulness==='Awake'&&report.power.displayStates.includes('ON')&&report.foreground.chromiumResumed;
  if(!report.deviceReady)fail('DEVICE_NOT_CONFIRMED_AWAKE_UNLOCKED_CHROMIUM');
  stage('device-ready');
  report.mapping.attempted=true;report.mapping.outcome='pending';
  try{
    const output=adb('forward-create',['forward','--no-rebind','tcp:0','localabstract:chrome_devtools_remote']);
    if(!/^[0-9]+$/.test(output))fail('FORWARD_PORT_NOT_REPORTED');
    const port=Number(output);if(!Number.isSafeInteger(port)||port<1||port>65535)fail('FORWARD_PORT_INVALID');
    report.mapping.port=port;report.mapping.acquired=true;report.mapping.outcome='acquired';
  }catch(error){report.mapping.outcome='acquisition-unknown';throw error;}
  endpoint='http://127.0.0.1:'+report.mapping.port;
  const version=await readJson('/json/version','browser-endpoint-discovery');
  browserUrl=ownedSocketUrl(version.webSocketDebuggerUrl,'browser');
  phase='diagnosing';rootClient=await connect('browser-root',browserUrl);
  const versionReply=await rootClient.issue('Browser.getVersion').promise;
  report.browserVersionResponded=versionReply.ok;if(!versionReply.ok)fail('BROWSER_GET_VERSION_FAILED');
  stage('browser-root-responsive');
  createHandle=rootClient.issue('Target.createTarget',{url:'about:blank'},'browser-owned-create',undefined,result=>{
    const id=opaqueId(result.targetId);if(!id)return {valid:false};
    report.owned.creationAcknowledged=true;report.owned.targetId=id;report.owned.creationAckAtMs=clock();
    resolveCreateAck();return {valid:true,ownedIdAccepted:true};
  });
  report.owned.creationAttempted=createHandle.row.sendAttempted;
  report.owned.createCommand={connection:rootClient.kind,id:createHandle.row.id};
  const created=await createHandle.promise;if(!created.ok||!report.owned.targetId)fail('OWNED_TARGET_CREATION_NOT_CONFIRMED_IN_TIME');
  stage('owned-target-confirmed');
  const activated=await rootClient.issue('Target.activateTarget',{targetId:report.owned.targetId},'browser-owned-activate').promise;
  report.owned.activationResponded=activated.ok;
  report.primary.attempted=true;
  const attached=await rootClient.issue('Target.attachToTarget',{targetId:report.owned.targetId,flatten:true},'browser-owned-attach',undefined,result=>{
    const id=opaqueId(result.sessionId);if(!id)return {valid:false};
    ownedSession=id;report.owned.flattenedSessionSha256=sha(id);return {valid:true,ownedSessionAccepted:true};
  }).promise;
  report.primary.attachResponded=attached.ok;
  if(attached.ok&&ownedSession)Object.assign(report.primary,await pageChecks(rootClient,'owned-flattened',ownedSession));
  else report.primary.passed=false;
  stage('owned-flattened-finished',{attachResponded:report.primary.attachResponded,passed:report.primary.passed});
  if(!report.primary.passed||!report.owned.activationResponded){
    report.pageSocket.attempted=true;
    try{
      const pageUrl=await lookupOwned('owned-page-socket-discovery',true);
      const pageClient=await connect('owned-page',pageUrl);
      Object.assign(report.pageSocket,await pageChecks(pageClient,'owned-page'));
    }catch(error){report.pageSocket.error=safeError(error);report.pageSocket.passed=false;}
    stage('owned-page-socket-finished',{passed:report.pageSocket.passed});
  }
}catch(error){report.failure={atMs:clock(),...safeError(error)};stage('retained-probe-failure',report.failure);}
finally{
  report.beforeCleanup=snapshot();phase='cleanup';
  if(report.owned.creationAttempted&&!report.owned.creationAcknowledged&&!createHandle?.row.responsePresent){
    try{await bounded(createAckObserved,LIMITS.createAckGraceMs,'CREATE_ACK_REMAINS_UNKNOWN');}
    catch(error){report.owned.creationAckGraceError=safeError(error);}
  }
  await nextTurn();await nextTurn();
  await closeKnownTarget();
  for(const client of [...clients])await closeClient(client);
  // A create ACK that arrived during close can still establish this exact owner.
  // Only that confirmed ID may receive one late cleanup connection; no tab diff.
  await nextTurn();await nextTurn();
  if(report.owned.creationAcknowledged&&!report.owned.closeAttempted){
    await closeKnownTarget();for(const client of [...clients])await closeClient(client);
  }
  await verifyOwnedAbsent();
  report.owned.creationOutcomeUnknown=report.owned.creationAttempted&&!report.owned.creationAcknowledged;
  report.owned.cleanupQualified=!report.owned.creationAttempted||
    report.owned.creationAcknowledged&&report.owned.closeAttempted&&report.owned.absentAfterCleanup===true;
  if(report.mapping.acquired){
    try{
      const current=forwards(adb('forward-list-cleanup',['forward','--list']));
      if(ownedForward(current))adb('forward-remove-exact',['forward','--remove','tcp:'+report.mapping.port]);
      else report.mapping.cleanupSkipped='exact-owned-tuple-already-absent';
      report.mapping.exactOwnedTupleAbsent=!ownedForward(forwards(adb('forward-list-after',['forward','--list'])));
      if(!report.mapping.exactOwnedTupleAbsent)fail('OWNED_FORWARD_REMAINS');
    }catch(error){report.cleanupIssues.push({boundary:'forward-cleanup',...safeError(error)});}
  }else if(report.mapping.attempted){
    report.mapping.exactOwnedTupleAbsent=null;report.cleanupIssues.push({boundary:'forward-acquisition',code:'MAPPING_OUTCOME_UNKNOWN'});
  }else report.mapping.exactOwnedTupleAbsent=true;
  await nextTurn();await nextTurn();report.afterCleanup=snapshot();
  try{report.sourceAfter=sourcePaths.map(identity);}catch(error){report.cleanupIssues.push({boundary:'source-identity',...safeError(error)});}
  report.sourceStable=report.sourceBefore.length>0&&JSON.stringify(report.sourceBefore)===JSON.stringify(report.sourceAfter);
  report.cleanupQualified=report.owned.cleanupQualified&&clients.every(client=>client.state.closed)&&report.mapping.exactOwnedTupleAbsent===true;
  report.diagnosticComplete=!report.failure&&report.primary.attempted&&report.beforeCleanup.complete&&
    report.afterCleanup.complete&&report.sourceStable&&report.cleanupQualified&&clients.every(client=>client.snapshot().queued===0);
  report.flattenedResponsive=report.primary.passed===true;
  report.pageSocketResponsive=report.pageSocket.passed===true;
  report.ownedTargetResponsive=report.flattenedResponsive||report.pageSocketResponsive;
  report.outcome=!report.diagnosticComplete?'inconclusive-or-incomplete':report.flattenedResponsive?
    'owned-flattened-responsive':report.pageSocketResponsive?'owned-page-socket-responsive-after-primary-failure':
    'owned-target-not-responsive-within-declared-deadlines';
  report.endedAt=new Date().toISOString();report.endedAtMs=clock();
  writeFileSync(reportPath,JSON.stringify(report,null,2)+'\n',{flag:'wx'});
  console.log(JSON.stringify({report:identity(reportPath),diagnosticComplete:report.diagnosticComplete,outcome:report.outcome,
    flattenedResponsive:report.flattenedResponsive,pageSocketResponsive:report.pageSocketResponsive,
    cleanupQualified:report.cleanupQualified,sourceStable:report.sourceStable,
    creationOutcomeUnknown:report.owned.creationOutcomeUnknown}));
  if(!report.diagnosticComplete||!report.ownedTargetResponsive)process.exitCode=1;
}
