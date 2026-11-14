import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {appendFile, writeFile} from 'node:fs/promises';

// Never acknowledge a failed or partly written batch. A failed persistence is
// terminal for this collector, so the final flush cannot overwrite evidence.
export async function persistFramebufferCapture(dir, seat, capture, cursor, evidence) {
  if (cursor.failure) throw Error(cursor.failure);
  try {
    if (capture.candidate && !cursor.candidateWritten) {
      const candidate=capture.candidate;
      if(candidate.rgbaBase64) await writeFile(`${dir}/framebuffer-unmatched-${seat}.rgba`,Buffer.from(candidate.rgbaBase64,'base64'),{flag:'wx'});
      await writeFile(`${dir}/framebuffer-unmatched-${seat}.json`,JSON.stringify({...candidate,rgbaBase64:undefined},null,2)+'\n',{flag:'wx'});
      cursor.candidateWritten=true;
    }
    const attempts=(capture.attempts || []).slice(cursor.attemptsWritten || 0);
    if (attempts.length) {
      await appendFile(`${dir}/framebuffer-attempts-${seat}.ndjson`,attempts.map(row=>JSON.stringify(row)+'\n').join(''));
      cursor.attemptsWritten=(cursor.attemptsWritten || 0)+attempts.length;
    }
    const fresh = capture.captures.slice(cursor.rawPending);
    for (const item of fresh) {
      const pixels = Buffer.from(item.rgbaBase64, 'base64');
      const metadata = {...item, rgbaBase64: undefined, bytes: pixels.length,
        sha256: createHash('sha256').update(pixels).digest('hex'), format: 'RGBA8 bottom-left origin'};
      const base = `${dir}/framebuffer-${seat}-${item.sequence}`;
      ++cursor.rawPending;
      await writeFile(base + '.rgba', pixels, {flag: 'wx'});
      await writeFile(base + '.json', JSON.stringify(metadata, null, 2) + '\n', {flag: 'wx'});
    }
    if (capture.failures.length)
      await appendFile(`${dir}/framebuffer-errors-${seat}.ndjson`, JSON.stringify({failures:capture.failures}) + '\n');
    assert.deepEqual(capture.failures, [], 'Framebuffer capture errors');
    const accepted = [];
    for (const item of capture.captures) {
      assert.equal(item.sequence, cursor.next + accepted.length, 'Framebuffer sequence');
      const pixels = Buffer.from(item.rgbaBase64, 'base64');
      assert.equal(pixels.length, item.width * item.height * 4, 'Complete RGBA buffer');
      assert(pixels.some(byte => byte !== 0), 'Framebuffer is entirely zero');
      accepted.push({...item, rgbaBase64: undefined, bytes:pixels.length,
        sha256:createHash('sha256').update(pixels).digest('hex'), format:'RGBA8 bottom-left origin'});
    }
    return accepted;
  } catch (error) {
    cursor.failure = `Framebuffer persistence/validation failed: ${error}`;
    throw error;
  }
}

export function finishFramebufferAck(cursor, evidence, accepted) {
  cursor.next += accepted.length;
  cursor.rawPending -= accepted.length;
  evidence.push(...accepted);
}
