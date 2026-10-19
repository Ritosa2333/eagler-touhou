/** Persist each browser startup transition before and after it runs. */
export function createStageRecorder(record, save, {now=()=>new Date(), monotonic=()=>performance.now()}={}) {
  record.stages ??= [];
  return async (stage, details, operation) => {
    const entry={stage,...details,status:'running',startedAt:now().toISOString()};
    record.stages.push(entry);
    await save();
    const started=monotonic();
    try {
      const result=await operation();
      entry.status='passed';
      entry.finishedAt=now().toISOString();
      entry.durationMs=Math.round(monotonic()-started);
      await save();
      return result;
    } catch(error) {
      entry.status='failed';
      entry.finishedAt=now().toISOString();
      entry.durationMs=Math.round(monotonic()-started);
      entry.error=String(error.stack||error);
      await save();
      throw error;
    }
  };
}
