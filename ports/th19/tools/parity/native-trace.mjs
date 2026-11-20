import {createReadStream} from "node:fs";
import {createHash} from "node:crypto";
import {StringDecoder} from "node:string_decoder";

// Preserve every parsed Present row while releasing source buffers as we read.
// A terminated observer may leave one incomplete, non-newline-terminated row.
export async function readNativeTrace(path) {
  const rows = [], hash = createHash("sha256"), decoder = new StringDecoder("utf8");
  let pending = "", incompleteTail = false;
  for await (const bytes of createReadStream(path)) {
    hash.update(bytes);
    pending += decoder.write(bytes);
    let start = 0, end;
    while ((end = pending.indexOf("\n", start)) >= 0) {
      rows.push(JSON.parse(pending.slice(start, end)));
      start = end + 1;
    }
    pending = pending.slice(start);
  }
  pending += decoder.end();
  if (pending.trim()) {
    try { rows.push(JSON.parse(pending)); }
    catch { incompleteTail = true; }
  }
  return {rows, sha256: hash.digest("hex"), incompleteTail};
}
