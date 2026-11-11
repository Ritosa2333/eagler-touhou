import {mountManagedPractice} from './pc98-practice.mjs';
// Appended to the imported solo module by the managed build. All operations
// enter the existing attempt/input/Replay paths rather than another game loop.
const managedReplayWrites = new Set();
const managedSavedReplays = new WeakMap();
async function managedSaveReplay(blob) {
  if (managedSavedReplays.has(blob)) return managedSavedReplays.get(blob);
  const pending = (async () => {
    const path = 'replay/th02_' + Date.now() + '_' + crypto.randomUUID().slice(0,8) + '.th02r';
    await globalThis.__eaglerPC98Files.write(path, await blob.arrayBuffer());
    return path;
  })();
  managedSavedReplays.set(blob, pending); managedReplayWrites.add(pending);
  try { return await pending; }
  catch (error) { managedSavedReplays.delete(blob); globalThis.__eaglerPC98Emit({event:'error',message:'录像保存失败：'+error.message}); }
  finally { managedReplayWrites.delete(pending); }
}
async function managedNaturalExit() {
  const deadline = performance.now() + 10000;
  while (recorder?.state.ending && !recorder.state.ready && !recorder.state.failed && performance.now() < deadline) await new Promise(resolve => setTimeout(resolve,50));
  await Promise.allSettled([...managedReplayWrites]);
  try { await managedAPI.files({command:'sync'}); }
  finally { globalThis.__eaglerPC98Emit({event:'exit',code:0}); }
}
export const managedAPI = {
  practiceState:()=>({...managedPractice.state(),retry:readRetryState()}),
  async start({language, music:musicMode, options}) {
    $('language').value = language === 'ja' ? 'jp' : 'cn';
    $('audio-mode').value = 'independent';
    volume.querySelector('input').value = musicMode === 'none' ? '0' : '70';
    if (options.replayPath) {
      const {userPath}=await import('./managed-storage.mjs');
      const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(options.replayPath));
      if(!bytes)throw Error('录像不存在');
      loadedReplay=await decodeExternalReplay(new Blob([bytes]),'02');
    }
    await startGame(options.replayPath?loadedReplay:null);
    if (!emulator) throw Error($('status').textContent);
    if (musicMode === 'none') music?.player.setVolume(0);
    managedPractice.start({readOnly:!!options.replayPath});
  },
  health() {
    return {frames:emulator?.module.renderMetrics?.drawn ?? emulator?.module.framePresenter?.info().drawn ?? 0, audio:{state:emulator?.context?.state ?? emulator?.module.SDL2?.audioContext?.state}};
  },
  async files(message) {
    const {userPath} = await import('./managed-storage.mjs'), store = globalThis.__eaglerPC98Files;
    const sync = async () => {
      if (!emulator) return;
      const {Fat12} = await import('./solo-fat12.js');
      const fat = new Fat12(await emulator.getDiskImage('th02-solo.hdi'));
      for (const name of ['HUUHI.DAT','HUUMA.CFG']) {
        const entry = fat.find('HUMAROKU/' + name, {optional:true});
        if (entry !== undefined) await store.write(name, fat.read(entry));
      }
    };
    switch (message.command) {
    case 'sync': await sync(); return {};
    case 'list': await sync(); return {files:[...store.memory].map(([path,bytes]) => ({path,size:bytes.byteLength}))};
    case 'read': { await sync(); const value = store.memory.get(userPath(message.path)); if (!value) throw Error('文件不存在'); return {bytes:Array.from(value)}; }
    case 'write': {
      if (emulator) throw Error('请退出游戏后导入文件');
      const path=userPath(message.path);
      if (!Array.isArray(message.bytes) || message.bytes.some(byte=>!Number.isInteger(byte)||byte<0||byte>255)) throw Error('用户文件字节无效');
      const bytes=new Uint8Array(message.bytes);
      if (path.endsWith('.th02r')) await decodeExternalReplay(new Blob([bytes]), '02');
      await store.write(path,bytes); return {};
    }
    case 'remove': if (emulator) throw Error('请退出游戏后删除文件'); await store.remove(message.path); return {};
    default: throw Error('用户文件操作无效');
    }
  },
  async playback(message) {
    if (message.action !== 'open' || emulator) throw Error('请从启动器打开录像');
    const {userPath} = await import('./managed-storage.mjs');
    const bytes = globalThis.__eaglerPC98Files.memory.get(userPath(message.path));
    if (!bytes) throw Error('录像不存在');
    loadedReplay = await decodeExternalReplay(new Blob([bytes]), '02'); await startGame(loadedReplay); return {};
  },
  stop() { managedPractice.stop();release(); discardRetry(); activeAttempt?.stop(); recorder?.dispose(); music?.dispose(); emulator?.dispose(); },
};

const managedPractice=mountManagedPractice(practiceUI,globalThis.__eaglerPC98Input,{release,readState:()=>({...readSoloState(),practice:emulator?.isWorker?emulator.snapshot.practice:emulator?.practice?.status})});

$('record-save').onclick=async()=>{if(!recordBlob)return;const path=await managedSaveReplay(recordBlob);if(path)$('record-save').textContent='已存入主项目录像';};
