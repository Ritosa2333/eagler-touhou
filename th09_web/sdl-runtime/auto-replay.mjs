// Local archive sidecars keep mutable player names out of the native payload.
export function parseReplayMatch(value) {
  if (!value || value.schema !== 'eagler-touhou/th09-match-replay/1' ||
      !Array.isArray(value.names) || value.names.length !== 2 ||
      value.names.some(name => typeof name !== 'string' || name.length > 64) ||
      !Number.isSafeInteger(value.startedAt) || value.startedAt < 0 ||
      !Number.isSafeInteger(value.frames) || value.frames < 30 ||
      ![0, 1].includes(value.winner)) return null;
  return {schema: value.schema, names: value.names.map((name, side) => name || `P${side + 1}`),
    startedAt: value.startedAt, frames: value.frames, winner: value.winner};
}
export function createAutoReplay({core, saveRoot, options, enabled, sync, notice, now = Date.now}) {
  let match = null, lastEnd = -1;
  const sidecar = path => path.replace(/\.rpyx?$/, '.json');
  const readMatch = path => {
    if (!/\.rpyx?$/.test(path)) return null;
    try {return parseReplayMatch(JSON.parse(new TextDecoder().decode(core.FS.readFile(sidecar(path)))));}
    catch {return null;}
  };
  const writeMatch = (path, value) => {
    if (!/\.rpyx?$/.test(path)) return;
    const parsed = parseReplayMatch(value);
    if (parsed) core.FS.writeFile(sidecar(path), new TextEncoder().encode(JSON.stringify(parsed)));
  };
  return {
    readMatch, writeMatch,
    removeMatch(path) {if (!/\.rpyx?$/.test(path)) return;try {core.FS.unlink(sidecar(path));} catch {}},
    begin(frame) {
      const names = options().netplayReplayNames;
      match = {frame, startedAt: now(), names: [0, 1].map(side => String(names?.[side] || `P${side + 1}`).slice(0, 64))};
      lastEnd = -1;
    },
    end(frame, winner) {
      if (!enabled() || options().netplaySpectator || options().netplayReplayAutoSave === false ||
          !match || frame <= match.frame || frame === lastEnd || ![0, 1].includes(winner)) return;
      lastEnd = frame;
      try {
        const folder = match.names.join(' vs ').replace(/[<>:"/\\|?*\u0000-\u001f\u007f]/g, '_').slice(0, 128).replace(/[. ]+$/g, '') || 'P1 vs P2';
        const directory = `${saveRoot}/replay/${folder}`;
        const encoded = new TextEncoder().encode(folder);
        if (encoded.length >= 512) throw Error('录像目录名过长');
        const pointer = core._th09_auto_replay_folder_buffer();
        core.HEAPU8.set(encoded, pointer); core.HEAPU8[pointer + encoded.length] = 0;
        core.FS.mkdirTree(directory);
        const occupied = new Set(core.FS.readdir(directory).map(name => name.toLowerCase()));
        let slot = 0;
        for (; slot <= 0xffff; slot++) {
          const base = `th9_ud${slot.toString(16).padStart(4, '0')}`;
          if (![base + '.rpy', base + '.rpyx', base + '.json'].some(name => occupied.has(name))) break;
        }
        if (slot > 0xffff) throw Error('自动保存 Replay 失败：录像存储已满，请下载并整理旧录像');
        if (!core._th09_auto_save_replay(slot)) throw Error('自动保存 Replay 失败，请在游戏内手动保存');
        const path = `${directory}/th9_ud${slot.toString(16).padStart(4, '0')}.rpy`;
        writeMatch(path, {schema: 'eagler-touhou/th09-match-replay/1', names: match.names,
          startedAt: match.startedAt, frames: frame - match.frame, winner});
        const label = match.names.join(' vs ');
        // Capture synchronously before another match can replace the recording.
        // A flush failure must not terminate the player's connection.
        void sync().then(() => notice('Replay 已自动保存：' + label),
          () => notice('Replay 尚未写入浏览器存储，请下载备份后再退出'));
      } catch (error) {notice(error.message || String(error));}
    },
  };
}
