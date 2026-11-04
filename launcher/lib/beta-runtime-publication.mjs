import {readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {installRuntimeStorageChannel} from './runtime-storage-channel.mjs';

const begin = '<!-- BEGIN GENERATED RUNTIME STORAGE CHANNEL -->';
const end = '<!-- END GENERATED RUNTIME STORAGE CHANNEL -->';

/** Applied after verifying the input release, before freezing its publication. */
export function betaRuntimeHtml(html) {
  const block = `${begin}\n<script>\n(${installRuntimeStorageChannel.toString()})(globalThis);\n</script>\n${end}`;
  if (html.includes(begin)) {
    const start = html.indexOf(begin), finish = html.indexOf(end, start);
    if (finish < 0) throw Error('Incomplete Runtime storage channel');
    return html.slice(0, start) + block + html.slice(finish + end.length);
  }
  if (!/<head(?:\s[^>]*)?>/i.test(html)) throw Error('Runtime is missing an HTML head');
  return html.replace(/<head(?:\s[^>]*)?>/i, head => head + '\n' + block);
}

export async function prepareBetaRuntimePublication(directory, entry, files = null) {
  if (files && !files[entry]) throw Error('Beta Runtime entry is not declared');
  const path = resolve(directory, entry);
  const html = betaRuntimeHtml(await readFile(path, 'utf8'));
  await writeFile(path, html);
  const bytes = Buffer.from(html);
  return {...files, [entry]: {bytes: bytes.length, sha256: createHash('sha256').update(bytes).digest('hex')}};
}
