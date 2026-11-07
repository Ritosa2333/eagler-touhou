import {sha256Hex} from '../../src/launcher/sha256.mts';
import type {DecisionComparison} from '../models/decisions';
import type {Translate} from '../i18n';
import {previewCloudSave,type CloudSaveSnapshot,type SaveStatistics} from './cloud-saves';
import {linkAbortSignals,waitWithAbort} from './abort-signals';

export type SaveRelation = 'same' | 'browser-ahead' | 'cloud-ahead' | 'diverged' | 'unknown';
export type SaveBaseline = {revision: number; fingerprint: string; verifiedBase?: boolean};

export const manifestFingerprint = (files: readonly {name: string; sha256: string}[]) =>
  files.map(file => file.name + ':' + file.sha256).sort().join('|');

export async function localSaveFingerprint(files: readonly File[]) {
  return manifestFingerprint(await Promise.all(files.map(async file => ({
    name: file.name,
    sha256: await sha256Hex(new Uint8Array(await file.arrayBuffer())),
  }))));
}

export function relateSaveVersions(
  browser: string, cloud: CloudSaveSnapshot | null, revision: number, base?: SaveBaseline,
): SaveRelation {
  if (!cloud) return base && revision === base.revision && browser !== base.fingerprint ? 'browser-ahead' : 'unknown';
  const remote = manifestFingerprint(cloud.files);
  if (browser === remote) return 'same';
  if (!browser) return 'cloud-ahead';
  if (!base || revision < base.revision) return 'unknown';
  if (remote === base.fingerprint) return 'browser-ahead';
  if (!base.verifiedBase || revision === base.revision) return 'unknown';
  return browser === base.fingerprint ? 'cloud-ahead' : 'diverged';
}

export interface SaveVersionComparison {
  local: {files: readonly {name: string; bytes: number}[]; modifiedAt: number | null; statistics: SaveStatistics | null};
  cloud: CloudSaveSnapshot | null;
  relation: SaveRelation;
}

/** Parse statistics only when the comparison is displayed. */
export async function compareSaveVersions(
  product: string, files: File[], cloud: CloudSaveSnapshot | null, relation: SaveRelation, signal: AbortSignal,
): Promise<SaveVersionComparison> {
  let statistics: SaveStatistics | null = null;
  if (relation === 'same') statistics = cloud?.statistics ?? null;
  else if (files.length) {
    const timeout = new AbortController(), linked = linkAbortSignals(signal, timeout.signal);
    const timer = setTimeout(() => timeout.abort(new Error('Save preview timed out')), 2500);
    try {
      statistics = (await waitWithAbort(previewCloudSave(product, files, linked.signal), linked.signal)).statistics;
    } catch (error) {
      if (signal.aborted) throw error;
    } finally {
      clearTimeout(timer);
      linked.dispose();
    }
  }
  const dates = files.map(file => file.lastModified).filter(value => Number.isFinite(value) && value > 0 && value <= 8.64e15);
  return {
    local: {files: files.map(file => ({name: file.name, bytes: file.size})), modifiedAt: dates.length ? Math.max(...dates) : null, statistics},
    cloud,
    relation,
  };
}

export function saveComparisonCards(value: SaveVersionComparison, locale: string, t: Translate): DecisionComparison {
  const {local, cloud, relation} = value;
  const missing = t('cloudSave.unknownValue');
  const none = t('cloudSave.noVersion');
  const numbers = new Intl.NumberFormat(locale);
  const sizes = new Intl.NumberFormat(locale, {maximumFractionDigits: 1});
  const units = {
    hour: new Intl.NumberFormat(locale, {style: 'unit', unit: 'hour', unitDisplay: 'narrow'}),
    minute: new Intl.NumberFormat(locale, {style: 'unit', unit: 'minute', unitDisplay: 'narrow'}),
    second: new Intl.NumberFormat(locale, {style: 'unit', unit: 'second', unitDisplay: 'narrow'}),
  };
  const duration = (seconds: number) => {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor(seconds / 60) % 60;
    const remainder = Math.floor(seconds) % 60;
    if (hours) return units.hour.format(hours) + (minutes ? ' ' + units.minute.format(minutes) : '');
    if (minutes) return units.minute.format(minutes) + (remainder ? ' ' + units.second.format(remainder) : '');
    return seconds > 0 && seconds < 1 ? '<' + units.second.format(1) : units.second.format(remainder);
  };
  const dates = new Intl.DateTimeFormat(locale, {dateStyle: 'medium', timeStyle: 'short'});
  const uploadedAt = cloud ? Date.parse(cloud.uploadedAt) : null;
  const mark = (side: 'browser' | 'cloud') => relation === 'diverged'
    ? {tone: 'changed' as const, label: t('cloudSave.changed')}
    : local.files.length && cloud && relation === side + '-ahead'
      ? {tone: 'newer' as const, label: t('cloudSave.newer')}
      : undefined;
  function version(side: 'browser' | 'cloud', timestamp: number | null, statistics: SaveStatistics | null | undefined,
    files: readonly {name: string; bytes: number}[]): DecisionComparison['versions'][number] {
    const empty = !files.length;
    const scores = statistics?.progress.flatMap(row => row.bestScore === null ? [] : [row.bestScore]) ?? [];
    const cleared = [...new Set(statistics?.progress.filter(row => row.cleared === true).map(row => row.difficulty))];
    const clear = cleared.length ? cleared.map(name => name[0]!.toUpperCase() + name.slice(1)).join(' | ')
      : statistics?.progress.some(row => row.cleared !== null) ? t('profile.clear.uncleared') : missing;
    return {
      title: t(side === 'browser' ? 'cloudSave.browserVersion' : 'cloudSave.accountVersion'),
      mark: mark(side),
      empty: empty ? none : undefined,
      timestamp: empty ? undefined : {
        label: t(side === 'browser' ? 'cloudSave.localModified' : 'cloudSave.cloudUploaded'),
        value: timestamp ? dates.format(timestamp) : missing,
      },
      rows: empty ? [] : [
        {label: t('cloudSave.duration'), value: statistics?.playtimeSeconds == null ? missing : duration(statistics.playtimeSeconds)},
        {label: t('cloudSave.playCount'), value: statistics?.playCount == null ? missing : numbers.format(statistics.playCount)},
        {label: t('cloudSave.clearDifficulties'), value: clear},
        {label: t('cloudSave.bestScore'), value: scores.length ? numbers.format(Math.max(...scores)) : missing},
      ],
      files: files.map(file => ({name: file.name, size: file.bytes < 1024 ? numbers.format(file.bytes) + ' B' : sizes.format(file.bytes / 1024) + ' KB'})),
    };
  }
  return {
    filesLabel: t('cloudSave.files'),
    versions: [
      version('browser', local.modifiedAt, local.statistics, local.files),
      version('cloud', uploadedAt, cloud?.statistics, cloud?.files ?? []),
    ],
  };
}
