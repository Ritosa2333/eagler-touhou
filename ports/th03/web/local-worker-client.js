// The existing TH03 controls and pause menu stay on the page. Native memory,
// input commands and DOSBox-X execution belong to the dedicated worker.
export function workerLocalPause(emulator) {
  let focus = 0, pending = null;
  return {
    get selecting() { return emulator.snapshot.selecting === true; },
    get pending() { return pending || emulator.snapshot.nativePending || null; },
    update: () => emulator.snapshot.state,
    markers: () => emulator.snapshot.markers || [],
    setFocus(value) { focus = value; },
    setTouch(touch) { emulator.setInput({generation:emulator.snapshot.state?.generation, focus, touch}); },
    reset() {
      focus = 0; pending = null; emulator.nativeResults.length = 0;
      emulator.snapshot = {state:null, markers:[]}; emulator.clearInput(); emulator.send('nativeReset');
    },
    request(command, seat) {
      const state = emulator.snapshot.state;
      if (!state || state.phase !== 1 || this.pending || ![1,2].includes(command) || ![0,1].includes(seat)) return false;
      pending = {generation:state.generation};
      emulator.call('nativeRequest', [command, seat, state.generation]).then(accepted => {
        if (!accepted) { pending = null; emulator.nativeResults.push('rejected'); }
      }).catch(error => emulator.fail(error));
      return true;
    },
    result() {
      const result = emulator.nativeResults.shift();
      if (result) { pending = null; return result; }
      return this.pending ? 'pending' : null;
    },
  };
}
