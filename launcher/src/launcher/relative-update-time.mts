const SECOND = 1_000;
const MINUTE = 60 * SECOND;
const HOUR = 60 * MINUTE;
const DAY = 24 * HOUR;

export const APP_SHELL_UPDATE_STATUS_PATH = "./__app-shell-update-status__";

export function appliedAppShellUpdateAt(status: unknown): number | null {
  if (!status || typeof status !== "object" || (status as { updated?: unknown }).updated !== true) return null;
  const value = Number((status as { appliedAt?: unknown }).appliedAt);
  return Number.isFinite(value) && value > 0 ? value : null;
}

export function formatRelativeUpdateAge(elapsedMs: number): string {
  const elapsed = Math.max(0, Number.isFinite(elapsedMs) ? elapsedMs : 0);
  if (elapsed < MINUTE) return `${Math.floor(elapsed / SECOND)}s`;
  if (elapsed < HOUR) return `${Math.floor(elapsed / MINUTE)}min`;
  if (elapsed < DAY) return `${Math.floor(elapsed / HOUR)}h`;
  return `${Math.floor(elapsed / DAY)}d`;
}

export function nextRelativeUpdateRefresh(elapsedMs: number): number {
  const elapsed = Math.max(0, Number.isFinite(elapsedMs) ? elapsedMs : 0);
  const unit = elapsed < MINUTE ? SECOND : elapsed < HOUR ? MINUTE : elapsed < DAY ? HOUR : DAY;
  return Math.max(50, unit - (elapsed % unit));
}
