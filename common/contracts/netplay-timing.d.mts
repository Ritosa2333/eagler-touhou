export interface MeasuredNetplayTiming {
    phase: "ready";
    automatic: boolean;
    adonisMode: 1 | 2;
    inputDelay: number;
    fullDelay: number;
    predictionReserve: number;
    rttP95Us: number;
    samples: number;
    lost: number;
    route: "rtc" | "relay" | "spectator";
}
export declare function resolveAdonisPredictionReserve(fullDelay: number, configuredReserve: number, automatic: boolean): number;
export declare function parseMeasuredNetplayTiming(value: unknown): MeasuredNetplayTiming | null;
