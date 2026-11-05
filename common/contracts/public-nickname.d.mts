/** Review and rendering use this exact spelling, including Unicode normalization. */
export declare function normalizePublicNickname(value: unknown): string;
export declare const PUBLIC_NICKNAME_STATUSES: readonly ['idle', 'reviewing', 'queued', 'approved', 'rejected', 'cooldown', 'rate-limited', 'unavailable'];
export interface PublicNicknameState {
    name: string;
    status: typeof PUBLIC_NICKNAME_STATUSES[number];
    nextChangeAt: number;
    retryAt: number;
    attemptsLeft: 0 | 1 | null;
    /** Private owner-only draft; never a roster/directory display name. */
    pendingName?: string;
}
export declare function parsePublicNicknameState(value: unknown): PublicNicknameState | null;
