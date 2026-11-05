/** Review and rendering use this exact spelling, including Unicode normalization. */
export function normalizePublicNickname(value) {
    if (typeof value !== 'string' || value.length > 256)
        return '';
    return [...value.normalize('NFKC').replace(/[\p{Cc}\p{Cf}]/gu, '').replace(/\s+/gu, ' ').trim()].slice(0, 12).join('');
}
export const PUBLIC_NICKNAME_STATUSES = ['idle', 'reviewing', 'queued', 'approved', 'rejected', 'cooldown', 'rate-limited', 'unavailable'];
export function parsePublicNicknameState(value) {
    if (!value || typeof value !== 'object')
        return null;
    const source = value;
    if (!PUBLIC_NICKNAME_STATUSES.includes(source.status) || typeof source.name !== 'string')
        return null;
    const time = (value) => typeof value === 'number' && Number.isSafeInteger(value) && value >= 0 ? value : 0;
    return { name: normalizePublicNickname(source.name), status: source.status,
        nextChangeAt: time(source.nextChangeAt), retryAt: time(source.retryAt), attemptsLeft: source.attemptsLeft === 0 || source.attemptsLeft === 1 ? source.attemptsLeft : null,
        ...(source.status === 'queued' && typeof source.pendingName === 'string' ? { pendingName: normalizePublicNickname(source.pendingName) } : {}) };
}
