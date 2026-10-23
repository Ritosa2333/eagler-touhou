const TRAVERSAL_PROBE_PATH='../outside-offline-acceptance.dat';
const TRAVERSAL_PROBE_ERRORS=new Set([
  'Error: Invalid save path',
  `Error: Invalid save path: ${TRAVERSAL_PROBE_PATH}`,
]);

/** Match only the three exact storage traversal probes used by the offline lane. */
export function matchExpectedOfflinePathTraversal(event,expectation){
  if(!event||typeof event!=='object'||!expectation||typeof expectation!=='object')return null;
  if(!['read','write','remove'].includes(expectation.command)||expectation.path!==TRAVERSAL_PROBE_PATH||
    typeof expectation.requestId!=='string'||!expectation.requestId||typeof expectation.game!=='string'||
    !Number.isSafeInteger(expectation.epoch)||expectation.epoch<1)return null;
  if(event.protocol!=='eagler-touhou/1'||event.game!==expectation.game||event.epoch!==expectation.epoch||
    event.request!==expectation.requestId||event.ok!==false||!TRAVERSAL_PROBE_ERRORS.has(event.error))return null;
  return{kind:'expected-storage-path-traversal-rejection',requestId:expectation.requestId,
    game:expectation.game,epoch:expectation.epoch,command:expectation.command,path:expectation.path,
    outcome:'rejected',error:event.error};
}

/** Classify browser errors from the offline Launcher/Runtime page without hiding
 * failures based on asset suffixes, URL paths, or assumed optional origins.
 */
export function classifyOfflineBrowserEvents(events,{origin}={}){
  if(!Array.isArray(events))throw new TypeError('Offline browser events must be an array');
  let siteOrigin;
  try{siteOrigin=new URL(origin).origin;}
  catch{throw new TypeError('Offline site origin must be a valid URL');}

  const blockingEvents=[],sameOriginTransportEvents=[],crossOriginTransportEvents=[];
  for(const event of events){
    if(event?.type==='offline-console'||event?.type==='offline-pageerror'){
      blockingEvents.push(event);
      continue;
    }
    if(event?.type==='offline-requestfailed'||event?.type==='offline-http'){
      let eventOrigin;
      try{eventOrigin=new URL(event.url).origin;}
      catch{
        blockingEvents.push(event);
        continue;
      }
      blockingEvents.push(event);
      if(eventOrigin===siteOrigin)sameOriginTransportEvents.push(event);
      else crossOriginTransportEvents.push(event);
      continue;
    }
    // Fail closed if the event recorder adds a new offline event type.
    blockingEvents.push(event);
  }
  return{blockingEvents,sameOriginTransportEvents,crossOriginTransportEvents};
}
