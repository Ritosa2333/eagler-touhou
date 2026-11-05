// Compact, reproducible evidence from the full observed packet-chain analysis.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const [rawPath,fullPath,output]=process.argv.slice(2);
assert.ok(rawPath&&fullPath&&output,'Pass raw report, full analysis, and a new compact JSON path');
const hash=b=>createHash('sha256').update(b).digest('hex');
const rawBytes=readFileSync(rawPath),fullBytes=readFileSync(fullPath),raw=JSON.parse(rawBytes),full=JSON.parse(fullBytes);
assert.equal(hash(rawBytes),full.source.sha256,'Raw report changed');
assert.equal(hash(readFileSync(full.analyzer.path)),full.analyzer.sha256,'Full analyzer changed');
const base=full.commonTimeOrigin,peers=raw.results;
const stop=side=>peers[side].networkTransport.clock.stopped.timeOrigin-base+peers[side].networkTransport.clock.stopped.after;
function group(rows,key){
 const result={};for(const row of rows){const reason=key(row);const out=result[reason]??={count:0,first:null,last:null,nonemptyInput:0};
  ++out.count;if(row.header.inputCount>0)++out.nonemptyInput;
  const entry={ref:row.ref,at:row.at,sender:row.header.sender,destination:row.header.destination,sequence:row.header.sequence,inputCount:row.header.inputCount};
  if(!out.first||entry.at<out.first.at)out.first=entry;if(!out.last||entry.at>out.last.at)out.last=entry;
 }return result;
}
const missingWs=full.chains.filter(c=>!c.ws).map(c=>({ref:c.receive.ref,at:c.receive.at,header:c.header}));
const missingMessage=full.chains.flatMap(c=>c.forwards.filter(f=>f.send&&!f.message).map(f=>({ref:f.send.ref,at:f.send.at,header:c.header})));
const missingSafe=full.chains.flatMap(c=>c.forwards.filter(f=>f.copy&&!f.safeFrontier).map(f=>({ref:f.copy.ref,at:f.copy.at,header:c.header})));
assert.equal(missingWs.length,full.pairings.ws_to_relay_receive.unmatchedRight.length);
assert.equal(missingMessage.length,full.pairings.relay_send_to_message.unmatchedLeft.length);
const selected=new Set(full.episode?.coveringChains||[]);
const selectedChains=full.chains.filter(c=>selected.has(c.receive.ref));
const earliestMissingSend=selectedChains.reduce((at,c)=>Math.min(at,c.ws?.at??Infinity),Infinity);
const summary={version:1,
 rawInput:{path:resolve(rawPath),sha256:hash(rawBytes),identity:raw.identity,parameters:raw.parameters,passed:raw.passed},
 fullAnalysis:{sha256:hash(fullBytes),bytes:fullBytes.length,analyzer:full.analyzer,chainCount:full.chains.length},
 compiler:{path:fileURLToPath(import.meta.url),sha256:hash(readFileSync(fileURLToPath(import.meta.url)))},
 commonTimeOrigin:base,scope:full.scope,limitations:full.limitations,valid:full.valid,clocks:full.clocks,
 coverage:Object.fromEntries(Object.entries(full.pairings).map(([name,p])=>[name,{
  leftCount:p.leftCount,rightCount:p.rightCount,matched:p.matched,leftDuplicateOccurrences:p.leftDuplicateOccurrences,
  rightDuplicateOccurrences:p.rightDuplicateOccurrences,unmatchedLeft:p.unmatchedLeft.length,unmatchedRight:p.unmatchedRight.length
 }])),
 unmatchedReasons:{
  relayReceivesWithoutObservedSenderSend:group(missingWs,c=>c.at>stop(c.header.sender)?'after_sender_observer_stop':'inside_sender_observation_unexplained'),
  relaySendsWithoutObservedPeerMessage:group(missingMessage,c=>c.at>stop(c.header.destination)?'after_receiver_observer_stop':'inside_receiver_observation_unexplained'),
  positiveCopiesWithoutFollowingSafeFrontier:group(missingSafe,()=> 'no_following_safe_observation_before_trace_stop'),
  otherUnmatchedBrowserEvents:full.unmatchedBrowserEvents.map(e=>({ref:e.ref,side:e.side,at:e.at,phase:e.raw.phase})),
  failedOrPeekCopies:full.failedOrPeekCopies.length,relayPending:full.relayPending.length},
 stageSummary:full.stageSummary,relayCounts:full.relayCounts,relayCoverage:full.relayCoverage,
 displayGaps:full.displayGaps,allRafPendingOver250ms:full.allRafPendingOver250ms,allRafCallsOver250ms:full.allRafCallsOver250ms,
 activeEpisode:full.episode,selectedPacketChains:selectedChains,
 conclusions:{
  observed:{owner:full.episode?.side,pendingRafMs:full.episode?.request.pendingMs,ownerGameCallbacksInsideGap:full.episode?.peers[full.episode.side].gameCallbacks,
   ownerPumpObservationsInsideGap:full.episode?.peers[full.episode.side].pacing.filter(row=>row[1]===0).length},
  transport:{missingSourceFrame:full.episode?.missingSourceFrame,firstCoveringWsAt:Number.isFinite(earliestMissingSend)?earliestMissingSend:null,
   firstCoveringWsAfterRafResumes:Number.isFinite(earliestMissingSend)&&full.episode?earliestMissingSend>=full.episode.end:null},
  audio:{zeroPeakBlockIndicesBySide:full.episode?.peers.map(p=>({side:p.side,indices:p.audio.approximateBlocksInWindow.filter(b=>b.row[4]<=1e-6).map(b=>b.index)})),
   limitation:'Audio wall-clock window alignment is inferred because the original probe omitted its origin; sampled block peaks and relative callback gaps are observed directly.'},
  causeBoundary:'The report does not identify which browser/compositor or injected task caused RAF dispatch to wait. Host load and unrelated security instrumentation are not proof of that cause.',
  startup:'Every >250ms RAF request delay remains listed, including pre-active startup intervals. Selecting the active episode does not delete startup observations.'}
};
const bytes=Buffer.from(JSON.stringify(summary,null,2));assert.ok(bytes.length<500000,'Compact evidence exceeded 500 kB');
writeFileSync(output,bytes,{flag:'wx'});
process.stdout.write(JSON.stringify({output:resolve(output),bytes:bytes.length,sha256:hash(bytes),compiler:summary.compiler,unmatchedReasons:summary.unmatchedReasons},null,2));

