import createFFmpegCore from '@ffmpeg/core';

let core: ReturnType<typeof createFFmpegCore> | undefined;

self.onmessage = async (event: MessageEvent<{input: Blob; pcm: boolean; wasmUrl: string}>) => {
  try {
    core ??= (async () => {
      const response = await fetch(event.data.wasmUrl);
      if (!response.ok) throw Error('Local audio encoder failed to load');
      return createFFmpegCore({wasmBinary: new Uint8Array(await response.arrayBuffer())});
    })();
    const ffmpeg = await core;
    const input = event.data.pcm ? 'input.pcm' : 'input.wav';
    const output = 'output.ogg';
    ffmpeg.FS.writeFile(input, new Uint8Array(await event.data.input.arrayBuffer()));
    try {
      const format = event.data.pcm ? ['-f', 's16le', '-ar', '44100', '-ac', '2'] : [];
      const status = ffmpeg.exec('-loglevel', 'error', ...format, '-i', input, '-c:a', 'libvorbis', '-q:a', '5', output);
      if (status !== 0) throw Error(`Local OGG conversion failed (${status})`);
      const bytes = ffmpeg.FS.readFile(output);
      if (!bytes.length) throw Error('Local OGG conversion produced no audio');
      self.postMessage({blob: new Blob([bytes], {type: 'audio/ogg'})});
    } finally {
      ffmpeg.reset();
      for (const file of [input, output]) {
        try {ffmpeg.FS.unlink(file);} catch { /* Output may not exist after an encoding failure. */ }
      }
    }
  } catch (error) {
    self.postMessage({error: error instanceof Error ? error.message : String(error)});
  }
};
