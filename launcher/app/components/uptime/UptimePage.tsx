import {useEffect, useState} from 'react';
import {useLocale} from '../../i18n';
import {linkAbortSignals} from '../../services/abort-signals';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import './uptime.css';

export type UptimeRange = '24h' | '7d' | '90d';
interface Point {at: string; samples: number; availability: number | null; latencyMs: number | null}
interface Service {id: string; label: string; group: string; status: string; checkedAt: string | null; latencyMs: number | null; availability: number | null; points: Point[]}
interface Summary {schema: 'eagler-uptime/1'; range: UptimeRange; updatedAt: string; services: Service[]}
const ranges = ['24h', '7d', '90d'] as const;
const groups = ['site', 'download', 'netplay', 'turn'] as const;
const percentage = (value: number | null) => value === null ? '\u2014' : `${(value * 100).toFixed(2)}%`;
const latency = (value: number | null) => value === null ? '\u2014' : `${Math.round(value)} ms`;
const record = (value: unknown): value is Record<string, unknown> => !!value && typeof value === 'object' && !Array.isArray(value);
const text = (value: unknown, max: number): value is string => typeof value === 'string' && value.length > 0 && value.length <= max;
const date = (value: unknown): value is string => typeof value === 'string' && value.length <= 64 && Number.isFinite(Date.parse(value));
const ratio = (value: unknown) => value === null || typeof value === 'number' && Number.isFinite(value) && value >= 0 && value <= 1;
const duration = (value: unknown) => value === null || typeof value === 'number' && Number.isFinite(value) && value >= 0;

/** Validate the bounded public response before it reaches formatting or JSX. */
export function parseUptimeSummary(value: unknown, range: UptimeRange): Summary {
  if (!record(value) || value.schema !== 'eagler-uptime/1' || value.range !== range || !date(value.updatedAt) ||
    !Array.isArray(value.services) || value.services.length > 24) throw new Error('Invalid uptime summary');
  const ids = new Set<string>();
  for (const service of value.services) {
    if (!record(service) || !text(service.id, 128) || ids.has(service.id) || !text(service.label, 160) ||
      !groups.some(group => group === service.group) || !['operational', 'outage', 'stale', 'unknown'].includes(String(service.status)) ||
      !(service.checkedAt === null || date(service.checkedAt)) || !ratio(service.availability) || !duration(service.latencyMs) ||
      !Array.isArray(service.points) || service.points.length > 90) throw new Error('Invalid uptime service');
    ids.add(service.id);
    const times = new Set<string>();
    for (const point of service.points) {
      if (!record(point) || !date(point.at) || times.has(point.at) || !Number.isSafeInteger(point.samples) || Number(point.samples) < 0 ||
        !ratio(point.availability) || !duration(point.latencyMs)) throw new Error('Invalid uptime history');
      times.add(point.at);
    }
  }
  return value as unknown as Summary;
}

export function UptimePage({endpoint}: {endpoint: string}) {
  const {locale, t} = useLocale();
  const [range, setRange] = useState<UptimeRange>('24h');
  const [data, setData] = useState<Summary | null>(null), [failed, setFailed] = useState(false);
  useEffect(() => {
    setFailed(false);
    let disposed = false, busy = false;
    let deadline: ReturnType<typeof setTimeout> | undefined;
    const controller = new AbortController();
    async function refresh() {
      if (busy || document.hidden) return;
      busy = true;
      const timeout = new AbortController(), linked = linkAbortSignals(controller.signal, timeout.signal);
      deadline = setTimeout(() => timeout.abort(), 10_000);
      try {
        const url = new URL(endpoint, document.baseURI); url.searchParams.set('range', range);
        const response = await fetch(url, {cache: 'no-store', signal: linked.signal});
        if (!response.ok) throw new Error('Uptime request failed');
        const value = parseUptimeSummary(await response.json(), range);
        if (!disposed) {setData(value); setFailed(false);}
      } catch {if (!disposed) setFailed(true);}
      finally {clearTimeout(deadline); deadline = undefined; linked.dispose(); busy = false;}
    }
    void refresh();
    const timer = setInterval(() => void refresh(), 30_000);
    const visible = () => {if (!document.hidden) void refresh();};
    document.addEventListener('visibilitychange', visible);
    return () => {disposed = true; controller.abort(); clearTimeout(deadline); clearInterval(timer); document.removeEventListener('visibilitychange', visible);};
  }, [range, endpoint]);
  const pending = !data || data.range !== range;
  const stateLabel = (status: string) => t(status === 'operational' ? 'uptime.normal' : status === 'outage' ? 'uptime.outage' : 'uptime.unknown');
  return <>
    <div className="uptime-heading"><h1>{t('nav.serviceStatus')}</h1>
      <ChoiceGroup type="single" className="ui-kit-choice-group uptime-ranges" aria-label={t('uptime.historyRange')} value={range}
        onValueChange={value => {if (ranges.some(range => range === value)) setRange(value as UptimeRange);}}>
        {ranges.map(value => <ChoiceItem className="min-h-11 flex-1" key={value} value={value}>{t(value === '24h' ? 'uptime.range24h' : value === '7d' ? 'uptime.range7d' : 'uptime.range90d')}</ChoiceItem>)}
      </ChoiceGroup>
    </div>
    <p role={failed ? 'alert' : 'status'} className="uptime-message">{failed ? t('uptime.failed') : pending ? t('uptime.loading') : ''}</p>
    <section className="uptime-list" data-empty-failure={failed && !data || undefined} aria-label={t('uptime.services')} aria-busy={pending && !failed}>
      {(!failed || data) && <div className="uptime-columns" aria-hidden="true"><span>{t('uptime.status')}</span><span>{t('uptime.service')}</span><span>{t('uptime.history', {range: data?.range ?? range})}</span><span>{t('uptime.response')}</span><span>{t('uptime.availability')}</span></div>}
      {groups.map(group => {
        const services = data?.services.filter(service => service.group === group) ?? [];
        return services.length > 0 && <section className="uptime-group" key={group} aria-label={t(`uptime.${group}`)}><h2>{t(`uptime.${group}`)}</h2>
          {services.map(service => {
            const age = service.checkedAt ? Date.now() - Date.parse(service.checkedAt) : Infinity;
            const stale = failed || age > 90_000 || age < -30_000;
            const status = stale ? 'unknown' : service.status;
            return <div className="uptime-row" key={service.id} data-status={status}>
              <span className="uptime-status">{stateLabel(status)}</span><span className="uptime-service">{service.label}</span>
              <div className="uptime-history" role="img" aria-label={`${service.label} | ${data?.range} | ${percentage(service.availability)}`}>
                {service.points.map(point => <i key={point.at} data-state={point.availability === null ? 'unknown' : point.availability === 1 ? 'up' : point.availability === 0 ? 'down' : 'partial'}
                  title={`${new Date(point.at).toLocaleString(locale)} | ${percentage(point.availability)} | ${latency(point.latencyMs)}`}/>)}
              </div>
              <span className="uptime-latency">{latency(stale ? null : service.latencyMs)}</span><span className="uptime-availability" title={t('uptime.availabilityHint')}>{percentage(service.availability)}</span>
            </div>;
          })}
        </section>;
      })}
      {data && !data.services.length && <p className="uptime-empty">{t('uptime.empty')}</p>}
    </section>
  </>;
}
