const failures=new Set(['missing-interpolation','downstream-held','nonfinite','non-idempotent','endpoint-mismatch',
  'endpoint-mismatch-current','endpoint-mismatch-previous','ambiguous','unscoped','identity-uncertain']);
const requireValue=(value,message)=>{if(!value)throw Error(message);};

// A title supplies its actual ABI decoder and declared fields. This gate reads
// those frames again; no saved summary or human verdict can complete it.
export function assertContinuousVisualEvidence(row,contract,label=row?.label||'visual window'){
  requireValue(typeof contract?.readNativeWindow==='function',`${label}: missing native visual contract`);
  const window=contract.readNativeWindow(row,{label}),report=window.recomputed;
  requireValue(report.valid&&report.purityStatus==='pass'&&report.lifecycle?.status==='complete',`${label}: native window, authority purity or lifecycle is incomplete`);
  const previous=new Map(window.previous.records.map(record=>[contract.keyOf(record),record]));
  const samples=window.samples.map(frame=>new Map(frame.records.map(record=>[contract.keyOf(record),record])));
  let measuredFields=0,changingFields=0,boundaryObjects=0,frozenObjects=0;
  for(const object of report.objects){
    requireValue(!failures.has(object.status),`${label}: ${object.key} has ${object.status}`);
    if(!contract.worldOwnerIds.has(object.ownerId))continue;
    const current=window.current.records.find(record=>contract.keyOf(record)===object.key),prior=previous.get(object.key);
    if(!prior||prior.meta[5]!==current.meta[5]||prior.meta[6]!==current.meta[6]||(current.meta[8]|0)<(prior.meta[8]|0)){
      boundaryObjects++;continue;
    }
    if(object.status==='offscreen')continue;
    requireValue(samples.every(sample=>sample.has(object.key)),`${label}: measured world object ${object.key} is missing an alpha submission`);
    if(report.worldFrozen){
      requireValue(samples.every(sample=>contract.newbornSubmissionMatches(current,sample.get(object.key))),
        `${label}: paused world object ${object.key} changed during a draw-only sweep`);
      frozenObjects++;continue;
    }
    for(const field of object.fields){
      requireValue(!failures.has(field.status),`${label}: ${object.key}/${field.id} has ${field.status}`);
      if(field.authoredContinuous===true){measuredFields++;if(field.motion>field.tolerance*2)changingFields++;}
    }
  }
  return {schema:'eagler-presentation/continuous-fields/1',status:'passed',machineComplete:true,complete:false,
    measuredFields,changingFields,frozenObjects,boundaryObjects,
    boundaryChecksRequired:boundaryObjects>0,humanReview:'pending'};
}
