export type ResultStatus = 'PASS' | 'DIVERGED' | 'INCOMPLETE' | 'INCOMPATIBLE' | 'ERROR';
export type CanonicalValue = null | boolean | string | number | CanonicalValue[] | {[key: string]: CanonicalValue};
export interface ComparisonIdentity {
  game: string; profile: string; replaySha256: string; executableSha256: string;
  resourceSha256: string; stateSchema: string; traceCodec: string; digestAlgorithm: string;
}
export interface RunStart {
  type: 'run-start'; schema: 'eagler/replay-trace/v1'; comparisonIdentity: ComparisonIdentity;
  coverage: {requiredCategories: string[]; optionalCategories: string[]};
  provenance: {provider: string; [key: string]: CanonicalValue};
}
export interface SegmentStart {type: 'segment-start'; sequence: number; segmentId: string; [key: string]: CanonicalValue;}
export interface TickRecord {
  type: 'tick'; sequence: number; segmentId: string; logicalTick: number; replaySampleIndex: number;
  clockDisposition: string; appliedInput: CanonicalValue; clocks: {[key: string]: CanonicalValue};
  scalars: {[key: string]: CanonicalValue}; categories: {[key: string]: string};
}
export interface SegmentEnd {type: 'segment-end'; sequence: number; segmentId: string; logicalTick: number; reason: string; complete: boolean;}
export interface RunEnd {type: 'run-end'; sequence: number; reason: string; complete: boolean; summary?: CanonicalValue;}
export type TraceRecord = RunStart | SegmentStart | TickRecord | SegmentEnd | RunEnd;
export interface ReplayVerifierAdapter {
  describe(): {adapterApiVersion: 1; game: string; adapterVersion: string; features: {original?: boolean; [key: string]: boolean};};
  inspectReplay(input: Uint8Array, options?: {signal?: AbortSignal}): unknown | Promise<unknown>;
  openCandidate(spec: unknown, options?: {signal?: AbortSignal}): Promise<TraceSession>;
  openOriginal?(spec: unknown, options?: {signal?: AbortSignal}): Promise<TraceSession>;
}
export interface TraceSession {records: AsyncIterable<TraceRecord>; cancel(reason?: unknown): void; close(): void | Promise<void>;}
