// Compatibility entry point for existing th08 Replay callers. New adapters
// should import their format parser from their own title module.
export {
  assessTh08ReplayInputConsumption,
  deriveTh08ReplayInputExpectation,
  inspectTh08ReplayFixture,
  replayTerminalPolicy,
} from './titles/th08-replay-format.mjs';
