import {PlaybackClock} from './playback-clock.mjs';
import {decodeTh09MatchArchive, TH09_MATCH_ARCHIVE_COMPRESSED_LIMIT, TH09_MATCH_ARCHIVE_LIMIT} from './th09-match-archive.mjs';
import {SharedNetplay} from './shared-netplay.mjs';

export async function readMatchArchive(source) {
  if (!source?.body) throw Error('录像文件缺失');
  const compressed = source.body.getReader();
  let downloaded = 0;
  const bounded = new ReadableStream({
    async pull(controller) {
      const {done, value} = await compressed.read();
      if (done) return controller.close();
      downloaded += value.byteLength;
      if (downloaded > TH09_MATCH_ARCHIVE_COMPRESSED_LIMIT) {
        controller.error(Error('录像压缩文件过大'));
        await compressed.cancel();
        return;
      }
      controller.enqueue(value);
    },
    cancel(reason) {return compressed.cancel(reason);},
  });
  const reader = bounded.pipeThrough(new DecompressionStream('gzip')).getReader();
  let size = 0;
  const pieces = [];
  try {
    for (;;) {
      const {done, value} = await reader.read();
      if (done) break;
      size += value.byteLength;
      if (size > TH09_MATCH_ARCHIVE_LIMIT) throw Error('录像内容过大');
      pieces.push(value);
    }
  } finally {await reader.cancel().catch(() => {});}
  const bytes = new Uint8Array(size);
  let offset = 0;
  for (const piece of pieces) {bytes.set(piece, offset);offset += piece.byteLength;}
  return decodeTh09MatchArchive(bytes);
}

export function createMatchArchivePlayer({core, archive, path, onState, onError}) {
  const clock = new PlaybackClock();
  const spectator = new SharedNetplay(core, {});
  let packets, queueEnded, raf, closed = false, lastReport = -Infinity;
  const frame = () => core._th09_spectator_frame();
  const position = () => frame() - archive.start;
  const total = archive.finish - archive.start;
  const ended = () => position() >= total;
  const state = () => ({
    kind:'replay', path, frame:position(), total, start:0, paused:clock.paused, seeking:clock.target !== null,
    target:clock.target, rate:clock.rate, ended:ended(), difficulty:archive.difficulty,
    left:archive.characters[0], right:archive.characters[1], stage:9, canRewind:true, roundStarts:archive.rounds,
  });
  const report = () => onState(state());
  function step(render) {
    // The native spectator lane keeps a 90-frame safety lead. For a stored
    // file we can always supply that lead, then explicitly close at EOF.
    while (!queueEnded && spectator.spectatorFrame <= frame() + 90) {
      const {done, value} = packets.next();
      if (done) {
        spectator.finishSpectatorMatch(archive.finish);
        queueEnded = true;
      } else if (value.length !== 28) spectator.feedSpectatorFrame(value);
    }
    const before = frame();
    return core._th09_game_tick(render) === 1 && frame() === before + 1;
  }
  function restart() {
    spectator.restartSpectatorMatch(archive.begin, false, true);
    packets = archive.packets();
    queueEnded = false;
    clock.reset();
    if (!step(1)) throw Error('录像首帧无法播放');
  }
  function draw(time) {
    if (closed) return;
    try {
      if (document.hidden) clock.last = null;
      else {
        const seeking = clock.target !== null;
        core._th09_replay_view_silent(+seeking);
        clock.tick(time, {position, ended, step:(index, last) => step(+(index === 0 || last || (seeking && position() + 1 >= clock.target)))});
        core._th09_replay_view_silent(+(clock.target !== null));
        core._th09_audio_pump();
        core._th09_suspend(+(clock.paused || clock.target !== null || ended()));
        if (time - lastReport >= 100) {lastReport = time;report();}
      }
      raf = requestAnimationFrame(draw);
    } catch (error) {clock.pause(true);report();onError(error);}
  }
  core._th09_loop_stop();
  core._th09_loop_pause(0);
  if (!core._th09_spectator_prepare()) throw Error('录像运行环境初始化失败');
  try {restart();} catch (error) {core._th09_spectator_end();throw error;}
  report();
  raf = requestAnimationFrame(draw);
  return {
    active: true,
    state,
    control(action, value) {
      if (action === 'pause') clock.pause(true);
      else if (action === 'play') {if (ended()) restart();clock.pause(false);}
      else if (action === 'rate') clock.speed(value);
      else if (action === 'seek') {
        if (!Number.isSafeInteger(value) || value < 0 || value > total) throw Error('录像位置无效');
        if (value < position() || ended()) restart();
        clock.seek(value);
      } else throw Error('不支持的播放操作');
      report();
      return state();
    },
    close() {
      closed = true;
      cancelAnimationFrame(raf);
      core._th09_replay_view_silent(0);
      core._th09_spectator_end();
    },
  };
}
