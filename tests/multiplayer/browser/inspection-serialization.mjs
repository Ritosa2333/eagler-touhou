// A timed-out operation may still finish later: keep ownership until the real
// promise settles, rather than releasing it when its caller stops waiting.
export function createInspectionGuard() {
  const running=new Set();
  return async function guarded(key, operation) {
    if(running.has(key)) throw Error(`Concurrent inspection of ${key}`);
    running.add(key);
    try {return await operation();} finally {running.delete(key);}
  };
}
export async function settleInspections(tasks) {
  const results=await Promise.allSettled(tasks.map(task=>Promise.resolve().then(task)));
  const errors=results.flatMap((result,index)=>result.status==='rejected'?[new Error(`seat ${index}: ${result.reason}`,{cause:result.reason})]:[]);
  if(errors.length)throw new AggregateError(errors,'One or more inspections failed');
  return results.map(result=>result.value);
}
