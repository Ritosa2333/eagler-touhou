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
