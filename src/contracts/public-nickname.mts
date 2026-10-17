/** Review and rendering use this exact spelling, including Unicode normalization. */
export function normalizePublicNickname(value: unknown): string {
  if (typeof value !== 'string' || value.length > 256) return '';
  return [...value.normalize('NFKC').replace(/[\p{Cc}\p{Cf}]/gu, '').replace(/\s+/gu, ' ').trim()].slice(0, 12).join('');
}

export const PUBLIC_NICKNAME_STATUSES = ['idle', 'reviewing', 'queued', 'approved', 'rejected', 'cooldown', 'rate-limited', 'unavailable'] as const;
export interface PublicNicknameState {
  name: string;
  status: typeof PUBLIC_NICKNAME_STATUSES[number];
  nextChangeAt: number;
  retryAt: number;
  attemptsLeft: 0 | 1 | null;
  /** Private owner-only draft; never a roster/directory display name. */
  pendingName?: string;
}
export function parsePublicNicknameState(value: unknown): PublicNicknameState | null {
  if (!value || typeof value !== 'object') return null;
  const source = value as Record<string, unknown>;
  if (!PUBLIC_NICKNAME_STATUSES.includes(source.status as PublicNicknameState['status']) || typeof source.name !== 'string') return null;
  const time = (value: unknown) => typeof value === 'number' && Number.isSafeInteger(value) && value >= 0 ? value : 0;
  return {name: normalizePublicNickname(source.name), status: source.status as PublicNicknameState['status'],
    nextChangeAt: time(source.nextChangeAt), retryAt: time(source.retryAt), attemptsLeft: source.attemptsLeft === 0 || source.attemptsLeft === 1 ? source.attemptsLeft : null,
    ...(source.status === 'queued' && typeof source.pendingName === 'string' ? {pendingName:normalizePublicNickname(source.pendingName)} : {})};
}
