// Main-thread and Worker adapters use the same ordered proof vocabulary.
// A bounded host proof per step accompanies the existing full DOS hash every 120 steps.
export function battleProof(emulator,pause,controls,music,pauseSeat,selection,hidden){
  return JSON.stringify([emulator.module.netInfo().tick,pauseSeat,selection,[...hidden].sort(),pause.capture(),controls.capture(),music.capture()]);
}
