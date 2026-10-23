const requireValue=(value,message)=>{if(!value)throw Error(message);};
const sameEvent=(a,b)=>a&&b&&['owner','object','generation','tick','operation'].every(key=>a[key]===b[key]);
const samePhysical=(a,b)=>a?.owner===b?.owner&&a?.object===b?.object;

// This predicate follows full native-window validation. It requires one real
// adjacent reference pair; it never joins frames from different ticks/epochs.
export function assertPoolReuseBoundary(capture,contract,label='pool reuse'){
  requireValue(capture?.event?.retired&&capture.event.born&&capture.report,`${label}: missing actual boundary report`);
  const window=contract.readNativeWindow(capture.report,{label}),{retired,born}=capture.event;
  requireValue(capture.report.valid&&capture.report.purityStatus==='pass'&&capture.report.lifecycle?.status==='complete',`${label}: invalid native boundary`);
  requireValue(samePhysical(retired,born)&&retired.operation===2&&born.operation===1&&
    Number.isSafeInteger(retired.generation)&&born.generation===retired.generation+1&&contract.lifecycleOwnerIds.has(born.owner),
  `${label}: not consecutive generations of the same registered physical owner`);
  const events=window.lifecycle.events,ri=events.findIndex(event=>sameEvent(event,retired)),bi=events.findIndex(event=>sameEvent(event,born));
  const from=window.previous.native.lifecycle.count,to=window.current.native.lifecycle.count;
  requireValue(ri>=from&&bi>ri&&bi<to,`${label}: retirement and birth are not in this adjacent reference interval`);
  requireValue(!events.slice(ri+1,bi).some(event=>samePhysical(event,born)),`${label}: an intervening generation or event was omitted`);
  const records=(frame,event)=>frame.records.filter(record=>record.meta[0]===event.owner&&record.meta[1]===event.object&&record.meta[12]===event.generation);
  const old=records(window.previous,retired),current=records(window.current,born);
  requireValue(old.length>0&&current.length>0&&records(window.previous,born).length===0&&records(window.current,retired).length===0,
    `${label}: old disappearance and new birth are not present in their real references`);
  for(const sample of window.samples){
    requireValue(records(sample,retired).length===0,`${label}: retired generation was submitted again`);
    const fresh=records(sample,born),byKey=new Map(fresh.map(record=>[contract.keyOf(record),record]));
    requireValue(fresh.length===current.length&&current.every(record=>contract.newbornSubmissionMatches(record,byKey.get(contract.keyOf(record)))),
      `${label}: newborn submission did not snap all measured fields and geometry to the current generation`);
  }
  return {schema:'eagler-presentation/pool-boundary/1',status:'passed',complete:false,machineComplete:true,
    previousTick:window.previous.tick,tick:window.current.tick,retireIndex:ri,birthIndex:bi,
    oldSubmissions:old.length,newSubmissions:current.length,samples:window.samples.length,event:{retired,born}};
}
