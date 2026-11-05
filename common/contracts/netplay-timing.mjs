// Configured reserve is a ceiling. Auto retains one queued input frame.
export function resolveAdonisPredictionReserve(fullDelay, configuredReserve, automatic) {
    return Math.min(2, configuredReserve, Math.max(0, fullDelay - (automatic ? 1 : 0)));
}
export function parseMeasuredNetplayTiming(value) {
    if (!value || typeof value !== "object" || Array.isArray(value))
        return null;
    const v = value;
    const integer = (key, min, max) => typeof v[key] === "number" && Number.isInteger(v[key]) && v[key] >= min && v[key] <= max;
    if (v.phase !== "ready" || typeof v.automatic !== "boolean" || (v.adonisMode !== 1 && v.adonisMode !== 2) ||
        !integer("inputDelay", 0, 9) || !integer("fullDelay", 1, 31) || !integer("predictionReserve", 0, 2) ||
        !integer("rttP95Us", 1, 1_000_000) || !integer("samples", 96, 120) || !integer("lost", 0, 72) ||
        !["rtc", "relay", "spectator"].includes(String(v.route)))
        return null;
    const fullDelay = v.fullDelay, predictionReserve = v.predictionReserve;
    const reserveBudget = fullDelay - (v.automatic ? 1 : 0);
    if (fullDelay !== Math.max(1, Math.ceil(Math.floor(v.rttP95Us / 2) * 60 / 1_000_000)) ||
        predictionReserve > reserveBudget || (v.adonisMode === 1 ? predictionReserve !== 0 : reserveBudget === 0 ? predictionReserve !== 0 : predictionReserve < 1) ||
        (v.automatic && v.inputDelay !== Math.max(0, fullDelay - predictionReserve)))
        return null;
    return { phase: "ready", automatic: v.automatic, adonisMode: v.adonisMode, inputDelay: v.inputDelay,
        fullDelay, predictionReserve, rttP95Us: v.rttP95Us, samples: v.samples,
        lost: v.lost, route: v.route };
}
