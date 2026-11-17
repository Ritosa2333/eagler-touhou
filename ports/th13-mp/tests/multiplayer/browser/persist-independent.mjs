// Evidence streams have independent ownership. One failed stream must not
// prevent saving the other streams from that same observed callback batch.
export async function persistIndependent(streams) {
  const errors=[];
  for (const [name,persist] of streams) {
    try { await persist(); }
    catch(error) { errors.push(new Error(`${name}: ${error}`, {cause:error})); }
  }
  if(errors.length) throw new AggregateError(errors,'Evidence persistence failed: '+errors.map(String).join('; '));
}

// A partial append or a lost ack has unknown outcome. Do not replay the same
// stateful stream; other streams are still independently flushed.
export async function terminalStream(cursor, label, persist) {
  if(cursor.failure) throw Error(cursor.failure);
  try {return await persist();}
  catch(error) {cursor.failure=`${label} persistence/acknowledgement failed: ${error}`;throw error;}
}
