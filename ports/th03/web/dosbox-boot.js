// Boot with no user input. Work yields regularly so room heartbeats, loading
// progress and browser input continue even on slow devices.
export async function bootToSelection({emulator, pause, music, progress, clearInput}) {
  let ready = false, lastProgress = -Infinity;
  const events = [];
  emulator.module.warmingBoot = true;
  emulator.run();
  try {
    for (let count = 0; count < 10800 && !ready;) {
      const started = performance.now();
      do {
        emulator.step(); count++;
        const now = emulator.tick * 1000 / 60;
        // Disk/driver setup has no live game mailbox. Scan only once OP runs,
        // at 250 ms intervals, then keep advancing candidate heartbeats.
        const module=emulator.module;
        if(count%15===0&&module.UTF8ToString(module._emGetRunningProgram()).toUpperCase()==='OP'){
          const heap=module.HEAPU8;
          events.push(...music.read(heap,heap.length));
          let range=null;
          if(music.valid(heap,music.at)){
            const segment=new DataView(heap.buffer).getUint16(music.at+16,true);
            const base=music.at-music.patch.mailbox_com_offset-segment*16;
            range={start:base,end:base+0xa0000};
          }
          pause.update(now,range);
          ready = pause.selecting && music.valid(module.HEAPU8, music.at);
        }
      } while (!ready && count < 10800 && count % 15 !== 0 && performance.now() - started < 25);
      if (performance.now() - lastProgress >= 500 || ready) {
        progress(`正在启动原游戏：${(emulator.tick / 60).toFixed(1)} 秒`);
        lastProgress = performance.now();
      }
      await new Promise(resolve => setTimeout(resolve, 0));
    }
    if (!ready) throw Error('梦时空启动超过 180 模拟秒仍未进入选人画面');
    return events;
  } finally {
    emulator.pause(); clearInput?.(); emulator.module.warmingBoot = false;
  }
}
