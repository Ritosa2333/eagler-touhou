import {createExtractorFromData, UnrarError} from 'node-unrar-js/esm/index.esm.js';
import {selectRetailArchiveEntries} from './retail-archive-entries';

self.onmessage = async ({data}: MessageEvent<{input: Blob; limits: Record<string, number>; password: string; wasmUrl: string}>) => {
  try {
    const response = await fetch(data.wasmUrl);
    if (!response.ok) throw Error('Local RAR extractor failed to load');
    const extractor = await createExtractorFromData({
      wasmBinary: await response.arrayBuffer(),
      data: await data.input.arrayBuffer(),
      password: data.password,
    });
    const {arcHeader, fileHeaders} = extractor.getFileList();
    if (arcHeader.flags.volume) throw Error('Multi-volume RAR archives are not supported');
    const candidates = selectRetailArchiveEntries(fileHeaders, data.limits, file => ({
      path: file.flags.directory ? '' : file.name.replaceAll('\\', '/'),
      size: file.unpSize,
    }));
    const selected = new Map(candidates.map(candidate => [candidate.entry.name, candidate]));
    const files: {path: string; blob: Blob}[] = [];
    if (selected.size) {
      for (const {fileHeader, extraction} of extractor.extract({files: [...selected.keys()]}).files) {
        const candidate = selected.get(fileHeader.name);
        if (!candidate || !extraction || extraction.byteLength !== candidate.size) throw Error('RAR entry size mismatch');
        files.push({path: candidate.path, blob: new Blob([extraction as Uint8Array<ArrayBuffer>])});
      }
      if (files.length !== selected.size) throw Error('RAR archive is missing an expected game file');
    }
    self.postMessage({files});
  } catch (error) {
    const passwordError = error instanceof UnrarError && (error.reason === 'ERAR_MISSING_PASSWORD' || error.reason === 'ERAR_BAD_PASSWORD');
    self.postMessage({passwordRequired: passwordError, error: passwordError ? 'This RAR requires a valid password' : error instanceof Error ? error.message : String(error)});
  }
};
