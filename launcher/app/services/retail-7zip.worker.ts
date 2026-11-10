import SevenZip from '7z-wasm';
import {selectRetailArchiveEntries} from './retail-archive-entries';

interface ArchiveEntry {
  path: string;
  size: number;
  encrypted: boolean;
  directory: boolean;
  link: boolean;
}

function parseListing(listing: string): ArchiveEntry[] {
  return listing.split(/\r?\n\r?\n/).flatMap(block => {
    const fields = new Map(block.split(/\r?\n/).filter(line => line.includes(' = ')).map(line => {
      const split = line.indexOf(' = ');
      return [line.slice(0, split), line.slice(split + 3)];
    }));
    const path = fields.get('Path');
    if (!path) return [];
    const attributes = fields.get('Attributes') ?? '';
    return [{
      path, size: Number(fields.get('Size')), encrypted: fields.get('Encrypted') === '+',
      directory: fields.get('Folder') === '+' || attributes.startsWith('D'),
      link: fields.has('Symbolic Link') || fields.has('Hard Link') || /(?:^|\s)l[rwx-]{9}/.test(attributes),
    }];
  });
}

// 7z-wasm throws a numeric C++ exception when it cannot decode a protected
// header. The signature header points to the encoded header's coder metadata.
async function hasEncrypted7zHeader(blob: Blob) {
  const start = new DataView(await blob.slice(0, 32).arrayBuffer());
  if (start.byteLength < 32) return false;
  const offset = 32 + Number(start.getBigUint64(12, true));
  const size = Number(start.getBigUint64(20, true));
  if (!Number.isSafeInteger(offset) || !Number.isSafeInteger(size) || offset + size > blob.size) return false;
  const header = new Uint8Array(await blob.slice(offset, offset + Math.min(size, 65536)).arrayBuffer());
  return header[0] === 0x17 && header.some((byte, i) =>
    byte === 0x24 && header[i + 1] === 0x06 && header[i + 2] === 0xf1 && header[i + 3] === 0x07 && header[i + 4] === 0x01);
}

const isPasswordError = (message: string) => /wrong password|password is incorrect|password required/i.test(message);

self.onmessage = async ({data}: MessageEvent<{
  input: Blob; kind: 'zip' | '7z'; limits: Record<string, number>; password: string; wasmUrl: string;
}>) => {
  try {
    let out = '', errors = '';
    const zip = await SevenZip({
      locateFile: () => data.wasmUrl,
      print: line => {
        if (out.length + line.length > 8 * 1024 * 1024) throw Error('Archive directory is too large');
        out += line + '\n';
      },
      printErr: line => {if (errors.length < 16384) errors += line + '\n';},
      stdin: () => -1,
    });
    const run = (args: string[]) => {
      out = errors = '';
      let status: number | void;
      try {
        status = (zip.callMain as (args: string[]) => number | void)(args);
      } catch (error) {
        if (typeof error !== 'object' || error === null || !('status' in error) || typeof error.status !== 'number') throw error;
        status = error.status;
      }
      return {status, out, errors};
    };
    const archive = 'archive.' + data.kind;
    const path = '/input/' + archive;
    zip.FS.mkdir('/input');
    zip.FS.mkdir('/output');
    zip.FS.mount(zip.WORKERFS, {blobs: [{name: archive, data: data.input}]}, '/input');

    const password = '-p' + (data.password || '-');
    const listing = run(['l', '-slt', '-ba', '-bd', '-sccUTF-8', password, path]);
    if (listing.status || listing.errors) throw Error(listing.errors.trim() || 'Unable to list the game archive');
    const candidates = selectRetailArchiveEntries(parseListing(listing.out), data.limits, entry => ({
      path: entry.directory ? '' : entry.path.replaceAll('\\', '/'),
      size: entry.size,
    }));
    // biome-ignore lint/suspicious/noControlCharactersInRegex: The 7-Zip include list uses a newline to delimit each path.
    if (candidates.some(({entry}) => entry.link || /[\x00-\x1f\x7f]/.test(entry.path))) throw Error('Unsafe game archive entry');
    if (!data.password && candidates.some(({entry}) => entry.encrypted)) throw Error('Password required');
    const files: {path: string; blob: Blob}[] = [];
    if (candidates.length) {
      if (new Set(candidates.map(({path: entryPath}) => entryPath)).size !== candidates.length) throw Error('Duplicate game archive entry');
      zip.FS.writeFile('/entries.txt', candidates.map(({entry}) => entry.path).join('\n') + '\n');
      const result = run(['x', '-y', '-bd', '-spd', '-sccUTF-8', '-scsUTF-8', password, '-o/output', '-i@/entries.txt', path]);
      if (result.status || result.errors) throw Error(result.errors.trim() || 'Unable to extract the game archive');
      for (const candidate of candidates) {
        const target = '/output/' + candidate.path;
        const stat = zip.FS.lstat(target);
        if (!zip.FS.isFile(stat.mode) || stat.size !== candidate.size) throw Error('Game archive entry size mismatch');
        files.push({path: candidate.path, blob: new Blob([zip.FS.readFile(target) as Uint8Array<ArrayBuffer>])});
        zip.FS.unlink(target);
      }
    }
    self.postMessage({files});
  } catch (error) {
    const message = error instanceof Error ? error.message : 'Unable to open the game archive';
    const passwordRequired = isPasswordError(message) ||
      (typeof error === 'number' && data.kind === '7z' && await hasEncrypted7zHeader(data.input));
    self.postMessage({passwordRequired, error: passwordRequired ? 'This archive requires a valid password' : message});
  }
};
