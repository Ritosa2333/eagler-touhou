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
  readReferences(): {previous: unknown; current: unknown};
  readObservation(): unknown;
  readStateEvidence(): unknown;
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
