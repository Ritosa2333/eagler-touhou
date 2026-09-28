export interface Th08TimingRecommendation {
  inputDelay: number;
  predictionLimit: number;
  networkFrames: number;
  mobileSeats: number;
}

// RTT is deliberately used as a conservative whole-frame arrival budget:
// browser scheduling and peer pacing can consume more than RTT / 2.
export function recommendTh08InputTiming(
  mobileSeats: number,
  rttMs: number | null,
  jitterMs: number | null,
  sustainableMobileRollback = 2,
): Th08TimingRecommendation {
  const phones = Math.max(0, Math.trunc(mobileSeats));
  if (!phones) return { inputDelay: 0, predictionLimit: 8, networkFrames: 0, mobileSeats: 0 };
  const robust = Math.max(1, Math.min(4, Math.trunc(sustainableMobileRollback)));
  const rtt = Number.isFinite(rttMs) ? Math.max(0, rttMs!) : 100;
  const jitter = Number.isFinite(jitterMs) ? Math.max(0, jitterMs!) : 10;
  const networkFrames = Math.max(1, Math.min(8, Math.ceil((rtt + 2 * jitter) * 60 / 1000)));
  const predictionLimit = phones >= 2 ? robust : Math.min(8, robust * 2);
  return {
    inputDelay: Math.max(0, Math.min(8, networkFrames - predictionLimit)),
    predictionLimit,
    networkFrames,
    mobileSeats: phones,
  };
}
