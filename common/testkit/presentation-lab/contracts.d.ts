export type TickStatus = 'advanced' | 'blocked-loading' | 'finished' | 'error';
export interface Capabilities {
  driverApiVersion: 1;
  game: string;
  adapterVersion: string;
  nativeAbi: string;
  features: Record<string, boolean> & {
    freeze: true; resume: true; step: true; drawOnly: true;
    references: true; stateEvidence: true; timing: true; capture: true;
  };
}
export interface FreezeToken { sessionEpoch: number | string; generation: number; }
export interface TickReceipt { status: TickStatus; advancedTicks: 0 | 1; detail?: string; }
export interface ObservationField {
  id: string;
  label?: string;
  value: number | number[];
  tolerance: number;
  period?: number;
  interpolationPolicy: 'continuous' | 'snap-current' | 'curve' | 'unknown';
  evidence?: unknown;
}
export interface ObservationRecord {
  key?: string;
  ownerId: string | number;
  ownerLabel?: string;
  objectId: string | number;
  generation?: string | number;
  identityConfidence: 'proven' | 'uncertain';
  partId?: string | number;
  drawId?: string | number;
  lifecycle?: {continuous: boolean; reason?: string};
  coordinateSpace?: string;
  bounds?: number[];
  geometryQuality?: string;
  fields: ObservationField[];
}
export interface ObservationFrame {
  sessionEpoch: string | number;
  simulationTick: number;
  referenceDrawSerial?: number;
  alpha?: number;
  completeness: 'complete' | 'incomplete';
  dropped: number;
  records: ObservationRecord[];
  limitations?: string[];
}
export interface StateEvidence {
  coverageVersion: string;
  groups: Array<{id: string; digest: string | number; includedFields?: string[]}>;
  missingGroups: string[];
}
export interface RuntimeDriverV1 {
  describe(): Capabilities;
  freeze(): FreezeToken;
  resume(token: FreezeToken): void;
  advanceOneTick(input?: unknown): TickReceipt;
  drawOnly(request: {alpha: number; world: boolean}): {status: 'drawn'};
  setInput(input: {code: string; down: boolean}): void;
  clearInput(): void;
  close(): void | Promise<void>;
}
export interface ObservationAdapterV1 {
  observationApiVersion: 1;
  scanSchema: string;
  sessionSchema: string;
  enable(on: boolean): void;
  readReferences(): {previous: ObservationFrame; current: ObservationFrame};
  readObservation(): ObservationFrame;
  readStateEvidence(): StateEvidence;
  readTiming(): unknown;
  readTrace(): unknown;
  readScene(): unknown;
  worldFrozen(): boolean;
  readGate(): boolean;
  setNegativeControl(on: boolean): void;
  captureImage(frame: unknown, alpha: number): unknown;
  analyze(input: unknown): unknown;
  compact(report: unknown, options?: unknown): unknown;
  mergeIssues(reports: unknown[]): unknown;
}
