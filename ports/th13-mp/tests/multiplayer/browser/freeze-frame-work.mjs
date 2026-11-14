// Serialized into the page. Stopping and reading the cumulative work boundary
// are one JS task, so all previously completed callback samples remain queued.
export function freezeFrameWork() {
  const w=document.querySelector('#gameFrame')?.contentWindow, m=w?.Module;
  if (!m?._th13_loop_stop || !w?.__frameWork || m.onGameWorkFrame!==w.__frameWork.observer)
    throw Error('Cannot freeze complete work observation');
  m._th13_loop_stop();
  return {...w.__frameWork.capture,frozenAt:performance.now(),
    unmatchedCandidate:!!w.__framebufferCapture?.candidate};
}
