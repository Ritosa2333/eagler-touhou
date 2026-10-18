import {appendFileSync, mkdirSync, writeFileSync} from 'node:fs';
import {dirname} from 'node:path';
import {validateRecord} from './contracts.mjs';
import {digestCanonicalJson} from './adapter.mjs';

// Append synchronously so a failed provider retains its completed prefix.
// Only the title provider may decide what constitutes a completed transaction.
export class TraceCapture {
  constructor(path, {identity, provider, categories, provenance = {}}) {
    this.path = path; this.sequence = 0; this.ticks = 0; this.segments = 0;
    this.stage = null; this.logicalTick = 0; this.finished = false;
    mkdirSync(dirname(path), {recursive: true}); writeFileSync(path, '');
    this.write({type: 'run-start', schema: 'eagler/replay-trace/v1',
      comparisonIdentity: identity,
      coverage: {requiredCategories: categories, optionalCategories: []},
      provenance: {...provenance, provider}});
  }
  write(record) { validateRecord(record); appendFileSync(this.path, JSON.stringify(record) + '\n'); }
  closeSegment(complete, reason) {
    if (this.stage === null) return;
    this.write({type: 'segment-end', sequence: this.sequence++, segmentId: this.segmentId,
      logicalTick: this.logicalTick, complete, reason});
  }
  tick({stage, replaySampleIndex, appliedInput, clocks, scalars, categories}) {
    if (this.finished) throw Error('Cannot append to a completed capture');
    if (stage !== this.stage) {
      this.closeSegment(true, 'stage-transition'); this.stage = stage;
      this.segmentId = `stage-${stage}#${this.segments++}`; this.logicalTick = 0;
      this.write({type: 'segment-start', sequence: this.sequence++, segmentId: this.segmentId, route: `stage-${stage}`});
    }
    this.write({type: 'tick', sequence: this.sequence++, segmentId: this.segmentId,
      logicalTick: this.logicalTick++, replaySampleIndex, clockDisposition: 'advanced',
      appliedInput, clocks, scalars,
      categories: Object.fromEntries(Object.entries(categories).map(([key, value]) => [key, digestCanonicalJson(value)]))});
    this.ticks++;
  }
  finish({complete, reason, evidence}) {
    if (this.finished) throw Error('Capture already finished');
    if (!this.ticks) complete = false;
    this.closeSegment(complete, reason);
    this.write({type: 'run-end', sequence: this.sequence++, complete, reason,
      summary: {segments: this.segments, ticks: this.ticks}, evidence});
    this.finished = true;
  }
}
