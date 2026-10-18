/**
 * One explicitly owned Android Chromium target, via raw flattened CDP.
 * Import is inert: no device action, file read/write, browser or mapping occurs.
 * The caller supplies an existing ADB, selected device, installed dependencies
 * and its own server port. This module creates one about:blank page and exact
 * mappings; it never auto-attaches, navigates or closes another target.
 *
 * send() returns the actual CDP result object, never a diagnostic substitute.
 * It performs no retries. A timeout is an unknown command outcome; after an
 * unknown Input outcome further Input is refused. Observe or close instead.
 * snapshot() is synchronous and detached; use at phase/failure boundaries,
 * not for every MOVE. close() is idempotent and returns the final snapshot.
 * Open/send failures expose the snapshot as ownedAndroidSession/diagnostics.
 * The caller persists its own report/screenshots; this module writes no file
 * and logs no protocol payload, page URL, evaluation body/result or screenshot.
 */
import {createHash} from 'node:crypto';
import {readFileSync, realpathSync, statSync} from 'node:fs';
import {createRequire} from 'node:module';
import {dirname, isAbsolute, relative, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {performance} from 'node:perf_hooks';
import {setImmediate as nextTurn, setTimeout as delay} from 'node:timers/promises';

const SELF=fileURLToPath(import.meta.url);
const METHODS=new Set(['Page.addScriptToEvaluateOnNewDocument','Page.navigate',
  'Page.enable','Page.captureScreenshot','Page.getLayoutMetrics','Runtime.evaluate',
  'Input.dispatchTouchEvent','Input.dispatchKeyEvent']);
const LIMITS=Object.freeze({adbMs:20000,httpMs:10000,httpBytes:1024*1024,
  wsOpenMs:10000,wsPayloadBytes:32*1024*1024,dispatchBytes:64*1024*1024,
  outboundBytes:16*1024*1024,dispatchMessages:128,pending:128,
  commands:4096,cleanupCommandReserve:8,cleanupPendingReserve:1,records:8192,faults:64,
  createAckGraceMs:2000,cleanupCommandMs:5000,closeMs:3000,terminateMs:1000,
  absenceChecks:3,absenceIntervalMs:100,maxCommandMs:60000});
const now=()=>performance.now();
const sha=value=>createHash('sha256').update(value).digest('hex');
const identity=path=>{const bytes=readFileSync(path);return {path,bytes:bytes.length,sha256:sha(bytes)};};
const atom=value=>typeof value==='string'&&/^[A-Za-z0-9_.-]{1,80}$/.test(value)?value:null;
const safeError=error=>({name:atom(error?.name),code:Number.isSafeInteger(error?.code)?error.code:atom(error?.code)});
const opaqueId=value=>typeof value==='string'&&/^[A-Za-z0-9_-]{1,256}$/.test(value)?value:null;
const methodName=value=>typeof value==='string'&&value.length<=120&&/^[A-Za-z][A-Za-z0-9_]*(\.[A-Za-z][A-Za-z0-9_]*)+$/.test(value)?value:null;
const errorFor=code=>{const error=new Error(code);error.code=code;return error;};
const fail=code=>{throw errorFor(code);};
const versionToken=value=>typeof value==='string'&&/^[A-Za-z0-9./_-]{1,120}$/.test(value)?value:null;
const contains=(owner,path)=>{const part=relative(owner,path);return part===''||!isAbsolute(part)&&part!=='..'&&!part.startsWith('../')&&!part.startsWith('..\\');};
async function bounded(promise,ms,code){
  let timer;try{return await Promise.race([promise,new Promise((_,reject)=>{timer=setTimeout(()=>reject(errorFor(code)),ms);})]);}
  finally{clearTimeout(timer);}
}

/** No device work runs until this function is explicitly called. */
export async function openOwnedAndroidSession({adbPath,serial,dependenciesRoot,reversePort,commandTimeoutMs=30000}={}){
  const report={schema:'th09-managed-touch-owned-android-session/v1',
    startedAt:new Date().toISOString(),startedAtMs:now(),
    clock:{name:'node:perf_hooks.performance',timeOrigin:performance.timeOrigin},
    transport:'Raw CDP over one owned flattened session; no Playwright browser initialization or isLocal emulation',
    scope:'Real caller-owned page only. Node command timing is not Android CPU, physical input latency, wire RTT or FPS.',
    metadataPolicy:'No params/results, other target identity or URL, protocol error.message, stack or raw frame. Only owned identity, two browser version tokens, clocks and command/response/cleanup metadata.',
    deviceStateScope:'Initial parsed keyguard/power/activity checks only; no continuous device monitoring.',
    limits:{...LIMITS,commandMs:Number.isSafeInteger(commandTimeoutMs)?commandTimeoutMs:null},phase:'preparing',stages:[],
    owner:{adbPath:typeof adbPath==='string'?adbPath:null,deviceId:typeof serial==='string'?serial:null,package:'org.chromium.chrome'},
    sourceBefore:[],sourceAfter:[],sourceStable:null,commands:[],records:[],
    failures:[],adbOperations:[],httpOperations:[],keyguardParses:[],lookups:[],
    counts:{publicCalls:0,rejectedPublicCalls:0,recordAttempts:0,recordOverflow:0,commandOverflow:0,
      pendingOverflow:0,faults:0,faultOverflow:0,commandFailures:0},
    forward:{attempted:false,acquired:false,verified:false,port:null,outcome:'not-attempted',exactOwnedTupleAbsent:null},
    reverse:{attempted:false,acquired:false,verified:false,port:Number.isInteger(reversePort)?reversePort:null,outcome:'not-attempted',exactOwnedTupleAbsent:null},
    owned:{creationAttempted:false,creationAcknowledged:false,targetId:null,flattenedSessionSha256:null,
      closeAttempted:false,closeResponseSucceeded:false,absentAfterCleanup:null},
    inputOutcomeUnknown:false,cleanup:{started:false,finished:false,qualified:null,issues:[]}};
  let sourcePaths=[SELF],WebSocket,endpoint,browserUrl,rootClient,ownedSession,createHandle,closePromise;
  let closedForPublic=false,cleanupConnectionUsed=false,recordingFailed=false;
  const clients=[];
  let resolveCreateAck;const createAckObserved=new Promise(resolve=>{resolveCreateAck=resolve;});
  const stage=name=>report.stages.push({name,atMs:now()});
  function fault(boundary,error,cleanup=false){
    report.counts.faults++;
    const rows=cleanup?report.cleanup.issues:report.failures;
    if(rows.length<LIMITS.faults)rows.push({boundary,atMs:now(),...safeError(error)});
    else report.counts.faultOverflow++;
  }
  function append(value){
    const sequence=++report.counts.recordAttempts;
    if(report.records.length<LIMITS.records)report.records.push({sequence,phase:report.phase,...value});
    else{report.counts.recordOverflow++;recordingFailed=true;}
  }
  function adb(operation,args){
    const startedAtMs=now(),result=spawnSync(adbPath,['-s',serial,...args],
      {encoding:'utf8',windowsHide:true,timeout:LIMITS.adbMs,maxBuffer:1024*1024});
    report.adbOperations.push({operation,startedAtMs,returnedAtMs:now(),status:result.status,
      signal:atom(result.signal),error:safeError(result.error)});
    if(result.error||result.status!==0)fail('ADB_OPERATION_FAILED');
    return (result.stdout||'').trim();
  }
  function rows(text){return text.split(/\r?\n/).map(line=>line.trim()).filter(Boolean).map(line=>line.split(/\s+/));}
  const sameForward=row=>row[0]===serial&&row[1]==='tcp:'+report.forward.port&&row[2]==='localabstract:chrome_devtools_remote';
  const sameReverse=row=>row.at(-2)==='tcp:'+reversePort&&row.at(-1)==='tcp:'+reversePort;
  function liveKeyguard(text){
    const lines=text.split(/\r?\n/),heads=[];
    for(let i=0;i<lines.length;i++)if(/^\s*KeyguardServiceDelegate\b/.test(lines[i]))heads.push(i);
    const parse={atMs:now(),blockMatches:heads.length,fieldMatches:{}};report.keyguardParses.push(parse);
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
    const operation={purpose,startedAtMs:now(),status:null,bodyParsed:false};report.httpOperations.push(operation);
    const abort=new AbortController(),timer=setTimeout(()=>abort.abort(),LIMITS.httpMs);let reader;
    try{
      const response=await fetch(endpoint+path,{signal:abort.signal,redirect:'error'});operation.status=response.status;
      if(response.status!==200||!response.body)fail('DEVTOOLS_HTTP_STATUS');
      reader=response.body.getReader();const chunks=[];let bytes=0;
      while(true){const part=await reader.read();if(part.done)break;
        bytes+=part.value.length;if(bytes>LIMITS.httpBytes){abort.abort();fail('DEVTOOLS_HTTP_BODY_LIMIT');}
        chunks.push(part.value);
      }
      const value=JSON.parse(Buffer.concat(chunks,bytes).toString('utf8'));operation.bodyParsed=true;return value;
    }catch(error){operation.error=safeError(error);throw error;}
    finally{clearTimeout(timer);abort.abort();reader?.releaseLock();operation.returnedAtMs=now();}
  }
  function ownedSocketUrl(value){
    if(typeof value!=='string')fail('DEVTOOLS_WEBSOCKET_MISSING');
    const url=new URL(value),base=new URL(endpoint);
    if(url.protocol!=='ws:'||!(url.pathname==='/devtools/browser'||url.pathname.startsWith('/devtools/browser/')))
      fail('DEVTOOLS_WEBSOCKET_KIND_INVALID');
    url.host=base.host;url.username='';url.password='';return url.href;
  }
  function rawConnection(kind,url){
    const state={kind,createdAtMs:now(),opened:false,closed:false,closeRequested:false,
      sent:0,responses:0,events:0,unknownResponses:0,duplicateResponses:0,invalidMetadata:0,
      sessionMismatches:0,dispatchOverflow:0,dispatchPeak:0,dispatchBytesPeak:0};
    const socket=new WebSocket(url,[],{maxPayload:LIMITS.wsPayloadBytes,followRedirects:false,
      perMessageDeflate:{clientNoContextTakeover:true,zlibDeflateOptions:{level:3},
        zlibInflateOptions:{chunkSize:10*1024},threshold:10*1024}});
    const entries=new Map(),queue=new Array(LIMITS.dispatchMessages);
    let head=0,size=0,queuedBytes=0,scheduled=0,nextId=0,pending=0;
    let resolveOpen,rejectOpen,resolveClosed;
    const opened=new Promise((resolve,reject)=>{resolveOpen=resolve;rejectOpen=reject;});
    const closed=new Promise(resolve=>{resolveClosed=resolve;});
    const isOpen=()=>socket.readyState===WebSocket.OPEN;
    function requestClose(){state.closeRequested=true;if(socket.readyState!==WebSocket.CLOSED)socket.close();}
    function transportFault(boundary,error){
      recordingFailed=true;fault(kind+'-'+boundary,error);
      try{requestClose();}catch(second){fault(kind+'-fault-close',second);}
    }
    function issue(method,params={},route='browser-root',sessionId,timeoutMs=commandTimeoutMs,observeResult,cleanup=false){
      const cap=cleanup?LIMITS.commands:LIMITS.commands-LIMITS.cleanupCommandReserve;
      if(report.commands.length>=cap){report.counts.commandOverflow++;recordingFailed=true;fail('COMMAND_LIMIT');}
      if(!cleanup&&report.records.length>=LIMITS.records-1){
        report.counts.recordAttempts++;report.counts.recordOverflow++;recordingFailed=true;fail('RECORD_LIMIT');
      }
      if(pending>=(cleanup?LIMITS.pending:LIMITS.pending-LIMITS.cleanupPendingReserve)){
        report.counts.pendingOverflow++;recordingFailed=true;fail('PENDING_COMMAND_LIMIT');
      }
      if(!methodName(method))fail('AUTHORED_METHOD_INVALID');
      const row={connection:kind,route,id:++nextId,method,issuedAtMs:now(),timeoutMs,
        sendAttempted:false,responsePresent:false,timedOut:false,settled:false,outcomeUnknown:false};
      report.commands.push(row);
      let finish;const promise=new Promise(resolve=>{finish=resolve;});
      const entry={row,sessionId,observeResult,timer:null,finish};entries.set(row.id,entry);pending++;
      function settle(outcome,result){
        if(row.settled)return;
        row.settled=true;row.outcome=outcome;row.settledAtMs=now();clearTimeout(entry.timer);entry.timer=null;pending--;
        row.outcomeUnknown=row.sendAttempted&&
          ['timeout','send-failed','connection-closed','session-mismatch','invalid-result'].includes(outcome);
        if(row.outcomeUnknown&&row.method.startsWith('Input.'))report.inputOutcomeUnknown=true;
        if(outcome!=='response')report.counts.commandFailures++;
        const resolveResult=finish;finish=null;entry.finish=null;entry.settle=null;
        resolveResult({ok:outcome==='response',row,result:outcome==='response'?result:undefined});
      }
      entry.settle=settle;
      if(!isOpen()){settle('socket-not-open');return {row,promise};}
      let encoded;
      try{
        const message={id:row.id,method,params};if(sessionId!==undefined)message.sessionId=sessionId;
        encoded=JSON.stringify(message);if(Buffer.byteLength(encoded)>LIMITS.outboundBytes)fail('OUTBOUND_PAYLOAD_LIMIT');
      }catch(error){row.localError=safeError(error);settle('not-sent-invalid-payload');return {row,promise};}
      row.sendAttempted=true;row.sentAtMs=now();state.sent++;
      append({kind:'command',connection:kind,route,id:row.id,method,atMs:row.sentAtMs});
      entry.timer=setTimeout(()=>{row.timedOut=true;settle('timeout');},timeoutMs);
      try{
        socket.send(encoded,error=>{
          row.sendCallbackAtMs=now();row.sendCallbackOk=!error;
          if(error){row.sendError=safeError(error);settle('send-failed');}
        });
      }catch(error){row.sendError=safeError(error);settle('send-failed');}
      return {row,promise};
    }
    function receive(message,receivedAtMs){
      const atMs=now(),id=Number.isSafeInteger(message?.id)?message.id:null;
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
      row.responseOutcome=outcome;entry.observeResult=null;entry.settle?.(outcome,message.result);
      // No result, error.message or incoming packet is retained in the entry.
    }
    function schedule(){
      while(scheduled<size){
        scheduled++;setImmediate(()=>{
          scheduled--;if(!size)return;
          const item=queue[head];queue[head]=undefined;head=(head+1)%queue.length;size--;queuedBytes-=item.bytes;
          let value;try{value=JSON.parse(item.data.toString());}
          catch(error){transportFault('json-decode',error);return;}
          try{receive(value,item.receivedAtMs);}catch(error){transportFault('message-processing',error);}
        });
      }
    }
    socket.on('message',data=>{
      const bytes=data.length??data.byteLength;
      if(!Number.isSafeInteger(bytes)||size>=queue.length||queuedBytes+bytes>LIMITS.dispatchBytes){
        state.dispatchOverflow++;transportFault('queue-overflow',errorFor('DISPATCH_QUEUE_LIMIT'));return;
      }
      queue[(head+size)%queue.length]={data,bytes,receivedAtMs:now()};size++;queuedBytes+=bytes;
      state.dispatchPeak=Math.max(state.dispatchPeak,size);state.dispatchBytesPeak=Math.max(state.dispatchBytesPeak,queuedBytes);schedule();
    });
    socket.on('open',()=>{state.opened=true;state.openedAtMs=now();resolveOpen();});
    socket.on('error',error=>{state.socketError=safeError(error);rejectOpen(error);transportFault('socket-error',error);});
    socket.on('unexpected-response',(_request,response)=>{
      state.handshakeStatus=response.statusCode??null;response.resume();
      const error=errorFor('WEBSOCKET_HANDSHAKE_STATUS');rejectOpen(error);transportFault('handshake',error);
    });
    socket.on('close',code=>{
      state.closed=true;state.closedAtMs=now();state.closeCode=code;resolveClosed();
      if(!state.opened)rejectOpen(errorFor('WEBSOCKET_CLOSED_BEFORE_OPEN'));
      for(const entry of entries.values())if(!entry.row.settled)entry.settle?.('connection-closed');
    });
    return {kind,state,opened,closed,issue,isOpen,requestClose,terminate:()=>socket.terminate(),
      snapshot:()=>({...state,queued:size,queuedBytes,scheduled,pendingUnsettled:pending,
        noResponse:[...entries.values()].filter(entry=>entry.row.sendAttempted&&!entry.row.responsePresent)
          .map(entry=>({id:entry.row.id,method:entry.row.method,route:entry.row.route,
            outcome:entry.row.outcome??null,outcomeUnknown:entry.row.outcomeUnknown,ageAtSnapshotMs:now()-entry.row.sentAtMs}))})};
  }
  async function connect(kind,url){
    const client=rawConnection(kind,url);clients.push(client);
    await bounded(client.opened,LIMITS.wsOpenMs,'WEBSOCKET_OPEN_TIMEOUT');return client;
  }
  function snapshot(){
    const connections=clients.map(client=>client.snapshot());
    const metadataComplete=!recordingFailed&&report.counts.faultOverflow===0&&
      connections.every(value=>value.dispatchOverflow===0&&value.unknownResponses===0&&
        value.duplicateResponses===0&&value.invalidMetadata===0&&value.sessionMismatches===0);
    return structuredClone({...report,atMs:now(),connections,metadataComplete,
      pendingAtSnapshot:connections.reduce((sum,connection)=>sum+connection.pendingUnsettled,0)});
  }
  function diagnosticError(error,row){
    const safe=errorFor(atom(error?.code)||'OWNED_ANDROID_SESSION_FAILED');
    safe.name=atom(error?.name)||'Error';
    if(row)safe.command={connection:row.connection,id:row.id,method:row.method,
      outcome:row.outcome,sendAttempted:row.sendAttempted,outcomeUnknown:row.outcomeUnknown};
    const data=snapshot();safe.ownedAndroidSession=data;safe.diagnostics=data;return safe;
  }
  async function closeClient(client){
    if(client.state.closed)return;
    try{client.requestClose();}catch(error){fault(client.kind+'-close-call',error,true);}
    try{await bounded(client.closed,LIMITS.closeMs,'WEBSOCKET_CLOSE_TIMEOUT');}
    catch(error){
      fault(client.kind+'-graceful-close',error,true);
      try{client.terminate();client.state.forcedTermination=true;}catch(second){fault(client.kind+'-terminate-call',second,true);}
      try{await bounded(client.closed,LIMITS.terminateMs,'WEBSOCKET_TERMINATE_TIMEOUT');}
      catch(second){fault(client.kind+'-terminate-wait',second,true);}
    }
  }
  async function closeKnownTarget(){
    if(!report.owned.creationAcknowledged||!report.owned.targetId||report.owned.closeAttempted)return;
    report.owned.closeAttempted=true;report.owned.closeStartedAtMs=now();
    try{
      let client=rootClient;
      if(!client?.isOpen()){
        if(cleanupConnectionUsed)fail('CLEANUP_CONNECTION_ALREADY_USED');
        cleanupConnectionUsed=true;client=await connect('browser-cleanup',browserUrl);
      }
      const handle=client.issue('Target.closeTarget',{targetId:report.owned.targetId},'browser-owned-cleanup',undefined,
        Math.min(commandTimeoutMs,LIMITS.cleanupCommandMs),result=>({valid:result.success===true,closeSucceeded:result.success===true}),true);
      report.owned.closeCommand={connection:client.kind,id:handle.row.id};
      const result=await handle.promise;report.owned.closeResponseSucceeded=result.ok;
      if(!result.ok)fault('owned-target-close-response',errorFor('OWNED_CLOSE_NOT_CONFIRMED'),true);
    }catch(error){fault('owned-target-close',error,true);}
  }
  async function verifyOwnedAbsent(){
    if(!report.owned.targetId)return;
    try{
      for(let i=0;i<LIMITS.absenceChecks;i++){
        const list=await readJson('/json/list','owned-absence-'+(i+1));
        if(!Array.isArray(list))fail('DEVTOOLS_TARGET_LIST_INVALID');
        const matches=list.reduce((count,value)=>count+(value?.id===report.owned.targetId?1:0),0);
        report.lookups.push({purpose:'owned-absence',attempt:i+1,atMs:now(),ownedMatches:matches});
        if(matches>1)fail('OWNED_TARGET_ID_AMBIGUOUS');
        report.owned.absentAfterCleanup=matches===0;report.owned.absenceObservedAtMs=now();
        if(matches===0)return;
        if(i+1<LIMITS.absenceChecks)await delay(LIMITS.absenceIntervalMs);
      }
      fault('owned-target-absence',errorFor('OWNED_TARGET_STILL_LISTED'),true);
    }catch(error){report.owned.absentAfterCleanup=null;fault('owned-target-absence',error,true);}
  }
  function cleanupMapping(name,mapping,predicate){
    if(!mapping.attempted){mapping.exactOwnedTupleAbsent=true;return;}
    if(!mapping.acquired){
      mapping.exactOwnedTupleAbsent=null;mapping.ownershipUnknown=true;
      fault(name+'-acquisition',errorFor('MAPPING_OUTCOME_UNKNOWN'),true);return;
    }
    try{
      const current=rows(adb(name+'-list-cleanup',[name,'--list']));
      if(current.some(predicate))adb(name+'-remove-exact',[name,'--remove','tcp:'+mapping.port]);
      else mapping.cleanupSkipped='exact-owned-tuple-already-absent';
      mapping.exactOwnedTupleAbsent=!rows(adb(name+'-list-after',[name,'--list'])).some(predicate);
      if(!mapping.exactOwnedTupleAbsent)fail('OWNED_MAPPING_REMAINS');
    }catch(error){fault(name+'-cleanup',error,true);}
  }
  async function performClose(){
    report.cleanup.started=true;report.cleanup.startedAtMs=now();report.phase='cleanup';stage('cleanup-started');
    try{
      if(report.owned.creationAttempted&&!report.owned.creationAcknowledged&&!createHandle?.row.responsePresent){
        try{await bounded(createAckObserved,LIMITS.createAckGraceMs,'CREATE_ACK_REMAINS_UNKNOWN');}
        catch(error){report.owned.creationAckGraceError=safeError(error);}
      }
      await nextTurn();await nextTurn();await closeKnownTarget();
      for(const client of [...clients])await closeClient(client);
      // A late create ACK may establish only this exact ID during disconnect.
      await nextTurn();await nextTurn();
      if(report.owned.creationAcknowledged&&!report.owned.closeAttempted){
        await closeKnownTarget();for(const client of [...clients])await closeClient(client);
      }
      await verifyOwnedAbsent();
    }catch(error){fault('cleanup-unexpected',error,true);}
    finally{
      for(const client of [...clients])await closeClient(client);
      report.owned.creationOutcomeUnknown=report.owned.creationAttempted&&!report.owned.creationAcknowledged;
      report.owned.cleanupQualified=!report.owned.creationAttempted||
        report.owned.creationAcknowledged&&report.owned.closeAttempted&&report.owned.absentAfterCleanup===true;
      cleanupMapping('reverse',report.reverse,sameReverse);cleanupMapping('forward',report.forward,sameForward);
      await nextTurn();await nextTurn();
      try{report.sourceAfter=sourcePaths.map(identity);}catch(error){fault('source-identity',error,true);}
      report.sourceStable=report.sourceBefore.length>0&&JSON.stringify(report.sourceBefore)===JSON.stringify(report.sourceAfter);
      report.cleanup.qualified=report.owned.cleanupQualified&&clients.every(client=>client.state.closed&&client.snapshot().queued===0)&&
        report.reverse.exactOwnedTupleAbsent===true&&report.forward.exactOwnedTupleAbsent===true;
      report.cleanup.finished=true;report.cleanup.finishedAtMs=now();report.phase='closed';stage('cleanup-finished');
    }
    return snapshot();
  }
  function close(){
    closedForPublic=true;
    if(!closePromise)closePromise=performClose().catch(error=>{
      fault('close-unexpected',error,true);report.cleanup.qualified=false;
      report.cleanup.finished=true;report.cleanup.finishedAtMs=now();report.phase='closed';
      return snapshot();
    });
    return closePromise;
  }
  async function send(method,params={},timeoutMs=commandTimeoutMs){
    report.counts.publicCalls++;let handle;
    try{
      if(closedForPublic||report.phase!=='ready')fail('OWNED_SESSION_NOT_ACTIVE');
      if(!METHODS.has(method))fail('OWNED_METHOD_NOT_ALLOWED');
      if(!params||typeof params!=='object'||Array.isArray(params)||
        ![Object.prototype,null].includes(Object.getPrototypeOf(params)))fail('PARAMS_OBJECT_REQUIRED');
      if(!Number.isSafeInteger(timeoutMs)||timeoutMs<1||timeoutMs>LIMITS.maxCommandMs)fail('COMMAND_TIMEOUT_INVALID');
      if(recordingFailed)fail('DIAGNOSTIC_RECORDING_FAILED');
      if(report.inputOutcomeUnknown&&method.startsWith('Input.'))fail('PRIOR_INPUT_OUTCOME_UNKNOWN');
      handle=rootClient.issue(method,params,'owned-flattened',ownedSession,timeoutMs);
      const result=await handle.promise;
      if(!result.ok)throw errorFor('CDP_COMMAND_'+String(handle.row.outcome).toUpperCase().replace(/-/g,'_'));
      if(recordingFailed)throw errorFor('DIAGNOSTIC_RECORDING_FAILED');
      return result.result;
    }catch(error){
      if(!handle)report.counts.rejectedPublicCalls++;
      fault('public-send',error);
      throw diagnosticError(error,handle?.row);
    }
  }
  try{
    report.sourceBefore=sourcePaths.map(identity);
    if(typeof adbPath!=='string'||!isAbsolute(adbPath)||typeof dependenciesRoot!=='string'||!isAbsolute(dependenciesRoot)||
      typeof serial!=='string'||!/^[A-Za-z0-9_.:-]{1,128}$/.test(serial))fail('EXPLICIT_EXISTING_DEPENDENCIES_REQUIRED');
    if(!Number.isInteger(reversePort)||reversePort<1||reversePort>65535)fail('EXPLICIT_REVERSE_PORT_REQUIRED');
    if(!Number.isSafeInteger(commandTimeoutMs)||commandTimeoutMs<1||commandTimeoutMs>LIMITS.maxCommandMs)fail('COMMAND_TIMEOUT_INVALID');
    adbPath=realpathSync(adbPath);dependenciesRoot=realpathSync(dependenciesRoot);
    if(!statSync(adbPath).isFile()||!statSync(dependenciesRoot).isDirectory())fail('EXISTING_DEPENDENCIES_TYPE_INVALID');
    report.owner={adbPath,deviceId:serial,package:'org.chromium.chrome',dependenciesRoot};
    const dependencies=createRequire(resolve(dependenciesRoot,'package.json'));
    const corePackage=dependencies.resolve('playwright-core/package.json'),coreRequire=createRequire(corePackage),coreDir=dirname(corePackage);
    if(!contains(dependenciesRoot,corePackage))fail('DEPENDENCY_OWNER_MISMATCH');
    const utilsPath=coreRequire.resolve('playwright-core/lib/utilsBundle');
    sourcePaths=[SELF,corePackage,utilsPath,resolve(coreDir,'types/protocol.d.ts')];report.sourceBefore=sourcePaths.map(identity);
    const info=JSON.parse(readFileSync(corePackage,'utf8'));if(info.version!=='1.63.0')fail('UNREVIEWED_BUNDLED_WS_VERSION');
    report.playwrightCoreVersion=info.version;
    const utilities=coreRequire('playwright-core/lib/utilsBundle');
    if(typeof utilities.ws!=='function')fail('BUNDLED_WS_UNAVAILABLE');
    WebSocket=utilities.ws; // No Playwright connection or protocol logger is constructed.
    const beforeForward=rows(adb('forward-list-before',['forward','--list']));
    const beforeReverse=rows(adb('reverse-list-before',['reverse','--list']));
    report.forward.preexistingConflict=beforeForward.some(row=>row[0]===serial&&row[2]==='localabstract:chrome_devtools_remote');
    report.reverse.preexistingConflict=beforeReverse.some(row=>row.at(-2)==='tcp:'+reversePort);
    if(report.forward.preexistingConflict)fail('EXISTING_FORWARD_OWNER');
    if(report.reverse.preexistingConflict)fail('EXISTING_REVERSE_OWNER');
    const packages=adb('package-check',['shell','pm','list','packages','org.chromium.chrome']);
    if(!packages.split(/\r?\n/).includes('package:org.chromium.chrome'))fail('EXPECTED_CHROMIUM_PACKAGE_MISSING');
    const activity=adb('resolve-activity',['shell','cmd','package','resolve-activity','--brief','org.chromium.chrome'])
      .split(/\r?\n/).find(line=>/^org\.chromium\.chrome\/[A-Za-z0-9_.$]+$/.test(line));
    if(!activity)fail('EXPECTED_CHROMIUM_ACTIVITY_MISSING');report.owner.activity=activity;
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
    const resumed=adb('resumed-activity',['shell','dumpsys','activity','activities']).split(/\r?\n/).filter(line=>/\bmResumedActivity\b/.test(line));
    report.foreground={commandReturned:true,resumedLines:resumed.length,
      chromiumResumed:resumed.length>0&&resumed.every(line=>/\borg\.chromium\.chrome\//.test(line))};
    report.deviceReady=!report.keyguardAfter.showing&&!report.keyguardAfter.inputRestricted&&report.power.fieldsUnambiguous&&
      report.power.wakefulness==='Awake'&&report.power.displayStates.includes('ON')&&report.foreground.chromiumResumed;
    if(!report.deviceReady)fail('DEVICE_NOT_CONFIRMED_AWAKE_UNLOCKED_CHROMIUM');
    stage('device-ready');
    report.reverse.attempted=true;report.reverse.outcome='pending';
    try{
      adb('reverse-create',['reverse','--no-rebind','tcp:'+reversePort,'tcp:'+reversePort]);
      report.reverse.acquired=true;report.reverse.outcome='acquired';
    }catch(error){report.reverse.outcome='acquisition-unknown';throw error;}
    report.reverse.verified=rows(adb('reverse-list-verify',['reverse','--list'])).filter(sameReverse).length===1;
    if(!report.reverse.verified)fail('OWNED_REVERSE_VERIFICATION_FAILED');
    report.forward.attempted=true;report.forward.outcome='pending';
    try{
      const output=adb('forward-create',['forward','--no-rebind','tcp:0','localabstract:chrome_devtools_remote']);
      if(!/^[0-9]+$/.test(output))fail('FORWARD_PORT_NOT_REPORTED');
      const port=Number(output);if(!Number.isSafeInteger(port)||port<1||port>65535)fail('FORWARD_PORT_INVALID');
      report.forward.port=port;report.forward.acquired=true;report.forward.outcome='acquired';
    }catch(error){report.forward.outcome='acquisition-unknown';throw error;}
    const afterForward=rows(adb('forward-list-verify',['forward','--list']));
    report.forward.verified=afterForward.filter(sameForward).length===1&&
      afterForward.filter(row=>row[0]===serial&&row[2]==='localabstract:chrome_devtools_remote').length===1;
    if(!report.forward.verified)fail('OWNED_FORWARD_VERIFICATION_FAILED');
    endpoint='http://127.0.0.1:'+report.forward.port;stage('owned-mappings-ready');
    const version=await readJson('/json/version','browser-endpoint-discovery');browserUrl=ownedSocketUrl(version.webSocketDebuggerUrl);
    rootClient=await connect('browser-root',browserUrl);report.phase='opening';
    const versionReply=await rootClient.issue('Browser.getVersion').promise;
    report.browserVersionResponded=versionReply.ok;if(!versionReply.ok)fail('BROWSER_GET_VERSION_FAILED');
    report.browserVersion={product:versionToken(versionReply.result.product),protocolVersion:versionToken(versionReply.result.protocolVersion)};
    createHandle=rootClient.issue('Target.createTarget',{url:'about:blank'},'browser-owned-create',undefined,commandTimeoutMs,result=>{
      const id=opaqueId(result.targetId);if(!id)return {valid:false};
      report.owned.creationAcknowledged=true;report.owned.targetId=id;report.owned.creationAckAtMs=now();
      resolveCreateAck();return {valid:true,ownedIdAccepted:true};
    });
    report.owned.creationAttempted=createHandle.row.sendAttempted;report.owned.createCommand={connection:rootClient.kind,id:createHandle.row.id};
    const created=await createHandle.promise;if(!created.ok||!report.owned.targetId)fail('OWNED_TARGET_CREATION_NOT_CONFIRMED_IN_TIME');
    stage('owned-target-confirmed');
    const activated=await rootClient.issue('Target.activateTarget',{targetId:report.owned.targetId},'browser-owned-activate').promise;
    report.owned.activationResponded=activated.ok;if(!activated.ok)fail('OWNED_TARGET_ACTIVATION_NOT_CONFIRMED');
    const attached=await rootClient.issue('Target.attachToTarget',{targetId:report.owned.targetId,flatten:true},'browser-owned-attach',undefined,commandTimeoutMs,result=>{
      const id=opaqueId(result.sessionId);if(!id)return {valid:false};
      ownedSession=id;report.owned.flattenedSessionSha256=sha(id);return {valid:true,ownedSessionAccepted:true};
    }).promise;
    report.owned.attachResponded=attached.ok;if(!attached.ok||!ownedSession)fail('OWNED_SESSION_ATTACH_NOT_CONFIRMED');
    if(recordingFailed)fail('DIAGNOSTIC_RECORDING_FAILED');
    report.phase='ready';report.readyAtMs=now();stage('owned-session-ready');
    return Object.freeze({send,snapshot,close});
  }catch(error){
    fault('open',error);report.openFailure={atMs:now(),...safeError(error)};
    await close();throw diagnosticError(error);
  }
}
