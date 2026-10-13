interface LinkedAbortSignal {signal: AbortSignal | undefined; dispose(): void;}

export function linkAbortSignals(first: AbortSignal, ...rest: (AbortSignal | undefined)[]): LinkedAbortSignal & {signal: AbortSignal};
export function linkAbortSignals(...signals: (AbortSignal | undefined)[]): LinkedAbortSignal;
/** Operation-owned cancellation fan-in, without AbortSignal.any or a global shim. */
export function linkAbortSignals(...signals: (AbortSignal | undefined)[]): LinkedAbortSignal {
  const sources = [...new Set(signals.filter((signal): signal is AbortSignal => signal !== undefined))];
  if (sources.length < 2) return {signal: sources[0], dispose() {}};
  const controller = new AbortController(), listeners = new Map<AbortSignal, () => void>();
  let disposed = false;
  function dispose() {
    if (disposed) return;
    disposed = true;
    for (const [source, listener] of listeners) source.removeEventListener('abort', listener);
    listeners.clear();
  }
  const aborted = sources.find(source => source.aborted);
  if (aborted) {controller.abort(aborted.reason); return {signal: controller.signal, dispose};}
  for (const source of sources) {
    if (disposed) break;
    const listener = () => {if (!disposed) {controller.abort(source.reason); dispose();}};
    listeners.set(source, listener);source.addEventListener('abort', listener, {once: true});
    if (source.aborted) listener();
  }
  return {signal: controller.signal, dispose};
}
