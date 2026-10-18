// Offline, privacy-safe summary; no browser, relay, build, system change or benchmark.
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
import assert from 'node:assert/strict';
const [input,output]=process.argv.slice(2);
assert(input&&output,'Usage: node analyze-health-constructor.mjs raw-report.json NEW-evidence.json');
assert(path.resolve(input)!==path.resolve(output));
const sha=b=>createHash('sha256').update(b).digest('hex'),bytes=fs.readFileSync(input),r=JSON.parse(bytes);
const decode=(columns,row)=>Object.fromEntries(columns.map((key,index)=>[key,row[index]]));
const sanitize=value=>String(value).replace(/(?:https?:\/\/|file:\/\/\/)[^\s]+/g,'<redacted-url>');
const peers=r.peers.map(p=>{
  const t=p.rtcConstructorTrace;assert(t?.valid&&t.coverage.exercised,'Constructor coverage required');
  const calls=t.events.filter(e=>e.event==='enter').map(e=>{
    const exits=t.events.filter(x=>x.call===e.call&&x.event!=='enter');assert.equal(exits.length,1);
    const x=exits[0];return {call:e.call,enteredAt:e.at,exitedAt:x.at,exit:x.event,errorName:x.errorName,
      durationMs:x.at-e.at,enteredEpochMs:t.timeOrigin+e.at,exitedEpochMs:t.timeOrigin+x.at};
  });
  const packets=new Map(p.packets.map(row=>{const value=decode(p.columns.packets,row);return [value.id,value];}));
  const channels=new Map(p.channels.map(c=>[c.id,c.kind]));
  const events=p.events.map(row=>decode(p.columns.events,row));
  const expected=r.parameters.frames*2;
  const packetAudit=p.columns.phases.map((phaseName,phase)=>{
    const es=events.filter(e=>e.phase===phase),ps=es.map(e=>packets.get(e.packetId));
    const sequences=ps.map(p=>p.sequence).toSorted((a,b)=>a-b);
    const exactlyOnce=sequences.length===expected&&sequences.every((s,i)=>s===i+1);
    const sender=phase<2?p.side:1-p.side;
    return {phase:phaseName,count:es.length,expected,exactlyOnce,
      minSequence:sequences[0]??null,maxSequence:sequences.at(-1)??null,
      correctSenderAndDestination:ps.every(q=>q.sender===sender&&q.destination===1-sender),
      noThrows:es.every(e=>!e.threw),allApiAccepted:phase===0?es.every(e=>e.result===1):null,
      channelKinds:phase===1||phase===2?Array.from(new Set(es.map(e=>channels.get(e.channelId)??'missing'))):null};
  });
  const selected=p.rtc.flatMap((s,index)=>s.peers.filter(q=>q.pair).map(q=>({
    sampleIndex:index,at:s.at,epochMs:p.clock.started.timeOrigin+s.at,reason:s.reason,route:s.route,peerId:q.peerId,
    pc:q.connectionState,ice:q.iceConnectionState,pair:{state:q.pair.state,nominated:q.pair.nominated},
    localCandidateType:q.local?.candidateType??null,remoteCandidateType:q.remote?.candidateType??null
  })));
  const failPair=s=>s.pair.state!=='succeeded'||!s.localCandidateType||!s.remoteCandidateType||
    s.localCandidateType==='relay'||s.remoteCandidateType==='relay';
  const stateEdges=p.states.filter(s=>s.kind==='transport').map(s=>({at:s.at,route:s.state.route,
    failed:s.state.failed,closed:s.state.closed,disconnected:s.state.disconnected,
    peers:s.state.peers.map(q=>({id:q.peerId,pc:q.connectionState,ice:q.iceConnectionState}))}));
  return {side:p.side,clock:p.clock,plannedScheduleStartEpochMs:p.startEpochMs,
    constructorTrace:t,calls,packetAudit,counts:p.counts,overflow:p.overflow,
    fixtureErrors:p.errors.map(e=>({at:e.at,code:e.code,detail:sanitize(e.detail)})),
    final:{route:p.current.route,failed:p.current.failed,closed:p.current.closed,disconnected:p.current.disconnected,
      queue:p.current.queue,peers:p.current.peers.map(q=>({id:q.peerId,pc:q.connectionState,ice:q.iceConnectionState,
        input:{state:q.input?.readyState,ordered:q.input?.ordered,maxRetransmits:q.input?.maxRetransmits},
        control:{state:q.control?.readyState,ordered:q.control?.ordered,maxRetransmits:q.control?.maxRetransmits}}))},
    stateEdges,allSelectedCandidateSamples:selected,allOriginalStrictPairViolations:selected.filter(failPair),
    selectedCandidateScope:'Candidate type/state only; no addresses, ports, candidate text, SDP or opaque candidate IDs.'};
});
const allCalls=peers.flatMap(p=>p.calls),lo=Math.min(...allCalls.map(c=>c.enteredEpochMs)),hi=Math.max(...allCalls.map(c=>c.exitedEpochMs));
const counts={};const nonResourceFailures=[];
r.errors.forEach((e,index)=>{if(e.startsWith('Unexpected resource request in no-WASM fixture:'))counts.externalResourceRequests=(counts.externalResourceRequests||0)+1;
 else nonResourceFailures.push({index,message:sanitize(e)});});
const result={schema:'th09/health-constructor-evidence/1',
 input:{path:path.resolve(input),bytes:bytes.length,sha256:sha(bytes)},
 analyzer:{path:fileURLToPath(import.meta.url),sha256:sha(fs.readFileSync(fileURLToPath(import.meta.url)))},
 label:r.label,passed:r.passed,parameters:r.parameters,
 sourceIdentity:{commonHead:r.identity.commonGit.head,expectedCommonHead:r.identity.expectedCommonHead,
   files:r.identity.files,after:r.identity.after,rtcConstructorTraceSourceSha256:r.identity.rtcConstructorTraceSourceSha256,
   functionBodies:r.identity.functionBodies},
 browsers:r.browsers.map(b=>({side:b.side,version:b.version,args:b.args,requestedCpuRate:b.requestedCpuRate,
   timeOrigin:b.timeOrigin,visibility:b.visibility,hasFocus:b.hasFocus})),
 failures:{total:r.errors.length,redactedResourceCount:counts.externalResourceRequests||0,allNonResourceFailures:nonResourceFailures},
 peers,
 alignedConstructors:{windowEpochMs:[lo,hi],enterSpreadMs:Math.max(...allCalls.map(c=>c.enteredEpochMs))-lo,
   returnSpreadMs:hi-Math.min(...allCalls.map(c=>c.exitedEpochMs))},
 relay:{valid:r.relayTrace.valid,counts:r.relayTrace.counts,coverage:r.relayTrace.coverage,overflow:r.relayTrace.overflow,
   errors:r.relayTrace.errors,clock:r.relayTrace.clock,sourceIdentity:r.relayTrace.sourceIdentity,
   scope:'Binary E9NP path is expected empty for actual RTC; no relay signaling trace was enabled in this health condition.'},
 hostLoad:r.hostLoad,
 hostDuringConstructor:r.hostLoad?.cpuSamples.filter(s=>s.utcMs>=lo-2000&&s.utcMs<=hi+2000)??[],
 limitations:[
   'The observed synchronous captured NativePC constructor includes any preinstalled browser/page wrapper. No internal browser/GPU/audio/security-service attribution is measured.',
   'The 1440-packet schedule begins after route readiness. Complete scheduled delivery does not mean RTC startup was fast.',
   'All original non-resource failures remain failures; a connected host/host final path does not overwrite an earlier or periodic strict pair-state violation.',
   'External resource requests are counted, with their URLs and queries intentionally omitted. Their presence does not prove they caused the constructor interval.',
   'Host process samples are spaced about five seconds and cover candidate names; they cannot rule out short exited tasks, unlisted processes, single-core saturation or clock changes.',
   'This is local real RTC between two separate headless browser processes, no WASM/game/audio/draw. It is not public-network or production performance acceptance.'
 ]};
fs.writeFileSync(output,JSON.stringify(result,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({path:path.resolve(output),bytes:fs.statSync(output).size,sha256:sha(fs.readFileSync(output)),
 analyzerSha256:result.analyzer.sha256,rawSha256:result.input.sha256,
 constructors:peers.map(p=>({side:p.side,calls:p.calls,packetAudit:p.packetAudit,strictPairViolations:p.allOriginalStrictPairViolations})),
 failures:result.failures}));

